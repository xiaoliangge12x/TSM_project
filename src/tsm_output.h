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

void WrapAndSend();
#endif