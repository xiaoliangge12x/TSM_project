/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  MRM top state machine main structure implementation
 *  \author zengxiaoliang (zengxiaoliang@holomatic.com)
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_chart.h"

// 全局变量的定义
StateMachine tsm;

void MRM_TSM_MODULE_Init(void)
{
    memset(&tsm, 0, sizeof(tsm));
    // 默认该信号跳转正常，一定要注意
    tsm.inter_media_msg.automaton_transit_normal_flag = 1;   // 1 跳转正常 ， 0 跳转异常
    tsm.action_param.mrm_activation_st = 1;   // 1 exit  0 not exit
}

void MRM_Swc_V_TSM(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, 
    const Dt_RECORD_DecisionArbitrator2TSM *rtu_DeDecisionArbitrator2TSM, 
    const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm,
    Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb,
    Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag, Dt_RECORD_TSM2HMI *rty_DeTSM2HMI,
    Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE)
{

    SignalHandling(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);  // 信号处理接口
    TsmChartManager(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM);   // 运行状态机接口
    WrapAndSend(rtu_DeCtrlArb2TSM, rtu_DeDecisionArbitrator2TSM, rtu_DeCANGATE2TSM, 
        rtu_DeDiag2TSM, rtu_DePlanlite2Tsm, rty_DeTsm2Planlite, rty_DeTSM2CtrlArb,
        rty_DeTSM2DecisionArbitrator, rty_DeTSM2Diag, rty_DeTSM2HMI, rty_DeTSM2CANGATE);
#ifdef _NEED_LOG
    LOG("rty_DeTSM2CtrlArb->MRM_Status: %d", rty_DeTsm2Planlite->MRM_Status);
#endif
}

// ------------- 状态机 ---------------

