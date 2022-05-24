/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Necessary Data Needed by TSM definition
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_PARAMETER_H_
#define TSM_PARAMETER_H_

#include "TSM.h"
#ifdef _NEED_LOG
#include "common.h"
#endif
#ifdef _DEBUG_MODE
#include <assert.h>
#endif

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0])) 

typedef unsigned long size_t;

typedef Dt_RECORD_VehicleSignal2TSM tsm_veh_sig;
typedef Dt_RECORD_Soc_Info tsm_soc_info;
typedef Dt_RECORD_Automaton_State tsm_soc_st;
typedef Dt_RECORD_Diag2TSM tsm_diag;

enum tsm_drvr_attention_st {
    DRVR_AWAKE_NOT_DISTRACTED,
    DRVR_AWAKE_DISTRACTED,
    DRVR_FATIGUE,
};

enum nda_func_st {
    NDA_FAILURE_LIGHTING = 0U,
    NDA_FAILURE_NO_LIGHTING,
    NDA_DISABLE,
    NDA_PASSIVE,
    NDA_STANDBY_NORMAL,
    NDA_STANDBY_PHASE_IN,
    NDA_ACTIVE_HANDS_FREE_NORMAL,
    NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE,
    NDA_ACTIVE_HANDS_FREE_STAND_WAIT,
    NDA_ACTIVE_HANDS_ON_NORMAL,
    NDA_ACTIVE_HANDS_ON_STAND_ACTIVE,
    NDA_ACTIVE_HANDS_ON_STAND_WAIT,
    NDA_ACTIVE_EPB_PHASE_IN,
    NDA_LNG_OVERRIDE,
    NDA_LNG_LAT_OVERRIDE,
    NDA_LAT_OVERRIDE,
    NDA_TOR_LAT_CTRL,
    NDA_TOR_LNG_LAT_CTRL,
    NDA_TOR_STAND,
    NDA_MRM_ACTIVE_PO_LAT_CTRL,
    NDA_MRM_ACTIVE_PO_LNG_LAT_CTRL,
    NDA_MRM_ACTIVE_CP_LAT_CTRL,
    NDA_MRM_ACTIVE_CP_LNG_LAT_CTRL,
    NDA_MRM_ACTIVE_ES_LAT_CTRL,
    NDA_MRM_ACTIVE_ES_LNG_LAT_CTRL,
    NDA_MRC,
};

enum ica_func_st
{
    ICA_FAILURE_LIGHTING = 0U,
    ICA_FAILURE_NO_LIGHTING,
    ICA_DISABLE,
    ICA_PASSIVE,
    ICA_STANDBY,
    ICA_HANDS_ON_NORMAL,
    ICA_HANDS_ON_STANDACTIVE,
    ICA_HANDS_ON_STANDWAIT,
    ICA_EPB_PHASE_IN,
    ICA_LNG_OVERRIDE,
    ICA_LNG_LAT_OVERRIDE,
    ICA_LAT_OVERRIDE,
    ICA_TOR_LAT_CTRL,
    ICA_TOR_LNG_LAT_CTRL,
    ICA_MRM_PO_LAT_CTRL,
    ICA_MRM_PO_LNG_LAT_CTRL,
    ICA_MRM_CP_LAT_CTRL,
    ICA_MRM_CP_LNG_LAT_CTRL,
    ICA_MRM_ES_LAT_CTRL,
    ICA_MRM_ES_LNG_LAT_CTRL,
    ICA_MRC_STATE,
};

enum acc_func_st
{
    ACC_DISABLE = 0,
    ACC_PASSIVE,
    ACC_STANDBY,
    ACC_ACTIVE_NORMAL,
    ACC_ACTIVE_STANDWAIT,
    ACC_ACTIVE_STANDACTIVE,
    ACC_ACTIVE_EPB_PHASE_IN,
    ACC_OVERRIDE,
    ACC_BOM,
    ACC_FAILURE_LIGHT,
    ACC_FAILURE_NO_LIGHT
};

typedef enum 
{
    NDA_SAFE_PARK_FAULT = 0,
    NDA_TOR_LEVEL_THREE_FAULT,
    NDA_TOR_LEVEL_TWO_FAULT,
    NDA_TOR_LEVEL_ONE_FAULT,
    NDA_CAN_NOT_ACTIVE_FAULT,
    NDA_NO_FAULT,
    DOWN_GRADE_ICA_FAULT,
    DOWN_GRADE_ACC_FAULT,
    NDA_MRM_FAULT,
} NDASystemFaultLevel;

enum tsm_brk_duration_type {
    NO_BRAKE_INTERVENTION,
    SHORT_INTERVENTION,
    LONG_INTERVENTION
};

enum base_state
{
    BASE_ST_INITIAL       = 0,
    BASE_ST_TSM_START     = 10,
    BASE_ST_WARNING_START = 40,
};

