#include "open_test.h"

#define MAX_HEADER_NAME          50U
#define MAX_KEY_NAME             50U
#define MAX_VALUE_NAME           20U
#define MAX_TYPE_NAME            20U
#define MAX_SIGNAL_NAME          50U

struct tsm_test_signal {
    enum tsm_test_sig_type sig_type;  
    char signal_name[MAX_SIGNAL_NAME];
    
    void* (*get_signal_ptr) 
        (struct tsm_simulink_input* simu_in);
};

static void*
tsm_getptr_veh_standstill_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_VehicleStandStillSt);
}

static void*
tsm_getptr_hazard_lamp_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCM_HazardLampSt);
}

static void*
tsm_getptr_brkpedal_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EBB_BrkPedalAppliedSt);
}

static void*
tsm_getptr_brkpedal(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EBB_BrkPedalApplied);
}

static void*
tsm_getptr_touchzone1(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        HOD_TouchZone1);
}

static void*
tsm_getptr_touchzone2(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        HOD_TouchZone2);
}

static void*
tsm_getptr_touchzone3(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        HOD_TouchZone3);
}

static void*
tsm_getptr_hod_cali_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        HOD_CalibrationSt);
}

static void*
tsm_getptr_hod_fault_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        HOD_FaultStatus);
}

static void*
tsm_getptr_strwhl_torq(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EPS_StrngWhlTorq);
}

static void*
tsm_getptr_strwhl_torq_vd(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EPS_StrngWhlTorqVD);
}

static void*
tsm_getptr_gaspedal_actpos(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EMS_GasPedalActPstforMRR);
}
static void*
tsm_getptr_veh_spd(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_VehSpd);
}

static void*
tsm_getptr_yaw_rate(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_YawRate);
}

static void*
tsm_getptr_lat_accel(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_ActVehLaltrlAccel);
}

static void*
tsm_getptr_lng_accel(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_ActVehLongAccel);
}

static void*
tsm_getptr_steering_ang_spd(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EPS_SteeringAngleSpd);
}

static void*
tsm_getptr_drvr_gear_vd(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        VCU_CrntGearLvlVD);
}

static void*
tsm_getptr_drvr_gear(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        VCU_CrntGearLvl);
}

static void*
tsm_getptr_veh_rdy_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        VCU_VehRdySt);
}

static void*
tsm_getptr_gaspedal_actpos_vd(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EMS_GasPedalActPstforMRRVD);
}

static void*
tsm_getptr_epb_switch_st_vd(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EPB_SwitchStVD);
}

static void*
tsm_getptr_epb_switch_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EPB_SwitchSt);
}

static void*
tsm_getptr_epb_sys_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        EPB_SysSt);
}

static void*
tsm_getptr_key_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCM_KeySt);
}

static void*
tsm_getptr_hdc_ctrl_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_HDCCtrlSt);
}

static void*
tsm_getptr_vdc_active_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_VDCActiveS);
}

static void*
tsm_getptr_abs_active_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_ABSActiveSt);
}

static void*
tsm_getptr_tcs_active_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_TCSActiveSt);
}

static void*
tsm_getptr_ebd_active_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_EBDActiveSt);
}

static void*
tsm_getptr_cdd_avl(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.\
        BCS_CDDAvailable);
}

static void*
tsm_getptr_mcu_mrm_fault_level(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_diag_tsm.mcu_mrm_system_fault_level);
}

static void*
tsm_getptr_support_lane_stop(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_diag_tsm.is_support_lane_stop);
}

static void*
tsm_getptr_support_es(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_diag_tsm.is_support_emergency_stop);
}

static void*
tsm_getptr_pc_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_planlite_tsm.\
        planningLite_control_state);
}

static void*
tsm_getptr_nda_func_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Soc_Info.\
        automaton_state.NDA_Function_State);
}

static void*
tsm_getptr_ica_func_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Soc_Info.\
        automaton_state.ICA_Function_State);
}

