#include "tsm_worker.h"
#include "tsm_chart.h"
#include "tsm_signalhandling.h"

/* TSM module worker */
void TsmModulerWorker()
{
    memset(&tsm, 0, sizeof(tsm));

    // 外部信号初始化
    memset(&rt_in_cangate_tsm, 0, sizeof(Dt_RECORD_CANGATE2TSM));
    memset(&rt_in_diag_tsm, 0, sizeof(Dt_RECORD_Diag2TSM));
    memset(&rt_in_planlite_tsm, 0, sizeof(Dt_RECORD_PLANLITE2TSM));
    memset(&rt_out_tsm_planlite, 0, sizeof(Dt_RECORD_TSM2PLANLITE));
    memset(&rt_out_tsm_ctrlarb, 0, sizeof(Dt_RECORD_TSM2CtrlArb));
    memset(&rt_out_tsm_deciarb, 0, sizeof(Dt_RECORD_TSM2DecisionArbitrator));
    memset(&rt_out_tsm_diag, 0, sizeof(Dt_RECORD_TSM2Diag));

    // 计时器初始化
    brakeset_cnt = 0;

    while(1)
    {
        // 对应IFC环境的MRM_TSM_MODULE函数
        MrmTsmModule(&rt_in_cangate_tsm, &rt_in_diag_tsm, &rt_in_planlite_tsm, &rt_out_tsm_planlite,
            &rt_out_tsm_ctrlarb, &rt_out_tsm_deciarb, &rt_out_tsm_diag);
        usleep(20000);   // 休眠20ms
    }
}

void MrmTsmModule(const Dt_RECORD_CANGATE2TSM *rtu_DeCANGATE2TSM, 
    const Dt_RECORD_Diag2TSM *rtu_DeDiag2TSM, 
    const Dt_RECORD_PLANLITE2TSM *rtu_DePlanlite2Tsm,
    Dt_RECORD_TSM2PLANLITE *rty_DeTsm2Planlite,
    Dt_RECORD_TSM2CtrlArb *rty_DeTSM2CtrlArb,
    Dt_RECORD_TSM2DecisionArbitrator *rty_DeTSM2DecisionArbitrator, 
    Dt_RECORD_TSM2Diag *rty_DeTSM2Diag)
{
    SignalHandling(rtu_DeCANGATE2TSM, rtu_DeDiag2TSM, rtu_DePlanlite2Tsm);  // 信号处理接口
    TsmChartManager();   // 运行状态机接口
}