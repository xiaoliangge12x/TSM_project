#include <pthread.h>
#include "tsm_worker.h"
#ifdef _NEED_LOG
#include "common.h"
#endif

sint32 main() 
{
  pthread_t main_thread;
  uint8*    message = "main_thread";
  sint32    ret_thrd;

  ret_thrd = pthread_create(&main_thread, NULL, &TsmModulerWorker, (void*)message);

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