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

#ifndef __TABLE_LOOKUP_INLINE_H
#define __TABLE_LOOKUP_INLINE_H

#include "table.h"
#include "fsl_errors.h"


inline int table_lookup_by_keyid_default_frame(enum table_hw_accel_id acc_id,
					       t_tbl_id table_id,
					       uint8_t keyid,
					       struct table_lookup_result
							*lookup_result)
{

#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("table_inline.h",(uint32_t)lookup_result, ALIGNMENT_16B);
	DEBUG_ALIGN("table_inline.h",(uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif

	int32_t status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(TABLE_LOOKUP_KEYID_EPRS_TMSTMP_RPTR_MTYPE,
	       (uint32_t)lookup_result, table_id | (((uint32_t)keyid) << 16),
	       0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_SUCCESS){}
	else if (status == TABLE_HW_STATUS_BIT_MISS){}
	else if (status &
		 (TABLE_HW_STATUS_BIT_TIDE |
		  TABLE_HW_STATUS_BIT_NORSC |
		  TABLE_HW_STATUS_BIT_KSE))
	{
		table_lookup_inline_exception_handler(
			TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_FUNC_ID,
			__LINE__,
			status,
			TABLE_ENTITY_HW);
	}
	else if (status & TABLE_HW_STATUS_BIT_EOFH)
		status = -EIO;
	else
		/* Call fatal error handler */
		table_lookup_inline_exception_handler(
			TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_FUNC_ID,
			__LINE__,
			status,
			TABLE_ENTITY_HW);

	return status;
}


inline int table_lookup_by_keyid(enum table_hw_accel_id acc_id,
				 t_tbl_id table_id,
				 uint8_t keyid,
				 uint32_t flags,
				 struct table_lookup_non_default_params
					*ndf_params,
				 struct table_lookup_result *lookup_result)
{
	
#ifdef CHECK_ALIGNMENT
	DEBUG_ALIGN("table.c", (uint32_t)ndf_params, ALIGNMENT_16B);
	DEBUG_ALIGN("table.c", (uint32_t)lookup_result, ALIGNMENT_16B);
	DEBUG_ALIGN("table.c", (uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	int32_t status;

	/* optimization 1 clock */
	uint32_t arg2 = (uint32_t)lookup_result;
	arg2 = __e_rlwimi(arg2, ((uint32_t)ndf_params), 16, 0, 15);

	/* Clearing reserved fields */
	ndf_params->reserved0 = 0;
	ndf_params->reserved1 = 0;
	ndf_params->reserved2 = 0;

	/* Prepare HW context for TLU accelerator call */
	__stqw(flags | TABLE_LOOKUP_KEYID_EPRS_TMSTMP_RPTR_MTYPE, arg2,
	       table_id | (((uint32_t)keyid) << 16), 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_SUCCESS){}
	else if (status == TABLE_HW_STATUS_BIT_MISS){}
	else if (status &
		 (TABLE_HW_STATUS_BIT_TIDE |
		  TABLE_HW_STATUS_BIT_NORSC |
		  TABLE_HW_STATUS_BIT_KSE))
	{
		table_lookup_inline_exception_handler(
					  TABLE_LOOKUP_BY_KEYID_FUNC_ID,
					  __LINE__,
					  status,
					  TABLE_ENTITY_HW);
	}
	else if (status & TABLE_HW_STATUS_BIT_EOFH) {
		status = -EIO;
	}
	else {
		/* Call fatal error handler */
		table_lookup_inline_exception_handler(
					  TABLE_LOOKUP_BY_KEYID_FUNC_ID,
					  __LINE__,
					  status,
					  TABLE_ENTITY_HW);
	}
	return status;
}


inline int table_lookup_by_key(enum table_hw_accel_id acc_id,
			       t_tbl_id table_id,
			       union table_lookup_key_desc key_desc,
			       uint8_t key_size,
			       struct table_lookup_result *lookup_result)
{

#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("table_inline.h",(uint32_t)key_desc.em_key, ALIGNMENT_16B);
	DEBUG_ALIGN("table_inline.h",(uint32_t)lookup_result, ALIGNMENT_16B);
#endif

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
	if (status == TABLE_HW_STATUS_SUCCESS){}
	else if (status == TABLE_HW_STATUS_BIT_MISS){}
	else
		table_lookup_inline_exception_handler(
				TABLE_LOOKUP_BY_KEY_FUNC_ID,
				__LINE__,
				status,
				TABLE_ENTITY_HW);
	return status;
}


/*****************************************************************************/
/*				Internal API				     */
/*****************************************************************************/

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

inline void table_lookup_inline_exception_handler(
				 enum table_function_identifier func_id,
				 uint32_t line,
				 int32_t status,
				 enum table_entity entity)
					__attribute__ ((noreturn)) {
	table_exception_handler(__FILE__, func_id, line, status, entity);
}

#pragma pop

#endif /* __TABLE_LOOKUP_INLINE_H */
