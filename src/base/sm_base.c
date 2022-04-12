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

void StateMachineWork(const StateMachine* state_machine, uint8* cur_state)
{
    uint8 eventId_Table[MAX_EVENT_SIZE];
    uint8 size = 0;
    // 找到触发事件ID的数组
    uint8 event_index   = 0;
    while ((event_index < MAX_EVENT_SIZE) && (state_machine->event_table[event_index].transit_event != NULL_PTR)) {
        if (state_machine->event_table[event_index].transit_event()) {
            eventId_Table[size++] = state_machine->event_table[event_index].event_id;
        }
        ++event_index;
    }

    uint8 transit_index = 0;
    if (size != 0) {
        while ((transit_index < MAX_STATE_TRANSIT_SIZE) && 
            (state_machine->state_transit_table[transit_index].cur_st != 0)) {
            if (*cur_state == state_machine->state_transit_table[transit_index].cur_st) {
                uint8 is_found = 0;
                for (uint8 j = 0; j < size; ++j) {
                    if (eventId_Table[j] == state_machine->state_transit_table[transit_index].event_id) {
                        *cur_state = state_machine->state_transit_table[transit_index].next_st;
                        is_found = 1;
                        break;
                    }
                }
                if (is_found) {
                    break;
                }
            }
            ++transit_index;
        }
    }

    uint8 state_index   = 0;
    while ((state_index < MAX_STATE_SIZE) && state_machine->state_table[state_index].action != NULL_PTR) {
        if (*cur_state == state_machine->state_table[state_index].state) {
            state_machine->state_table[state_index].action();
            break;
        }
        ++state_index;
    }
}