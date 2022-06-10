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

#define TK_TORQ_THOLD_NUM 4

uint16  K_GasPedalAppliedThresholdTime_Cnt = 10U;
uint16  K_LngOverrideTakeOverTime_Cnt = 500U;   // 纵向超越至判断接管的持续时间(10s)
uint16  K_BrakPedalAppliedThresholdTime_Cnt = 10U;    // 制动判断的持续时间(200ms周期)
uint16  K_BrakeTOR_TimeThreshold_Cnt = 150U;
uint16  K_OverrideHandTorqCheckTime_Cnt = 50U;
uint16  K_RainlevelTime_Cnt = 25U;
uint16  K_WiperFastTime_Cnt = 500U;
uint16  K_ODCHandTorqCheckTime_Cnt = 5U;
uint16  K_TakeOverAvailHandTorqCheckTime_Cnt = 5U;
uint16  K_Standstill_waitTime_Cnt = 15000U;    // 停车计时  300s
uint16  K_HDCCtrlFalseTime_Cnt = 20U;
uint16  K_VDCActiveFalseTime_Cnt = 10U;
uint16  K_ABSActiveFalseTime_Cnt = 15U;
uint16  K_TCSActiveFalseTime_Cnt = 15U;
uint16  K_EBDActiveFalseTime_Cnt = 10U;

uint16  K_YAW_TimeThreshold_Cnt = 25U;
uint16  K_LatAcc_TimeThreshold_Cnt = 25U;
uint16  K_LngAcc_TimeThreshold_Cnt = 25U;
uint16  K_StrWhlAngSpd_TimeThreshold_Cnt = 25U;

float32 K_GasPedalAppliedThresholdTime = 0.2;
float32 K_BrakPedalAppliedThresholdTime = 0.2;
float32 K_OverrideHandTorqCheckTime = 1.0;
float32 K_LngOverrideTakeOverTime = 10.0;

float32 K_GasPedalPosThresholdValue = 20.0;
float32 K_OverrideHandTorqThreshold_LessTwoZone = 1.8;    // 少于2区的手力矩是否超越的手扶阈值
float32 K_OverrideHandTorqThreshold_TwoZone     = 1.5;    // 2区的是否超越的手力矩阈值
float32 K_OverrideHandTorqThreshold_ThreeZone   = 1.0;    // 3区的是否超越的手力矩阈值
float32 K_VehSpdThresholdInOddCheck = 120.0;
float32 K_lightLuxThresholdInOddCheck = 30.0;

float32 K_YAW_THRESHOLD_ActiveSt = 15.0;
float32 K_YAW_THRESHOLD_NotActiveSt = 20.0;
float32 K_YAW_false_THRESHOLD_ActiveSt = 20.0;
float32 K_YAW_false_THRESHOLD_NotActiveSt = 25.0;

float32 K_LatAcc_THRESHOLD_ActiveSt = 1.5;
float32 K_LatAcc_THRESHOLD_NotActiveSt = 2.0;
float32 K_LatAcc_false_THRESHOLD_ActiveSt = 2.0;
float32 K_LatAcc_false_THRESHOLD_NotActiveSt = 2.5;

float32 K_LngAcc_THRESHOLD_ActiveSt = 6.0;
float32 K_LngAcc_THRESHOLD_NotActiveSt = 7.0;
float32 K_LngAcc_false_THRESHOLD_ActiveSt = 6.5;
float32 K_LngAcc_false_THRESHOLD_NotActiveSt = 7.5;

float32 K_StrWhlAngSpd_THRESHOLD_ActiveSt = 360.0;
float32 K_StrWhlAngSpd_THRESHOLD_NotActiveSt = 400.0;
float32 K_StrWhlAngSpd_false_THRESHOLD_ActiveSt = 380.0;
float32 K_StrWhlAngSpd_false_THRESHOLD_NotActiveSt = 420.0;

float32 K_StrngWhlTorqueWeight = 0.6;

sint32   K_GeoEndDist_NotActive = 100;
sint32   K_GeoEndDist_Active = 150;

enum tsm_nda_active_st {
    NDA_ACTIVE = 0,
    NDA_NOT_ACTIVE,
};

enum tsm_touch_zone_num {
    LESS_TWO_ZONE = 0,
    TWO_ZONE,
    THREE_ZONE,
};

enum tsm_motion_params_type {
    YAW_RATE = 0,
    LAT_ACC,
    LNG_ACC,
    STEERING_WHL_ANGLE_SPD,
};

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
    boolean flag = last_flag;
    if (!flag) {
        if (*timecnt > timecnt_threshold) {
            *timecnt = 0;
            flag = true;
        } else {
            ++(*timecnt);
        }
    }
    return flag;
}

