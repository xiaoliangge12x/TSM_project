#ifndef IHBC_HSM_DATA_H_
#define IHBC_HSM_DATA_H_

#include "base/hsm_data.h"

extern Hsm g_hsm;
extern dispatch g_dispatchArray[MAX_HSM_STATES];

typedef enum {
    IHBC_DISABLE = ROOT + 1,
    IHBC_STANBY,
    IHBC_ACTIVE,
    IHBC_FAILURE,
    STANDBY,
    ACTIVE,
    LOWBEAM,
    HIGHBEAM,
} IhbcState;

typedef enum {
    IHBC_EVENT_DISABLE = IHBC_USER_START + 1,
    IHBC_EVENT_ERROR,
    IHBC_EVENT_NOOBJECTDATA,      // 无障碍物数据
    IHBC_EVENT_GLARE,             // 恶劣天气
    IHBC_EVENT_ROADLIGHTING,      // 路灯
    IHBC_EVENT_OVERTAKING,        // 超车
    IHBC_EVENT_DRIVEPAST,         // 被超车
    IHBC_EVENT_BRIGHTNESS,        // 环境亮度
    IHBC_EVENT_ONCOMING,          // 对向来车
    IHBC_EVENT_PRECEDING,         // 跟车
    IHBC_EVENT_BLINDNESS,         // 致盲
    IHBC_EVENT_VELTOOLOW,         // 车速过低
} IhbcEvent;

// 给到
typedef enum {
    HMA_OFF = 0,
    HMA_STANBY,
    HMA_ACTIVE,
    HMA_FAILURE,
    HMA_CAMERA_BLOCKED,
} HMASt;
#endif