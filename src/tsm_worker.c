/*
 * Copyright (C) HoloMatic Technology(Beijing) Co., Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
/*!
 *  \brief  MRM top state machine main structure implementation
 *  \author zxl
 *  \date   2022-03-20
 *  \attention Copyright © Holomatic Technology (Beijing) Co.Ltd
 *  \attention Please refer to COPYRIGHT.txt for complete terms of copyright Juni24.
 */

#include "tsm_worker.h"
#include "tsm_signalhandling.h"
#include "tsm_user.h"
#include "tsm_warning.h"

#define TIMESTAMP_MAX_NUM 3

void 
MRM_TSM_MODULE_Init(void) {
}

static inline void
tsm_set_entry(struct tsm_entry* p_entry, 
              const Dt_RECORD_CtrlArb2TSM* rtu_DeCtrlArb2TSM,
              const Dt_RECORD_DecisionArbitrator2TSM* rtu_DeDecisionArbitrator2TSM,
              const Dt_RECORD_CANGATE2TSM* rtu_DeCANGATE2TSM,
              const Dt_RECORD_Diag2TSM* rtu_DeDiag2TSM,
              const Dt_RECORD_PLANLITE2TSM* rtu_DePlanlite2Tsm) {
    p_entry->in_can_gate = rtu_DeCANGATE2TSM;
    p_entry->in_ctrl_arb = rtu_DeCtrlArb2TSM;
    p_entry->in_deci_arb = rtu_DeDecisionArbitrator2TSM;
    p_entry->in_diag = rtu_DeDiag2TSM;
    p_entry->in_planlite = rtu_DePlanlite2Tsm;
}

static inline void 
tsm_set_exit(struct tsm_exit* p_exit,
             Dt_RECORD_TSM2PLANLITE* rty_DeTsm2Planlite,
             Dt_RECORD_TSM2CtrlArb* rty_DeTSM2CtrlArb, 
             Dt_RECORD_TSM2DecisionArbitrator* rty_DeTSM2DecisionArbitrator, 
             Dt_RECORD_TSM2Diag* rty_DeTSM2Diag, 
             Dt_RECORD_TSM2HMI* rty_DeTSM2HMI, 
             Dt_RECORD_TSM2CANGATE* rty_DeTSM2CANGATE) {
    p_exit->out_can_gate = rty_DeTSM2CANGATE;
    p_exit->out_ctrl_arb = rty_DeTSM2CtrlArb;
    p_exit->out_deci_arb = rty_DeTSM2DecisionArbitrator;
    p_exit->out_diag = rty_DeTSM2Diag;
    p_exit->out_hmi = rty_DeTSM2HMI;
    p_exit->out_planlite = rty_DeTsm2Planlite;
}

static boolean
tsm_is_timestamp_error(const Dt_RECORD_TimeStamp* cur_ts,
                       const Dt_RECORD_TimeStamp* last_ts) {
    return (!cur_ts->Timestamp_valid ||
            (cur_ts->Timestamp_sec <= last_ts->Timestamp_sec));
}

static boolean
tsm_is_timestamp_lost(const Dt_RECORD_TimeStamp* cur_st,
                      const Dt_RECORD_TimeStamp* last_st) {
    static const float32 NS_IN_MS = 1000000.0;
    static const float32 STAND_CYCLE = 20.0;
    static const float32 LIMIT_DEVIATION = 2.0;

    float32 delta_time = 
        (float32)(cur_st->Timestamp_nsec - last_st->Timestamp_nsec) / NS_IN_MS;
    
    return (fabs(delta_time - STAND_CYCLE) > LIMIT_DEVIATION);
}

static boolean
tsm_validate_timestamp(const struct tsm_entry* p_entry) {
    // 1st col is cur, 2nd col is last
    static Dt_RECORD_TimeStamp timestamp[TIMESTAMP_MAX_NUM][2] = {0};
    boolean res = true;

    memcpy(&timestamp[0][0], &(p_entry->in_can_gate->TimeStamp), 
           sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp[1][0], &(p_entry->in_diag->Diag_TimeStamp),
           sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp[2][0], &(p_entry->in_planlite->DeTimeStamp),
           sizeof(Dt_RECORD_TimeStamp));
    
    for (size_t i = 0; i < ARRAY_LEN(timestamp); ++i) {
        if (tsm_is_timestamp_error(&timestamp[i][0], &timestamp[i][1]) ||
            tsm_is_timestamp_lost(&timestamp[i][0], &timestamp[i][1])) {
            res = false;
            break;
        }
    }

    memcpy(&timestamp[0][1], &(p_entry->in_can_gate->TimeStamp), 
           sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp[1][1], &(p_entry->in_diag->Diag_TimeStamp),
           sizeof(Dt_RECORD_TimeStamp));
    memcpy(&timestamp[2][1], &(p_entry->in_planlite->DeTimeStamp),
           sizeof(Dt_RECORD_TimeStamp));
    return res;
}