static boolean 
is_flag_reset_with_timecnt(const uint16 timecnt_threshold, uint16* timecnt, 
                           const boolean last_flag) {
    boolean flag = last_flag;
    if (flag) {
        if (*timecnt > timecnt_threshold) {
            *timecnt = 0;
            flag = false;
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
                         const enum tsm_mcu_mrm_func_st mrm_st) {
    enum acc_drvr_orvd
    {
        NO_OVERRIDE,
        DRIVER_OVERRIDE,
    };

    static boolean is_lng_override_long_time = false;
#ifndef CONSUME_TIME
    static uint16 timecnt = 0;
#else
    static sint64 time = 0;
    static uint8 time_flag = 0;
#endif
    boolean is_gas_pedal_applied = tsm_check_gas_pedal_applied(p_veh_sig);
    if ((p_veh_sig->VCU_AccDriverOrvd == DRIVER_OVERRIDE) ||
        is_gas_pedal_applied) {
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_LNG_OVERRIDE_ST);
        if (tsm_is_mrm_active(mrm_st)) {
#ifndef CONSUME_TIME
        
            is_lng_override_long_time = 
                is_flag_set_with_timecnt(K_LngOverrideTakeOverTime_Cnt,
                                         &timecnt, is_lng_override_long_time);
#else 
        is_lng_override_long_time = 
            is_flag_set_with_time(K_LngOverrideTakeOverTime, &time
                                  &time_flag, is_lng_override_long_time);
#endif
        } else {
            is_lng_override_long_time = false;
#ifndef CONSUME_TIME
            timecnt = 0;
#else
            tsm_stop_timing(&time_flag);
#endif
        }
    } else {
        is_lng_override_long_time = false;
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                   BITNO_LNG_OVERRIDE_ST);
#ifndef CONSUME_TIME
        timecnt = 0;
#else
        tsm_stop_timing(&time_flag);
#endif
    }

    (is_lng_override_long_time) ?
    tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                             BITNO_LONG_TIME_LNG_OVERRIDE) :
    tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                               BITNO_LONG_TIME_LNG_OVERRIDE);
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
                           const enum tsm_mcu_mrm_func_st mrm_st) {
    static uint16 timecnt = 0;

    boolean is_brake_applied = tsm_check_brake_applied(p_veh_sig);
    if (is_brake_applied) {
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                 BITNO_SET_BRAKE);
        if (tsm_is_mrm_active(mrm_st)) {
            if (p_int_sig->brk_du_type == LONG_INTERVENTION) {
                p_int_sig->brk_du_type = LONG_INTERVENTION;
            } else {
                if (timecnt >= K_BrakeTOR_TimeThreshold_Cnt) {
                    timecnt = 0;
                    p_int_sig->brk_du_type = LONG_INTERVENTION;
                } else {
                    if (timecnt > 0) {
                        p_int_sig->brk_du_type = SHORT_INTERVENTION;
                    }
                    ++timecnt;
                }
            }
        } else {
            timecnt = 0;
            p_int_sig->brk_du_type = NO_BRAKE_INTERVENTION;
        }
    } else {
        timecnt = 0;
        p_int_sig->brk_du_type = NO_BRAKE_INTERVENTION;
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                   BITNO_SET_BRAKE);
    }
}

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
    const float32 rate_one = 1.0;
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

static boolean
tsm_process_hand_torque(const tsm_veh_sig* p_veh_sig, 
                        const float32 hand_torq_thold, uint16* p_time_cnt,
                        const boolean is_last_st, const uint16 check_time_cnt, 
                        boolean judge_exceed) {
    float32 strng_whl_torque = 
        tsm_smooth_strng_whl_torque(p_veh_sig->EPS_StrngWhlTorq);
    boolean is_hand_torque_set = is_last_st;
    boolean is_hand_torq_satisfy = 
        (judge_exceed) ? (fabs(strng_whl_torque) > hand_torq_thold) :
                         (fabs(strng_whl_torque) <= hand_torq_thold);
    if (p_veh_sig->EPS_StrngWhlTorqVD && is_hand_torq_satisfy) {
        is_hand_torque_set = 
            is_flag_set_with_timecnt(check_time_cnt, p_time_cnt, 
                                     is_hand_torque_set);
    } else {
        *p_time_cnt = 0;
        is_hand_torque_set = false;
    }
    return is_hand_torque_set;
}

static boolean
tsm_process_lat_torq_odc(const tsm_veh_sig* p_veh_sig) {
    static boolean is_lat_torque_odc = true;
    static uint16 timecnt = 0;

    static const struct tsm_spd_handtorq odd_torq_thold[] = {
        {
            .veh_spd = 0.0,
            .hand_torque = 1.8,
        },
        {
            .veh_spd = 30.0,
            .hand_torque = 1.6,
        },
        {
            .veh_spd = 60.0,
            .hand_torque = 1.4,
        },
        {
            .veh_spd = 90.0,
            .hand_torque = 1.2,
        }
    };

    size_t odc_torq_size = ARRAY_LEN(odd_torq_thold);
    float32 veh_spd = p_veh_sig->BCS_VehSpd;
    float32 odc_torque_threshold = 
        tsm_get_hand_torq_thold(veh_spd, odd_torq_thold, odc_torq_size);

    is_lat_torque_odc = 
        tsm_process_hand_torque(p_veh_sig, odc_torque_threshold, &timecnt,
                                is_lat_torque_odc, K_ODCHandTorqCheckTime_Cnt,
                                false);
#ifdef _NEED_LOG
    if (!is_lat_torque_odc) {
        LOG(COLOR_GREEN, "<tsm_process_lat_torq_odc> odc_torque_threshold: %f, "
            "is_lat_torque_odc: %d", odc_torque_threshold, is_lat_torque_odc);
    }
#endif
    return is_lat_torque_odc;
}

static void
tsm_process_lat_override(const tsm_veh_sig* p_veh_sig,
                         struct tsm_intermediate_sig* p_int_sig,
                         const enum tsm_mcu_mrm_func_st mrm_st) {
    static boolean is_lat_override = false;
    static uint16 timecnt = 0;

    static const struct tsm_spd_handtorq ovrd_torq_thold[] = {
        {
            .veh_spd = 0.0,
            .hand_torque = 1.8,
        },
        {
            .veh_spd = 30.0,
            .hand_torque = 1.6,
        },
        {
            .veh_spd = 60.0,
            .hand_torque = 1.4,
        },
        {
            .veh_spd = 90.0,
            .hand_torque = 1.2,
        }
    };

    size_t ovrd_torq_size = ARRAY_LEN(ovrd_torq_thold);
    float32 veh_spd = p_veh_sig->BCS_VehSpd;
    float32 ovrd_torque_threshold = 
        tsm_get_hand_torq_thold(veh_spd, ovrd_torq_thold, ovrd_torq_size);
    if (tsm_is_mrm_active(mrm_st)) {
        is_lat_override = 
            tsm_process_hand_torque(p_veh_sig, ovrd_torque_threshold, &timecnt,
                                    is_lat_override, 
                                    K_OverrideHandTorqCheckTime_Cnt, true);
    } else {
        is_lat_override = false;
        timecnt = 0;
    }
#ifdef _NEED_LOG
    if (is_lat_override) {
        LOG(COLOR_GREEN, "<tsm_process_lat_override> ovrd_torque_threshold: "
            "%f, is_lat_override: %d", ovrd_torque_threshold, is_lat_override);
    }
#endif

    (is_lat_override) ?
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_DRVR_HANDTORQUE_OVERRIDE_ST) :
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                   BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
}

