#include "ihbc/ihbc_hsm.h"
#include "ihbc/ihbc_params.h"

void InitUser()
{
    g_dispatchArray[(uint8_t)DISABLE] = dispatchForDisable;
    g_dispatchArray[(uint8_t)FAILURE] = dispatchForFailure;
}

void RunUser()
{
    // 更新参数
    UpdateParams();

    uint8_t ihbc_event;
    if (ihbc_params.system_initialize_flag) {
        ihbc_event = IHBC_BRIGHTNESS;
    }

    // 进行状态分发
    Dispatch(ihbc_event);
    
    // do work
    ihbc_event = IHBC_WORK;
    // 进行状态分发
    Dispatch(ihbc_event);
}

// 状态分发函数
enum HsmRet dispatchForDisable(const uint8_t event)
{
    return HSM_TRAN;
}

enum HsmRet dispatchForFailure(const uint8_t event)
{
    return HSM_TRAN;
}
