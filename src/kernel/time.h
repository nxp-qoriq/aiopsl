/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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

/**************************************************************************//**
@Function      _gettime

@Description   routine to receive time in microseconds from CM-GW TS registers
               (1588).


@Param[in]     time - on success: time is stored in the pointer as the number of
               microseconds since the Epoch,
               1970-01-01 00:00:00 +0000 (UTC)
@Return          standard POSIX error code.
 	 	 For error posix refer to
		\ref error_g
*//***************************************************************************/
int _gettime(uint64_t *time);


__HOT_CODE static inline uint64_t divu3(uint64_t n) {
	uint64_t q, r;
	q = (n >> 2) + (n >> 4); // q = n*0.0101 (approx).
	q = q + (q >> 4); // q = n*0.01010101.
	q = q + (q >> 8);
	q = q + (q >> 16);
	r = n - q*3; // 0 <= r <= 15.
	return q + (11*r >> 5); // Returning q + r/3.

}

__HOT_CODE static inline uint64_t divu5(uint64_t n) {
	uint64_t q, r;
	q = (n >> 3) + (n >> 4);
	q = q + (q >> 4);
	q = q + (q >> 8);
	q = q + (q >> 16);
	r = n - q*5;
	return q + (13*r >> 6);
}

__HOT_CODE static inline uint64_t divu9(uint64_t n) {
	uint64_t q, r;
	q = n - (n >> 3);
	q = q + (q >> 6);
	q = q + (q>>12) + (q>>24);
	q = q >> 3;
	r = n - q*9;
	return q + ((r + 7) >> 4);
}
__HOT_CODE static inline uint64_t divu1000(uint64_t n) {
	uint64_t q, r, t;
	t = (n >> 7) + (n >> 8) + (n >> 12);
	q = (n >> 1) + t + (n >> 15) + (t >> 11) + (t >> 14);
	q = q >> 9;
	r = n - q*1000;
	return q + ((r + 24) >> 10);
}


#endif /* _TIME_H */
