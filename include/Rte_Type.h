#ifndef RTE_TYPE_H_
#define RTE_TYPE_H_

#include <stdint.h>
#include <stdbool.h>
// -------- enum definition ---------

// -------------- seperate line ----------------

typedef struct tagTimeStamp
{
    bool is_valid;  // enum invalid
    uint32_t sec;
    uint32_t nsec;
} Dt_RECORD_TimeStamp;

typedef struct tagVehicleControlReq
{
    uint8_t           EPB_Request;           // EPB 请求
    uint8_t           Hazard_Light_Request;  // 双闪请求
    uint8_t           Ecall_Request;         // 打电话请求
    uint8_t           Door_Unlock_Request;   // 车门解锁请求
} Dt_RECORD_VehicleControlReq;

typedef struct tagAutomaton_State
{
    uint8_t NDA_Function_State;   // NDA 功能主状态， enum NdaFunctionSt
    uint8_t ICA_Function_State;   // ICA 功能主状态,  enum IcaFunctionSt
    uint8_t ACC_Function_State;   // ACC 功能主状态,  enum AccFunctionSt
} Dt_RECORD_Automaton_State;

typedef struct tagControl_Arbitrator_Results
{
    uint8_t CtrlArb_EPS_Control_Echo;  
    uint8_t CtrlArb_AEB_Control_Echo;
} Dt_RECORD_Control_Arbitrator_Results;

typedef struct tagNdaSmMonitorFlag
{
    uint16_t Standby_HandsFree_St_Monitor_Flag;
    uint16_t HandsOn_St_Monitor_Flag;
    uint16_t Override_St_Monitor_Flag;
} Dt_RECORD_NdaSmMonitorFlag;

typedef struct tagSoc_Info
{
    uint8_t                   NDA_Lane_Change_Type;       //  变道类型
    uint8_t                   NDA_Lane_Change_Direction;  //  变道方向
    uint8_t                   NDA_Lane_Change_State;      //   变道状态
    uint8_t                   NDA_Split_State;            //  分流
    uint8_t                   NDA_Behavior;               // NDA行为
    uint8_t                   NDA_Area_Type;              // NDA区域类型
    uint8_t                   NDA_Road_Type;              // NDA道路类型
    uint8_t                   NDA_Lane_Type;              // NDA车道线类型
    uint8_t                   ADC_Path_Merge_Type;        // 合流类型
    Dt_RECORD_Automaton_State Automaton_State;            // NDA ICA ACC状态
    uint8_t                   EPB_Request;                // soc的EPB 请求
    uint8_t                   Hazard_Light_Request;       // soc的双闪请求
    uint8_t                   Ecall_Request;              // soc的打电话请求
    uint8_t                   Door_Unlock_Request;        // soc的车门解锁请求
    uint8_t                   NDA_Odc_Flag_Before_Active; // NDA激活前的ODD标志位
    uint8_t                   NDA_Odc_Flag_After_Active;  // NDA激活后的ODD标志位
    uint8_t                   NDA_Enable_State;           // NDA使能状态
    uint8_t                   Planning_DriveOff_Req;         // soc侧的planning起步请求
    uint8_t                   SD_Map_HD_Map_Match_St;     // SD地图和高精地图的匹配状态， from localMap
    uint8_t                   User_Set_Navi;              // 用户设置导航， from navi
} Dt_RECORD_Soc_Info;

