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

typedef struct 
{
    NdaFunctionSt        start_st;
    NdaFunctionSt        next_st;
    NdaTransitEnableFlag transit_enable_flag;
    NdaTransitCondition  nda_transit_cond;
} NdaStMonitorInfo;
// ----------------------------- driving table declaration ----------------------------
static const NdaStMonitorInfo nda_st_transit_monitor_array[MONITOR_ARRAY_SIZE];
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
#endif