static void
tsm_calculate_timestamp(Dt_RECORD_TimeStamp* tsm_ts) {
    // todo:
    tsm_ts->Timestamp_valid = 0;
    tsm_ts->Timestamp_sec = 0;
    tsm_ts->Timestamp_nsec = 0;
}

static void
tsm_fill_planlite_exit(const struct tsm_entry* p_entry,
                       const struct tsm_action* p_action,
                       const Dt_RECORD_TimeStamp* tsm_ts,
                       Dt_RECORD_TSM2PLANLITE* exit_planlite) {
    memcpy(&exit_planlite->TimeStamp, &p_entry->in_can_gate->TimeStamp, 
           sizeof(Dt_RECORD_TimeStamp));
    memcpy(&exit_planlite->DeTimeStamp, tsm_ts, sizeof(Dt_RECORD_TimeStamp));
    exit_planlite->NDA_Lane_Change_Type = 
        p_entry->in_can_gate->Soc_Info.scenario_type.NDA_Lane_Change_Type;
    exit_planlite->NDA_Lane_Change_Direction = 
        p_entry->in_can_gate->Soc_Info.scenario_type.NDA_Lane_Change_Direction;
    exit_planlite->NDA_Lane_Change_State = 
        p_entry->in_can_gate->Soc_Info.scenario_type.NDA_Lane_Change_State; 
    exit_planlite->MRM_Status = p_action->mrm_activation_st;
}

static void
tsm_fill_ctrl_arb_exit(const struct tsm_entry* p_entry,
                       const struct tsm_action* p_action,
                       const Dt_RECORD_TimeStamp* tsm_ts,
                       Dt_RECORD_TSM2CtrlArb* exit_ctrl_arb) {
    memcpy(&exit_ctrl_arb->TimeStamp_CanGate, 
           &p_entry->in_can_gate->TimeStamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&exit_ctrl_arb->TimeStamp_PlannLite, 
           &p_entry->in_planlite->DeTimeStamp, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&exit_ctrl_arb->DeTimeStamp, tsm_ts, sizeof(Dt_RECORD_TimeStamp));

    exit_ctrl_arb->holo_planning_control_status = 
        p_entry->in_planlite->planningLite_control_state;
    memcpy(&exit_ctrl_arb->Automaton_State, 
           &p_entry->in_can_gate->Soc_Info.automaton_state, sizeof(tsm_soc_st));
    exit_ctrl_arb->long_override_flag = p_action->lng_override_flag;
    exit_ctrl_arb->lat_override_flag = p_action->lat_override_flag;
    // todo:
    exit_ctrl_arb->NDA_LatState = 0;
    exit_ctrl_arb->NDA_LongState = 0;
    exit_ctrl_arb->NDA_ILC_State = 0;
	exit_ctrl_arb->control_arb_request = p_action->control_arb_request;
}

static void
tsm_fill_deci_arb_exit(const struct tsm_entry* p_entry,
                       Dt_RECORD_TSM2DecisionArbitrator* exit_deci_arb) {
    memcpy(&exit_deci_arb->TimeStamp, &p_entry->in_can_gate->TimeStamp, 
           sizeof(Dt_RECORD_TimeStamp));
    memcpy(&exit_deci_arb->DeScenarioType, 
           &p_entry->in_can_gate->Soc_Info.scenario_type,
           sizeof(Dt_RECORD_ScenarioType));
}

static void
tsm_fill_diag_exit(const Dt_RECORD_TimeStamp* tsm_ts, 
                   Dt_RECORD_TSM2Diag* exit_diag) {
    memcpy(&exit_diag->Tsm_TimeStamp, tsm_ts, sizeof(Dt_RECORD_TimeStamp));
    // todo:
    exit_diag->Tsm_Status = 0;
}

static void
tsm_fill_hmi_exit(const Dt_RECORD_TimeStamp* tsm_ts,
                  Dt_RECORD_TSM2HMI* exit_hmi) {
    memcpy(&exit_hmi->Tsm_TimeStamp, tsm_ts, sizeof(Dt_RECORD_TimeStamp));
    // TODO:
    exit_hmi->Fault_Info = 0;  
    // TODO:
    exit_hmi->Tor_Request = 0;
}

