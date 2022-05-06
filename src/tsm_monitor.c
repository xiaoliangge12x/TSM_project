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
#include "tsm_signalhandling.h"

// ------------------------------ global variable def -------------------------------------------
// ------------------------------ driving table Initilize ----------------------------------------

// ----------------------------------- function -------------------------------------------------
void CheckMonitorPrecondition(const Veh_Sig* veh_info, const Soc_Info* soc_info)
{
    // 
}

void RunNdaTranistionMonitor(const Veh_Sig* veh_info, const Soc_Info* soc_info)
{
    CheckMonitorPrecondition(veh_info, soc_info);

    RunMonitorNdaTransitionLogic(&soc_info->automaton_state, veh_info);
}

boolean IsRainFallSatisfy(const Veh_Sig* vehicle_signal)
{
    // TODO:
    return true;
}

boolean IsOddSatisfy(const CheckMoment activation_time, const Veh_Sig* vehicle_signal, const Soc_Info* soc_info)
{
    // TODO:  默认激活前 不满足， 激活后 满足
    if (!soc_info->monitor_sig_src.EEA_Status || !IsRainFallSatisfy(vehicle_signal)) {
        return false;
    }

    if ((activation_time == BEFORE_ACTIVATION) && 
        (!vehicle_signal->BCS_VehSpdVD || (vehicle_signal->BCS_VehSpdVD && (vehicle_signal->BCS_VehSpd > K_VehSpdThreshold)))) {
        return false;
    }

    if (activation_time == BEFORE_ACTIVATION) {
        return (soc_info->monitor_sig_src.HD_Map_Warning_Dist > K_GeoEndDist_NotActive);
    } else {
        return (soc_info->monitor_sig_src.HD_Map_Warning_Dist > K_GeoEndDist_Active);
    }

    return true;
}

boolean IsNdaPassiveVD(const Soc_Info* soc_info) 
{
    // TODO:
    return true;
}

boolean ValidateNdaAvlCond(const Soc_Info* soc_info)
{

}

void CheckNdaNeedPhaseIn()
{
    // TODO:
    ResetSignalBitFields(&g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_NEED_PHASE_IN);
}

void RunMonitorNdaTransitionLogic(const Soc_State* automaton_state, const Veh_Sig* veh_sig)
{
    MonitorNdaChangeFromStandbyToActive(automaton_state, veh_sig);
    MonitorNdaChangeFromActiveToOverride(automaton_state);
    MonitorNdaIgnoreOverrideReq(automaton_state);
    MonitorNdaChangeFromOverrideToActive(automaton_state);
    MonitorNdaStuckInOverrideSt(automaton_state);
    MonitorIcaUpgradeToNda(automaton_state);
    MonitorNdaUnableToExit(automaton_state);
}
// 监控点1
void MonitorNdaChangeFromStandbyToActive(const Soc_State* automaton_st, const Veh_Sig* veh_sig)
{
    static boolean monitor_enter_normal_flag      = true;
    static boolean monitor_enter_standactive_flag = true;

    static AtomicData active_nda_abnormal_atomic_data_table[] =
    {
        {NDA_ACTIVE_HANDS_ON_NORMAL, &monitor_enter_normal_flag, "NDA transit from Stanby to Hands On Normal"},
        {NDA_ACTIVE_HANDS_ON_STAND_ACTIVE, &monitor_enter_standactive_flag, 
            "NDA transit from Stanby to Hands On standactive"},
    };

    if (automaton_st->NDA_Function_State == NDA_STANDBY_NORMAL) {
        monitor_enter_normal_flag = (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_AVL_BEFORE_ACT) ||
            (veh_sig->BCS_VehicleStandStillSt != VEH_STANDSTILL_ST_NOT_STANDSTILL)) ? false : true;
        monitor_enter_standactive_flag = (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_AVL_BEFORE_ACT) ||
            (veh_sig->BCS_VehicleStandStillSt != VEH_STANDSTILL_ST_STANDSTILL)) ? false : true;
        return;
    } 
    
    for (uint8 i = 0; i < (uint8)(sizeof(active_nda_abnormal_atomic_data_table) / sizeof(AtomicData)); ++i) {
        if ((g_inter_media_msg.last_automaton_st.NDA_Function_State) == NDA_STANDBY_NORMAL &&
            (automaton_st->NDA_Function_State == active_nda_abnormal_atomic_data_table[i].current_st)) {
            if (!(*active_nda_abnormal_atomic_data_table[i].enter_flag)) {
                ResetSignalBitFields(&g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_TRANSIT_NORMAL_FLAG);
#ifdef _NEED_LOG
                LOG(COLOR_RED, "Trigger Mrm, monitor %s abnormally.", active_nda_abnormal_atomic_data_table[i].log);
#endif
            }
            return;
        }
    }
}

