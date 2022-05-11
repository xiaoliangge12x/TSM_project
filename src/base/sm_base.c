/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  base state machine definition
 *  \author zxl
 *  \date   2022-04-06
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "base/sm_base.h"

uint8
run_state_transit(const struct state_transit p_state_transit[],
                  const size_t transit_num, const uint8* event_table, 
                  const size_t event_num, const uint8 cur_st) {
    uint8 next_st = cur_st;
    
    for (size_t i = 0; i < transit_num; ++i) {
        if (cur_st == p_state_transit[i].cur_st) {
            uint8 event = p_state_transit[i].event_id;
            for (size_t j = 0; j < event_num; ++j) {
                if (event_table[j] == event) {
                    next_st = p_state_transit[i].next_st;
                    return next_st;
                }
            }
        }
    }
    return next_st;
}