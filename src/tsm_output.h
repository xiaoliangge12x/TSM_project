/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  tsm output declaration
 *  \author zxl
 *  \date   2022-04-26
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_OUTPUT_H_
#define TSM_OUTPUT_H_

// --------------------------   #include -----------------------------------
#include "tsm_parameter.h"

// --------------------------   macro    -----------------------------------


// --------------------------   typedef  -----------------------------------
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

// --------------------------   func     -----------------------------------
//  function pointer
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

// Output Function
void DoNoFunctionWork();

void OutputLatLngOverrideStatus(const OverrideSt lng_override_st, const OverrideSt lat_override_st);
void OutputMrmStatus(const MrmStateToPlanning mrm_st);
void OutputCtrlArbReq(const CtrlArbRequest ctrl_arb_req);

void WrapAndSend(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, const Dt_RECORD_DecisionArbitrator2TSM *rtu_DeDecisionArbitrator2TSM, 
    const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm, Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag, Dt_RECORD_TSM2HMI *rty_DeTSM2HMI, Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE);
#endif