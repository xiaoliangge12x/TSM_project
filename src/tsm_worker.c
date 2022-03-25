#include "tsm_worker.h"
#include "tsm_chart.h"
#include "tsm_signalhandling.h"

/* TSM module worker */
void TsmModulerWorker()
{
#ifdef _OPEN_TEST    
    char* filename = "../../config/SImulinkDataInput.yaml";
#endif

    SimulinkData simulink_data;
    Dt_RECORD_TSM2PLANLITE rt_out_tsm_planlite;
    Dt_RECORD_TSM2CtrlArb rt_out_tsm_ctrlarb;
    Dt_RECORD_TSM2DecisionArbitrator rt_out_tsm_deciarb;
    Dt_RECORD_TSM2Diag rt_out_tsm_diag;
    Dt_RECORD_TSM2HMI rt_out_tsm_hmi;
    Dt_RECORD_TSM2CANGATE rt_out_tsm_cangate;
    // 外部信号初始化
    memset(&simulink_data, 0, sizeof(SimulinkData));
    memset(&rt_out_tsm_planlite, 0, sizeof(Dt_RECORD_TSM2PLANLITE));
    memset(&rt_out_tsm_ctrlarb, 0, sizeof(Dt_RECORD_TSM2CtrlArb));
    memset(&rt_out_tsm_deciarb, 0, sizeof(Dt_RECORD_TSM2DecisionArbitrator));
    memset(&rt_out_tsm_diag, 0, sizeof(Dt_RECORD_TSM2Diag));
    memset(&rt_out_tsm_hmi, 0, sizeof(Dt_RECORD_TSM2HMI));
    memset(&rt_out_tsm_cangate, 0, sizeof(Dt_RECORD_TSM2CANGATE));

    MRM_TSM_MODULE_Init();

    while(1)
    {
#ifdef _OPEN_TEST
        ReadFromYamlAndSetData(filename, &simulink_data);
#endif
        // 对应MCU环境的MRM_Swc_V_TSM函数
        MRM_Swc_V_TSM(&simulink_data.rt_in_ctrlarb_tsm, &simulink_data.rt_in_deciarb_tsm, &simulink_data.rt_in_cangate_tsm, 
            &simulink_data.rt_in_diag_tsm, &simulink_data.rt_in_planlite_tsm, &rt_out_tsm_planlite, &rt_out_tsm_ctrlarb, 
            &rt_out_tsm_deciarb, &rt_out_tsm_diag, &rt_out_tsm_hmi, &rt_out_tsm_cangate);
        usleep(20000);   // 休眠20ms
    }
}