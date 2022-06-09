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
    uint8_t EPB_Request;           // MCU依据automaton st 发送，
    uint8_t Ecall_Request;         // MCU依据automaton st 发送
} Dt_RECORD_VehicleStateReq; 

typedef struct 
{
    uint8_t NDA_Function_State;   
    uint8_t ICA_Function_State;   
    uint8_t ACC_Function_State;   
} Dt_RECORD_Automaton_State;

typedef struct
{
    uint8_t NDA_Enable_State;           
    uint8_t Planning_DriveOff_Req;       // add, Planning起步请求
    uint8_t Target_Lost_St;              // add, 目标丢失标志位
    uint8_t SD_Map_HD_Map_Match_St;      // add, SD地图与高精地图的匹配状态
    uint8_t EEA_Status;                  // add, 电子围栏状态
    uint8_t User_Set_Navi_Status;        // add, 用户是否设置导航的状态
    uint8_t Global_Location_Accuracy;    // add,  全局定位精度
    uint8_t Relative_Location_Accuracy;  // add, 相对定位精度
    uint8_t NDA_System_Fault_Level;      // add, nda系统故障等级
    int     HD_Map_Warning_Dist;         // add, HD map 报警距离
    uint8_t is_sys_open_flag;            // NDA系统是否请求双闪
    uint8_t Vehicle_Posture_St;          // add, 车辆姿态状态
    uint8_t Vehicle_Position_St;         // add, 车辆位置状态
} Dt_RECORD_Monitor_Signal_Source;

typedef struct
{
    Dt_RECORD_Automaton_State       automaton_state;
    uint8_t                         soc_warn_card_st;
    Dt_RECORD_Monitor_Signal_Source monitor_sig_src;   // add
    uint8_t                         soc_veh_lamp_req;    // soc侧传过来的灯光开启请求， 0 - no req , 1 - hazard light req, 2 - left lamp, 3 - right lamp
    uint8_t                         frc_fault_from_soc;     // add, SOC的FRC请求MCU接管故障等级
    uint8_t                         request_mrm_from_soc;   // add, soc_请求接管
} Dt_RECORD_Soc_Info;

typedef struct
{
    Dt_RECORD_TimeStamp tsm_timestamp; 
    uint8_t             mcu_tor_req;       // add
    uint8_t             mcu_mrm_active_st; // add
    uint8_t             mcu_adc_fault_st;   // 同步最新dbc
} Dt_RECORD_MCU2IFC;

typedef struct
{
    uint8_t  EPS_StrngWhlTorqVD;          
    float    EPS_StrngWhlTorq; 
    uint8_t  EPS_SteeringAngleSpdVD;      
    float    EPS_SteeringAngleSpd;  
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
    uint8_t  VCU_CrntGearLvlVD;  
    uint8_t  VCU_CrntGearLvl;
    uint8_t  BCM_FrontWiperSt;
    uint8_t  BCM_DriverDoorAjarSt;
    uint8_t  BCM_PsngrDoorAjarSt;
    uint8_t  BCM_RLDoorAjarSt;
    uint8_t  BCM_RRDoorAjarSt; 
    uint8_t  BCM_TrunkAjarSt;
    uint8_t  BCM_BonnetAjarSt;
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
    uint8_t  DMS_DrvrDetSts;            
    uint16_t DMS_L3DriverFatigueSt;
    uint8_t  DMS_L3DriverDistractionSt;
    uint8_t  BCS_VehicleStandStillSt;   
    uint8_t  EMS_GasPedalActPstforMRRVD; 
    float    EMS_GasPedalActPstforMRR;
    uint8_t  BCS_VehSpdVD;              
    float    BCS_VehSpd;
    uint16_t RLS_RainfallLevel;         
    float    RLS_AMBBrightness;          // add, 光照亮度，确认  change
    uint8_t  BCS_HDCCtrlSt;             // add, 底盘功能安全相关
    uint8_t  BCS_VDCActiveS;            // add, 底盘功能安全相关
    uint8_t  BCS_ABSActiveSt;           // add, 底盘功能安全相关
    uint8_t  BCS_TCSActiveSt;           // add, 底盘功能安全相关
    uint8_t  BCS_EBDActiveSt;           // add, 底盘功能安全相关
    uint8_t  BCS_CDDAvailable;          // add, 底盘功能安全相关
} Dt_RECORD_VehicleSignal2TSM;

typedef struct tagTSM2Soc
{
    Dt_RECORD_TimeStamp                   tsm_timestamp;    
    uint8_t                               MCU_MRM_Active_St;     // add , MCU的MRM激活状态
    uint8_t                               Request_NDA_Exit;     // add, 使用场景： MCU主动监控NDA状态机跳转错误，但此时又不需要MCU激活功能的场景
} Dt_RECORD_TSM2Soc;

typedef struct 
{
    uint8_t AS_lng_ctrl_st;    // 0 - false   1 - true
    uint8_t AS_lat_ctrl_st;    // 0 - false   1 - true 
    struct {
        uint8_t AEB:1;
        uint8_t AES:1;
        uint8_t ESA:1;
        uint8_t ELK:1;
    } AS_Switch_St;     
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
} Dt_RECORD_CANGATE2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp Diag_TimeStamp;
    uint8_t             mcu_mrm_system_fault_level;   // add  0 - 无安全停车故障   1 - 安全停车故障
    uint8_t             is_support_lane_stop;     // add 具备车道内停车的能力
    uint8_t             is_support_emergency_stop;   // add 具备紧急停车的能力
} Dt_RECORD_Diag2TSM;

typedef struct
{
    Dt_RECORD_TimeStamp DeTimeStamp;
    uint8_t             planningLite_control_state;
} Dt_RECORD_PLANLITE2TSM;

//****************************************************** output *******************************************//
typedef struct 
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp;
    uint8_t             MRM_Status;  // 0 - tor  1 - mrm  2 - mrm_es   3 - invalid
} Dt_RECORD_TSM2PLANLITE;

typedef struct
{
    Dt_RECORD_TimeStamp       Tsm_TimeStamp; 
    uint8_t                   MCU_MRM_St;
	uint8_t                   control_arb_request;   // 0 - soc  1 - mcu 
} Dt_RECORD_TSM2CtrlArb;

typedef struct
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp; 
    uint8_t             Tsm_Status;  //  0 - no error  1 - error
} Dt_RECORD_TSM2Diag; 

typedef struct
{
    Dt_RECORD_TimeStamp         Tsm_TimeStamp; 
    Dt_RECORD_VehicleStateReq   post_veh_st_req;  // 处理后的车辆状态请求， 是透传soc的，还是用MCU自己的，由MCU判断
    uint8_t                     post_warn_card_st;
    Dt_RECORD_TSM2Soc           Tsm_To_Soc;            
    Dt_RECORD_MCU2IFC           Mcu_To_Ifc;
} Dt_RECORD_TSM2CANGATE;
#endif