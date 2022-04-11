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
#define MAX_STATE_TRANSIT_SIZE 80U
#define MAX_EVENT_SIZE         40U
#define MAX_STATE_SIZE         40U
// ------------------------ typedef ------------------------------------
typedef void (*Action) ();
typedef boolean (*TransitEvent) ();

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
    Event        event_table[MAX_EVENT_SIZE];
    State        state_table[MAX_STATE_SIZE];
} StateMachine;

extern void StateMachineWork(const StateMachine* state_machine, uint8* cur_state);
#endif