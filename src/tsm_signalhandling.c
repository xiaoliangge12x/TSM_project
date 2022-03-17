#include "tsm_signalhandling.h"

void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
                    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    DrvrAttentionStJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    DriverGasPedalAppliedJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    LngOverrideFlagJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
    BrakeIsSetJudge(&rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm);
#ifdef _NEED_LOG
    LOG("Lng_override_st: %d, Brake_is_set: %d", (uint8)tsm.inter_media_msg.lng_override_st, 
        tsm.inter_media_msg.brake_is_set);
#endif
}


// 判断驾驶员注意力状态
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

// 判断 纵向超越标志位
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

// 判断 刹车是否踩下
void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    if (vehicle_signal->EBB_BrkPedalApplied == (uint8)BRAKE_PEDAL_APPLIED) {
        if (tsm.inter_media_msg.brake_is_set) {
            tsm.inter_media_msg.brake_is_set = 1;
        } else {
            if (brakeset_cnt > K_BrakPedalAppliedThresholdTime_Cnt) {
                tsm.inter_media_msg.brake_is_set = 1;
                brakeset_cnt = 0;
            } else {
                tsm.inter_media_msg.brake_is_set = 0;
                brakeset_cnt++;
            }
        }
    } else {
        tsm.inter_media_msg.brake_is_set = 0;
        brakeset_cnt = 0;
    }
}

// 判断 驾驶员是否踩下油门, 系统需求中无此项，针对3.17测试增加
void DriverGasPedalAppliedJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal)
{
    if ((vehicle_signal->EMS_GasPedalActPstforMRRVD == 1) && 
        (vehicle_signal->EMS_GasPedalActPstforMRR > K_GasPedalPosThresholdValue)) {
        if (tsm.inter_media_msg.driver_acc_pedal_applied_flag) {
            tsm.inter_media_msg.driver_acc_pedal_applied_flag = 1;
        } else {
            if (gasPedalPos_cnt > K_GasPedalAppliedThresholdTime_Cnt) {
                tsm.inter_media_msg.driver_acc_pedal_applied_flag = 1;
                gasPedalPos_cnt = 0;
            } else {
                tsm.inter_media_msg.driver_acc_pedal_applied_flag = 0;
                gasPedalPos_cnt++;
            }
        }
    } else {
        tsm.inter_media_msg.driver_acc_pedal_applied_flag = 0;
        gasPedalPos_cnt = 0;
    }
}

// 判断SOC侧跳转是否正常