// 监控点2
void MonitorNdaChangeFromActiveToOverride(const Soc_State* automaton_st)
{
    static boolean monitor_enter_lat_override_flag  = true;
    static boolean monitor_enter_lng_override_flag  = true;
    static boolean monitor_enter_both_override_flag = true;
    static uint8 last_drvr_hand_torque_override_st  = 0;

    static AtomicData enter_override_abnormal_atomic_data_table[] =
    {
        {NDA_LNG_OVERRIDE, &monitor_enter_lng_override_flag, "NDA transit from active to lng override"},
        {NDA_LNG_LAT_OVERRIDE, &monitor_enter_both_override_flag, "NDA transit from active to both override"},
        {NDA_LAT_OVERRIDE, &monitor_enter_lat_override_flag, "NDA transit from active to lat override"},
    };

    if (CheckNdaInActiveSt(automaton_st)) {
        monitor_enter_both_override_flag = (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_AVL_AFTER_ACT) ||
            !IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_LNG_OVERRIDE_ST) ||
            !IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST)) ? false : true;
        monitor_enter_lng_override_flag = (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_AVL_AFTER_ACT) ||
            !IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_LNG_OVERRIDE_ST) ||
            IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST)) ? false : true;
        monitor_enter_lat_override_flag = (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_AVL_AFTER_ACT) ||
            IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_LNG_OVERRIDE_ST) ||
            !IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST)) ? false : true;
        last_drvr_hand_torque_override_st = IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
        return;
    } 
    
    for (uint8 i = 0; i < (uint8)(sizeof(enter_override_abnormal_atomic_data_table) / sizeof(AtomicData)); ++i) {
        if (automaton_st->NDA_Function_State == enter_override_abnormal_atomic_data_table[i].current_st) {
            if (automaton_st->NDA_Function_State == NDA_LAT_OVERRIDE) {
                if (g_inter_media_msg.last_automaton_st.NDA_Function_State == NDA_ACTIVE_HANDS_ON_NORMAL) {
                    PostHandleForTransitAbnormal(*enter_override_abnormal_atomic_data_table[i].enter_flag,
                                                 last_drvr_hand_torque_override_st, 
                                                 enter_override_abnormal_atomic_data_table[i].log);
                }
            } else {
                if (CheckNdaInActiveSt(&g_inter_media_msg.last_automaton_st)) {
                    PostHandleForTransitAbnormal(*enter_override_abnormal_atomic_data_table[i].enter_flag,
                                                 last_drvr_hand_torque_override_st, 
                                                 enter_override_abnormal_atomic_data_table[i].log);
                }
            }
            return;
        }
    }
}

// 监控点3
void MonitorNdaIgnoreOverrideReq(const Soc_State* automaton_st)
{
    static uint16  ignore_overrideReq_cnt = 0;

    if (CheckNdaInActiveSt(automaton_st)) {
        char log[] = "NDA ignore override req";
        PostHandleForStuckInSt(&ignore_overrideReq_cnt, IsDriverLatOrLngOverride, K_MissOverrideTime_Cnt, log);
    } else {
        ignore_overrideReq_cnt = 0;
    }
}

// 监控点4
void MonitorNdaChangeFromOverrideToActive(const Soc_State* automaton_st)
{
    static boolean monitor_enter_normal_flag         = true;
    static uint8   last_drvr_hand_torque_override_st = 0;

    if (CheckNdaInOverrideSt(automaton_st)) {
        monitor_enter_normal_flag = (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_AVL_AFTER_ACT) ||
            IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_LNG_OVERRIDE_ST) ||
            IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST)) ? false : true;
            last_drvr_hand_torque_override_st = 
                IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
            return;
    }
    
    if (automaton_st->NDA_Function_State == NDA_ACTIVE_HANDS_ON_NORMAL) {
        if (CheckNdaInOverrideSt(&g_inter_media_msg.last_automaton_st)) {
            char log[] = "NDA transit from override to active";
            PostHandleForTransitAbnormal(monitor_enter_normal_flag, last_drvr_hand_torque_override_st, log);
        }
        return;
    }
}

