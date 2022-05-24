import ctypes

class MyStructure(ctypes.Structure):

    def set_attr(self, attr_name, attr_value):
        for key, attr_type in self._fields_:
            if type(attr_type) == type(ctypes.Structure):
                ak = getattr(self, key)
                ak.set_attr(attr_name, attr_value)
            elif attr_name == key:
                if attr_type == ctypes.c_ubyte or \
                   attr_type == ctypes.c_uint:
                    value = int(attr_value)
                elif attr_type == ctypes.c_float:
                    value = float(attr_value)
                setattr(self, attr_name, attr_type(value))


class TimeStamp(MyStructure):
    _fields_ = [
        ("Timestamp_valid", ctypes.c_bool),
        ("Timestamp_sec", ctypes.c_uint),
        ("Timestamp_nsec", ctypes.c_uint)
    ]

class VehSignal(MyStructure):
    _fields_ = [
        ("VCU_EMS_AccPedalActPstVD", ctypes.c_ubyte),
        ("VCU_EMS_AccPedalActPst", ctypes.c_float),
        ("BCS_MasterCylinderPrVD", ctypes.c_ubyte),
        ("BCS_MasterCylinderPr", ctypes.c_float),
        ("EPS_StrngWhlTorqVD", ctypes.c_ubyte),
        ("EPS_StrngWhlTorq", ctypes.c_float),
        ("EPS_SteeringAngleVD", ctypes.c_ubyte),
        ("EPS_SteeringAngle", ctypes.c_float),
        ("EPS_SteeringAngleSpdVD", ctypes.c_ubyte),
        ("EPS_SteeringAngleSpd", ctypes.c_float),
        ("VCU_EMS_BrkPedalStVD", ctypes.c_ubyte),
        ("VCU_EMS_BrkPedalSt", ctypes.c_ubyte),
        ("EBB_BrkPedPstVD", ctypes.c_ubyte),
        ("EBB_BrkPedPst", ctypes.c_float),
        ("EBB_BrkPedalApplied", ctypes.c_ubyte),
        ("EBB_BrkPedalAppliedSt", ctypes.c_ubyte),
        ("EPB_SwitchSt", ctypes.c_ubyte),
        ("EPB_SwitchStVD", ctypes.c_ubyte),
        ("EPB_SysSt", ctypes.c_ubyte),
        ("EPS_HandsOffDetStVD", ctypes.c_ubyte),
        ("EPS_HandsOffDetSt", ctypes.c_ubyte),
        ("HOD_TouchZone1", ctypes.c_ubyte),
        ("HOD_TouchZone2", ctypes.c_ubyte),
        ("HOD_TouchZone3", ctypes.c_ubyte),
        ("HOD_CalibrationSt", ctypes.c_ubyte),
        ("HOD_FaultStatus", ctypes.c_ubyte),
        ("VCU_AccDriverOrvd", ctypes.c_ubyte),
        ("VCU_CrntGearLvlVD", ctypes.c_ubyte),
        ("VCU_CrntGearLvl", ctypes.c_ubyte),
        ("BCM_FrontWiperSt", ctypes.c_ubyte),
        ("BCM_DriverDoorAjarSt", ctypes.c_ubyte),
        ("BCM_PsngrDoorAjarSt", ctypes.c_ubyte),
        ("BCS_MasterCylinderPrVD", ctypes.c_ubyte),
        ("BCM_RLDoorAjarSt", ctypes.c_ubyte),
        ("BCM_RRDoorAjarSt", ctypes.c_ubyte),
        ("BCM_TrunkAjarSt", ctypes.c_ubyte),
        ("SRS_DriverSeatBeltSt", ctypes.c_ubyte),
        ("BCS_YawRateSt", ctypes.c_ubyte),
        ("BCS_YawRate", ctypes.c_float),
        ("BCS_ActVehLaltrlAccelVD", ctypes.c_ubyte),
        ("BCS_ActVehLaltrlAccel", ctypes.c_float),
        ("BCS_ActVehLongAccelVD", ctypes.c_ubyte),
        ("BCS_ActVehLongAccel", ctypes.c_float),
        ("VCU_VehRdySt", ctypes.c_ubyte),
        ("BCM_KeySt", ctypes.c_ubyte),
        ("BCM_HazardLampSt", ctypes.c_ubyte),
        ("SRS_CrashOutputSt", ctypes.c_ubyte),
        ("DMS_DrvrDetSts", ctypes.c_ubyte),
        ("DMS_InterestsRegion", ctypes.c_ubyte),
        ("DMS_L3DriverFatigueSt", ctypes.c_ubyte),
        ("DMS_L3DriverDistractionSt", ctypes.c_ubyte),
        ("BCS_VehicleStandStillSt", ctypes.c_ubyte),
        ("EMS_GasPedalActPstforMRRVD", ctypes.c_ubyte),
        ("EMS_GasPedalActPstforMRR", ctypes.c_float),
        ("BCM_LeftTurnLampSt", ctypes.c_ubyte),
        ("BCM_RightTurnLampSt", ctypes.c_ubyte),
        ("BCS_VehSpdVD", ctypes.c_ubyte),
        ("BCS_VehSpd", ctypes.c_float),
    ]

