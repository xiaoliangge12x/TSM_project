/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  MRM top state machine main structure implementation
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_chart.h"
#include "tsm_warning.h"
#include "tsm_output.h"

// -------------------------------  global variable definition ----------------------------
TSMParam      g_tsm;
// -------------------------------- driving table initilize -------------------------------
static const StateMachine g_top_state_machine = 
{
    .state_transit_table = 
    {
        {MCU_MRM_PASSIVE,             EVENT_STANDBY,              MCU_MRM_STANDBY},
        {MCU_MRM_PASSIVE,             EVENT_LIGHTING,             MCU_MRM_FAILURE_LIGHTING},
        {MCU_MRM_PASSIVE,             EVENT_NO_LIGHTING,          MCU_MRM_FAILURE_NO_LIGHTING},
        {MCU_MRM_FAILURE_LIGHTING,    EVENT_NO_LIGHTING,          MCU_MRM_FAILURE_NO_LIGHTING},
        {MCU_MRM_FAILURE_LIGHTING,    EVENT_FAULT_NOT_EXIST,      MCU_MRM_PASSIVE},
        {MCU_MRM_FAILURE_NO_LIGHTING, EVENT_LIGHTING,             MCU_MRM_FAILURE_LIGHTING},
        {MCU_MRM_FAILURE_NO_LIGHTING, EVENT_FAULT_NOT_EXIST,      MCU_MRM_PASSIVE},
        {MCU_MRM_STANDBY,             EVENT_LIGHTING,             MCU_MRM_FAILURE_LIGHTING},
        {MCU_MRM_STANDBY,             EVENT_NO_LIGHTING,          MCU_MRM_FAILURE_NO_LIGHTING},
        {MCU_MRM_STANDBY,             EVENT_MRM_BOTH_CTRL,        MCU_MRM_ACTIVE_LNG_LAT_CTRL},
        {MCU_MRM_STANDBY,             EVENT_MRM_LAT_CTRL,         MCU_MRM_ACTIVE_LAT_CTRL},
        {MCU_MRM_STANDBY,             EVENT_MRC,                  MCU_MRM_MRC},
        {MCU_MRM_STANDBY,             EVENT_TOR_BOTH_CTRL,        MCU_MRM_TOR_LNG_LAT_CTRL},
        {MCU_MRM_STANDBY,             EVENT_TOR_LAT_CTRL,         MCU_MRM_TOR_LAT_CTRL},
        {MCU_MRM_STANDBY,             EVENT_NO_STANDBY,           MCU_MRM_PASSIVE},
        {MCU_MRM_TOR_LNG_LAT_CTRL,    EVENT_TOR_LAT_CTRL_SWITCH,  MCU_MRM_TOR_LAT_CTRL},
        {MCU_MRM_TOR_LNG_LAT_CTRL,    EVENT_VEH_STANDSTILL,       MCU_MRM_TOR_STAND},
        {MCU_MRM_TOR_LNG_LAT_CTRL,    EVENT_TOR_TO_MRM_BOTH,      MCU_MRM_ACTIVE_LNG_LAT_CTRL},
        {MCU_MRM_TOR_LNG_LAT_CTRL,    EVENT_TOR_TO_MRM_LAT,       MCU_MRM_ACTIVE_LAT_CTRL},
        {MCU_MRM_TOR_LNG_LAT_CTRL,    EVENT_FUNCTION_EXIT,        MCU_MRM_PASSIVE},
        {MCU_MRM_TOR_LAT_CTRL,        EVENT_TOR_BOTH_CTRL_SWITCH, MCU_MRM_TOR_LNG_LAT_CTRL},
        {MCU_MRM_TOR_LAT_CTRL,        EVENT_VEH_STANDSTILL,       MCU_MRM_TOR_STAND},
        {MCU_MRM_TOR_LAT_CTRL,        EVENT_TOR_TO_MRM_BOTH,      MCU_MRM_ACTIVE_LNG_LAT_CTRL},
        {MCU_MRM_TOR_LAT_CTRL,        EVENT_TOR_TO_MRM_LAT,       MCU_MRM_ACTIVE_LAT_CTRL},
        {MCU_MRM_TOR_LAT_CTRL,        EVENT_FUNCTION_EXIT,        MCU_MRM_PASSIVE},
        {MCU_MRM_TOR_STAND,           EVENT_WAIT_EPB_RES,         MCU_MRM_PASSIVE},
        {MCU_MRM_ACTIVE_LNG_LAT_CTRL, EVENT_MRM_LAT_CTRL_SWITCH,  MCU_MRM_ACTIVE_LAT_CTRL},
        {MCU_MRM_ACTIVE_LNG_LAT_CTRL, EVENT_VEH_STANDSTILL,       MCU_MRM_MRC},
        {MCU_MRM_ACTIVE_LNG_LAT_CTRL, EVENT_FUNCTION_EXIT,        MCU_MRM_PASSIVE},
        {MCU_MRM_ACTIVE_LAT_CTRL,     EVENT_MRM_BOTH_CTRL_SWITCH, MCU_MRM_ACTIVE_LNG_LAT_CTRL},
        {MCU_MRM_ACTIVE_LAT_CTRL,     EVENT_VEH_STANDSTILL,       MCU_MRM_MRC},
        {MCU_MRM_ACTIVE_LAT_CTRL,     EVENT_FUNCTION_EXIT,        MCU_MRM_PASSIVE},
        {MCU_MRM_MRC,                 EVENT_FUNCTION_EXIT,        MCU_MRM_PASSIVE}
    },
    .event_table =
    {
        {EVENT_FAULT_NOT_EXIST,      IsMrmSystemFaultNotExist},
        {EVENT_LIGHTING,             IsLightingConditionMeet},
        {EVENT_NO_LIGHTING,          IsNoLightingConditionMeet},
        {EVENT_STANDBY,              IsStandbyConditionMeet},
        {EVENT_NO_STANDBY,           IsStandbyConditionNotMeet},
        {EVENT_MRM_BOTH_CTRL,        IsMrmBothCtrlConditionMeet},
        {EVENT_MRM_LAT_CTRL,         IsMrmLatCtrlConditionMeet},
        {EVENT_MRC,                  IsCanEnterMrcFromStandby},
        {EVENT_TOR_BOTH_CTRL,        IsTorBothCtrlCondMeet},
        {EVENT_TOR_LAT_CTRL,         IsTorLatCtrlCondMeet},
        {EVENT_VEH_STANDSTILL,       IsVehStandStillCondMeet},
        {EVENT_FUNCTION_EXIT,        IsFuncExitCondMeet},
        {EVENT_WAIT_EPB_RES,         IsWaitEpbSt},
        {EVENT_MRM_BOTH_CTRL_SWITCH, IsSwitchToMrmBothCtrl},
        {EVENT_MRM_LAT_CTRL_SWITCH,  IsSwitchToMrmLatCtrl},
        {EVENT_TOR_BOTH_CTRL_SWITCH, IsSwitchToTorBothCtrl},
        {EVENT_TOR_LAT_CTRL_SWITCH,  IsSwitchToTorLatCtrl},
        {EVENT_TOR_TO_MRM_BOTH,      IsTorToMrmBoth},
        {EVENT_TOR_TO_MRM_LAT,       IsTorToMrmLat},
    },
    .state_table = 
    {
        {MCU_MRM_PASSIVE,             ActionInPassive},
        {MCU_MRM_STANDBY,             ActionInStandby},
        {MCU_MRM_FAILURE_LIGHTING,    ActionInFailureLighting},
        {MCU_MRM_FAILURE_NO_LIGHTING, ActionInFailureNoLighting},
        {MCU_MRM_TOR_LNG_LAT_CTRL,    ActionInTorBothCtrl},
        {MCU_MRM_TOR_LAT_CTRL,        ActionInTorLatCtrl},
        {MCU_MRM_TOR_STAND,           ActionInTorStand},
        {MCU_MRM_ACTIVE_LNG_LAT_CTRL, ActionInMrmBothCtrl},
        {MCU_MRM_ACTIVE_LAT_CTRL,     ActionInMrmLatCtrl},
        {MCU_MRM_MRC,                 ActionInMrc}
    }, 
};

