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

#define MAX_LOG_LEN 50

static uint16  K_MissOverrideTime_Cnt = 5U; // Active未响应进入Override St的时间阈值
static uint16  K_StuckOverrideTime_Cnt = 5U; // Override未响应进入Active St的时间间隔
static uint16  K_MissQuitTime_Cnt = 5U;

// boolean IsRainFallSatisfy(const Veh_Sig* vehicle_signal)
// {
//     // TODO:
//     return true;
// }

// boolean IsOddSatisfy(const CheckMoment activation_time, const Veh_Sig* vehicle_signal, const Soc_Info* soc_info)
// {
//     // TODO:  默认激活前 不满足， 激活后 满足
//     if (!soc_info->monitor_sig_src.EEA_Status || !IsRainFallSatisfy(vehicle_signal)) {
//         return false;
//     }

//     if ((activation_time == BEFORE_ACTIVATION) && 
//         (!vehicle_signal->BCS_VehSpdVD || (vehicle_signal->BCS_VehSpdVD && (vehicle_signal->BCS_VehSpd > K_VehSpdThreshold)))) {
//         return false;
//     }

//     if (activation_time == BEFORE_ACTIVATION) {
//         return (soc_info->monitor_sig_src.HD_Map_Warning_Dist > K_GeoEndDist_NotActive);
//     } else {
//         return (soc_info->monitor_sig_src.HD_Map_Warning_Dist > K_GeoEndDist_Active);
//     }

//     return true;
// }

// boolean IsNdaPassiveVD(const Soc_Info* soc_info) 
// {
//     // TODO:
//     return true;
// }

// boolean IsPhaseInAvailable() 
// {
//     // TODO:
//     return true;
// }

// boolean IsNdaNeedPhaseIn()
// {
//     return true;
// }

// boolean ValidateNdaAvlCond(const Soc_Info* soc_info)
// {

// }
struct tsm_atomic_data {
    enum nda_func_st cur_st;
    boolean* p_flag;
    uint16*  p_timecnt;
    char log[MAX_LOG_LEN];
};

static boolean
tsm_is_nda_in_active(const enum nda_func_st nda_st) {
    return ((nda_st == NDA_ACTIVE_HANDS_ON_NORMAL) ||
            (nda_st == NDA_ACTIVE_HANDS_ON_STAND_ACTIVE) ||
            (nda_st == NDA_ACTIVE_HANDS_ON_STAND_WAIT));
}

static boolean
tsm_is_nda_in_override(const enum nda_func_st nda_st) {
    return ((nda_st == NDA_LNG_OVERRIDE) ||
            (nda_st == NDA_LNG_LAT_OVERRIDE) ||
            (nda_st == NDA_LAT_OVERRIDE));
}

static boolean
tsm_false_transit_post_handle(const boolean monitor, const boolean lat_ovrd_st,
                              const char* log, 
                              struct tsm_intermediate_sig* p_int_sig) {
    boolean ret = false;
    if (!monitor) {
        if (!lat_ovrd_st) {
            tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                       BITNO_NDA_TRANSIT_NORMAL_FLAG);
#ifdef _NEED_LOG
            LOG(COLOR_RED, "Trigger Mrm, monitor %s abnormally.", log);
#endif
        } else {
            // todo:
#ifdef _NEED_LOG
            LOG(COLOR_YELLOW, "No trigger Mrm, but monitor %s abnormally.", 
                log);
#endif
        }
        ret = true;
    }
    return ret;
}

static boolean
tsm_stuck_post_handle(uint16* p_timecnt, const boolean flag, 
                      const uint16 timecnt_threshold, 
                      struct tsm_intermediate_sig* p_int_sig,
                      const char* log) {
    boolean ret = false;
    if (flag) {
        if (*p_timecnt > timecnt_threshold) {
            // todo:
            boolean is_lat_ovrd = 
                tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                               BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
            if (is_lat_ovrd) {
                // todo:
#ifdef _NEED_LOG
                LOG(COLOR_YELLOW, "No Trigger Mrm, but monitor %s", log);
#endif
            } else {
                tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                           BITNO_NDA_TRANSIT_NORMAL_FLAG);
#ifdef _NEED_LOG
                LOG(COLOR_RED, "Trigger Mrm, monitor %s.", log);
#endif
            }
            *p_timecnt = timecnt_threshold + 1;
            ret = true;
        } else {
            ++(*p_timecnt);
        }
    } else {
        *p_timecnt = 0;
    }
    return ret;
}

