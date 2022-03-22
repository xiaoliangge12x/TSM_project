#include "ihbc/ihbc_hsm.h"
#include "ihbc/ihbc_params.h"

void InitUser()
{
    // 刷新状态分发函数指针数组
    g_dispatchArray[(uint8_t)DISABLE] = dispatchForDisable;
    g_dispatchArray[(uint8_t)FAILURE] = dispatchForFailure;
    g_dispatchArray[(uint8_t)ENABLE] = dispatchForEnable;
    g_dispatchArray[(uint8_t)PASSIVE] = dispatchForPassive;
    g_dispatchArray[(uint8_t)STANDBY] = dispatchForStandby;
    g_dispatchArray[(uint8_t)ACTIVE] = dispatchForActive;
    g_dispatchArray[(uint8_t)HIGHBEAM] = dispatchForHighBeamRunning;
    g_dispatchArray[(uint8_t)LOWBEAM] = dispatchForLowBeamRunning;

    g_hsm.currentSt = HIGHBEAM;
    // DoInitialTransition(true);
}

void RunUser()
{
    // 更新参数
    UpdateParams();

    uint8_t ihbc_event = IHBC_DISABLE;
    
    if (ihbc_params.system_initialize_flag) {
        ihbc_event = IHBC_BRIGHTNESS;
    }

    ihbc_event = IHBC_FAILURE;
    // 进行状态分发
    Dispatch(ihbc_event);
    
    // do work
    ihbc_event = IHBC_WORK;
    // 进行状态分发
    Dispatch(ihbc_event);
}