// -------------------------------- function  ---------------------------------------------
void MRM_TSM_MODULE_Init(void)
{
    memset(&g_tsm, 0, sizeof(TSMParam));
    memset(&g_inter_media_msg, 0, sizeof(InterMediaMsg));
    g_inter_media_msg.automaton_transit_normal_flag = 1;
    g_tsm.tsm_action_param.mrm_activation_st        = 1;
    g_tsm.state                                     = MCU_MRM_PASSIVE;
    g_warning_sm.warning_state                      = NO_WARNING;
}

void MRM_Swc_V_TSM(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, const Dt_RECORD_DecisionArbitrator2TSM *rtu_DeDecisionArbitrator2TSM, 
    const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm, Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag, Dt_RECORD_TSM2HMI *rty_DeTSM2HMI, Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE)
{
	// TODO:
    // validate timestamp
	(void)ValidateRcvMsgTimeStamp(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);

    // Preprocess msg
    SignalHandling(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);

    // run tsm user
    RunTsmUser(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);

    // run warning user
    RunWarningUser();

    // wrap package and send
    WrapAndSend(rtu_DeCtrlArb2TSM, rtu_DeDecisionArbitrator2TSM, rtu_DeCANGATE2TSM, 
        rtu_DeDiag2TSM, rtu_DePlanlite2Tsm, rty_DeTsm2Planlite, rty_DeTSM2CtrlArb,
        rty_DeTSM2DecisionArbitrator, rty_DeTSM2Diag, rty_DeTSM2HMI, rty_DeTSM2CANGATE);
    
    // 保存SOC状态
    memcpy(&g_inter_media_msg.last_automaton_st, &rtu_DeCANGATE2TSM->Soc_Info.Automaton_State,
        sizeof(Dt_RECORD_Automaton_State));

#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "request_mrm_from_mcu: %d", g_tsm.tsm_action_param.request_mrm);
#endif
}