static boolean 
tsm_monitor_nda_false_activate(const tsm_soc_st* last_soc_st,
                               const struct tsm_entry* p_entry,
                               struct tsm_intermediate_sig* p_int_sig) {
    static boolean normal_enter_monitor = true;
    static boolean standactive_enter_monitor = true;
    boolean ret = false;

    static struct tsm_atomic_data false_act_ad[] = {
        {
            .cur_st =  NDA_ACTIVE_HANDS_ON_NORMAL,
            .p_flag = &normal_enter_monitor,
            .p_timecnt = NULL_PTR,
            .log = "NDA transit from Stanby to Hands On Normal",
        },
        {
            .cur_st = NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,
            .p_flag = &standactive_enter_monitor,
            .p_timecnt = NULL_PTR,
            .log = "NDA transit from Stanby to Hands On standactive",
        }
    };

    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    if (cur_nda_st == NDA_STANDBY_NORMAL) {
        uint32 int_sig_bitfield = p_int_sig->int_sig_bitfields;
        enum tsm_veh_standstill_st veh_standstill_st =
            p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt;
        normal_enter_monitor =
            (tsm_is_bit_set(int_sig_bitfield, BITNO_NDA_AVL_BEFORE_ACT) &&
            (veh_standstill_st == VEH_STANDSTILL_ST_NOT_STANDSTILL));
        standactive_enter_monitor =
            (tsm_is_bit_set(int_sig_bitfield, BITNO_NDA_AVL_BEFORE_ACT) &&
            (veh_standstill_st == VEH_STANDSTILL_ST_STANDSTILL));
        return ret;
    }

    enum nda_func_st last_nda_st = last_soc_st->NDA_Function_State;
    for (size_t i = 0; i < ARRAY_LEN(false_act_ad); ++i) {
        if ((last_nda_st == NDA_STANDBY_NORMAL) && 
            (cur_nda_st == false_act_ad[i].cur_st)) {
            if (!(*false_act_ad[i].p_flag)) {
                tsm_reset_bit_in_bitfields(&p_int_sig->int_sig_bitfields,
                                           BITNO_NDA_TRANSIT_NORMAL_FLAG);
                ret = true;
#ifdef _NEED_LOG
                LOG(COLOR_RED, "Trigger Mrm, monitor %s abnormally.", 
                    false_act_ad[i].log);
#endif
            }
        }
    }
    return ret;
}

