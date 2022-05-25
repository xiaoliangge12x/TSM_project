/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Signal Preprocess for TSM implementation
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */


#include "tsm_signalhandling.h"
#include "tsm_parameter.h"

// 考虑为局部变量
static uint16  K_GasPedalAppliedThresholdTime_Cnt = 10U;
static uint16  K_LngOverrideTakeOverTime_Cnt = 250U;   // 纵向超越至判断接管的持续时间(10s)
static uint16  K_LngOvrdNeedDetectTouchZoneTime_Cnt = 25U;
static uint16  K_BrakPedalAppliedThresholdTime_Cnt = 10U;    // 制动判断的持续时间(200ms周期)
static uint16  K_BrakeTOR_TimeThreshold_Cnt = 25U;
static uint16  K_OverrideHandTorqCheckTime_Cnt = 25U;
static uint16  K_HandsTouchDetectedTime_Cnt = 25U;

static float32 K_GasPedalAppliedThresholdTime = 0.2;
static float32 K_BrakPedalAppliedThresholdTime = 0.2;
static float32 K_OverrideHandTorqCheckTime = 1.0;
static float32 K_LngOverrideTakeOverTime = 10.0;

static float32 K_GasPedalPosThresholdValue = 20.0;
static float32 K_OverrideHandTorqThreshold_LessTwoZone = 1.8;    // 少于2区的手力矩是否超越的手扶阈值
static float32 K_OverrideHandTorqThreshold_TwoZone     = 1.5;    // 2区的是否超越的手力矩阈值
static float32 K_OverrideHandTorqThreshold_ThreeZone   = 1.0;    // 3区的是否超越的手力矩阈值

static float32 K_StrngWhlTorqueWeight = 0.6;

struct tsm_spd_handtorq {
    float32 veh_spd;
    float32 hand_torque;
};

static void
tsm_process_drvr_attention_st(const tsm_veh_sig* p_veh_sig,
                              struct tsm_intermediate_sig* p_int_sig) {
    enum tsm_drvr_det_st {
        DET_UNAVAILABLE, 
        DET_NOT_DETECTED,
        DET_EYE_DETECTED,
        DET_HEAD_DETECTED,
    };

    enum tsm_drvr_distraction_st {
        DISTRACTION_UNAVAILABLE = 0x0U,
        DISTRACTION_UNKNOWN = 0x15U,
        DISTRACTION_DISTRACTIVE = 0x2AU,
        DISTRACTION_NON_DISTRACTIVE = 0x3FU,
    };

    enum tsm_drvr_fatigue_st {
        FATIGUE_UNAVAILABLE = 0x0U,   
        FATIGUE_UNKNOWN = 0x249U, 
        FATIGUE_ALERT = 0x492U,  // 警惕
        FATIGUE_FATIGUE = 0x6DBU,  // 疲惫
        FATIGUE_H_FATIGUE = 0x924U,  // 非常疲惫
        FATIGUE_SU = 0xB6DU,  // 怀疑反应迟钝
        FATIGUE_CU = 0xDB6U,  // 确定反应迟钝
        FATIGUE_NOT_USED = 0xFFFU,  // 未使用
    };

    uint16 fatigue_st = p_veh_sig->DMS_L3DriverFatigueSt;
    switch (fatigue_st) {
        case FATIGUE_ALERT: {
            uint8 det_st = p_veh_sig->DMS_DrvrDetSts;
            uint8 distraction_st = p_veh_sig->DMS_L3DriverDistractionSt;
            if (((det_st == DET_EYE_DETECTED) || 
                (det_st == DET_HEAD_DETECTED)) &&
                (distraction_st == DISTRACTION_NON_DISTRACTIVE)) {
                p_int_sig->drvr_att_st = DRVR_AWAKE_NOT_DISTRACTED;
            } else if ((det_st == DET_NOT_DETECTED) ||
                       (distraction_st == DISTRACTION_DISTRACTIVE)) {
                p_int_sig->drvr_att_st = DRVR_AWAKE_DISTRACTED;
            } else {
                // do nothing;
            }
            break;
        }
        case FATIGUE_FATIGUE:
        case FATIGUE_H_FATIGUE:
        case FATIGUE_UNKNOWN:
        case FATIGUE_SU:
        case FATIGUE_CU: {
            p_int_sig->drvr_att_st = DRVR_FATIGUE;
            break;
        }

        default: {
            // do nothing;
        }
    }
}

