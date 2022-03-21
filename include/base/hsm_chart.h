#ifndef HSM_CHART_H_
#define HSM_CHART_H_

#include "hsm_data.h"

void Dispatch(uint8_t event);

enum HsmRet ExitState(const uint8_t state);
enum HsmRet EntryState(const uint8_t state);
enum HsmRet GoSuperState(const uint8_t state); 

#endif