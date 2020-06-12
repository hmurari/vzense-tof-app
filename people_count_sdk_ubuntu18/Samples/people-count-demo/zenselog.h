#ifndef ZENSELOG_H
#define ZENSELOG_H

#include <time.h>
#ifdef _WIN32
#else
#include <sys/timeb.h>
#endif

#define LOG_CRITICAL	0	/* Critical */
#define LOG_ERROR   	1	/* Error */
#define LOG_WARNING	    2	/* Warning */
#define LOG_INFO	    3	/* Info */
#define LOG_DEBUG	    4	/* Debug */

extern int LOG_LEVEL;

#ifdef _WIN32
#define ZENSE_LOG(level, format, ...)  do { \
    if((level) <= LOG_LEVEL) { \
        time_t t = time(0);\
        struct  tm      ptm;\
        struct  timeb   stTimeb;\
        ftime(&stTimeb);\
        localtime_s(&ptm, &t);\
        fprintf(stderr, ("<%04d-%02d-%02d %02d:%02d:%02d:%03d> [%s:%05d] " format),\
            ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday,\
            ptm.tm_hour, ptm.tm_min, ptm.tm_sec, stTimeb.millitm,\
            __func__, __LINE__, ##__VA_ARGS__);\
    } \
} while(0)
#else
#define ZENSE_LOG(level, format, ...)  do { \
    if((level) <= LOG_LEVEL) { \
        time_t t = time(0);\
        struct  tm      ptm;\
        struct  timeb   stTimeb;\
        ftime(&stTimeb);\
        localtime_r(&t, &ptm);\
        fprintf(stderr, ("<%04d-%02d-%02d %02d:%02d:%02d:%03d> [%s:%05d] " format),\
            ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday,\
            ptm.tm_hour, ptm.tm_min, ptm.tm_sec, stTimeb.millitm,\
            __func__, __LINE__, ##__VA_ARGS__);\
    } \
} while(0)
#endif

#endif // ZENSELOG_H
