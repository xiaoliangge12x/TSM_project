#include "tsm_parameter.h"

float32 K_BrakeTOR_TimeThreshold                     = 3.0;
float32 K_Tor3RampUpToMrm4Time                       = 10.0;   // TOR3级升级到MRM4级的持续时间

float32 K_TakeOverAvailHandTorqThreshold_LessTwoZone = 0;      // 少于2区的具备接管能力的手扶阈值
float32 K_TakeOverAvailHandTorqThreshold_TwoZone     = 0;      // 2区的具备接管能力的手力矩阈值
float32 K_TakeOverAvailHandTorqThreshold_ThreeZone   = 0;      // 3区的具备接管能力的手力矩阈值

#ifdef CONSUME_TIME

void 
tsm_start_timing(sint64* cur_time, uint8* flag) {
    hb_TimeSync_GetTime(cur_time);
    *flag = 1;
}

void 
tsm_stop_timing(uint8* flag) {
    *flag = 0;
}

float32
tsm_get_delta_time(const sint64 start_time, const uint8 flag) {
    static const float32 TEN_NS_PER_S = 100000000.0;

    float32 delta_time = 0.0;
    if (flag == 1) {
        sint64 cur_time = 0;
        hb_TimeSync_GetTime(&cur_time);
        delta_time = (float32)(cur_time - start_time) / TEN_NS_PER_S;
    }
    return delta_time;
}

#endif

boolean 
tsm_is_bit_set(const uint32 bitfields, const uint8 bitno) {
    uint32 ans = bitfields & ((uint32)1U << bitno);
    return (ans != 0);
}

void 
tsm_set_bit_in_bitfields(uint32* bitfields, const uint8 bitno) {
    *bitfields |= (uint32)1U << bitno;
}

void
tsm_reset_bit_in_bitfields(uint32* bitfields, const uint8 bitno) {
    *bitfields &= ~((uint32)1U << bitno);
}

boolean
tsm_is_mrm_active(const enum tsm_mcu_mrm_func_st mrm_st) {
    return ((mrm_st == MCU_TOR_LNG_LAT_CTRL) ||
            (mrm_st == MCU_TOR_LAT_CTRL) ||
            (mrm_st == MCU_TOR_STAND) ||
            (mrm_st == MCU_MRM_LNG_LAT_CTRL) ||
            (mrm_st == MCU_MRM_LAT_CTRL) ||
            (mrm_st == MCU_MRM_MRC));
}

boolean
tsm_is_nda_active(const enum nda_func_st nda_st) {
    return ((nda_st == NDA_ACTIVE_EPB_PHASE_IN) || 
            (nda_st == NDA_ACTIVE_HANDS_FREE_NORMAL) ||
            (nda_st == NDA_ACTIVE_HANDS_FREE_STAND_ACTIVE) || 
            (nda_st == NDA_ACTIVE_HANDS_FREE_STAND_WAIT) ||
            (nda_st == NDA_ACTIVE_HANDS_ON_NORMAL) || 
            (nda_st == NDA_ACTIVE_HANDS_ON_STAND_ACTIVE) ||
            (nda_st == NDA_ACTIVE_HANDS_ON_STAND_WAIT) || 
            (nda_st == NDA_ACTIVE_EPB_PHASE_IN) ||
            (nda_st == NDA_LNG_OVERRIDE) || 
            (nda_st == NDA_LNG_LAT_OVERRIDE) ||
            (nda_st == NDA_LAT_OVERRIDE) || 
            (nda_st == NDA_TOR_LAT_CTRL) ||
            (nda_st == NDA_TOR_LNG_LAT_CTRL) || 
            (nda_st == NDA_TOR_STAND) ||
            (nda_st == NDA_MRM_ACTIVE_PO_LAT_CTRL) || 
            (nda_st == NDA_MRM_ACTIVE_PO_LNG_LAT_CTRL) ||
            (nda_st == NDA_MRM_ACTIVE_CP_LAT_CTRL) || 
            (nda_st == NDA_MRM_ACTIVE_CP_LNG_LAT_CTRL) ||
            (nda_st == NDA_MRM_ACTIVE_ES_LAT_CTRL) || 
            (nda_st == NDA_MRM_ACTIVE_ES_LNG_LAT_CTRL) ||
            (nda_st == NDA_MRC));
}