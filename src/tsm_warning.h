/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  TSM warning sm declaration
 *  \author zxl
 *  \date   2022-04-06
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_WARNING_H_
#define TSM_WARNING_H_

#include "tsm_parameter.h"

enum warning_event_id {
    EVENT_CHECK_PASSIVE_ST = BASE_EVENT_WARNING_START + 1,
    EVENT_COME_LEVEL_1,
    EVENT_COME_LEVEL_2,
    EVENT_COME_LEVEL_3,
    EVENT_CHECK_MRM_ST,
    EVENT_CHECK_MRC_ST,
    EVENT_RAMPUP_LEVEL_2,
    EVENT_RAMPUP_LEVEL_3,
    EVENT_RAMPUP_LEVEL_3_WITH_TIME,
    EVENT_RAMPUP_LEVEL_4,
};

enum tsm_warning_st
tsm_run_warning_user(const enum tsm_warning_st warning_st, 
                     const enum tsm_ifc_mrm_func_st mrm_st,
                     const struct tsm_entry* p_entry, 
                     const struct tsm_intermediate_sig* p_int_sig);


#endif