static enum tsm_touch_zone_num
tsm_get_touch_type(const tsm_veh_sig* p_veh_sig) {
    uint8 hod_fault_st = p_veh_sig->HOD_FaultStatus;
    uint8 hod_cali_st = p_veh_sig->HOD_CalibrationSt;
    uint8 zone_1 = p_veh_sig->HOD_TouchZone1;
    uint8 zone_2 = p_veh_sig->HOD_TouchZone2;
    uint8 zone_3 = p_veh_sig->HOD_TouchZone3;

    if ((hod_fault_st == 0) && (hod_cali_st == 1)) {
        if (zone_1 && zone_2 && zone_3) {
            return THREE_ZONE;
        } else if ((zone_1 && zone_2) || (zone_1 && zone_3) || 
                   (zone_2 && zone_3)) {
            return TWO_ZONE;
        }
    }
    return LESS_TWO_ZONE;
}

static void
tsm_process_hands_takeover(const tsm_veh_sig* p_veh_sig,
                           struct tsm_intermediate_sig* p_int_sig,
                           const enum tsm_mcu_mrm_func_st mrm_st) {
    static boolean is_hands_can_tk = false;
    static uint16 timecnt = 0;

    static const struct tsm_spd_handtorq tk_torq_thold[][TK_TORQ_THOLD_NUM] = {
        [LESS_TWO_ZONE] = {
            {
                .veh_spd = 0.0,
                .hand_torque = 1.5,
            },
            {
                .veh_spd = 30.0,
                .hand_torque = 1.3,
            },
            {
                .veh_spd = 60.0,
                .hand_torque = 1.1,
            },
            {
                .veh_spd = 90.0,
                .hand_torque = 0.9,
            }
        },
        [TWO_ZONE] = {
            {
                .veh_spd = 0.0,
                .hand_torque = 1.0,
            },
            {
                .veh_spd = 30.0,
                .hand_torque = 0.8,
            },
            {
                .veh_spd = 60.0,
                .hand_torque = 0.6,
            },
            {
                .veh_spd = 90.0,
                .hand_torque = 0.4,
            }
        },
        [THREE_ZONE] = {
            {
                .veh_spd = 0.0,
                .hand_torque = 0.6,
            },
            {
                .veh_spd = 30.0,
                .hand_torque = 0.5,
            },
            {
                .veh_spd = 60.0,
                .hand_torque = 0.4,
            },
            {
                .veh_spd = 90.0,
                .hand_torque = 0.3,
            }
        },
    };

    size_t tk_torq_size = TK_TORQ_THOLD_NUM;
    float32 veh_spd = p_veh_sig->BCS_VehSpd;
    enum tsm_touch_zone_num touch_type = tsm_get_touch_type(p_veh_sig);
    float32 tk_torque_threshold = 
        tsm_get_hand_torq_thold(veh_spd, tk_torq_thold[touch_type], 
                                tk_torq_size);
    if (tsm_is_mrm_active(mrm_st)) {
        is_hands_can_tk = 
            tsm_process_hand_torque(p_veh_sig, tk_torque_threshold, &timecnt,
                                    is_hands_can_tk, 
                                    K_TakeOverAvailHandTorqCheckTime_Cnt, true);
    } else {
        is_hands_can_tk = false;
        timecnt = 0;
    }
#ifdef _NEED_LOG
    if (is_hands_can_tk) {
        LOG(COLOR_GREEN, "<tsm_process_hands_takeover> tk_torque_threshold: "
            "%f, is_hands_can_takeover: %d", tk_torque_threshold, 
            is_hands_can_tk);
    }
#endif

    (is_hands_can_tk) ?
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_HANDS_CAN_TAKEOVER) :
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                   BITNO_HANDS_CAN_TAKEOVER);
}

static void
tsm_process_driver_behavior(const struct tsm_entry* p_entry,
                            struct tsm_intermediate_sig* p_int_sig,
                            const enum tsm_mcu_mrm_func_st mrm_st) {
    tsm_veh_sig veh_sig = p_entry->in_can_gate->Vehicle_Signal_To_Tsm;
    tsm_process_drvr_attention_st(&veh_sig, p_int_sig);

    tsm_process_lng_override(&veh_sig, p_int_sig, mrm_st);

    tsm_process_brake_behavior(&veh_sig, p_int_sig, mrm_st);

    tsm_process_lat_override(&veh_sig, p_int_sig, mrm_st);

    tsm_process_hands_takeover(&veh_sig, p_int_sig, mrm_st);
}

static boolean 
tsm_is_veh_door_closed(const tsm_veh_sig* p_veh_sig) {
    static boolean ret = false;
    enum tsm_closed_st {
        CLOSED = 0,
        OPEN,
    };

    if ((p_veh_sig->BCM_DriverDoorAjarSt == CLOSED) && 
        (p_veh_sig->BCM_PsngrDoorAjarSt == CLOSED) &&
        (p_veh_sig->BCM_RLDoorAjarSt == CLOSED) &&
        (p_veh_sig->BCM_RRDoorAjarSt == CLOSED) &&
        (p_veh_sig->BCM_TrunkAjarSt == CLOSED) &&
        (p_veh_sig->BCM_BonnetAjarSt == CLOSED)) {
        ret = true;
    } else {
        ret = false;
    }
    // for debug
#ifdef _NEED_LOG
    if (!ret) {
        LOG(COLOR_RED, "<tsm_is_veh_door_closed> door is open.");
    }
#endif

    return ret;
}

