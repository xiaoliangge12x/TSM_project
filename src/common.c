#include "common.h"
#include "tsm_worker.h"

struct timeval g_init_time;
// -------------------------- function def ---------------------------------
void hb_TimeSync_GetTime(int64_t* value)
{
    struct timeval timeval_time;
    gettimeofday(&timeval_time, NULL);
    *value = (int64_t)(timeval_time.tv_sec - g_init_time.tv_sec) * S_TEN_NS_CONV_RATE + 
        (int64_t)(timeval_time.tv_usec - g_init_time.tv_usec) * US_TEN_NS_CONV_RATE;
}