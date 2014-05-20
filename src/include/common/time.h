/**************************************************************************//**
 @file          time.h

 @brief         AIOP Service Layer Time Queries routines

 @details       Contains AIOP SL Time Queries routines API declarations.

 @@internal
 @requirements CR:ENGR00272897
 @implements   See CR's Analysis information
 @warning      POSIX used for the API, however, no full POSIX implementation is guaranteed
*//***************************************************************************/

#ifndef _TIME_H
#define _TIME_H

#include "common/types.h"
#include "common/errors.h"

/**************************************************************************//**
 @ingroup         fsl_lib_g   Utility Library

 @{
 *//***************************************************************************/
/**************************************************************************//**
 @Group       aiopsl_time_queries TIME QUERIES

 @Description   The AIOP Service Layer Time Queries group provides
 		standard time queries functions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @brief      time

 @details   returns the time as the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).

 @param    t - If t is non-NULL, the return value is also stored in the memory pointed to by t.

 @returns  On success, the value of time in seconds since the Epoch is returned. On error, ((time_t) -1) is returned
 @retval   -1 error
 @retval   time in seconds on sucess

 @internal
 @requirements CR:ENGR00272897
 @implements   See CR's Analysis information
 @warning      POSIX used for the API, however, no full POSIX implementation is guaranteed

*//***************************************************************************/
#define TSCRU_OFF		0x30
#define TSCRL_OFF		0x34


typedef uint64_t time_t;

/* type suseconds_t shall be a signed integer type capable of storing values at least
 * in the range [-1, 1000000]. */
typedef uint32_t suseconds_t;

time_t time(
    time_t *t
);

typedef struct timeval{
    time_t      tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
}timeval;

typedef struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
}timezone;
/**************************************************************************//**
 @brief   gettimeofday

 @details  Gets time of day

 @param     tv - if not null the tv struct is filled
 @param     tx - if not null the tz structure is filled

 @returns  error or sucess value as defined below
 @retval   0 sucess
 @retval   -1 failure

 @internal
 @requirements CR:ENGR00272890
 @implements   See CR's Analysis information
 @warning      POSIX used for the API, however, no full POSIX implementation is guaranteed

*//***************************************************************************/
int fsl_os_gettimeofday(timeval *tv, timezone *tz);

/** @} */ /* end of aiopsl_time_queries group */
/** @} *//* end of fsl_lib_g group */


#endif /* _TIME_H */

