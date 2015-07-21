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

#ifndef __TABLE_RULE_INLINE_H
#define __TABLE_RULE_INLINE_H

#include "general.h"
#include "table.h"
#include "fsl_errors.h"

inline int table_rule_create(enum table_hw_accel_id acc_id,
			     uint16_t table_id,
			     struct table_rule *rule,
			     uint8_t key_size)
{

#ifdef CHECK_ALIGNMENT
	DEBUG_ALIGN("table_inline.h",(uint32_t)rule, ALIGNMENT_16B);
#endif

	int32_t status;
	struct table_old_result aged_res __attribute__((aligned(16)));
	uint32_t arg2 = (uint32_t)&aged_res;
	uint32_t arg3 = table_id;

	/* Set Opaque1, Opaque2 valid bits*/
	*(uint16_t *)(&(rule->result.type)) |=
			TABLE_TLUR_OPAQUE_VALID_BITS_MASK;

	/* Clear byte in offset 2*/
	*((uint8_t *)&(rule->result) + 2) = 0;

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, (uint32_t)key_size, 16, 0, 15);
	/* Not using RPTR DEC because aging is disabled */
	__stqw(TABLE_RULE_CREATE_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Status Handling */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_BIT_MISS) {
		/* A rule with the same match description is not found in the
		 * table. New rule is created. */
		status = TABLE_STATUS_SUCCESS;
	}
	else if (status == TABLE_HW_STATUS_SUCCESS)
		/* A rule with the same match description (and not aged) is
		 * found in the table. */
		status = -EIO;
	/* Redirected to exception handler since aging is removed
	else if (status == TABLE_HW_STATUS_PIEE)
		 * A rule with the same match description (and aged) is found
		 * in the table. The rule is replaced. Output message is
		 * valid if command MTYPE is w/o RPTR counter decrement.*
		status = TABLE_STATUS_SUCCESS;
	*/
	else if (status & TABLE_HW_STATUS_BIT_TIDE) {
		table_rule_inline_exception_handler(TABLE_RULE_CREATE_FUNC_ID,
						    __LINE__,
						    status,
						    TABLE_ENTITY_HW);
	}
	else if (status & TABLE_HW_STATUS_BIT_NORSC){
		status = -ENOMEM;
	}
	else {
		/* Call fatal error handler */
		table_rule_inline_exception_handler(TABLE_RULE_CREATE_FUNC_ID,
						    __LINE__,
						    status,
						    TABLE_ENTITY_HW);
	}
	return status;
}


inline int table_rule_replace(enum table_hw_accel_id acc_id,
		       uint16_t table_id,
		       struct table_rule *rule,
		       uint8_t key_size,
		       struct table_result *old_res)
{
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("table_inline.h",(uint32_t)rule, ALIGNMENT_16B);
#endif

	int32_t status;

	struct table_old_result hw_old_res __attribute__((aligned(16)));
	uint32_t arg2 = (uint32_t)&hw_old_res;
	uint32_t arg3 = table_id;

	/* Set Opaque1, Opaque2 valid bits*/
	*(uint16_t *)(&(rule->result.type)) |=
			TABLE_TLUR_OPAQUE_VALID_BITS_MASK;

	/* Clear byte in offset 2*/
	*((uint8_t *)&(rule->result) + 2) = 0;

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, (uint32_t)key_size, 16, 0, 15);
	__stqw(TABLE_RULE_REPLACE_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Accelerator call */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_SUCCESS) {
		if (old_res)
			/* STQW optimization is not done here so we do not
			 * force alignment */
			*old_res = hw_old_res.result;
	}
	else if (status == TABLE_HW_STATUS_BIT_MISS)
		status = -EIO;
	else
		/* Call fatal error handler */
		table_rule_inline_exception_handler(TABLE_RULE_REPLACE_FUNC_ID,
						    __LINE__,
						    status,
						    TABLE_ENTITY_HW);

	return status;
}


inline int table_rule_delete(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      union table_key_desc *key_desc,
		      uint8_t key_size,
		      struct table_result *result)
{

#ifdef CHECK_ALIGNMENT
	DEBUG_ALIGN("table_inline.h",(uint32_t)key_desc, ALIGNMENT_16B);
#endif

