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
    EVENT_BOTH_CTRL_EXIT,
    EVENT_LAT_CTRL_EXIT,
    EVENT_EMERGENCY_STOP_EXIT,
    EVENT_TOR_TO_MRM_BOTH,
    EVENT_TOR_TO_MRM_LAT,
    EVENT_MRM_ACTIVATE_EMERGENCY_STOP,
    EVENT_MRM_SWITCH_EMERGENCY_STOP,
    EVENT_TOR_SWITCH_MRM,
    EVENT_DRIVER_LNG_OVERRIDE,
    EVENT_DRIVER_NO_LNG_OVERRIDE,
    EVENT_VEH_STANDSTILL,
    EVENT_MRC_EXIT,
    EVENT_WAIT_EPB_RES,
};

enum pc_st
{
    PC_TOR = 0,
    PC_MRM,
};

static const struct state_transit tsm_state_flow[] = 
{
    {
        .cur_st = MCU_PASSIVE,
        .event_id = EVENT_FAILURE,
        .next_st = MCU_FAILURE,
    },
    {
        .cur_st = MCU_PASSIVE,
        .event_id = EVENT_STANDBY,
        .next_st = MCU_STANDBY,
    },
    {
        .cur_st = MCU_FAILURE,
        .event_id = EVENT_FAULT_NOT_EXIST,
        .next_st = MCU_PASSIVE,
    },
    {
        .cur_st = MCU_STANDBY,
        .event_id = EVENT_FAILURE,
        .next_st = MCU_FAILURE,
    },
    {
        .cur_st = MCU_STANDBY,
        .event_id = EVENT_MRM_ACTIVATE_EMERGENCY_STOP, 
        .next_st = MCU_MRM_EMERGENCY_BOTH_CTRL,
    },
    {
        .cur_st = MCU_STANDBY,
        .event_id = EVENT_ACTIVATE_TOR_BOTH, 
        .next_st = MCU_TOR_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_STANDBY,
        .event_id = EVENT_ACTIVATE_TOR_LAT,
        .next_st = MCU_TOR_LAT_CTRL,
    },
    {
        .cur_st = MCU_STANDBY,
        .event_id = EVENT_NO_STANDBY,
        .next_st = MCU_PASSIVE,
    },
    {
        .cur_st = MCU_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_BOTH_CTRL_EXIT,
        .next_st = MCU_PASSIVE,
    },
    {
        .cur_st = MCU_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_MRM_SWITCH_EMERGENCY_STOP,
        .next_st = MCU_MRM_EMERGENCY_BOTH_CTRL,
    },
    {
        .cur_st = MCU_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_TOR_SWITCH_MRM,
        .next_st = MCU_MRM_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_DRIVER_LNG_OVERRIDE,
        .next_st = MCU_TOR_LAT_CTRL,
    },
    {
        .cur_st = MCU_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = MCU_TOR_STAND,
    },
    {
        .cur_st = MCU_TOR_LAT_CTRL,
        .event_id = EVENT_LAT_CTRL_EXIT,
        .next_st = MCU_PASSIVE,
    },
    {
        .cur_st = MCU_TOR_LAT_CTRL,
        .event_id = EVENT_TOR_SWITCH_MRM,
        .next_st = MCU_MRM_LAT_CTRL,
    },
    {
        .cur_st = MCU_TOR_LAT_CTRL,
        .event_id = EVENT_DRIVER_NO_LNG_OVERRIDE,
        .next_st = MCU_TOR_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_TOR_STAND,
        .event_id = EVENT_WAIT_EPB_RES,
        .next_st = MCU_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_BOTH_CTRL_EXIT,
        .next_st = MCU_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_MRM_SWITCH_EMERGENCY_STOP,
        .next_st = MCU_MRM_EMERGENCY_BOTH_CTRL,
    },
    {
        .cur_st = MCU_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_DRIVER_LNG_OVERRIDE,
        .next_st = MCU_MRM_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_LNG_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = MCU_MRM_MRC,
    },
    {
        .cur_st = MCU_MRM_LAT_CTRL,
        .event_id = EVENT_LAT_CTRL_EXIT,
        .next_st = MCU_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_LAT_CTRL,
        .event_id = EVENT_DRIVER_NO_LNG_OVERRIDE,
        .next_st = MCU_MRM_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_MRC,
        .event_id = EVENT_MRC_EXIT,
        .next_st = MCU_PASSIVE,
    },
};

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
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_SOC);
    tsm_set_mrm_st(p_action, MRM_ST_INVALID);
}

static void
tsm_passive_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in mcu passive st.");
#endif
    tsm_reset_action(p_action);
}