static void
tsm_fill_cangate_exit(const struct tsm_entry* p_entry,
                      const struct tsm_action* p_action,
                      const Dt_RECORD_TimeStamp* tsm_ts,
                      const enum tsm_ifc_mrm_func_st mrm_state, 
                      Dt_RECORD_TSM2CANGATE* exit_can_gate) {
    memcpy(&exit_can_gate->Tsm_TimeStamp, tsm_ts, sizeof(Dt_RECORD_TimeStamp));
    memcpy(&exit_can_gate->Decision_Arbitrator_TimeStamp, 
           &p_entry->in_deci_arb->Decision_Arbitrator_TimeStamp,
           sizeof(Dt_RECORD_TimeStamp));
    memcpy(&exit_can_gate->Control_Arbitrator_TimeStamp, 
           &p_entry->in_ctrl_arb->timestamp, sizeof(Dt_RECORD_TimeStamp));
    // todo:
    exit_can_gate->post_hmi_request.EPB_Request = 0;
    // todo:
    exit_can_gate->post_hmi_request.Hazard_Light_Request = 0;
    // todo:
    exit_can_gate->post_hmi_request.Ecall_Request = 0;
    // todo:
    exit_can_gate->post_hmi_request.Door_Unlock_Request = 0;
    exit_can_gate->Tsm_To_Soc.AS_Status = p_entry->in_ctrl_arb->AS_Status;
    memcpy(&exit_can_gate->Tsm_To_Soc.Control_Arbitrator_Results, 
           &p_entry->in_ctrl_arb->Control_Arbitrator_Results, 
           sizeof(Dt_RECORD_Control_Arbitrator_Results));
    exit_can_gate->Tsm_To_Soc.Lane_Change_Allow_Flag = 
        p_entry->in_deci_arb->Lane_Change_Allow_Flag;
    exit_can_gate->Tsm_To_Soc.Parking_EPS_handshake_state = 
        p_entry->in_ctrl_arb->Parking_EPS_handshake_state;
    exit_can_gate->Tsm_To_Soc.MCU_MRM_Active_St = tsm_is_mrm_active(mrm_state);
    memcpy(&exit_can_gate->Mcu_To_Ifc.time_stamp, 
           tsm_ts, sizeof(Dt_RECORD_TimeStamp));
    // todo:
    exit_can_gate->Mcu_To_Ifc.MCU_MRM_status = 0;
}

static void
tsm_process_exit(const struct tsm_entry* p_entry,
                 const struct tsm_action* p_action,
                 const enum tsm_ifc_mrm_func_st mrm_state, 
                 struct tsm_exit* p_exit) {
    Dt_RECORD_TimeStamp tsm_timestamp;
    tsm_calculate_timestamp(&tsm_timestamp);

    tsm_fill_planlite_exit(p_entry, p_action, &tsm_timestamp,
                           p_exit->out_planlite);

    tsm_fill_ctrl_arb_exit(p_entry, p_action, &tsm_timestamp,
                           p_exit->out_ctrl_arb);
    
    tsm_fill_deci_arb_exit(p_entry, p_exit->out_deci_arb);

    
    tsm_fill_diag_exit(&tsm_timestamp, p_exit->out_diag);

    tsm_fill_hmi_exit(&tsm_timestamp, p_exit->out_hmi);

    tsm_fill_cangate_exit(p_entry, p_action, &tsm_timestamp, mrm_state, 
                          p_exit->out_can_gate);
}

void 
MRM_Swc_V_TSM(const Dt_RECORD_CtrlArb2TSM *rtu_DeCtrlArb2TSM, 
              const Dt_RECORD_DecisionArbitrator2TSM *rtu_DeDecisionArbitrator2TSM, 
              const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
              const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM,
              const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm, 
              Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
              Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb, 
              Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
              Dt_RECORD_TSM2Diag *rty_DeTSM2Diag, 
              Dt_RECORD_TSM2HMI *rty_DeTSM2HMI, 
              Dt_RECORD_TSM2CANGATE *rty_DeTSM2CANGATE)
{
    static struct tsm_entry entry;
    static struct tsm_exit exit;
    tsm_set_entry(&entry, rtu_DeCtrlArb2TSM, rtu_DeDecisionArbitrator2TSM, 
                  rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);
    tsm_set_exit(&exit, rty_DeTsm2Planlite, rty_DeTSM2CtrlArb, 
                 rty_DeTSM2DecisionArbitrator, rty_DeTSM2Diag, 
                 rty_DeTSM2HMI, rty_DeTSM2CANGATE);

    static enum tsm_ifc_mrm_func_st mrm_state = IFC_PASSIVE;
    static enum tsm_warning_st warning_state = NO_WARNING;
    static struct tsm_action action;
    // todo:
    (void)tsm_validate_timestamp(&entry);

    static struct tsm_intermediate_sig int_sig;
    tsm_preprocess_input(&int_sig, &entry, mrm_state);

    mrm_state = 
        tsm_run_user(mrm_state, warning_state, &entry, &int_sig, &action);

    warning_state = 
        tsm_run_warning_user(warning_state, mrm_state, &entry, &int_sig);

    tsm_process_exit(&entry, &action, mrm_state, &exit);
}