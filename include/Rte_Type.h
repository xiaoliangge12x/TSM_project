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
    uint8_t EPB_Request;           
    uint8_t Hazard_Light_Request;  
    uint8_t Ecall_Request;        
    uint8_t Door_Unlock_Request;   
} Dt_RECORD_VehicleControlReq;

typedef struct 
{
    uint8_t NDA_Function_State;   
    uint8_t ICA_Function_State;   
    uint8_t ACC_Function_State;   
} Dt_RECORD_Automaton_State;

typedef struct
{
    uint8_t CtrlArb_EPS_Control_Echo;  
    uint8_t CtrlArb_AEB_Control_Echo;
} Dt_RECORD_Control_Arbitrator_Results;

typedef struct
{
  uint8_t NDA_Lane_Change_Type;
  uint8_t NDA_Lane_Change_Direction;
  uint8_t NDA_Lane_Change_State;
  uint8_t NDA_Split_State;
  uint8_t NDA_behavior;
  uint8_t NDA_Area_Type;
  uint8_t NDA_road_type;
  uint8_t NDA_lane_type;
  uint8_t ADC_path_merge_type;
} Dt_RECORD_ScenarioType;

typedef struct
{
    uint8_t NDA_Enable_State;           
    uint8_t Planning_DriveOff_Req;      
    uint8_t SD_Map_HD_Map_Match_St;  
    uint8_t EEA_Status; 
    uint8_t User_Set_Navi_Status;
    uint8_t Global_Location_Accuracy;
    uint8_t Relative_Location_Accuracy;
    uint8_t HandsOn_HandsFree_Flag;
    uint8_t NDA_Planning_Request; 
    uint8_t NDA_System_Fault_Level;
    int     HD_Map_Warning_Dist;
    uint8_t is_sys_open_flag;     // NDA系统是否请求双闪
} Dt_RECORD_Monitor_Signal_Source;

typedef struct
{
    Dt_RECORD_ScenarioType          scenario_type;
    Dt_RECORD_Automaton_State       automaton_state;     
    Dt_RECORD_VehicleControlReq     soc_hmi_request;      
    Dt_RECORD_Monitor_Signal_Source monitor_sig_src;
    uint8_t frc_fault_from_soc;
    uint8_t request_mrm_from_soc;
} Dt_RECORD_Soc_Info;

typedef struct
{
    Dt_RECORD_TimeStamp time_stamp; 
    uint8_t             MCU_MRM_status;
    uint8_t             Request_Mrm_From_MCU;
} Dt_RECORD_MCU2IFC;

typedef struct
{
    uint8_t  VCU_EMS_AccPedalActPstVD;
    float    VCU_EMS_AccPedalActPst;
    uint8_t  BCS_MasterCylinderPrVD;
    float    BCS_MasterCylinderPr; 
    uint8_t  EPS_StrngWhlTorqVD;
    float    EPS_StrngWhlTorq; 
    uint8_t  EPS_SteeringAngleVD; 
    float    EPS_SteeringAngle;
    uint8_t  EPS_SteeringAngleSpdVD;
    float    EPS_SteeringAngleSpd;  
    uint8_t  VCU_EMS_BrkPedalStVD;
    uint8_t  VCU_EMS_BrkPedalSt;  
    uint8_t  EBB_BrkPedPstVD;
    float    EBB_BrkPedPst; 
    uint8_t  EBB_BrkPedalApplied;   
    uint8_t  EBB_BrkPedalAppliedSt;
    uint8_t  EPB_SwitchSt;
    uint8_t  EPB_SwitchStVD;
    uint8_t  EPB_SysSt;
    uint8_t  EPS_HandsOffDetStVD;
    uint8_t  EPS_HandsOffDetSt; 
    uint8_t  HOD_TouchZone1;
    uint8_t  HOD_TouchZone2;
    uint8_t  HOD_TouchZone3; 
    uint8_t  HOD_CalibrationSt; 
    uint8_t  HOD_FaultStatus;  
    uint8_t  VCU_AccDriverOrvd;
    uint8_t  VCU_CrntGearLvlVD;  
    uint8_t  VCU_CrntGearLvl;
    uint8_t  BCM_FrontWiperSt;
    uint8_t  BCM_DriverDoorAjarSt;
    uint8_t  BCM_PsngrDoorAjarSt;
    uint8_t  BCM_RLDoorAjarSt;
    uint8_t  BCM_RRDoorAjarSt; 
    uint8_t  BCM_TrunkAjarSt;
    uint8_t  SRS_DriverSeatBeltSt;
    uint8_t  BCS_YawRateSt;     
    float    BCS_YawRate;    
    uint8_t  BCS_ActVehLaltrlAccelVD; 
    float    BCS_ActVehLaltrlAccel; 
    uint8_t  BCS_ActVehLongAccelVD;
    float    BCS_ActVehLongAccel;
    uint8_t  VCU_VehRdySt; 
    uint8_t  BCM_KeySt;
    uint8_t  BCM_HazardLampSt;
    uint8_t  SRS_CrashOutputSt; 
    uint8_t  DMS_DrvrDetSts; 
    uint8_t  DMS_InterestsRegion; 
    uint16_t DMS_L3DriverFatigueSt;
    uint8_t  DMS_L3DriverDistractionSt;
    uint8_t  BCS_VehicleStandStillSt;
    uint8_t  EMS_GasPedalActPstforMRRVD; 
    float    EMS_GasPedalActPstforMRR;
    uint8_t  BCM_LeftTurnLampSt; 
    uint8_t  BCM_RightTurnLampSt;
    uint8_t  BCS_VehSpdVD;
    float    BCS_VehSpd;
    uint16_t RLS_RainfallLevel; 
    float    cal_light_lux_up;
} Dt_RECORD_VehicleSignal2TSM;

