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
@File		table_inline.h

@Description	This file contains the AIOP SW Table API Inline implementation.

*//***************************************************************************/

#ifndef __TABLE_INLINE_H
#define __TABLE_INLINE_H

#include "general.h"
#include "fsl_table.h"
#include "table.h"


inline int table_lookup_by_keyid_default_frame(enum table_hw_accel_id acc_id,
					uint16_t table_id,
					uint8_t keyid,
					struct table_lookup_result
					       *lookup_result)
{
	int32_t status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(TABLE_LOOKUP_KEYID_EPRS_TMSTMP_RPTR_MTYPE,
	       (uint32_t)lookup_result, table_id | (((uint32_t)keyid) << 16),
	       0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		break;
	case (TABLE_HW_STATUS_MISS):
		break;
	case (TABLE_HW_STATUS_EOFH):
		status = -EIO;
		break;
	/*TODO EOFH with LOOKUP hit/miss */
	case (TABLE_HW_STATUS_EOFH | TABLE_HW_STATUS_MISS):
		status = -EIO;
		break;
	default:
		/* Call fatal error handler */
		table_exception_handler_wrp(
			TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_FUNC_ID,
			__LINE__,
			status);
		break;
	} /* Switch */

	return status;
}

inline int table_lookup_by_key(enum table_hw_accel_id acc_id,
			uint16_t table_id,
			union table_lookup_key_desc key_desc,
			uint8_t key_size,
			struct table_lookup_result *lookup_result)
{
	int32_t status;
	/* optimization 1 clock */
	uint32_t arg2 = (uint32_t)lookup_result;
	arg2 = __e_rlwimi(arg2, *((uint32_t *)(&key_desc)), 16, 0, 15);

	/* Prepare HW context for TLU accelerator call */
	__stqw(TABLE_LOOKUP_KEY_TMSTMP_RPTR_MTYPE, arg2,
	       table_id | (((uint32_t)key_size) << 16), 0, HWC_ACC_IN_ADDRESS,
	       0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		break;
	case (TABLE_HW_STATUS_MISS):
		break;
	default:
		table_exception_handler_wrp(
				TABLE_LOOKUP_BY_KEY_FUNC_ID,
				__LINE__,
				status);
		break;
	} /* Switch */
	return status;
}


#endif /* __TABLE_INLINE_H */