static void
tsm_failure_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in mcu failure st.");
#endif
    tsm_reset_action(p_action);
}

static void
tsm_standby_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in mcu standby st.");
#endif
    tsm_reset_action(p_action);
}

static void
tsm_tor_both_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "It's in mcu tor both ctrl st.");
#endif
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_st(p_action, MRM_ST_TOR);
}

static void
tsm_tor_lat_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "It's in mcu tor lat ctrl st.");
#endif
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_st(p_action, MRM_ST_TOR);
}

static void
tsm_tor_stand_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_GREEN, "It's in mcu tor stand st.");
#endif
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_st(p_action, MRM_ST_TOR);
}

static void 
tsm_mrm_es_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_RED, "It's in mcu emergency stop st.");
#endif
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_st(p_action, MRM_ST_ES);
}

static void 
tsm_mrm_both_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in mcu mrm both ctrl St.");
#endif
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_st(p_action, MRM_ST_ACTIVE);
}

static void 
tsm_mrm_lat_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in mcu mrm lat ctrl St.");
#endif
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_st(p_action, MRM_ST_ACTIVE);
}

static void 
tsm_mrc_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in mcu mrc St.");
#endif
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_st(p_action, MRM_ST_ACTIVE);
}

static boolean
tsm_check_activation_cond(const struct tsm_entry* p_entry,
                          const struct tsm_intermediate_sig* p_int_sig) {
    boolean check_ret = false;
    uint8 soc_frc_fault = 
        p_entry->in_can_gate->Soc_Info.frc_fault_from_soc;
    boolean soc_req_mrm =
        p_entry->in_can_gate->Soc_Info.request_mrm_from_soc;
    boolean mcu_mrm_fault = p_entry->in_diag->mcu_mrm_system_fault_level;
    if ((soc_frc_fault == TOR_LEVEL3_FAULT) || soc_req_mrm) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "soc tor fault or soc request trigger mrm.");
#endif
        check_ret = true;
    } else if (mcu_mrm_fault) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "mcu mrm fault trigger mrm");
#endif
        check_ret = true;
    } else if (!tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                               BITNO_NDA_TRANSIT_NORMAL_FLAG)) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "transit abnormal trigger Mrm.");
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
                    const boolean mcu_unable_to_stop,
                    const enum nda_func_st nda_st) {
    // todo: 发一帧mrm请求给ifc
    if (mcu_unable_to_stop) {
        p_event[num++] = EVENT_FAILURE;
    } else {
        p_event[num++] = EVENT_FAULT_NOT_EXIST;
    }

    if (tsm_is_nda_active(nda_st)) {
        p_event[num++] = EVENT_STANDBY;
    } else {
        p_event[num++] = EVENT_NO_STANDBY;
    }

    // for debug
    // p_event[num++] = EVENT_STANDBY;

    return num;
}

static size_t 
tsm_run_non_standstill_st(uint8* p_event, size_t num,
                          const boolean is_need_to_switch_es,
                          const boolean can_mrm_activate,
                          const boolean is_drvr_lng_ctrl) {
    if (is_need_to_switch_es) {
        p_event[num++] = (can_mrm_activate) ?
            EVENT_MRM_ACTIVATE_EMERGENCY_STOP :
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
tsm_is_directly_exit(const uint32 bitfields, const boolean mcu_unable_to_stop,
                     const boolean is_as_active) {
    boolean is_brake_set = tsm_is_bit_set(bitfields, BITNO_SET_BRAKE);
    if (is_brake_set) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "<tsm_is_directly_exit> Driver take brake.");
#endif
        return true;
    }

    boolean is_lat_override =
        tsm_is_bit_set(bitfields, BITNO_DRVR_HANDTORQUE_OVERRIDE_ST);
    if (is_lat_override) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "<tsm_is_directly_exit> Driver take lat override.");
#endif
        return true;
    }

    if (mcu_unable_to_stop) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "<tsm_is_directly_exit> mcu unable to safe stop and "
            "emergency stop");
#endif
        return true;
    }

    if (is_as_active) {
#ifdef _NEED_LOG
        LOG(COLOR_RED, "<tsm_is_directly_exit> AS take ctrl of the veh");
#endif
        return true;
    }
    return false;
}