class ScenarioType(MyStructure):
    _fields_ = [
        ("NDA_Lane_Change_Type", ctypes.c_ubyte),
        ("NDA_Lane_Change_Direction", ctypes.c_ubyte),
        ("NDA_Lane_Change_State", ctypes.c_ubyte),
        ("NDA_Split_State", ctypes.c_ubyte),
        ("NDA_behavior", ctypes.c_ubyte),
        ("NDA_Area_Type", ctypes.c_ubyte),
        ("NDA_road_type", ctypes.c_ubyte),
        ("NDA_lane_type", ctypes.c_ubyte),
        ("ADC_path_merge_type", ctypes.c_ubyte)
    ]

class AutomatonState(MyStructure):
    _fields_ = [
        ("NDA_Function_State", ctypes.c_ubyte),
        ("ICA_Function_State", ctypes.c_ubyte),
        ("ACC_Function_State", ctypes.c_ubyte)
    ]

class VehCtrlReq(MyStructure):
    _fields_ = [
        ("EPB_Request", ctypes.c_ubyte),
        ("Hazard_Light_Request", ctypes.c_ubyte),
        ("Ecall_Request", ctypes.c_ubyte),
        ("Door_Unlock_Request", ctypes.c_ubyte)
    ]

class MonitorSigSrc(MyStructure):
    _fields_ = [
        ("NDA_Enable_State", ctypes.c_ubyte),
        ("Planning_DriveOff_Req", ctypes.c_ubyte),
        ("SD_Map_HD_Map_Match_St", ctypes.c_ubyte),
        ("EEA_Status", ctypes.c_ubyte),
        ("User_Set_Navi_Status", ctypes.c_ubyte),
        ("Global_Location_Accuracy", ctypes.c_ubyte),
        ("Relative_Location_Accuracy", ctypes.c_ubyte),
        ("HandsOn_HandsFree_Flag", ctypes.c_ubyte),
        ("NDA_Planning_Request", ctypes.c_ubyte),
        ("HD_Map_Warning_Dist", ctypes.c_float)
    ]

class SocInfo(MyStructure):
    _fields_ = [
        ("scenario_type", ScenarioType),
        ("automaton_state", AutomatonState),
        ("soc_hmi_request", VehCtrlReq),
        ("monitor_sig_src", MonitorSigSrc),
        ("soc_tor_req", ctypes.c_ubyte),
    ]