static boolean 
tsm_is_seat_belt_fasten(const tsm_veh_sig* p_veh_sig) {
    static ret = false;
    enum tsm_fasten_st {
        FASTENED = 0,
        UNFASTENED,
        NOT_USED,
        INVALID,
    };

    if (p_veh_sig->SRS_DriverSeatBeltSt == FASTENED) {
        ret = true;
    } else if (p_veh_sig->SRS_DriverSeatBeltSt == UNFASTENED) {
        ret = false;
    }

    // for debug
#ifdef _NEED_LOG
    if (!ret) {
        LOG(COLOR_RED, "<tsm_is_seat_belt_fasten> seat belt not fasten.");
    }
#endif

    return ret;
}

static boolean
tsm_is_veh_in_drive_gear(const tsm_veh_sig* p_veh_sig) {
    static ret = false;
    enum tsm_gear_level {
        INVALID = 0,
        DRIVE_GEAR,
        NEUTRAL_GEAR,
        REVERSE_GEAR,
        PARK_GEAR,
    };
    enum tsm_veh_rdy_st {
        NOT_READY = 0,
        READY,
    };

    if (p_veh_sig->VCU_CrntGearLvlVD && (p_veh_sig->VCU_VehRdySt == READY) &&
        (p_veh_sig->VCU_CrntGearLvl == DRIVE_GEAR)) {
        ret = true;
    } else {
        ret = false;
    }

    // for debug
#ifdef _NEED_LOG
    if (!ret) {
        LOG(COLOR_RED, "<tsm_is_veh_in_drive_gear> gear not in drive.");
    }
#endif

    return ret;
}

static boolean 
tsm_process_veh_motion_params(const tsm_veh_sig* p_veh_sig,
                              const enum tsm_motion_params_type param_type,
                              const enum tsm_nda_active_st nda_active_st,
                              const boolean judge_exceed, const boolean last_st, 
                              uint16* p_timecnt) {
    struct param_threshold {
        float32* true_threshold[2];
        float32* false_threshold[2];
        uint16* time_threshold;
    };

    static const struct param_threshold motion_params_thold[] = {
        [YAW_RATE] = {
            .true_threshold = {
                [NDA_ACTIVE] = &K_YAW_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_YAW_THRESHOLD_NotActiveSt,
            },
            .false_threshold = {
                [NDA_ACTIVE] = &K_YAW_false_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_YAW_false_THRESHOLD_NotActiveSt,
            },
            .time_threshold = &K_YAW_TimeThreshold_Cnt,
        },
        [LAT_ACC] = {
            .true_threshold = {
                [NDA_ACTIVE] = &K_LatAcc_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_LatAcc_THRESHOLD_NotActiveSt,
            },
            .false_threshold = {
                [NDA_ACTIVE] = &K_LatAcc_false_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_LatAcc_false_THRESHOLD_NotActiveSt,
            },
            .time_threshold = &K_LatAcc_TimeThreshold_Cnt,
        },
        [LNG_ACC] = {
            .true_threshold = {
                [NDA_ACTIVE] = &K_LngAcc_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_LngAcc_THRESHOLD_NotActiveSt,
            },
            .false_threshold = {
                [NDA_ACTIVE] = &K_LngAcc_false_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_LngAcc_false_THRESHOLD_NotActiveSt,
            },
            .time_threshold = &K_LngAcc_TimeThreshold_Cnt,
        },
        [STEERING_WHL_ANGLE_SPD] = {
            .true_threshold = {
                [NDA_ACTIVE] = &K_StrWhlAngSpd_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_StrWhlAngSpd_THRESHOLD_NotActiveSt,
            },
            .false_threshold = {
                [NDA_ACTIVE] = &K_StrWhlAngSpd_false_THRESHOLD_ActiveSt,
                [NDA_NOT_ACTIVE] = &K_StrWhlAngSpd_false_THRESHOLD_NotActiveSt,
            },
            .time_threshold = &K_StrWhlAngSpd_TimeThreshold_Cnt,
        },
    };

    float32 param_value = 0;
    if (param_type == YAW_RATE) {
        param_value = p_veh_sig->BCS_YawRate;
    } else if (param_type == LAT_ACC) {
        param_value = p_veh_sig->BCS_ActVehLaltrlAccel;
    } else if (param_type == LNG_ACC) {
        param_value = p_veh_sig->BCS_ActVehLongAccel;
    } else if (param_type == STEERING_WHL_ANGLE_SPD) {
        param_value = p_veh_sig->EPS_SteeringAngleSpd;
    }

    boolean is_motion_param_satisfy = false;
    boolean ret_st = last_st;
    float32 param_value_thold = 0.0;
    uint16 time_cnt_thold =
        *motion_params_thold[param_type].time_threshold;
    if (judge_exceed) {
        param_value_thold = 
            *motion_params_thold[param_type].false_threshold[nda_active_st];
        is_motion_param_satisfy = (fabs(param_value) > param_value_thold);
    } else {
        param_value_thold = 
            *motion_params_thold[param_type].true_threshold[nda_active_st];
        is_motion_param_satisfy = (fabs(param_value) < param_value_thold);
    }

    if (is_motion_param_satisfy) {
        ret_st = (judge_exceed) ? 
            is_flag_reset_with_timecnt(time_cnt_thold, p_timecnt, ret_st) :
            is_flag_set_with_timecnt(time_cnt_thold, p_timecnt, ret_st);
    } else {
        *p_timecnt = 0;
    }

    // LOG(COLOR_YELLOW, "<tsm_process_veh_motion_params> param_type: %d, "
    //     "active st: %d, st: %d, threshold: %f", param_type, nda_active_st,
    //     !judge_exceed, param_value_thold);
    return ret_st;
}

