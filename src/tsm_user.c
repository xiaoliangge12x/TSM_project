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
    EVENT_LIGHTING,
    EVENT_NO_LIGHTING,
    EVENT_STANDBY,
    EVENT_NO_STANDBY,
    EVENT_MRM_BOTH_CTRL,
    EVENT_MRM_LAT_CTRL,
    EVENT_MRC,
    EVENT_TOR_BOTH_CTRL,
    EVENT_TOR_LAT_CTRL,
    EVENT_TOR_TO_MRM_BOTH,
    EVENT_TOR_TO_MRM_LAT,
    EVENT_MRM_BOTH_CTRL_SWITCH,
    EVENT_MRM_LAT_CTRL_SWITCH,
    EVENT_TOR_BOTH_CTRL_SWITCH,
    EVENT_TOR_LAT_CTRL_SWITCH,
    EVENT_VEH_STANDSTILL,
    EVENT_FUNCTION_EXIT,
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
        .cur_st = MCU_MRM_PASSIVE,
        .event_id = EVENT_LIGHTING,
        .next_st = MCU_MRM_FAILURE_LIGHTING,
    },
    {
        .cur_st = MCU_MRM_PASSIVE,
        .event_id = EVENT_NO_LIGHTING,
        .next_st = MCU_MRM_FAILURE_NO_LIGHTING,
    },
    {
        .cur_st = MCU_MRM_PASSIVE,
        .event_id = EVENT_STANDBY,
        .next_st = MCU_MRM_STANDBY,
    },
    {
        .cur_st = MCU_MRM_FAILURE_LIGHTING,
        .event_id = EVENT_NO_LIGHTING,
        .next_st = MCU_MRM_FAILURE_NO_LIGHTING,
    },
    {
        .cur_st = MCU_MRM_FAILURE_LIGHTING,
        .event_id = EVENT_FAULT_NOT_EXIST,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_FAILURE_NO_LIGHTING,
        .event_id = EVENT_LIGHTING,
        .next_st = MCU_MRM_FAILURE_LIGHTING,
    },
    {
        .cur_st = MCU_MRM_FAILURE_NO_LIGHTING,
        .event_id = EVENT_FAULT_NOT_EXIST,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_LIGHTING,
        .next_st = MCU_MRM_FAILURE_LIGHTING,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_NO_LIGHTING,
        .next_st = MCU_MRM_FAILURE_NO_LIGHTING,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_MRM_BOTH_CTRL,
        .next_st = MCU_MRM_ACTIVE_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_MRM_LAT_CTRL,
        .next_st = MCU_MRM_ACTIVE_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_MRC,
        .next_st = MCU_MRM_MRC,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_TOR_BOTH_CTRL,
        .next_st = MCU_MRM_TOR_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_TOR_LAT_CTRL,
        .next_st = MCU_MRM_TOR_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_STANDBY,
        .event_id = EVENT_NO_STANDBY,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_TOR_LAT_CTRL_SWITCH,
        .next_st = MCU_MRM_TOR_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = MCU_MRM_TOR_STAND,
    },
    {
        .cur_st = MCU_MRM_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_TOR_TO_MRM_BOTH,
        .next_st = MCU_MRM_ACTIVE_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_TOR_TO_MRM_LAT,
        .next_st = MCU_MRM_ACTIVE_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_TOR_LNG_LAT_CTRL,
        .event_id = EVENT_FUNCTION_EXIT,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_TOR_LAT_CTRL,
        .event_id = EVENT_TOR_BOTH_CTRL_SWITCH,
        .next_st = MCU_MRM_TOR_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_TOR_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = MCU_MRM_TOR_STAND,
    },
    {
        .cur_st = MCU_MRM_TOR_LAT_CTRL,
        .event_id = EVENT_TOR_TO_MRM_BOTH,
        .next_st = MCU_MRM_ACTIVE_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_TOR_LAT_CTRL,
        .event_id = EVENT_TOR_TO_MRM_LAT,
        .next_st = MCU_MRM_ACTIVE_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_TOR_LAT_CTRL,
        .event_id = EVENT_FUNCTION_EXIT,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_TOR_STAND,
        .event_id = EVENT_WAIT_EPB_RES,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_ACTIVE_LNG_LAT_CTRL,
        .event_id = EVENT_MRM_LAT_CTRL_SWITCH,
        .next_st = MCU_MRM_ACTIVE_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_ACTIVE_LNG_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = MCU_MRM_MRC,
    },
    {
        .cur_st = MCU_MRM_ACTIVE_LNG_LAT_CTRL,
        .event_id = EVENT_FUNCTION_EXIT,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_ACTIVE_LAT_CTRL,
        .event_id = EVENT_MRM_BOTH_CTRL_SWITCH,
        .next_st = MCU_MRM_ACTIVE_LNG_LAT_CTRL,
    },
    {
        .cur_st = MCU_MRM_ACTIVE_LAT_CTRL,
        .event_id = EVENT_VEH_STANDSTILL,
        .next_st = MCU_MRM_MRC,
    },
    {
        .cur_st = MCU_MRM_ACTIVE_LAT_CTRL,
        .event_id = EVENT_FUNCTION_EXIT,
        .next_st = MCU_MRM_PASSIVE,
    },
    {
        .cur_st = MCU_MRM_MRC,
        .event_id = EVENT_FUNCTION_EXIT,
        .next_st = MCU_MRM_PASSIVE,
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
tsm_set_mrm_St(struct tsm_action* p_action,
               const enum tsm_mrm_st mrm_state) {
    p_action->mrm_activation_st = mrm_state;
}

static inline void
tsm_reset_action(struct tsm_action* p_action) {
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_INVALID);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_SOC);
    tsm_set_mrm_St(p_action, MRM_ST_INVALID);
}

