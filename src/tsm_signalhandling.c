#include "tsm_signalhandling.h"

void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
                    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    // 判断驾驶员注意力状态
    DrvrAttentionStJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 驾驶员是否踩下油门, 系统需求中无此项，针对3.17测试增加
    DriverGasPedalAppliedJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 纵向超越标志位
    // LngOverrideFlagJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 判断 刹车是否踩下
    BrakeIsSetJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    // 监控SOC侧跳转是否正常
    MonitorNdaStateTransition(&rtu_DeCANGATE2TSM->Soc_Info.Automaton_State);
    // 判断SOC侧跳转是否错误,
    // 0328，初始版本，先用harzard light去判断跳转是否正常，
    NdaStTransitNormalJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm, &rtu_DeCANGATE2TSM->Soc_Info);
#ifdef _NEED_LOG
    LOG("Lng_override_st: %d, Brake_is_set: %d", (uint8)tsm.inter_media_msg.lng_override_st, 
        tsm.inter_media_msg.brake_is_set);
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
    // 增加 驾驶员踩油门判断， 3.17测试新加
    if ((vehicle_signal->VCU_AccDriverOrvd == (uint8)DRIVER_OVERRIDE) ||
        (tsm.inter_media_msg.driver_acc_pedal_applied_flag)) {
        tsm.inter_media_msg.lng_override_st = OVERRIDE_SATISFY;
    } else {
        tsm.inter_media_msg.lng_override_st = OVERRIDE_NOT_SATISFY;
    }
}

void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    if (vehicle_signal->EBB_BrkPedalApplied == (uint8)BRAKE_PEDAL_APPLIED) {
        if (tsm.inter_media_msg.brake_is_set) {
            tsm.inter_media_msg.brake_is_set = 1;
        } else {
            if (tsm.timer_cnt.brakeset_cnt > K_BrakPedalAppliedThresholdTime_Cnt) {
                tsm.inter_media_msg.brake_is_set = 1;
                tsm.timer_cnt.brakeset_cnt = 0;
            } else {
                tsm.inter_media_msg.brake_is_set = 0;
                tsm.timer_cnt.brakeset_cnt++;
            }
        }
    } else {
        tsm.inter_media_msg.brake_is_set = 0;
        tsm.timer_cnt.brakeset_cnt = 0;
    }
}

void DriverGasPedalAppliedJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    if ((vehicle_signal->EMS_GasPedalActPstforMRRVD == 1) && 
        (vehicle_signal->EMS_GasPedalActPstforMRR > K_GasPedalPosThresholdValue)) {
        if (tsm.inter_media_msg.driver_acc_pedal_applied_flag) {
            tsm.inter_media_msg.driver_acc_pedal_applied_flag = 1;
        } else {
            if (tsm.timer_cnt.gasPedalPos_cnt > K_GasPedalAppliedThresholdTime_Cnt) {
                tsm.inter_media_msg.driver_acc_pedal_applied_flag = 1;
                tsm.timer_cnt.gasPedalPos_cnt = 0;
            } else {
                tsm.inter_media_msg.driver_acc_pedal_applied_flag = 0;
                tsm.timer_cnt.gasPedalPos_cnt++;
            }
        }
    } else {
        tsm.inter_media_msg.driver_acc_pedal_applied_flag = 0;
        tsm.timer_cnt.gasPedalPos_cnt = 0;
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
// 0328版使用hazard light 作为判断条件
    if ((vehicle_signal->BCM_LeftTurnLampSt && vehicle_signal->BCM_RightTurnLampSt) ||
        (vehicle_signal->BCM_HazardLampSt)) {
        tsm.inter_media_msg.automaton_transit_normal_flag = 0;
    }
    tsm.inter_media_msg.automaton_transit_normal_flag = 1;

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

boolean TransitCondFromStandbyToHandsFreeNormal() {
    return true;
}

boolean TransitCondFromStandbyToHandsOnNormal() {
    return false;
}

boolean TransitCondFromStandbyToHandsFreeStandActive() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToHandsFreeStandActive() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToBothOverride() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToLatOverride() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToLngOverride() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToHandsOnNormal() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToLngOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToBothOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToLatOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandWaitToLngOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandWaitToBothOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandWaitToLatOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToHandsOnStandActive() {
    return false;
}

boolean TransitCondFromHandsOnNormalToBothOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToLatOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToLngOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToHandsOnNormal() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToLngOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToBothOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToLatOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandWaitToLngOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandWaitToBothOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandWaitToLatOverride() {
    return false;
}

boolean TransitCondFromLngOverrideToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromLngOverrideToHandsOnNormal() {
    return false;
}

boolean TransitCondFromLngOverrideToBothOverride() {
    return false;
}

boolean TransitCondFromLngOverrideToLatOverride() {
    return false;
}

boolean TransitCondFromLatOverrideToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromLatOverrideToHandsOnNormal() {
    return false;
}

boolean TransitCondFromLatOverrideToBothOverride() {
    return false;
}

boolean TransitCondFromLatOverrideToLngOverride() {
    return false;
}

boolean TransitCondFromBothOverrideToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromBothOverrideToHandsOnNormal() {
    return false;
}

boolean TransitCondFromBothOverrideToLngOverride() {
    return false;
}

boolean TransitCondFromBothOverrideToLatOverride() {
    return false;
}