static boolean
tsm_is_veh_yawrate_satisfy(const tsm_veh_sig* p_veh_sig,
                           const enum nda_func_st nda_st) {
    static uint16 false_timecnt = 0;
    static uint16 true_timecnt = 0;
    static boolean is_veh_yawrate_satisfy = true;

    enum tsm_nda_active_st nda_active_st =
        (tsm_is_nda_active(nda_st)) ? NDA_ACTIVE : NDA_NOT_ACTIVE;

    is_veh_yawrate_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, YAW_RATE, nda_active_st, 
                                      false, is_veh_yawrate_satisfy, 
                                      &true_timecnt);
    is_veh_yawrate_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, YAW_RATE, nda_active_st, 
                                      true, is_veh_yawrate_satisfy, 
                                      &false_timecnt);
#ifdef _NEED_LOG
    if (!is_veh_yawrate_satisfy) {
        LOG(COLOR_RED, "<tsm_is_veh_yawrate_satisfy> yawrate: %f, "
            "is_veh_yawrate_satisfy: %d", p_veh_sig->BCS_YawRate, 
            is_veh_yawrate_satisfy);
    }
#endif
    return is_veh_yawrate_satisfy;
}

static boolean
tsm_is_veh_latacc_satisfy(const tsm_veh_sig* p_veh_sig,
                          const enum nda_func_st nda_st) {
    static uint16 false_timecnt = 0;
    static uint16 true_timecnt = 0;
    static boolean is_veh_latacc_satisfy = true;

    enum tsm_nda_active_st nda_active_st =
        (tsm_is_nda_active(nda_st)) ? NDA_ACTIVE : NDA_NOT_ACTIVE;

    is_veh_latacc_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, LAT_ACC, nda_active_st, 
                                      false, is_veh_latacc_satisfy, 
                                      &true_timecnt);
    is_veh_latacc_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, LAT_ACC, nda_active_st, 
                                      true, is_veh_latacc_satisfy, 
                                      &false_timecnt);
#ifdef _NEED_LOG
    if (!is_veh_latacc_satisfy) {
        LOG(COLOR_RED, "<tsm_is_veh_latacc_satisfy> lat_acc: %f, "
            "is_veh_latacc_satisfy: %d", p_veh_sig->BCS_ActVehLaltrlAccel, 
            is_veh_latacc_satisfy);
    }
#endif
    return is_veh_latacc_satisfy;
}

static boolean
tsm_is_veh_lngacc_satisfy(const tsm_veh_sig* p_veh_sig,
                          const enum nda_func_st nda_st) {
    static uint16 false_timecnt = 0;
    static uint16 true_timecnt = 0;
    static boolean is_veh_lngacc_satisfy = true;

    enum tsm_nda_active_st nda_active_st =
        (tsm_is_nda_active(nda_st)) ? NDA_ACTIVE : NDA_NOT_ACTIVE;

    is_veh_lngacc_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, LNG_ACC, nda_active_st, 
                                      false, is_veh_lngacc_satisfy, 
                                      &true_timecnt);
    is_veh_lngacc_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, LNG_ACC, nda_active_st, 
                                      true, is_veh_lngacc_satisfy, 
                                      &false_timecnt);
#ifdef _NEED_LOG
    if (!is_veh_lngacc_satisfy) {
        LOG(COLOR_RED, "<tsm_is_veh_lngacc_satisfy> lng_acc: %f, "
            "is_veh_lngacc_satisfy: %d", p_veh_sig->BCS_ActVehLongAccel, 
            is_veh_lngacc_satisfy);
    }
#endif
    return is_veh_lngacc_satisfy;
}

static boolean
tsm_is_str_whl_ang_spd_satisfy(const tsm_veh_sig* p_veh_sig,
                               const enum nda_func_st nda_st) {
    static uint16 false_timecnt = 0;
    static uint16 true_timecnt = 0;
    static boolean is_strnwhlang_spd_satisfy = true;

    enum tsm_nda_active_st nda_active_st =
        (tsm_is_nda_active(nda_st)) ? NDA_ACTIVE : NDA_NOT_ACTIVE;

    is_strnwhlang_spd_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, STEERING_WHL_ANGLE_SPD, 
                                      nda_active_st, false, 
                                      is_strnwhlang_spd_satisfy, &true_timecnt);
    is_strnwhlang_spd_satisfy = 
        tsm_process_veh_motion_params(p_veh_sig, STEERING_WHL_ANGLE_SPD, 
                                      nda_active_st, true, 
                                      is_strnwhlang_spd_satisfy, 
                                      &false_timecnt);
#ifdef _NEED_LOG
    if (!is_strnwhlang_spd_satisfy) {
        LOG(COLOR_RED, "<tsm_is_str_whl_ang_spd_satisfy> strn whl ang spd: %f, "
            "is_strnwhlang_spd_satisfy: %d", p_veh_sig->EPS_SteeringAngleSpd, 
            is_strnwhlang_spd_satisfy);
    }
#endif
    return is_strnwhlang_spd_satisfy;
}

static boolean 
tsm_is_epb_satisfy(const tsm_veh_sig* p_veh_sig) {
    static boolean is_epb_satisfy = false;
    enum tsm_epb_switch_st {
        NOT_ACTIVE = 0,
        RELEASED,
        APPLIED,
        NOT_USED,
    };

    enum tsm_epb_sys_st {
        FULLY_RELEASED = 0,
        FULLY_APPLIED,
        RELEASING,
        APPLYING,
        DISENGAGED,
        FAULT = 7,
    };

    enum tsm_epb_switch_st epb_switch_st = p_veh_sig->EPB_SwitchSt;
    enum tsm_epb_sys_st epb_system_st = p_veh_sig->EPB_SysSt;
    boolean is_ebp_switch_not_applied = (epb_switch_st != APPLIED);
    
    if (p_veh_sig->EPB_SwitchStVD && is_ebp_switch_not_applied && 
        (epb_system_st == FULLY_RELEASED)) {
        is_epb_satisfy = true;
    } else {
        is_epb_satisfy = false;
    }
#ifdef _NEED_LOG
    if (!is_epb_satisfy) {
        LOG(COLOR_RED, "<tsm_is_veh_in_drive_gear> epb is not satisfy.");
    }
#endif
    return is_epb_satisfy;
}