static void*
tsm_getptr_acc_func_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Soc_Info.\
        automaton_state.ACC_Function_State);
}

static void*
tsm_getptr_veh_posture_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Soc_Info.\
        monitor_sig_src.Vehicle_Posture_St);
}

static void*
tsm_getptr_veh_position_st(struct tsm_simulink_input* simu_in) {
    return &(simu_in->rt_in_cangate_tsm.Soc_Info.\
        monitor_sig_src.Vehicle_Position_St);
}

static const struct tsm_test_signal ts_sig[] = {
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCS_VehicleStandStillSt",
        .get_signal_ptr = tsm_getptr_veh_standstill_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCM_HazardLampSt",
        .get_signal_ptr = tsm_getptr_hazard_lamp_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "EBB_BrkPedalAppliedSt",
        .get_signal_ptr = tsm_getptr_brkpedal_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "EBB_BrkPedalApplied",
        .get_signal_ptr = tsm_getptr_brkpedal,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "HOD_TouchZone1",
        .get_signal_ptr = tsm_getptr_touchzone1,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "HOD_TouchZone2",
        .get_signal_ptr = tsm_getptr_touchzone2,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "HOD_TouchZone3",
        .get_signal_ptr = tsm_getptr_touchzone3,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "HOD_CalibrationSt",
        .get_signal_ptr = tsm_getptr_hod_cali_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "HOD_FaultStatus",
        .get_signal_ptr = tsm_getptr_hod_fault_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "EPS_StrngWhlTorqVD",
        .get_signal_ptr = tsm_getptr_strwhl_torq_vd,
    },
    {
        .sig_type = TYPE_FLOAT32,
        .signal_name = "EPS_StrngWhlTorq",
        .get_signal_ptr = tsm_getptr_strwhl_torq,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "EMS_GasPedalActPstforMRRVD",
        .get_signal_ptr = tsm_getptr_gaspedal_actpos_vd,
    },
    {
        .sig_type = TYPE_FLOAT32,
        .signal_name = "EMS_GasPedalActPstforMRR",
        .get_signal_ptr = tsm_getptr_gaspedal_actpos,
    },
    {
        .sig_type = TYPE_FLOAT32,
        .signal_name = "BCS_VehSpd",
        .get_signal_ptr = tsm_getptr_veh_spd,
    },
    {
        .sig_type = TYPE_FLOAT32,
        .signal_name = "BCS_YawRate",
        .get_signal_ptr = tsm_getptr_yaw_rate,
    },
    {
        .sig_type = TYPE_FLOAT32,
        .signal_name = "BCS_ActVehLaltrlAccel",
        .get_signal_ptr = tsm_getptr_lat_accel,
    },
    {
        .sig_type = TYPE_FLOAT32,
        .signal_name = "BCS_ActVehLongAccel",
        .get_signal_ptr = tsm_getptr_lng_accel,
    },
    {
        .sig_type = TYPE_FLOAT32,
        .signal_name = "EPS_SteeringAngleSpd",
        .get_signal_ptr = tsm_getptr_steering_ang_spd,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "VCU_CrntGearLvlVD",
        .get_signal_ptr = tsm_getptr_drvr_gear_vd,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "VCU_CrntGearLvl",
        .get_signal_ptr = tsm_getptr_drvr_gear,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "VCU_VehRdySt",
        .get_signal_ptr = tsm_getptr_veh_rdy_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "EPB_SwitchStVD",
        .get_signal_ptr = tsm_getptr_epb_switch_st_vd,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "EPB_SwitchSt",
        .get_signal_ptr = tsm_getptr_epb_switch_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "EPB_SysSt",
        .get_signal_ptr = tsm_getptr_epb_sys_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCM_KeySt",
        .get_signal_ptr = tsm_getptr_key_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCS_HDCCtrlSt",
        .get_signal_ptr = tsm_getptr_hdc_ctrl_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCS_VDCActiveS",
        .get_signal_ptr = tsm_getptr_vdc_active_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCS_ABSActiveSt",
        .get_signal_ptr = tsm_getptr_abs_active_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCS_TCSActiveSt",
        .get_signal_ptr = tsm_getptr_tcs_active_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCS_EBDActiveSt",
        .get_signal_ptr = tsm_getptr_ebd_active_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "BCS_CDDAvailable",
        .get_signal_ptr = tsm_getptr_cdd_avl,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "mcu_mrm_fault_level",
        .get_signal_ptr = tsm_getptr_mcu_mrm_fault_level,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "is_support_lane_stop",
        .get_signal_ptr = tsm_getptr_support_lane_stop,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "is_support_emergency_stop",
        .get_signal_ptr = tsm_getptr_support_es,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "planningLite_control_state",
        .get_signal_ptr = tsm_getptr_pc_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "NDA_Function_State",
        .get_signal_ptr = tsm_getptr_nda_func_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "ICA_Function_State",
        .get_signal_ptr = tsm_getptr_ica_func_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "Vehicle_Posture_St",
        .get_signal_ptr = tsm_getptr_veh_posture_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "Vehicle_Position_St",
        .get_signal_ptr = tsm_getptr_veh_position_st,
    },
    {
        .sig_type = TYPE_UINT8,
        .signal_name = "ACC_Function_State",
        .get_signal_ptr = tsm_getptr_acc_func_st,
    },
};

