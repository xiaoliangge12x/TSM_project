/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Signal Preprocess for TSM implementation
 *  \author zengxiaoliang (zengxiaoliang@holomatic.com)
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */


#include "tsm_signalhandling.h"

void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
                    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    // 判断驾驶员注意力状态
    // DrvrAttentionStJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 驾驶员是否踩下油门, 系统需求中无此项，针对3.17测试增加
    DriverGasPedalAppliedJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 纵向超越标志位
    // LngOverrideFlagJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 刹车是否踩下
    BrakeIsSetJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 刹车长时介入标志位
    BrakeInervationFlagJudge();
    // 判断 驾驶员手力矩超越标志位
    DriveHandTorqueOverrideStJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 监控 SOC侧NDA的状态跳转
    MonitorNdaStateTransition(&rtu_DeCANGATE2TSM->Soc_Info.Automaton_State);
    // 判断 SOC侧NDA的跳转是否错误
    // 0328，初始版本，先用harzard light去判断跳转是否正常，
    NdaStTransitNormalJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm, &rtu_DeCANGATE2TSM->Soc_Info);
#ifdef _NEED_LOG
    LOG("EMS_GasPedalActPstforMRRVD: %d, EMS_GasPedalActPstforMRR: %f", 
        rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.EMS_GasPedalActPstforMRRVD, 
        rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.EMS_GasPedalActPstforMRR);
#endif
}

void DrvrAttentionStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    // 判断 清醒且未分神
    if (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)ALERT) {
        if ((vehicle_signal->DMS_DrvrDetSts == (uint8)EYE_DETECTED) ||
            (vehicle_signal->DMS_DrvrDetSts == (uint8)HEAD_DETECTED)) {
            if (vehicle_signal->DMS_InterestsRegion == (uint8)WINDSHIELD) {
                tsm.inter_media_msg.driver_attention_st = AWAKE_AND_NOT_DISTRACTED;
                return;
            }
            // 判断 清醒且注视后视镜||车机
            if ((vehicle_signal->DMS_InterestsRegion == (uint8)REAR_VIEW_MIRROR) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INSTRUMENT_CLUSTER) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)INFOTAIMENT_DISPLAY)) {
                tsm.inter_media_msg.driver_attention_st = AWAKE_AND_LOOK_REARVIEW_OR_HU;
                return;
            }
        }
        // 判断 清醒且分神
        if (vehicle_signal->DMS_DrvrDetSts == (uint8)NOT_DETECTED) {
            if ((vehicle_signal->DMS_InterestsRegion == (uint8)UNAVAILABLE_DMS_INTERESTS_REGION) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)DRIVER_SIDE_WINDOW) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)PASSENGER_SIDE_WINDOW) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)DRIVER_SIDE_DOOR_PANEL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)DRIVER_SIDE_FOOTWELL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)PASSENGER_SIDE_DOOR_PANEL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)PASSENGER_SIDE_FOOTWELL) ||
                (vehicle_signal->DMS_InterestsRegion == (uint8)OTHER_AREA)) {
                tsm.inter_media_msg.driver_attention_st = AWAKE_AND_DISTRACTED;
                return;
            }
        }
    }

    // 判断 疲劳
    if ((vehicle_signal->DMS_L3DriverFatigueSt == (uint16)FATIGUE_DMS_DRIVER_FATIGUE_ST) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)HIGHLY_FATIGUE) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)UNKNOWN_DMS_DRIVER_FATIGUE_ST) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)SUSPECTED_UNRESPONSIVE) ||
        (vehicle_signal->DMS_L3DriverFatigueSt == (uint16)CONFIRMED_UNRESPONSIVE)) {
        tsm.inter_media_msg.driver_attention_st = FATIGUE_DRIVER_ATTENTION_ST;
        return;
    }
    
    tsm.inter_media_msg.driver_attention_st = UNKNOWN;
}

void LngOverrideFlagJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    static uint16 lng_override_cnt = 0;
    static VarValue var_value = {1, 0, 0};

    var_value.time_threshold_cnt = K_LngOverrideTakeOverTime_Cnt;

#ifdef _NEED_LOG
    LOG("lng_override_cnt: %d", lng_override_cnt);
#endif

    // to do: 未加变道状态
    // 增加 驾驶员踩油门判断， 3.17测试新加
    if ((vehicle_signal->VCU_AccDriverOrvd == (uint8)DRIVER_OVERRIDE) ||
        (tsm.inter_media_msg.driver_acc_pedal_applied_flag)) {
        tsm.inter_media_msg.lng_override_st = OVERRIDE_SATISFY;
        FlagSetWithTimeCount(&tsm.inter_media_msg.lng_override_long_duration_flag, &lng_override_cnt, &var_value);
    } else {
        tsm.inter_media_msg.lng_override_st = OVERRIDE_NOT_SATISFY;
        lng_override_cnt = 0;
        tsm.inter_media_msg.lng_override_long_duration_flag = var_value.flag_unset_val;
    }
}

