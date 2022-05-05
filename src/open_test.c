#include "open_test.h"

// -------------------- driving table initilize ---------------------
static const Signal g_signal_table[] =
{
    {TYPE_UINT8,   "BCS_VehicleStandStillSt",       GetBCS_VehicleStandStillSt},
    {TYPE_UINT8,   "BCM_LeftTurnLampSt",            GetBCM_LeftTurnLampSt},
    {TYPE_UINT8,   "BCM_RightTurnLampSt",           GetBCM_RightTurnLampSt},
    {TYPE_UINT8,   "BCM_HazardLampSt",              GetBCM_HazardLampSt},
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
    {TYPE_UINT8,   "Fault_Level",                   GetFault_Level},  
    {TYPE_UINT8,   "mrm_system_fault_level",        GetMRMSystemFaultLevel},
    {TYPE_UINT8,   "planningLite_control_state",    GetPlanningLite_control_state},
    {TYPE_UINT8,   "NDA_Function_State",            GetNDA_Function_State},
    {TYPE_UINT8,   "ICA_Function_State",            GetICA_Function_State},
    {TYPE_UINT8,   "ACC_Function_State",            GetACC_Function_State},
};

static const IntermediateSig g_intermediate_sig_table[] = 
{
    {"FAILURE_LIGHTING_FLAG", BITNO_FAILURE_LIGHTING_FLAG},
    {"NDA_TRANSIT_NORMAL_FLAG", BITNO_NDA_TRANSIT_NORMAL_FLAG},
    {"DRVR_HANDTORQUE_OVERRIDE_ST", BITNO_DRVR_HANDTORQUE_OVERRIDE_ST},
    {"LNG_OVERRIDE_ST", BITNO_LNG_OVERRIDE_ST},
    {"LONG_TIME_LNG_OVERRIDE", BITNO_LONG_TIME_LNG_OVERRIDE},
    {"HANDS_CAN_TAKEOVER", BITNO_HANDS_CAN_TAKEOVER},
    {"SET_BRAKE", BITNO_SET_BRAKE},
    {"PHASE_IN_AVAILABLE", BITNO_PHASE_IN_AVAILABLE},
    {"NDA_NEED_PHASE_IN", BITNO_NDA_NEED_PHASE_IN},
    {"NDA_PASSIVE_VD", BITNO_NDA_PASSIVE_VD},
    {"NDA_AVL_BEFORE_ACT", BITNO_NDA_AVL_BEFORE_ACT},
    {"NDA_AVL_AFTER_ACT", BITNO_NDA_AVL_AFTER_ACT},
    {"DRVR_ACC_PEDAL_APPLIED", BITNO_DRVR_ACC_PEDAL_APPLIED},
};
// -------------------- function definition -----------------------------------------
// 从yaml中读取数据并设置
void ReadFromYamlAndSetData(const char* filename, SimulinkData* simulink_data)
{
    static int   keyFlag                      = 0;
    static char  key_name[MAX_KEY_NAME]       = "";
    static char  value_name[MAX_VALUE_NAME]   = "";
    static char  header_name[MAX_HEADER_NAME] = "";
    static char  type_name[MAX_TYPE_NAME]     = "";

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
                    if (strlen(key_name) != 0) {
                        // 说明无value, 有value会刷新key_name为""
                        memset(header_name, 0, MAX_HEADER_NAME);
                        strncpy(header_name, key_name, strlen(key_name));
                    }
                    memset(key_name, 0, MAX_KEY_NAME);
                    strncpy(key_name, token.data.scalar.value,
                            strlen(token.data.scalar.value));
                } else {
                    keyFlag = 0;

                    if (!strncmp(key_name, "type", strlen("type"))) {
                        memset(type_name, 0, MAX_TYPE_NAME);
                        strncpy(type_name, token.data.scalar.value, strlen(token.data.scalar.value));
                    } else {
                        memset(value_name, 0, MAX_VALUE_NAME);
                        strncpy(value_name, token.data.scalar.value, strlen(token.data.scalar.value));
                        SimulinkDataSet(key_name, value_name, header_name, type_name, simulink_data);
                    }
                    memset(key_name, 0, MAX_KEY_NAME);
                }
            }
            /* Others */
            default: {  
                keyFlag = 0;
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
void SimulinkDataSet(const char* key_str, const char* value_str, const char* header_str, 
    const char* type_str, SimulinkData* simulink_data) 
{
    // int number = sizeof(g_intermediate_sig_table) / sizeof(IntermediateSig);
    // LOG(COLOR_GREEN, "header_str: %s, type_str: %s, number: %d", header_str, type_str, number);
    if (!strncmp(header_str, "Inter_Media_Msg", strlen("Inter_Media_Msg"))) {
#ifdef ENABLE_SET_INTERMEDIATE
        SetInterMediateMsg(key_str, value_str, type_str, simulink_data);
#endif
    } else {
        SetOriginInputandInterMediateSig(key_str, value_str, simulink_data);
    }
}

void SetOriginInputandInterMediateSig(const char* key_str, const char* value_str, 
    SimulinkData* simulink_data)
{
    for (uint8 i = 0; i < (sizeof(g_signal_table) / sizeof(Signal)); ++i) {
        if (!strcmp(key_str, g_signal_table[i].signal_name)) {
            switch (g_signal_table[i].signal_type) {
                case TYPE_UINT8: {
                    if (!strcmp(key_str, "BCM_HazardLampSt")) {
                        // 模拟车上环境，暂时
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
                    } else {
                        *((uint8*)g_signal_table[i].signal_ptr_get(simulink_data)) = 
                            (uint8)atoi(value_str);
                    }
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

void SetInterMediateMsg(const char* key_str, const char* value_str, const char* type_str, 
    SimulinkData* simulink_data)
{
    if (!strncmp(type_str, "nonbool", strlen("nonbool"))) {
        SetOriginInputandInterMediateSig(key_str, value_str, simulink_data);
    } else if (!strncmp(type_str, "bool", strlen("bool"))) {
        for (int i = 0; i < (int)(sizeof(g_intermediate_sig_table) / sizeof(IntermediateSig)); ++i) {
            if (!strcmp(key_str, g_intermediate_sig_table[i].intermediate_sig_name)) {
                (!strncmp(value_str, "1", 1)) ?
                    SetSignalBitFields(&g_inter_media_msg.intermediate_sig_bitfields, 
                        g_intermediate_sig_table[i].bitno_intermediate_sig) :
                    ResetSignalBitFields(&g_inter_media_msg.intermediate_sig_bitfields, 
                        g_intermediate_sig_table[i].bitno_intermediate_sig);
                break;
            }
        }
    } else {
        // do nothing;
    }
}