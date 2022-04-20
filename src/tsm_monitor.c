/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Monitor SOC NDA state transit implementation
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_monitor.h"

// ------------------------------ driving table Initilize ----------------------------------------
const NdaStMonitorInfo nda_st_transit_monitor_array[MONITOR_ARRAY_SIZE] = 
{
    {NDA_STANDBY_NORMAL,                 NDA_ACTIVE_HANDS_FREE_NORMAL,       STANDBY_HANDSFREE_NORMAL, 
        TransitCondFromStandbyToHandsFreeNormal},
    {NDA_STANDBY_NORMAL,                 NDA_ACTIVE_HANDS_ON_NORMAL,         STANDBY_HANDSON_NORMAL, 
        TransitCondFromStandbyToHandsOnNormal},
    {NDA_STANDBY_NORMAL,                 NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE, STANDBY_HANDSFREE_STANDACTIVE, 
        TransitCondFromStandbyToHandsFreeStandActive},
    {NDA_ACTIVE_HANDS_FREE_NORMAL,       NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE, HANDSFREE_NORMAL_HANDSFREE_STANDACTIVE, 
        TransitCondFromHandsFreeNormalToHandsFreeStandActive},
    {NDA_ACTIVE_HANDS_FREE_NORMAL,       NDA_LNG_LAT_OVERRIDE,               HANDSFREE_NORMAL_BOTH_OVERRIDE, 
        TransitCondFromHandsFreeNormalToBothOverride},
    {NDA_ACTIVE_HANDS_FREE_NORMAL,       NDA_LAT_OVERRIDE,                   HANDSFREE_NORMAL_LAT_OVERRIDE, 
        TransitCondFromHandsFreeNormalToLatOverride},
    {NDA_ACTIVE_HANDS_FREE_NORMAL,       NDA_LNG_OVERRIDE,                   HANDSFREE_NORMAL_LNG_OVERRIDE, 
        TransitCondFromHandsFreeNormalToLngOverride},
    {NDA_ACTIVE_HANDS_FREE_NORMAL,       NDA_ACTIVE_HANDS_ON_NORMAL,         HANDSFREE_NORMAL_HANDSON_NORMAL, 
        TransitCondFromHandsFreeNormalToHandsOnNormal},
    {NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE, NDA_ACTIVE_HANDS_FREE_NORMAL,       HANDSFREE_STANDACTIVE_HANDSFREE_NORMAL, 
        TransitCondFromHandsFreeStandActiveToHandsFreeNormal},
    {NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE, NDA_LNG_OVERRIDE,                   HANDSFREE_STANDACTIVE_LNG_OVERRIDE, 
        TransitCondFromHandsFreeStandActiveToLngOverride},
    {NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE, NDA_LNG_LAT_OVERRIDE,               HANDSFREE_STANDACTIVE_BOTH_OVERRIDE, 
        TransitCondFromHandsFreeStandActiveToBothOverride},
    {NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE, NDA_LAT_OVERRIDE,                   HANDSFREE_STANDACTIVE_LAT_OVERRIDE, 
        TransitCondFromHandsFreeStandActiveToLatOverride},
    {NDA_ACTIVE_HANDS_FREE_STAND_WAIT,   NDA_LNG_OVERRIDE,                   HANDSFREE_STANDWAIT_LNG_OVERRIDE, 
        TransitCondFromHandsFreeStandWaitToLngOverride},
    {NDA_ACTIVE_HANDS_FREE_STAND_WAIT,   NDA_LNG_LAT_OVERRIDE,               HANDSFREE_STANDWAIT_BOTH_OVERRIDE, 
        TransitCondFromHandsFreeStandWaitToBothOverride},
    {NDA_ACTIVE_HANDS_FREE_STAND_WAIT,   NDA_LAT_OVERRIDE,                   HANDSFREE_STANDWAIT_LAT_OVERRIDE, 
        TransitCondFromHandsFreeStandWaitToLatOverride},
    {NDA_ACTIVE_HANDS_ON_NORMAL,         NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,   HANDSON_NORMAL_HANDSON_STANDACTIVE, 
        TransitCondFromHandsOnNormalToHandsOnStandActive},
    {NDA_ACTIVE_HANDS_ON_NORMAL,         NDA_LNG_LAT_OVERRIDE,               HANDSON_NORMAL_BOTH_OVERRIDE, 
        TransitCondFromHandsOnNormalToBothOverride},
    {NDA_ACTIVE_HANDS_ON_NORMAL,         NDA_LAT_OVERRIDE,                   HANDSON_NORMAL_LAT_OVERRIDE, 
        TransitCondFromHandsOnNormalToLatOverride},
    {NDA_ACTIVE_HANDS_ON_NORMAL,         NDA_LNG_OVERRIDE,                   HANDSON_NORMAL_LNG_OVERRIDE, 
        TransitCondFromHandsOnNormalToLngOverride},
    {NDA_ACTIVE_HANDS_ON_NORMAL,         NDA_ACTIVE_HANDS_FREE_NORMAL,       HANDSON_NORMAL_HANDSFREE_NORMAL, 
        TransitCondFromHandsOnNormalToHandsFreeNormal},
    {NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,   NDA_ACTIVE_HANDS_ON_NORMAL,         HANDSON_STANDACTIVE_HANDSON_NORMAL, 
        TransitCondFromHandsOnStandActiveToHandsOnNormal},
    {NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,   NDA_LNG_OVERRIDE,                   HANDSON_STANDACTIVE_LNG_OVERRIDE, 
        TransitCondFromHandsOnStandActiveToLngOverride},
    {NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,   NDA_LNG_LAT_OVERRIDE,               HANDSON_STANDACTIVE_BOTH_OVERRIDE, 
        TransitCondFromHandsOnStandActiveToBothOverride},
    {NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,   NDA_LAT_OVERRIDE,                   HANDSON_STANDACTIVE_LAT_OVERRIDE, 
        TransitCondFromHandsOnStandActiveToLatOverride},
    {NDA_ACTIVE_HANDS_ON_STAND_WAIT,     NDA_LNG_OVERRIDE,                   HANDSON_STANDWAIT_LNG_OVERRIDE, 
        TransitCondFromHandsOnStandWaitToLngOverride},
    {NDA_ACTIVE_HANDS_ON_STAND_WAIT,     NDA_LNG_LAT_OVERRIDE,               HANDSON_STANDWAIT_BOTH_OVERRIDE, 
        TransitCondFromHandsOnStandWaitToBothOverride},
    {NDA_ACTIVE_HANDS_ON_STAND_WAIT,     NDA_LAT_OVERRIDE,                   HANDSON_STANDWAIT_LAT_OVERRIDE, 
        TransitCondFromHandsOnStandWaitToLatOverride},
    {NDA_LNG_OVERRIDE,                   NDA_ACTIVE_HANDS_FREE_NORMAL,       LNG_OVERRIDE_HANDSFREE_NORMAL, 
        TransitCondFromLngOverrideToHandsFreeNormal},
    {NDA_LNG_OVERRIDE,                   NDA_ACTIVE_HANDS_ON_NORMAL,         LNG_OVERRIDE_HANDSON_NORMAL, 
        TransitCondFromLngOverrideToHandsOnNormal},
    {NDA_LNG_OVERRIDE,                   NDA_LNG_LAT_OVERRIDE,               LNG_OVERRIDE_BOTH_OVERRIDE, 
        TransitCondFromLngOverrideToBothOverride},
    {NDA_LNG_OVERRIDE,                   NDA_LAT_OVERRIDE,                   LNG_OVERRIDE_LAT_OVERRIDE, 
        TransitCondFromLngOverrideToLatOverride},
    {NDA_LAT_OVERRIDE,                   NDA_ACTIVE_HANDS_FREE_NORMAL,       LAT_OVERRIDE_HANDSFREE_NORMAL, 
        TransitCondFromLatOverrideToHandsFreeNormal},
    {NDA_LAT_OVERRIDE,                   NDA_ACTIVE_HANDS_ON_NORMAL,         LAT_OVERRIDE_HANDSON_NORMAL, 
        TransitCondFromLatOverrideToHandsOnNormal},
    {NDA_LAT_OVERRIDE,                   NDA_LNG_LAT_OVERRIDE,               LAT_OVERRIDE_BOTH_OVERRIDE, 
        TransitCondFromLatOverrideToBothOverride},
    {NDA_LAT_OVERRIDE,                   NDA_LNG_OVERRIDE,                   LAT_OVERRIDE_LNG_OVERRIDE, 
        TransitCondFromLatOverrideToLngOverride},
    {NDA_LNG_LAT_OVERRIDE,               NDA_ACTIVE_HANDS_FREE_NORMAL,       BOTH_OVERRIDE_HANDSFREE_NORMAL, 
        TransitCondFromBothOverrideToHandsFreeNormal},
    {NDA_LNG_LAT_OVERRIDE,               NDA_ACTIVE_HANDS_ON_NORMAL,         BOTH_OVERRIDE_HANDSON_NORMAL, 
        TransitCondFromBothOverrideToHandsOnNormal},
    {NDA_LNG_LAT_OVERRIDE,               NDA_LNG_OVERRIDE,                   BOTH_OVERRIDE_LNG_OVERRIDE, 
        TransitCondFromBothOverrideToLngOverride},
    {NDA_LNG_LAT_OVERRIDE,               NDA_LAT_OVERRIDE,                   BOTH_OVERRIDE_LAT_OVERRIDE, 
        TransitCondFromBothOverrideToLatOverride}
};
// ----------------------------------- function -------------------------------------------------
boolean TransitCondFromStandbyToHandsFreeNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_STANDBY_HANDSFREE_NORMAL);
}

boolean TransitCondFromStandbyToHandsOnNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_STANDBY_HANDSON_NORMAL);
}

boolean TransitCondFromStandbyToHandsFreeStandActive() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_STANDBY_HANDSFREE_STANDACTIVE);
}

boolean TransitCondFromHandsFreeNormalToHandsFreeStandActive() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_NORMAL_HANDSFREE_STANDACTIVE);
}

boolean TransitCondFromHandsFreeNormalToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_NORMAL_BOTH_OVERRIDE);
}

boolean TransitCondFromHandsFreeNormalToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_NORMAL_LAT_OVERRIDE);
}

boolean TransitCondFromHandsFreeNormalToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_NORMAL_LNG_OVERRIDE);
}

boolean TransitCondFromHandsFreeNormalToHandsOnNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_NORMAL_HANDSON_NORMAL);
}

boolean TransitCondFromHandsFreeStandActiveToHandsFreeNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_STANDACTIVE_HANDSFREE_NORMAL);
}

boolean TransitCondFromHandsFreeStandActiveToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_STANDACTIVE_LNG_OVERRIDE);
}

boolean TransitCondFromHandsFreeStandActiveToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_STANDACTIVE_BOTH_OVERRIDE);
}

boolean TransitCondFromHandsFreeStandActiveToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_STANDACTIVE_LAT_OVERRIDE);
}

