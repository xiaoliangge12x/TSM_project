/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Monitor SOC NDA state transit definition
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_MONITOR_H_
#define TSM_MONITOR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "tsm_parameter.h"

#define MONITOR_ARRAY_SIZE 39

void 
tsm_run_monitor(const struct tsm_entry* p_entry, 
                struct tsm_intermediate_sig* p_int_sig);

#ifdef __cplusplus
}
#endif

#endif