static boolean
tsm_is_actuator_active(const boolean false_flag, 
                       const uint16 timecnt_threshold, 
                       uint16* cur_timecnt) {
    boolean ret = false;
    
    if (false_flag) {
        if (*cur_timecnt > timecnt_threshold) {
            ret = true;
        } else {
            ++(*cur_timecnt);
            ret = false;
        }
    } else {
        *cur_timecnt = 0;
        ret = false;
    }

    return ret;
}

static boolean
tsm_is_chassis_safety_satisfy(const tsm_veh_sig* p_veh_sig) {
    enum tsm_hdc_ctrl_st {
        OFF,
        ON_ACTIVE_BRAKING,
        ON_NOT_ACTIVE_BRAKING,
        NOT_USED
    };

    enum tsm_actuator_active_st {
        NOT_ACTIVE,
        ACTIVE
    };

    enum tsm_cdd_available_st {
        NOT_AVAILABLE,
        AVAILABLE
    };

    static boolean is_chassis_safety = true;
    uint8_t hdc_ctrl_st = p_veh_sig->BCS_HDCCtrlSt;
    uint8_t vdc_active_st = p_veh_sig->BCS_VDCActiveS;
    uint8_t abs_active_st = p_veh_sig->BCS_ABSActiveSt;
    uint8_t tcs_active_st = p_veh_sig->BCS_TCSActiveSt;
    uint8_t ebd_active_st = p_veh_sig->BCS_EBDActiveSt;
    uint8_t cdd_available = p_veh_sig->BCS_CDDAvailable;

    boolean is_hdc_ctrl_not_active = 
        ((hdc_ctrl_st == OFF) || (hdc_ctrl_st == ON_NOT_ACTIVE_BRAKING));
    
    if ((vdc_active_st == NOT_ACTIVE) && is_hdc_ctrl_not_active &&
        (abs_active_st == NOT_ACTIVE) && (tcs_active_st == NOT_ACTIVE) &&
        (ebd_active_st == NOT_ACTIVE) && (cdd_available == AVAILABLE)) {
        is_chassis_safety = true;
    }

    boolean is_hdc_ctrl_active = 
        ((hdc_ctrl_st == ON_ACTIVE_BRAKING) || (hdc_ctrl_st == NOT_USED));
    static uint16 hdc_false_timecnt = 0;
    is_hdc_ctrl_active = 
        tsm_is_actuator_active(is_hdc_ctrl_active, K_HDCCtrlFalseTime_Cnt,
                               &hdc_false_timecnt);

    boolean is_vdc_active = (vdc_active_st == ACTIVE);
    static uint16 vdc_false_timecnt = 0;
    is_vdc_active =
        tsm_is_actuator_active(is_vdc_active, K_VDCActiveFalseTime_Cnt,
                               &vdc_false_timecnt);
    
    boolean is_abs_active = (abs_active_st == ACTIVE);
    static uint16 abs_false_timecnt = 0;
    is_abs_active = 
        tsm_is_actuator_active(is_abs_active, K_ABSActiveFalseTime_Cnt,
                               &abs_false_timecnt);

    boolean is_tcs_active = (tcs_active_st == ACTIVE);
    static uint16 tcs_false_timecnt = 0;
    is_tcs_active =
        tsm_is_actuator_active(is_tcs_active, K_TCSActiveFalseTime_Cnt,
                               &tcs_false_timecnt);
    
    boolean is_ebd_active = (ebd_active_st == ACTIVE);
    static uint16 ebd_false_timecnt = 0;
    is_ebd_active = 
        tsm_is_actuator_active(is_ebd_active, K_EBDActiveFalseTime_Cnt,
                               &ebd_false_timecnt);
    
    boolean is_cdd_not_available = (cdd_available == NOT_ACTIVE);

    if (is_hdc_ctrl_active || is_hdc_ctrl_active || is_abs_active || 
        is_tcs_active || is_ebd_active || is_cdd_not_available) {
        is_chassis_safety = false;
    }

    // LOG(COLOR_YELLOW, "<tsm_is_chassis_safety_satisfy> is_chassis_safety: %d",
    //     is_chassis_safety);
    return is_chassis_safety;
}

static boolean
tsm_is_user_set_hazard_light(const tsm_veh_sig* p_veh_sig,
                             const tsm_monitor_sig* p_monitor_sig) {
    static boolean ret = false;
    enum tsm_hazard_lamp_st {
        NOT_ACTIVE = 0,
        ACTIVE,
    };
    boolean is_sys_open_flag = p_monitor_sig->is_sys_open_flag;

    if ((p_veh_sig->BCM_HazardLampSt == ACTIVE) && !is_sys_open_flag) {
        ret = true;
    } else if ((p_veh_sig->BCM_HazardLampSt == NOT_ACTIVE) || 
                is_sys_open_flag ) {
        ret = false;
    }
#ifdef _NEED_LOG
    if (ret) {
        LOG(COLOR_RED, "<tsm_is_veh_in_drive_gear> user set hazard light.");
    }
#endif
    return ret;
}

static boolean 
tsm_is_key_st_on(const tsm_veh_sig* p_veh_sig) {
    static boolean is_key_st_on = false;
    enum tsm_bcm_key_st {
        OFF = 0,
        ACC,
        ON,
        CRANK,
    };

    if (p_veh_sig->BCM_KeySt == ON) {
        is_key_st_on = true;
    } else {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "<tsm_is_veh_cond_satisfy> key_st is not on.");
#endif
        is_key_st_on = false;
    }
    return  is_key_st_on;
} 

