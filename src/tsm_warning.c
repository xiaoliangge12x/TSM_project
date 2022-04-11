/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  TSM warning sm definition
 *  \author zxl
 *  \date   2022-04-06
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_warning.h"

// ------------------------------------ global variable definition -------------------------------
static uint16      g_warning_signal_bitfileds = 0U;
WarningSMParam     g_warning_sm;
const StateMachine g_warning_state_machine = 
{
    .state_transit_table = 
    {
        {NO_WARNING,          EVENT_COME_LEVEL_1,             WARNING_TOR_LEVEL_1},
        {NO_WARNING,          EVENT_COME_LEVEL_2,             WARNING_TOR_LEVEL_2},
        {NO_WARNING,          EVENT_COME_LEVEL_3,             WARNING_TOR_LEVEL_3},
        {NO_WARNING,          EVENT_CHECK_IN_MRM,             WARNING_MRM_LEVEL_4},
        {NO_WARNING,          EVENT_CHECK_IN_MRC,             WARNING_MRM_LEVEL_5},
        {WARNING_TOR_LEVEL_1, EVENT_RAMPUP_LEVEL_2,           WARNING_TOR_LEVEL_2},
        {WARNING_TOR_LEVEL_1, EVENT_RAMPUP_LEVEL_3,           WARNING_TOR_LEVEL_3},
        {WARNING_TOR_LEVEL_1, EVENT_CHECK_IN_MRM,             WARNING_MRM_LEVEL_4},
        {WARNING_TOR_LEVEL_1, EVENT_CHECK_IN_PASSIVE,         NO_WARNING},
        {WARNING_TOR_LEVEL_2, EVENT_RAMPUP_LEVEL_3_WITH_TIME, WARNING_TOR_LEVEL_3},
        {WARNING_TOR_LEVEL_2, EVENT_CHECK_IN_MRM,             WARNING_MRM_LEVEL_4},
        {WARNING_TOR_LEVEL_2, EVENT_CHECK_IN_PASSIVE,         NO_WARNING},
        {WARNING_TOR_LEVEL_3, EVENT_RAMPUP_LEVEL_4,           WARNING_MRM_LEVEL_4},
        {WARNING_TOR_LEVEL_3, EVENT_CHECK_IN_PASSIVE,         NO_WARNING},
        {WARNING_MRM_LEVEL_4, EVENT_CHECK_IN_MRC,             WARNING_MRM_LEVEL_5},
        {WARNING_MRM_LEVEL_4, EVENT_CHECK_IN_PASSIVE,         NO_WARNING},
        {WARNING_MRM_LEVEL_5, EVENT_CHECK_IN_PASSIVE,         NO_WARNING},
    },
    .state_transit_size = TOTAL_WARNING_TRANS_NUM,
    .event_table = 
    {
        {EVENT_CHECK_IN_PASSIVE,         CheckInPassiveSt},
        {EVENT_COME_LEVEL_1,             TorLevel1WarningCome},
        {EVENT_COME_LEVEL_2,             TorLevel2WaningCome},
        {EVENT_COME_LEVEL_3,             TorLevel3WarningCome}, 
        {EVENT_CHECK_IN_MRM,             CheckInMrmSt},
        {EVENT_CHECK_IN_MRC,             CheckInMrcSt},
        {EVENT_RAMPUP_LEVEL_2,           RampUpToTorLevel2},
        {EVENT_RAMPUP_LEVEL_3,           RampUpToTorLevel3WithTiming},
        {EVENT_RAMPUP_LEVEL_3_WITH_TIME, RampUpToTorLevel3WithoutTiming},
        {EVENT_RAMPUP_LEVEL_4,           RampUpToMrmLevel4WithTiming}
    },
    .event_size = TOTAL_WARNING_EVENT_NUM,
    .state_table =
    {
        {NO_WARNING,          ActionInNoWarning},
        {WARNING_TOR_LEVEL_1, ActionInWarningTorLevel1},
        {WARNING_TOR_LEVEL_2, ActionInWarningTorLevel2},
        {WARNING_TOR_LEVEL_3, ActionInWarningTorLevel3},
        {WARNING_MRM_LEVEL_4, ActionInWarningMrmLevel4},
        {WARNING_MRM_LEVEL_5, ActionInWarningMrmLevel5},
    },
    .state_size = TOTAL_WARNING_STATE_NUM
};

