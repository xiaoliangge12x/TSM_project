#ifndef TSM_WORKER_H_
#define TSM_WORKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "tsm_parameter.h"

// --------------------- global declaration ------------------
extern struct timeval g_timeval;

void TsmModulerWorker();

#endif