#ifndef TSM_DATA_H_
#define TSM_DATA_H_

#include "Rte_Type.h"
#include "data_type.h"

typedef void (*Action[])();
typedef boolean (*TransitEvent[])();
// 定义全局变量(可标定量)
#define TOTAL_TRANS_NUM 23
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
    NDA_DISABLE = 0,
    NDA_PASSIVE,
    NDA_STANDBY_NORMAL,
    NDA_STANDBY_PHASE_IN,
    NDA_ACTIVE_HAND_ON_NORMAL,
    NDA_ACTIVE_HAND_FREE_NORMAL,
    NDA_ACTIVE_HAND_ON_STANDWAIT,
    NDA_ACTIVE_HAND_FREE_STANDWAIT,
    NDA_ACTIVE_HAND_ON_STANDACTIVE,
    NDA_ACTIVE_HAND_FREE_STANDACTIVE,
    NDA_ACTIVE_EPB_PHASE_IN,
    NDA_LAT_OVERRIDE,
    NDA_LNG_OVERRIDE_HANDS_ON,
    NDA_LNG_OVERRIDE_HANDS_FREE,
    NDA_LAT_LNG_OVERRIDE,
    NDA_TOR_WITH_LAT_CONTROL,
    NDA_TOR_WITH_LAT_LNG_CONTROL,
    NDA_TOR_WITH_STAND,
    NDA_MRM_EMERGENCY_LANE_WITH_LAT_CONTROL,
    NDA_MRM_EMERGENCY_LANE_WITH_LAT_LNG_CONTROL,
    NDA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_CONTROL,
    NDA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_LNG_CONTROL,
    NDA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_CONTROL,
    NDA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_LNG_CONTROL,
    NDA_MRC,
    NDA_FAILURE_LIGHT,
    NDA_FAILURE_NO_LIGHT
};

typedef enum IcaFunctionSt
{
    ICA_DISABLE = 0,
    ICA_PASSIVE,
    ICA_STANDBY,
    ICA_ACTIVE_HAND_ON_NORMAL,
    ICA_ACTIVE_HAND_ON_STANDWAIT,
    ICA_ACTIVE_HAND_ON_STAICACTIVE,
    ICA_ACTIVE_HAND_FREE_STAICACTIVE,
    ICA_ACTIVE_EPB_PHASE_IN,
    ICA_LAT_OVERRIDE,
    ICA_LNG_OVERRIDE_HANDS_ON,
    ICA_LNG_OVERRIDE_HANDS_FREE,
    ICA_LAT_LNG_OVERRIDE,
    ICA_TOR_WITH_LAT_CONTROL,
    ICA_TOR_WITH_LAT_LNG_CONTROL,
    ICA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_CONTROL,
    ICA_MRM_EGO_LANE_COMFORTABLE_WITH_LAT_LNG_CONTROL,
    ICA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_CONTROL,
    ICA_MRM_EGO_LANE_EMERGENCY_WITH_LAT_LNG_CONTROL,
    ICA_MRC,
    ICA_FAILURE_LIGHT,
    ICA_FAILURE_NO_LIGHT
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
    EVENT_1 = 0,
    EVENT_2,
    EVENT_3,
    EVENT_4,
    EVENT_5,
    EVENT_6,
    EVENT_7,
    EVENT_8,
    EVENT_9,
    EVENT_10,
    EVENT_11,
    EVENT_12,
    EVENT_13,

    EVENT_COUNT,
    EVENT_NONE = 0xFF
};
// 定义内部结构体
// ------------------ 中间变量 ---------------

typedef struct
{
    enum MrmSystemFaultLevel mrm_system_fault_level;   // mrm 系统故障等级
    uint8 mrm_failure_lighting_flag;                   // mrm 故障点灯标志位
    Dt_RECORD_Automaton_State automaton_st;            // soc状态机状态 (option)
    enum MrmType mrm_type;                             // mrm 类型
    uint8 automaton_transit_normal_flag;               // soc状态机跳转是否正常标志位
    enum OverrideSt driver_hand_torque_st;       // 驾驶员手力矩超越标志
    enum BrakeInterventionType brake_intervention_type; // 刹车介入类型
    enum OverrideSt lng_override_st;                 // 纵向超越标志位
    enum LaneChangeState lane_change_st;                // 车辆变道状态(option)
    enum DrvrAttentionSt driver_attention_st;           // 驾驶员注意力状态
    uint8 hands_can_takeover;                           // 手可以接管标志位
    uint8 planning_control_st;                                // 规控能力(option)
    uint8 brake_is_set;                                       // 刹车是否踩下
    uint8 vehicle_standstill_flag;                            // 车辆是否静止
} InterMediaMsg;

typedef struct 
{
    enum State cur_st;     // 当前状态
    enum EventID event_id;   // 触发事件, 共通的
    enum State next_st;    // 下一状态
} StateTransit;

typedef struct 
{
    enum State state;      // 当前状态
    InterMediaMsg inter_media_msg;
} StateMachine;
// Action ID 对应 action 函数数组的Index 
#endif