// ------------------------------------ func def -----------------------------------------
void RunWarningSit()
{
    g_warning_signal_bitfileds = 0;

    if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL2_FAULT) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_2);
    }
    
    if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL3_FAULT) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_3);
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_3_2);
    }
    // TODO: 1级TOR延时
    if (false) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_2);
    }
    // TODO: 2级TOR延时
    if (false) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_3_2);
    }

    // TODO: 3级TOR延时
    if (false) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_RAMPUP_MRM_LEVEL_4);
    }

    if ((g_tsm.state == MCU_MRM_TOR_LNG_LAT_CTRL) || (g_tsm.state == MCU_MRM_TOR_LAT_CTRL)) {
        if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL1_FAULT) {
            SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_TOR_LEVEL_1_COME);
        } else if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL2_FAULT) {
            SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_TOR_LEVEL_2_COME);
        } else if (g_inter_media_msg.mrm_system_fault_level == TOR_LEVEL3_FAULT) {
            SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_TOR_LEVEL_3_COME);
        } else {
            // do nothing;
        }
    } else if ((g_tsm.state == MCU_MRM_ACTIVE_LNG_LAT_CTRL) || (g_tsm.state == MCU_MRM_ACTIVE_LAT_CTRL)) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_CHECK_MRM_ST);
    } else if (g_tsm.state == MCU_MRM_MRC) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_CHECK_MRC_ST);
    } else if (g_tsm.state == MCU_MRM_PASSIVE) {
        SetSignalBitFields(&g_warning_signal_bitfileds, BITNO_CHECK_PASSIVE_ST);
    } else {
        // do nothing;
    }

#ifdef _NEED_LOG
    LOG(COLOR_NONE, "RunWarningSit g_warning_signal_bitfileds: %d", g_warning_signal_bitfileds);
#endif
}

void ActionInNoWarning()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "No warnng st");
#endif
}

void ActionInWarningTorLevel1()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "Warning Tor Level_1 st");
#endif
}

void ActionInWarningTorLevel2()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "Warning Tor Level_2 st");
#endif
}

void ActionInWarningTorLevel3()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "Warning Tor Level_3 st");
#endif
}

void ActionInWarningMrmLevel4()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "Warning Mrm Level_4 st");
#endif
}

void ActionInWarningMrmLevel5()
{
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "Warning Mrm Level_5 st");
#endif
}

boolean TorLevel1WarningCome()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_TOR_LEVEL_1_COME);
}

boolean TorLevel2WaningCome()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_TOR_LEVEL_2_COME);
}

boolean TorLevel3WarningCome()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_TOR_LEVEL_3_COME);
}

boolean CheckInMrmSt()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_CHECK_MRM_ST);
}

boolean CheckInMrcSt()
{
    LOG(COLOR_NONE, "CheckInMrcSt: %d", IsBitSet(g_warning_signal_bitfileds, BITNO_CHECK_MRC_ST));
    return IsBitSet(g_warning_signal_bitfileds, BITNO_CHECK_MRC_ST);
}

boolean CheckInPassiveSt()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_CHECK_PASSIVE_ST);
}

boolean RampUpToTorLevel2()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_2);
}

boolean RampUpToTorLevel3WithTiming()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_3);
}

boolean RampUpToTorLevel3WithoutTiming()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_RAMPUP_TOR_LEVEL_3_2);
}

boolean RampUpToMrmLevel4WithTiming()
{
    return IsBitSet(g_warning_signal_bitfileds, BITNO_RAMPUP_MRM_LEVEL_4);
}