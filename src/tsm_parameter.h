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

#include "TSM_MODULE.h"
#ifdef _NEED_LOG
#include "common.h"
#endif

// ---------------------------------- typedef   ------------------------------------------------
typedef enum 
{
    AWAKE_AND_NOT_DISTRACTED = 0,
    AWAKE_AND_LOOK_REARVIEW_OR_HU,
    AWAKE_AND_DISTRACTED,
    FATIGUE_DRIVER_ATTENTION_ST,
    UNKNOWN,
} DrvrAttentionSt;

typedef enum 
{
    OVERRIDE_NOT_SATISFY = 2,
    OVERRIDE_SATISFY,
} OverrideSt;

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

typedef enum
{
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
} NdaFunctionSt;

typedef enum
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
} IcaFunctionSt;

typedef enum 
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
} AccFunctionSt;

typedef enum 
{
    VEH_STANDSTILL_ST_NOT_STANDSTILL = 0,
    VEH_STANDSTILL_ST_STANDSTILL,
    VEH_STANDSTILL_ST_INVALID,
    VEH_STANDSTILL_ST_NOT_USED,
} BCSVehicleStandStillSt;

typedef enum
{
    NO_BRAKE_INTERVENTION = 0,
    SHORT_TERM_INTERVENTION,
    LONG_TERM_INTERVENTION,
} BrakeInterventionType;

typedef enum
{
    BASE_ST_INITIAL       = 0,
    BASE_ST_TSM_START     = 10,
    BASE_ST_WARNING_START = 40,
} BaseState;

typedef enum
{
    BASE_EVENT_INITIAL       = 0,
    BASE_EVENT_TSM_START     = 10,
    BASE_EVENT_WARNING_START = 50,
} BaseEvent;

typedef enum
{
    MCU_MRM_PASSIVE = BASE_ST_TSM_START + 1,
    MCU_MRM_STANDBY,
    MCU_MRM_FAILURE_LIGHTING,
    MCU_MRM_FAILURE_NO_LIGHTING,
    MCU_MRM_ACTIVE_LNG_LAT_CTRL,
    MCU_MRM_ACTIVE_LAT_CTRL,
    MCU_MRM_MRC
} MCUMRMFunctionSt;

typedef enum
{
    NO_WARNING = BASE_ST_WARNING_START + 1,
    WARNING_TOR_LEVEL_1,
    WARNING_TOR_LEVEL_2,
    WARNING_TOR_LEVEL_3,
    WARNING_MRM_LEVEL_4,
    WARNING_MRM_LEVEL_5,
} WarningState;

typedef enum
{
    NO_FAULT = 0,
    FAILURE_FAULT,
    SAFE_STOP_FAULT,
    OTHER
} MrmSystemFaultLevel;

typedef enum 
{
    COMFORTABLE_STOP = 0,
    EMERGENCY_STOP
} MrmType;

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

