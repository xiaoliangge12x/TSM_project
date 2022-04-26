/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  tsm output
 *  \author zxl
 *  \date   2022-04-26
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_output.h"

// ------------------------------- function   -------------------------------------
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
    OutputMrmStatus(MRM_ST_ACTIVE);
    OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
}

void ActionInMrmLatCtrl()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in Mrm Lat Ctrl St.");
#endif
    OutputLatLngOverrideStatus(OVERRIDE_SATISFY, OVERRIDE_NOT_SATISFY);
    OutputMrmStatus(MRM_ST_ACTIVE);
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

void WrapAndSend(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, const Dt_RECORD_DecisionArbitrator2TSM *rtu_DeDecisionArbitrator2TSM, 
    const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm, Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag, Dt_RECORD_TSM2HMI *rty_DeTSM2HMI, Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE)
{
	static Dt_RECORD_TimeStamp tsm_timestamp  = {0};
#ifdef CONSUME_TIME
    static sint64              time_in_ten_ns = 0;
    static uint32              TEN_NS_PER_S   = 100000000;
    static uint32              NS_PER_TEN_NS  = 10;
    tsm_timestamp.Timestamp_valid = 1;
    hb_TimeSync_GetTime(&time_in_ten_ns);   // 10ns per
    tsm_timestamp.Timestamp_nsec = (uint32)time_in_ten_ns * NS_PER_TEN_NS;
    tsm_timestamp.Timestamp_sec  = (uint32)time_in_ten_ns / TEN_NS_PER_S;
#endif
    
    // ---------------------------------------- planlite ----------------------------------------------
    memcpy(&rty_DeTsm2Planlite->TimeStamp, &rtu_DeCANGATE2TSM->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO: 
    memcpy(&rty_DeTsm2Planlite->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    rty_DeTsm2Planlite->NDA_Lane_Change_Type = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_Type;
    rty_DeTsm2Planlite->NDA_Lane_Change_Direction = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_Direction;
    rty_DeTsm2Planlite->NDA_Lane_Change_State = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State;
    // TODO: 
    rty_DeTsm2Planlite->MRM_Status = g_tsm.tsm_action_param.mrm_activation_st;

    // ----------------------------------------- CtrlArb ----------------------------------------------
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_CanGate, &rtu_DeCANGATE2TSM->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_PlannLite, &rtu_DePlanlite2Tsm->DeTimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    memcpy(&rty_DeTSM2CtrlArb->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    rty_DeTSM2CtrlArb->holo_planning_control_status = rtu_DePlanlite2Tsm->planningLite_control_state;
    // TODO:
    memcpy(&rty_DeTSM2CtrlArb->Automaton_State, &rtu_DeCANGATE2TSM->Soc_Info.Automaton_State,
        sizeof(Dt_RECORD_Automaton_State));
    rty_DeTSM2CtrlArb->long_override_flag = g_tsm.tsm_action_param.lng_override_flag;
    // TODO:
    rty_DeTSM2CtrlArb->NDA_LatState = 0;
    rty_DeTSM2CtrlArb->NDA_LongState = 0;
    rty_DeTSM2CtrlArb->NDA_ILC_State = 0;
	rty_DeTSM2CtrlArb->control_arb_request = g_tsm.tsm_action_param.control_arb_request;


    // ----------------------------------------- DeciArb -----------------------------------------------
    memcpy(&rty_DeTSM2DecisionArbitrator->TimeStamp, &rtu_DeCANGATE2TSM->TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2DecisionArbitrator->DeScenarioType, &rtu_DeCANGATE2TSM->Soc_Info,
        sizeof(Dt_RECORD_ScenarioType));

    // ------- 给到Diag诊断相关 --------
    // TODO:
    memcpy(&rty_DeTSM2Diag->Tsm_TimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    rty_DeTSM2Diag->Tsm_Status = 0;
    
    // ------------------------------------------ HMI -------------------------------------------------
    // TODO:
    memcpy(&rty_DeTSM2HMI->Tsm_TimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    rty_DeTSM2HMI->Fault_Info = 0;  
    // TODO:
    rty_DeTSM2HMI->Tor_Request = 0;

    // ------------------------------------------ CANgate ----------------------------------------------
    // TODO:
    memcpy(&rty_DeTSM2CANGATE->Tsm_TimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CANGATE->Decision_Arbitrator_TimeStamp, &rtu_DeDecisionArbitrator2TSM->Decision_Arbitrator_TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CANGATE->Control_Arbitrator_TimeStamp, &rtu_DeCtrlArb2TSM->timestamp,
        sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    rty_DeTSM2CANGATE->EPB_Request = 0;
    // TODO:
    rty_DeTSM2CANGATE->Hazard_Light_Request = 0;
    // TODO:
    rty_DeTSM2CANGATE->Ecall_Request = 0;
    // TODO:
    rty_DeTSM2CANGATE->Door_Unlock_Request = 0;
    rty_DeTSM2CANGATE->Tsm_To_Soc.AS_Status = rtu_DeCtrlArb2TSM->AS_Status;
    memcpy(&rty_DeTSM2CANGATE->Tsm_To_Soc.Control_Arbitrator_Results, &rtu_DeCtrlArb2TSM->Control_Arbitrator_Results, 
        sizeof(Dt_RECORD_Control_Arbitrator_Results));
    rty_DeTSM2CANGATE->Tsm_To_Soc.Lane_Change_Allow_Flag = rtu_DeDecisionArbitrator2TSM->Lane_Change_Allow_Flag;
    rty_DeTSM2CANGATE->Tsm_To_Soc.Parking_EPS_handshake_state = rtu_DeCtrlArb2TSM->Parking_EPS_handshake_state;
    // TODO:
    rty_DeTSM2CANGATE->Tsm_To_Soc.AutomatonTransitMonitorFlag.Standby_HandsFree_St_Monitor_Flag = 
        g_inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag;;
    rty_DeTSM2CANGATE->Tsm_To_Soc.AutomatonTransitMonitorFlag.HandsOn_St_Monitor_Flag = 0;
    rty_DeTSM2CANGATE->Tsm_To_Soc.AutomatonTransitMonitorFlag.Override_St_Monitor_Flag = 0;
    // TODO:
    memcpy(&rty_DeTSM2CANGATE->Mcu_To_Ifc.time_stamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    rty_DeTSM2CANGATE->Mcu_To_Ifc.MCU_MRM_status = 0; 
}