typedef struct tagVehicleSignal2TSM
{
    uint8_t VCU_EMS_AccPedalActPstVD;
    float   VCU_EMS_AccPedalActPst;  // 油门开度
    uint8_t BCS_MasterCylinderPrVD;
    float   BCS_MasterCylinderPr;  // 主缸压力
    uint8_t EPS_StrngWhlTorqVD;
    float   EPS_StrngWhlTorq;  // 驾驶员输入力矩, 【方向盘扭力】， 需要做低通滤波，去除路面噪声， 滤波频率??
    uint8_t EPS_SteeringAngleVD; 
    float   EPS_SteeringAngle;   // 方向盘角度
    uint8_t EPS_SteeringAngleSpdVD;
    float   EPS_SteeringAngleSpd;   // 方向盘转角速度
    uint8_t VCU_EMS_BrkPedalStVD;
    uint8_t VCU_EMS_BrkPedalSt;  // 刹车踏板状态
    uint8_t EBB_BrkPedPstVD;
    float   EBB_BrkPedPst;   //  当前刹车踏板位置
    uint8_t EBB_BrkPedalApplied;    // 刹车踏板是否启用, enum BrkPedalApplied
    uint8_t EBB_BrkPedalAppliedSt; // EBB_BrkPedalApplied 信号的状态， enum BrkPedalAppliedSt;
    uint8_t EPB_SwitchSt;   // EPB手刹信号， enum SwitchSt
    uint8_t EPB_SwitchStVD;
    uint8_t EPB_SysSt; // EPB系统状态， enum EpbSysSt
    uint8_t EPS_HandsOffDetStVD;
    uint8_t EPS_HandsOffDetSt;  // 手扶检测
    uint8_t HOD_TouchZone1;    // 手扶检测区域1 
    uint8_t HOD_TouchZone2;    // 手扶检测区域2
    uint8_t HOD_TouchZone3;     // 手扶检测区域3， HOD Status, 0 - false , 1 - true
    uint8_t HOD_CalibrationSt;    // HOD 标定状态, eum HodCalibrationSt
    uint8_t HOD_FaultStatus;      // HOD 故障状态， enum HODFaultStatus
    uint8_t VCU_AccDriverOrvd;   // 指示驾驶员的力矩是否高于ACC提供的力矩， enum AccDriverOrvd
    uint8_t VCU_CrntGearLvlVD;    //  0 - invalid， 1 - valid
    uint8_t VCU_CrntGearLvl;    // 车辆行驶挡位, enum CrntGearLevel
    uint8_t BCM_FrontWiperSt;    // 雨刮速度， enum FrontWiperSt
    uint8_t BCM_DriverDoorAjarSt;   // 主驾车门状态， enum DoorSt
    uint8_t BCM_PsngrDoorAjarSt;    // 副驾车门状态， enum DoorSt
    uint8_t BCM_RLDoorAjarSt;       // 左后门状态， enum DoorSt
    uint8_t BCM_RRDoorAjarSt;       // 右后门状态， enum DoorSt
    uint8_t BCM_TrunkAjarSt;        // 后备箱门状态， enum DoorSt
    uint8_t SRS_DriverSeatBeltSt;   // 驾驶员系安全带状态， enum DriverSeatBeltSt
    uint8_t BCS_YawRateSt;     // 横摆角速度状态， IsValid
    float   BCS_YawRate;    // 横摆角速度大小
    uint8_t BCS_ActVehLaltrlAccelVD; 
    float   BCS_ActVehLaltrlAccel;   // 横向加速度大小
    uint8_t BCS_ActVehLongAccelVD;
    float   BCS_ActVehLongAccel;    // 纵向加速度大小
    uint8_t VCU_VehRdySt;     // 牵引满足标志位
    uint8_t BCM_KeySt;      // 车辆点火状态， enum KeySt
    uint8_t BCM_HazardLampSt;    // 车辆双闪灯状态， enum IsActive
    uint8_t SRS_CrashOutputSt;     // 碰撞信息， enum CrashOutputSt
    uint8_t DMS_DrvrDetSts; // 驾驶员监控状态， enum DMSDrvrDetSts
    uint8_t DMS_InterestsRegion;  // DMS关注区域， enum DMSInterestsRegion
    uint16_t DMS_L3DriverFatigueSt;   // L3自动驾驶下级驾驶员疲劳状态， enum DMSL3DriverFatigueSt
    uint8_t BCS_VehicleStandStillSt;   // 车辆静止状态， enum BCSVehicleStandStillSt
} Dt_RECORD_VehicleSignal2TSM;

typedef struct tagTSM2Soc
{
    // 把对应的时间戳一并收了转了，例如以下决策仲裁的时间戳 和 控制仲裁的时间戳
    uint32_t                             AS_Status;                   // AS 状态
    Dt_RECORD_Control_Arbitrator_Results Control_Arbitrator_Results;  // 控制仲裁结果
    uint8_t                              Lane_Change_Allow_Flag;      // 是否允许变道

} Dt_RECORD_TSM2Soc;