void RunTsmUser(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    // Run Sit
    RunTsmSit(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);

    // Run tsm sm
    StateMachineWork(&g_top_state_machine, &g_tsm.state);
}

void RunTsmSit(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    
    // TODO:
    g_tsm_signal_bitfileds = 0;
    // TODO:
    g_tsm.tsm_action_param.request_mrm = 0;   // 就发一帧请求mrm, 避免误触发
    // TODO: delete mrm_system_fault_level judge
    if (rtu_DeDiag2TSM->Fault_Level == NO_FAULT) {
        if (g_inter_media_msg.mrm_system_fault_level == NO_FAULT) {
            SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_FAULT_NOT_EXIST);
        }
    } else {
        if (g_inter_media_msg.mrm_failure_lighting_flag) {
            SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_LIGHTING);
        } else {
            SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_NO_LIGHTING);
        }
    }

    // IsNDAInActiveSt(rtu_DeCANGATE2TSM->Soc_Info.Automaton_State.NDA_Function_State) ?
    //     SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_STANDBY) :
    //     SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_NO_STANDBY);
    SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_STANDBY);

    if (rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt == VEH_STANDSTILL_ST_STANDSTILL) {
        SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_VEH_STANDSTILL);
        if (ValidateActivationCond(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
            SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_MRC);
        }
    } else {
        SetCtrlType(BITNO_MRM_BOTH_CTRL_SWITCH, BITNO_MRM_LAT_CTRL_SWITCH);
        if (rtu_DePlanlite2Tsm->planningLite_control_state == PC_TOR) {
            SetCtrlType(BITNO_TOR_BOTH_CTRL_SWITCH, BITNO_TOR_LAT_CTRL_SWITCH);
            if (ValidateActivationCond(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
                SetCtrlType(BITNO_TOR_BOTH_CTRL, BITNO_TOR_LAT_CTRL);
            }
        } else if (rtu_DePlanlite2Tsm->planningLite_control_state == PC_MRM) {
            SetCtrlType(BITNO_TOR_TO_MRM_BOTH, BITNO_TOR_TO_MRM_LAT);
            if (ValidateActivationCond(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
                SetCtrlType(BITNO_MRM_BOTH_CTRL, BITNO_MRM_LAT_CTRL);
            }
        } else {
            // do nothing;
        }
    }

    if (g_warning_sm.warning_state == WARNING_MRM_LEVEL_4) {
        SetCtrlType(BITNO_TOR_TO_MRM_BOTH, BITNO_TOR_TO_MRM_LAT);
    }

    if (IsDriverTakeOver() || (rtu_DePlanlite2Tsm->planningLite_control_state == PC_EXIT)) {
        SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_FUNCTION_EXIT);
    } else if (rtu_DeDiag2TSM->Fault_Level != NO_FAULT) {
        g_tsm.tsm_action_param.request_mrm = 1;
        SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_FUNCTION_EXIT);
    }
}

boolean IsDriverTakeOver()
{
    if ((g_tsm.state == MCU_MRM_ACTIVE_LAT_CTRL) || (g_tsm.state == MCU_MRM_TOR_LAT_CTRL) || 
        (g_tsm.state == MCU_MRM_MRC)) {
        if (g_inter_media_msg.brake_intervention_type == LONG_TERM_INTERVENTION) {
            return true;
        }
        if ((g_inter_media_msg.driver_attention_st == AWAKE_AND_NOT_DISTRACTED) &&
            g_inter_media_msg.lng_override_long_duration_flag) {
            return true;
        }
        if ((g_inter_media_msg.driver_attention_st == AWAKE_AND_LOOK_REARVIEW_OR_HU) ||
            (g_inter_media_msg.driver_attention_st == AWAKE_AND_DISTRACTED) ||
            (g_inter_media_msg.driver_attention_st == FATIGUE_DRIVER_ATTENTION_ST)) {
            if (g_inter_media_msg.hands_can_takeover && g_inter_media_msg.lng_override_long_duration_flag) {
                return true;
            }
        }
    }

    if ((g_inter_media_msg.driver_attention_st == AWAKE_AND_NOT_DISTRACTED) &&
        (g_inter_media_msg.hands_can_takeover)) {
        return true;
    }

    if (g_inter_media_msg.driver_hand_torque_st == OVERRIDE_SATISFY) {
        return true;
    }
    return false;
}

