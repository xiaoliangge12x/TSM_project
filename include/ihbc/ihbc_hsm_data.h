#ifndef IHBC_HSM_DATA_H_
#define IHBC_HSM_DATA_H_

#include "base/hsm_data.h"

extern Hsm g_hsm;
extern dispatch g_dispatchArray[MAX_HSM_STATES];

enum IhbcState {
    DISABLE = ROOT + 1,
    FAILURE,
    ENABLE,
    PASSIVE,
    STANDBY,
    ACTIVE,
};

enum IhbcEvent {
    IHBC_BRIGHTNESS = IHBC_USER_START + 1,
    IHBC_WORK,
    IHBC_DISABLE,
};
#endif