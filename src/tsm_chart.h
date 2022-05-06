/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  MRM top state machine main structure definition
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_CHART_H_
#define TSM_CHART_H_

#include "tsm_parameter.h"
#include "base/sm_base.h"

// ---------------------------------- macro definition     --------------------------------------
#define TIMESTAMP_MAX_NUM     3
#define EPSINON_TIME          ((float32)0.00001)
#define NS_IN_MS              ((float32)1000000.0)
#define UPPER_CYCLE           ((float32)21.0)
#define LOWER_CYCLE           ((float32)19.0)

// ---------------------------------- typedef              --------------------------------------
typedef enum
{
    EVENT_FAULT_NOT_EXIST = BASE_EVENT_TSM_START + 1,
    EVENT_LIGHTING,
    EVENT_NO_LIGHTING,
    EVENT_STANDBY,
    EVENT_NO_STANDBY,
    EVENT_MRM_BOTH_CTRL,
    EVENT_MRM_LAT_CTRL,
    EVENT_MRC,
    EVENT_TOR_BOTH_CTRL,
    EVENT_TOR_LAT_CTRL,
    EVENT_TOR_TO_MRM_BOTH,
    EVENT_TOR_TO_MRM_LAT,
    EVENT_MRM_BOTH_CTRL_SWITCH,
    EVENT_MRM_LAT_CTRL_SWITCH,
    EVENT_TOR_BOTH_CTRL_SWITCH,
    EVENT_TOR_LAT_CTRL_SWITCH,
    EVENT_VEH_STANDSTILL,
    EVENT_FUNCTION_EXIT,
    EVENT_WAIT_EPB_RES,
} EventID;

typedef enum
{
    PC_TOR = 0,
    PC_MRM,
    PC_EXIT,
} PCSt;

typedef enum
{
    BITNO_FAULT_NOT_EXIST = 0,
    BITNO_LIGHTING,
    BITNO_NO_LIGHTING,
    BITNO_STANDBY,
    BITNO_NO_STANDBY,
    BITNO_MRM_BOTH_CTRL,
    BITNO_MRM_LAT_CTRL,
    BITNO_MRC,
    BITNO_TOR_BOTH_CTRL,
    BITNO_TOR_LAT_CTRL,
    BITNO_TOR_TO_MRM_BOTH,
    BITNO_TOR_TO_MRM_LAT,
    BITNO_MRM_BOTH_CTRL_SWITCH,
    BITNO_MRM_LAT_CTRL_SWITCH,
    BITNO_TOR_BOTH_CTRL_SWITCH,
    BITNO_TOR_LAT_CTRL_SWITCH,
    BITNO_VEH_STANDSTILL,
    BITNO_FUNCTION_EXIT,
    BITNO_WAIT_EPB_RES,
} TsmEventBitNo;

// ---------------------------------- global variable def  --------------------------------------
static uint32 g_tsm_signal_bitfileds = 0U;
// ---------------------------------- function declaration --------------------------------------
void RunTsmUser(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean ValidateRcvMsgTimeStamp(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsTimeStampLost(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp);
boolean IsTimeStampError(const Dt_RECORD_TimeStamp* cur_timestamp, const Dt_RECORD_TimeStamp* last_timestamp);
void RunTsmSit(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);
boolean IsDriverTakeOver();
boolean ValidateActivationCond(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM);
boolean IsNDAInActiveSt(const uint8 nda_st);
void SetCtrlType(const uint8 both_ctrl, const uint8 lat_ctrl);


boolean IsMrmSystemFaultNotExist()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_FAULT_NOT_EXIST);
}

boolean IsLightingConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_LIGHTING);
}

boolean IsNoLightingConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_NO_LIGHTING);
}

boolean IsStandbyConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_STANDBY);
}

boolean IsStandbyConditionNotMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_NO_STANDBY);
}

boolean IsMrmBothCtrlConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_BOTH_CTRL);
}

boolean IsMrmLatCtrlConditionMeet()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_LAT_CTRL);
}

boolean IsCanEnterMrcFromStandby() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRC);
}

boolean IsTorBothCtrlCondMeet() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_BOTH_CTRL);
}

boolean IsTorLatCtrlCondMeet() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_LAT_CTRL);
}

boolean IsVehStandStillCondMeet() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_VEH_STANDSTILL);
}

boolean IsFuncExitCondMeet() 
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_FUNCTION_EXIT);
}

boolean IsWaitEpbSt()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_WAIT_EPB_RES);
}

boolean IsSwitchToMrmBothCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_BOTH_CTRL_SWITCH);
}

boolean IsSwitchToMrmLatCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_MRM_LAT_CTRL_SWITCH);
}

boolean IsSwitchToTorBothCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_BOTH_CTRL_SWITCH);
}

boolean IsSwitchToTorLatCtrl()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_LAT_CTRL_SWITCH);
}

boolean IsTorToMrmBoth()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_TO_MRM_BOTH);
}

boolean IsTorToMrmLat()
{
    return IsBitSet(g_tsm_signal_bitfileds, BITNO_TOR_TO_MRM_LAT);
}

#endif