boolean TransitCondFromHandsFreeStandWaitToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_STANDWAIT_LNG_OVERRIDE);
}

boolean TransitCondFromHandsFreeStandWaitToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_STANDWAIT_BOTH_OVERRIDE);
}

boolean TransitCondFromHandsFreeStandWaitToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_standby_handsfree_bitfields, BITNO_HANDSFREE_STANDWAIT_LAT_OVERRIDE);
}

boolean TransitCondFromHandsOnNormalToHandsOnStandActive() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_NORMAL_HANDSON_STANDACTIVE);
}

boolean TransitCondFromHandsOnNormalToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_NORMAL_BOTH_OVERRIDE);
}

boolean TransitCondFromHandsOnNormalToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_NORMAL_LAT_OVERRIDE);
}

boolean TransitCondFromHandsOnNormalToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_NORMAL_LNG_OVERRIDE);
}

boolean TransitCondFromHandsOnNormalToHandsFreeNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_NORMAL_HANDSFREE_NORMAL);
}

boolean TransitCondFromHandsOnStandActiveToHandsOnNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_STANDACTIVE_HANDSON_NORMAL);
}

boolean TransitCondFromHandsOnStandActiveToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_STANDACTIVE_LNG_OVERRIDE);
}

boolean TransitCondFromHandsOnStandActiveToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_STANDACTIVE_BOTH_OVERRIDE);
}

boolean TransitCondFromHandsOnStandActiveToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_STANDACTIVE_LAT_OVERRIDE);
}

boolean TransitCondFromHandsOnStandWaitToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_STANDWAIT_LNG_OVERRIDE);
}

boolean TransitCondFromHandsOnStandWaitToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_STANDWAIT_BOTH_OVERRIDE);
}

boolean TransitCondFromHandsOnStandWaitToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_handson_bitfields, BITNO_HANDSON_STANDWAIT_LAT_OVERRIDE);
}

boolean TransitCondFromLngOverrideToHandsFreeNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LNG_OVERRIDE_HANDSFREE_NORMAL);
}

boolean TransitCondFromLngOverrideToHandsOnNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LNG_OVERRIDE_HANDSON_NORMAL);
}

boolean TransitCondFromLngOverrideToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LNG_OVERRIDE_BOTH_OVERRIDE);
}

boolean TransitCondFromLngOverrideToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LNG_OVERRIDE_LAT_OVERRIDE);
}

boolean TransitCondFromLatOverrideToHandsFreeNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LAT_OVERRIDE_HANDSFREE_NORMAL);
}

boolean TransitCondFromLatOverrideToHandsOnNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LAT_OVERRIDE_HANDSON_NORMAL);
}

boolean TransitCondFromLatOverrideToBothOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LAT_OVERRIDE_BOTH_OVERRIDE);
}

boolean TransitCondFromLatOverrideToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_LAT_OVERRIDE_LNG_OVERRIDE);
}

boolean TransitCondFromBothOverrideToHandsFreeNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_BOTH_OVERRIDE_HANDSFREE_NORMAL);
}

boolean TransitCondFromBothOverrideToHandsOnNormal() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_BOTH_OVERRIDE_HANDSON_NORMAL);
}

boolean TransitCondFromBothOverrideToLngOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_BOTH_OVERRIDE_LNG_OVERRIDE);
}

boolean TransitCondFromBothOverrideToLatOverride() 
{
    return IsBitSet(g_monitor_bitfields.monitor_override_bitfields, BITNO_BOTH_OVERRIDE_LAT_OVERRIDE);
}