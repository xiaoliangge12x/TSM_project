/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  TSM warning sm declaration
 *  \author zxl
 *  \date   2022-04-06
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_WARNING_H_
#define TSM_WARNING_H_

#include "tsm_parameter.h"
#include "base/sm_base.h"
// -----------------------  macro    -----------------------------------------
// ----------------------- typedef         -----------------------------------
typedef enum
{
    EVENT_CHECK_IN_PASSIVE = BASE_EVENT_WARNING_START + 1,
    EVENT_COME_LEVEL_1,
    EVENT_COME_LEVEL_2,
    EVENT_COME_LEVEL_3,
    EVENT_CHECK_IN_MRM,
    EVENT_CHECK_IN_MRC,
    EVENT_RAMPUP_LEVEL_2,
    EVENT_RAMPUP_LEVEL_3,
    EVENT_RAMPUP_LEVEL_3_WITH_TIME,
    EVENT_RAMPUP_LEVEL_4,
} WarningEventID;

typedef enum
{
    BITNO_TOR_LEVEL_1_COME = 0,
    BITNO_TOR_LEVEL_2_COME,
    BITNO_TOR_LEVEL_3_COME,
    BITNO_CHECK_MRM_ST,
    BITNO_CHECK_MRC_ST,
    BITNO_CHECK_PASSIVE_ST,
    BITNO_RAMPUP_TOR_LEVEL_2,
    BITNO_RAMPUP_TOR_LEVEL_3,
    BITNO_RAMPUP_TOR_LEVEL_3_2,
    BITNO_RAMPUP_MRM_LEVEL_4
} WarningEventBitNo;

typedef struct 
{
    uint8 warning_level;    // for build pass
} WarningActionParam;

typedef struct 
{
    WarningState       warning_state;
    WarningActionParam warning_action_param;
} WarningSMParam;
// ----------------------- global variable -------------------------------
extern const StateMachine   g_warning_state_machine;
extern WarningSMParam       g_warning_sm;
// ----------------------- function declaration --------------------------
void RunWarningSit();

void ActionInNoWarning();
void ActionInWarningTorLevel1();
void ActionInWarningTorLevel2();
void ActionInWarningTorLevel3();
void ActionInWarningMrmLevel4();
void ActionInWarningMrmLevel5();

boolean TorLevel1WarningCome();
boolean TorLevel2WaningCome();
boolean TorLevel3WarningCome();
boolean CheckInMrmSt();
boolean CheckInMrcSt();
boolean CheckInPassiveSt();
boolean RampUpToTorLevel2();
boolean RampUpToTorLevel3WithTiming();
boolean RampUpToTorLevel3WithoutTiming();
boolean RampUpToMrmLevel4WithTiming();


#endif