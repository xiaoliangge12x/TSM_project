/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  MRM top state machine main structure definition
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_CHART_H_
#define TSM_CHART_H_

#include "tsm_parameter.h"
#include "base/sm_base.h"
#ifdef _NEED_LOG
#include "common.h"
#endif
// ---------------------------------- macro definition     --------------------------------------
#define TOTAL_TSM_TRANS_NUM   34U
#define TOTAL_TSM_EVENT_NUM   13U
#define TOTAL_TSM_STATE_NUM   10U
#define TIMESTAMP_MAX_NUM     3
#define EPSINON_TIME      ((float32)0.00001)
#define NS_IN_MS          ((float32)1000000.0)
#define UPPER_CYCLE       ((float32)21.0)
#define LOWER_CYCLE       ((float32)19.0)
// ---------------------------------- typedef              --------------------------------------
typedef enum
{
    EVENT_FAULT_NOT_EXIST = BASE_EVENT_TSM_START + 1,
    EVENT_LIGHTING,
    EVENT_NO_LIGHTING,
    EVENT_STANDBY,
    EVENT_NO_STANDBY,
    EVENT_MRM_BOTH_CTRL,
    EVENT_MRM_LAT_CTRL,
    EVENT_MRC,
    EVENT_TOR_BOTH_CTRL,
    EVENT_TOR_LAT_CTRL,
    EVENT_VEH_STANDSTILL,
    EVENT_FUNCTION_EXIT,
    EVENT_WAIT_EPB_RES
} EventID;

typedef enum
{
    PC_TOR = 0,
    PC_MRM,
    PC_EXIT,
} PCState;

// ---------------------------------- driving table        --------------------------------------
// static const StateTransit g_state_transit_table[TOTAL_TRANS_NUM];   // 此处非声明，而是定义
// static const Action       g_action;                                 // 此处非声明，而是定义
// static const TransitEvent g_event;                                  // 此处非声明，而是定义
// ---------------------------------- function declaration --------------------------------------
boolean ValidateRcvMsgTimeStamp(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsTimeStampLost(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp);
boolean IsTimeStampError(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp);
void WrapAndSend(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm, Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag);
boolean IsMrmSystemFaultNotExist(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm); 
boolean IsLightingConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsNoLightingConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsStandbyConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsStandbyConditionNotMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsMrmBothCtrlConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsMrmLatCtrlConditionMeet(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsCanEnterMrcFromStandby(const Dt_RECORD_CANGATE2TSM* rtu_DeCANGATE2TSM,
    const Dt_RECORD_Diag2TSM* rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsTorBothCtrlCondMeet(const Dt_RECORD_CANGATE2TSM* rtu_DeCANGATE2TSM,
    const Dt_RECORD_Diag2TSM* rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsTorLatCtrlCondMeet(const Dt_RECORD_CANGATE2TSM* rtu_DeCANGATE2TSM,
    const Dt_RECORD_Diag2TSM* rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsVehStandStillCondMeet(const Dt_RECORD_CANGATE2TSM* rtu_DeCANGATE2TSM,
    const Dt_RECORD_Diag2TSM* rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsFuncExitCondMeet(const Dt_RECORD_CANGATE2TSM* rtu_DeCANGATE2TSM,
    const Dt_RECORD_Diag2TSM* rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsWaitEpbSt(const Dt_RECORD_CANGATE2TSM* rtu_DeCANGATE2TSM,
    const Dt_RECORD_Diag2TSM* rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsDriverTakeOver();
boolean IsInTorFault(); 
void ActionInPassive();
void ActionInFailureLighting();
void ActionInFailureNoLighting();
void ActionInStandby();
void ActionInTorBothCtrl();
void ActionInTorLatCtrl();
void ActionInTorStand();
void ActionInMrmBothCtrl();
void ActionInMrmLatCtrl();
void ActionInMrc();
#endif