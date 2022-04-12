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

// --------------------------------- global variable definition ---------------------------------------
InterMediaMsg g_inter_media_msg;
// --------------------------------- function definition        ---------------------------------------
void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
                    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    // 判断驾驶员注意力状态
    DrvrAttentionStJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 驾驶员是否踩下油门, 系统需求中无此项，针对3.17测试增加
    DriverGasPedalAppliedJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 纵向超越标志位
    LngOverrideFlagJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 刹车是否踩下
    BrakeIsSetJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 刹车长时介入标志位
    BrakeInervationFlagJudge();
    // 判断 驾驶员手力矩超越标志位
    DriveHandTorqueOverrideStJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 监控 SOC侧NDA的状态跳转
    MonitorNdaStateTransition(&rtu_DeCANGATE2TSM->Soc_Info.Automaton_State);
    // 判断 SOC侧NDA的跳转是否错误, 初始版本，先用harzard light去判断跳转是否正常
    NdaStTransitNormalJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm, &rtu_DeCANGATE2TSM->Soc_Info);
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "lng_override_long_duration_flag: %d, brake_is_set: %d, driver_acc_pedal_applied_flag: %d, "
        "driver_hand_torque_st: %d", g_inter_media_msg.lng_override_long_duration_flag, 
        g_inter_media_msg.brake_is_set, g_inter_media_msg.driver_acc_pedal_applied_flag, 
        g_inter_media_msg.driver_hand_torque_st);
#endif
}

void DrvrAttentionStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    // 判断 清醒且未分神
    if (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)DRVR_FATIGUE_ST_ALERT) {
        if ((vehicle_signal->DMS_DrvrDetSts == (uint8)DRVR_DET_ST_EYE_DETECTED) ||
            (vehicle_signal->DMS_DrvrDetSts == (uint8)DRVR_DET_ST_HEAD_DETECTED)) {
            if (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_WINDSHIELD) {
                g_inter_media_msg.driver_attention_st = AWAKE_AND_NOT_DISTRACTED;
                return;
            }
            // 判断 清醒且注视后视镜||车机
            if ((vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_REARVIEW_MIRROR) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_INST_CLUSTER) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_INFOTAIMENT_DISPLAY)) {
                g_inter_media_msg.driver_attention_st = AWAKE_AND_LOOK_REARVIEW_OR_HU;
                return;
            }
        }
        // 判断 清醒且分神
        if (vehicle_signal->DMS_DrvrDetSts == (uint8)DRVR_DET_ST_NOT_DETECTED) {
            if ((vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_UNAVAILABLE) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_DRVR_SIDEWINDOW) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_PAX_SIDEWINDOW) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_DRVR_SIDEDOOR_PANEL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_DRVR_SIDEFOOTWELL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_PAX_SIDEDOOR_PANEL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_PAX_SIDE_FOOTWELL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INT_RGN_OTHER_AREA)) {
                g_inter_media_msg.driver_attention_st = AWAKE_AND_DISTRACTED;
                return;
            }
        }
    }

    // 判断 疲劳
    if ((vehicle_signal->DMS_L3DriverFatigueSt == (uint16)DRVR_FATIGUE_ST_FATIGUE) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)DRVR_FATIGUE_ST_H_FATIGUE) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)DRVR_FATIGUE_ST_UNKNOWN) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)DRVR_FATIGUE_ST_SU) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)DRVR_FATIGUE_ST_CU)) {
        g_inter_media_msg.driver_attention_st = FATIGUE_DRIVER_ATTENTION_ST;
        return;
    }
    
    g_inter_media_msg.driver_attention_st = UNKNOWN;
}

void LngOverrideFlagJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
#ifndef CONSUME_TIME
    static uint16   lng_override_cnt = 0;
    static VarValue var_value        = {1, 0, 0};

    var_value.time_threshold_cnt = K_LngOverrideTakeOverTime_Cnt;

    // TODO: 未加变道状态
    if ((vehicle_signal->VCU_AccDriverOrvd == DRIVER_OVERRIDE) || g_inter_media_msg.driver_acc_pedal_applied_flag) {
        g_inter_media_msg.lng_override_st = OVERRIDE_SATISFY;
        FlagSetWithTimeCount(&g_inter_media_msg.lng_override_long_duration_flag, &lng_override_cnt, &var_value);
    } else {
        g_inter_media_msg.lng_override_st                 = OVERRIDE_NOT_SATISFY;
        lng_override_cnt                                  = 0;
        g_inter_media_msg.lng_override_long_duration_flag = var_value.flag_unset_val;
    }