static boolean
tsm_is_exit_with_lat_ctrl(const enum tsm_drvr_attention_st drvr_att_st, 
                           const uint32 bitfields) {
    boolean is_drvr_awake = (drvr_att_st == DRVR_AWAKE_NOT_DISTRACTED);
    if (is_drvr_awake) {
        if (tsm_is_bit_set(bitfields, BITNO_HANDS_CAN_TAKEOVER) ||
            tsm_is_bit_set(bitfields, BITNO_LONG_TIME_LNG_OVERRIDE)) {
#ifdef _NEED_LOG
            LOG(COLOR_RED, "<tsm_is_exit_with_lat_ctrl> Driver attention st: "
                "%d, Driver hands can takeover or Driver take lng override"
                " long time.", drvr_att_st);
#endif
            return true;
        }
    } else {
        if (tsm_is_bit_set(bitfields, BITNO_HANDS_CAN_TAKEOVER) &&
            tsm_is_bit_set(bitfields, BITNO_LONG_TIME_LNG_OVERRIDE)) {
#ifdef _NEED_LOG
            LOG(COLOR_RED, "<tsm_is_exit_with_lat_ctrl> Driver attention st: "
                "%d, Driver hands can takeover and Driver take lng override"
                " long time.", drvr_att_st);
#endif
            return true;
        }
    }
    return false;
}

static boolean
tsm_is_exit_with_hands_to(const enum tsm_drvr_attention_st drvr_att_st, 
                          const uint32 bitfields) {
    boolean is_drvr_awake = (drvr_att_st == DRVR_AWAKE_NOT_DISTRACTED);
    if (is_drvr_awake) {
        if (tsm_is_bit_set(bitfields, BITNO_HANDS_CAN_TAKEOVER)) {
            return true;
        }
    } 
    return false;
}

static boolean
tsm_is_drvr_takeover(const boolean mcu_unable_to_stop, 
                     const struct tsm_intermediate_sig* p_int_sig,
                     const enum tsm_mcu_mrm_func_st mrm_state) {
    enum tsm_drvr_attention_st drvr_att_st = p_int_sig->drvr_att_st;
    if (mrm_state == MCU_TOR_STAND) {
        // todo:
    } else {
        boolean is_as_active = 
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, BITNO_AS_ACTIVE);
        boolean ret =
            tsm_is_directly_exit(p_int_sig->int_sig_bitfields, 
                                 mcu_unable_to_stop, is_as_active);
        if (ret) {
            return true;
        }

        if (mrm_state == MCU_MRM_EMERGENCY_BOTH_CTRL) {
            boolean is_lng_override =
                tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                               BITNO_LNG_OVERRIDE_ST);
            if (is_lng_override) {
#ifdef _NEED_LOG
                LOG(COLOR_RED, "<tsm_is_drvr_takeover> Driver take lng "
                    "override.");
#endif
                return true;
            }

            boolean ret =
                tsm_is_exit_with_hands_to(drvr_att_st, 
                                          p_int_sig->int_sig_bitfields);
            if (ret) {
#ifdef _NEED_LOG
                LOG(COLOR_RED, "<tsm_is_drvr_takeover>  Driver attention st:"
                    " %d, Driver hands can take over.", drvr_att_st);
#endif
                return true;
            }
        } else if ((mrm_state == MCU_TOR_LNG_LAT_CTRL) || 
                   (mrm_state == MCU_MRM_LNG_LAT_CTRL)) {
            boolean ret = 
                tsm_is_exit_with_hands_to(drvr_att_st,
                                          p_int_sig->int_sig_bitfields);
            if (ret) {
#ifdef _NEED_LOG
                LOG(COLOR_RED, "<tsm_is_drvr_takeover> Driver attention st:"
                    " %d, Driver hands can take over.", drvr_att_st);
#endif
                return true;
            }
        } else if ((mrm_state == MCU_TOR_LAT_CTRL) || 
                   (mrm_state == MCU_MRM_LAT_CTRL)) {
            boolean ret = 
                tsm_is_exit_with_lat_ctrl(drvr_att_st,
                                          p_int_sig->int_sig_bitfields);
            if (ret) {
                return true;
            }
        } else if (mrm_state == MCU_MRM_MRC) {
            boolean is_lng_override =
                tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                    BITNO_LNG_OVERRIDE_ST);
            if (is_lng_override) {
#ifdef _NEED_LOG
                LOG(COLOR_RED, "<tsm_is_drvr_takeover> Driver take lng" 
                    "override.");
#endif
                return true;
            }

            boolean is_hands_to =
                tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                               BITNO_HANDS_CAN_TAKEOVER);
            if (is_hands_to) {
#ifdef _NEED_LOG
                LOG(COLOR_RED, "<tsm_is_drvr_takeover> Driver hands can "
                    "takeover");
#endif
                return true;
            }
        }
    }
    
    return false;
}

