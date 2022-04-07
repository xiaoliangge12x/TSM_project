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
#include "sm_base.h"
// ----------------------- typedef         -------------------
typedef enum
{
    EVENT_NO_WARNING = BASE_EVENT_WARNING_START + 1,
    EVENT_TOR_LEVEL_1,
    EVENT_TOR_LEVEL_2,
    EVENT_TOR_LEVEL_3,
    EVENT_MRM_LEVEL_4,
    EVENT_MRM_LEVEL_5,
} WarningEventID;

typedef struct 
{
    /* data */
} WarningActionParam;

typedef struct 
{
    WarningState       warning_state;
    WarningActionParam warning_action_param;
} WarningSMParam;
// ----------------------- global variable -------------------------------
extern const StateMachine g_warning_state_machine;
// ----------------------- function declaration --------------------------
#endif