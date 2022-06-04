#ifndef RTE_TYPE_H_
#define RTE_TYPE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    bool     Timestamp_valid; 
    uint32_t Timestamp_sec;
    uint32_t Timestamp_nsec;
} Dt_RECORD_TimeStamp;

typedef struct 
{
    uint8_t ADAS_HWA_TurnLampReq;  // 0 - no request, 1 - left request on, 2 - right request on, 3 - not used
    uint8_t ADAS_SafeStopLampWarn; // 0 - no warning , 1 - level 1, 2 - level 2, 3 - not used
    // uint8_t EPB_Request;              // 电子手刹请求 MRC
    // uint8_t Ecall_Request;            // 电话请求      MRC
} Dt_RECORD_VehicleStateReq;

typedef struct 
{
    uint8_t NDA_Function_State;   
    uint8_t ICA_Function_State;     
    uint8_t ACC_Function_State;   
} Dt_RECORD_Automaton_State;

typedef struct
{
    uint8_t             Tor_Warning_Level;      // TOR报警等级(包含MRM和MRC)， 来自SOC
    uint8_t             Hands_On_Level;   // 手扶报警等级， 来自SOC
    uint8_t             Focus_On_Level;   // 脱眼报警等级， 来自SOC
} Dt_RECORD_Hmi_Request;

typedef struct
{
    Dt_RECORD_Automaton_State       automaton_state;  
    Dt_RECORD_Hmi_Request           soc_hmi_req;
    uint8_t                         soc_veh_lamp_req;    // soc侧传过来的灯光开启请求， 0 - no req , 1 - hazard light req, 2 - left lamp, 3 - right lamp
    uint8_t                         soc_tor_req;             // add
    uint8_t                         soc_mrm_active_st;       // add
} Dt_RECORD_Soc_Info;

typedef struct
{
    uint8_t  EPS_StrngWhlTorqVD;       
    float    EPS_StrngWhlTorq; 
    uint8_t  EBB_BrkPedalApplied;   
    uint8_t  EBB_BrkPedalAppliedSt;
    uint8_t  EPB_SwitchSt;             
    uint8_t  EPB_SwitchStVD;           
    uint8_t  EPB_SysSt;                
    uint8_t  HOD_TouchZone1;
    uint8_t  HOD_TouchZone2;
    uint8_t  HOD_TouchZone3; 
    uint8_t  HOD_CalibrationSt; 
    uint8_t  HOD_FaultStatus;  
    uint8_t  VCU_AccDriverOrvd;                   
    uint8_t  BCM_HazardLampSt;        
    uint8_t  DMS_DrvrDetSts;           
    uint16_t DMS_L3DriverFatigueSt; 
    uint8_t  DMS_L3DriverDistractionSt;
    uint8_t  BCS_VehicleStandStillSt;  
    uint8_t  EMS_GasPedalActPstforMRRVD; 
    float    EMS_GasPedalActPstforMRR;  
    uint8_t  BCS_VehSpdVD;         
    float    BCS_VehSpd;
} Dt_RECORD_VehicleSignal2TSM;

typedef struct 
{
    uint8_t AS_lng_ctrl_st;    // 0 - false   1 - true
    uint8_t AS_lat_ctrl_st;    // 0 - false   1 - true 
} Dt_RECORD_AS_Info;
//****************************************************** input *******************************************//
typedef struct
{
    Dt_RECORD_TimeStamp                  timestamp;            
    Dt_RECORD_AS_Info                    as_info;
} Dt_RECORD_CtrlArb2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp         TimeStamp;
    Dt_RECORD_VehicleSignal2TSM Vehicle_Signal_To_Tsm;    
    Dt_RECORD_Soc_Info          Soc_Info;
    uint8_t                     EPB_Request;
    uint8_t                     Ecall_Request;
    uint8_t                     Hazard_Light_Request;
    uint8_t                     Brake_Light_Request;
    uint8_t                     Turning_Light_Request;                
    uint8_t                     mcu_tor_req;
    uint8_t                     mcu_mrm_active_st;     
} Dt_RECORD_CANGATE2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp Diag_TimeStamp;
    uint16_t            ifc_mrm_system_fault_level;  //  0 - 无安全停车故障   1 - 安全停车故障
    uint8_t             is_support_lane_stop;   // 
    uint8_t             is_support_emergency_stop; //
    uint8_t             is_support_transmit_ctrl;  // 
} Dt_RECORD_Diag2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp DeTimeStamp;
    uint8_t             planningLite_control_state;   // 归控能力 0 - TOR or mrm  1 - 紧急mrm
} Dt_RECORD_PLANLITE2TSM;

//****************************************************** output *******************************************//
typedef struct 
{
    Dt_RECORD_TimeStamp TimeStamp;  
    uint8_t             MRM_Status;  // 0 - tor  1 - mrm  2 - mrm_es   3 - invalid
} Dt_RECORD_TSM2PLANLITE;

typedef struct
{
    Dt_RECORD_TimeStamp       TimeStamp; 
    uint8_t                   MCU_MRM_St;
	uint8_t                   control_arb_request;    // 0 - soc  1 - mcu  2 - invalid
} Dt_RECORD_TSM2CtrlArb;

typedef struct
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp;           
    uint8_t             Tsm_Status;  
} Dt_RECORD_TSM2Diag;

typedef struct
{
    Dt_RECORD_TimeStamp         Tsm_TimeStamp; 
    uint8_t                     IFC_ICMWarnCardSt;
    uint8_t                     IFC_MRMAvailability;   // add, 是否具备安全停车能力
    uint8_t                     IFC_ADCFault_request;    // for debug use    
} Dt_RECORD_TSM2CANGATE;

#endif