static void
tsm_passive_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in mcu passive st.");
#endif
    tsm_reset_action(p_action);
}

static void
tsm_lighting_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in mcu lighting st.");
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
    LOG(COLOR_NONE, "It's in mcu tor both ctrl st.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_St(p_action, MRM_ST_TOR);
}

static void
tsm_tor_lat_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in mcu tor lat ctrl st.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_St(p_action, MRM_ST_TOR);
}

static void
tsm_tor_stand_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "It's in mcu tor stand st.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_St(p_action, MRM_ST_TOR);
}

static void 
tsm_mrm_both_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in mcu mrm both ctrl St.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_St(p_action, MRM_ST_ACTIVE);
}

static void 
tsm_mrm_lat_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in mcu mrm lat ctrl St.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_St(p_action, MRM_ST_ACTIVE);
}

static void 
tsm_mrc_post_process(struct tsm_action* p_action) {
#ifdef _NEED_LOG
    LOG(COLOR_YELLOW, "It's in mcu mrc St.");
#endif
    tsm_set_drvr_lng_ovrd(p_action, OVERRIDE_NOT_SATISFY);
    tsm_set_ctrlarb_req(p_action, CTRLARB_RESPOND_TO_MCU);
    tsm_set_mrm_St(p_action, MRM_ST_ACTIVE);
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
tsm_check_activation_cond(const struct tsm_entry* p_entry,
                          const struct tsm_intermediate_sig* p_int_sig) {
    boolean check_ret = false;
    // todo:
    uint8 soc_frc_fault = 
        p_entry->in_can_gate->Soc_Info.frc_fault_from_soc;
    boolean soc_req_mrm =
        p_entry->in_can_gate->Soc_Info.request_mrm_from_soc;
    boolean soc_com_fault = p_entry->in_diag->Com_Fault_with_SOC;
    if ((soc_frc_fault == TOR_LEVEL3_FAULT) || soc_req_mrm) {
        LOG(COLOR_RED, "soc tor fault or soc request trigger mrm.");
        check_ret = true;
    } else if (soc_com_fault) {
        LOG(COLOR_RED, "com fault with soc trigger mrm");
        check_ret = true;
    } else if (!tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                               BITNO_NDA_TRANSIT_NORMAL_FLAG)) {
        LOG(COLOR_RED, "transit abnormal trigger Mrm.");
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
                    const enum fault_level mcu_fault_lvl,
                    const enum nda_func_st nda_st) {
    // todo: 发一帧mrm请求给ifc
    if (mcu_fault_lvl == NO_FAULT) {
        p_event[num++] = EVENT_FAULT_NOT_EXIST;
    } else {
        p_event[num++] = EVENT_LIGHTING;
    }

    // if (tsm_is_nda_active(nda_st)) {
    //     p_event[num++] = EVENT_STANDBY;
    // } else {
    //     p_event[num++] = EVENT_NO_STANDBY;
    // }
    p_event[num++] = EVENT_STANDBY;

    return num;
}

static size_t
tsm_run_standstill_sit(uint8* p_event, size_t num, 
                       const boolean can_mrm_activate) {
    p_event[num++] = EVENT_VEH_STANDSTILL;
    if (can_mrm_activate) {
        p_event[num++] = EVENT_MRC;
    }
    return num;
}

static size_t 
tsm_run_tor_st(uint8* p_event, size_t num, 
               const boolean can_mrm_activate,
               const boolean is_drvr_lng_ctrl) {
    p_event[num++] = 
            tsm_choose_event_id(EVENT_TOR_LAT_CTRL_SWITCH,
                                EVENT_TOR_BOTH_CTRL_SWITCH, 
                                is_drvr_lng_ctrl);
    if (can_mrm_activate) {
        p_event[num++] = 
            tsm_choose_event_id(EVENT_TOR_LAT_CTRL,
                                EVENT_TOR_BOTH_CTRL, 
                                is_drvr_lng_ctrl);
    }
    return num;
}

static size_t 
tsm_run_mrm_st(uint8* p_event, size_t num, 
               const boolean can_mrm_activate,
               const boolean is_drvr_lng_ctrl) {
    p_event[num++] = 
            tsm_choose_event_id(EVENT_TOR_TO_MRM_LAT,
                                EVENT_TOR_TO_MRM_BOTH, 
                                is_drvr_lng_ctrl);
    if (can_mrm_activate) {
        p_event[num++] = 
            tsm_choose_event_id(EVENT_MRM_LAT_CTRL,
                                EVENT_MRM_BOTH_CTRL, 
                                is_drvr_lng_ctrl);
    }
    return num;
}

static size_t 
tsm_run_non_standstill_st(uint8* p_event, size_t num,
                          const enum pc_st plan_ctrl_st,
                          const boolean can_mrm_activate,
                          const boolean is_drvr_lng_ctrl) {
    p_event[num++] = 
        tsm_choose_event_id(EVENT_MRM_LAT_CTRL_SWITCH,
                            EVENT_MRM_BOTH_CTRL_SWITCH, is_drvr_lng_ctrl);
    
    if (plan_ctrl_st == PC_TOR) {
        num = tsm_run_tor_st(p_event, num, can_mrm_activate, is_drvr_lng_ctrl);
    } else if (plan_ctrl_st == PC_MRM) {
        num = tsm_run_mrm_st(p_event, num, can_mrm_activate, is_drvr_lng_ctrl);
    }

    return num;
}

static size_t
tsm_run_func_exit_sit(uint8* p_event, size_t num, 
                      const enum fault_level mcu_fault_lvl,
                      const boolean is_drvr_to) {
    if (is_drvr_to) {
        p_event[num++] = EVENT_FUNCTION_EXIT;
    } else if (mcu_fault_lvl != NO_FAULT) {
        // todo: 增加向IFC的mrm_req
        p_event[num++] = EVENT_FUNCTION_EXIT;
    }
    return num;
}

static boolean
tsm_is_drvr_takeover(const enum tsm_mcu_mrm_func_st mrm_state, 
                     const struct tsm_intermediate_sig* p_int_sig) {
    boolean is_mrm_lat_ctrl = ((mrm_state == MCU_MRM_ACTIVE_LAT_CTRL) ||
                               (mrm_state == MCU_MRM_TOR_LAT_CTRL) ||
                               (mrm_state == MCU_MRM_MRC));
    if (is_mrm_lat_ctrl) {
        if (p_int_sig->brk_du_type == LONG_INTERVENTION) {
            return true;
        }
        if ((p_int_sig->drvr_att_st == DRVR_AWAKE_NOT_DISTRACTED) &&
            tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                           BITNO_LONG_TIME_LNG_OVERRIDE)) {
            return true;
        }
        boolean is_drvr_fatigue_or_distracted = 
            ((p_int_sig->drvr_att_st == DRVR_AWAKE_DISTRACTED) ||
             (p_int_sig->drvr_att_st == DRVR_FATIGUE));
        if (is_drvr_fatigue_or_distracted && 
            tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                           BITNO_HANDS_CAN_TAKEOVER) &&
            tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                           BITNO_LONG_TIME_LNG_OVERRIDE)) {
            return true;
        }
    }

    if ((p_int_sig->drvr_att_st == DRVR_AWAKE_NOT_DISTRACTED) &&
        (tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                        BITNO_HANDS_CAN_TAKEOVER))) {
        return true;
    }

    if (tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                       BITNO_DRVR_HANDTORQUE_OVERRIDE_ST)) {
        return true;
    }

    return false;
}

