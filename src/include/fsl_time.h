/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

#ifdef UNDER_CONSTRUCTION
/**************************************************************************//**
 @Deprecated - Will be removed in future release
 @Function   fsl_os_current_time

 @Description  not implemented

 @Return   0, not implemented

*//***************************************************************************/
#endif //UNDER_CONSTRUCTION
uint32_t fsl_os_current_time(void);
/** @} */ /* end of time_g Time Queries group */

#endif /* __FSL_TIME_H */