static size_t
tsm_run_func_exit_sit(uint8* p_event, size_t num,
                      const struct tsm_entry* p_entry,
                      const boolean mcu_unable_to_stop,
                      const struct tsm_intermediate_sig* p_int_sig,
                      const enum tsm_mcu_mrm_func_st mrm_state) {
    static const enum tsm_event_id tsm_exit_event[] = {
        [MCU_MRM_EMERGENCY_BOTH_CTRL] = EVENT_EMERGENCY_STOP_EXIT,
        [MCU_TOR_LNG_LAT_CTRL] = EVENT_BOTH_CTRL_EXIT,
        [MCU_MRM_LNG_LAT_CTRL] = EVENT_BOTH_CTRL_EXIT,
        [MCU_TOR_LAT_CTRL] = EVENT_LAT_CTRL_EXIT,
        [MCU_MRM_LAT_CTRL] = EVENT_LAT_CTRL_EXIT,
        [MCU_TOR_STAND] = EVENT_WAIT_EPB_RES,
        [MCU_MRM_MRC] = EVENT_MRC_EXIT,
    };

    if (tsm_is_drvr_takeover(mcu_unable_to_stop, p_int_sig, mrm_state)) {
        p_event[num++] = tsm_exit_event[mrm_state];
    }

    return num;
}

static size_t
tsm_run_situation(uint8* p_event, 
                  const enum tsm_mcu_mrm_func_st mrm_state,
                  const struct tsm_entry* p_entry,
                  const struct tsm_intermediate_sig* p_int_sig,
                  const enum tsm_warning_st warning_state) {
    size_t event_num = 0;

    boolean mcu_unable_to_stop = 
        (!p_entry->in_diag->is_support_lane_stop &&
         !p_entry->in_diag->is_support_emergency_stop);
    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    event_num = 
        tsm_run_initial_sit(p_event, event_num, mcu_unable_to_stop, cur_nda_st);

    // judge mcu_activate_emergecy_stop

    enum tsm_veh_standstill_st veh_standstill_st =
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt;
    if (veh_standstill_st == VEH_STANDSTILL_ST_STANDSTILL) {
        p_event[event_num++] = EVENT_VEH_STANDSTILL;
    } else if (veh_standstill_st == VEH_STANDSTILL_ST_NOT_STANDSTILL) {
        boolean is_need_to_switch_es = 
            (((p_entry->in_diag->is_support_lane_stop = false) && 
            (p_entry->in_diag->is_support_emergency_stop = true)) ||
            (p_entry->in_planlite->planningLite_control_state == PC_MRM));
        boolean can_mrm_activate = 
            tsm_check_activation_cond(p_entry, p_int_sig);
        boolean is_drvr_lng_ctrl = 
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                           BITNO_LNG_OVERRIDE_ST);
        event_num =
            tsm_run_non_standstill_st(p_event, event_num, is_need_to_switch_es,
                                      can_mrm_activate, is_drvr_lng_ctrl);
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
        tsm_run_func_exit_sit(p_event, event_num, p_entry, mcu_unable_to_stop, 
                              p_int_sig, mrm_state);

    return event_num;
}

enum tsm_mcu_mrm_func_st
tsm_run_user(const enum tsm_mcu_mrm_func_st mrm_state,
             const enum tsm_warning_st warning_state,
             const struct tsm_entry* p_entry, 
             const struct tsm_intermediate_sig* p_int_sig,
             struct tsm_action* p_action) {
    typedef void (*tsm_post_process) (struct tsm_action* p_action);
    static const tsm_post_process post_process[] = {
        [MCU_PASSIVE] = tsm_passive_post_process,
        [MCU_STANDBY] = tsm_standby_post_process,
        [MCU_FAILURE] = tsm_failure_post_process,
        [MCU_TOR_LNG_LAT_CTRL] = tsm_tor_both_post_process,
        [MCU_TOR_LAT_CTRL] = tsm_tor_lat_post_process,
        [MCU_TOR_STAND] = tsm_tor_stand_post_process,
        [MCU_MRM_EMERGENCY_BOTH_CTRL] = tsm_mrm_es_post_process,
        [MCU_MRM_LNG_LAT_CTRL] = tsm_mrm_both_post_process,
        [MCU_MRM_LAT_CTRL] = tsm_mrm_lat_post_process,
        [MCU_MRM_MRC] = tsm_mrc_post_process,
    };  

    uint8 event_id[MAX_EVENT_SIZE];
    size_t event_num = 
        tsm_run_situation(event_id, mrm_state, p_entry, p_int_sig, 
                          warning_state);

    if (event_num == 0) {
        post_process[mrm_state](p_action);
        return mrm_state;
    }

    enum tsm_mcu_mrm_func_st next_mrm_st = 
        run_state_transit(tsm_state_flow, ARRAY_LEN(tsm_state_flow), 
                          event_id, event_num, mrm_state);

    post_process[next_mrm_st](p_action);

    return next_mrm_st;
}