void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    static uint16 brakeset_cnt = 0;
    static VarValue var_value = {1, 0, 0};

    var_value.time_threshold_cnt = K_BrakPedalAppliedThresholdTime_Cnt;

    if ((vehicle_signal->EBB_BrkPedalAppliedSt == NORMAL) && 
        (vehicle_signal->EBB_BrkPedalApplied == BRAKE_PEDAL_APPLIED)) {
        FlagSetWithTimeCount(&tsm.inter_media_msg.brake_is_set, &brakeset_cnt, &var_value);
    } else {
        tsm.inter_media_msg.brake_is_set = var_value.flag_unset_val;
        brakeset_cnt = 0;
    }
}

void DriverGasPedalAppliedJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    static uint16 gasPedalPos_cnt = 0;
    static VarValue var_value = {1, 0, 0};   // set flag, unset flag

    var_value.time_threshold_cnt = K_GasPedalAppliedThresholdTime_Cnt;
#ifdef _NEED_LOG
    LOG("gasPedalPos_cnt: %d", gasPedalPos_cnt);
#endif
    if (vehicle_signal->EMS_GasPedalActPstforMRRVD && 
        (vehicle_signal->EMS_GasPedalActPstforMRR > K_GasPedalPosThresholdValue)) {
        FlagSetWithTimeCount(&tsm.inter_media_msg.driver_acc_pedal_applied_flag, &gasPedalPos_cnt, &var_value);
    } else {
        tsm.inter_media_msg.driver_acc_pedal_applied_flag = var_value.flag_unset_val;
        gasPedalPos_cnt = 0;
    }
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
    static float32 overrideHandTorqThreshold = 0;
    static uint16 lat_override_withHOD_cnt = 0;
    static VarValue var_value = {OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY, 0};

    if (vehicle_signal->HOD_TouchZone1 && vehicle_signal->HOD_TouchZone2 && vehicle_signal->HOD_TouchZone3) {
        overrideHandTorqThreshold = K_OverrideHandTorqThreshold_ThreeZone;
    } else if ((vehicle_signal->HOD_TouchZone1 && vehicle_signal->HOD_TouchZone1) || 
               (vehicle_signal->HOD_TouchZone2 && vehicle_signal->HOD_TouchZone3) || 
               (vehicle_signal->HOD_TouchZone1 && vehicle_signal->HOD_TouchZone3)) {
        overrideHandTorqThreshold = K_OverrideHandTorqThreshold_TwoZone;
    } else {
        overrideHandTorqThreshold = K_OverrideHandTorqThreshold_LessTwoZone;   // 1区检测或无区检测
    }

    var_value.time_threshold_cnt = (uint16)K_OverrideHandTorqCheckTime_Cnt;
#ifdef _NEED_LOG
    LOG("lat_override_withHOD_cnt: %d", lat_override_withHOD_cnt);
#endif

    if (vehicle_signal->EPS_StrngWhlTorqVD && (fabs(vehicle_signal->EPS_StrngWhlTorq) > overrideHandTorqThreshold)) {
        FlagSetWithTimeCount(&tsm.inter_media_msg.driver_hand_torque_st, &lat_override_withHOD_cnt, &var_value);
    } else {
        tsm.inter_media_msg.driver_hand_torque_st = var_value.flag_unset_val;
        lat_override_withHOD_cnt = 0;
    }
}

void TorqueOverrideStJudgeWithoutHodDetection(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    static uint16 lat_override_withoutHOD_cnt = 0;
    static VarValue var_value = {OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY, 0};

    var_value.time_threshold_cnt = (uint16)K_OverrideHandTorqCheckTime_Cnt;

#ifdef _NEED_LOG
    LOG("lat_override_withoutHOD_cnt: %d", lat_override_withoutHOD_cnt);
#endif

    if (vehicle_signal->EPS_StrngWhlTorqVD && (fabs(vehicle_signal->EPS_StrngWhlTorq) > K_OverrideHandTorqThreshold_LessTwoZone)) {
        FlagSetWithTimeCount(&tsm.inter_media_msg.driver_hand_torque_st, &lat_override_withoutHOD_cnt, &var_value);
    } else {
        tsm.inter_media_msg.driver_hand_torque_st = var_value.flag_unset_val;
        lat_override_withoutHOD_cnt = 0;
    }
}

