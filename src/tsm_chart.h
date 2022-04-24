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
#define TIMESTAMP_MAX_NUM     3
#define EPSINON_TIME          ((float32)0.00001)
#define NS_IN_MS              ((float32)1000000.0)
#define UPPER_CYCLE           ((float32)21.0)
#define LOWER_CYCLE           ((float32)19.0)
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
    EVENT_TOR_TO_MRM_BOTH,
    EVENT_TOR_TO_MRM_LAT,
    EVENT_MRM_BOTH_CTRL_SWITCH,
    EVENT_MRM_LAT_CTRL_SWITCH,
    EVENT_TOR_BOTH_CTRL_SWITCH,
    EVENT_TOR_LAT_CTRL_SWITCH,
    EVENT_VEH_STANDSTILL,
    EVENT_FUNCTION_EXIT,
    EVENT_WAIT_EPB_RES,
} EventID;

typedef enum
{
    PC_TOR = 0,
    PC_MRM,
    PC_EXIT,
} PCSt;

typedef enum
{
    BITNO_FAULT_NOT_EXIST = 0,
    BITNO_LIGHTING,
    BITNO_NO_LIGHTING,
    BITNO_STANDBY,
    BITNO_NO_STANDBY,
    BITNO_MRM_BOTH_CTRL,
    BITNO_MRM_LAT_CTRL,
    BITNO_MRC,
    BITNO_TOR_BOTH_CTRL,
    BITNO_TOR_LAT_CTRL,
    BITNO_TOR_TO_MRM_BOTH,
    BITNO_TOR_TO_MRM_LAT,
    BITNO_MRM_BOTH_CTRL_SWITCH,
    BITNO_MRM_LAT_CTRL_SWITCH,
    BITNO_TOR_BOTH_CTRL_SWITCH,
    BITNO_TOR_LAT_CTRL_SWITCH,
    BITNO_VEH_STANDSTILL,
    BITNO_FUNCTION_EXIT,
    BITNO_WAIT_EPB_RES,
} TsmEventBitNo;

typedef enum
{
    MRM_ST_TOR = 0,
    MRM_ST_ACTIVE,
    MRM_ST_INVALID,
} MrmStateToPlanning;

typedef enum
{
    CTRLARB_RESPOND_TO_SOC = 0,
    CTRLARB_RESPOND_TO_MCU,
} CtrlArbRequest;
// ---------------------------------- function declaration --------------------------------------
boolean ValidateRcvMsgTimeStamp(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsTimeStampLost(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp);
boolean IsTimeStampError(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp);

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

void RunTsmSit(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsMrmSystemFaultNotExist(); 
boolean IsLightingConditionMeet();
boolean IsNoLightingConditionMeet();
boolean IsStandbyConditionMeet();
boolean IsStandbyConditionNotMeet();
boolean IsMrmBothCtrlConditionMeet();
boolean IsMrmLatCtrlConditionMeet();
boolean IsCanEnterMrcFromStandby();
boolean IsTorBothCtrlCondMeet();
boolean IsTorLatCtrlCondMeet();
boolean IsVehStandStillCondMeet();
boolean IsFuncExitCondMeet();
boolean IsWaitEpbSt();
boolean IsSwitchToMrmBothCtrl();
boolean IsSwitchToMrmLatCtrl();
boolean IsSwitchToTorBothCtrl();
boolean IsSwitchToTorLatCtrl();
boolean IsTorToMrmBoth();
boolean IsTorToMrmLat();
boolean IsDriverTakeOver();
boolean IsInTorFault();
boolean IsNDAInActiveSt(const uint8 nda_st);
void SetCtrlType(const uint8 both_ctrl, const uint8 lat_ctrl);
void DoNoFunctionWork();
void OutputLatLngOverrideStatus(const OverrideSt lng_override_st, const OverrideSt lat_override_st);
void OutputMrmStatus(const MrmStateToPlanning mrm_st);
void OutputCtrlArbReq(const CtrlArbRequest ctrl_arb_req);

void ActionInPassive();
void ActionInStandby();   
void ActionInFailureLighting();
void ActionInFailureNoLighting();
void ActionInTorBothCtrl();
void ActionInTorLatCtrl();
void ActionInTorStand();  
void ActionInMrmBothCtrl();
void ActionInMrmLatCtrl();     
void ActionInMrc();
#endif