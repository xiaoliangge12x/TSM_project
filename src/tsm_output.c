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

void WrapAndSend()
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
    memcpy(&g_tsm_output.p_planlite->TimeStamp, &g_tsm_input.p_can_gate->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO: 
    memcpy(&g_tsm_output.p_planlite->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    g_tsm_output.p_planlite->NDA_Lane_Change_Type = g_tsm_input.p_can_gate->Soc_Info.scenario_type.NDA_Lane_Change_Type;
    g_tsm_output.p_planlite->NDA_Lane_Change_Direction = g_tsm_input.p_can_gate->Soc_Info.scenario_type.NDA_Lane_Change_Direction;
    g_tsm_output.p_planlite->NDA_Lane_Change_State = g_tsm_input.p_can_gate->Soc_Info.scenario_type.NDA_Lane_Change_State;
    // TODO: 
    g_tsm_output.p_planlite->MRM_Status = g_tsm.tsm_action_param.mrm_activation_st;

    // ----------------------------------------- CtrlArb ----------------------------------------------
    memcpy(&g_tsm_output.p_ctrl_arb->TimeStamp_CanGate, &g_tsm_input.p_can_gate->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&g_tsm_output.p_ctrl_arb->TimeStamp_PlannLite, &g_tsm_input.p_planlite->DeTimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    memcpy(&g_tsm_output.p_ctrl_arb->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    g_tsm_output.p_ctrl_arb->holo_planning_control_status = g_tsm_input.p_planlite->planningLite_control_state;
    // TODO:
    memcpy(&g_tsm_output.p_ctrl_arb->Automaton_State, &g_tsm_input.p_can_gate->Soc_Info.automaton_state,
        sizeof(Soc_State));
    g_tsm_output.p_ctrl_arb->long_override_flag = g_tsm.tsm_action_param.lng_override_flag;
    g_tsm_output.p_ctrl_arb->lat_override_flag = g_tsm.tsm_action_param.lat_override_flag;
    // TODO:
    g_tsm_output.p_ctrl_arb->NDA_LatState = 0;
    g_tsm_output.p_ctrl_arb->NDA_LongState = 0;
    g_tsm_output.p_ctrl_arb->NDA_ILC_State = 0;
	g_tsm_output.p_ctrl_arb->control_arb_request = g_tsm.tsm_action_param.control_arb_request;


    // ----------------------------------------- DeciArb -----------------------------------------------
    memcpy(&g_tsm_output.p_deci_arb->TimeStamp, &g_tsm_input.p_can_gate->TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    memcpy(&g_tsm_output.p_deci_arb->DeScenarioType, &g_tsm_input.p_can_gate->Soc_Info.scenario_type,
        sizeof(Dt_RECORD_ScenarioType));

    // ------- 给到Diag诊断相关 --------
    // TODO:
    memcpy(&g_tsm_output.p_diag->Tsm_TimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    g_tsm_output.p_diag->Tsm_Status = 0;
    
    // ------------------------------------------ HMI -------------------------------------------------
    // TODO:
    memcpy(&g_tsm_output.p_hmi->Tsm_TimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    g_tsm_output.p_hmi->Fault_Info = 0;  
    // TODO:
    g_tsm_output.p_hmi->Tor_Request = 0;

    // ------------------------------------------ CANgate ----------------------------------------------
    // TODO:
    memcpy(&g_tsm_output.p_can_gate->Tsm_TimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&g_tsm_output.p_can_gate->Decision_Arbitrator_TimeStamp, &g_tsm_input.p_deci_arb->Decision_Arbitrator_TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    memcpy(&g_tsm_output.p_can_gate->Control_Arbitrator_TimeStamp, &g_tsm_input.p_ctrl_arb->timestamp,
        sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    g_tsm_output.p_can_gate->post_hmi_request.EPB_Request = 0;
    // TODO:
    g_tsm_output.p_can_gate->post_hmi_request.Hazard_Light_Request = 0;
    // TODO:
    g_tsm_output.p_can_gate->post_hmi_request.Ecall_Request = 0;
    // TODO:
    g_tsm_output.p_can_gate->post_hmi_request.Door_Unlock_Request = 0;
    g_tsm_output.p_can_gate->Tsm_To_Soc.AS_Status = g_tsm_input.p_ctrl_arb->AS_Status;
    memcpy(&g_tsm_output.p_can_gate->Tsm_To_Soc.Control_Arbitrator_Results, &g_tsm_input.p_ctrl_arb->Control_Arbitrator_Results, 
        sizeof(Dt_RECORD_Control_Arbitrator_Results));
    g_tsm_output.p_can_gate->Tsm_To_Soc.Lane_Change_Allow_Flag = g_tsm_input.p_deci_arb->Lane_Change_Allow_Flag;
    g_tsm_output.p_can_gate->Tsm_To_Soc.Parking_EPS_handshake_state = g_tsm_input.p_ctrl_arb->Parking_EPS_handshake_state;
    g_tsm_output.p_can_gate->Tsm_To_Soc.MCU_MRM_Active_St = IsInMCUMRMActiveSt();
    // TODO:
    memcpy(&g_tsm_output.p_can_gate->Mcu_To_Ifc.time_stamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    g_tsm_output.p_can_gate->Mcu_To_Ifc.MCU_MRM_status = 0; 
}