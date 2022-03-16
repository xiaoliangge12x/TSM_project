#ifndef TSM_SIGNAL_HANDLING_H_
#define TSM_SIGNAL_HANDLING_H_

#include "tsm_data.h"
#ifdef _NEED_LOG
#include "common.h"
#endif

// 总的信号处理入口
void SignalHandling(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
                    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm);


// 具体处理函数
// 判断 驾驶员注意力状态
void DrvrAttentionStJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 纵向超越标志位
void LngOverrideFlagJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
// 判断 刹车是否踩下
void BrakeIsSetJudge(const Dt_RECORD_VehicleSignal2TSM *vehicle_signal);
#endif