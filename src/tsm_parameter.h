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

// ----------------------  macro ------------------------------------------------------
// #define CONSUME_TIME

// ----------------------  global variable(calibration) -------------------------------
extern uint16  K_BrakPedalAppliedThresholdTime_Cnt;         
extern uint16  K_GasPedalAppliedThresholdTime_Cnt;         
extern uint16  K_LngOverrideTakeOverTime_Cnt;                
extern uint16  K_BrakeTOR_TimeThreshold_Cnt;                 
extern uint16  K_OverrideHandTorqCheckTime_Cnt;
extern uint16  K_Tor3RampUpToMrm4Time_Cnt;

extern uint16  K_MissQuitTime_Cnt;
extern uint16  K_MissOverrideTime_Cnt;
extern uint16  K_StuckOverrideTime_Cnt;

extern float32 K_BrakPedalAppliedThresholdTime;
extern float32 K_GasPedalAppliedThresholdTime;
extern float32 K_LngOverrideTakeOverTime;
extern float32 K_BrakeTOR_TimeThreshold;
extern float32 K_OverrideHandTorqCheckTime;
extern float32 K_Tor3RampUpToMrm4Time;

extern float32 K_OverrideHandTorqThreshold_LessTwoZone;
extern float32 K_OverrideHandTorqThreshold_TwoZone;
extern float32 K_OverrideHandTorqThreshold_ThreeZone;
extern float32 K_TakeOverAvailHandTorqThreshold_LessTwoZone;
extern float32 K_TakeOverAvailHandTorqThreshold_TwoZone;
extern float32 K_TakeOverAvailHandTorqThreshold_ThreeZone;
extern float32 K_GasPedalPosThresholdValue;

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
    OVERRIDE_NOT_SATISFY = 0,
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

// Automaton St Enum
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
    MCU_MRM_TOR_LNG_LAT_CTRL,
    MCU_MRM_TOR_LAT_CTRL,
    MCU_MRM_TOR_STAND,
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
    TOR_LEVEL1_FAULT,
    TOR_LEVEL2_FAULT,
    TOR_LEVEL3_FAULT,
    OTHER
} MrmSystemFaultLevel;

typedef enum 
{
    COMFORTABLE_STOP = 0,
    EMERGENCY_STOP
} MrmType;

typedef enum
{
    FUNC_NOT_AVAILABLE = 0,
    FUNC_HANDSFREE,
    FUNC_HANDSON,
} HandsFreeOnFuncFlag;

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

typedef enum
{
    BITNO_FAILURE_LIGHTING_FLAG = 0U,
    BITNO_NDA_TRANSIT_NORMAL_FLAG,
    BITNO_DRVR_HANDTORQUE_OVERRIDE_ST,
    BITNO_LNG_OVERRIDE_ST,
    BITNO_LONG_TIME_LNG_OVERRIDE,
    BITNO_HANDS_CAN_TAKEOVER,
    BITNO_SET_BRAKE,
    BITNO_PHASE_IN_AVAILABLE,
    BITNO_NDA_NEED_PHASE_IN,
    BITNO_NDA_PASSIVE_VD,
    BITNO_NDA_AVL_BEFORE_ACT,
    BITNO_NDA_AVL_AFTER_ACT,
    BITNO_DRVR_ACC_PEDAL_APPLIED,
    BITNO_AS_ACTIVE,
} BitNoForInterMediaSig;

// ------------------ 中间变量 ---------------
typedef struct
{
    MrmSystemFaultLevel       mrm_system_fault_level;           // mrm 系统故障等级   待定
    MrmType                   mrm_type;                         // mrm 类型            
    BrakeInterventionType 	  brake_intervention_type;          // 刹车介入类型             
    DrvrAttentionSt           driver_attention_st;              // 驾驶员注意力状态              
    Dt_RECORD_Automaton_State last_automaton_st;        		// 上一帧soc侧automaton状态
    HandsFreeOnFuncFlag       handsfree_handson_func_flag;      // HandsFree 和 HandsOn 的功能标志位
    uint32                    intermediate_sig_bitfields;       // bool型变量的中间信号位域
} InterMediaMsg;

typedef struct 
{
    uint8 lng_override_flag;
    uint8 lat_override_flag;
    uint8 mrm_activation_st;
    uint8 control_arb_request;
    uint8 request_mrm;
} TSMActionParam;

typedef struct 
{
    MCUMRMFunctionSt state;              // 当前状态
    TSMActionParam   tsm_action_param;      // 行为参数
} TSMParam;

// --------------------------------- global declaration ----------------------------
extern InterMediaMsg g_inter_media_msg;
// 声明全局变量
extern TSMParam g_tsm;
// --------------------------------- function declaration --------------------------

#ifdef CONSUME_TIME
void StartTiming(sint64* cur_time, uint8* flag);   // 开始计时
void StopTiming(uint8* flag);    // 停止计时
float32 GetTimeGapInSec(const sint64 start_time, const uint8 flag); // 计算持续时间
#endif

boolean IsBitSet(const uint32 event_bitfields, const uint8 bit_no);
void SetSignalBitFields(uint32* event_bitfields, const uint8 bit_no);
void ResetSignalBitFields(uint32* event_bitfields, const uint8 bit_no);
#endif