#ifndef COMMON_H_
#define COMMON_H_

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

// -------------------- macro ---------------------------------------
#define S_TEN_NS_CONV_RATE  ((int64_t)100000000)
#define US_TEN_NS_CONV_RATE ((int64_t)100)
#define COLOR_NONE          "\033[0m"
#define COLOR_RED           "\033[31m"
#define COLOR_GREEN         "\033[32m"
#define COLOR_YELLOW        "\033[33m"

#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0])) 
#define LOG(str_color, str, ...) { \
    time_t t_time; \
    struct tm *tm_time; \
    struct timeval timeval_time; \
    t_time = time(NULL); \
    tm_time = localtime(&t_time); \ 
    gettimeofday(&timeval_time, NULL); \
    int msTime = timeval_time.tv_usec/1000; \
    printf("[%02d-%02d-%02d-%03d]: " str_color str "\n" COLOR_NONE, \
        tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec, msTime, ##__VA_ARGS__); \
} \

extern struct timeval g_init_time;
extern void hb_TimeSync_GetTime(int64_t* value);

#endif