enum base_event
{
    BASE_EVENT_INITIAL       = 0,
    BASE_EVENT_TSM_START     = 10,
    BASE_EVENT_WARNING_START = 50,
};

enum tsm_ifc_mrm_func_st
{
    IFC_PASSIVE = BASE_ST_TSM_START + 1,
    IFC_STANDBY,
    IFC_FAILURE,
    IFC_ACTIVE_SUPPORT_SOC,
    IFC_TOR_LNG_LAT_CTRL,
    IFC_TOR_LAT_CTRL,
    IFC_TOR_STAND,
    IFC_MRM_LNG_LAT_CTRL,
    IFC_MRM_LAT_CTRL,
    IFC_MRM_EMERGENCY_BOTH_CTRL,
    IFC_MRM_MRC
};

enum tsm_warning_st {
    NO_WARNING = BASE_ST_WARNING_START + 1,
    WARNING_TOR_LEVEL_1,
    WARNING_TOR_LEVEL_2,
    WARNING_TOR_LEVEL_3,
    WARNING_MRM_LEVEL_4,
    WARNING_MRM_LEVEL_5,
};

enum tsm_veh_standstill_st
{
    VEH_STANDSTILL_ST_NOT_STANDSTILL = 0,
    VEH_STANDSTILL_ST_STANDSTILL,
    VEH_STANDSTILL_ST_INVALID,
    VEH_STANDSTILL_ST_NOT_USED,
};

enum fault_level
{
    NO_FAULT = 0,
    FAILURE_FAULT,
    SAFE_STOP_FAULT,
    TOR_LEVEL1_FAULT,
    TOR_LEVEL2_FAULT,
    TOR_LEVEL3_FAULT,
    OTHER
};

typedef enum 
{
    COMFORTABLE_STOP = 0,
    EMERGENCY_STOP
} MrmType;

typedef enum
{
    NO_REQUEST = 0U,
    DRIVEOFF_REQUEST,
    LANE_CHANGE_LEFT_REQUEST,
    LANE_CHANGE_RIGHT_REQUEST,
    LANE_CHANGE_CANCEL_REQUEST,
} NdaPlanningRequest;

typedef enum 
{
    NO_LANE_CHANGE = 0U,
    REQUEST_LANE_CHANGE_LEFT,
    REQUEST_LANE_CHANGE_RIGHT,
    PREPARE_LANE_CHANGE_LEFT,
    PREPARE_LANE_CHANGE_RIGHT,
    CANCEL_LANE_CHANGE_LEFT,
    CANCEL_LANE_CHANGE_RIGHT,
    LANE_CHANGE_LEFT,
    LANE_CHANGE_RIGHT
} LaneChangeState;

enum tsm_bitno_int_sig
{
    BITNO_FAILURE_LIGHTING_FLAG,
    BITNO_DRVR_HANDTORQUE_OVERRIDE_ST,
    BITNO_LNG_OVERRIDE_ST,
    BITNO_LONG_TIME_LNG_OVERRIDE,
    BITNO_HANDS_CAN_TAKEOVER,
    BITNO_SET_BRAKE,
    BITNO_NDA_AVL_BEFORE_ACT,
    BITNO_NDA_AVL_AFTER_ACT,
    BITNO_AS_ACTIVE,
};

struct tsm_intermediate_sig {
    enum tsm_brk_duration_type brk_du_type;
    enum tsm_drvr_attention_st drvr_att_st;
    uint32 int_sig_bitfields; 
};

struct tsm_entry
{
    Dt_RECORD_CtrlArb2TSM*            in_ctrl_arb;
    Dt_RECORD_DecisionArbitrator2TSM* in_deci_arb;
    Dt_RECORD_CANGATE2TSM*            in_can_gate;
    Dt_RECORD_Diag2TSM*               in_diag;
    Dt_RECORD_PLANLITE2TSM*           in_planlite;
};

struct tsm_exit
{
    Dt_RECORD_TSM2PLANLITE*           out_planlite;
    Dt_RECORD_TSM2CtrlArb*            out_ctrl_arb;
    Dt_RECORD_TSM2DecisionArbitrator* out_deci_arb;
    Dt_RECORD_TSM2Diag*               out_diag;
    Dt_RECORD_TSM2HMI*                out_hmi;
    Dt_RECORD_TSM2CANGATE*            out_can_gate;
};

#ifdef CONSUME_TIME

void 
tsm_start_timing(sint64* cur_time, uint8* flag);

void 
tsm_stop_timing(uint8* flag);

float32
tsm_get_delta_time(const sint64 start_time, const uint8 flag);

#endif

boolean 
tsm_is_bit_set(const uint32 bitfields, const uint8 bitno);

void 
tsm_set_bit_in_bitfields(uint32* bitfields, const uint8 bitno);

void
tsm_reset_bit_in_bitfields(uint32* bitfields, const uint8 bitno);

boolean
tsm_is_mrm_active(const enum tsm_ifc_mrm_func_st mrm_st);

#endif