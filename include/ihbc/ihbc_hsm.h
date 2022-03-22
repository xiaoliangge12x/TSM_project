#ifndef IHBC_HSM_H_
#define IHBC_HSM_H_

#include "common.h"
#include "base/hsm_chart.h"
#include "ihbc_hsm_data.h"

void InitUser();

void RunUser();

// 状态分发函数
enum HsmRet dispatchForDisable(const uint8_t event);
enum HsmRet dispatchForFailure(const uint8_t event);
enum HsmRet dispatchForEnable(const uint8_t event);
enum HsmRet dispatchForPassive(const uint8_t event);
enum HsmRet dispatchForStandby(const uint8_t event);
enum HsmRet dispatchForActive(const uint8_t event);
enum HsmRet dispatchForLowBeamRunning(const uint8_t event);
enum HsmRet dispatchForHighBeamRunning(const uint8_t event);

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