// 监控点5
void MonitorNdaStuckInOverrideSt(const Soc_State* automaton_st)
{
    static uint16 stuck_in_lat_override_cnt  = 0;
    static uint16 stuck_in_lng_override_cnt  = 0;
    static uint16 stuck_in_both_override_cnt = 0;

    typedef struct 
    {
        uint8        override_st;
        CondJudge    cond_judge;
        uint16*      time_cnt;
        char         log[MAX_LOG_LEN];
    } AtomicData;
    
    static AtomicData override_stuck_atomic_data_table[] = 
    {
        {NDA_LNG_LAT_OVERRIDE, IsDriverNotLatOrLngOverride, &stuck_in_both_override_cnt, "NDA stuck in both override st"},
        {NDA_LNG_OVERRIDE, IsDriverNotLngOverride, &stuck_in_lng_override_cnt, "NDA stuck in lng override st"},
        {NDA_LAT_OVERRIDE, IsDriverNotLatOverride, &stuck_in_lat_override_cnt, "NDA stuck in lat override st"},
    };
    
    for (uint8 i = 0; i < (uint8)(sizeof(override_stuck_atomic_data_table) / sizeof(AtomicData)); ++i) {
        if (automaton_st->NDA_Function_State == override_stuck_atomic_data_table[i].override_st) {
            PostHandleForStuckInSt(override_stuck_atomic_data_table[i].time_cnt, 
                                   override_stuck_atomic_data_table[i].cond_judge,
                                   K_StuckOverrideTime_Cnt, 
                                   override_stuck_atomic_data_table[i].log);
            for (uint8 j = 0; j < (uint8)(sizeof(override_stuck_atomic_data_table) / sizeof(AtomicData)); ++j) {
                if (j == i) {
                    continue;
                } else {
                    *override_stuck_atomic_data_table[j].time_cnt = 0;
                }
            }
            return;
        }
    }

    for (uint8 i = 0; i < (uint8)(sizeof(override_stuck_atomic_data_table) / sizeof(AtomicData)); ++i) {
        *override_stuck_atomic_data_table[i].time_cnt = 0;
    }
}

// 监控点6
void MonitorIcaUpgradeToNda(const Soc_State* automaton_st)
{
    static boolean monitor_upgrade_nda               = true;
    static uint8   last_drvr_hand_torque_override_st = 0;

    typedef struct 
    {
        uint8 ica_st;
        uint8 nda_st;
        char  log[MAX_LOG_LEN];
    } AtomicData;

    static AtomicData upgrade_abnormal_atomic_data_table[] = 
    {
        {ICA_HANDS_ON_NORMAL, NDA_ACTIVE_HANDS_ON_NORMAL, "NDA upgrade from ica normal to nda normal"},
        {ICA_HANDS_ON_STANDACTIVE, NDA_ACTIVE_HANDS_ON_STAND_ACTIVE, 
            "NDA upgrade from ica standactive to nda standactive"},
        {ICA_HANDS_ON_STANDWAIT, NDA_ACTIVE_HANDS_ON_STAND_WAIT, 
            "NDA upgrade from ica standwait to nda standwait"},
        {ICA_LNG_OVERRIDE, NDA_LNG_OVERRIDE, "NDA upgrade from ica lng override to nda lng override"},
    };
    // TODO:
    if (CheckIcaInActiveOrOverrideSt(automaton_st)) {
        monitor_upgrade_nda = IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_AVL_BEFORE_ACT);
        last_drvr_hand_torque_override_st = 
            IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
        return;
    } 
    
    for (uint8 i = 0; i < (uint8)(sizeof(upgrade_abnormal_atomic_data_table) / sizeof(AtomicData)); ++i) {
        if ((automaton_st->NDA_Function_State == upgrade_abnormal_atomic_data_table[i].nda_st) &&
            (g_inter_media_msg.last_automaton_st.ICA_Function_State ==
                upgrade_abnormal_atomic_data_table[i].ica_st)) {
            PostHandleForTransitAbnormal(monitor_upgrade_nda, last_drvr_hand_torque_override_st,
                upgrade_abnormal_atomic_data_table[i].log);
            return;
        }
    }
}

