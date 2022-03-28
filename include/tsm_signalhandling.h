#ifndef TSM_SIGNAL_HANDLING_H_
#define TSM_SIGNAL_HANDLING_H_

#include "tsm_data.h"
#ifdef _NEED_LOG
#include "common.h"
#endif

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

// 总的信号处理入口
void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
                    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);


// 具体处理函数
// 判断 驾驶员注意力状态
void DrvrAttentionStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 纵向超越标志位
void LngOverrideFlagJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 刹车是否踩下
void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 驾驶员是否踩下油门
void DriverGasPedalAppliedJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);

// soc侧状态机跳转监控判断
void MonitorNdaStateTransition(const Dt_RECORD_Automaton_State* automaton_state);

void NdaStTransitNormalJudge(const Dt_RECORD_VehicleSignal2TSM* vehicle_signal, const Dt_RECORD_Soc_Info* soc_info);

static const NdaStMonitorInfo nda_st_transit_monitor_array[MONITOR_ARRAY_SIZE] = {
    {NDA_STANDBY_NORMAL, NDA_ACTIVE_HAND_FREE_NORMAL, STANDBY_HANDSFREE_NORMAL, TransitCondFromStandbyToHandsFreeNormal},
    {NDA_STANDBY_NORMAL, NDA_ACTIVE_HAND_ON_NORMAL, STANDBY_HANDSON_NORMAL, TransitCondFromStandbyToHandsOnNormal},
    {NDA_STANDBY_NORMAL, NDA_ACTIVE_HAND_FREE_STANDACTIVE, STANDBY_HANDSFREE_STANDACTIVE, TransitCondFromStandbyToHandsFreeStandActive},
    {NDA_ACTIVE_HAND_FREE_NORMAL, NDA_ACTIVE_HAND_FREE_STANDACTIVE, HANDSFREE_NORMAL_HANDSFREE_STANDACTIVE, TransitCondFromHandsFreeNormalToHandsFreeStandActive},
    {NDA_ACTIVE_HAND_FREE_NORMAL, NDA_LAT_LNG_OVERRIDE, HANDSFREE_NORMAL_BOTH_OVERRIDE, TransitCondFromHandsFreeNormalToBothOverride},
    {NDA_ACTIVE_HAND_FREE_NORMAL, NDA_LAT_OVERRIDE, HANDSFREE_NORMAL_LAT_OVERRIDE, TransitCondFromHandsFreeNormalToLatOverride},
    {NDA_ACTIVE_HAND_FREE_NORMAL, NDA_LNG_OVERRIDE_HANDS_ON, HANDSFREE_NORMAL_LNG_OVERRIDE, TransitCondFromHandsFreeNormalToLngOverride},
    {NDA_ACTIVE_HAND_FREE_NORMAL, NDA_ACTIVE_HAND_ON_NORMAL, HANDSFREE_NORMAL_HANDSON_NORMAL, TransitCondFromHandsFreeNormalToHandsOnNormal},
    {NDA_ACTIVE_HAND_FREE_STANDACTIVE, NDA_ACTIVE_HAND_FREE_NORMAL, HANDSFREE_STANDACTIVE_HANDSFREE_NORMAL, TransitCondFromHandsFreeStandActiveToHandsFreeNormal},
    {NDA_ACTIVE_HAND_FREE_STANDACTIVE, NDA_LNG_OVERRIDE_HANDS_ON, HANDSFREE_STANDACTIVE_LNG_OVERRIDE, TransitCondFromHandsFreeStandActiveToLngOverride},
    {NDA_ACTIVE_HAND_FREE_STANDACTIVE, NDA_LAT_LNG_OVERRIDE, HANDSFREE_STANDACTIVE_BOTH_OVERRIDE, TransitCondFromHandsFreeStandActiveToBothOverride},
    {NDA_ACTIVE_HAND_FREE_STANDACTIVE, NDA_LAT_OVERRIDE, HANDSFREE_STANDACTIVE_LAT_OVERRIDE, TransitCondFromHandsFreeStandActiveToLatOverride},
    {NDA_ACTIVE_HAND_FREE_STANDWAIT, NDA_LNG_OVERRIDE_HANDS_ON, HANDSFREE_STANDWAIT_LNG_OVERRIDE, TransitCondFromHandsFreeStandWaitToLngOverride},
    {NDA_ACTIVE_HAND_FREE_STANDWAIT, NDA_LAT_LNG_OVERRIDE, HANDSFREE_STANDWAIT_BOTH_OVERRIDE, TransitCondFromHandsFreeStandWaitToBothOverride},
    {NDA_ACTIVE_HAND_FREE_STANDWAIT, NDA_LAT_OVERRIDE, HANDSFREE_STANDWAIT_LAT_OVERRIDE, TransitCondFromHandsFreeStandWaitToLatOverride},
    {NDA_ACTIVE_HAND_ON_NORMAL, NDA_ACTIVE_HAND_ON_STANDACTIVE, HANDSON_NORMAL_HANDSON_STANDACTIVE, TransitCondFromHandsOnNormalToHandsOnStandActive},
    {NDA_ACTIVE_HAND_ON_NORMAL, NDA_LAT_LNG_OVERRIDE, HANDSON_NORMAL_BOTH_OVERRIDE, TransitCondFromHandsOnNormalToBothOverride},
    {NDA_ACTIVE_HAND_ON_NORMAL, NDA_LAT_OVERRIDE, HANDSON_NORMAL_LAT_OVERRIDE, TransitCondFromHandsOnNormalToLatOverride},
    {NDA_ACTIVE_HAND_ON_NORMAL, NDA_LNG_OVERRIDE_HANDS_ON, HANDSON_NORMAL_LNG_OVERRIDE, TransitCondFromHandsOnNormalToLngOverride},
    {NDA_ACTIVE_HAND_ON_NORMAL, NDA_ACTIVE_HAND_FREE_NORMAL, HANDSON_NORMAL_HANDSFREE_NORMAL, TransitCondFromHandsOnNormalToHandsFreeNormal},
    {NDA_ACTIVE_HAND_ON_STANDACTIVE, NDA_ACTIVE_HAND_ON_NORMAL, HANDSON_STANDACTIVE_HANDSON_NORMAL, TransitCondFromHandsOnStandActiveToHandsOnNormal},
    {NDA_ACTIVE_HAND_ON_STANDACTIVE, NDA_LNG_OVERRIDE_HANDS_ON, HANDSON_STANDACTIVE_LNG_OVERRIDE, TransitCondFromHandsOnStandActiveToLngOverride},
    {NDA_ACTIVE_HAND_ON_STANDACTIVE, NDA_LAT_LNG_OVERRIDE, HANDSON_STANDACTIVE_BOTH_OVERRIDE, TransitCondFromHandsOnStandActiveToBothOverride},
    {NDA_ACTIVE_HAND_ON_STANDACTIVE, NDA_LAT_OVERRIDE, HANDSON_STANDACTIVE_LAT_OVERRIDE, TransitCondFromHandsOnStandActiveToLatOverride},
    {NDA_ACTIVE_HAND_ON_STANDWAIT, NDA_LNG_OVERRIDE_HANDS_ON, HANDSON_STANDWAIT_LNG_OVERRIDE, TransitCondFromHandsOnStandWaitToLngOverride},
    {NDA_ACTIVE_HAND_ON_STANDWAIT, NDA_LAT_LNG_OVERRIDE, HANDSON_STANDWAIT_BOTH_OVERRIDE, TransitCondFromHandsOnStandWaitToBothOverride},
    {NDA_ACTIVE_HAND_ON_STANDWAIT, NDA_LAT_OVERRIDE, HANDSON_STANDWAIT_LAT_OVERRIDE, TransitCondFromHandsOnStandWaitToLatOverride},
    {NDA_LNG_OVERRIDE_HANDS_ON, NDA_ACTIVE_HAND_FREE_NORMAL, LNG_OVERRIDE_HANDSFREE_NORMAL, TransitCondFromLngOverrideToHandsFreeNormal},
    {NDA_LNG_OVERRIDE_HANDS_ON, NDA_ACTIVE_HAND_ON_NORMAL, LNG_OVERRIDE_HANDSON_NORMAL, TransitCondFromLngOverrideToHandsOnNormal},
    {NDA_LNG_OVERRIDE_HANDS_ON, NDA_LAT_LNG_OVERRIDE, LNG_OVERRIDE_BOTH_OVERRIDE, TransitCondFromLngOverrideToBothOverride},
    {NDA_LNG_OVERRIDE_HANDS_ON, NDA_LAT_OVERRIDE, LNG_OVERRIDE_LAT_OVERRIDE, TransitCondFromLngOverrideToLatOverride},
    {NDA_LAT_OVERRIDE, NDA_ACTIVE_HAND_FREE_NORMAL, LAT_OVERRIDE_HANDSFREE_NORMAL, TransitCondFromLatOverrideToHandsFreeNormal},
    {NDA_LAT_OVERRIDE, NDA_ACTIVE_HAND_ON_NORMAL, LAT_OVERRIDE_HANDSON_NORMAL, TransitCondFromLatOverrideToHandsOnNormal},
    {NDA_LAT_OVERRIDE, NDA_LAT_LNG_OVERRIDE, LAT_OVERRIDE_BOTH_OVERRIDE, TransitCondFromLatOverrideToBothOverride},
    {NDA_LAT_OVERRIDE, NDA_LNG_OVERRIDE_HANDS_ON, LAT_OVERRIDE_LNG_OVERRIDE, TransitCondFromLatOverrideToLngOverride},
    {NDA_LAT_LNG_OVERRIDE, NDA_ACTIVE_HAND_FREE_NORMAL, BOTH_OVERRIDE_HANDSFREE_NORMAL, TransitCondFromBothOverrideToHandsFreeNormal},
    {NDA_LAT_LNG_OVERRIDE, NDA_ACTIVE_HAND_ON_NORMAL, BOTH_OVERRIDE_HANDSON_NORMAL, TransitCondFromBothOverrideToHandsOnNormal},
    {NDA_LAT_LNG_OVERRIDE, NDA_LNG_OVERRIDE_HANDS_ON, BOTH_OVERRIDE_LNG_OVERRIDE, TransitCondFromBothOverrideToLngOverride},
    {NDA_LAT_LNG_OVERRIDE, NDA_LAT_OVERRIDE, BOTH_OVERRIDE_LAT_OVERRIDE, TransitCondFromBothOverrideToLatOverride}
};
#endif