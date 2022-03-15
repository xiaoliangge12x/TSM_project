#ifndef TSM_WORKER_H_
#define TSM_WORKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "data_type.h"
#include "tsm_data.h"

void TsmModulerWorker();
void MrmTsmModule(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm,
    Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb,
    Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag);

#endif