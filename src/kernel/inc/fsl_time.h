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

 @Description  Gets time of day returns the time as the number of seconds and 
	microseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).

@Param[in]  tv - if not null the tv struct is filled
@Param[in]  tx - obsolete, should be null


 @Return   standard POSIX error code

*//***************************************************************************/
int fsl_os_gettimeofday(timeval *tv, timezone *tz);


/**************************************************************************//**
 @Function   fsl_os_current_time

 @Description  not implemented yet

 @Return   0, not implemented yet

*//***************************************************************************/

uint32_t fsl_os_current_time(void);
/** @} */ /* end of aiopsl_time_queries group */

#endif /* __FSL_TIME_H */

