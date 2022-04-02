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

// -------------------------------  global variable definition ----------------------------
StateMachine        g_tsm;
// -------------------------------- driving table initilize -------------------------------
static const StateTransit g_state_transit_table[TOTAL_TRANS_NUM] = 
{
    {MCU_MRM_PASSIVE,             EVENT_STANDBY,              MCU_MRM_STANDBY},
    {MCU_MRM_PASSIVE,             EVENT_LIGHTING,             MCU_MRM_FAILURE_LIGHTING},
    {MCU_MRM_PASSIVE,             EVENT_NO_LIGHTING,          MCU_MRM_FAILURE_NO_LIGHTING},
    {MCU_MRM_FAILURE_LIGHTING,    EVENT_NO_LIGHTING,          MCU_MRM_FAILURE_NO_LIGHTING},
    {MCU_MRM_FAILURE_LIGHTING,    EVENT_FAULT_NOT_EXIST,      MCU_MRM_PASSIVE},
    {MCU_MRM_FAILURE_NO_LIGHTING, EVENT_LIGHTING,             MCU_MRM_FAILURE_LIGHTING},
    {MCU_MRM_FAILURE_NO_LIGHTING, EVENT_FAULT_NOT_EXIST,      MCU_MRM_PASSIVE},
    {MCU_MRM_STANDBY,             EVENT_NO_STANDBY,           MCU_MRM_PASSIVE},
    {MCU_MRM_STANDBY,             EVENT_LIGHTING,             MCU_MRM_FAILURE_LIGHTING},
    {MCU_MRM_STANDBY,             EVENT_NO_LIGHTING,          MCU_MRM_FAILURE_NO_LIGHTING},
    {MCU_MRM_STANDBY,             EVENT_MRM_BOTH_CTRL,        MCU_MRM_ACTIVE_LNG_LAT_CTRL},
    {MCU_MRM_STANDBY,             EVENT_MRM_LAT_CTRL,         MCU_MRM_ACTIVE_LAT_CTRL},
    {MCU_MRM_STANDBY,             EVENT_MRC,                  MCU_MRM_MRC},
    {MCU_MRM_ACTIVE_LNG_LAT_CTRL, EVENT_MRM_LAT_CTRL,         MCU_MRM_ACTIVE_LAT_CTRL},
    {MCU_MRM_ACTIVE_LNG_LAT_CTRL, EVENT_MRC_FROM_MRM,         MCU_MRM_MRC},
    {MCU_MRM_ACTIVE_LNG_LAT_CTRL, EVENT_LIGHTING_FROM_MRM,    MCU_MRM_FAILURE_LIGHTING},
    {MCU_MRM_ACTIVE_LNG_LAT_CTRL, EVENT_NO_LIGHTING_FROM_MRM, MCU_MRM_FAILURE_NO_LIGHTING},
    {MCU_MRM_ACTIVE_LAT_CTRL,     EVENT_MRM_BOTH_CTRL,        MCU_MRM_ACTIVE_LNG_LAT_CTRL},
    {MCU_MRM_ACTIVE_LAT_CTRL,     EVENT_MRC_FROM_MRM,         MCU_MRM_MRC},
    {MCU_MRM_ACTIVE_LAT_CTRL,     EVENT_LIGHTING_FROM_MRM,    MCU_MRM_FAILURE_LIGHTING},
    {MCU_MRM_ACTIVE_LAT_CTRL,     EVENT_NO_LIGHTING_FROM_MRM, MCU_MRM_FAILURE_NO_LIGHTING},
    {MCU_MRM_MRC,                 EVENT_LIGHTING_FROM_MRM,    MCU_MRM_FAILURE_LIGHTING},
    {MCU_MRM_MRC,                 EVENT_NO_LIGHTING_FROM_MRM, MCU_MRM_FAILURE_NO_LIGHTING}
};

static const Action g_action = 
{
    ActionInPassive,     ActionInFailureLighting, ActionInFailureNoLighting, ActionInStandby,
    ActionInMrmBothCtrl, ActionInMrmLatCtrl,      ActionInMrc
};