static size_t
tsm_run_situation(uint8* p_event, 
                  const enum tsm_mcu_mrm_func_st mrm_state,
                  const struct tsm_entry* p_entry,
                  const struct tsm_intermediate_sig* p_int_sig) {
    size_t event_num = 0;

    enum fault_level mcu_fault_lvl = p_entry->in_diag->Fault_Level;
    enum nda_func_st cur_nda_st = 
        p_entry->in_can_gate->Soc_Info.automaton_state.NDA_Function_State;
    event_num = 
        tsm_run_initial_sit(p_event, event_num, mcu_fault_lvl, cur_nda_st);

    enum tsm_veh_standstill_st veh_standstill_st =
        p_entry->in_can_gate->Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt;
    boolean can_mrm_activate = tsm_check_activation_cond(p_entry, p_int_sig);
    boolean is_drvr_lng_ctrl = 
            (tsm_is_bit_set(p_int_sig->int_sig_bitfields, 
                            BITNO_LNG_OVERRIDE_ST) ||
             tsm_is_bit_set(p_int_sig->int_sig_bitfields,
                            BITNO_SET_BRAKE));
    if (veh_standstill_st == VEH_STANDSTILL_ST_STANDSTILL) {
        event_num = 
            tsm_run_standstill_sit(p_event, event_num, can_mrm_activate);
    } else if (veh_standstill_st == VEH_STANDSTILL_ST_NOT_STANDSTILL) {
        enum pc_st plan_ctrl_st = 
            p_entry->in_planlite->planningLite_control_state;
        event_num =
            tsm_run_non_standstill_st(p_event, event_num, plan_ctrl_st,
                                      can_mrm_activate, is_drvr_lng_ctrl);
    } else if (true/* warning st */) {
    
    } else {
        // do nothing;
    }

    boolean is_drvr_to = tsm_is_drvr_takeover(mrm_state, p_int_sig);
    event_num = 
        tsm_run_func_exit_sit(p_event, event_num, mcu_fault_lvl, is_drvr_to);

    return event_num;
}

