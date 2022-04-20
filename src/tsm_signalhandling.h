/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Signal Preprocess for TSM definition
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_SIGNAL_HANDLING_H_
#define TSM_SIGNAL_HANDLING_H_

#include "tsm_monitor.h"
#ifdef _NEED_LOG
#include "common.h"
#endif
// ---------------------- macro  ------------------------------------------------------------

// --------------------- typedef ------------------------------------------------------------
typedef enum 
{
    DRVR_DET_ST_UNAVAILABLE = 0, 
    DRVR_DET_ST_NOT_DETECTED,
    DRVR_DET_ST_EYE_DETECTED,
    DRVR_DET_ST_HEAD_DETECTED,
} DMSDrvrDetSt;

typedef enum 
{
    INT_RGN_UNAVAILABLE = 0,
    INT_RGN_WINDSHIELD,
    INT_RGN_REARVIEW_MIRROR,
    INT_RGN_DRVR_SIDEWINDOW,
    INT_RGN_PAX_SIDEWINDOW,
    INT_RGN_INST_CLUSTER,
    INT_RGN_INFOTAIMENT_DISPLAY,
    INT_RGN_DRVR_SIDEDOOR_PANEL,
    INT_RGN_DRVR_SIDEFOOTWELL,
    INT_RGN_PAX_SIDE_FOOTWELL,
    INT_RGN_PAX_SIDEDOOR_PANEL,
    INT_RGN_OTHER_AREA,
} DMSInterestsRegion;

typedef enum
{
    DRVR_FATIGUE_ST_UNAVAILABLE = 0x0U,    // 不可用
    DRVR_FATIGUE_ST_UNKNOWN     = 0x249U,  // 未知
    DRVR_FATIGUE_ST_ALERT       = 0x492U,  // 警惕
    DRVR_FATIGUE_ST_FATIGUE     = 0x6DBU,  // 疲惫
    DRVR_FATIGUE_ST_H_FATIGUE   = 0x924U,  // 非常疲惫
    DRVR_FATIGUE_ST_SU          = 0xB6DU,  // 怀疑反应迟钝
    DRVR_FATIGUE_ST_CU          = 0xDB6U,  // 确定反应迟钝
    DRVR_FATIGUE_ST_NOT_USED    = 0xFFFU,  // 未使用
} DMSL3DriverFatigueSt;

typedef enum
{
    BRK_APPLIED_ST_NOT_INITIALIZED = 0,
    BRK_APPLIED_ST_NORMAL,
    BRK_APPLIED_ST_FAULTY,
    BRK_APPLIED_ST_RESERVED
} BrkPedalAppliedSt;

typedef enum AccDriverOrvd
{
    NO_OVERRIDE = 0,
    DRIVER_OVERRIDE
};

typedef enum BrkPedalApplied
{
    BRAKE_PEDAL_NOT_APPLIED = 0,
    BRAKE_PEDAL_APPLIED
};
typedef struct 
{
    DMSL3DriverFatigueSt driver_fatigue_st;
    DMSDrvrDetSt         driver_detect_st;
    DMSInterestsRegion   interset_region;
    DrvrAttentionSt      driver_attention_st;
} DrvrAttStResult;

typedef struct 
{
    uint8   flag_set_val;
    uint8   flag_unset_val;
    float32 time_threshold;
} VarValueInTime;

typedef struct 
{
    uint8  flag_set_val;
    uint8  flag_unset_val;
    uint16 time_threshold_cnt;
} VarValue;

// --------------------- function declaration -----------------------------------------------
void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
void DrvrAttentionStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void CheckNdaAvailableSt(const Dt_RECORD_Soc_Info* soc_info);
boolean ValidateNdaAvlCond(const Dt_RECORD_Soc_Info* soc_info);
boolean IsDriverNotFatigue();
void LngOverrideFlagJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void BrakeInervationFlagJudge();
void DriverGasPedalAppliedJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void DriveHandTorqueOverrideStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void MonitorNdaStateTransition(const Dt_RECORD_Automaton_State* automaton_state);
void NdaStTransitNormalJudge(const Dt_RECORD_VehicleSignal2TSM* vehicle_signal, const Dt_RECORD_Soc_Info* soc_info);
void TorqueOverrideStJudgeWithHodDetection(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void TorqueOverrideStJudgeWithoutHodDetection(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void FlagSetWithTimeCount(uint8* flag_set_var, uint16* time_cnt, const VarValue* var_value);
void FlagSetWithTime(uint8* flag_set_var, const sint64 time, const uint8 time_flag, const VarValueInTime* var_value);
void CheckNdaActiveTransitCond(const Dt_RECORD_Soc_Info* soc_info);

boolean IsInMCUMRMActiveSt();

void SetLaneChangeSt(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM);
#endif