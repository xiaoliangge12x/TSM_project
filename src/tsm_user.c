/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  tsm user
 *  \author zxl
 *  \date   2022-04-26
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_user.h"
#include "tsm_parameter.h"
#include "base/sm_base.h"

uint16 K_StayInPassiveTime_Cnt = 500U;    // 连续两次的激活间隔必须要在10s以上
uint16 K_LastActivationTime_Cnt = 1500U;  // 距离上次激活的时间间隔

static uint16 passive_timer_cnt = 0;

enum tsm_event_id
{
    EVENT_FAULT_NOT_EXIST = BASE_EVENT_TSM_START + 1,
    EVENT_FAILURE,
    EVENT_NO_LIGHTING,
    EVENT_STANDBY,
    EVENT_NO_STANDBY,
    EVENT_MRM_BOTH_CTRL,
    EVENT_MRM_LAT_CTRL,
    EVENT_MRC,
    EVENT_ACTIVATE_TOR_BOTH,
    EVENT_ACTIVATE_TOR_LAT,
    EVENT_ACTIVATE_SUPPORT_SOC,
    EVENT_ACTIVATE_IFC_MRM,
    EVENT_BOTH_CTRL_EXIT,
    EVENT_LAT_CTRL_EXIT,
    EVENT_EMERGENCY_STOP_EXIT,
    EVENT_SUPPORT_SOC_EXIT,
    EVENT_TOR_TO_MRM_BOTH,
    EVENT_TOR_TO_MRM_LAT,
    EVENT_MRM_ACTIVATE_EMERGENCY_STOP,
    EVENT_MRM_SWITCH_EMERGENCY_STOP,
    EVENT_TOR_SWITCH_MRM,
    EVENT_DRIVER_LNG_OVERRIDE,
    EVENT_DRIVER_NO_LNG_OVERRIDE,
    EVENT_VEH_STANDSTILL,
    EVENT_ES_OR_MRC_EXIT,
    EVENT_WAIT_EPB_RES,
};

enum pc_st
{
    PC_TOR = 0,
    PC_MRM,
};

enum exit_type {
    EXIT_FROM_EMERGENCY_STOP,
    EXIT_FROM_BOTH_CTRL,
    EXIT_FROM_LAT_CTRL,
};

enum tsm_tor_req_st {
    NO_TOR_REQUEST,
    TOR_LEVEL1_REQUEST,  // soc failure
    TOR_LEVEL2_REQUEST,  // mcu failure
    TOR_LEVEL3_REQUEST,  // adc failure
};