void MonitorNdaStateTransition(const Dt_RECORD_Automaton_State* automaton_state)
{
    // 此时在发5帧阶段，不用再去判断可跳转条件， 若5帧已发出，状态仍未跳转，则重新判断并刷新计数
    for (uint8 i = 0; i < (uint8)MONITOR_ARRAY_SIZE; ++i) {
        if (automaton_state->NDA_Function_State == nda_st_transit_monitor_array[i].start_st) {
            if ((tsm.inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag == 
                    nda_st_transit_monitor_array[i].transit_enable_flag) && 
                (tsm.inter_media_msg.nda_st_transit_monitor.frame_cnt != MAX_FRAME_CNT)) {
                ++tsm.inter_media_msg.nda_st_transit_monitor.frame_cnt;
                return;
            }
        }
    }

    for (uint8 i = 0; i < (uint8)MONITOR_ARRAY_SIZE; ++i) {
        // 此时为通过可跳转条件去判断该给SOC的标志， 由于跳转是唯一且独立的，只要满足跳转条件就跳，然后直接退出循环
        if (automaton_state->NDA_Function_State == nda_st_transit_monitor_array[i].start_st) {
            if (nda_st_transit_monitor_array[i].nda_transit_cond()) {
                tsm.inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag = 
                    nda_st_transit_monitor_array[i].transit_enable_flag;
                tsm.inter_media_msg.nda_st_transit_monitor.frame_cnt = 1;
                return;
            }
        }
    }
    // 如果上述情况都不满足，说明不满足可跳转，那么跳转标志位为NONE
    tsm.inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag = NONE;
    tsm.inter_media_msg.nda_st_transit_monitor.frame_cnt = 0;
}

void NdaStTransitNormalJudge(const Dt_RECORD_VehicleSignal2TSM* vehicle_signal, const Dt_RECORD_Soc_Info* soc_info)
{
// 增加延时判断， 判断200ms内是不是有flag置起, 下一个200ms到来时，会清0
    static uint8 time_cnt = 0;
    static uint8 flag_cnt = 0;
    static uint8 hazardlight_on = 0;
    if (time_cnt == 25) {
        // hazardlight_on = 0;
        if (flag_cnt) {
            hazardlight_on = 1;
        } else {
            hazardlight_on = 0;
        }
        flag_cnt = 0;
        time_cnt = 0;
    } else {
        time_cnt++;
        if (vehicle_signal->BCM_LeftTurnLampSt && vehicle_signal->BCM_RightTurnLampSt) {
            flag_cnt++;
        }
    }

// 0328版使用hazard light 作为判断条件
    /*if ((vehicle_signal->BCM_LeftTurnLampSt && vehicle_signal->BCM_RightTurnLampSt) ||
        (vehicle_signal->BCM_HazardLampSt)) */ 
    if (hazardlight_on) {
        tsm.inter_media_msg.automaton_transit_normal_flag = 0;
        // 0328版本暂用，此时顺便把系统故障置为1，后续删除
        tsm.inter_media_msg.mrm_system_fault_level = 1;
        return;
    }
    tsm.inter_media_msg.automaton_transit_normal_flag = 1;
    // 0328版本暂用，后续删除
    tsm.inter_media_msg.mrm_system_fault_level = 0;

// 这段逻辑保留，后期使用
    // for (uint8 i = 0; i < (uint8)MONITOR_ARRAY_SIZE; ++i) {
    //     if ((tsm.inter_media_msg.last_automaton_st.NDA_Function_State == nda_st_transit_monitor_array[i].start_st) &&
    //         (soc_info->Automaton_State.NDA_Function_State == nda_st_transit_monitor_array[i].next_st)) {
    //         if (tsm.inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag == NONE) {
    //             tsm.inter_media_msg.automaton_transit_normal_flag = 0;
    //             return;
    //         }
    //     }
    // }
    // tsm.inter_media_msg.automaton_transit_normal_flag = 1;
}

void BrakeInervationFlagJudge()
{
    static uint16 brake_intervation_cnt = 0;
    // 无制动介入
    if (!tsm.inter_media_msg.brake_is_set) {
        tsm.inter_media_msg.brake_intervention_type = NO_BRAKE_INTERVENTION;
        brake_intervation_cnt = 0;
    } else {
        // 制动有介入
        if ((tsm.state == MRM_LAT_CTRL) || (tsm.state == MRM_LNG_LAT_CTRL) || (tsm.state == MRC)) {
            if (tsm.inter_media_msg.brake_intervention_type == LONG_TERM_INTERVENTION) {
                tsm.inter_media_msg.brake_intervention_type = LONG_TERM_INTERVENTION;
            } else {
                if (brake_intervation_cnt >= K_BrakeTOR_TimeThreshold_Cnt) {
                    brake_intervation_cnt = 0;
                    tsm.inter_media_msg.brake_intervention_type = LONG_TERM_INTERVENTION;
                } else {
                    if ((brake_intervation_cnt < K_BrakeTOR_TimeThreshold_Cnt) &&
                        (brake_intervation_cnt > 0)) {
                        tsm.inter_media_msg.brake_intervention_type = SHORT_TERM_INTERVENTION;
                    } else {
                        brake_intervation_cnt = 0;
                        tsm.inter_media_msg.brake_intervention_type = NO_BRAKE_INTERVENTION;
                    }
                    brake_intervation_cnt++;
                }
            }
        } else {
            brake_intervation_cnt = 0;
            tsm.inter_media_msg.brake_intervention_type = NO_BRAKE_INTERVENTION;
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
            *time_cnt = 0;
        } else {
            *flag_set_var = var_value->flag_unset_val;
            (*time_cnt)++;
        }
    }
}