#else
    static sint64         lngOverride_time        = 0;
    static VarValueInTime var_value               = {1, 0, 0.0};
    static uint8          lngOverride_timing_flag = 0;

    var_value.time_threshold = K_LngOverrideTakeOverTime;
    if ((vehicle_signal->VCU_AccDriverOrvd == DRIVER_OVERRIDE) || g_inter_media_msg.driver_acc_pedal_applied_flag) {
        g_inter_media_msg.lng_override_st = OVERRIDE_SATISFY;
        if (!lngOverride_timing_flag) {
            StartTiming(&lngOverride_time, &lngOverride_timing_flag);
        }
        FlagSetWithTime(&g_inter_media_msg.lng_override_long_duration_flag, lngOverride_time, 
            lngOverride_timing_flag, &var_value);
    } else {
        StopTiming(&lngOverride_timing_flag);
        g_inter_media_msg.lng_override_st                 = OVERRIDE_NOT_SATISFY;
        g_inter_media_msg.lng_override_long_duration_flag = var_value.flag_unset_val;
    }
#endif
}

void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
#ifndef CONSUME_TIME
    static uint16   brakeset_cnt = 0;
    static VarValue var_value    = {1, 0, 0};

    var_value.time_threshold_cnt = K_BrakPedalAppliedThresholdTime_Cnt;

    if ((vehicle_signal->EBB_BrkPedalAppliedSt == BRK_APPLIED_ST_NORMAL) && 
        (vehicle_signal->EBB_BrkPedalApplied == BRAKE_PEDAL_APPLIED)) {
        FlagSetWithTimeCount(&g_inter_media_msg.brake_is_set, &brakeset_cnt, &var_value);
    } else {
        g_inter_media_msg.brake_is_set = var_value.flag_unset_val;
        brakeset_cnt                     = 0;
    }
#else
    static sint64         brakeset_time        = 0;
    static VarValueInTime var_value            = {1, 0, 0.0};
    static uint8          brakeset_timing_flag = 0;
    var_value.time_threshold = K_BrakPedalAppliedThresholdTime;
    if ((vehicle_signal->EBB_BrkPedalAppliedSt == BRK_APPLIED_ST_NORMAL) && 
        (vehicle_signal->EBB_BrkPedalApplied == BRAKE_PEDAL_APPLIED)) {
        if (!brakeset_timing_flag) {
            StartTiming(&brakeset_time, &brakeset_timing_flag);
        }
        FlagSetWithTime(&g_inter_media_msg.brake_is_set, brakeset_time, brakeset_timing_flag, &var_value);
    } else {
        StopTiming(&brakeset_timing_flag);
        g_inter_media_msg.brake_is_set = var_value.flag_unset_val;
    }
#endif
}

void DriverGasPedalAppliedJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
#ifndef CONSUME_TIME
    static uint16   gasPedalPos_cnt = 0;
    static VarValue var_value       = {1, 0, 0};

    var_value.time_threshold_cnt = K_GasPedalAppliedThresholdTime_Cnt;
    if (vehicle_signal->EMS_GasPedalActPstforMRRVD && 
        (vehicle_signal->EMS_GasPedalActPstforMRR > K_GasPedalPosThresholdValue)) {
        FlagSetWithTimeCount(&g_inter_media_msg.driver_acc_pedal_applied_flag, &gasPedalPos_cnt, &var_value);
    } else {
        g_inter_media_msg.driver_acc_pedal_applied_flag = var_value.flag_unset_val;
        gasPedalPos_cnt                                     = 0;
    }
