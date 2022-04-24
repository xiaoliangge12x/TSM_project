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

// ----------------------------- macro definition -------------------------------------
#define MAX_FRAME_CNT      5
#define MONITOR_ARRAY_SIZE 39
// ----------------------------- typedef          -------------------------------------
typedef boolean (*NdaTransitCondition) ();

typedef enum
{
    BITNO_STANDBY_HANDSFREE_NORMAL = 0,
    BITNO_STANDBY_HANDSON_NORMAL,
    BITNO_STANDBY_HANDSFREE_STANDACTIVE,
    BITNO_HANDSFREE_NORMAL_HANDSFREE_STANDACTIVE,
    BITNO_HANDSFREE_NORMAL_BOTH_OVERRIDE,
    BITNO_HANDSFREE_NORMAL_LAT_OVERRIDE,
    BITNO_HANDSFREE_NORMAL_LNG_OVERRIDE,
    BITNO_HANDSFREE_NORMAL_HANDSON_NORMAL,
    BITNO_HANDSFREE_STANDACTIVE_HANDSFREE_NORMAL,
    BITNO_HANDSFREE_STANDACTIVE_LNG_OVERRIDE,
    BITNO_HANDSFREE_STANDACTIVE_BOTH_OVERRIDE,
    BITNO_HANDSFREE_STANDACTIVE_LAT_OVERRIDE,
    BITNO_HANDSFREE_STANDWAIT_LNG_OVERRIDE,
    BITNO_HANDSFREE_STANDWAIT_BOTH_OVERRIDE,
    BITNO_HANDSFREE_STANDWAIT_LAT_OVERRIDE,
} StandbyHandsFreeBitNo;

typedef enum
{
    BITNO_HANDSON_NORMAL_HANDSON_STANDACTIVE = 0,
    BITNO_HANDSON_NORMAL_BOTH_OVERRIDE,
    BITNO_HANDSON_NORMAL_LAT_OVERRIDE,
    BITNO_HANDSON_NORMAL_LNG_OVERRIDE,
    BITNO_HANDSON_NORMAL_HANDSFREE_NORMAL,
    BITNO_HANDSON_STANDACTIVE_HANDSON_NORMAL,
    BITNO_HANDSON_STANDACTIVE_LNG_OVERRIDE,
    BITNO_HANDSON_STANDACTIVE_BOTH_OVERRIDE,
    BITNO_HANDSON_STANDACTIVE_LAT_OVERRIDE,
    BITNO_HANDSON_STANDWAIT_LNG_OVERRIDE,
    BITNO_HANDSON_STANDWAIT_BOTH_OVERRIDE,
    BITNO_HANDSON_STANDWAIT_LAT_OVERRIDE,
} HandsOnBitNo;

typedef enum
{
    BITNO_LNG_OVERRIDE_HANDSFREE_NORMAL = 0,
    BITNO_LNG_OVERRIDE_HANDSON_NORMAL,
    BITNO_LNG_OVERRIDE_BOTH_OVERRIDE,
    BITNO_LNG_OVERRIDE_LAT_OVERRIDE,
    BITNO_LAT_OVERRIDE_HANDSFREE_NORMAL,
    BITNO_LAT_OVERRIDE_HANDSON_NORMAL,
    BITNO_LAT_OVERRIDE_BOTH_OVERRIDE,
    BITNO_LAT_OVERRIDE_LNG_OVERRIDE,
    BITNO_BOTH_OVERRIDE_HANDSFREE_NORMAL,
    BITNO_BOTH_OVERRIDE_HANDSON_NORMAL,
    BITNO_BOTH_OVERRIDE_LNG_OVERRIDE,
    BITNO_BOTH_OVERRIDE_LAT_OVERRIDE,
} OverrideBitNo;

typedef struct 
{
    NdaFunctionSt        start_st;
    NdaFunctionSt        next_st;
    NdaTransitEnableFlag transit_enable_flag;
    NdaTransitCondition  nda_transit_cond;
} NdaStMonitorInfo;

typedef struct
{
    uint32 monitor_standby_handsfree_bitfields;
    uint32 monitor_handson_bitfields;
    uint32 monitor_override_bitfields;
} MonitorBitfields;
// ----------------------------- driving table declaration ----------------------------
extern const NdaStMonitorInfo nda_st_transit_monitor_array[MONITOR_ARRAY_SIZE];  // 千万不能声明成static
extern MonitorBitfields       g_monitor_bitfields;
// ----------------------------- function declaration ---------------------------------
boolean TransitCondFromStandbyToHandsFreeNormal();
boolean TransitCondFromStandbyToHandsOnNormal();
boolean TransitCondFromStandbyToHandsFreeStandActive();
boolean TransitCondFromHandsFreeNormalToHandsFreeStandActive();
boolean TransitCondFromHandsFreeNormalToBothOverride();
boolean TransitCondFromHandsFreeNormalToLatOverride();
boolean TransitCondFromHandsFreeNormalToLngOverride();
boolean TransitCondFromHandsFreeNormalToHandsOnNormal();
boolean TransitCondFromHandsFreeStandActiveToHandsFreeNormal();
boolean TransitCondFromHandsFreeStandActiveToLngOverride();
boolean TransitCondFromHandsFreeStandActiveToBothOverride();
boolean TransitCondFromHandsFreeStandActiveToLatOverride();
boolean TransitCondFromHandsFreeStandWaitToLngOverride();
boolean TransitCondFromHandsFreeStandWaitToBothOverride();
boolean TransitCondFromHandsFreeStandWaitToLatOverride();
boolean TransitCondFromHandsOnNormalToHandsOnStandActive();
boolean TransitCondFromHandsOnNormalToBothOverride();
boolean TransitCondFromHandsOnNormalToLatOverride();
boolean TransitCondFromHandsOnNormalToLngOverride();
boolean TransitCondFromHandsOnNormalToHandsFreeNormal();
boolean TransitCondFromHandsOnStandActiveToHandsOnNormal();
boolean TransitCondFromHandsOnStandActiveToLngOverride();
boolean TransitCondFromHandsOnStandActiveToBothOverride();
boolean TransitCondFromHandsOnStandActiveToLatOverride();
boolean TransitCondFromHandsOnStandWaitToLngOverride();
boolean TransitCondFromHandsOnStandWaitToBothOverride();
boolean TransitCondFromHandsOnStandWaitToLatOverride();
boolean TransitCondFromLngOverrideToHandsFreeNormal();
boolean TransitCondFromLngOverrideToHandsOnNormal();
boolean TransitCondFromLngOverrideToBothOverride();
boolean TransitCondFromLngOverrideToLatOverride();
boolean TransitCondFromLatOverrideToHandsFreeNormal();
boolean TransitCondFromLatOverrideToHandsOnNormal();
boolean TransitCondFromLatOverrideToBothOverride();
boolean TransitCondFromLatOverrideToLngOverride();
boolean TransitCondFromBothOverrideToHandsFreeNormal();
boolean TransitCondFromBothOverrideToHandsOnNormal();
boolean TransitCondFromBothOverrideToLngOverride();
boolean TransitCondFromBothOverrideToLatOverride();

boolean IsNdaAvailable();
#endif