static void
tsm_set_name(char* name, const uint8 name_len, const yaml_token_t* token) {
    memset(name, 0, name_len);
    strncpy(name, token->data.scalar.value, 
            strlen(token->data.scalar.value));
}

static void
tsm_set_simu_input(const char* key_str, const char* value_str,
                   struct tsm_simulink_input* simu_in)
{
    for (size_t i = 0; i < ARRAY_LEN(ts_sig); ++i) {
        int cmp_ret = strcmp(key_str, ts_sig[i].signal_name);
        if (!cmp_ret) {
            switch (ts_sig[i].sig_type) {
                case TYPE_UINT8: {
                    *((uint8*)ts_sig[i].get_signal_ptr(simu_in)) =
                        (uint8)atoi(value_str);
                    break;
                }

                case TYPE_UINT16: {
                    *((uint16*)ts_sig[i].get_signal_ptr(simu_in)) =
                        (uint16)atoi(value_str);
                    break;
                }

                case TYPE_FLOAT32: {
                    *((float32*)ts_sig[i].get_signal_ptr(simu_in)) =
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

void 
tsm_read_yaml_set_data(const char* filename, struct tsm_simulink_input* simu_in) {
    static int key_flag = 0;
    static char key_name[MAX_KEY_NAME];
    static char value_name[MAX_VALUE_NAME];

    FILE* f = fopen(filename, "r");
    static yaml_parser_t parser;
    static yaml_token_t token;

    if (!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize parser.\n", stderr);
    }
    if (f == NULL) {
        fputs("Failed to open file!\n", stderr);
    }
    yaml_parser_set_input_file(&parser, f);

    do {
        yaml_parser_scan(&parser, &token);
        switch(token.type) {
            case YAML_KEY_TOKEN: {
                key_flag = 1;
                break;
            }

            case YAML_VALUE_TOKEN: {
                key_flag = 0;
                break;
            }

            case YAML_SCALAR_TOKEN: {
                if (key_flag == 1) {
                    tsm_set_name(key_name, MAX_KEY_NAME, &token);
                } else {
                    tsm_set_name(value_name, MAX_VALUE_NAME, &token);
                    tsm_set_simu_input(key_name, value_name, simu_in);
                    key_flag = 0;
                }
            }

            default: {  
                key_flag = 0;
            }
        }

        if (token.type != YAML_STREAM_END_TOKEN) {
            yaml_token_delete(&token);
        }
    } while (token.type != YAML_STREAM_END_TOKEN);

    yaml_token_delete(&token);
    yaml_parser_delete(&parser);
    fclose(f);
}