typedef struct tagTSM2Soc
{
    uint32_t                              AS_Status;                   
    Dt_RECORD_Control_Arbitrator_Results  Control_Arbitrator_Results;
    uint8_t                               Lane_Change_Allow_Flag; 
    uint8_t                               AS_HandsoffTimerRequest;
    Dt_RECORD_TimeStamp                   tsm_timestamp; 
    uint8_t                               Parking_EPS_handshake_state;
    uint8_t                               MCU_MRM_Active_St; 
    uint8_t                               Request_NDA_Exit;
} Dt_RECORD_TSM2Soc;

//****************************************************** input *******************************************//
typedef struct
{
    Dt_RECORD_Control_Arbitrator_Results Control_Arbitrator_Results;  
    uint32_t                             AS_Status;                   
    Dt_RECORD_TimeStamp                  timestamp;
    uint8_t                              Parking_EPS_handshake_state;
} Dt_RECORD_CtrlArb2TSM;

typedef struct
{
    uint8_t             Lane_Change_Allow_Flag; 
    uint16_t            Check_Info;            
    Dt_RECORD_TimeStamp Decision_Arbitrator_TimeStamp;
} Dt_RECORD_DecisionArbitrator2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp         TimeStamp;
    Dt_RECORD_VehicleSignal2TSM Vehicle_Signal_To_Tsm;
    Dt_RECORD_Soc_Info          Soc_Info;             
} Dt_RECORD_CANGATE2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp Diag_TimeStamp;
    uint8_t             com_fault_with_soc; 
    uint8_t             is_support_lane_stop;
    uint8_t             is_support_emergency_stop;
} Dt_RECORD_Diag2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp DeTimeStamp;
    uint8_t             planningLite_control_state;
} Dt_RECORD_PLANLITE2TSM;

//****************************************************** output *******************************************//
typedef struct 
{
    Dt_RECORD_TimeStamp DeTimeStamp; 
    Dt_RECORD_TimeStamp TimeStamp;  
    uint8_t             NDA_Lane_Change_Type;
    uint8_t             NDA_Lane_Change_Direction;
    uint8_t             NDA_Lane_Change_State;
    uint8_t             MRM_Status;  // 0 - mrm  1 - tor   2 - exit
} Dt_RECORD_TSM2PLANLITE;

typedef struct
{
    Dt_RECORD_TimeStamp       DeTimeStamp; 
    Dt_RECORD_TimeStamp       TimeStamp_PlannLite; 
    Dt_RECORD_TimeStamp       TimeStamp_CanGate;
    uint8_t                   holo_planning_control_status;   // 0 - tor 1 - mrm  2 - exit
    Dt_RECORD_Automaton_State Automaton_State; 
    uint8_t                   long_override_flag;
    uint8_t                   lat_override_flag;
    uint8_t                   NDA_LatState; 
    uint8_t                   NDA_ILC_State;
    uint8_t                   NDA_LongState;
	uint8_t                   control_arb_request;    // 0 - soc  1 - mcu
} Dt_RECORD_TSM2CtrlArb;

typedef struct
{
    Dt_RECORD_TimeStamp    TimeStamp;
    Dt_RECORD_ScenarioType DeScenarioType;
} Dt_RECORD_TSM2DecisionArbitrator;

typedef struct
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp; 
    uint8_t             Tsm_Status;
} Dt_RECORD_TSM2Diag;

typedef struct
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp;
    uint8_t             Fault_Info;
    uint8_t             Tor_Request;
} Dt_RECORD_TSM2HMI;

typedef struct
{
    Dt_RECORD_TimeStamp         Tsm_TimeStamp; 
    Dt_RECORD_TimeStamp         Control_Arbitrator_TimeStamp;
    Dt_RECORD_TimeStamp         Decision_Arbitrator_TimeStamp; 
    Dt_RECORD_VehicleControlReq post_hmi_request;
    Dt_RECORD_TSM2Soc           Tsm_To_Soc;            
    Dt_RECORD_MCU2IFC           Mcu_To_Ifc;
} Dt_RECORD_TSM2CANGATE;
#endif