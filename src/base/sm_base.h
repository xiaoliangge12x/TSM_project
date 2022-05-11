/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  base state machine declaration
 *  \author zxl
 *  \date   2022-04-06
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef SM_BASE_H_
#define SM_BASE_H_

#include "tsm_parameter.h"

#define MAX_EVENT_SIZE 40U

struct state_transit
{
    uint8  cur_st;     
    uint8  event_id;
    uint8  next_st;
};

uint8
run_state_transit(const struct state_transit p_state_transit[],
                  const size_t transit_num, const uint8* event_table, 
                  const size_t event_num, const uint8 cur_st);
#endif