static const struct state_transit tsm_state_flow[] = 
{
    {
        .cur_st = IFC_PASSIVE,
        .event_id = EVENT_FAILURE,
        .next_st = IFC_FAILURE,
    },
    {
        .cur_st = IFC_PASSIVE,
        .event_id = EVENT_STANDBY,
        .next_st = IFC_STANDBY,
    },
    {
        .cur_st = IFC_FAILURE,
        .event_id = EVENT_FAULT_NOT_EXIST,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_STANDBY,
        .event_id = EVENT_FAILURE,
        .next_st = IFC_FAILURE,
    },
    {
        .cur_st = IFC_STANDBY,
        .event_id = EVENT_MRM_ACTIVATE_EMERGENCY_STOP, 
        .next_st = IFC_MRM_EMERGENCY_BOTH_CTRL,
    },
    {
        .cur_st = IFC_STANDBY,
        .event_id = EVENT_ACTIVATE_TOR_BOTH, 
        .next_st = IFC_TOR_LNG_LAT_CTRL,
    },
    {
        .cur_st = IFC_STANDBY,
        .event_id = EVENT_ACTIVATE_TOR_LAT,
        .next_st = IFC_TOR_LAT_CTRL,
    },
    {
        .cur_st = IFC_STANDBY,
        .event_id = EVENT_ACTIVATE_SUPPORT_SOC,
        .next_st = IFC_ACTIVE_SUPPORT_SOC,
    },
    {
        .cur_st = IFC_STANDBY,
        .event_id = EVENT_NO_STANDBY,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_ACTIVE_SUPPORT_SOC,
        .event_id = EVENT_ACTIVATE_IFC_MRM, 
        .next_st = IFC_MRM_LNG_LAT_CTRL,
    },
    {
        .cur_st = IFC_ACTIVE_SUPPORT_SOC,
        .event_id = EVENT_SUPPORT_SOC_EXIT,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_BOTH_CTRL_EXIT,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_MRM_SWITCH_EMERGENCY_STOP,
        .next_st = IFC_MRM_EMERGENCY_BOTH_CTRL,
    },
    {
        .cur_st = IFC_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_TOR_SWITCH_MRM,
        .next_st = IFC_MRM_LNG_LAT_CTRL,
    },
    {
        .cur_st = IFC_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_DRIVER_LNG_OVERRIDE,
        .next_st = IFC_TOR_LAT_CTRL,
    },
    {
        .cur_st = IFC_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = IFC_MRM_MRC,
    },
    {
        .cur_st = IFC_TOR_LAT_CTRL,
        .event_id = EVENT_LAT_CTRL_EXIT,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_TOR_LAT_CTRL,
        .event_id = EVENT_TOR_SWITCH_MRM,
        .next_st = IFC_MRM_LAT_CTRL,
    },
    {
        .cur_st = IFC_TOR_LAT_CTRL,
        .event_id = EVENT_DRIVER_NO_LNG_OVERRIDE,
        .next_st = IFC_TOR_LNG_LAT_CTRL,
    },
    {
        .cur_st = IFC_MRM_EMERGENCY_BOTH_CTRL,
        .event_id = EVENT_ES_OR_MRC_EXIT,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_MRM_EMERGENCY_BOTH_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = IFC_MRM_MRC,
    },
    {
        .cur_st = IFC_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_BOTH_CTRL_EXIT,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_MRM_SWITCH_EMERGENCY_STOP,
        .next_st = IFC_MRM_EMERGENCY_BOTH_CTRL,
    },
    {
        .cur_st = IFC_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_DRIVER_LNG_OVERRIDE,
        .next_st = IFC_MRM_LAT_CTRL,
    },
    {
        .cur_st = IFC_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = IFC_MRM_MRC,
    },
    {
        .cur_st = IFC_MRM_LAT_CTRL,
        .event_id = EVENT_LAT_CTRL_EXIT,
        .next_st = IFC_PASSIVE,
    },
    {
        .cur_st = IFC_MRM_LAT_CTRL,
        .event_id = EVENT_DRIVER_NO_LNG_OVERRIDE,
        .next_st = IFC_MRM_LNG_LAT_CTRL,
    },
    {
        .cur_st = IFC_MRM_MRC,
        .event_id = EVENT_ES_OR_MRC_EXIT,
        .next_st = IFC_PASSIVE,
    },
};

static inline void
tsm_set_drvr_lng_ovrd(struct tsm_action* p_action, 
                      const enum tsm_ovrd_st ovrd_st) {
    p_action->lng_override_flag = ovrd_st;
}

static inline void 
tsm_set_ctrlarb_req(struct tsm_action* p_action, 
                    const enum tsm_ctrl_arb_req ctrl_arb_req) {
    p_action->control_arb_request = ctrl_arb_req;
}

static inline void 
tsm_set_mrm_st(struct tsm_action* p_action,
               const enum tsm_mrm_st mrm_state) {
    p_action->mrm_activation_st = mrm_state;
}

static inline void
tsm_reset_action(struct tsm_action* p_action) {
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_INVALID);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_SOC);
    tsm_set_mrm_st(p_action, MRM_ST_INVALID);
}

static void
tsm_passive_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in ifc passive st.");
#endif
    tsm_reset_action(p_action);
    p_action->mrm_available_st = 1;
    if (passive_timer_cnt > K_StayInPassiveTime_Cnt) {
        passive_timer_cnt = K_StayInPassiveTime_Cnt + 1;
    } else {
        ++passive_timer_cnt;
    }
}

static void
tsm_failure_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in ifc failure st.");
#endif
    tsm_reset_action(p_action);
    passive_timer_cnt = 0;
    p_action->mrm_available_st = 0;
}

static void
tsm_standby_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in ifc standby st.");
#endif
    tsm_reset_action(p_action);
    passive_timer_cnt = 0;
}

static void
tsm_tor_both_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "It's in ifc tor both ctrl st.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_IFC);
    tsm_set_mrm_st(p_action, MRM_ST_TOR);
}

static void
tsm_tor_lat_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "It's in ifc tor lat ctrl st.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_IFC);
    tsm_set_mrm_st(p_action, MRM_ST_TOR);
}

static void
tsm_tor_stand_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "It's in ifc tor stand st.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_IFC);
    tsm_set_mrm_st(p_action, MRM_ST_TOR);
}

