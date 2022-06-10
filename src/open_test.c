#include "open_test.h"

// -------------------- driving table initilize ---------------------
static const Signal g_signal_table[] =
{
    {TYPE_UINT8,   "BCS_VehicleStandStillSt",       GetBCS_VehicleStandStillSt},
    {TYPE_UINT8,   "BCM_LeftTurnLampSt",            GetBCM_LeftTurnLampSt},
    {TYPE_UINT8,   "BCM_RightTurnLampSt",           GetBCM_RightTurnLampSt},
    {TYPE_UINT8,   "BCM_HazardLampSt",              GetBCM_HazardLampSt},
    {TYPE_UINT8,   "BCM_TurnLightSwitchSt",         GetBCM_TurnLightSwitchSt},
    {TYPE_UINT8,   "EBB_BrkPedalAppliedSt",         GetEBB_BrkPedalAppliedSt},
    {TYPE_UINT8,   "EBB_BrkPedalApplied",           GetEBB_BrkPedalApplied},
    {TYPE_UINT8,   "HOD_TouchZone1",                GetHOD_TouchZone1},
    {TYPE_UINT8,   "HOD_TouchZone2",                GetHOD_TouchZone2},
    {TYPE_UINT8,   "HOD_TouchZone3",                GetHOD_TouchZone3},
    {TYPE_UINT8,   "HOD_CalibrationSt",             GetHOD_CalibrationSt},
    {TYPE_UINT8,   "HOD_FaultStatus",               GetHOD_FaultStatus},
    {TYPE_UINT8,   "EPS_StrngWhlTorqVD",            GetEPS_StrngWhlTorqVD},
    {TYPE_FLOAT32, "EPS_StrngWhlTorq",              GetEPS_StrngWhlTorq},
    {TYPE_UINT8,   "EMS_GasPedalActPstforMRRVD",    GetEMS_GasPedalActPstforMRRVD},
    {TYPE_FLOAT32, "EMS_GasPedalActPstforMRR",      GetEMS_GasPedalActPstforMRR},
// -------------------- seperate line -----------------------------------------------    
    {TYPE_UINT8,   "lng_override_st",               GetLngOverrideSt},
    {TYPE_UINT8,   "driver_hand_torque_st",         GetDriverHandTorqueSt},
    {TYPE_UINT8,   "mrm_system_fault_level",        GetMRMSystemFaultLevel},
    {TYPE_UINT8,   "planningLite_control_state",    GetPlanningLite_control_state},
};
// -------------------- function definition -----------------------------------------
// 从yaml中读取数据并设置
void ReadFromYamlAndSetData(const char* filename, SimulinkData* simulink_data)
{
    static int   keyFlag      = 0;
    static char* key_value   = " ";
    static char* value_value = " ";

    FILE*  f = fopen(filename, "r");
    static yaml_parser_t parser;  
    static yaml_token_t  token;

    if (!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize parser.\n", stderr);
    }
    if (f == NULL) {
        fputs("Failed to open file!\n", stderr);
    }
    yaml_parser_set_input_file(&parser, f);   // 该工具是将文本句柄和解析器绑定的操作

    do {
        yaml_parser_scan(&parser, &token);
        switch(token.type){
            case YAML_KEY_TOKEN: {
                keyFlag = 1;
                break;
            }
            case YAML_VALUE_TOKEN: {
                keyFlag = 0;
                break;
            }
            
            case YAML_SCALAR_TOKEN: {
                if (keyFlag == 1) {
                    key_value = strdup(token.data.scalar.value);
                } else {
                    value_value = strdup(token.data.scalar.value);
                    keyFlag = 0;
                    SimulinkDataSet(key_value, value_value, simulink_data);
                }
            }
            /* Others */
            default: {
                int keyFlag = 0;
            }
        }
        if (token.type != YAML_STREAM_END_TOKEN)
        yaml_token_delete(&token);
    } while (token.type != YAML_STREAM_END_TOKEN);   // 令牌类型不到末尾
    
    yaml_token_delete(&token);
 
    yaml_parser_delete(&parser);
    fclose(f);
}

// 设置原始数据
void SimulinkDataSet(const char* key_str, const char* value_str, SimulinkData* simulink_data) 
{
    for (uint8 i = 0; i < (sizeof(g_signal_table) / sizeof(Signal)); ++i) {
        if (!strcmp(key_str, g_signal_table[i].signal_name)) {
            switch (g_signal_table[i].signal_type) {
                case TYPE_UINT8: {
                    // if (!strcmp(key_str, "BCM_HazardLampSt")) {
                    //     // 模拟车上环境，暂时
                    //     if (atoi(value_str) == 1) {
                    //         if (simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt) {
                    //             simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt = 0;
                    //             simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt = 0;
                    //         } else {
                    //             simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt = 1;
                    //             simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt = 1;
                    //         }
                    //     } else {
                    //         simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt = 0;
                    //         simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt = 0;
                    //     }
                    // } else {
                        *((uint8*)g_signal_table[i].signal_ptr_get(simulink_data)) = 
                            (uint8)atoi(value_str);
                    // }
                    break;
                }

                case TYPE_UINT16: {
                    *((uint16*)g_signal_table[i].signal_ptr_get(simulink_data)) =
                        (uint16)atoi(value_str);
                    break;
                }

                case TYPE_FLOAT32: {
                    *((float32*)g_signal_table[i].signal_ptr_get(simulink_data)) = 
                        (float32)atof(value_str);
                    break;
                }

                default: {
                    // do nothing;
                }
            }
            return;
        }
    }
}