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

void StateMachineWork(const StateMachine* state_machine, const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, uint8* cur_state)
{
    uint8 eventId_Table[MAX_EVENT_SIZE];
    uint8 size = 0;
    // 找到触发事件ID的数组
    for (uint8 i = 0; i < state_machine->event_size; ++i) {
        if (state_machine->event_table[i].transit_event(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM)) {
            eventId_Table[size++] = state_machine->event_table[i].event_id;
        }
    }

    if (size != 0) {
        for (uint8 i = 0; i < state_machine->state_transit_size; ++i) {
            if (*cur_state == state_machine->state_transit_table[i].cur_st) {
                uint8 is_found = 0;
                for (uint8 j = 0; j < size; ++j) {
                    if (eventId_Table[j] == state_machine->state_transit_table[i].event_id) {
                        *cur_state = state_machine->state_transit_table[i].next_st;
                        is_found = 1;
                        break;
                    }
                }
                if (is_found) {
                    break;
                }
            }
        }
    }
    for (uint8 i = 0; i < state_machine->state_size; ++i) {
        if (*cur_state == state_machine->state_table[i].state) {
            state_machine->state_table[i].action();
        }
    }
}