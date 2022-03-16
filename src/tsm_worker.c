#include "tsm_worker.h"
#include "tsm_chart.h"
#include "tsm_signalhandling.h"

/* TSM module worker */
void TsmModulerWorker()
{
    Dt_RECORD_CANGATE2TSM rt_in_cangate_tsm;
    Dt_RECORD_Diag2TSM rt_in_diag_tsm;
    Dt_RECORD_PLANLITE2TSM rt_in_planlite_tsm;
    Dt_RECORD_TSM2PLANLITE rt_out_tsm_planlite;
    Dt_RECORD_TSM2CtrlArb rt_out_tsm_ctrlarb;
    Dt_RECORD_TSM2DecisionArbitrator rt_out_tsm_deciarb;
    Dt_RECORD_TSM2Diag rt_out_tsm_diag;
    // 外部信号初始化
    memset(&rt_in_cangate_tsm, 0, sizeof(Dt_RECORD_CANGATE2TSM));
    memset(&rt_in_diag_tsm, 0, sizeof(Dt_RECORD_Diag2TSM));
    memset(&rt_in_planlite_tsm, 0, sizeof(Dt_RECORD_PLANLITE2TSM));
    memset(&rt_out_tsm_planlite, 0, sizeof(Dt_RECORD_TSM2PLANLITE));
    memset(&rt_out_tsm_ctrlarb, 0, sizeof(Dt_RECORD_TSM2CtrlArb));
    memset(&rt_out_tsm_deciarb, 0, sizeof(Dt_RECORD_TSM2DecisionArbitrator));
    memset(&rt_out_tsm_diag, 0, sizeof(Dt_RECORD_TSM2Diag));

    rt_in_cangate_tsm.Vehicle_Signal_To_Tsm.VCU_AccDriverOrvd = 0;

    MRM_TSM_MODULE_Init();
    while(1)
    {
        // 对应IFC环境的MRM_TSM_MODULE函数
        MRM_TSM_MODULE(&rt_in_cangate_tsm, &rt_in_diag_tsm, &rt_in_planlite_tsm, &rt_out_tsm_planlite,
            &rt_out_tsm_ctrlarb, &rt_out_tsm_deciarb, &rt_out_tsm_diag);
        usleep(20000);   // 休眠20ms
    }
}