static void 
tsm_mrm_es_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_RED, "It's in ifc emergency stop st.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_IFC);
    tsm_set_mrm_st(p_action, MRM_ST_ES);
}

static void 
tsm_mrm_both_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in ifc mrm both ctrl St.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_IFC);
    tsm_set_mrm_st(p_action, MRM_ST_ACTIVE);
}

static void 
tsm_mrm_lat_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in ifc mrm lat ctrl St.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_IFC);
    tsm_set_mrm_st(p_action, MRM_ST_ACTIVE);
}

static void 
tsm_mrc_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in ifc mrc St.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_IFC);
    tsm_set_mrm_st(p_action, MRM_ST_ACTIVE);
}

static boolean
tsm_is_nda_active(const enum nda_func_st nda_st) {
    return ((nda_st == NDA_ACTIVE_EPB_PHASE_IN) || 
            (nda_st == NDA_ACTIVE_HANDS_FREE_NORMAL) ||
            (nda_st == NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE) || 
            (nda_st == NDA_ACTIVE_HANDS_FREE_STAND_WAIT) ||
            (nda_st == NDA_ACTIVE_HANDS_ON_NORMAL) || 
            (nda_st == NDA_ACTIVE_HANDS_ON_STAND_ACTIVE) ||
            (nda_st == NDA_ACTIVE_HANDS_ON_STAND_WAIT) || 
            (nda_st == NDA_ACTIVE_EPB_PHASE_IN) ||
            (nda_st == NDA_LNG_OVERRIDE) || 
            (nda_st == NDA_LNG_LAT_OVERRIDE) ||
            (nda_st == NDA_LAT_OVERRIDE) || 
            (nda_st == NDA_TOR_LAT_CTRL) ||
            (nda_st == NDA_TOR_LNG_LAT_CTRL) || 
            (nda_st == NDA_TOR_STAND) ||
            (nda_st == NDA_MRM_ACTIVE_PO_LAT_CTRL) || 
            (nda_st == NDA_MRM_ACTIVE_PO_LNG_LAT_CTRL) ||
            (nda_st == NDA_MRM_ACTIVE_CP_LAT_CTRL) || 
            (nda_st == NDA_MRM_ACTIVE_CP_LNG_LAT_CTRL) ||
            (nda_st == NDA_MRM_ACTIVE_ES_LAT_CTRL) || 
            (nda_st == NDA_MRM_ACTIVE_ES_LNG_LAT_CTRL) ||
            (nda_st == NDA_MRC));
}

static boolean
tsm_check_activation_cond(const struct tsm_entry* p_entry) {
    boolean check_ret = false;
    uint8 tor_req_from_soc = p_entry->in_can_gate->Soc_Info.soc_tor_req;
    uint8 tor_req_from_mcu = p_entry->in_can_gate->mcu_tor_req;
    boolean adc_com_fault = p_entry->in_diag->ifc_mrm_system_fault_level;

    if ((tor_req_from_soc == TOR_LEVEL3_REQUEST) || 
        (tor_req_from_mcu == TOR_LEVEL3_REQUEST)) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "adc failure request trigger mrm.");
#endif
        check_ret = true;
    } else if (adc_com_fault) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "com fault with adc trigger mrm");
#endif
        check_ret = true;
    } else {
        check_ret = false;
    }

    return check_ret;
}

static enum tsm_event_id
tsm_choose_event_id(const enum tsm_event_id lat_event_id, 
                    const enum tsm_event_id both_event_id,
                    const boolean is_drvr_lng_ctrl) {
    enum tsm_event_id expect = 
        (is_drvr_lng_ctrl) ? lat_event_id : both_event_id;
    return expect;
}

static size_t
tsm_run_initial_sit(uint8* p_event, size_t num, 
                    const boolean ifc_unable_to_stop,
                    const enum nda_func_st nda_st) {
    if (ifc_unable_to_stop) {
        p_event[num++] = EVENT_FAILURE;
    } else {
        p_event[num++] = EVENT_FAULT_NOT_EXIST;
    }
    
#ifdef _NEED_LOG
    if (passive_timer_cnt != 0) {
        float log_passive_time = (float)(20 * passive_timer_cnt) / 1000;
        LOG(COLOR_GREEN, "<tsm_run_initial_sit> have stayed in passive for %f "
            "s", log_passive_time);
    }
#endif

    if (tsm_is_nda_active(nda_st)) {
        if (passive_timer_cnt > K_StayInPassiveTime_Cnt) {
            p_event[num++] = EVENT_STANDBY;
        }
    } else {
        p_event[num++] = EVENT_NO_STANDBY;
    }
    
    return num;
}