static boolean 
is_flag_set_with_timecnt(const uint16 timecnt_threshold, uint16* timecnt, 
                         const boolean last_flag) {
    boolean flag = false;
    if (last_flag) {
        flag = true;
    } else {
        if (*timecnt > timecnt_threshold) {
            *timecnt = 0;
            flag = true;
        } else {
            ++(*timecnt);
        }
    }

    return flag;
}

#ifdef CONSUME_TIME

static boolean
is_flag_set_with_time(const float32 time_threshold, sint64* time,
                      uint8* time_flag, const boolean last_flag) {
    boolean flag = false;
    if (*time_flag == 0) {
        tsm_start_timing(time, time_flag);
    }

    if (last_flag) {
        flag = true;
    } else {
        float32 actual_delta_time = tsm_get_delta_time(*time, *time_flag);
        if (actual_delta_time > time_threshold) {
            tsm_stop_timing(time_flag);
            flag = true;
        }
    }
    return flag;
}

#endif

static boolean
tsm_check_gas_pedal_applied(const tsm_veh_sig* p_veh_sig) {
    static boolean is_gas_pedal_applied = false;
#ifndef CONSUME_TIME
    static uint16 timecnt = 0;
#else
    static sint64 time = 0;
    static uint8 time_flag = 0;
#endif
    float32 gas_pedal_act_pst = p_veh_sig->EMS_GasPedalActPstforMRR;
    if (p_veh_sig->EMS_GasPedalActPstforMRRVD &&
        (gas_pedal_act_pst > K_GasPedalPosThresholdValue)) {
#ifndef CONSUME_TIME
        is_gas_pedal_applied = 
            is_flag_set_with_timecnt(K_GasPedalAppliedThresholdTime_Cnt,
                                     &timecnt, is_gas_pedal_applied);
#else
        is_gas_pedal_applied =
            is_flag_set_with_time(K_GasPedalAppliedThresholdTime, &time,
                                  &time_flag, is_gas_pedal_applied);
#endif
    } else {
        is_gas_pedal_applied = false;
#ifndef CONSUME_TIME
        timecnt = 0;
#else
        tsm_stop_timing(&time_flag);
#endif
    }
    return is_gas_pedal_applied;
}

static void
tsm_process_lng_override(const tsm_veh_sig* p_veh_sig,
                         struct tsm_intermediate_sig* p_int_sig, 
                         const enum tsm_ifc_mrm_func_st mrm_st) {
    enum acc_drvr_orvd
    {
        NO_OVERRIDE,
        DRIVER_OVERRIDE,
    };

    static enum tsm_lng_ovrd_duration_type lng_ovrd_du_type = NO_LNG_OVERRIDE;
    static uint16 timecnt = 0;

    boolean is_gas_pedal_applied = tsm_check_gas_pedal_applied(p_veh_sig);
    if ((p_veh_sig->VCU_AccDriverOrvd == DRIVER_OVERRIDE) ||
        is_gas_pedal_applied) {
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_LNG_OVERRIDE_ST);
                                 
        if (tsm_is_mrm_active(mrm_st)) {
            if (lng_ovrd_du_type != LONG_TIME_LNG_OVERRIDE) {
                if (timecnt > K_LngOverrideTakeOverTime_Cnt) {
                    lng_ovrd_du_type = LONG_TIME_LNG_OVERRIDE;
                } else if (timecnt > K_LngOvrdNeedDetectTouchZoneTime_Cnt) {
                    lng_ovrd_du_type = SHORT_TIME_LNG_OVERRIDE;
                } else {
                    lng_ovrd_du_type = INSTANT_LNG_OVERRIDE;
                }
                ++timecnt;
            }
        } else {
            lng_ovrd_du_type = NO_LNG_OVERRIDE;
            timecnt = 0;
        }
    } else {
        lng_ovrd_du_type = NO_LNG_OVERRIDE;
        timecnt = 0;
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                   BITNO_LNG_OVERRIDE_ST);
    }

    p_int_sig->lng_ovrd_du_type = lng_ovrd_du_type;
    // LOG(COLOR_GREEN, "<tsm_process_lng_override> lng_ovrd_du_type: %d", 
    //     lng_ovrd_du_type);
}