static const TransitEvent g_event = 
{
    IsMrmSystemFaultNotExist,  IsLightingConditionMeet,    IsNoLightingConditionMeet, IsStandbyConditionMeet,
    IsStandbyConditionNotMeet, IsMrmBothCtrlConditionMeet, IsMrmLatCtrlConditionMeet, IsEnterMrcFromStandby,
    IsEnterMrcFromMrm,         IsEnterLightingFromMrm,     IsEnterNoLightingFromMrm
};

// -------------------------------- function definition   ---------------------------------
void MRM_TSM_MODULE_Init(void)
{
    memset(&g_tsm, 0, sizeof(g_tsm));
    g_tsm.inter_media_msg.automaton_transit_normal_flag = 1;
    g_tsm.action_param.mrm_activation_st = 1;
}

void MRM_TSM_MODULE(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm, Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag)
{
    // TODO:
    (void)ValidateRcvMsgTimeStamp(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);
    SignalHandling(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);
    TsmChartManager(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM);
    WrapAndSend(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm, rty_DeTsm2Planlite, rty_DeTSM2CtrlArb,
        rty_DeTSM2DecisionArbitrator, rty_DeTSM2Diag);
#ifdef _NEED_LOG
    // LOG("rty_DeTSM2CtrlArb->MRM_Status: %d", rty_DeTsm2Planlite->MRM_Status);
#endif
}

void TsmChartManager(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM) 
{
    EventID event_id_array[(uint8)EVENT_COUNT];
    uint8   size = 0;
    // 找到触发事件ID的数组
    for (uint8 i = 0; i < (uint8)EVENT_COUNT; ++i) {
        if (g_event[i](rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
            event_id_array[size++] = i;
        }
    }
    StateTransit* state_transit = FindTrans(&g_tsm, event_id_array, size);
    if (state_transit == NULL_PTR) {
#ifdef _NEED_LOG
        LOG("No transit, stay in current state.");
#endif
        g_action[(uint8)g_tsm.state]();
        return;
    }

    g_action[(uint8)state_transit->next_st]();
    g_tsm.state = state_transit->next_st;
}

StateTransit* FindTrans(StateMachine* state_machine, const EventID event_id_array[], const uint8 array_length)
{
    if (array_length == 0) {
        return NULL_PTR;
    }
    for (uint8 i = 0; i < (uint8)TOTAL_TRANS_NUM; ++i) {
        if (state_machine->state == g_state_transit_table[i].cur_st) {
            for (uint8 j = 0; j < array_length; ++j) {
                if (g_state_transit_table[i].event_id == event_id_array[j]) {
                    return &g_state_transit_table[i];
                }
            }  
        }
    }
    return NULL_PTR;
}

boolean IsMrmSystemFaultNotExist(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    if (g_tsm.inter_media_msg.mrm_system_fault_level == NO_FAULT) {
        return true;
    }
    return false;
}

boolean IsLightingConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    return false;
    /*
    if ((g_tsm.inter_media_msg.mrm_system_fault_level == FAILURE_FAULT) &&
        g_tsm.inter_media_msg.mrm_failure_lighting_flag) {
        return true;
    }
    return false;
    */
}

boolean IsNoLightingConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    return false;
    /*
    if ((g_tsm.inter_media_msg.mrm_system_fault_level == FAILURE_FAULT) &&
        !g_tsm.inter_media_msg.mrm_failure_lighting_flag) {
        return true;
    }
    return false;
    */
}

boolean IsStandbyConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    return true;
    /*
    if ((g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_EPB_PHASE_IN) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_ON_NORMAL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_ON_STANDACTIVE) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_ON_STANDWAIT) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_FREE_NORMAL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_FREE_STANDACTIVE) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_FREE_STANDWAIT) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LNG_OVERRIDE_HANDS_FREE) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LNG_OVERRIDE_HANDS_ON) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LAT_LNG_OVERRIDE) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LAT_OVERRIDE) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_TOR_WITH_STAND) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_TOR_WITH_LAT_CONTROL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_TOR_WITH_LAT_LNG_CONTROL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EMERGENCY_LANE_WITH_LAT_LNG_CONTROL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EMERGENCY_LANE_WITH_LAT_CONTROL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_CONTROL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_LNG_CONTROL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_CONTROL) ||
        (g_tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_LNG_CONTROL)) {
        return true;
    }
    return false;
    */
}

