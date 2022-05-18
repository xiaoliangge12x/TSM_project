/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Signal Preprocess for TSM definition
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_SIGNAL_HANDLING_H_
#define TSM_SIGNAL_HANDLING_H_

#ifdef _NEED_LOG
#include "common.h"
#endif

#include "tsm_parameter.h"

extern uint16  K_Standstill_waitTime_Cnt;

void 
tsm_preprocess_input(const struct tsm_entry* p_entry,
                     const enum tsm_mcu_mrm_func_st mrm_st,
                     struct tsm_intermediate_sig* p_int_sig);

#endif