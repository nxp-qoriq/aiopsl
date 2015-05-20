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
 @file          time.h

 @brief         AIOP Service Layer Time Queries routines

 @details       Contains AIOP SL Time Queries internal routines API declarations.

*//***************************************************************************/

#ifndef __TIME_H
#define __TIME_H

#include "common/types.h"
#include "fsl_errors.h"
#include "fsl_time.h"

#define udiv1000(numerator) ((numerator * 0x83126e98ULL) >> 41);
/** Macro to divide unsigned long numerator by 1000 */

/**************************************************************************//**
@Function      _get_time_tman

@Description   routine to receive time in milliseconds from tman registers
               using time base passed from MC


@Param[in]     time - on success: time is stored in the pointer as the number of
               milliseconds since the Epoch,
               1970-01-01 00:00:00 +0000 (UTC)
@Return          standard POSIX error code.
 	 	 For error posix refer to
		\ref error_g
*//***************************************************************************/
int _get_time_tman(uint64_t *time);

/**************************************************************************//**
@Function      ulldiv1000

@Description   routine to receive time in microseconds from CM-GW TS registers
               (1588).


@Param[in]     numerator - unsigned long long numerator to be divided by
				1000
@Return        quotient - (result of dividing by 1000)
*//***************************************************************************/
static inline uint64_t ulldiv1000(uint64_t numerator) {
	uint32_t msw , lsw, w;
	uint64_t quotient;

	lsw = (uint32_t) (numerator);
	msw = (uint32_t) (numerator >> 32);
	w = udiv1000(msw);
	msw = msw-(1000*w); // remainder

	quotient = w;
	quotient <<= 32;

	msw = (msw << 16) | (lsw >> 16);

	w = udiv1000(msw);
	msw = msw - (1000 * w); // remainder

	quotient |= ((uint64_t) w) << 16;

	msw = (msw << 16) | (lsw & 0xffff);
	quotient |= udiv1000(msw);

	return quotient;
}


#endif /* _TIME_H */
