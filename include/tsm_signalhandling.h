/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  Signal Preprocess for TSM definition
 *  \author zengxiaoliang (zengxiaoliang@holomatic.com)
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#ifndef TSM_SIGNAL_HANDLING_H_
#define TSM_SIGNAL_HANDLING_H_

#include "tsm_monitor.h"
#ifdef _NEED_LOG
#include "common.h"
#endif

// 总的信号处理入口
void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
                    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);


// 具体处理函数
// 判断 驾驶员注意力状态
void DrvrAttentionStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 纵向超越标志位以及判断 驾驶员是否长时纵向超越
void LngOverrideFlagJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 刹车是否踩下
void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 刹车长时介入标志位
void BrakeInervationFlagJudge();
// 判断 驾驶员是否踩下油门
void DriverGasPedalAppliedJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 驾驶员手力矩超越标志位
void DriveHandTorqueOverrideStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// soc侧状态机跳转监控判断
void MonitorNdaStateTransition(const Dt_RECORD_Automaton_State* automaton_state);
// soc侧状态机跳转错误判断
void NdaStTransitNormalJudge(const Dt_RECORD_VehicleSignal2TSM* vehicle_signal, const Dt_RECORD_Soc_Info* soc_info);

// user defined
void TorqueOverrideStJudgeWithHodDetection(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void TorqueOverrideStJudgeWithoutHodDetection(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
void FlagSetWithTimeCount(uint8* flag_set_var, uint16* time_cnt, const VarValue* var_value);
#endif