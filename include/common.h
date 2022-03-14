#ifndef COMMON_H_
#define COMMON_H_

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define LOG(str, ...) { \
    time_t t_time; \
    struct tm *tm_time; \
    struct timeval timeval_time; \
    t_time = time(NULL); \
    tm_time = localtime(&t_time); \ 
    gettimeofday(&timeval_time, NULL); \
    int msTime = timeval_time.tv_usec/1000; \
    printf("[%02d-%02d-%02d-%03d]: " str "\n", \
    tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec, msTime, ##__VA_ARGS__); \
} \

#endif