static boolean
tsm_check_brake_applied(const tsm_veh_sig* p_veh_sig) {
    enum tsm_brk_pedal_applied {
        BRAKE_PEDAL_NOT_APPLIED,
        BRAKE_PEDAL_APPLIED
    };

    enum tsm_brk_pedal_applied_st {
        BRK_APPLIED_ST_NOT_INITIALIZED,
        BRK_APPLIED_ST_NORMAL,
        BRK_APPLIED_ST_FAULTY,
        BRK_APPLIED_ST_RESERVED
    };

    static boolean is_brake_applied = false;
#ifndef CONSUME_TIME
    static uint16 timecnt = 0;
#else
    static sint64 time = 0;
    static uint8 time_flag = 0;
#endif
    float32 brk_pedal_applied_st = p_veh_sig->EBB_BrkPedalAppliedSt;
    uint8 brk_pedal_applied = p_veh_sig->EBB_BrkPedalApplied;
    if ((brk_pedal_applied_st == BRK_APPLIED_ST_NORMAL) &&
        (brk_pedal_applied == BRAKE_PEDAL_APPLIED)) {
#ifndef CONSUME_TIME
        is_brake_applied = 
            is_flag_set_with_timecnt(K_BrakPedalAppliedThresholdTime_Cnt,
                                     &timecnt, is_brake_applied);
#else
        is_brake_applied =
            is_flag_set_with_time(K_BrakPedalAppliedThresholdTime, &time,
                                  &time_flag, is_brake_applied);
#endif
    } else {
        is_brake_applied = false;
#ifndef CONSUME_TIME
        timecnt = 0;
#else
        tsm_stop_timing(&time_flag);
#endif
    }
    return is_brake_applied;
}

static void
tsm_process_brake_behavior(const tsm_veh_sig* p_veh_sig,
                           struct tsm_intermediate_sig* p_int_sig,
                           const enum tsm_ifc_mrm_func_st mrm_st) {
    static uint16 timecnt = 0;
    static enum tsm_brk_duration_type brk_du_type = NO_BRAKE_INTERVENTION;

    boolean is_brake_applied = tsm_check_brake_applied(p_veh_sig);
    if (is_brake_applied) {
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                 BITNO_SET_BRAKE);
        if (tsm_is_mrm_active(mrm_st)) {
            if (brk_du_type != LONG_INTERVENTION) {
                if (timecnt > K_BrakeTOR_TimeThreshold_Cnt) {
                    brk_du_type = LONG_INTERVENTION;
                } else {
                    brk_du_type = SHORT_INTERVENTION;
                }
                ++timecnt;
            }
        } else {
            timecnt = 0;
            brk_du_type = NO_BRAKE_INTERVENTION;
        }
    } else {
        timecnt = 0;
        brk_du_type = NO_BRAKE_INTERVENTION;
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                   BITNO_SET_BRAKE);
    }

    p_int_sig->brk_du_type = brk_du_type;
// #ifdef _NEED_LOG
//     float brake_time = (float)(20 * timecnt) / 1000;
//     LOG(COLOR_YELLOW, "<tsm_process_brake_behavior> brake tme: %fs, "
//         "brk_du_type: %d, is_brake_applied: %d", brake_time, brk_du_type,
//         is_brake_applied);
// #endif
}

// static float32
// tsm_calculate_orvd_torque(const tsm_veh_sig* p_veh_sig) {
//     float32 ovrd_torque_threshold = K_OverrideHandTorqThreshold_LessTwoZone;

//     uint8 hod_fault_st = p_veh_sig->HOD_FaultStatus;
//     uint8 hod_cali_st = p_veh_sig->HOD_CalibrationSt;
//     uint8 zone_1 = p_veh_sig->HOD_TouchZone1;
//     uint8 zone_2 = p_veh_sig->HOD_TouchZone2;
//     uint8 zone_3 = p_veh_sig->HOD_TouchZone3;
    
