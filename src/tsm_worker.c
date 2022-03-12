#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "common.h"

/* TSM module worker */
void TsmModulerWorker()
{
    while(1)
    {
        LOG("Say Hello, %d !", 5);
        usleep(20000);   // 休眠20ms
    }
}