// 思路： 当前状态， 输入事件(跳转条件)， 下一状态， 具体行为    ---> 状态跳转
// 维护一张 状态跳转表格， 在表格中找到对应的状态跳转(寻找方式是
// 当前状态和输入事件匹配，则去赋值下一状态，进行具体行为)
void TsmChartManager(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM) {
    enum EventID event_id_array[(uint8)EVENT_COUNT];
    uint8 size = 0;
    // 找到触发事件ID的数组,因为状态和事件有相关性，我们会先找到符合的事件，然后用事件去匹配状态的思想，
    // 并按优先级，找到跳转
    for (uint8 i = 0; i < (uint8)EVENT_COUNT; ++i) {
        if (event[i](rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
            event_id_array[size++] = (enum EventID)i;
        }
    }
    // 基于触发事件数组和状态机当前状态，寻找对应的transit
    StateTransit* state_transit = FindTrans(&tsm, event_id_array, size);
    if (state_transit == NULL_PTR) {
#ifdef _NEED_LOG
        LOG("No transit, stay in current state.");
#endif
        // 执行当前操作
        action[(uint8)tsm.state]();
        return;
    }

    // 如果找到transit, 执行下一状态所需要做的action,并将当前状态刷新
    action[(uint8)state_transit->next_st]();
    tsm.state = state_transit->next_st;
}

StateTransit* FindTrans(StateMachine* state_machine, 
    const enum EventID event_id_array[], const uint8 array_length)
{
    if (array_length == 0) {
        return NULL_PTR;
    }
    for (uint8 i = 0; i < (uint8)TOTAL_TRANS_NUM; ++i) {
        if (state_machine->state == state_transit_array[i].cur_st) {
            for (uint8 j = 0; j < array_length; ++j) {
                if (state_transit_array[i].event_id == event_id_array[j]) {
                    return &state_transit_array[i];
                }
            }  
        }
    }
    return NULL_PTR;
}

// event声明
boolean IsMrmSystemFaultNotExist(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    if (tsm.inter_media_msg.mrm_system_fault_level == NO_FAULT) {
        return true;
    }
    return false;
}

boolean IsLightingConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    return false;
    /*
    if ((tsm.inter_media_msg.mrm_system_fault_level == FAILURE_FAULT) &&
        tsm.inter_media_msg.mrm_failure_lighting_flag) {
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
    if ((tsm.inter_media_msg.mrm_system_fault_level == FAILURE_FAULT) &&
        !tsm.inter_media_msg.mrm_failure_lighting_flag) {
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
    if ((tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_EPB_PHASE_IN) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HANDS_ON_NORMAL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HANDS_ON_STAND_ACTIVE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HANDS_ON_STAND_WAIT) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HANDS_FREE_NORMAL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HANDS_FREE_STAND_WAIT) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LNG_OVERRIDE_HANDS_FREE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LNG_OVERRIDE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LNG_LAT_OVERRIDE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LAT_OVERRIDE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_TOR_WITH_STAND) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_TOR_WITH_LAT_CONTROL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_TOR_WITH_LAT_LNG_CONTROL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EMERGENCY_LANE_WITH_LAT_LNG_CONTROL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EMERGENCY_LANE_WITH_LAT_CONTROL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_CONTROL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_LNG_CONTROL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_CONTROL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_LNG_CONTROL)) {
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
    // TO DO： 未考虑soc激活状态， MCU安全停车系统故障
    if ((rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt != STANDSTILL) &&
        (!tsm.inter_media_msg.automaton_transit_normal_flag)) {
        if ((tsm.inter_media_msg.lng_override_st == OVERRIDE_NOT_SATISFY) &&
            (!tsm.inter_media_msg.brake_is_set)) {
            return true;
        }        
    }
    return false;
}

boolean IsMrmLatCtrlConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    // TODO: 未考虑soc激活状态， MCU安全停车系统故障
    if ((rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt != STANDSTILL) &&
        (!tsm.inter_media_msg.automaton_transit_normal_flag)) {
        if ((tsm.inter_media_msg.lng_override_st == OVERRIDE_SATISFY) ||
            (tsm.inter_media_msg.brake_is_set)) {
            return true;
        }   
    }
    return false;
}

boolean IsEnterMrcFromStandby(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    // TODO: 未考虑soc激活状态， MCU安全停车系统故障
    if ((rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt == STANDSTILL) &&
        (!tsm.inter_media_msg.automaton_transit_normal_flag)) {
        return true;
    }   
    return false;
}

boolean IsEnterMrcFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    if (rtu_DeCANGATE2TSM->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt == STANDSTILL) {
        return true;
    }
    return false;
}

// to do : 超出规控没给
boolean IsEnterLightingFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{   
    // to do : 跳转错误先摘掉
    // 驾驶员接管 && lighting && 跳转错误
    if (IsDriverTakeOver() &&
        tsm.inter_media_msg.mrm_failure_lighting_flag &&
        !tsm.inter_media_msg.automaton_transit_normal_flag) {
        return true;
    }
}

// to do : 超出规控没给
boolean IsEnterNoLightingFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM)
{
    // to do: 跳转错误先摘掉
    // 驾驶员接管 && lighting && 跳转错误
    if (IsDriverTakeOver() &&
        !tsm.inter_media_msg.mrm_failure_lighting_flag &&
        !tsm.inter_media_msg.automaton_transit_normal_flag) {
        return true;
    }
    return false;
}

boolean IsDriverTakeOver()
{
    if ((tsm.state == MRM_LAT_CTRL) || (tsm.state == MRC)) {
        // 需要判断纵向的长时踩油门 和 踩刹车
        if (tsm.inter_media_msg.brake_intervention_type == LONG_TERM_INTERVENTION) {
            return true;
        }

        if ((tsm.inter_media_msg.driver_attention_st == AWAKE_AND_NOT_DISTRACTED) &&
            tsm.inter_media_msg.lng_override_long_duration_flag) {
            return true;
        }

        if ((tsm.inter_media_msg.driver_attention_st == AWAKE_AND_LOOK_REARVIEW_OR_HU) ||
            (tsm.inter_media_msg.driver_attention_st == AWAKE_AND_DISTRACTED) ||
            (tsm.inter_media_msg.driver_attention_st == FATIGUE_DRIVER_ATTENTION_ST)) {
            if (tsm.inter_media_msg.hands_can_takeover && 
                tsm.inter_media_msg.lng_override_long_duration_flag) {
                return true;
            }
        }
    }

    // 手是否具备接管
    if ((tsm.inter_media_msg.driver_attention_st == AWAKE_AND_NOT_DISTRACTED) &&
        (tsm.inter_media_msg.hands_can_takeover)) {
        return true;
    }

    // 手力矩超越判断
    if (tsm.inter_media_msg.driver_hand_torque_st == OVERRIDE_SATISFY) {
        return true;
    }
    return false;
}

// action definition
void ActionInPassive()
{
#ifdef _NEED_LOG
    LOG("It's in Passive St.");
#endif
    tsm.action_param.mrm_activation_st = 1;
}

void ActionInFailureLighting()
{
#ifdef _NEED_LOG
    LOG("It's in Failure Lighting St.");
#endif
    tsm.action_param.lng_override_flag = 0;
    tsm.action_param.mrm_activation_st = 1;
}

void ActionInFailureNoLighting()
{
#ifdef _NEED_LOG
    LOG("It's in Failure No Lighting St.");
#endif
    tsm.action_param.mrm_activation_st = 1;
}

void ActionInStandby()
{
#ifdef _NEED_LOG
    LOG("It's in Standby St.");
#endif
    tsm.action_param.mrm_activation_st = 1;
}

void ActionInMrmBothCtrl()
{
#ifdef _NEED_LOG
    LOG("It's in Mrm Both Ctrl St.");
#endif
    // car test
    tsm.action_param.lng_override_flag = 0;
    tsm.action_param.mrm_activation_st = 0;
}

void ActionInMrmLatCtrl()
{
#ifdef _NEED_LOG
    LOG("It's in Mrm Lat Ctrl St.");
#endif
    // car test
    tsm.action_param.lng_override_flag = 1;
    tsm.action_param.mrm_activation_st = 0;
}

void ActionInMrc()
{
#ifdef _NEED_LOG
    LOG("It's in Mrc St.");
#endif
    tsm.action_param.lng_override_flag = 0;
    tsm.action_param.mrm_activation_st = 1;
}


// 组包并发送
void WrapAndSend(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, 
    const Dt_RECORD_DecisionArbitrator2TSM *rtu_DeDecisionArbitrator2TSM, 
    const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm,
    Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb,
    Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator,
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag, Dt_RECORD_TSM2HMI *rty_DeTSM2HMI,
    Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE)
{
    // 保存SOC状态
    memcpy(&tsm.inter_media_msg.last_automaton_st, &rtu_DeCANGATE2TSM->Soc_Info.Automaton_State,
        sizeof(Dt_RECORD_Automaton_State));
    
    // 组包发送相关
    // ----- 给到planlite相关 ---------
    memcpy(&rty_DeTsm2Planlite->TimeStamp, &rtu_DeCANGATE2TSM->TimeStamp, 
        sizeof(Dt_RECORD_TimeStamp));
    // to do: 状态机提供
    memset(&rty_DeTsm2Planlite->DeTimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    rty_DeTsm2Planlite->NDA_Lane_Change_Type = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_Type;
    rty_DeTsm2Planlite->NDA_Lane_Change_Direction = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_Direction;
    rty_DeTsm2Planlite->NDA_Lane_Change_State = rtu_DeCANGATE2TSM->Soc_Info.NDA_Lane_Change_State;
    // to do: 状态机提供
    rty_DeTsm2Planlite->MRM_Status = tsm.action_param.mrm_activation_st;   // 1 激活， 0 取消

    // ----- 给到CtrlArb相关 --------
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_CanGate, &rtu_DeCANGATE2TSM->TimeStamp, 
        sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CtrlArb->TimeStamp_PlannLite, &rtu_DePlanlite2Tsm->DeTimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    // to do: 状态机提供
    memset(&rty_DeTSM2CtrlArb->DeTimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // 状态机输出信号相关
    rty_DeTSM2CtrlArb->holo_planning_control_status = rtu_DePlanlite2Tsm->planningLite_control_state;
    // to do: 是否需要封装一层状态
    memcpy(&rty_DeTSM2CtrlArb->Automaton_State, &rtu_DeCANGATE2TSM->Soc_Info.Automaton_State,
        sizeof(Dt_RECORD_Automaton_State));
    rty_DeTSM2CtrlArb->lng_override_flag = tsm.action_param.lng_override_flag;
    // to do: 暂时全发0，逻辑还没定 
    rty_DeTSM2CtrlArb->NDA_LatState = 0;
    rty_DeTSM2CtrlArb->NDA_LongState = 0;
    rty_DeTSM2CtrlArb->NDA_ILC_State = 0;


    // ------- 给到DeciArb相关 ---------
    memcpy(&rty_DeTSM2DecisionArbitrator->TimeStamp, &rtu_DeCANGATE2TSM->TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2DecisionArbitrator->DeScenarioType, &rtu_DeCANGATE2TSM->Soc_Info,
        sizeof(Dt_RECORD_ScenarioType));

    // ------- 给到Diag诊断相关 --------
    // to do: 状态机提供
    memset(&rty_DeTSM2Diag->Tsm_TimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // to do: 状态机提供， TSM故障状态
    rty_DeTSM2Diag->Tsm_Status = 0;
    
    // -------- 给到HMI相关 --------
    // to do: 状态机提供
    memset(&rty_DeTSM2HMI->Tsm_TimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // to do: 状态机提供
    rty_DeTSM2HMI->Fault_Info = 0;     // 故障信息
    // to do: 状态机提供
    rty_DeTSM2HMI->Tor_Request = 0;    // tor请求

    // -------- 给到CANgate相关 -------
    // to do: 状态机提供
    memset(&rty_DeTSM2CANGATE->Tsm_TimeStamp, 0, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CANGATE->Decision_Arbitrator_TimeStamp, &rtu_DeDecisionArbitrator2TSM->Decision_Arbitrator_TimeStamp,
        sizeof(Dt_RECORD_TimeStamp));
    memcpy(&rty_DeTSM2CANGATE->Control_Arbitrator_TimeStamp, &rtu_DeCtrlArb2TSM->timestamp,
        sizeof(Dt_RECORD_TimeStamp));
    // to do: 状态机判断提供
    rty_DeTSM2CANGATE->EPB_Request = 0;
    // to do: 状态机判断提供
    rty_DeTSM2CANGATE->Hazard_Light_Request = 0;
    // to do: 状态机判断提供
    rty_DeTSM2CANGATE->Ecall_Request = 0;
    // to do: 状态机判断提供
    rty_DeTSM2CANGATE->Door_Unlock_Request = 0;
    rty_DeTSM2CANGATE->Tsm_To_Soc.AS_Status = rtu_DeCtrlArb2TSM->AS_Status;
    memcpy(&rty_DeTSM2CANGATE->Tsm_To_Soc.Control_Arbitrator_Results, &rtu_DeCtrlArb2TSM->Control_Arbitrator_Results, 
        sizeof(Dt_RECORD_Control_Arbitrator_Results));
    rty_DeTSM2CANGATE->Tsm_To_Soc.Lane_Change_Allow_Flag = rtu_DeDecisionArbitrator2TSM->Lane_Change_Allow_Flag;
    rty_DeTSM2CANGATE->Tsm_To_Soc.Parking_EPS_handshake_state = rtu_DeCtrlArb2TSM->Parking_EPS_handshake_state;
    // to do: 状态机提供状态跳转监控标志位
    rty_DeTSM2CANGATE->Tsm_To_Soc.AutomatonTransitMonitorFlag.Standby_HandsFree_St_Monitor_Flag = 
        tsm.inter_media_msg.nda_st_transit_monitor.nda_transit_enable_flag;;
    rty_DeTSM2CANGATE->Tsm_To_Soc.AutomatonTransitMonitorFlag.HandsOn_St_Monitor_Flag = 0;
    rty_DeTSM2CANGATE->Tsm_To_Soc.AutomatonTransitMonitorFlag.Override_St_Monitor_Flag = 0;
    // to do: 状态机提供
    memset(&rty_DeTSM2CANGATE->Mcu_To_Ifc.time_stamp, 0, sizeof(Dt_RECORD_TimeStamp));
    // to do:
    rty_DeTSM2CANGATE->Mcu_To_Ifc.MCU_MRM_status = 0;  // MRM状态, 表示MCU安全停车是否可用,以供IFC判断是否启动安全停车 -- 功能故障,和明江讨论下
}