// ---------------- inside struct ------------------
typedef struct tagInsideTimeStamp
{
    Dt_RECORD_TimeStamp Control_Arbitrator_TimeStamp;
    Dt_RECORD_TimeStamp Decision_Arbitrator_TimeStamp;
    Dt_RECORD_TimeStamp CAN_Switch_TimeStamp;
    Dt_RECORD_TimeStamp Diag_TimeStamp;
    Dt_RECORD_TimeStamp Planning_Lite_TimeStamp;
} InsideTimeStamp;

typedef struct tagInsideControlArbRelative
{
    uint32_t                             AS_Status;                   // AS 状态
    Dt_RECORD_Control_Arbitrator_Results                  Control_Arbitrator_Results;
} InsideControlArbRelative;

typedef struct tagInsideDecisionArbRelative
{
    uint8_t             Lane_Change_Allow_Flag;  // 是否允许变道
} InsideDecisionArbRelative;

typedef struct tagInsideSocInfo
{
    Dt_RECORD_Soc_Info          Soc_Info;               // soc的消息
    uint8_t             Soc_Fault_Status;  // soc故障状态   0 - 未故障   1 - 故障
} InsideSocInfo;

typedef struct tagInsideVehicleToTsm
{
    Dt_RECORD_VehicleSignal2TSM Vehicle_Signal_To_Tsm;  // 车身信号
} InsideVehicleToTsm;

typedef struct tagInsidePlanningLiteRelative
{
    uint8_t Planning_Control_State;
} InsidePlanningLiteRelative;
// ---------------- seperate line ------------------

//****************************************************** input *******************************************//
/*  
    - CANSwtich   CANGATE
    - 控制仲裁     CONARB
    - 控制模块  CON
    - 最小定位   LOCLITE
    - 最小规划   PLANLITE
    - 人机交互  HMI
    - SOC    SOC
    - 决策仲裁    DECIARB
    - 融合   FUSN
*/
// ControlArbitrator --> TSM
typedef struct tagControlArbitrator2TSM
{
    Dt_RECORD_Control_Arbitrator_Results Control_Arbitrator_Results;  // 控制仲裁结果
    uint32_t                             AS_Status;                   // AS 状态
    Dt_RECORD_TimeStamp                  Control_Arbitrator_TimeStamp;
} Dt_RECORD_ControlArbitrator2TSM;

// DecisionArbitrator --> TSM
typedef struct tagDecisionArbitrator2TSM
{
    uint8_t             Lane_Change_Allow_Flag;  // 是否允许变道
    uint16_t            Check_Info;              // 各种check的结果
    Dt_RECORD_TimeStamp Decision_Arbitrator_TimeStamp;
} Dt_RECORD_DecisionArbitrator2TSM;

// CANSwitch --> TSM  保持和IFC环境同步
typedef struct tagDeCANGATE2TSM
{
    Dt_RECORD_TimeStamp         TimeStamp;
    Dt_RECORD_VehicleSignal2TSM Vehicle_Signal_To_Tsm;   // 车身信号
    Dt_RECORD_Soc_Info          Soc_Info;                // soc的消息
} Dt_RECORD_CANGATE2TSM;
// typedef struct tagDeCANSwitch2TSM
// {
//     Dt_RECORD_TimeStamp         CAN_Switch_TimeStamp;
//     Dt_RECORD_VehicleSignal2TSM Vehicle_Signal_To_Tsm;  // 车身信号
//     Dt_RECORD_Soc_Info          Soc_Info;               // soc的消息  
// } Dt_RECORD_CANSwitch2TSM;

// Diag --> TSM
typedef struct tagDiag2TSM
{
    Dt_RECORD_TimeStamp Diag_TimeStamp;
    uint8_t             Fault_Level;       // 故障等级
    uint8_t             Soc_Fault_Status;  // soc故障状态   0 - 未故障   1 - 故障
    uint8_t             NDA_System_Fault_Level; // NDA系统故障等级， enum NDASystemFaultLevel
} Dt_RECORD_Diag2TSM;

// PlanningLite --> TSM
typedef struct tagPlanning2TSM
{
    Dt_RECORD_TimeStamp Planning_Lite_TimeStamp;
    uint8_t             Planning_Control_State;  // 规控能力
} Dt_RECORD_PLANLITE2TSM;

