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
#ifndef _RUN_IN_LINUX
#include "hb_TimeSync.h"
#endif

// -------------------------------  global variable definition ----------------------------
TSMParam      g_tsm;
static uint32 g_tsm_signal_bitfileds = 0U;
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

// -------------------------------- function definition   ---------------------------------
void MRM_TSM_MODULE_Init(void)
{
    memset(&g_tsm, 0, sizeof(TSMParam));
    memset(&g_inter_media_msg, 0, sizeof(InterMediaMsg));
    g_tsm.tsm_action_param.mrm_activation_st        = 1;
    g_tsm.state                                     = MCU_MRM_PASSIVE;
    g_warning_sm.warning_state                      = NO_WARNING;
}

void MRM_TSM_MODULE(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, const
  Dt_RECORD_DECIARB2TSM *rtu_DeDecisionArbitrator2TSM, const
  Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM
  *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm,
  Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite, Dt_RECORD_TSM2CtrlArb
  *rty_DeTSM2CtrlArb, Dt_RECORD_TSM2DecisionArbitrator
  *rty_DeTSM2DecisionArbitrator, Dt_RECORD_TSM2Diag *rty_DeTSM2Diag)
{
    // TODO:
    (void)ValidateRcvMsgTimeStamp(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);
    SignalHandling(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);
    RunTsmSit(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);
    RunWarningSit();
    StateMachineWork(&g_top_state_machine, &g_tsm.state);
    StateMachineWork(&g_warning_state_machine, &g_warning_sm.warning_state);
    WrapAndSend(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm, rty_DeTsm2Planlite, rty_DeTSM2CtrlArb,
        rty_DeTSM2DecisionArbitrator, rty_DeTSM2Diag);
    
#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "tsm state: %d, warning state: %d", g_tsm.state, g_warning_sm.warning_state);
#endif
}

void RunTsmSit(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    
    // TODO:
    g_tsm_signal_bitfileds = 0;
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
        if (IsInTorFault(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
            SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_MRC);
        }
    } else {
        SetCtrlType(BITNO_MRM_BOTH_CTRL_SWITCH, BITNO_MRM_LAT_CTRL_SWITCH);
        if (rtu_DePlanlite2Tsm->planningLite_control_state == PC_TOR) {
            SetCtrlType(BITNO_TOR_BOTH_CTRL_SWITCH, BITNO_TOR_LAT_CTRL_SWITCH);
            if (IsInTorFault(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
                SetCtrlType(BITNO_TOR_BOTH_CTRL, BITNO_TOR_LAT_CTRL);
            }
        } else if (rtu_DePlanlite2Tsm->planningLite_control_state == PC_MRM) {
            SetCtrlType(BITNO_TOR_TO_MRM_BOTH, BITNO_TOR_TO_MRM_LAT);
            if (IsInTorFault(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
                SetCtrlType(BITNO_MRM_BOTH_CTRL, BITNO_MRM_LAT_CTRL);
            }
        } else {
            // do nothing;
        }
    }

    if (g_warning_sm.warning_state == WARNING_MRM_LEVEL_4) {
        SetCtrlType(BITNO_TOR_TO_MRM_BOTH, BITNO_TOR_TO_MRM_LAT);
    }

    // TODO: MCU此处需要对Fault_Level的判断增加request_mrm的判断
#if ENABLE_INTERNAL_TEST
    if (g_inter_media_msg.mrm_system_fault_level == NO_FAULT) {
        SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_FUNCTION_EXIT);
    }
#else
    if (IsDriverTakeOver() || (rtu_DePlanlite2Tsm->planningLite_control_state == PC_EXIT) ||
        (rtu_DeDiag2TSM->Fault_Level != NO_FAULT) ) {
        SetSignalBitFields(&g_tsm_signal_bitfileds, BITNO_FUNCTION_EXIT);
    }
#endif
}

boolean IsMrmSystemFaultNotExist()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_FAULT_NOT_EXIST);
}

boolean IsLightingConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_LIGHTING);
}

boolean IsNoLightingConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_NO_LIGHTING);
}

boolean IsStandbyConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_STANDBY);
}

boolean IsStandbyConditionNotMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_NO_STANDBY);
}

boolean IsMrmBothCtrlConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_BOTH_CTRL);
}

boolean IsMrmLatCtrlConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_LAT_CTRL);
}

boolean IsCanEnterMrcFromStandby() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRC);
}

boolean IsTorBothCtrlCondMeet() 
{
#ifdef _NEED_LOG
    // LOG(COLOR_RED, "IsTorBothCtrlCondMeet: %d", IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_BOTH_CTRL));
#endif
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_BOTH_CTRL);
}