static boolean
tsm_is_veh_cond_satisfy(const struct tsm_entry* p_entry) {
    tsm_veh_sig veh_sig = p_entry->in_can_gate->Vehicle_Signal_To_Tsm;
    tsm_monitor_sig monitor_sig = 
        p_entry->in_can_gate->Soc_Info.monitor_sig_src;
    enum nda_func_st nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;

    boolean is_veh_posture_satisfy = 
        p_entry->in_can_gate->Soc_Info.monitor_sig_src.Vehicle_Posture_St;
    boolean is_veh_position_satisfy = 
        p_entry->in_can_gate->Soc_Info.monitor_sig_src.Vehicle_Position_St;
#ifdef _NEED_LOG
    if (!is_veh_posture_satisfy || !is_veh_position_satisfy) {
        LOG(COLOR_RED, "<tsm_is_veh_cond_satisfy> vehicle position or posture "
            "is not satisfy.");
    }
#endif
    
    return (tsm_is_veh_door_closed(&veh_sig) &&
            tsm_is_seat_belt_fasten(&veh_sig) &&
            tsm_is_veh_in_drive_gear(&veh_sig) &&
            tsm_is_veh_yawrate_satisfy(&veh_sig, nda_st) &&
            tsm_is_veh_latacc_satisfy(&veh_sig, nda_st) &&
            tsm_is_veh_lngacc_satisfy(&veh_sig, nda_st) &&
            tsm_is_str_whl_ang_spd_satisfy(&veh_sig, nda_st) &&
            tsm_is_epb_satisfy(&veh_sig) &&
            tsm_is_chassis_safety_satisfy(&veh_sig) && 
            !tsm_is_user_set_hazard_light(&veh_sig, &monitor_sig) &&
            tsm_is_key_st_on(&veh_sig) && is_veh_posture_satisfy &&
            is_veh_position_satisfy);
}

static boolean
tsm_is_rf_lvl_not_satisfy(const uint32 rainfall_level) {
    enum bitno_rainfall_level {
        BITNO_NO_RAIN = 0,
        BITNO_LEVEL_1,
        BITNO_LEVEL_2,
        BITNO_LEVEL_3,
        BITNO_LEVEL_4,
        BITNO_INIT_VALUE = 14,
        BITNO_INVALID_VALUE = 15,
    };

    static boolean is_rf_lvl_not_satisfy = false;
    static uint16 timecnt = 0;
    if (!tsm_is_bit_set(rainfall_level, BITNO_NO_RAIN) && 
        !tsm_is_bit_set(rainfall_level, BITNO_LEVEL_1) &&
        !tsm_is_bit_set(rainfall_level, BITNO_LEVEL_2)) {
        is_rf_lvl_not_satisfy = 
            is_flag_set_with_timecnt(K_RainlevelTime_Cnt, &timecnt, 
                                     is_rf_lvl_not_satisfy);
    } else {
        is_rf_lvl_not_satisfy = false;
        timecnt = 0;
    }

    return is_rf_lvl_not_satisfy;
}



static boolean
tsm_is_front_wiper_fast(const uint8 front_wiper_st) {
    enum tsm_front_wiper_st {
        OFF,
        ERROR,
        CONTINUOUS_SLOW,
        CONTINUOUS_FAST,
    };

    static boolean is_front_wiper_fast = false;
    static uint16 timecnt = 0;

    if (front_wiper_st == CONTINUOUS_FAST) {
        is_front_wiper_fast =
            is_flag_set_with_timecnt(K_WiperFastTime_Cnt, &timecnt,
                                     is_front_wiper_fast);
    } else {
        is_front_wiper_fast = false;
        timecnt = 0;
    }

    return is_front_wiper_fast;
}

static boolean 
tsm_is_rainfall_discontent(const uint32 rainfall_level, 
                           const uint8 front_wiper_st) {
    return (tsm_is_rf_lvl_not_satisfy(rainfall_level) ||
            tsm_is_front_wiper_fast(front_wiper_st));
}

static boolean 
tsm_is_veh_in_eea() {
    // todo:
    return true;
}

static boolean
tsm_is_veh_exceed_warning_dist(const sint32 warning_distance, 
                               const enum tsm_nda_active_st state) {
    sint32  warning_dist_threshold = 
        (state == NDA_ACTIVE) ? K_GeoEndDist_Active : K_GeoEndDist_NotActive;
    
    return (warning_distance > warning_dist_threshold);
}

static boolean
tsm_is_odd_avl(const struct tsm_entry* p_entry,
               const enum tsm_nda_active_st state) { 
    boolean is_veh_in_eea = tsm_is_veh_in_eea();
    sint32 warning_dist = 
        p_entry->in_can_gate->Soc_Info.monitor_sig_src.HD_Map_Warning_Dist;
    boolean is_warning_dist_exceed =
        tsm_is_veh_exceed_warning_dist(warning_dist, state);
    float32 veh_spd = 
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehSpd;
    boolean is_veh_spd_lower = (veh_spd < K_VehSpdThresholdInOddCheck);
    float32 cal_light_lux =
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.RLS_AMBBrightness;
    boolean is_lux_up = (cal_light_lux > K_lightLuxThresholdInOddCheck);
    uint32 rainfall_level = 
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.RLS_RainfallLevel;
    uint8 front_wiper_st =
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCM_FrontWiperSt;
    boolean is_rainfall_content = 
        !tsm_is_rainfall_discontent(rainfall_level, front_wiper_st);

    boolean is_odd_avl_after_active = 
        (is_veh_in_eea && is_warning_dist_exceed && 
         is_lux_up && is_rainfall_content);

    if (state == NDA_ACTIVE) {
        return is_odd_avl_after_active;
    } else {
        return (is_odd_avl_after_active && is_veh_spd_lower);
    }
}

