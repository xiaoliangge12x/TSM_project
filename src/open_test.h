#ifndef OPEN_TEST_H_
#define OPEN_TEST_H_

#include <yaml.h>
#include <stdlib.h>

#include "Rte_Type.h"
#include "tsm_parameter.h"
// -------------------------  macro         ---------------------------------
#define MAX_INTERMEDIATE_NAME    50U
#define ENABLE_SET_INTERMEDIATE

enum tsm_test_sig_type {
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_FLOAT32,
};

struct tsm_simulink_input {
    Dt_RECORD_CtrlArb2TSM            rt_in_ctrlarb_tsm;
    Dt_RECORD_CANGATE2TSM            rt_in_cangate_tsm;
    Dt_RECORD_Diag2TSM               rt_in_diag_tsm;
    Dt_RECORD_PLANLITE2TSM           rt_in_planlite_tsm;
};

struct tsm_simulink_output {
    Dt_RECORD_TSM2PLANLITE           rt_out_tsm_planlite;
    Dt_RECORD_TSM2CtrlArb            rt_out_tsm_ctrlarb;
    Dt_RECORD_TSM2Diag               rt_out_tsm_diag;
    Dt_RECORD_TSM2CANGATE            rt_out_tsm_cangate;
};

// read from yaml and set test data
void 
tsm_read_yaml_set_data(const char* filename, struct tsm_simulink_input* simu_in);

#endif