boolean IsTorLatCtrlCondMeet() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_LAT_CTRL);
}

boolean IsVehStandStillCondMeet() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_VEH_STANDSTILL);
}

boolean IsFuncExitCondMeet() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_FUNCTION_EXIT);
}

boolean IsWaitEpbSt()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_WAIT_EPB_RES);
}

boolean IsSwitchToMrmBothCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_BOTH_CTRL_SWITCH);
}

boolean IsSwitchToMrmLatCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_LAT_CTRL_SWITCH);
}

boolean IsSwitchToTorBothCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_BOTH_CTRL_SWITCH);
}

boolean IsSwitchToTorLatCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_LAT_CTRL_SWITCH);
}

boolean IsTorToMrmBoth()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_TO_MRM_BOTH);
}

boolean IsTorToMrmLat()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_TO_MRM_LAT);
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

boolean IsInTorFault(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM) 
{
#if ENABLE_INTERNAL_TEST
    // for debug
    if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL3_FAULT) {
        return true;
    }
#else
    // TODO:
    // ADC失效
    // if (IsBitSet((uint32)rtu_DeCANGATE2TSM->SOC_ADCFailureSt, BITNO_ADC_FAILURE) ||
    //     IsBitSet((uint32)rtu_DeCANGATE2TSM->MCU_ADCFailureSt, BITNO_ADC_FAILURE)) {
    //     return true;
    // }
    // // 和ADC通讯故障
    // if (rtu_DeDiag2TSM->Com_Fault_with_ADC) {
    //     return true;
    // }
    // // MCU失效，SOC安全停车时突然失效
    // if (IsBitSet((uint32)rtu_DeCANGATE2TSM->SOC_ADCFailureSt, BITNO_MCU_FAILURE)) {
    //     if (rtu_DeCANGATE2TSM->Tor_Fault_From_SOC || rtu_DeCANGATE2TSM->Request_Mrm_From_SOC) {
    //         return true;
    //     }
    // }
    // // SOC失效， MCU安全停车时突然失效
    // if (IsBitSet((uint32)rtu_DeCANGATE2TSM->MCU_ADCFailureSt, BITNO_SOC_FAILURE)) {
    //     if (rtu_DeCANGATE2TSM->Request_Mrm_From_MCU) {
    //         return true;
    //     }
    // }

    // for debug
    if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL3_FAULT) {
        return true;
    }
#endif
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

void ActionInPassive()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in MCU_MRM_PASSIVE St.");
#endif
    DoNoFunctionWork();
}

void ActionInFailureLighting()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Failure Lighting St.");
#endif
    DoNoFunctionWork();
}

void ActionInFailureNoLighting()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Failure No Lighting St.");
#endif
    DoNoFunctionWork();
}

void ActionInStandby()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Standby St.");
#endif
    DoNoFunctionWork();
}

void ActionInTorBothCtrl()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Tor Both Ctrl St.");
#endif
    OutputLatLngOverrideStatus(OVERRIDE_NOT_SATISFY, OVERRIDE_NOT_SATISFY);
    OutputMrmStatus(MRM_ST_TOR);
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
}

void ActionInTorLatCtrl()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Tor Lat Ctrl St.");
#endif
    OutputLatLngOverrideStatus(OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY);
    OutputMrmStatus(MRM_ST_TOR);
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
}

void ActionInTorStand()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Tor Stand St.");
#endif
    OutputLatLngOverrideStatus(OVERRIDE_NOT_SATISFY, OVERRIDE_NOT_SATISFY);
    OutputMrmStatus(MRM_ST_TOR);
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
}

void ActionInMrmBothCtrl()
{
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in Mrm Both Ctrl St.");
#endif
    OutputLatLngOverrideStatus(OVERRIDE_NOT_SATISFY, OVERRIDE_NOT_SATISFY);
#if ENABLE_INTERNAL_TEST
    OutputMrmStatus(MRM_ST_TOR);
#else
    OutputMrmStatus(MRM_ST_ACTIVE);
#endif
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
}

void ActionInMrmLatCtrl()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Mrm Lat Ctrl St.");
#endif
    OutputLatLngOverrideStatus(OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY);
#if ENABLE_INTERNAL_TEST
    OutputMrmStatus(MRM_ST_TOR);
#else
    OutputMrmStatus(MRM_ST_ACTIVE);
#endif
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
}

void ActionInMrc()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Mrc St.");
#endif
    OutputLatLngOverrideStatus(OVERRIDE_NOT_SATISFY, OVERRIDE_NOT_SATISFY);
    OutputMrmStatus(MRM_ST_INVALID);
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_SOC);
}

