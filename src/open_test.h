#ifndef OPEN_TEST_H_
#define OPEN_TEST_H_

#include <yaml.h>
#include <stdlib.h>

#include "Rte_Type.h"
#include "tsm_parameter.h"
// -------------------------  macro         ---------------------------------
#define MAX_HEADER_NAME          50U
#define MAX_KEY_NAME             50U
#define MAX_VALUE_NAME           20U
#define MAX_TYPE_NAME            20U
#define MAX_INTERMEDIATE_NAME    50U
#define SIGNAL_NAME_MAX_LEN      50U

#define ENABLE_SET_INTERMEDIATE
// -------------------------  typedef       ----------------------------------
typedef void* (*GetSignalPtr) (SimulinkData* simulink_data);

typedef enum {
    TYPE_UINT8 = 0,
    TYPE_UINT16,
    TYPE_FLOAT32,
} SignalType;

typedef struct {
    SignalType   signal_type;  
    char         signal_name[SIGNAL_NAME_MAX_LEN];
    GetSignalPtr signal_ptr_get;
} Signal;

typedef struct 
{
  char                  intermediate_sig_name[MAX_INTERMEDIATE_NAME];
  BitNoForInterMediaSig bitno_intermediate_sig;
} IntermediateSig;
// -------------------------  driving table -----------------------------------
// static const Signal g_signal_table[];  // 此处非声明，而是定义

// -------------------------  function declaration ----------------------------
void  ReadFromYamlAndSetData(const char* filename, SimulinkData* simulink_data);
void  SimulinkDataSet(const char* key_str, const char* value_str, const char* header_str, 
    const char* type_str,SimulinkData* simulink_data);
void SetOriginInputandInterMediateSig(const char* key_str, const char* value_str, 
    SimulinkData* simulink_data);
void SetInterMediateMsg(const char* key_str, const char* value_str, const char* type_str, 
    SimulinkData* simulink_data);
void* GetBCS_VehicleStandStillSt(SimulinkData* simulink_data) 
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCS_VehicleStandStillSt);
}
void* GetBCM_LeftTurnLampSt(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_LeftTurnLampSt);
}
void* GetBCM_RightTurnLampSt(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_RightTurnLampSt);
}
void* GetBCM_HazardLampSt(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.BCM_HazardLampSt);
}
void* GetEBB_BrkPedalAppliedSt(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EBB_BrkPedalAppliedSt);
}
void* GetEBB_BrkPedalApplied(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EBB_BrkPedalApplied);
}
void* GetHOD_TouchZone1(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_TouchZone1);
}
void* GetHOD_TouchZone2(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_TouchZone2);
}
void* GetHOD_TouchZone3(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_TouchZone3);
}
void* GetHOD_CalibrationSt(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_CalibrationSt);
}
void* GetHOD_FaultStatus(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.HOD_FaultStatus);
}
void* GetEPS_StrngWhlTorqVD(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EPS_StrngWhlTorqVD);
}
void* GetEPS_StrngWhlTorq(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EPS_StrngWhlTorq);
}
void* GetEMS_GasPedalActPstforMRRVD(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EMS_GasPedalActPstforMRRVD);
}
void* GetEMS_GasPedalActPstforMRR(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.EMS_GasPedalActPstforMRR);
}
void* GetFault_Level(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_diag_tsm.Fault_Level);
}
void* GetPlanningLite_control_state(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_planlite_tsm.planningLite_control_state);
}

void* GetMRMSystemFaultLevel(SimulinkData* simulink_data)
{
    (void)simulink_data;
    return &(g_inter_media_msg.mrm_system_fault_level);
}

void* GetNDA_Function_State(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Soc_Info.Automaton_State.NDA_Function_State);
}

void* GetICA_Function_State(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Soc_Info.Automaton_State.ICA_Function_State);
}

void* GetACC_Function_State(SimulinkData* simulink_data)
{
    return &(simulink_data->rt_in_cangate_tsm.Soc_Info.Automaton_State.ACC_Function_State);
}
#endif