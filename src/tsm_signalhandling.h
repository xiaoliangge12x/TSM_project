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

#include "tsm_parameter.h"
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

// --------------------- function declaration -----------------------------------------------
void SignalHandling();

void RunDriverOperationCheck(const Veh_Sig* vehicle_signal);
void RunCommonConditionCheck(const Veh_Sig* veh_info);
void DrvrAttentionStJudge(const Veh_Sig *vehicle_signal);
boolean IsDriverNotFatigue();
void LngOverrideFlagJudge(const Veh_Sig *vehicle_signal);
void BrakeIsSetJudge(const Veh_Sig *vehicle_signal);
void BrakeInervationFlagJudge();
void DriverGasPedalAppliedJudge(const Veh_Sig *vehicle_signal);
void DriveHandTorqueOverrideStJudge(const Veh_Sig *vehicle_signal);
void NdaStTransitNormalJudge(const Veh_Sig* vehicle_signal, const Soc_Info* soc_info);

// user defined
void TorqueOverrideStJudgeWithHodDetection(const Veh_Sig *vehicle_signal);
void TorqueOverrideStJudgeWithoutHodDetection(const Veh_Sig *vehicle_signal);
void FlagSetWithTimeCount(const uint32 bit_no, const uint16 time_threshold_cnt, uint16* time_cnt);
#ifdef CONSUME_TIME
void FlagSetWithTime(const uint32 bit_no, const float32 time_threshold, sint64* time, uint8* time_flag);
#endif
#endif