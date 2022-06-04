/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  tsm output declaration
 *  \author zxl
 *  \date   2022-04-26
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_USER_H_
#define TSM_USER_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "tsm_parameter.h"

enum tsm_ctrl_arb_req
{
    CTRLARB_RESPOND_TO_SOC = 0,
    CTRLARB_RESPOND_TO_IFC,
};

enum tsm_ovrd_st
{
    OVERRIDE_NOT_SATISFY = 0,
    OVERRIDE_SATISFY,
    OVERRIDE_INVALID,
};

enum tsm_mrm_st
{
    MRM_ST_TOR = 0,
    MRM_ST_ACTIVE,
    MRM_ST_ES,
    MRM_ST_INVALID,
};

struct tsm_action {
    enum tsm_ovrd_st lng_override_flag;
    enum tsm_ovrd_st lat_override_flag;
    enum tsm_mrm_st mrm_activation_st;
    enum tsm_ctrl_arb_req control_arb_request;
    uint8 request_mrm;
    uint8 tor_warning_level;
    uint8 mrm_available_st;
    uint16*  p_passive_timer_cnt;
};

enum tsm_ifc_mrm_func_st
tsm_run_user(const enum tsm_ifc_mrm_func_st mrm_state,
             const enum tsm_warning_st warning_state,
             const struct tsm_entry* p_entry, 
             const struct tsm_intermediate_sig* p_int_sig,
             struct tsm_action* p_action);

#ifdef __cplusplus
}
#endif

#endif