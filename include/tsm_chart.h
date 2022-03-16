#ifndef TSM_CHART_H_
#define TSM_CHART_H_

#include "tsm_data.h"
#include "common.h"

void TsmChartManager();
StateTransit* FindTrans(StateMachine* state_machine, 
    const enum EventID event_id_array[], const uint8 array_length);

// event声明， 条件判断中可以不再加状态，实现状态和条件的解耦(如果真要状态，那么可以在中间条件里增加状态信号)
boolean IsMrmSystemFaultNotExist();  // event_1
boolean IsLightingConditionMeet();
boolean IsNoLightingConditionMeet();
boolean IsStandbyConditionMeet();
boolean IsStandbyConditionNotMeet();
boolean IsMrmBothCtrlConditionMeet();
boolean IsMrmLatCtrlConditionMeet();
boolean IsEnterMrcFromStandby();
boolean IsEnterMrcFromMrm();
boolean IsEnterLightingFromMrm();
boolean IsEnterNoLightingFromMrm();
boolean IsEnterLightingFromMrc();
boolean IsEnterNoLightingFromMrc();

// action 声明
void ActionInPassive();
void ActionInFailureLighting();
void ActionInFailureNoLighting();
void ActionInStandby();
void ActionInMrmBothCtrl();
void ActionInMrmLatCtrl();
void ActionInMrc();

static const StateTransit state_transit_array[] = {
    {PASSIVE, EVENT_4, STANDBY},
    {PASSIVE, EVENT_2, FAILURE_LIGHTING},
    {PASSIVE, EVENT_3, FAILURE_NO_LIGHTING},
    {FAILURE_LIGHTING, EVENT_3, FAILURE_NO_LIGHTING},
    {FAILURE_LIGHTING, EVENT_1, PASSIVE},
    {FAILURE_NO_LIGHTING, EVENT_2, FAILURE_LIGHTING},
    {FAILURE_NO_LIGHTING, EVENT_1, PASSIVE},
    {STANDBY, EVENT_5, PASSIVE},
    {STANDBY, EVENT_2, FAILURE_LIGHTING},
    {STANDBY, EVENT_3, FAILURE_NO_LIGHTING},
    {STANDBY, EVENT_6, MRM_LNG_LAT_CTRL},
    {STANDBY, EVENT_7, MRM_LAT_CTRL},
    {STANDBY, EVENT_8, MRC},
    {MRM_LNG_LAT_CTRL, EVENT_7, MRM_LAT_CTRL},
    {MRM_LNG_LAT_CTRL, EVENT_9, MRC},
    {MRM_LNG_LAT_CTRL, EVENT_10, FAILURE_LIGHTING},
    {MRM_LNG_LAT_CTRL, EVENT_11, FAILURE_NO_LIGHTING},
    {MRM_LAT_CTRL, EVENT_6, MRM_LNG_LAT_CTRL},
    {MRM_LAT_CTRL, EVENT_9, MRC},
    {MRM_LAT_CTRL, EVENT_10, FAILURE_LIGHTING},
    {MRM_LAT_CTRL, EVENT_11, FAILURE_NO_LIGHTING},
    {MRC, EVENT_12, FAILURE_LIGHTING},
    {MRC, EVENT_13, FAILURE_NO_LIGHTING}
};

static const Action action = {
    ActionInPassive,
    ActionInFailureLighting,
    ActionInFailureNoLighting,
    ActionInStandby,
    ActionInMrmBothCtrl,
    ActionInMrmLatCtrl,
    ActionInMrc
};

static const TransitEvent event = {
    IsMrmSystemFaultNotExist, IsLightingConditionMeet,
    IsNoLightingConditionMeet, IsStandbyConditionMeet,
    IsStandbyConditionNotMeet, IsMrmBothCtrlConditionMeet,
    IsMrmLatCtrlConditionMeet, IsEnterMrcFromStandby,
    IsEnterMrcFromMrm, IsEnterLightingFromMrm,
    IsEnterNoLightingFromMrm, IsEnterLightingFromMrc,
    IsEnterNoLightingFromMrc
};

#endif