boolean IsStandbyConditionNotMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    return false;
}

boolean IsMrmBothCtrlConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    // TODO： 未考虑soc激活状态， MCU安全停车系统故障
    if ((rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt != VEH_STANDSTILL_ST_STANDSTILL) &&
        !g_tsm.inter_media_msg.automaton_transit_normal_flag) {
        if ((g_tsm.inter_media_msg.lng_override_st == OVERRIDE_NOT_SATISFY) && !g_tsm.inter_media_msg.brake_is_set) {
            return true;
        }        
    }
    return false;
}

boolean IsMrmLatCtrlConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    // TODO: 未考虑soc激活状态， MCU安全停车系统故障
    if ((rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt != VEH_STANDSTILL_ST_STANDSTILL) &&
        !g_tsm.inter_media_msg.automaton_transit_normal_flag) {
        if ((g_tsm.inter_media_msg.lng_override_st == OVERRIDE_SATISFY) || g_tsm.inter_media_msg.brake_is_set) {
            return true;
        }   
    }
    return false;
}

boolean IsEnterMrcFromStandby(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    // TODO: 未考虑soc激活状态， MCU安全停车系统故障
    if ((rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt == VEH_STANDSTILL_ST_STANDSTILL) &&
        !g_tsm.inter_media_msg.automaton_transit_normal_flag) {
        return true;
    }   
    return false;
}

boolean IsEnterMrcFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    if (rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt == VEH_STANDSTILL_ST_STANDSTILL) {
        return true;
    }
    return false;
}

boolean IsEnterLightingFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{   
    if (IsDriverTakeOver() && g_tsm.inter_media_msg.mrm_failure_lighting_flag &&
        !g_tsm.inter_media_msg.automaton_transit_normal_flag) {
        return true;
    }
    return false;
}

boolean IsEnterNoLightingFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    if (IsDriverTakeOver() && !g_tsm.inter_media_msg.mrm_failure_lighting_flag &&
        !g_tsm.inter_media_msg.automaton_transit_normal_flag) {
        return true;
    }
    return false;
}

boolean IsDriverTakeOver()
{
    if ((g_tsm.state == MCU_MRM_ACTIVE_LAT_CTRL) || (g_tsm.state == MCU_MRM_MRC)) {
        if (g_tsm.inter_media_msg.brake_intervention_type == LONG_TERM_INTERVENTION) {
            return true;
        }
        if ((g_tsm.inter_media_msg.driver_attention_st == AWAKE_AND_NOT_DISTRACTED) &&
            g_tsm.inter_media_msg.lng_override_long_duration_flag) {
            return true;
        }
        if ((g_tsm.inter_media_msg.driver_attention_st == AWAKE_AND_LOOK_REARVIEW_OR_HU) ||
            (g_tsm.inter_media_msg.driver_attention_st == AWAKE_AND_DISTRACTED) ||
            (g_tsm.inter_media_msg.driver_attention_st == FATIGUE_DRIVER_ATTENTION_ST)) {
            if (g_tsm.inter_media_msg.hands_can_takeover && g_tsm.inter_media_msg.lng_override_long_duration_flag) {
                return true;
            }
        }
    }

    if ((g_tsm.inter_media_msg.driver_attention_st == AWAKE_AND_NOT_DISTRACTED) &&
        (g_tsm.inter_media_msg.hands_can_takeover)) {
        return true;
    }

    if (g_tsm.inter_media_msg.driver_hand_torque_st == OVERRIDE_SATISFY) {
        return true;
    }
    return false;
}

void ActionInPassive()
{
#ifdef _NEED_LOG
    LOG("It's in MCU_MRM_PASSIVE St.");
#endif
    g_tsm.action_param.mrm_activation_st = 1;
}

void ActionInFailureLighting()
{
#ifdef _NEED_LOG
    LOG("It's in Failure Lighting St.");
#endif
    g_tsm.action_param.lng_override_flag = 0;
    g_tsm.action_param.mrm_activation_st = 1;
}