static size_t 
tsm_run_non_standstill_st(uint8* p_event, size_t num,
                          const boolean is_need_to_switch_es,
                          const boolean can_mrm_activate,
                          const boolean is_drvr_lng_ctrl) {
    if (is_need_to_switch_es) {
        p_event[num++] = 
            (can_mrm_activate) ? EVENT_MRM_ACTIVATE_EMERGENCY_STOP :
                                 EVENT_MRM_SWITCH_EMERGENCY_STOP;
    } else {
        if (can_mrm_activate) {
            p_event[num++] = tsm_choose_event_id(EVENT_ACTIVATE_TOR_LAT,
                                                 EVENT_ACTIVATE_TOR_BOTH,
                                                 is_drvr_lng_ctrl);
        }
    }

    return num;
}

static boolean
tsm_is_drvr_takeover(const boolean ifc_unable_to_stop, 
                     const struct tsm_intermediate_sig* p_int_sig,
                     const enum exit_type ex_type) {
    if (ifc_unable_to_stop) {
        return true;
    }

    boolean is_hands_touched =
            tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                           BITNO_HANDS_TOUCH_DETECTED);
    boolean is_lat_ovrd = 
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                           BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
    if (ex_type == EXIT_FROM_EMERGENCY_STOP) {
        boolean is_brake_set = 
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_SET_BRAKE);
        boolean is_lng_ovrd = 
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_LNG_OVERRIDE_ST);
        return (is_brake_set || is_lng_ovrd || is_lat_ovrd || is_hands_touched);
    } else {
        uint8 brk_du_type = p_int_sig->brk_du_type;
        if (brk_du_type == LONG_INTERVENTION) {
            return true;
        }

        if (is_lat_ovrd) {
            return true;
        }

        if (is_hands_touched && (brk_du_type == SHORT_INTERVENTION)) {
            return true;
        }

        if (ex_type == EXIT_FROM_LAT_CTRL) {
            uint8 lng_ovrd_du_type = p_int_sig->lng_ovrd_du_type;
            if (lng_ovrd_du_type == LONG_TIME_LNG_OVERRIDE) {
                return true;
            }

            if (is_hands_touched && 
                (lng_ovrd_du_type == LONG_TIME_LNG_OVERRIDE)) {
                return true;
            }
        }
    }

    return false;
}

static size_t
tsm_run_transmit_soc_sit(uint8* p_event, size_t num,
                         const struct tsm_entry* p_entry, 
                         const boolean can_mrm_activate) {
    // handle transmit soc ctrl sit
    uint8 soc_tor_req = p_entry->in_can_gate->Soc_Info.soc_tor_req;
    uint8 soc_mrm_active_st = p_entry->in_can_gate->Soc_Info.soc_mrm_active_st;
    if ((soc_tor_req == TOR_LEVEL2_REQUEST) && (soc_mrm_active_st == 1)) {
        p_event[num++] = EVENT_ACTIVATE_SUPPORT_SOC;
    }

    if (can_mrm_activate) {
        p_event[num++] = EVENT_ACTIVATE_IFC_MRM;
    }

    boolean is_support_transmit_soc_ctrl = 
        p_entry->in_diag->is_support_transmit_ctrl;

    if ((soc_mrm_active_st == 0) || (!is_support_transmit_soc_ctrl)) {
        p_event[num++] = EVENT_SUPPORT_SOC_EXIT;
    }
    
    return num;
}

static size_t
tsm_run_func_exit_sit(uint8* p_event, size_t num, 
                      const boolean ifc_unable_to_stop,
                      const struct tsm_intermediate_sig* p_int_sig) {
    if (tsm_is_drvr_takeover(ifc_unable_to_stop, p_int_sig, 
                             EXIT_FROM_EMERGENCY_STOP)) {
        p_event[num++] = EVENT_ES_OR_MRC_EXIT;
    }

    if (tsm_is_drvr_takeover(ifc_unable_to_stop, p_int_sig, 
                             EXIT_FROM_BOTH_CTRL)) {
        p_event[num++] = EVENT_BOTH_CTRL_EXIT;
    }

    if (tsm_is_drvr_takeover(ifc_unable_to_stop, p_int_sig, 
                             EXIT_FROM_LAT_CTRL)) {
        p_event[num++] = EVENT_LAT_CTRL_EXIT;
    }

    return num;
}