boolean ValidateActivationCond(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM) 
{
    // TODO:
    // SOC状态机退出且TO3故障发还给MCU， 或者SOC安全停车时发生故障
    if (rtu_DeCANGATE2TSM->Soc_Info.Tor_Fault_From_SOC || rtu_DeCANGATE2TSM->Soc_Info.Request_Mrm_From_SOC) {
        return true;
    }

    // 和 SOC 发生通信故障
    if (rtu_DeDiag2TSM->Com_Fault_with_SOC) {
        return true;
    }

    // 监控SOC侧NDA激活状态跳转错误
    if (!g_inter_media_msg.automaton_transit_normal_flag) {
        return true;
    }
    // for debug
    if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL3_FAULT) {
        return true;
    }
    return false;
}

boolean IsNDAInActiveSt(const uint8 nda_st)
{
    return ((nda_st == NDA_ACTIVE_EPB_PHASE_IN) || (nda_st == NDA_ACTIVE_HANDS_FREE_NORMAL) ||
        (nda_st == NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE) || (nda_st == NDA_ACTIVE_HANDS_FREE_STAND_WAIT) ||
        (nda_st == NDA_ACTIVE_HANDS_ON_NORMAL) || (nda_st == NDA_ACTIVE_HANDS_ON_STAND_ACTIVE) ||
        (nda_st == NDA_ACTIVE_HANDS_ON_STAND_WAIT) || (nda_st == NDA_ACTIVE_EPB_PHASE_IN) ||
        (nda_st == NDA_LNG_OVERRIDE) || (nda_st == NDA_LNG_LAT_OVERRIDE) ||
        (nda_st == NDA_LAT_OVERRIDE) || (nda_st == NDA_TOR_LAT_CTRL) ||
        (nda_st == NDA_TOR_LNG_LAT_CTRL) || (nda_st == NDA_TOR_STAND) ||
        (nda_st == NDA_MRM_ACTIVE_PO_LAT_CTRL) || (nda_st == NDA_MRM_ACTIVE_PO_LNG_LAT_CTRL) ||
        (nda_st == NDA_MRM_ACTIVE_CP_LAT_CTRL) || (nda_st == NDA_MRM_ACTIVE_CP_LNG_LAT_CTRL) ||
        (nda_st == NDA_MRM_ACTIVE_ES_LAT_CTRL) || (nda_st == NDA_MRM_ACTIVE_ES_LNG_LAT_CTRL) ||
        (nda_st == NDA_MRC));
}

void SetCtrlType(const uint8 both_ctrl, const uint8 lat_ctrl)
{
    ((g_inter_media_msg.lng_override_st == OVERRIDE_SATISFY) || g_inter_media_msg.brake_is_set) ?
        SetSignalBitFields(&g_tsm_signal_bitfileds, lat_ctrl) :
        SetSignalBitFields(&g_tsm_signal_bitfileds, both_ctrl);
}

boolean ValidateRcvMsgTimeStamp(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    static Dt_RECORD_TimeStamp timestamp_table[TIMESTAMP_MAX_NUM][2] = {0};  // 1st column is cur, 2nd columm is last
    
    memcpy(&timestamp_table[0][0], &(rtu_DeCANGATE2TSM->TimeStamp),                sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[1][0], &(rtu_DeDiag2TSM->Diag_TimeStamp),              sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[2][0], &(rtu_DePlanlite2Tsm->DeTimeStamp), sizeof(Dt_RECORD_TimeStamp));

    for (uint8 i = 0; i < TIMESTAMP_MAX_NUM; ++i) {
        return (IsTimeStampError(&timestamp_table[i][0], &timestamp_table[i][1]) || 
            IsTimeStampLost(&timestamp_table[i][0], &timestamp_table[i][1]));
    }

    memcpy(&timestamp_table[0][1], &(rtu_DeCANGATE2TSM->TimeStamp),                sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[1][1], &(rtu_DeDiag2TSM->Diag_TimeStamp),              sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[2][1], &(rtu_DePlanlite2Tsm->DeTimeStamp), sizeof(Dt_RECORD_TimeStamp));
}

boolean IsTimeStampLost(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp)
{
    if (((float32)(cur_timestamp->Timestamp_nsec - last_timestamp->Timestamp_nsec) / NS_IN_MS) > UPPER_CYCLE ||
        ((float32)(cur_timestamp->Timestamp_nsec - last_timestamp->Timestamp_nsec) / NS_IN_MS) < LOWER_CYCLE) {
        return false;
    }
    return true;
}
boolean IsTimeStampError(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp)
{
    if (!cur_timestamp->Timestamp_valid || cur_timestamp->Timestamp_sec < last_timestamp->Timestamp_sec) {
        return false;
    }
    return true;
}