static boolean
tsm_monitor_nda_false_override(const tsm_soc_st* last_soc_st,
                               const struct tsm_entry* p_entry,
                               struct tsm_intermediate_sig* p_int_sig) {
    static boolean enter_lat_orvd_monitor = true;
    static boolean enter_lng_orvd_monitor = true;
    static boolean enter_both_orvd_monitor = true;
    static boolean last_lat_ovrd_st = false;
    boolean ret = false;

    static struct tsm_atomic_data false_ovrd_ad[] = {
        {
            .cur_st =  NDA_LNG_OVERRIDE,
            .p_flag = &enter_lng_orvd_monitor,
            .p_timecnt = NULL_PTR,
            .log = "NDA transit from active to lng override",
        },
        {
            .cur_st = NDA_LNG_LAT_OVERRIDE,
            .p_flag = &enter_both_orvd_monitor,
            .p_timecnt = NULL_PTR,
            .log = "NDA transit from active to both override",
        },
        {
            .cur_st = NDA_LAT_OVERRIDE,
            .p_flag = &enter_lat_orvd_monitor,
            .p_timecnt = NULL_PTR,
            .log = "NDA transit from active to lat override",
        },
    };

    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    if (tsm_is_nda_in_active(cur_nda_st)) {
        uint32 int_sig_bitfield = p_int_sig->int_sig_bitfields;
        boolean lng_ovrd_st = 
            tsm_is_bit_set(int_sig_bitfield, BITNO_LNG_OVERRIDE_ST);
        last_lat_ovrd_st =
            tsm_is_bit_set(int_sig_bitfield, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
        boolean nda_avl_after_act =
            tsm_is_bit_set(int_sig_bitfield, BITNO_NDA_AVL_AFTER_ACT);
        
        enter_lat_orvd_monitor = 
            (nda_avl_after_act && !lng_ovrd_st && last_lat_ovrd_st);
        enter_lng_orvd_monitor = 
            (nda_avl_after_act && lng_ovrd_st && !last_lat_ovrd_st);
        enter_both_orvd_monitor =
            (nda_avl_after_act && lng_ovrd_st && last_lat_ovrd_st);
        return ret;
    }

    enum nda_func_st last_nda_st = last_soc_st->NDA_Function_State;
    for (size_t i = 0; i < ARRAY_LEN(false_ovrd_ad); ++i) {
        if (cur_nda_st == false_ovrd_ad[i].cur_st) {
            if ((cur_nda_st == NDA_LAT_OVERRIDE)) {
                if (last_nda_st == NDA_ACTIVE_HANDS_ON_NORMAL) {
                    ret = tsm_false_transit_post_handle(
                            *false_ovrd_ad[i].p_flag,
                            last_lat_ovrd_st,
                            false_ovrd_ad[i].log,
                            p_int_sig);
                }
            } else {
                if (tsm_is_nda_in_active(last_nda_st)) {
                    ret = tsm_false_transit_post_handle(
                            *false_ovrd_ad[i].p_flag,
                            last_lat_ovrd_st,
                            false_ovrd_ad[i].log,
                            p_int_sig);
                }
            }
        }
    }
    return ret;
}

static boolean 
tsm_monitor_nda_ignore_override(const tsm_soc_st* last_soc_st,
                                const struct tsm_entry* p_entry,
                                struct tsm_intermediate_sig* p_int_sig) {
    static uint16 timecnt = 0;
    boolean ret = false;

    boolean is_lng_or_lat_ovrd = 
        (tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_LNG_OVERRIDE_ST) || 
         tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
            BITNO_DRVR_HANDTORQUE_OVERRIDE_ST));

    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    if (tsm_is_nda_in_active(cur_nda_st)) {
        char log[] = "NDA ignore override req";
        ret = tsm_stuck_post_handle(&timecnt, is_lng_or_lat_ovrd, 
                                    K_MissOverrideTime_Cnt, p_int_sig, log);
    } else {
        timecnt = 0;
    }
    return ret;
}

static boolean
tsm_monitor_nda_false_ovrd_to_act(const tsm_soc_st* last_soc_st,
                                  const struct tsm_entry* p_entry,
                                  struct tsm_intermediate_sig* p_int_sig) {
    static boolean enter_normal_monitor = true;
    static boolean last_lat_ovrd = false;
    boolean ret = true;

    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    if (tsm_is_nda_in_override(cur_nda_st)) {
        boolean lng_ovrd_st = 
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_LNG_OVERRIDE_ST);
        last_lat_ovrd =
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                           BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
        boolean nda_avl_after_act =
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_NDA_AVL_AFTER_ACT);

        enter_normal_monitor = 
            (!nda_avl_after_act || lng_ovrd_st || last_lat_ovrd) ? false : true;
        return ret;
    }

    enum nda_func_st last_nda_st = last_soc_st->NDA_Function_State;
    if ((cur_nda_st == NDA_ACTIVE_HANDS_ON_NORMAL) && 
        tsm_is_nda_in_override(last_nda_st)) {
        char log[] = "NDA transit from override to active";
        ret = tsm_false_transit_post_handle(enter_normal_monitor, last_lat_ovrd,
                                            log, p_int_sig);
    }
    return ret;               
}