#else
    static sint64         gasPedalPos_time            = 0;
    static VarValueInTime var_value                   = {1, 0, 0.0};
    static uint8          gasPedalApplied_timing_flag = 0;

    var_value.time_threshold = K_GasPedalAppliedThresholdTime;
    if (vehicle_signal->EMS_GasPedalActPstforMRRVD && 
        (vehicle_signal->EMS_GasPedalActPstforMRR > K_GasPedalPosThresholdValue)) {
        if (!gasPedalApplied_timing_flag) {
            StartTiming(&gasPedalPos_time, &gasPedalApplied_timing_flag);
        }
        FlagSetWithTime(&g_inter_media_msg.driver_acc_pedal_applied_flag, gasPedalPos_time, 
            gasPedalApplied_timing_flag, &var_value);
    } else {
        StopTiming(&gasPedalApplied_timing_flag);
        g_inter_media_msg.driver_acc_pedal_applied_flag = var_value.flag_unset_val;
    }
#endif
}

void DriveHandTorqueOverrideStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    if ((vehicle_signal->HOD_FaultStatus == 0) && (vehicle_signal->HOD_CalibrationSt == 1)) {
        TorqueOverrideStJudgeWithHodDetection(vehicle_signal);
    } else {
        TorqueOverrideStJudgeWithoutHodDetection(vehicle_signal);
    }
}

void TorqueOverrideStJudgeWithHodDetection(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    static float32  overrideHandTorqThreshold = 0;

    if (vehicle_signal->HOD_TouchZone1 && vehicle_signal->HOD_TouchZone2 && vehicle_signal->HOD_TouchZone3) {
        overrideHandTorqThreshold = K_OverrideHandTorqThreshold_ThreeZone;
    } else if ((vehicle_signal->HOD_TouchZone1 && vehicle_signal->HOD_TouchZone1) || 
               (vehicle_signal->HOD_TouchZone2 && vehicle_signal->HOD_TouchZone3) || 
               (vehicle_signal->HOD_TouchZone1 && vehicle_signal->HOD_TouchZone3)) {
        overrideHandTorqThreshold = K_OverrideHandTorqThreshold_TwoZone;
    } else {
        overrideHandTorqThreshold = K_OverrideHandTorqThreshold_LessTwoZone;
    }

#ifndef CONSUME_TIME
    static uint16   lat_override_withHOD_cnt  = 0;
    static VarValue var_value                 = {OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY, 0};
    var_value.time_threshold_cnt = (uint16)K_OverrideHandTorqCheckTime_Cnt;
    if (vehicle_signal->EPS_StrngWhlTorqVD && (fabs(vehicle_signal->EPS_StrngWhlTorq) > overrideHandTorqThreshold)) {
        FlagSetWithTimeCount(&g_inter_media_msg.driver_hand_torque_st, &lat_override_withHOD_cnt, &var_value);
    } else {
        g_inter_media_msg.driver_hand_torque_st = var_value.flag_unset_val;
        lat_override_withHOD_cnt                = 0;
    }
#else
    static sint64         latOverrideWithHOD_time        = 0;
    static VarValueInTime var_value                      = {OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY, 0.0};
    static uint8          latOverrideWithHOD_timing_flag = 0;

    var_value.time_threshold = K_OverrideHandTorqCheckTime;
    if (vehicle_signal->EPS_StrngWhlTorqVD && (fabs(vehicle_signal->EPS_StrngWhlTorq) > overrideHandTorqThreshold)) {
        if (!latOverrideWithHOD_timing_flag) {
            StartTiming(&latOverrideWithHOD_time, &latOverrideWithHOD_timing_flag);
        }
        FlagSetWithTime(&g_inter_media_msg.driver_hand_torque_st, latOverrideWithHOD_time, 
            latOverrideWithHOD_timing_flag, &var_value);
    } else {
        StopTiming(&latOverrideWithHOD_timing_flag);
        g_inter_media_msg.driver_hand_torque_st = var_value.flag_unset_val;
    }
#endif
}

