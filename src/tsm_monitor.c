/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Monitor SOC NDA state transit implementation
 *  \author zengxiaoliang (zengxiaoliang@holomatic.com)
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_monitor.h"

boolean TransitCondFromStandbyToHandsFreeNormal() {
    return true;
}

boolean TransitCondFromStandbyToHandsOnNormal() {
    return false;
}

boolean TransitCondFromStandbyToHandsFreeStandActive() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToHandsFreeStandActive() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToBothOverride() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToLatOverride() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToLngOverride() {
    return false;
}

boolean TransitCondFromHandsFreeNormalToHandsOnNormal() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToLngOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToBothOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandActiveToLatOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandWaitToLngOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandWaitToBothOverride() {
    return false;
}

boolean TransitCondFromHandsFreeStandWaitToLatOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToHandsOnStandActive() {
    return false;
}

boolean TransitCondFromHandsOnNormalToBothOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToLatOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToLngOverride() {
    return false;
}

boolean TransitCondFromHandsOnNormalToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToHandsOnNormal() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToLngOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToBothOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandActiveToLatOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandWaitToLngOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandWaitToBothOverride() {
    return false;
}

boolean TransitCondFromHandsOnStandWaitToLatOverride() {
    return false;
}

boolean TransitCondFromLngOverrideToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromLngOverrideToHandsOnNormal() {
    return false;
}

boolean TransitCondFromLngOverrideToBothOverride() {
    return false;
}

boolean TransitCondFromLngOverrideToLatOverride() {
    return false;
}

boolean TransitCondFromLatOverrideToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromLatOverrideToHandsOnNormal() {
    return false;
}

boolean TransitCondFromLatOverrideToBothOverride() {
    return false;
}

boolean TransitCondFromLatOverrideToLngOverride() {
    return false;
}

boolean TransitCondFromBothOverrideToHandsFreeNormal() {
    return false;
}

boolean TransitCondFromBothOverrideToHandsOnNormal() {
    return false;
}

boolean TransitCondFromBothOverrideToLngOverride() {
    return false;
}

boolean TransitCondFromBothOverrideToLatOverride() {
    return false;
}