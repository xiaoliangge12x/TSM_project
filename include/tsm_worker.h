#ifndef TSM_WORKER_H_
#define TSM_WORKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "data_type.h"
#include "tsm_data.h"
#include "tsm_chart.h"
#include "tsm_signalhandling.h"

void TsmModulerWorker();
// action
void ActionInPassive();
void ActionInFailureLighting();
void ActionInFailureNoLighting();
void ActionInStandby();
void ActionInMrmBothCtrl();
void ActionInMrmLatCtrl();
void ActionInMrc();
// extern 声明
// event声明
extern boolean IsMrmSystemFaultNotExist();  // event_1
extern boolean IsLightingConditionMeet();
extern boolean IsNoLightingConditionMeet();
extern boolean IsStandbyConditionMeet();
extern boolean IsStandbyConditionNotMeet();
extern boolean IsMrmBothCtrlConditionMeet();
extern boolean IsMrmLatCtrlConditionMeet();
extern boolean IsEnterMrcFromStandby();
extern boolean IsEnterMrcFromMrm();
extern boolean IsEnterLightingFromMrm();
extern boolean IsEnterNoLightingFromMrm();
extern boolean IsEnterLightingFromMrc();
extern boolean IsEnterNoLightingFromMrc();
extern void SignalHandling();
// 定义全局变量
StateMachine tsm;

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
    {MRM_LNG_LAT_CTRL, EVENT_6, MRM_LAT_CTRL},
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