//     if ((hod_fault_st == 0) && (hod_cali_st == 1)) {
//         if (zone_1 && zone_2 && zone_3) {
//             ovrd_torque_threshold = K_OverrideHandTorqThreshold_ThreeZone;
//         } else if ((zone_1 && zone_2) || (zone_1 && zone_3) || 
//                    (zone_2 && zone_3)) {
//             ovrd_torque_threshold = K_OverrideHandTorqThreshold_TwoZone;
//         }
//     }

//     return ovrd_torque_threshold;
// }

static float32
tsm_get_hand_torq_thold(const float32 veh_spd,
                        const struct tsm_spd_handtorq* torq_table,
                        const size_t table_size) {
    for (size_t i = (table_size - 1); i >= 0; --i) {
        if (veh_spd >= torq_table[i].veh_spd) {
            return torq_table[i].hand_torque;
        }
    }
    return 0.0;
}

static float32
tsm_smooth_strng_whl_torque(const float32 strng_whl_torque) {
    static const float32 rate_one = 1.0;
    static boolean is_initial_set = true;
    static float32 smoothed_torque = 0.0;

    if (is_initial_set) {
        smoothed_torque = strng_whl_torque;
        is_initial_set = false;
    } else {
        smoothed_torque = K_StrngWhlTorqueWeight * smoothed_torque +
            (rate_one - K_StrngWhlTorqueWeight) * strng_whl_torque;
    }
    // LOG(COLOR_GREEN, "<tsm_smooth_strng_whl_torque> origin strng_whl_torque:"
    //     " %f, smoothed strng_whl_torque: %f", strng_whl_torque, 
    //     smoothed_torque);
    return smoothed_torque;
}

static void
tsm_process_lat_override(const tsm_veh_sig* p_veh_sig,
                         struct tsm_intermediate_sig* p_int_sig,
                         const enum tsm_ifc_mrm_func_st mrm_st) {
    static boolean is_lat_override = false;

    static const struct tsm_spd_handtorq ovrd_torq_thold[] = {
        {
            .veh_spd = 0.0,
            .hand_torque = 1.5
        },
        {
            .veh_spd = 30.0,
            .hand_torque = 1.3
        },
        {
            .veh_spd = 60.0,
            .hand_torque = 1.2
        },
        {
            .veh_spd = 90.0,
            .hand_torque = 1.0
        }
    };

#ifndef CONSUME_TIME
    static uint16 timecnt = 0;
#else
    static sint64 time = 0;
    static uint8 time_flag = 0;
#endif
    float32 strng_whl_torque = 
        tsm_smooth_strng_whl_torque(p_veh_sig->EPS_StrngWhlTorq);
    float32 ovrd_torq_size = ARRAY_LEN(ovrd_torq_thold);
    float32 veh_spd = p_veh_sig->BCS_VehSpd; 
    float32 orvd_torque_threshold = 
        tsm_get_hand_torq_thold(veh_spd, ovrd_torq_thold, ovrd_torq_size);
    if (p_veh_sig->EPS_StrngWhlTorqVD &&
        (fabs(strng_whl_torque) > orvd_torque_threshold)) {
        if (tsm_is_mrm_active(mrm_st)) {
#ifndef CONSUME_TIME
            is_lat_override = 
                is_flag_set_with_timecnt(K_OverrideHandTorqCheckTime_Cnt,
                                         &timecnt, is_lat_override);
#else 
        is_lat_override = 
            is_flag_set_with_time(K_OverrideHandTorqCheckTime, &time
                                  &time_flag, is_lat_override);
#endif
        } else {
            is_lat_override = false;
#ifndef CONSUME_TIME
            timecnt = 0;
#else
            tsm_stop_timing(&time_flag);
#endif
        }
    } else {
        is_lat_override = false;
#ifndef CONSUME_TIME
        timecnt = 0;
#else
        tsm_stop_timing(&time_flag);
#endif
    }

    LOG(COLOR_GREEN, "<tsm_process_lat_override> orvd_torque_threshold: %f, "
        "strng_whl_torque: %f, is_lat_override: %d", orvd_torque_threshold,
        strng_whl_torque, is_lat_override);

    (is_lat_override) ?
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_DRVR_HANDTORQUE_OVERRIDE_ST) :
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                   BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
}

