#ifndef IHBC_HSM_H_
#define IHBC_HSM_H_

#include "base/hsm_chart.h"
#include "ihbc_hsm_data.h"

void InitUser();

void RunUser();

// 状态分发函数
enum HsmRet dispatchForDisable(const uint8_t event);
enum HsmRet dispatchForFailure(const uint8_t event);
enum HsmRet dispatchForActive(const uint8_t event);
enum HsmRet dispatchForLowBeamRunning(const uint8_t event);
enum HsmRet dispatchForHighBeamRunning(const uint8_t event);

// 状态entry函数

// 状态exit函数

// 状态work函数
void workForDisable();
void workForFailure();
void workForActive();
void workForLowBeamRunning();
void workForHighBeamRuning();


#endif