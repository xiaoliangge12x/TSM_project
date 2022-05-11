#include <pthread.h>
#include <sys/wait.h>

#include "open_test.h"
#include "tsm_worker.h"
#ifdef _NEED_LOG
#include "common.h"
#endif

static void
tsm_module_work() {
#ifdef _OPEN_TEST
    char filename[] = "../../config/SImulinkDataInput.yaml";
#endif
    static struct tsm_simulink_input simu_in;
    static struct tsm_simulink_output simu_out;

    memset(&simu_in, 0, sizeof(struct tsm_simulink_input));
    memset(&simu_in, 0, sizeof(struct tsm_simulink_output));

    gettimeofday(&g_init_time, NULL);

    MRM_TSM_MODULE_Init();

    while(1)
    {
#ifdef _OPEN_TEST
        tsm_read_yaml_set_data(filename, &simu_in);
#endif
        MRM_Swc_V_TSM(&simu_in.rt_in_ctrlarb_tsm, &simu_in.rt_in_deciarb_tsm, 
                      &simu_in.rt_in_cangate_tsm, &simu_in.rt_in_diag_tsm, 
                      &simu_in.rt_in_planlite_tsm, &simu_out.rt_out_tsm_planlite, 
                      &simu_out.rt_out_tsm_ctrlarb, &simu_out.rt_out_tsm_deciarb, 
                      &simu_out.rt_out_tsm_diag, &simu_out.rt_out_tsm_hmi, 
                      &simu_out.rt_out_tsm_cangate);
        usleep(20000);
    }
}

sint32 
main(int argc, char *argv[]) {
    pthread_t main_thread;

    uint8* message = "main_thread";

    sint32 ret_thrd = pthread_create(&main_thread, NULL, &tsm_module_work, (void*)message);

    if (ret_thrd != 0) {
#ifdef _NEED_LOG
      LOG(COLOR_NONE, "main thread create failed!");
#endif
      exit(-1);
    } else {
#ifdef _NEED_LOG
      LOG(COLOR_NONE, "main thread create success!");
#endif
    }

    pthread_join(main_thread, NULL);

    return 0;
}