static boolean 
tsm_is_nda_passive_vd() {
    // todo:
    return true;
}

static boolean
tsm_is_nda_need_phase_in(const struct tsm_entry* p_entry) {
    // todo:
    return (!p_entry->in_ctrl_arb->as_info.AS_Switch_St.AEB &&
            !p_entry->in_ctrl_arb->as_info.AS_Switch_St.AES &&
            !p_entry->in_ctrl_arb->as_info.AS_Switch_St.ELK &&
            !p_entry->in_ctrl_arb->as_info.AS_Switch_St.ESA);
}

static boolean
tsm_process_parking_meter(const enum nda_func_st nda_st, 
                          const enum tsm_veh_standstill_st standstill_st,
                          struct tsm_intermediate_sig* p_int_sig) {
    boolean is_nda_in_active =
        ((nda_st == NDA_ACTIVE_HANDS_ON_NORMAL) ||
         (nda_st == NDA_ACTIVE_HANDS_ON_STAND_ACTIVE) ||
         (nda_st == NDA_ACTIVE_HANDS_ON_STAND_WAIT));
    static uint16 parking_meter_timecnt = 0;
    if (is_nda_in_active && (standstill_st == VEH_STANDSTILL_ST_STANDSTILL) &&
        !tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_LNG_OVERRIDE_ST)) {
        if (parking_meter_timecnt >= K_Standstill_waitTime_Cnt) {
            parking_meter_timecnt = (K_Standstill_waitTime_Cnt + 1);
        } else {
            ++parking_meter_timecnt;
        }
    } else {
        parking_meter_timecnt = 0;
    }

    return parking_meter_timecnt;
}

static void
tsm_process_monitor_signal(const struct tsm_entry* p_entry,
                           struct tsm_intermediate_sig* p_int_sig) {
    static boolean is_nda_avl_before_active = false;
    static boolean is_nda_avl_after_active = false;

    enum tsm_nda_enable_st {
        NDA_NOT_ENABLE = 0,
        NDA_ENABLE,
    };
    boolean is_odd_avl_before_active = tsm_is_odd_avl(p_entry, NDA_NOT_ACTIVE);
    boolean is_odd_avl_after_active = tsm_is_odd_avl(p_entry, NDA_ACTIVE);
    enum tsm_drvr_attention_st driver_att_st = p_int_sig->drvr_att_st;
    boolean is_nda_passive_vd = tsm_is_nda_passive_vd();
    uint8 nda_system_fault_level =
        p_entry->in_can_gate->Soc_Info.monitor_sig_src.NDA_System_Fault_Level;
    enum tsm_nda_enable_st nda_enable_st =
        p_entry->in_can_gate->Soc_Info.monitor_sig_src.NDA_Enable_State;
    boolean is_nda_need_phase_in = tsm_is_nda_need_phase_in(p_entry);
    boolean is_sd_hd_match_st = 
        p_entry->in_can_gate->Soc_Info.monitor_sig_src.SD_Map_HD_Map_Match_St;
    boolean is_user_set_navi =
        p_entry->in_can_gate->Soc_Info.monitor_sig_src.User_Set_Navi_Status;
    boolean is_veh_cond_satisfy = tsm_is_veh_cond_satisfy(p_entry);
    tsm_veh_sig veh_sig = p_entry->in_can_gate->Vehicle_Signal_To_Tsm;
    boolean is_hand_torq_odc = tsm_process_lat_torq_odc(&veh_sig);

    boolean is_common_cond_satisfy = 
        (is_nda_passive_vd && (nda_system_fault_level == 0) &&
         (nda_enable_st == NDA_ENABLE) && !is_nda_need_phase_in &&
         (is_sd_hd_match_st == 1) && is_user_set_navi && is_veh_cond_satisfy);
    
    if (is_common_cond_satisfy && is_odd_avl_before_active && 
        (driver_att_st == DRVR_AWAKE_NOT_DISTRACTED) && is_hand_torq_odc) {
        is_nda_avl_before_active = true;
    } else {
        is_nda_avl_before_active = false;
    }

    if (is_common_cond_satisfy && is_odd_avl_after_active &&
        (driver_att_st != DRVR_FATIGUE)) {
        is_nda_avl_after_active = true;
    } else {
        is_nda_avl_after_active = false;
    }

    (is_nda_avl_before_active) ?
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_NDA_AVL_BEFORE_ACT) :
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                   BITNO_NDA_AVL_BEFORE_ACT);

    (is_nda_avl_after_active) ?
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_NDA_AVL_AFTER_ACT) :
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                   BITNO_NDA_AVL_AFTER_ACT);
    
    enum nda_func_st nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    enum tsm_veh_standstill_st veh_still_st = 
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt;
    p_int_sig->parking_meter_cnt =     
        tsm_process_parking_meter(nda_st, veh_still_st, p_int_sig);
}

static boolean
tsm_process_as_active(const struct tsm_entry* p_entry,
                      struct tsm_intermediate_sig* p_int_sig) {
    boolean is_as_ctrl = 
        (p_entry->in_ctrl_arb->as_info.AS_lat_ctrl_st ||
         p_entry->in_ctrl_arb->as_info.AS_lng_ctrl_st);
    (is_as_ctrl) ? 
        tsm_set_bit_in_bitfields(&p_int_sig->int_sig_bitfields, 
                                 BITNO_AS_ACTIVE) :
        tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                   BITNO_AS_ACTIVE);
}

void 
tsm_preprocess_input(const struct tsm_entry* p_entry,
                     const enum tsm_mcu_mrm_func_st mrm_st,
                     struct tsm_intermediate_sig* p_int_sig) {
    tsm_process_as_active(p_entry, p_int_sig);

    tsm_process_driver_behavior(p_entry, p_int_sig, mrm_st);

    tsm_process_monitor_signal(p_entry, p_int_sig);
}