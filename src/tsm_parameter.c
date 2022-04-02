#include "tsm_parameter.h"

void StartTiming(sint64* cur_time, uint8* flag)
{
    hb_TimeSync_GetTime(cur_time);
    *flag = 1;
}

void StopTiming(uint8* flag)
{
    *flag = 0;
}

float32 GetTimeGapInSec(sint64 start_time, uint8* flag)
{
    static const float32 S_TEN_NS_CONV_RATE_FLOAT_TYPE = 100000000.0;

    if (*flag == 1) {
        sint64 cur_time = 0;
        hb_TimeSync_GetTime(&cur_time);
        return (float32)(cur_time - start_time) / S_TEN_NS_CONV_RATE_FLOAT_TYPE;
    } else {
        return 0.0;
    }
}