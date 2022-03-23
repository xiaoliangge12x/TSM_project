/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  MRM top state machine main structure definition
 *  \author zengxiaoliang (zengxiaoliang@holomatic.com)
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_CHART_H_
#define TSM_CHART_H_

#include "tsm_data.h"
#ifdef _NEED_LOG
#include "common.h"
#endif

void TsmChartManager();
StateTransit* FindTrans(StateMachine* state_machine, 
    const enum EventID event_id_array[], const uint8 array_length);
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
    Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE);

// event声明， 条件判断中可以不再加状态，实现状态和条件的解耦(如果真要状态，那么可以在中间条件里增加状态信号)
boolean IsMrmSystemFaultNotExist(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);  // event_1
boolean IsLightingConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsNoLightingConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsStandbyConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsStandbyConditionNotMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsMrmBothCtrlConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsMrmLatCtrlConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsEnterMrcFromStandby(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsEnterMrcFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsEnterLightingFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsEnterNoLightingFromMrm(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);

// user define
boolean IsDriverTakeOver();

// action 声明
void ActionInPassive();
void ActionInFailureLighting();
void ActionInFailureNoLighting();
void ActionInStandby();
void ActionInMrmBothCtrl();
void ActionInMrmLatCtrl();
void ActionInMrc();

static const StateTransit state_transit_array[TOTAL_TRANS_NUM] = {
    {PASSIVE, EVENT_STANDBY, STANDBY},
    {PASSIVE, EVENT_LIGHTING, FAILURE_LIGHTING},
    {PASSIVE, EVENT_NO_LIGHTING, FAILURE_NO_LIGHTING},
    {FAILURE_LIGHTING, EVENT_NO_LIGHTING, FAILURE_NO_LIGHTING},
    {FAILURE_LIGHTING, EVENT_FAULT_NOT_EXIST, PASSIVE},
    {FAILURE_NO_LIGHTING, EVENT_LIGHTING, FAILURE_LIGHTING},
    {FAILURE_NO_LIGHTING, EVENT_FAULT_NOT_EXIST, PASSIVE},
    {STANDBY, EVENT_NO_STANDBY, PASSIVE},
    {STANDBY, EVENT_LIGHTING, FAILURE_LIGHTING},
    {STANDBY, EVENT_NO_LIGHTING, FAILURE_NO_LIGHTING},
    {STANDBY, EVENT_MRM_BOTH_CTRL, MRM_LNG_LAT_CTRL},
    {STANDBY, EVENT_MRM_LAT_CTRL, MRM_LAT_CTRL},
    {STANDBY, EVENT_MRC, MRC},
    {MRM_LNG_LAT_CTRL, EVENT_MRM_LAT_CTRL, MRM_LAT_CTRL},
    {MRM_LNG_LAT_CTRL, EVENT_MRC_FROM_MRM, MRC},
    {MRM_LNG_LAT_CTRL, EVENT_LIGHTING_FROM_MRM, FAILURE_LIGHTING},
    {MRM_LNG_LAT_CTRL, EVENT_NO_LIGHTING_FROM_MRM, FAILURE_NO_LIGHTING},
    {MRM_LAT_CTRL, EVENT_MRM_BOTH_CTRL, MRM_LNG_LAT_CTRL},
    {MRM_LAT_CTRL, EVENT_MRC_FROM_MRM, MRC},
    {MRM_LAT_CTRL, EVENT_LIGHTING_FROM_MRM, FAILURE_LIGHTING},
    {MRM_LAT_CTRL, EVENT_NO_LIGHTING_FROM_MRM, FAILURE_NO_LIGHTING},
    {MRC, EVENT_LIGHTING_FROM_MRM, FAILURE_LIGHTING},
    {MRC, EVENT_NO_LIGHTING_FROM_MRM, FAILURE_NO_LIGHTING}
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
    IsEnterNoLightingFromMrm
};

#endif