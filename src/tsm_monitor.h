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

typedef enum 
{
    BEFORE_ACTIVATION = 0,
    AFTER_ACTIVATION,
} CheckMoment;

// ----------------------------- driving table declaration ----------------------------
// ----------------------------- function declaration ---------------------------------
// total
void RunNdaTranistionMonitor(const Veh_Sig* veh_info, const Soc_Info* soc_info);
// condition
void CheckMonitorPrecondition(const Veh_Sig* veh_info, const Soc_Info* soc_info);
boolean ValidateNdaAvlCond(const Soc_Info* soc_info);
boolean IsRainFallSatisfy(const Veh_Sig* vehicle_signal);
boolean IsOddSatisfy(const CheckMoment activation_time, const Veh_Sig* vehicle_signal, const Soc_Info* soc_info);
boolean IsNdaPassiveVD(const Soc_Info* soc_info);
void CheckNdaPhaseInAvailable();
void CheckNdaNeedPhaseIn();

// monitor logic
void RunMonitorNdaTransitionLogic(const Soc_State* automaton_state, const Veh_Sig* veh_sig);
void MonitorNdaChangeFromStandbyToActive(const Soc_State* automaton_st, const Veh_Sig* veh_sig);
void MonitorNdaChangeFromActiveToOverride(const Soc_State* automaton_st);
void MonitorNdaIgnoreOverrideReq(const Soc_State* automaton_st);
void MonitorNdaChangeFromOverrideToActive(const Soc_State* automaton_st);
void MonitorNdaStuckInOverrideSt(const Soc_State* automaton_st);
void MonitorIcaUpgradeToNda(const Soc_State* automaton_st);
void MonitorNdaUnableToExit(const Soc_State* automaton_st);

// post handling
void PostHandleForTransitAbnormal(const boolean monitor_flag, const uint8 last_hand_torque_override_st, const char* log);
void PostHandleForStuckInSt(uint16* time_cnt, const CondJudge cond_judge, const uint16 time_cnt_threshold, const char* log);

// user-defined check
boolean CheckNdaInActiveSt(const Soc_State* automaton_st);
boolean CheckNdaInOverrideSt(const Soc_State* automaton_st);
boolean CheckIcaInActiveOrOverrideSt(const Soc_State* automaton_st);
boolean IsDriverNotLatOverride();
boolean IsDriverNotLngOverride();
boolean IsDriverNotLatOrLngOverride();
boolean IsDriverLatOrLngOverride();
boolean IsASActiveOrBrakeSet();
#endif