class CangateToTsm(MyStructure):
    _fields_ = [
        ("TimeStamp", TimeStamp),
        ("Vehicle_Signal_To_Tsm", VehSignal),
        ("Soc_Info", SocInfo),
        ("mcu_tor_req", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class CtrlArbRes(MyStructure):
    _fields_ = [
        ("CtrlArb_EPS_Control_Echo", ctypes.c_ubyte),
        ("CtrlArb_AEB_Control_Echo", ctypes.c_ubyte)
    ]

class CtrlArbToTsm(MyStructure):
    _fields_ = [
        ("Control_Arbitrator_Results", CtrlArbRes),
        ("AS_Status", ctypes.c_uint),
        ("timestamp", TimeStamp),
        ("Parking_EPS_handshake_state", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class DeciArbToTsm(MyStructure):
    _fields_ = [
        ("Lane_Change_Allow_Flag", ctypes.c_ubyte),
        ("Check_Info", ctypes.c_ushort),
        ("Decision_Arbitrator_TimeStamp", TimeStamp)
    ]

# ------------------------------------------------------------------------------
class DiagToTsm(MyStructure):
    _fields_ = [
        ("Diag_TimeStamp", TimeStamp),
        ("com_fault_with_adc", ctypes.c_ubyte),
        ("is_support_lane_stop", ctypes.c_ubyte),
        ("is_support_emergency_stop", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class PlanliteToTsm(MyStructure):
    _fields_ = [
        ("DeTimeStamp", TimeStamp),
        ("planningLite_control_state", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class TsmToPlanlite(MyStructure):
    _fields_ = [
        ("DeTimeStamp", TimeStamp),
        ("TimeStamp", TimeStamp),
        ("NDA_Lane_Change_Type", ctypes.c_ubyte),
        ("NDA_Lane_Change_Direction", ctypes.c_ubyte),
        ("NDA_Lane_Change_State", ctypes.c_ubyte),
        ("MRM_Status", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class TsmToCtrlArb(MyStructure):
    _fields_ = [
        ("DeTimeStamp", TimeStamp),
        ("TimeStamp_PlannLite", TimeStamp),
        ("TimeStamp_CanGate", TimeStamp),
        ("holo_planning_control_status", ctypes.c_ubyte),
        ("Automaton_State", AutomatonState),
        ("long_override_flag", ctypes.c_ubyte),
        ("lat_override_flag", ctypes.c_ubyte),
        ("NDA_LatState", ctypes.c_ubyte),
        ("NDA_ILC_State", ctypes.c_ubyte),
        ("NDA_LongState", ctypes.c_ubyte),
        ("control_arb_request", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class TsmToDeciArb(MyStructure):
    _fields_ = [
        ("TimeStamp", TimeStamp),
        ("DeScenarioType", ScenarioType)
    ]

# ------------------------------------------------------------------------------
class TsmToDiag(MyStructure):
    _fields_ = [
        ("Tsm_TimeStamp", TimeStamp),
        ("Tsm_Status", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class TsmToHmi(MyStructure):
    _fields_ = [
        ("Tsm_TimeStamp", TimeStamp),
        ("Fault_Info", ctypes.c_ubyte),
        ("Tor_Request", ctypes.c_ubyte)
    ]

# ------------------------------------------------------------------------------
class TsmToSoc(MyStructure):
    _fields_ = [
        ("AS_Status", ctypes.c_uint),
        ("Control_Arbitrator_Results", CtrlArbRes),
        ("Lane_Change_Allow_Flag", ctypes.c_ubyte),
        ("AS_HandsoffTimerRequest", ctypes.c_ubyte),
        ("tsm_timestamp", TimeStamp),
        ("Parking_EPS_handshake_state", ctypes.c_ubyte),
        ("MCU_MRM_Active_St", ctypes.c_ubyte),
        ("Request_NDA_Exit", ctypes.c_ubyte)
    ]

class McuToIfc(MyStructure):
    _fields_ = [
        ("time_stamp", TimeStamp),
        ("MCU_MRM_status", ctypes.c_ubyte),
        ("Request_Mrm_From_MCU", ctypes.c_ubyte)
    ]

class TsmToCangate(MyStructure):
    _fields_ = [
        ("Tsm_TimeStamp", TimeStamp),
        ("Control_Arbitrator_TimeStamp", TimeStamp),
        ("Decision_Arbitrator_TimeStamp", TimeStamp),
        ("post_hmi_request", VehCtrlReq),
        ("Tsm_To_Soc", TsmToSoc),
        ("Mcu_To_Ifc", McuToIfc)
    ]