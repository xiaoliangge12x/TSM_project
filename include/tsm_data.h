/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Necessary Data Needed by TSM definition
 *  \author zengxiaoliang (zengxiaoliang@holomatic.com)
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_DATA_H_
#define TSM_DATA_H_

#include "TSM.h"

// 可标定量
static uint16 K_BrakPedalAppliedThresholdTime_Cnt = 10U;    // 制动判断的持续时间(20ms周期)
static uint16 K_GasPedalAppliedThresholdTime_Cnt = 10U;    // 油门踩下的持续时间
static uint16 K_LngOverrideTakeOverTime_Cnt      = 500U;  // 10s
static uint16 K_BrakeTOR_TimeThreshold_Cnt      = 150U;   // 刹车长时介入时间阈值， 3s
static uint16 K_OverrideHandTorqCheckTime_Cnt     = 50U;    // 手力矩是否超越的持续时间， 暂定1s
static float32 K_OverrideHandTorqThreshold_LessTwoZone = 1.8;   // 少于2区的手力矩是否超越的手扶阈值
static float32 K_OverrideHandTorqThreshold_TwoZone = 1.5;    // 2区的是否超越的手力矩阈值
static float32 K_OverrideHandTorqThreshold_ThreeZone = 1.0;   // 3区的是否超越的手力矩阈值
static float32 K_TakeOverAvailHandTorqThreshold_LessTwoZone = 0U;   // 少于2区的具备接管能力的手扶阈值
static float32 K_TakeOverAvailHandTorqThreshold_TwoZone = 0U;    // 2区的具备接管能力的手力矩阈值
static float32 K_TakeOverAvailHandTorqThreshold_ThreeZone = 0U;   // 3区的具备接管能力的手力矩阈值
static float32 K_GasPedalPosThresholdValue = 20.0;     // 油门开度阈值

// 定义宏常量
#define TOTAL_TRANS_NUM 23
#define MONITOR_ARRAY_SIZE 39
#define MAX_FRAME_CNT   5

// 函数指针的类型定义
typedef void (*Action[])();
typedef boolean (*TransitEvent[])(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
typedef boolean (*NdaTransitCondition)();

// 定义外部枚举量
typedef enum DMSDrvrDetSts
{
    UNAVAILABLE_DMS_DRIVER_DET_ST = 0, 
    NOT_DETECTED,
    EYE_DETECTED,
    HEAD_DETECTED,
};

typedef enum DMSInterestsRegion
{
    UNAVAILABLE_DMS_INTERESTS_REGION = 0,
    WINDSHIELD,
    REAR_VIEW_MIRROR,
    DRIVER_SIDE_WINDOW,
    PASSENGER_SIDE_WINDOW,
    INSTRUMENT_CLUSTER,
    INFOTAIMENT_DISPLAY,
    DRIVER_SIDE_DOOR_PANEL,
    DRIVER_SIDE_FOOTWELL,
    PASSENGER_SIDE_FOOTWELL,
    PASSENGER_SIDE_DOOR_PANEL,
    OTHER_AREA,
};

typedef enum DMSL3DriverFatigueSt
{
    UNAVAILABLE_DMS_DRIVER_FATIGUE_ST = 0x0U,
    UNKNOWN_DMS_DRIVER_FATIGUE_ST = 0x249U,
    ALERT = 0x492U,
    FATIGUE_DMS_DRIVER_FATIGUE_ST = 0x6DBU,
    HIGHLY_FATIGUE = 0x924U,
    SUSPECTED_UNRESPONSIVE = 0xB6DU,
    CONFIRMED_UNRESPONSIVE = 0xDB6U,
    NOT_USED = 0xFFFU,
};

typedef enum DrvrAttentionSt
{
    AWAKE_AND_NOT_DISTRACTED = 0,
    AWAKE_AND_LOOK_REARVIEW_OR_HU,
    AWAKE_AND_DISTRACTED,
    FATIGUE_DRIVER_ATTENTION_ST,
    UNKNOWN,
};

typedef enum OverrideSt
{
    OVERRIDE_NOT_SATISFY = 2,
    OVERRIDE_SATISFY,
};

typedef enum NDASystemFaultLevel
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
};

// Automaton St Enum
typedef enum NdaFunctionSt
{
    NDA_FAILURE_LIGHTING = 0,
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

typedef enum IcaFunctionSt
{
    ICA_FAILURE_LIGHTING = 0,
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
    ICA_MRC_SATATE,
};

typedef enum AccFunctionSt
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

typedef enum BCSVehicleStandStillSt
{
    NOT_STANDSTILL = 0,
    STANDSTILL,
    INVALID,
    NOT_USED_STAND_STILL,
};

typedef enum BrakeInterventionType
{
    NO_BRAKE_INTERVENTION = 0,
    SHORT_TERM_INTERVENTION,
    LONG_TERM_INTERVENTION,
};

typedef enum MCUMRMFunctionSt
{
    MCU_MRM_PASSIVE = 0,
    MCU_MRM_STANDBY,
    MCU_MRM_FAILURE_LIGHTING,
    MCU_MRM_FAILURE_NO_LIGHTING,
    MCU_MRM_ACTIVE_LNG_LAT_CTRL,
    MCU_MRM_ACTIVE_LAT_CTRL,
    MCU_MRM_MRC
};

typedef enum MrmSystemFaultLevel
{
    NO_FAULT = 0,
    FAILURE_FAULT,
    SAFE_STOP_FAULT,
    OTHER
};

typedef enum MrmType
{
    COMFORTABLE_STOP = 0,
    EMERGENCY_STOP
};

typedef enum LaneChangeState
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
};

