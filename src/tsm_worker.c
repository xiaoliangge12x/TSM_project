#include "tsm_worker.h"


/* TSM module worker */
void TsmModulerWorker()
{
    memset(&tsm, 0, sizeof(tsm));
    while(1)
    {
        SignalHandling();
        TsmChartManager();   // 运行状态机
        usleep(20000);   // 休眠20ms
    }
}


void ActionInPassive()
{
    LOG("It's in Passive St.");
}

void ActionInFailureLighting()
{
    LOG("It's in Failure Lighting St.");
}

void ActionInFailureNoLighting()
{
    LOG("It's in Failure No Lighting St.");
}

void ActionInStandby()
{
    LOG("It's in Standby St.");
}

void ActionInMrmBothCtrl()
{
    LOG("It's in Mrm Both Ctrl St.");
}

void ActionInMrmLatCtrl()
{
    LOG("It's in Mrm Lat Ctrl St.");
}

void ActionInMrc()
{
    LOG("It's in Mrc St.");
}
