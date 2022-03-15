#include "tsm_chart.h"
#include "tsm_signalhandling.h"

void MrmTsmModule(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm,
    Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb,
    Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag)
{
    SignalHandling(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);  // 信号处理接口
    TsmChartManager();   // 运行状态机接口
}

// ------------- 状态机 ---------------

// 思路： 当前状态， 输入事件(跳转条件)， 下一状态， 具体行为    ---> 状态跳转
// 维护一张 状态跳转表格， 在表格中找到对应的状态跳转(寻找方式是
// 当前状态和输入事件匹配，则去赋值下一状态，进行具体行为)
void TsmChartManager() {
    enum EventID event_id_array[(uint8)EVENT_COUNT];
    uint8 size = 0;
    // 找到触发事件ID的数组,因为状态和事件有相关性，我们会先找到符合的事件，然后用事件去匹配状态的思想，
    // 并按优先级，找到跳转
    for (uint8 i = 0; i < (uint8)EVENT_COUNT; ++i) {
        if (event[i](&tsm)) {
            event_id_array[size++] = (enum EventID)i;
        }
    }
    // 基于触发事件数组和状态机当前状态，寻找对应的transit
    StateTransit* state_transit = FindTrans(&tsm, event_id_array, size);
    if (state_transit == NULL_PTR) {
        LOG("No transit, stay in current state.");
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
boolean IsMrmSystemFaultNotExist()
{
    if (tsm.inter_media_msg.mrm_system_fault_level == NO_FAULT) {
        return true;
    }
    return false;
}

boolean IsLightingConditionMeet()
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

boolean IsNoLightingConditionMeet()
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

boolean IsStandbyConditionMeet()
{
    return true;
    /*
    if ((tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_EPB_PHASE_IN) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_ON_NORMAL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_ON_STANDACTIVE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_ON_STANDWAIT) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_FREE_NORMAL) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_FREE_STANDACTIVE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_ACTIVE_HAND_FREE_STANDWAIT) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LNG_OVERRIDE_HANDS_FREE) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LNG_OVERRIDE_HANDS_ON) ||
        (tsm.inter_media_msg.automaton_st.NDA_Function_State == (uint8)NDA_LAT_LNG_OVERRIDE) ||
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

boolean IsStandbyConditionNotMeet()
{
    
    return false;
}

boolean IsMrmBothCtrlConditionMeet()
{
    // TODO: 
    if ((tsm.inter_media_msg.lng_override_st == OVERRIDE_NOT_SATISFY) &&
        !tsm.inter_media_msg.brake_is_set) {
        return true;        
    }
    return false;
}

boolean IsMrmLatCtrlConditionMeet()
{
    // TODO:
    if ((tsm.inter_media_msg.lng_override_st == OVERRIDE_SATISFY) ||
        tsm.inter_media_msg.brake_is_set) {
        return true;
    }
    return false;
}

boolean IsEnterMrcFromStandby()
{
    return false;
}

boolean IsEnterMrcFromMrm()
{
    return false;
}

boolean IsEnterLightingFromMrm()
{
    return false;
}

boolean IsEnterNoLightingFromMrm()
{
    return false;
}

boolean IsEnterLightingFromMrc()
{
    return false;
}

boolean IsEnterNoLightingFromMrc()
{
    return false;
}

// action definition
void ActionInPassive()
{
    LOG("It's in Passive St.");
}

void ActionInFailureLighting()
{
    LOG("It's in Failure Lighting St.");
    tsm.action_param.lng_override_flag = 0;
}

void ActionInFailureNoLighting()
{
    LOG("It's in Failure No Lighting St.");
}

void ActionInStandby()
{
    LOG("It's in Standby St.");
}

void ActionInMrmBothCtrl()
{
    LOG("It's in Mrm Both Ctrl St.");
    // car test
    tsm.action_param.lng_override_flag = 0;
}

void ActionInMrmLatCtrl()
{
    LOG("It's in Mrm Lat Ctrl St.");
    // car test
    tsm.action_param.lng_override_flag = 1;
}

void ActionInMrc()
{
    LOG("It's in Mrc St.");
}