//****************************************************** output *******************************************//
// TSM --> PlanningLite   保持和IFC环境一致
typedef struct tagTSM2Planninglite
{
    Dt_RECORD_TimeStamp DeTimeStamp;        // 自己创建的时间戳
    Dt_RECORD_TimeStamp TimeStamp;   // 纯转发消息的时间戳
    uint8_t             NDA_Lane_Change_Type;
    uint8_t             NDA_Lane_Change_Direction;
    uint8_t             NDA_Lane_Change_State;  // 包含变道请求
    uint8_t             MRM_Status;             // 告知planning lite 退出的标志位， 状态机 0 - 退出 ， 1 - 激活 
} Dt_RECORD_TSM2PLANLITE;
// typedef struct tagTSM2Planninglite
// {
//     Dt_RECORD_TimeStamp Tsm_TimeStamp;        // 自己创建的时间戳
//     Dt_RECORD_TimeStamp CAN_Switch_TimeStamp;   // 纯转发消息的时间戳
//     bool                Lat_Takeover_Flag;  // 横向接管标志位
//     bool                Lon_Takeover_Flag;
//     bool                Lat_Override_Flag;
//     bool                Lon_Override_Flag;
//     uint8_t             NDA_Lane_Change_Type;
//     uint8_t             NDA_Lane_Change_Direction;
//     uint8_t             NDA_Lane_Change_State;  // 包含变道请求
//     uint8_t             MRM_Status;             // 告知planning lite 退出的标志位， 状态机 0 - 退出 ， 1 - 激活 
// } Dt_RECORD_TSM2Planninglite;

// tsm --> Control Arbitrator
typedef struct tagTSM2CtrlArb
{
    Dt_RECORD_TimeStamp TimeStamp;   // 接收消息的时间戳
    uint8_t holo_planning_control_status;   // from PlanLite  归控能力
    Dt_RECORD_Automaton_State Automaton_State;  // soc状态机状态
    uint8_t lng_override_flag;    // 纵向override状态， 1为 override
} Dt_RECORD_TSM2CtrlArb;
// typedef struct tagTSM2ControlArbitrator
// {
//     Dt_RECORD_TimeStamp CAN_Switch_TimeStamp;   // 接收消息的时间戳
//     uint8_t Planning_Control_State;   // from PlanLite  归控能力
//     Dt_RECORD_Automaton_State Automaton_State;  // soc状态机状态
// } Dt_RECORD_TSM2ControlArbitrator;

// tsm --> Decision Arbitrator
typedef struct tagTSM2DecisionArbitrator
{
    Dt_RECORD_TimeStamp CAN_Switch_TimeStamp;   // 接收消息的时间戳
    uint8_t NDA_Lane_Change_Type;
    uint8_t NDA_Lane_Change_Direction;
    uint8_t NDA_Lane_Change_State;
    uint8_t NDA_Split_State;
    uint8_t NDA_Behavior;
    uint8_t NDA_Area_Type;
    uint8_t NDA_Road_Type;
    uint8_t NDA_Lane_Type;
    uint8_t ADC_Path_Merge_Type;
} Dt_RECORD_TSM2DecisionArbitrator;

// tsm --> Diag
typedef struct tagTSM2Diag
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp;    // 自己创建的时间戳
    uint8_t             Tsm_Status;  // 模块状态
} Dt_RECORD_TSM2Diag;

// tsm --> HMI
typedef struct tagTSM2HMI
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp;   // 自己创建的时间戳
    uint8_t Fault_Info;   // 故障信息,点灯
    uint8_t Tor_Request;  // 请求接管
} Dt_RECORD_TSM2HMI;

// tsm --> CANswitch
typedef struct tagTSM2CANSwitch
{
    Dt_RECORD_TimeStamp Tsm_TimeStamp;   // 自己创建的时间戳
    Dt_RECORD_TimeStamp Control_Arbitrator_TimeStamp;  // 控制仲裁的时间戳
    Dt_RECORD_TimeStamp Decision_Arbitrator_TimeStamp;   // 决策仲裁的时间戳
    // 此处需要基于soc_fault_state 做区分
    uint8_t           EPB_Request;           // EPB 请求
    uint8_t           Hazard_Light_Request;  // 双闪请求
    uint8_t           Ecall_Request;         // 打电话请求
    uint8_t           Door_Unlock_Request;   // 车门解锁请求
    Dt_RECORD_TSM2Soc Tsm_To_Soc;            // 给到soc的消息
} Dt_RECORD_TSM2CANSwitch;

#endif