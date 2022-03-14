#ifndef TSM_CHART_H_
#define TSM_CHART_H_

#include "data_type.h"
#include "tsm_data.h"
#include "common.h"
#include "tsm_worker.h"

extern void TsmChartManager();
extern StateTransit* FindTrans(StateMachine* state_machine, 
    const enum EventID event_id_array[], const uint8 array_length);

// event声明
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

#endif