	int32_t status;

	struct table_old_result old_res __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg2 = (uint32_t)&old_res;
	uint32_t arg3 = table_id;
	arg2 = __e_rlwimi(arg2, (uint32_t)key_desc, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, (uint32_t)key_size, 16, 0, 15);
	__stqw(TABLE_RULE_DELETE_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Accelerator call */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_SUCCESS) {
		if (result)
			/* STQW optimization is not done here so we do not
			 * force alignment */
			*result = old_res.result;
	}
	else if (status == TABLE_HW_STATUS_BIT_MISS)
		/* Rule was not found */
		status = -EIO;
	else
		/* Call fatal error handler */
		table_rule_inline_exception_handler(TABLE_RULE_DELETE_FUNC_ID,
						    __LINE__,
						    status,
						    TABLE_ENTITY_HW);

	return status;
}


inline int table_rule_query(enum table_hw_accel_id acc_id,
		     uint16_t table_id,
		     union table_key_desc *key_desc,
		     uint8_t key_size,
		     struct table_result *result,
		     uint32_t *timestamp)
{

#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("table_inline.h",(uint32_t)key_desc, ALIGNMENT_16B);
#endif

	int32_t status;
	struct table_entry entry __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg3 = table_id;
	uint32_t arg2 = (uint32_t)&entry;
	uint8_t entry_type;
	arg3 = __e_rlwimi(arg3, (uint32_t)key_size, 16, 0, 15);
	arg2 = __e_rlwimi(arg2, (uint32_t)key_desc, 16, 0, 15);
	__stqw(TABLE_RULE_QUERY_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* get HW status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);

	if (status == TABLE_HW_STATUS_SUCCESS) {
		/* Copy result and timestamp */
		entry_type = entry.type & TABLE_ENTRY_ENTYPE_FIELD_MASK;
		if (entry_type == TABLE_ENTRY_ENTYPE_EME16) {
			*timestamp = entry.body.eme16.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.eme16.result;
		}
		else if (entry_type == TABLE_ENTRY_ENTYPE_EME24) {
			*timestamp = entry.body.eme24.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.eme24.result;
		}
		else if (entry_type == TABLE_ENTRY_ENTYPE_LPM_RES) {
			*timestamp = entry.body.lpm_res.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.lpm_res.result;
		}
		else if (entry_type == TABLE_ENTRY_ENTYPE_MFLU_RES) {
			*timestamp = entry.body.mflu_result.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.mflu_result.result;
		}
		else
			/* Call fatal error handler */
			table_rule_inline_exception_handler(
					TABLE_RULE_QUERY_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_QUERY_INVAL_ENTYPE,
					TABLE_ENTITY_SW);
	} else {
		/* Status Handling*/
		if (status == TABLE_HW_STATUS_BIT_MISS){}
			/* A rule with the same match description is not found
			 * in the table. */

		/* Redirected to exception handler since aging is removed
		else if (status == CTLU_HW_STATUS_TEMPNOR)
			* A rule with the same match description is found and
			 * rule is aged. *
			status = TABLE_STATUS_MISS;
		*/

		/* Redirected to exception handler since aging is removed - If
		aging is enabled once again, please check that it is indeed
		supported for MFLU, elsewhere it still needs to go to exception
		path.
		else if (status == MFLU_HW_STATUS_TEMPNOR)
			/* A rule with the same match description is found and
			 * rule is aged. *
			status = TABLE_STATUS_MISS;
		*/

		else
			/* Call fatal error handler */
			table_rule_inline_exception_handler(
					TABLE_RULE_QUERY_FUNC_ID,
					__LINE__,
					status,
					TABLE_ENTITY_HW);
	}

	return status;
}


/*****************************************************************************/
/*				Internal API				     */
/*****************************************************************************/

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

inline void table_rule_inline_exception_handler(
				enum table_function_identifier func_id,
				uint32_t line,
				int32_t status,
				enum table_entity entity)
					__attribute__ ((noreturn)) {
	table_exception_handler(__FILE__, func_id, line, status, entity);
}

#pragma pop

#endif /* __TABLE_RULE_INLINE_H */
