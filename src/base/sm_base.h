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
// ------------------------ macro   ------------------------------------
#define MAX_STATE_TRANSIT_SIZE 50U
#define MAX_EVENT_SIZE         30U
#define MAX_STATE_SIZE         30U
// ------------------------ typedef ------------------------------------
typedef void (*Action) ();
typedef boolean (*TransitEvent) (const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);

typedef struct 
{
    uint8  cur_st;     
    uint8  event_id;
    uint8  next_st;
} StateTransit;

typedef struct 
{
    uint8        event_id;
    TransitEvent transit_event;
} Event;

typedef struct
{
    uint8  state;
    Action action;
} State;

typedef struct 
{
    StateTransit state_transit_table[MAX_STATE_TRANSIT_SIZE];
    uint8        state_transit_size;
    Event        event_table[MAX_EVENT_SIZE];
    uint8        event_size;
    State        state_table[MAX_STATE_SIZE];
    uint8        state_size;
} StateMachine;

extern void StateMachineWork(const StateMachine* state_machine, const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, uint8* cur_state);
#endif