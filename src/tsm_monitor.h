/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Monitor SOC NDA state transit definition
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_MONITOR_H_
#define TSM_MONITOR_H_

#include "tsm_parameter.h"
#include "tsm_output.h"

// ----------------------------- macro definition -------------------------------------
#define MONITOR_ARRAY_SIZE 39
#define MAX_LOG_LEN        50
// ----------------------------- typedef          -------------------------------------
typedef boolean (*CondJudge)();

typedef struct
{
    uint8    current_st;
    boolean* enter_flag;
    char     log[MAX_LOG_LEN];
} AtomicData;
// ----------------------------- driving table declaration ----------------------------
// ----------------------------- function declaration ---------------------------------

void RunMonitorNdaTransitionLogic(const Dt_RECORD_Automaton_State* automaton_state, const Dt_RECORD_VehicleSignal2TSM* veh_sig);

void MonitorNdaChangeFromStandbyToActive(const Dt_RECORD_Automaton_State* automaton_st, const Dt_RECORD_VehicleSignal2TSM* veh_sig);

void MonitorNdaChangeFromActiveToOverride(const Dt_RECORD_Automaton_State* automaton_st);

void MonitorNdaIgnoreOverrideReq(const Dt_RECORD_Automaton_State* automaton_st);

void MonitorNdaChangeFromOverrideToActive(const Dt_RECORD_Automaton_State* automaton_st);

void MonitorNdaStuckInOverrideSt(const Dt_RECORD_Automaton_State* automaton_st);

void MonitorIcaUpgradeToNda(const Dt_RECORD_Automaton_State* automaton_st);

void MonitorNdaUnableToExit(const Dt_RECORD_Automaton_State* automaton_st);

boolean CheckNdaInActiveSt(const Dt_RECORD_Automaton_State* automaton_st);

boolean CheckNdaInOverrideSt(const Dt_RECORD_Automaton_State* automaton_st);

boolean CheckIcaInActiveOrOverrideSt(const Dt_RECORD_Automaton_State* automaton_st);

void PostHandleForTransitAbnormal(const boolean monitor_flag, const uint8 last_hand_torque_override_st, const char* log);

void PostHandleForStuckInSt(uint16* time_cnt, const CondJudge cond_judge, const uint16 time_cnt_threshold, const char* log);

boolean IsDriverNotLatOverride();

boolean IsDriverNotLngOverride();

boolean IsDriverNotLatOrLngOverride();

boolean IsDriverLatOrLngOverride();

boolean IsASActiveOrBrakeSet();
#endif