static boolean
tsm_monitor_nda_stuck_in_ovrd(const tsm_soc_st* last_soc_st,
                              const struct tsm_entry* p_entry,
                              struct tsm_intermediate_sig* p_int_sig) {
    static uint16 timecnt_lat_ovrd = 0;
    static uint16 timecnt_lng_ovrd = 0;
    static uint16 timecnt_both_ovrd = 0;
    static boolean is_not_lng_ovrd = false;
    static boolean is_not_lat_ovrd = false;
    static boolean is_not_both_ovrd = false;
    boolean ret = false;

    is_not_lng_ovrd = 
        !tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_LNG_OVERRIDE_ST);
    is_not_lat_ovrd =
        !tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                        BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
    is_not_both_ovrd = (is_not_lng_ovrd || is_not_lat_ovrd);

    static struct tsm_atomic_data stuck_in_ovrd_ad[] = {
        {
            .cur_st =  NDA_LNG_LAT_OVERRIDE,
            .p_flag = &is_not_both_ovrd,
            .p_timecnt = &timecnt_both_ovrd,
            .log = "NDA stuck in both override st",
        },
        {
            .cur_st = NDA_LNG_OVERRIDE,
            .p_flag = &is_not_lng_ovrd,
            .p_timecnt = &timecnt_lng_ovrd,
            .log = "NDA stuck in lng override st",
        },
        {
            .cur_st = NDA_LAT_OVERRIDE,
            .p_flag = &is_not_lat_ovrd,
            .p_timecnt = &timecnt_lat_ovrd,
            .log = "NDA stuck in lat override st",
        },
    };

    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    for (size_t i = 0; i < ARRAY_LEN(stuck_in_ovrd_ad); ++i) {
        if (cur_nda_st == stuck_in_ovrd_ad[i].cur_st) {
            ret = tsm_stuck_post_handle(stuck_in_ovrd_ad[i].p_timecnt,
                                        *stuck_in_ovrd_ad[i].p_flag,
                                        K_StuckOverrideTime_Cnt, p_int_sig, 
                                        stuck_in_ovrd_ad[i].log);
            for (size_t j = 0; j < ARRAY_LEN(stuck_in_ovrd_ad); ++j) {
                if (j == i) {
                    continue;
                } else {
                    *stuck_in_ovrd_ad[j].p_timecnt = 0;
                }
            }
            return ret;
        }
    }

    for (size_t i = 0; i < ARRAY_LEN(stuck_in_ovrd_ad); ++i) {
        *stuck_in_ovrd_ad[i].p_timecnt = 0;
    }
    return ret;
}

static boolean
tsm_monitor_false_upgrade_nda(const tsm_soc_st* last_soc_st,
                              const struct tsm_entry* p_entry,
                              struct tsm_intermediate_sig* p_int_sig) {
    static boolean upgrade_nda_monitor = true;
    static boolean last_lat_ovrd = false;
    boolean ret = false;

    struct ica_nda_mapping {
        enum ica_func_st ica_st;
        enum nda_func_st nda_st;
        char log[MAX_LOG_LEN];
    };

    static struct ica_nda_mapping ica_nda_map[] = {
        {
            .ica_st = ICA_HANDS_ON_NORMAL,
            .nda_st = NDA_ACTIVE_HANDS_ON_NORMAL,
            .log = "NDA upgrade from ica normal to nda normal",
        },
        {
            .ica_st = ICA_HANDS_ON_STANDACTIVE,
            .nda_st = NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,
            .log = "NDA upgrade from ica standactive to nda standactive",
        },
        {
            .ica_st = ICA_HANDS_ON_STANDWAIT,
            .nda_st = NDA_ACTIVE_HANDS_ON_STAND_WAIT,
            .log = "NDA upgrade from ica standwait to nda standwait",
        },
        {
            .ica_st = ICA_LNG_OVERRIDE,
            .nda_st = NDA_LNG_OVERRIDE,
            .log = "NDA upgrade from ica lng override to nda lng override",
        },
    };

    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    enum ica_func_st cur_ica_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.ICA_Function_State;
    enum ica_func_st last_ica_st = last_soc_st->ICA_Function_State;

    for (size_t i = 0; i < ARRAY_LEN(ica_nda_map); ++i) {
        if (cur_ica_st == ica_nda_map[i].ica_st) {
            last_lat_ovrd = 
                tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                               BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
            upgrade_nda_monitor = 
                tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                               BITNO_NDA_AVL_BEFORE_ACT);
            return ret;
        }

        if ((cur_nda_st == ica_nda_map[i].nda_st) && 
            (last_ica_st == ica_nda_map[i].ica_st)) {
            ret = tsm_false_transit_post_handle(upgrade_nda_monitor,
                                                last_lat_ovrd,
                                                ica_nda_map[i].log,
                                                p_int_sig);
        }
    }
    return ret;
}

