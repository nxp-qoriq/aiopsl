/**************************************************************************//**
 @file          time.h

 @brief         AIOP Service Layer Time Queries routines

 @details       Contains AIOP SL Time Queries internal routines API declarations.

*//***************************************************************************/

#ifndef __TIME_H
#define __TIME_H

#include "common/types.h"
#include "fsl_errors.h"
#include "fsl_time.h"

/**************************************************************************//**
@Function      _gettime

@Description   routine to receive time in microseconds from CM-GW TS registers
               (1588).


@Return        on success: time as the number of microseconds since the Epoch,
               1970-01-01 00:00:00 +0000 (UTC).
               -1 otherwise.
*//***************************************************************************/
time_t _gettime(void);

#endif /* _TIME_H */
