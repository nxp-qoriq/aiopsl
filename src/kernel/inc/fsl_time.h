/**************************************************************************//**
 @file          fsl_time.h

 @brief         AIOP Service Layer Time Queries routines

 @details       Contains AIOP SL Time Queries routines API declarations.

 @@internal
 @requirements CR:ENGR00272897
 @implements   See CR's Analysis information
 @warning      POSIX used for the API, however, no full POSIX implementation is guaranteed
*//***************************************************************************/

#ifndef __FSL_TIME_H
#define __FSL_TIME_H

#include "common/types.h"
#include "fsl_errors.h"

/**************************************************************************//**
 @Group       aiopsl_time_queries TIME QUERIES

 @Description   The AIOP Service Layer Time Queries group provides
 		standard time queries functions

 @{
*//***************************************************************************/

typedef uint64_t time_t;

/* type suseconds_t for microseconds [0,1000000]*/
typedef uint32_t suseconds_t;


typedef struct timeval{
    time_t      tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
}timeval;

typedef struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
}timezone;

/**************************************************************************//**
 @Function   fsl_os_gettimeofday

 @Description  Gets time of day

@Param[in]  tv - if not null the tv struct is filled
@Param[in]  tx - if not null the tz struct is filled


 @Return   standard POSIX error code

*//***************************************************************************/
int fsl_os_gettimeofday(timeval *tv, timezone *tz);


/**************************************************************************//**
 @Function   fsl_os_time

 @Decription returns the time as the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).

 @Param[in]  t - If t is non-NULL, the return value is also stored in the memory pointed by t.

 @Return   On success, the value of time in seconds since the Epoch is returned.
           On error, ((time_t) -1) is returned

*//***************************************************************************/
time_t fsl_os_time(time_t *t);

/** @} */ /* end of aiopsl_time_queries group */

#endif /* __FSL_TIME_H */