// 监控点7
void MonitorNdaUnableToExit(const Soc_State* automaton_st)
{
    // TODO:
    static uint16 nda_unable_exit_cnt = 0;

    if (CheckNdaInActiveSt(automaton_st) || CheckNdaInOverrideSt(automaton_st)) {
        char log[MAX_LOG_LEN];
        (CheckNdaInActiveSt(automaton_st)) ? strncpy(log, "NDA active st unable to exit", MAX_LOG_LEN) : 
            strncpy(log, "NDA override st unable to exit", MAX_LOG_LEN);
        
        if (IsASActiveOrBrakeSet()) {
            if (nda_unable_exit_cnt > K_MissQuitTime_Cnt) {
                OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
#ifdef _NEED_LOG
                LOG(COLOR_YELLOW, "No Trigger Mrm, but monitor %s", log);
#endif
            }
            nda_unable_exit_cnt = K_MissQuitTime_Cnt + 1;
        } else {
            ++nda_unable_exit_cnt;
        }
    } else {
        nda_unable_exit_cnt = 0;
    }
}

boolean CheckNdaInActiveSt(const Soc_State* automaton_st)
{
    return ((automaton_st->NDA_Function_State == NDA_ACTIVE_HANDS_ON_NORMAL) || 
            (automaton_st->NDA_Function_State == NDA_ACTIVE_HANDS_ON_STAND_ACTIVE) ||
            (automaton_st->NDA_Function_State == NDA_ACTIVE_HANDS_ON_STAND_WAIT));
}

boolean CheckIcaInActiveOrOverrideSt(const Soc_State* automaton_st)
{
    return ((automaton_st->ICA_Function_State == ICA_HANDS_ON_NORMAL) || 
            (automaton_st->ICA_Function_State == ICA_HANDS_ON_STANDACTIVE) ||
            (automaton_st->ICA_Function_State == ICA_HANDS_ON_STANDWAIT) ||
            (automaton_st->ICA_Function_State == ICA_LNG_OVERRIDE));
}

boolean CheckNdaInOverrideSt(const Soc_State* automaton_st)
{
    return ((automaton_st->NDA_Function_State == NDA_LNG_OVERRIDE) || 
            (automaton_st->NDA_Function_State == NDA_LNG_LAT_OVERRIDE) ||
            (automaton_st->NDA_Function_State == NDA_LAT_OVERRIDE));
}

void PostHandleForTransitAbnormal(const boolean monitor_flag, const uint8 last_hand_torque_override_st, const char* log) 
{
    if (!monitor_flag) {
        if (!last_hand_torque_override_st) {
            ResetSignalBitFields(&g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_TRANSIT_NORMAL_FLAG);
#ifdef _NEED_LOG
            LOG(COLOR_RED, "Trigger Mrm, monitor %s abnormally.", log);
#endif
        } else {
#ifdef _NEED_LOG
            LOG(COLOR_YELLOW, "No trigger Mrm, but monitor %s abnormally.", log);
#endif
            OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
        }
    }
}

void PostHandleForStuckInSt(uint16* time_cnt, const CondJudge cond_judge, const uint16 time_cnt_threshold, const char* log)
{
    if (cond_judge()) {
        if (*time_cnt > time_cnt_threshold) {
            // TODO
            if (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST)) {
#ifdef _NEED_LOG
                LOG(COLOR_RED, "Trigger Mrm, monitor %s.", log);
#endif
                ResetSignalBitFields(&g_inter_media_msg.intermediate_sig_bitfields, BITNO_NDA_TRANSIT_NORMAL_FLAG);
            } else {
                OutputCtrlArbReq(CTRLARB_RESPOND_TO_MCU);
#ifdef _NEED_LOG
                LOG(COLOR_YELLOW, "No Trigger Mrm, but monitor %s", log);
#endif
            }
            *time_cnt = time_cnt_threshold + 1;
        } else {
            ++(*time_cnt);
        }
    } else {
        *time_cnt = 0;
    }
}

boolean IsDriverNotLatOverride()
{
    return !IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
}

boolean IsDriverNotLngOverride()
{
    return !IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_LNG_OVERRIDE_ST);
}

boolean IsDriverNotLatOrLngOverride()
{
    return (!IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST) ||
        !IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_LNG_OVERRIDE_ST));
}

boolean IsDriverLatOrLngOverride()
{
    return (IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST) ||
        IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_LNG_OVERRIDE_ST));
}

boolean IsASActiveOrBrakeSet()
{
    return (IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_SET_BRAKE) ||
        IsBitSet(g_inter_media_msg.intermediate_sig_bitfields, BITNO_AS_ACTIVE));
}