void TorqueOverrideStJudgeWithoutHodDetection(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
#ifndef CONSUME_TIME
    static uint16   lat_override_withoutHOD_cnt = 0;
    static VarValue var_value                   = {OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY, 0};

    var_value.time_threshold_cnt = K_OverrideHandTorqCheckTime_Cnt;
    if (vehicle_signal->EPS_StrngWhlTorqVD && 
        (fabs(vehicle_signal->EPS_StrngWhlTorq) > K_OverrideHandTorqThreshold_LessTwoZone)) {
        FlagSetWithTimeCount(&g_inter_media_msg.driver_hand_torque_st, &lat_override_withoutHOD_cnt, &var_value);
    } else {
        g_inter_media_msg.driver_hand_torque_st = var_value.flag_unset_val;
        lat_override_withoutHOD_cnt               = 0;
    }
#else
    static sint64         latOverrideWithoutHOD_time        = 0;
    static VarValueInTime var_value                         = {OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY, 0.0};
    static uint8          latOverrideWithoutHOD_timing_flag = 0;

    var_value.time_threshold = K_OverrideHandTorqCheckTime;
    if (vehicle_signal->EPS_StrngWhlTorqVD && 
        (fabs(vehicle_signal->EPS_StrngWhlTorq) > K_OverrideHandTorqThreshold_LessTwoZone)) {
        if (!latOverrideWithoutHOD_timing_flag) {
            StartTiming(&latOverrideWithoutHOD_time, &latOverrideWithoutHOD_timing_flag);
        }
        FlagSetWithTime(&g_inter_media_msg.driver_hand_torque_st, latOverrideWithoutHOD_time, 
            latOverrideWithoutHOD_timing_flag, &var_value);
    } else {
        StopTiming(&latOverrideWithoutHOD_timing_flag);
        g_inter_media_msg.driver_hand_torque_st = var_value.flag_unset_val;
    }
#endif
}

void MonitorNdaStateTransition(const Dt_RECORD_Automaton_State* automaton_state)
{
    for (uint8 i = 0; i < (uint8)MONITOR_ARRAY_SIZE; ++i) {
        if (automaton_state->NDA_Function_State == nda_st_transit_monitor_array[i].start_st) {
            if ((g_inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag == 
                nda_st_transit_monitor_array[i].transit_enable_flag) && 
                (g_inter_media_msg.nda_st_transit_monitor.frame_cnt != MAX_FRAME_CNT)) {
                ++g_inter_media_msg.nda_st_transit_monitor.frame_cnt;
                return;
            }
        }
    }

    for (uint8 i = 0; i < (uint8)MONITOR_ARRAY_SIZE; ++i) {
        // 此时为通过可跳转条件去判断该给SOC的标志， 由于跳转是唯一且独立的，只要满足跳转条件就跳，然后直接退出循环
        if (automaton_state->NDA_Function_State == nda_st_transit_monitor_array[i].start_st) {
            if (nda_st_transit_monitor_array[i].nda_transit_cond()) {
                g_inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag = 
                    nda_st_transit_monitor_array[i].transit_enable_flag;
                g_inter_media_msg.nda_st_transit_monitor.frame_cnt = 1;
                return;
            }
        }
    }
    // 如果上述情况都不满足，说明不满足可跳转，那么跳转标志位为NONE
    g_inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag = NONE;
    g_inter_media_msg.nda_st_transit_monitor.frame_cnt               = 0;
}