enum tsm_mcu_mrm_func_st
tsm_run_user(const enum tsm_mcu_mrm_func_st mrm_state,
             const struct tsm_entry* p_entry, 
             const struct tsm_intermediate_sig* p_int_sig,
             struct tsm_action* p_action) {
    typedef void (*tsm_post_process) (struct tsm_action* p_action);
    static const tsm_post_process post_process[] = {
        [MCU_MRM_PASSIVE] = tsm_passive_post_process,
        [MCU_MRM_STANDBY] = tsm_standby_post_process,
        [MCU_MRM_FAILURE_LIGHTING] = tsm_lighting_post_process,
        [MCU_MRM_TOR_LNG_LAT_CTRL] = tsm_tor_both_post_process,
        [MCU_MRM_TOR_LAT_CTRL] = tsm_tor_lat_post_process,
        [MCU_MRM_TOR_STAND] = tsm_tor_stand_post_process,
        [MCU_MRM_ACTIVE_LNG_LAT_CTRL] = tsm_mrm_both_post_process,
        [MCU_MRM_ACTIVE_LAT_CTRL] = tsm_mrm_lat_post_process,
        [MCU_MRM_MRC] = tsm_mrc_post_process,
    };  

    uint8 event_id[MAX_EVENT_SIZE];
    size_t event_num = 
        tsm_run_situation(event_id, mrm_state, p_entry, p_int_sig);

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