typedef enum 
{
    NONE = 0,
    STANDBY_HANDSFREE_NORMAL,
    STANDBY_HANDSON_NORMAL,
    STANDBY_HANDSFREE_STANDACTIVE,
    HANDSFREE_NORMAL_HANDSFREE_STANDACTIVE,
    HANDSFREE_NORMAL_BOTH_OVERRIDE,
    HANDSFREE_NORMAL_LAT_OVERRIDE,
    HANDSFREE_NORMAL_LNG_OVERRIDE,
    HANDSFREE_NORMAL_HANDSON_NORMAL,
    HANDSFREE_STANDACTIVE_HANDSFREE_NORMAL,
    HANDSFREE_STANDACTIVE_LNG_OVERRIDE,
    HANDSFREE_STANDACTIVE_BOTH_OVERRIDE,
    HANDSFREE_STANDACTIVE_LAT_OVERRIDE,
    HANDSFREE_STANDWAIT_LNG_OVERRIDE,
    HANDSFREE_STANDWAIT_BOTH_OVERRIDE,
    HANDSFREE_STANDWAIT_LAT_OVERRIDE,
    HANDSON_NORMAL_HANDSON_STANDACTIVE,
    HANDSON_NORMAL_BOTH_OVERRIDE,
    HANDSON_NORMAL_LAT_OVERRIDE,
    HANDSON_NORMAL_LNG_OVERRIDE,
    HANDSON_NORMAL_HANDSFREE_NORMAL,
    HANDSON_STANDACTIVE_HANDSON_NORMAL,
    HANDSON_STANDACTIVE_LNG_OVERRIDE,
    HANDSON_STANDACTIVE_BOTH_OVERRIDE,
    HANDSON_STANDACTIVE_LAT_OVERRIDE,
    HANDSON_STANDWAIT_LNG_OVERRIDE,
    HANDSON_STANDWAIT_BOTH_OVERRIDE,
    HANDSON_STANDWAIT_LAT_OVERRIDE,
    LNG_OVERRIDE_HANDSFREE_NORMAL,
    LNG_OVERRIDE_HANDSON_NORMAL,
    LNG_OVERRIDE_BOTH_OVERRIDE,
    LNG_OVERRIDE_LAT_OVERRIDE,
    LAT_OVERRIDE_HANDSFREE_NORMAL,
    LAT_OVERRIDE_HANDSON_NORMAL,
    LAT_OVERRIDE_BOTH_OVERRIDE,
    LAT_OVERRIDE_LNG_OVERRIDE,
    BOTH_OVERRIDE_HANDSFREE_NORMAL,
    BOTH_OVERRIDE_HANDSON_NORMAL,
    BOTH_OVERRIDE_LNG_OVERRIDE,
    BOTH_OVERRIDE_LAT_OVERRIDE,
} NdaTransitEnableFlag;

typedef struct
{
    NdaTransitEnableFlag nda_transit_enable_flag; 
    uint8                frame_cnt;
} NdaStTransitMonitor;

typedef struct
{
    MrmSystemFaultLevel       mrm_system_fault_level;          // mrm 系统故障等级
    uint8                     mrm_failure_lighting_flag;       // mrm 故障点灯标志位
    MrmType                   mrm_type;                        // mrm 类型
    uint8                     automaton_transit_normal_flag;   // soc状态机跳转是否正常标志位
    OverrideSt                driver_hand_torque_st;           // 驾驶员手力矩超越标志
    BrakeInterventionType     brake_intervention_type;         // 刹车介入类型
    OverrideSt                lng_override_st;                 // 纵向超越标志位
    uint8                     lng_override_long_duration_flag; // 纵向超越长时持续标志位
    DrvrAttentionSt           driver_attention_st;             // 驾驶员注意力状态
    uint8                     hands_can_takeover;              // 手可以接管标志位
    uint8                     brake_is_set;                    // 刹车是否踩下
    uint8                     vehicle_standstill_flag;         // 车辆是否静止
    uint8                     nda_need_phase_in;               // nda 是否需要phase-in
    uint8                     nda_passive_vd_flag;             // nda passive的vd标志位， 1 为valid
    uint8                     driver_acc_pedal_applied_flag;   // 驾驶员是否踩下油门
    NdaStTransitMonitor       nda_st_transit_monitor;          // nda状态跳转使能标志位
    Dt_RECORD_Automaton_State last_automaton_st;               // 上一帧soc侧automaton状态 
} InterMediaMsg;

typedef struct 
{
    uint8 lng_override_flag;
    uint8 mrm_activation_st;
} TSMActionParam;

typedef struct 
{
    MCUMRMFunctionSt state;             
    TSMActionParam   tsm_action_param;
} TSMParam;

// --------------------------------- global declaration ----------------------------
extern InterMediaMsg g_inter_media_msg;
extern TSMParam      g_tsm;
// --------------------------------- function declaration --------------------------
void StartTiming(sint64* cur_time, uint8* flag);   // 开始计时
void StopTiming(uint8* flag);    // 停止计时
float32 GetTimeGapInSec(sint64 start_time, uint8* flag); // 计算持续时间
#endif