typedef enum AccDriverOrvd
{
    NO_OVERRIDE = 0,
    DRIVER_OVERRIDE
};

typedef enum BrkPedalApplied
{
    BRAKE_PEDAL_NOT_APPLIED = 0,
    BRAKE_PEDAL_APPLIED
};

typedef enum
{
    NOT_INITIALIZED = 0,
    NORMAL,
    FAULTY,
    RESERVED
} BrkPedalAppliedSt;

// 定义状态机枚举量
typedef enum State
{
    PASSIVE = 0,
    FAILURE_LIGHTING,
    FAILURE_NO_LIGHTING,
    STANDBY,
    MRM_LNG_LAT_CTRL,
    MRM_LAT_CTRL,
    MRC
};

typedef enum EventID
{
    EVENT_FAULT_NOT_EXIST = 0,
    EVENT_LIGHTING,
    EVENT_NO_LIGHTING,
    EVENT_STANDBY,
    EVENT_NO_STANDBY,
    EVENT_MRM_BOTH_CTRL,
    EVENT_MRM_LAT_CTRL,
    EVENT_MRC,
    EVENT_MRC_FROM_MRM,
    EVENT_LIGHTING_FROM_MRM,
    EVENT_NO_LIGHTING_FROM_MRM,

    EVENT_COUNT,
    EVENT_NONE = 0xFF
};

typedef enum NdaTransitEnableFlag
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
};

typedef enum {
    OFF_NDA_LAT_ST = 0,
    READY_NDA_LAT_ST,
    NDA_LATERAL_HANDSON_MODE_ACTIVE,
    NDA_LATERAL_HANDSOFF_MODE_ACTIVE,
    LAT_DRIVER_OVERRIDE,
} NdaLatSt;

typedef enum {
    OFF_NDA_LNG_ST = 0,
    READY_NDA_LNG_ST,
    NDA_LONG_HANDSON_MODE_ACTIVE,
    NDA_LONG_HANDSOFF_MODE_ACTIVE,
    LONG_DRIVER_OVERRIDE,
} NdaLngSt;

typedef enum {
    OFF_NDA_ILC_ST = 0,
    READY_NDA_ILC_ST,
    NDA_LANE_CHANGING,
} NdaILCSt;
// 定义内部结构体
// ------------------ 通用变量 --------------
typedef struct {
    uint8 flag_set_val;
    uint8 flag_unset_val;
    uint16 time_threshold_cnt;
} VarValue;

// ------------------ 中间变量 ---------------
typedef struct
{
    enum NdaTransitEnableFlag nda_transit_enable_flag; 
    uint8_t frame_cnt;   // 帧数计数器
} NdaStTransitMonitor;

typedef struct 
{
    enum NdaFunctionSt start_st;
    enum NdaFunctionSt next_st;
    enum NdaTransitEnableFlag transit_enable_flag;
    NdaTransitCondition nda_transit_cond;
} NdaStMonitorInfo;

typedef struct
{
    enum MrmSystemFaultLevel mrm_system_fault_level;    // mrm 系统故障等级
    uint8 mrm_failure_lighting_flag;                    // mrm 故障点灯标志位
    Dt_RECORD_Automaton_State automaton_st;             // soc状态机状态 (option)
    enum MrmType mrm_type;                              // mrm 类型
    uint8 automaton_transit_normal_flag;                // soc状态机跳转是否正常标志位， 需要临时做标定
    enum OverrideSt driver_hand_torque_st;              // 驾驶员手力矩超越标志
    enum BrakeInterventionType brake_intervention_type; // 刹车介入类型
    enum OverrideSt lng_override_st;                    // 纵向超越标志位
    uint8 lng_override_long_duration_flag;              // 纵向超越长时持续标志位
    enum LaneChangeState lane_change_st;                // 车辆变道状态(option)
    enum DrvrAttentionSt driver_attention_st;           // 驾驶员注意力状态
    uint8 hands_can_takeover;                           // 手可以接管标志位
    uint8 planning_control_st;                          // 规控能力(option)
    uint8 brake_is_set;                                 // 刹车是否踩下
    uint8 vehicle_standstill_flag;                      // 车辆是否静止
    // 上述是给状态机的直接输入，以下是间接中间变量
    uint8 nda_need_phase_in;                            // nda 是否需要phase-in
    uint8 nda_passive_vd_flag;                          // nda passive的vd标志位， 1 为valid
    uint8 nda_handsfree_handson_flag;                   // HandsFree 和 HandsOn 的配置码
    uint8 driver_acc_pedal_applied_flag;                // 驾驶员是否踩下油门
    NdaStTransitMonitor nda_st_transit_monitor;         // nda状态跳转使能标志位
    Dt_RECORD_Automaton_State last_automaton_st;        // 上一帧soc侧automaton状态 
} InterMediaMsg;

typedef struct 
{
    enum State cur_st;     // 当前状态
    enum EventID event_id; // 触发事件, 共通的
    enum State next_st;    // 下一状态
} StateTransit;

typedef struct 
{
    uint8 lng_override_flag;
    uint8 mrm_activation_st;   // 给到planlite的激活信号
} ActionParam;

typedef struct 
{
    enum State state;              // 当前状态
    InterMediaMsg inter_media_msg; // 中间信号
    ActionParam action_param;      // 行为参数
} StateMachine;

typedef struct 
{
    NdaLatSt NDA_LatState;
    NdaILCSt NDA_ILC_State;
    NdaLngSt NDA_LongState;
} NdaActivationSt;

// 声明全局变量
extern StateMachine tsm;
#endif