void DoNoFunctionWork()
{
    OutputLatLngOverrideStatus(OVERRIDE_NOT_SATISFY, OVERRIDE_NOT_SATISFY);
    OutputMrmStatus(MRM_ST_INVALID);
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_SOC);
}

void OutputLatLngOverrideStatus(const OverrideSt lng_override_st, const OverrideSt lat_override_st)
{
    g_tsm.tsm_action_param.lng_override_flag = lng_override_st;
    g_tsm.tsm_action_param.lat_override_flag = lat_override_st;
}

void OutputMrmStatus(const MrmStateToPlanning mrm_st)
{
    g_tsm.tsm_action_param.mrm_activation_st = mrm_st;
}

void OutputCtrlArbReq(const CtrlArbRequest ctrl_arb_req)
{
    g_tsm.tsm_action_param.control_arb_request = ctrl_arb_req;
}

boolean ValidateRcvMsgTimeStamp(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm)
{
    static Dt_RECORD_TimeStamp timestamp_table[TIMESTAMP_MAX_NUM][2] = {0};  // 1st column is cur, 2nd columm is last
    
    memcpy(&timestamp_table[0][0], &(rtu_DeCANGATE2TSM->TimeStamp),                sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[1][0], &(rtu_DeDiag2TSM->Diag_TimeStamp),              sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[2][0], &(rtu_DePlanlite2Tsm->Planning_Lite_TimeStamp), sizeof(Dt_RECORD_TimeStamp));

    for (uint8 i = 0; i < TIMESTAMP_MAX_NUM; ++i) {
        return (IsTimeStampError(&timestamp_table[i][0], &timestamp_table[i][1]) || 
            IsTimeStampLost(&timestamp_table[i][0], &timestamp_table[i][1]));
    }

    memcpy(&timestamp_table[0][1], &(rtu_DeCANGATE2TSM->TimeStamp),                sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[1][1], &(rtu_DeDiag2TSM->Diag_TimeStamp),              sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp_table[2][1], &(rtu_DePlanlite2Tsm->Planning_Lite_TimeStamp), sizeof(Dt_RECORD_TimeStamp));
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

static uint8 
set_lane_change_dir(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM) {
    enum lane_change_direction {
        NO_LANE_CHANGE,
        LANE_CHANGE_LEFT,
        LANE_CHANGE_RIGHT,
    };

#if ENABLE_INTERNAL_TEST
    if (rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCM_TurnLightSwitchSt == 1) {
        return LANE_CHANGE_LEFT;
    } else if (rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCM_TurnLightSwitchSt == 2) {
        return LANE_CHANGE_RIGHT;
    } else {
        return NO_LANE_CHANGE;
    }
#else
    if ((rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State == REQUEST_LANE_CHANGE_LEFT) ||
        (rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State == PREPARE_LANE_CHANGE_LEFT)) {
            return LANE_CHANGE_LEFT;
    } else if ((rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State == REQUEST_LANE_CHANGE_RIGHT) ||
        (rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State == PREPARE_LANE_CHANGE_RIGHT)) {
        return LANE_CHANGE_RIGHT;
    } else {
        return NO_LANE_CHANGE;
    }
#endif
}

void WrapAndSend(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm, Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag)
{
    static Dt_RECORD_TimeStamp tsm_timestamp  = {0};
    static sint64              time_in_ten_ns = 0;
    static uint32              TEN_NS_PER_S   = 100000000;
    static uint32              NS_PER_TEN_NS  = 10;
    tsm_timestamp.Timestamp_valid = 1;
    hb_TimeSync_GetTime(&time_in_ten_ns);   // 10ns per
    tsm_timestamp.Timestamp_nsec = (uint32)time_in_ten_ns * NS_PER_TEN_NS;
    tsm_timestamp.Timestamp_sec  = (uint32)time_in_ten_ns / TEN_NS_PER_S;

    //------------------------------------------- 给到planlite相关 -----------------------------------------
    memcpy(&rty_DeTsm2Planlite->TimeStamp, &rtu_DeCANGATE2TSM->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO: 状态机提供
    memcpy(&rty_DeTsm2Planlite->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    rty_DeTsm2Planlite->NDA_Lane_Change_Type = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_Type;
    rty_DeTsm2Planlite->NDA_Lane_Change_Direction = set_lane_change_dir(rtu_DeCANGATE2TSM);
    rty_DeTsm2Planlite->NDA_Lane_Change_State = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State;
    rty_DeTsm2Planlite->MRM_Status = g_tsm.tsm_action_param.mrm_activation_st;

    //------------------------------------------- 给到CtrlArb相关  ----------------------------------------
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_CanGate, &rtu_DeCANGATE2TSM->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_PlannLite, &rtu_DePlanlite2Tsm->Planning_Lite_TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    // TODO: 状态机提供
    memcpy(&rty_DeTSM2CtrlArb->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    rty_DeTSM2CtrlArb->holo_planning_control_status = rtu_DePlanlite2Tsm->planningLite_control_state;
    memcpy(&rty_DeTSM2CtrlArb->Automaton_State, &rtu_DeCANGATE2TSM->Soc_Info.Automaton_State,
        sizeof(Dt_RECORD_Automaton_State));
    rty_DeTSM2CtrlArb->lng_override_flag   = g_tsm.tsm_action_param.lng_override_flag;
    rty_DeTSM2CtrlArb->lat_override_flag   = g_tsm.tsm_action_param.lat_override_flag;
    rty_DeTSM2CtrlArb->control_arb_request = g_tsm.tsm_action_param.control_arb_request;

    //------------------------------------------- 给到DeciArb相关  ----------------------------------------
    memcpy(&rty_DeTSM2DecisionArbitrator->TimeStamp, &rtu_DeCANGATE2TSM->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2DecisionArbitrator->DeScenarioType, &rtu_DeCANGATE2TSM->Soc_Info, sizeof(Dt_RECORD_ScenarioType));

    //------------------------------------------- 给到Diag诊断相关  ----------------------------------------
    // TODO: 状态机提供
    memcpy(&rty_DeTSM2Diag->Tsm_TimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO: 状态机提供， TSM故障状态
    rty_DeTSM2Diag->Tsm_Status = 0;
    
    // // -------- 给到HMI相关 --------
    // // to do: 状态机提供
    // memset(&rty_DeTSM2HMI->Tsm_TimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // // to do: 状态机提供
    // rty_DeTSM2HMI->Fault_Info = 0;     // 故障信息
    // // to do: 状态机提供
    // rty_DeTSM2HMI->Tor_Request = 0;    // tor请求

    // // -------- 给到CANgate相关 -------
    // // to do: 状态机提供
    // memset(&rty_DeTSM2CANGATE->Tsm_TimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // // to do: 缺少输入
    // memset(&rty_DeTSM2CANGATE->Decision_Arbitrator_TimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // // memcpy(&rty_DeTSM2CANGATE->Decision_Arbitrator_TimeStamp, &rtu_DeDecisionArbitrator2TSM->Decision_Arbitrator_TimeStamp,
    // //     sizeof(Dt_RECORD_TimeStamp));
    // // to do: 缺少输入
    // memset(&rty_DeTSM2CANGATE->Control_Arbitrator_TimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // // memcpy(&rty_DeTSM2CANGATE->Control_Arbitrator_TimeStamp, &rtu_DeCtrlArb2TSM->timestamp,
    // //     sizeof(Dt_RECORD_TimeStamp));
    // // to do: 状态机判断提供
    // rty_DeTSM2CANGATE->EPB_Request = 0;
    // // to do: 状态机判断提供
    // rty_DeTSM2CANGATE->Hazard_Light_Request = 0;
    // // to do: 状态机判断提供
    // rty_DeTSM2CANGATE->Ecall_Request = 0;
    // // to do: 状态机判断提供
    // rty_DeTSM2CANGATE->Door_Unlock_Request = 0;
    // // rty_DeTSM2CANGATE->Tsm_To_Soc.AS_Status = rtu_DeCtrlArb2TSM->AS_Status;
    // // memcpy(&rty_DeTSM2CANGATE->Tsm_To_Soc.Control_Arbitrator_Results, &rtu_DeCtrlArb2TSM->Control_Arbitrator_Results, 
    // //     sizeof(Dt_RECORD_Control_Arbitrator_Results));
    // // rty_DeTSM2CANGATE->Tsm_To_Soc.Lane_Change_Allow_Flag = rtu_DeDecisionArbitrator2TSM->Lane_Change_Allow_Flag;
    // // rty_DeTSM2CANGATE->Tsm_To_Soc.Parking_EPS_handshake_state = rtu_DeCtrlArb2TSM->Parking_EPS_handshake_state;
    // // to do: 状态机提供状态跳转监控标志位
    // rty_DeTSM2CANGATE->Tsm_To_Soc.AutomatonTransitMonitorFlag.Nda_St_Transition_Monitor_Flag = 
    //     g_inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag;
    // // to do: 状态机提供
    // memset(&rty_DeTSM2CANGATE->Mcu_To_Ifc.time_stamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // // to do:
    // rty_DeTSM2CANGATE->Mcu_To_Ifc.MCU_MRM_status = 0;  // MRM状态, 表示MCU安全停车是否可用,以供IFC判断是否启动安全停车 -- 功能故障,和明江讨论下
}