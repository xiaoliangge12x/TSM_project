#include "tool/open_test.h"

// 从yaml中读取数据并设置
void ReadFromYamlAndSetData(const char* filename, SimulinkData* simulink_data)
{
    // do 之前可优化
    static int keyFlag = 0;
    static char *key_value = " ";
    static char *value_value = " ";

    FILE* f = fopen(filename, "r");
    static yaml_parser_t parser;  
    static yaml_token_t token;
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
                    OriginDataSet(key_value, value_value, simulink_data);
                    InterMediaMsgDataSet(key_value, value_value, simulink_data);
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

void OriginDataSet(const char* key_str, const char* value_str, SimulinkData* simulink_data) 
{
    if (!strcmp(key_str, "BCS_VehicleStandStillSt")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt =
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "BCM_LeftTurnLampSt")) {
        // temp comment
        /* simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt =
            (uint8_t)atoi(value_str); */
    } else if (!strcmp(key_str, "BCM_RightTurnLampSt")) {
        // temp comment
        /* simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt =
            (uint8_t)atoi(value_str); */
    } else if (!strcmp(key_str, "BCM_HazardLampSt")) {
        /* simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_HazardLampSt =
            (uint8_t)atoi(value_str); */
        // 模拟车上环境
        if (atoi(value_str) == 1) {
            if (simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt) {
                simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt = 0;
                simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt = 0;
            } else {
                simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt = 1;
                simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt = 1;
            }
        } else {
            simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt = 0;
            simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt = 0;
        }
    } else if (!strcmp(key_str, "EBB_BrkPedalApplied")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EBB_BrkPedalApplied = 
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "HOD_TouchZone1")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_TouchZone1 = 
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "HOD_TouchZone2")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_TouchZone2 = 
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "HOD_TouchZone3")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_TouchZone3 = 
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "HOD_CalibrationSt")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_CalibrationSt = 
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "HOD_FaultStatus")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_FaultStatus = 
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "EPS_StrngWhlTorqVD")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EPS_StrngWhlTorqVD = 
            (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "EPS_StrngWhlTorq")) {
        simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EPS_StrngWhlTorq = 
            (float32)atof(value_str);
    } else {
        // do nothing;
    }
}

void InterMediaMsgDataSet(const char* key_str, const char* value_str, SimulinkData* simulink_data)
{
    
    // printf("key_str: %s, value_str: %d\n", key_str, (uint8_t)atoi(value_str));
    if (!strcmp(key_str, "automaton_transit_normal_flag")) {
        tsm.inter_media_msg.automaton_transit_normal_flag = (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "lng_override_st")) {
        tsm.inter_media_msg.lng_override_st = (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "driver_hand_torque_st")) {
        tsm.inter_media_msg.driver_hand_torque_st = (uint8_t)atoi(value_str);
    } else if (!strcmp(key_str, "mrm_system_fault_level")) {
        tsm.inter_media_msg.mrm_system_fault_level = (uint8_t)atoi(value_str);
    } else {
        // do nothing;
    }
}   