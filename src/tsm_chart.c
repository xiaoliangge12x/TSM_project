#include "tsm_chart.h"

// ------------- 状态机 ---------------

// 思路： 当前状态， 输入事件(跳转条件)， 下一状态， 具体行为    ---> 状态跳转
// 维护一张 状态跳转表格， 在表格中找到对应的状态跳转(寻找方式是
// 当前状态和输入事件匹配，则去赋值下一状态，进行具体行为)
void TsmChartManager() {
    enum EventID event_id_array[(uint8)EVENT_COUNT];
    uint8 size = 0;
    // 找到对应的触发事件ID的数组
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
    return false;
}

boolean IsLightingConditionMeet()
{
    return false;
}

boolean IsNoLightingConditionMeet()
{
    return false;
}

boolean IsStandbyConditionMeet()
{
    return true;
}

boolean IsStandbyConditionNotMeet()
{
    return false;
}

boolean IsMrmBothCtrlConditionMeet()
{
    return false;
}

boolean IsMrmLatCtrlConditionMeet()
{
    return true;
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
    return true;
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