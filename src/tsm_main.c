#include <pthread.h>
#include "tsm_worker.h"

sint32 main() 
{
  // 定义线程变量
  pthread_t main_thread;
  // 定义线程名称
  uint8 *message = "main_thread";
  // 定义线程句柄
  sint32 ret_thrd;

  ret_thrd = pthread_create(&main_thread, NULL, &TsmModulerWorker, (void*)message);

  // 线程创建失败
  if (ret_thrd != 0) {
    LOG("main thread create failed!");
    exit(-1);
  } else {
    LOG("main thread create success!");
  }
  // 等待线程终止
  pthread_join(main_thread, NULL);

  return 0;
}