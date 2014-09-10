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
@File		cdma_inline.h

@Description	This file contains the AIOP SW CDMA Inline API implementation.

*//***************************************************************************/

#ifndef __CDMA_INLINE_H
#define __CDMA_INLINE_H


#include "cdma.h"
#include "dplib/fsl_cdma.h"
#include "general.h"
#include "common/types.h"


inline void cdma_refcount_increment(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_REFCNT_INC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) != CDMA_SUCCESS)
		cdma_exception_handler(CDMA_REFCOUNT_INCREMENT, __LINE__,
				(int32_t)res1);
}

inline int cdma_refcount_decrement(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_REFCNT_DEC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));


	if (((int32_t)res1) == CDMA_SUCCESS)
		return 0;
	if (((int32_t)res1) == CDMA_REFCOUNT_DECREMENT_TO_ZERO)
		return (int32_t)(res1);
	cdma_exception_handler(CDMA_REFCOUNT_DECREMENT, __LINE__,
			(int32_t)res1);
	return -1;
}

#endif /* __CDMA_INLINE_H */
