#include "tsm_parameter.h"

// ----------------------  global variable def(calibration) -------------------------------
uint16  K_BrakPedalAppliedThresholdTime_Cnt          = 10U;    // 制动判断的持续时间(200ms周期)
uint16  K_GasPedalAppliedThresholdTime_Cnt           = 10U;    // 油门踩下的持续时间
uint16  K_LngOverrideTakeOverTime_Cnt                = 500U;   // 纵向超越至判断接管的持续时间(10s)
uint16  K_BrakeTOR_TimeThreshold_Cnt                 = 150U;   // 刹车长时介入时间阈值， 3s
uint16  K_OverrideHandTorqCheckTime_Cnt              = 50U;    // 手力矩是否超越的持续时间， 暂定1s
uint16  K_Tor3RampUpToMrm4Time_Cnt                   = 500U;

uint16  K_MissQuitTime_Cnt                           = 5U;      // NDA功能无法退出的时间阈值
uint16  K_MissOverrideTime_Cnt                       = 5U;      // Active未响应进入Override St的时间阈值
uint16  K_StuckOverrideTime_Cnt                      = 5U;      // Override未响应进入Active St的时间间隔

float32 K_BrakPedalAppliedThresholdTime              = 0.2;
float32 K_GasPedalAppliedThresholdTime               = 0.2;
float32 K_LngOverrideTakeOverTime                    = 10.0;
float32 K_BrakeTOR_TimeThreshold                     = 3.0;
float32 K_OverrideHandTorqCheckTime                  = 1.0;
float32 K_Tor3RampUpToMrm4Time                       = 10.0;   // TOR3级升级到MRM4级的持续时间

float32 K_OverrideHandTorqThreshold_LessTwoZone      = 1.8;    // 少于2区的手力矩是否超越的手扶阈值
float32 K_OverrideHandTorqThreshold_TwoZone          = 1.5;    // 2区的是否超越的手力矩阈值
float32 K_OverrideHandTorqThreshold_ThreeZone        = 1.0;    // 3区的是否超越的手力矩阈值
float32 K_TakeOverAvailHandTorqThreshold_LessTwoZone = 0;      // 少于2区的具备接管能力的手扶阈值
float32 K_TakeOverAvailHandTorqThreshold_TwoZone     = 0;      // 2区的具备接管能力的手力矩阈值
float32 K_TakeOverAvailHandTorqThreshold_ThreeZone   = 0;      // 3区的具备接管能力的手力矩阈值
float32 K_GasPedalPosThresholdValue                  = 20.0;   // 油门开度阈值

#ifdef CONSUME_TIME

void StartTiming(sint64* cur_time, uint8* flag)
{
    hb_TimeSync_GetTime(cur_time);
    *flag = 1;
}

void StopTiming(uint8* flag)
{
    *flag = 0;
}

float32 GetTimeGapInSec(const sint64 start_time, const uint8 flag)
{
    static const float32 S_TEN_NS_CONV_RATE_FLOAT_TYPE = 100000000.0;

    if (flag == 1) {
        sint64 cur_time = 0;
        hb_TimeSync_GetTime(&cur_time);
        return (float32)(cur_time - start_time) / S_TEN_NS_CONV_RATE_FLOAT_TYPE;
    } else {
        return 0.0;
    }
}

#endif

boolean IsBitSet(const uint32 event_bitfields, const uint8 bit_no)
{
    if ((event_bitfields & ((uint32)1U << bit_no)) != 0) {
        return true;
    } else {
        return false;
    }
}

void SetSignalBitFields(uint32* event_bitfields, const uint8 bit_no)
{
    *event_bitfields |= (uint32)1U << bit_no;
}

void ResetSignalBitFields(uint32* event_bitfields, const uint8 bit_no)
{
    *event_bitfields &= ~((uint32)1U << bit_no);
}