static size_t
tsm_run_situation(uint8* p_event,
                  const enum tsm_ifc_mrm_func_st mrm_state,
                  const struct tsm_entry* p_entry,
                  const struct tsm_intermediate_sig* p_int_sig,
                  const enum tsm_warning_st warning_state) {
    size_t event_num = 0;

    boolean ifc_unable_to_stop = 
        (!p_entry->in_diag->is_support_lane_stop &&
         !p_entry->in_diag->is_support_emergency_stop);
    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    event_num = 
        tsm_run_initial_sit(p_event, event_num, ifc_unable_to_stop, cur_nda_st);

    // judge mcu_activate_emergecy_stop

    enum tsm_veh_standstill_st veh_standstill_st =
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt;
    boolean can_mrm_activate = tsm_check_activation_cond(p_entry);
    if (veh_standstill_st == VEH_STANDSTILL_ST_STANDSTILL) {
        p_event[event_num++] = EVENT_VEH_STANDSTILL;
    } else if (veh_standstill_st == VEH_STANDSTILL_ST_NOT_STANDSTILL) {
        boolean is_need_to_switch_es = 
            (((p_entry->in_diag->is_support_lane_stop = false) && 
            (p_entry->in_diag->is_support_emergency_stop = true)) ||
            (p_entry->in_planlite->planningLite_control_state == PC_MRM));
        boolean is_drvr_lng_ctrl = tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                                                  BITNO_LNG_OVERRIDE_ST);
        event_num = tsm_run_non_standstill_st(p_event, event_num, 
                                              is_need_to_switch_es,
                                              can_mrm_activate, 
                                              is_drvr_lng_ctrl);
        if (warning_state == WARNING_MRM_LEVEL_4) {
            p_event[event_num++] = EVENT_TOR_SWITCH_MRM;
        }
        if (is_drvr_lng_ctrl) {
            p_event[event_num++] = EVENT_DRIVER_LNG_OVERRIDE;
        } else {
            p_event[event_num++] = EVENT_DRIVER_NO_LNG_OVERRIDE;
        }
    } else {
        // do nothing;
    }
    event_num = 
        tsm_run_transmit_soc_sit(p_event, event_num, p_entry, can_mrm_activate);

    event_num = 
        tsm_run_func_exit_sit(p_event, event_num, ifc_unable_to_stop, 
                              p_int_sig);

    return event_num;
}

enum tsm_ifc_mrm_func_st
tsm_run_user(const enum tsm_ifc_mrm_func_st mrm_state,
             const enum tsm_warning_st warning_state,
             const struct tsm_entry* p_entry, 
             const struct tsm_intermediate_sig* p_int_sig,
             struct tsm_action* p_action) {
    typedef void (*tsm_post_process) (struct tsm_action* p_action);
    static const tsm_post_process post_process[] = {
        [IFC_PASSIVE] = tsm_passive_post_process,
        [IFC_STANDBY] = tsm_standby_post_process,
        [IFC_FAILURE] = tsm_failure_post_process,
        [IFC_TOR_LNG_LAT_CTRL] = tsm_tor_both_post_process,
        [IFC_TOR_LAT_CTRL] = tsm_tor_lat_post_process,
        [IFC_TOR_STAND] = tsm_tor_stand_post_process,
        [IFC_MRM_EMERGENCY_BOTH_CTRL] = tsm_mrm_es_post_process,
        [IFC_MRM_LNG_LAT_CTRL] = tsm_mrm_both_post_process,
        [IFC_MRM_LAT_CTRL] = tsm_mrm_lat_post_process,
        [IFC_MRM_MRC] = tsm_mrc_post_process,
    };  

    uint8 event_id[MAX_EVENT_SIZE];
    size_t event_num = 
        tsm_run_situation(event_id, mrm_state, p_entry, p_int_sig, 
                          warning_state);

    if (event_num == 0) {
        post_process[mrm_state](p_action);
        return mrm_state;
    }

    enum tsm_ifc_mrm_func_st next_mrm_st = 
        run_state_transit(tsm_state_flow, ARRAY_LEN(tsm_state_flow), 
                          event_id, event_num, mrm_state);

    post_process[next_mrm_st](p_action);

    return next_mrm_st;
}