static void
tsm_process_hands_detection(const tsm_veh_sig* p_veh_sig,
                            struct tsm_intermediate_sig* p_int_sig,
                            const enum tsm_ifc_mrm_func_st mrm_st) {
    uint8 hod_fault_st = p_veh_sig->HOD_FaultStatus;
    uint8 hod_cali_st = p_veh_sig->HOD_CalibrationSt;
    uint8 zone_1 = p_veh_sig->HOD_TouchZone1;
    uint8 zone_2 = p_veh_sig->HOD_TouchZone2;
    uint8 zone_3 = p_veh_sig->HOD_TouchZone3;

    static uint16 timecnt = 0;
    static const struct tsm_spd_handtorq handson_torq_thold[] = {
        {
            .veh_spd = 0.0,
            .hand_torque = 0.8 
        },
        {
            .veh_spd = 30.0,
            .hand_torque = 0.6
        },
        {
            .veh_spd = 60.0,
            .hand_torque = 0.4
        },
        {
            .veh_spd = 90.0,
            .hand_torque = 0.3
        }
    };

    static boolean is_hands_touch_detected = false;
    boolean is_hod_avl = ((hod_fault_st == 0) && (hod_cali_st == 1));
    // LOG(COLOR_YELLOW, "<tsm_process_hands_detection> hod is available: %d.",
    //     is_hod_avl);
    if (tsm_is_mrm_active(mrm_st)) {
        if ((hod_fault_st == 0) && (hod_cali_st == 1)) {
            if (!zone_1 && !zone_2 && !zone_3) {
                is_hands_touch_detected = false;
            } else {
                is_hands_touch_detected = true;
            }
        } else {
            float32 strng_whl_torque =  
                tsm_smooth_strng_whl_torque(p_veh_sig->EPS_StrngWhlTorq);
            float32 handson_torq_size = ARRAY_LEN(handson_torq_thold);
            float32 veh_spd = p_veh_sig->BCS_VehSpd;
            float32 handson_torque_threshold = 
                tsm_get_hand_torq_thold(veh_spd, handson_torq_thold, 
                                        handson_torq_size);
            if (fabs(strng_whl_torque) > handson_torque_threshold) {
                is_hands_touch_detected = 
                    is_flag_set_with_timecnt(K_HandsTouchDetectedTime_Cnt,
                                            &timecnt, is_hands_touch_detected);
            } else {
                is_hands_touch_detected = false;
                timecnt = 0;
            }

            float32 hands_touch_time = (float32)(20 * timecnt) / 1000;
            // LOG(COLOR_YELLOW, "<tsm_process_hands_detection> "
            //     "handson_torque_threshold: %f, time elapse %fs.", 
            //     handson_torque_threshold, hands_touch_time);
        }
    } else {
        is_hands_touch_detected = false;
        timecnt = 0;
    }

    (is_hands_touch_detected) ?
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_HANDS_TOUCH_DETECTED) :
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                   BITNO_HANDS_TOUCH_DETECTED);
    
    // LOG(COLOR_YELLOW, "<tsm_process_hands_detection> "
    //     "is_hands_touch_detected: %d.", is_hands_touch_detected);
}

static void
tsm_process_driver_behavior(struct tsm_intermediate_sig* p_int_sig,
                            const struct tsm_entry* p_entry,
                            const enum tsm_ifc_mrm_func_st mrm_st) {
    tsm_veh_sig veh_sig = p_entry->in_can_gate->Vehicle_Signal_To_Tsm;
    tsm_process_drvr_attention_st(&veh_sig, p_int_sig);

    tsm_process_lng_override(&veh_sig, p_int_sig, mrm_st);

    tsm_process_brake_behavior(&veh_sig, p_int_sig, mrm_st);

    tsm_process_lat_override(&veh_sig, p_int_sig, mrm_st);

    tsm_process_hands_detection(&veh_sig, p_int_sig, mrm_st);
}

void 
tsm_preprocess_input(struct tsm_intermediate_sig* p_int_sig,
                     const struct tsm_entry* p_entry,
                     const enum tsm_ifc_mrm_func_st mrm_st) {
    tsm_process_driver_behavior(p_int_sig, p_entry, mrm_st);
}