void NdaStTransitNormalJudge(const Dt_RECORD_VehicleSignal2TSM* vehicle_signal, const Dt_RECORD_Soc_Info* soc_info)
{
    // 延时
    static uint8 time_cnt       = 0;
    static uint8 flag_cnt       = 0;
    static uint8 hazardlight_on = 0;
    if (time_cnt == 25) {
        if (flag_cnt) {
            hazardlight_on = 1;
        } else {
            hazardlight_on = 0;
        }
        flag_cnt = 0;
        time_cnt = 0;
    } else {
        ++time_cnt;
        if (vehicle_signal->BCM_LeftTurnLampSt && vehicle_signal->BCM_RightTurnLampSt) {
            ++flag_cnt;
        }
    }

    if (hazardlight_on) {
        g_inter_media_msg.automaton_transit_normal_flag = 0;
        // 0328版本暂用，此时顺便把系统故障置为TOR故障，后续删除
        g_inter_media_msg.mrm_system_fault_level = TOR_LEVEL3_FAULT;
        return;
    }
    g_inter_media_msg.automaton_transit_normal_flag = 1;
    // 0328版本暂用，后续删除
    g_inter_media_msg.mrm_system_fault_level = 0;

    // 这段逻辑保留，后期使用
    // for (uint8 i = 0; i < (uint8)MONITOR_ARRAY_SIZE; ++i) {
    //     if ((g_inter_media_msg.last_automaton_st.NDA_Function_State == nda_st_transit_monitor_array[i].start_st) &&
    //         (soc_info->Automaton_State.NDA_Function_State == nda_st_transit_monitor_array[i].next_st)) {
    //         if (g_inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag == NONE) {
    //             g_inter_media_msg.automaton_transit_normal_flag = 0;
    //             return;
    //         }
    //     }
    // }
    // g_inter_media_msg.automaton_transit_normal_flag = 1;
}

void BrakeInervationFlagJudge()
{
    static uint16 brake_intervation_cnt = 0;
    if (!g_inter_media_msg.brake_is_set) {
        g_inter_media_msg.brake_intervention_type = NO_BRAKE_INTERVENTION;
        brake_intervation_cnt                       = 0;
    } else {
        if (IsInMCUMRMActiveSt()) {
            if (g_inter_media_msg.brake_intervention_type == LONG_TERM_INTERVENTION) {
                g_inter_media_msg.brake_intervention_type = LONG_TERM_INTERVENTION;
            } else {
                if (brake_intervation_cnt >= K_BrakeTOR_TimeThreshold_Cnt) {
                    brake_intervation_cnt                       = 0;
                    g_inter_media_msg.brake_intervention_type = LONG_TERM_INTERVENTION;
                } else {
                    if ((brake_intervation_cnt < K_BrakeTOR_TimeThreshold_Cnt) &&
                        (brake_intervation_cnt > 0)) {
                        g_inter_media_msg.brake_intervention_type = SHORT_TERM_INTERVENTION;
                    } else {
                        brake_intervation_cnt                       = 0;
                        g_inter_media_msg.brake_intervention_type = NO_BRAKE_INTERVENTION;
                    }
                    ++brake_intervation_cnt;
                }
            }
        } else {
            brake_intervation_cnt                     = 0;
            g_inter_media_msg.brake_intervention_type = NO_BRAKE_INTERVENTION;
        }
    }
}

void FlagSetWithTimeCount(uint8* flag_set_var, uint16* time_cnt, const VarValue* var_value) 
{
    if (*flag_set_var == var_value->flag_set_val) {
        *flag_set_var = var_value->flag_set_val;
    } else {
        if (*time_cnt > var_value->time_threshold_cnt) {
            *flag_set_var = var_value->flag_set_val;
            *time_cnt     = 0;
        } else {
            *flag_set_var = var_value->flag_unset_val;
            ++(*time_cnt);
        }
    }
}

#ifdef CONSUME_TIME

void FlagSetWithTime(uint8* flag_set_var, const sint64 time, const uint8 time_flag, const VarValueInTime* var_value)
{
    if (*flag_set_var == var_value->flag_set_val) {
        *flag_set_var = var_value->flag_set_val;
    } else {
        if (GetTimeGapInSec(time, time_flag) > var_value->time_threshold) {
            *flag_set_var = var_value->flag_set_val;
        } else {
            *flag_set_var = var_value->flag_unset_val;
        }
    }
}

#endif


boolean IsInMCUMRMActiveSt()
{
    return ((g_tsm.state == MCU_MRM_TOR_LNG_LAT_CTRL) ||
            (g_tsm.state == MCU_MRM_TOR_LAT_CTRL) ||
            (g_tsm.state == MCU_MRM_TOR_STAND) ||
            (g_tsm.state == MCU_MRM_ACTIVE_LNG_LAT_CTRL) ||
            (g_tsm.state == MCU_MRM_ACTIVE_LAT_CTRL) ||
            (g_tsm.state == MCU_MRM_MRC));
}


