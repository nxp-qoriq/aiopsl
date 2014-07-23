/**************************************************************************//**
 @file          fsl_time.h

 @details       Contains AIOP SL Time Queries routines API declarations.
 *//***************************************************************************/

#ifndef __FSL_TIME_H
#define __FSL_TIME_H

#include "common/types.h"
#include "fsl_errors.h"


/**************************************************************************//**
 @Group		time_g Time Queries

 @Description   The AIOP Service Layer Time Queries group provides
 		standard time queries functions

 @{
*//***************************************************************************/

typedef uint64_t time_t; /**< type of time used in fsl_os_gettimeofday(),
                              will be removed in future release. */

/* type suseconds_t for microseconds [0,1000000]*/
typedef uint32_t suseconds_t; /**< type of microseconds, will be removed in future release */


typedef struct timeval{
    time_t      tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
}timeval;

typedef struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
}timezone;

/**************************************************************************//**
@Deprecated - Will be removed in future release
@Function   fsl_os_gettimeofday

@Description  Gets time of day returns the time as the number of seconds and
	microseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).

 @Param[out]  tv - if not null, tv struct is filled with seconds since epoch
 @Param[out]  tz - obsolete, should be null (not supported)


@Return   standard POSIX error code

*//***************************************************************************/
int fsl_os_gettimeofday(timeval *tv, timezone *tz);

/**************************************************************************//**
@Function   fsl_get_time_ms

 @Description  Function returns the time as the number of milliseconds
		since midnight (UTC).

 @Param[out]  time - if not null, time is filled with milliseconds since
                     midnight UTC.

 @Return   standard POSIX error code.
 	 	 For error posix refer to
		\ref error_g

*//***************************************************************************/
int fsl_get_time_ms(uint32_t *time);

/**************************************************************************//**
@Function   fsl_get_time_since_epoch_ms

@Description  returns the time as the number of milliseconds since epoch,
		1970-01-01 00:00:00 +0000 (UTC).

 @Param[out]  time - if not null, time is filled with milliseconds since
                     epoch UTC.

 @Return   standard POSIX error code.
 	 	 For error posix refer to
		\ref error_g

*//***************************************************************************/
int fsl_get_time_since_epoch_ms(uint64_t *time);

/**************************************************************************//**
 @Deprecated - Will be removed in future release
 @Function   fsl_os_current_time

 @Description  not implemented

 @Return   0, not implemented

*//***************************************************************************/
uint32_t fsl_os_current_time(void);
/** @} */ /* end of time_g Time Queries group */

#endif /* __FSL_TIME_H */

