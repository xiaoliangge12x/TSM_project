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
#include "base/sm_base.h"

// ------------------------------------ global variable definition -------------------------------
static uint32      g_warning_signal_bitfileds = 0U;
static sint64      tor_level3_start_time = 0;
static uint8       tor_level3_timing_flag = 0;

static uint16  K_Tor3RampUpToMrm4Time_Cnt = 500U;

static const struct state_transit warning_state_flow[] = {
    {
        .cur_st = NO_WARNING,
        .event_id = EVENT_COME_LEVEL_1,
        .next_st = WARNING_TOR_LEVEL_1,
    },
    {
        .cur_st = NO_WARNING,
        .event_id = EVENT_COME_LEVEL_2,
        .next_st = WARNING_TOR_LEVEL_2,
    },
    {
        .cur_st = NO_WARNING,
        .event_id = EVENT_COME_LEVEL_3,
        .next_st = WARNING_TOR_LEVEL_3,
    },
    {
        .cur_st = NO_WARNING,
        .event_id = EVENT_CHECK_MRM_ST,
        .next_st = WARNING_MRM_LEVEL_4,
    },
    {
        .cur_st = NO_WARNING,
        .event_id = EVENT_CHECK_MRC_ST,
        .next_st = WARNING_MRM_LEVEL_5,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_1,
        .event_id = EVENT_RAMPUP_LEVEL_2,
        .next_st = WARNING_TOR_LEVEL_2,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_1,
        .event_id = EVENT_RAMPUP_LEVEL_3,
        .next_st = WARNING_TOR_LEVEL_3,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_1,
        .event_id = EVENT_CHECK_MRM_ST,
        .next_st = WARNING_MRM_LEVEL_4,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_1,
        .event_id = EVENT_CHECK_PASSIVE_ST,
        .next_st = NO_WARNING,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_2,
        .event_id = EVENT_RAMPUP_LEVEL_3_WITH_TIME,
        .next_st = WARNING_TOR_LEVEL_3,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_2,
        .event_id = EVENT_CHECK_MRM_ST,
        .next_st = WARNING_MRM_LEVEL_4,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_2,
        .event_id = EVENT_CHECK_PASSIVE_ST,
        .next_st = NO_WARNING,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_3,
        .event_id = EVENT_RAMPUP_LEVEL_4,
        .next_st = WARNING_MRM_LEVEL_4,
    },
    {
        .cur_st = WARNING_TOR_LEVEL_3,
        .event_id = EVENT_CHECK_PASSIVE_ST,
        .next_st = NO_WARNING,
    },
    {
        .cur_st = WARNING_MRM_LEVEL_4,
        .event_id = EVENT_CHECK_MRC_ST,
        .next_st = WARNING_MRM_LEVEL_5,
    },
    {
        .cur_st = WARNING_MRM_LEVEL_4,
        .event_id = EVENT_CHECK_PASSIVE_ST,
        .next_st = NO_WARNING,
    },
    {
        .cur_st = WARNING_MRM_LEVEL_5,
        .event_id = EVENT_CHECK_PASSIVE_ST,
        .next_st = NO_WARNING,
    },
};

static void
tsm_no_warning_post_process() {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "it's in no warnng st");
#endif
}

static void
tsm_tor_lvl_one_post_process() {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "it's in warning tor level 1 st");
#endif
}

static void
tsm_tor_lvl_two_post_process() {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "it's in warning tor level 2 st");
#endif
}

static void
tsm_tor_lvl_three_post_process() {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "it's in warning tor level 3 st");
#endif
}

static void
tsm_mrm_lvl_four_post_process() {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "it's in warning mrm level 4 st");
#endif
}

static void
tsm_mrm_lvl_five_post_process() {
#ifdef _NEED_LOG
    LOG(COLOR_NONE, "it's in warning mrm level 5 st");
#endif
}

static size_t
tsm_run_warning_sit(enum warning_event_id* event_id,
                    const enum tsm_warning_st warning_st,
                    const enum tsm_mcu_mrm_func_st mrm_st,
                    const struct tsm_entry* p_entry, 
                    const struct tsm_intermediate_sig* p_int_sig) {
    size_t event_num = 0;
    static uint16 tor_level3_du_cnt = 0;

    uint8 soc_tor_fault =
        p_entry->in_can_gate->Soc_Info.frc_fault_from_soc;
    boolean is_mcu_in_tor = ((mrm_st == MCU_MRM_TOR_LNG_LAT_CTRL) ||
                             (mrm_st == MCU_MRM_TOR_LAT_CTRL));
    boolean is_mcu_in_mrm = ((mrm_st == MCU_MRM_ACTIVE_LNG_LAT_CTRL) ||
                             (mrm_st == MCU_MRM_ACTIVE_LAT_CTRL));
    if (soc_tor_fault == TOR_LEVEL3_FAULT) {
        event_id[event_num++] = EVENT_COME_LEVEL_3;
    } else if (is_mcu_in_tor) {
        event_id[event_num++] = EVENT_COME_LEVEL_3;
    }

    if (is_mcu_in_mrm) {
        event_id[event_num++] = EVENT_CHECK_MRM_ST;
    } else if (mrm_st == MCU_MRM_MRC) {
        event_id[event_num++] = EVENT_CHECK_MRC_ST;
    } else if (mrm_st == MCU_MRM_PASSIVE) {
        event_id[event_num++] = EVENT_CHECK_PASSIVE_ST;
    } else {
        // do nothing;
    }

    if (is_mcu_in_mrm) {
        event_id[event_num++] = EVENT_RAMPUP_LEVEL_4;
    } else if (warning_st == WARNING_TOR_LEVEL_3) {
        if (tor_level3_du_cnt > K_Tor3RampUpToMrm4Time_Cnt) {
            event_id[event_num++] = EVENT_RAMPUP_LEVEL_4;
            tor_level3_du_cnt = 0;
        } else {
            ++tor_level3_du_cnt;
        }
    }

    return event_num;
}

enum tsm_warning_st
tsm_run_warning_user(const enum tsm_warning_st warning_st, 
                     const enum tsm_mcu_mrm_func_st mrm_st,
                     const struct tsm_entry* p_entry, 
                     const struct tsm_intermediate_sig* p_int_sig) {
    typedef void (*tsm_post_process)();
    static const tsm_post_process post_process[] = {
        [NO_WARNING] = tsm_no_warning_post_process,
        [WARNING_TOR_LEVEL_1] = tsm_tor_lvl_one_post_process,
        [WARNING_TOR_LEVEL_2] = tsm_tor_lvl_two_post_process,
        [WARNING_TOR_LEVEL_3] = tsm_tor_lvl_three_post_process,
        [WARNING_MRM_LEVEL_4] = tsm_mrm_lvl_four_post_process,
        [WARNING_MRM_LEVEL_5] = tsm_mrm_lvl_five_post_process,
    };

    enum warning_event_id event_id[MAX_EVENT_SIZE];
    size_t event_num = 
        tsm_run_warning_sit(event_id, warning_st, mrm_st, p_entry, p_int_sig);
    
    if (event_num == 0) {
        post_process[warning_st]();
        return warning_st;
    }

    enum tsm_warning_st next_warning_st = 
        run_state_transit(warning_state_flow, event_id, event_num, warning_st);
    
    post_process[next_warning_st]();

    return next_warning_st;
}