void ActionInFailureNoLighting()
{
#ifdef _NEED_LOG
    LOG("It's in Failure No Lighting St.");
#endif
    g_tsm.action_param.mrm_activation_st = 1;
}

void ActionInStandby()
{
#ifdef _NEED_LOG
    LOG("It's in Standby St.");
#endif
    g_tsm.action_param.mrm_activation_st = 1;
}

void ActionInMrmBothCtrl()
{
#ifdef _NEED_LOG
    LOG("It's in Mrm Both Ctrl St.");
#endif
    // car test
    g_tsm.action_param.lng_override_flag = 0;
    g_tsm.action_param.mrm_activation_st = 0;
}

void ActionInMrmLatCtrl()
{
#ifdef _NEED_LOG
    LOG("It's in Mrm Lat Ctrl St.");
#endif
    // car test
    g_tsm.action_param.lng_override_flag = 1;
    g_tsm.action_param.mrm_activation_st = 0;
}

void ActionInMrc()
{
#ifdef _NEED_LOG
    LOG("It's in Mrc St.");
#endif
    g_tsm.action_param.lng_override_flag = 0;
    g_tsm.action_param.mrm_activation_st = 1;
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
    if (((float32)(cur_timestamp->nsec - last_timestamp->nsec) / NS_IN_MS) > UPPER_CYCLE ||
        ((float32)(cur_timestamp->nsec - last_timestamp->nsec) / NS_IN_MS) < LOWER_CYCLE) {
        return false;
    }
    return true;
}
boolean IsTimeStampError(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp)
{
    if (!cur_timestamp->is_valid || cur_timestamp->sec < last_timestamp->sec) {
        return false;
    }
    return true;
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
    tsm_timestamp.is_valid = 1;
    hb_TimeSync_GetTime(&time_in_ten_ns);   // 10ns per
    tsm_timestamp.nsec = (uint32)time_in_ten_ns * NS_PER_TEN_NS;
    tsm_timestamp.sec  = (uint32)time_in_ten_ns / TEN_NS_PER_S;

    // 保存soc状态机状态
    memcpy(&g_tsm.inter_media_msg.last_automaton_st, &rtu_DeCANGATE2TSM->Soc_Info.Automaton_State,
        sizeof(Dt_RECORD_Automaton_State));

    //------------------------------------------- 给到planlite相关 -----------------------------------------
    memcpy(&rty_DeTsm2Planlite->TimeStamp, &rtu_DeCANGATE2TSM->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    // TODO: 状态机提供
    memcpy(&rty_DeTsm2Planlite->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    rty_DeTsm2Planlite->NDA_Lane_Change_Type = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_Type;
    rty_DeTsm2Planlite->NDA_Lane_Change_Direction = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_Direction;
    rty_DeTsm2Planlite->NDA_Lane_Change_State = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State;
    rty_DeTsm2Planlite->MRM_Status = g_tsm.action_param.mrm_activation_st;

    //------------------------------------------- 给到CtrlArb相关  ----------------------------------------
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_CanGate, &rtu_DeCANGATE2TSM->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_PlannLite, &rtu_DePlanlite2Tsm->Planning_Lite_TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    // TODO: 状态机提供
    memcpy(&rty_DeTSM2CtrlArb->DeTimeStamp, &tsm_timestamp, sizeof(Dt_RECORD_TimeStamp));
    rty_DeTSM2CtrlArb->holo_planning_control_status = rtu_DePlanlite2Tsm->planningLite_control_state;
    memcpy(&rty_DeTSM2CtrlArb->Automaton_State, &rtu_DeCANGATE2TSM->Soc_Info.Automaton_State,
        sizeof(Dt_RECORD_Automaton_State));
    rty_DeTSM2CtrlArb->lng_override_flag = g_tsm.action_param.lng_override_flag;

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
    //     g_tsm.inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag;
    // // to do: 状态机提供
    // memset(&rty_DeTSM2CANGATE->Mcu_To_Ifc.time_stamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // // to do:
    // rty_DeTSM2CANGATE->Mcu_To_Ifc.MCU_MRM_status = 0;  // MRM状态, 表示MCU安全停车是否可用,以供IFC判断是否启动安全停车 -- 功能故障,和明江讨论下
}