static boolean
tsm_monitor_nda_unable_exit(const tsm_soc_st* last_soc_st,
                            const struct tsm_entry* p_entry,
                            struct tsm_intermediate_sig* p_int_sig) {
    // todo:
    static uint16 timecnt_nda_unable_exit = 0;
    boolean ret = false;

    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    boolean stuck_cond = 
        (tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_SET_BRAKE) ||
         tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_AS_ACTIVE));
    boolean is_nda_active = tsm_is_nda_in_active(cur_nda_st);
    boolean is_nda_active_or_override = 
        (is_nda_active || tsm_is_nda_in_override(cur_nda_st));

    if (is_nda_active_or_override) {
        char log[MAX_LOG_LEN];
        (is_nda_active) ? 
            strncpy(log, "NDA active st unable to exit", MAX_LOG_LEN) :
            strncpy(log, "NDA override st unable to exit", MAX_LOG_LEN);
        if (stuck_cond) {
            if (timecnt_nda_unable_exit > K_MissQuitTime_Cnt) {
                // todo:
#ifdef _NEED_LOG
                LOG(COLOR_YELLOW, "No Trigger Mrm, but monitor %s", log);
#endif
            }
            timecnt_nda_unable_exit = K_MissQuitTime_Cnt + 1;
            ret = true;
        } else {
            ++timecnt_nda_unable_exit;
        }
    } else {
        timecnt_nda_unable_exit = 0;
    }
    return ret;
}

void 
tsm_run_monitor(const struct tsm_entry* p_entry, 
                struct tsm_intermediate_sig* p_int_sig) {
    
    typedef boolean (*monitor_func) (const tsm_soc_st* last_soc_st,
                                     const struct tsm_entry* p_entry,
                                     struct tsm_intermediate_sig* p_int_sig);
    
    static tsm_soc_st last_soc_st = 
    {
        .NDA_Function_State = NDA_DISABLE,
        .ICA_Function_State = ICA_DISABLE,
        .ACC_Function_State = ACC_DISABLE,
    };
    
    static monitor_func monitor_table[] = {
        tsm_monitor_nda_false_activate, tsm_monitor_nda_false_override,
        tsm_monitor_nda_ignore_override, tsm_monitor_nda_false_ovrd_to_act,
        tsm_monitor_nda_stuck_in_ovrd, tsm_monitor_false_upgrade_nda,
        tsm_monitor_nda_unable_exit
    };

    for (size_t i = 0; i < ARRAY_LEN(monitor_table); ++i) {
        boolean ret = monitor_table[i](&last_soc_st, p_entry, p_int_sig);
        if (ret) {
            memcpy(&last_soc_st, 
                   &p_entry->in_can_gate->Soc_Info.automaton_state,
                   sizeof(tsm_soc_st));
            return;
        }
    }

    memcpy(&last_soc_st, 
           &p_entry->in_can_gate->Soc_Info.automaton_state,
           sizeof(tsm_soc_st));
}