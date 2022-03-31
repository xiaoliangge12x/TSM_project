#ifndef IHBC_HSM_H_
#define IHBC_HSM_H_

#include "common.h"
#include "base/hsm_chart.h"
#include "ihbc_hsm_data.h"

void InitUser();

void RunUser();

// 状态分发函数
HsmRet dispatchForDisable(const uint8_t event);
HsmRet dispatchForFailure(const uint8_t event);
HsmRet dispatchForEnable(const uint8_t event);
HsmRet dispatchForPassive(const uint8_t event);
HsmRet dispatchForStandby(const uint8_t event);
HsmRet dispatchForActive(const uint8_t event);
HsmRet dispatchForLowBeamRunning(const uint8_t event);
HsmRet dispatchForHighBeamRunning(const uint8_t event);

// 状态entry函数

// 状态exit函数

// 状态work函数
void workInDisable();
void workInFailure();
void workInEnable();
void workInPassive();
void workInStandby();
void workInActive();
void workInLowBeamRunning();
void workInHighBeamRuning();

#endif