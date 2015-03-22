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
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN((uint32_t)lookup_result, ALIGNMENT_16B);
	DEBUG_ALIGN((uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
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
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN((uint32_t)key_desc.em_key, ALIGNMENT_16B);
	DEBUG_ALIGN((uint32_t)lookup_result, ALIGNMENT_16B);
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
	
inline int table_rule_create(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_rule *rule,
		      uint8_t key_size)
{  
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN((uint32_t)rule, ALIGNMENT_16B);
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

	/* TODO - Rev2
	if (rule->result.type == TABLE_RULE_RESULT_TYPE_CHAINING) {
		rule->result.op_rptr_clp.chain_parameters.reserved1 = 0;
		rule->result.op_rptr_clp.chain_parameters.reserved0 =
			TABLE_TLUR_TKIDV_BIT_MASK;
	}
	*/

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, key_size, 16, 0, 15);
	/* Not using RPTR DEC because aging is disabled */
	__stqw(TABLE_RULE_CREATE_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Status Handling */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_MISS):
		/* A rule with the same match description is not found in the
		 * table. New rule is created. */
		status = TABLE_STATUS_SUCCESS;
		break;
	case (TABLE_HW_STATUS_SUCCESS):
		/* A rule with the same match description (and not aged) is
		 * found in the table. */
		status = -EIO;
		break;
	/* Redirected to exception handler since aging is removed
	case (TABLE_HW_STATUS_PIEE):
		 * A rule with the same match description (and aged) is found
		 * in the table. The rule is replaced. Output message is
		 * valid if command MTYPE is w/o RPTR counter decrement.*
		status = TABLE_STATUS_SUCCESS;
		break;	
	*/
	case (CTLU_HW_STATUS_NORSC):
		status = -ENOMEM;
		break;
	case (MFLU_HW_STATUS_NORSC):
		status = -ENOMEM;
		break;
	case (CTLU_HW_STATUS_TEMPNOR):
		/* TODO Rev2 - consider to change it to EAGAIN. it is now
		 * ENOMEM since in Rev1 it may take a very long time until
		 * rules are released. */
		status = -ENOMEM;
		break;
	case (MFLU_HW_STATUS_TEMPNOR):
		/* TODO Rev2 - consider to change it to EAGAIN. it is now
		 * ENOMEM since in Rev1 it may take a very long time until
		 * rules are released. */
		status = -ENOMEM;
		break;
	default:
		/* Call fatal error handler */
		table_exception_handler_wrp(TABLE_RULE_CREATE_FUNC_ID,
					    __LINE__,
					    status);
		break;
	}
	return status;
}

inline int table_rule_delete(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      union table_key_desc *key_desc,
		      uint8_t key_size,
		      struct table_result *result)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN((uint32_t)key_desc, ALIGNMENT_16B);
#endif
	
	int32_t status;

	struct table_old_result old_res __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg2 = (uint32_t)&old_res;
	uint32_t arg3 = table_id;
	arg2 = __e_rlwimi(arg2, (uint32_t)key_desc, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(TABLE_RULE_DELETE_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Accelerator call */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		if (result)
			/* STQW optimization is not done here so we do not
			 * force alignment */
			*result = old_res.result;
		break;
	case (TABLE_HW_STATUS_MISS):
		/* Rule was not found */
		status = -EIO;
		break;
	default:
		/* Call fatal error handler */
		table_exception_handler_wrp(
				TABLE_RULE_DELETE_FUNC_ID,
				__LINE__,
				status);
		break;
	} /* Switch */

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
	DEBUG_ALIGN((uint32_t)key_desc, ALIGNMENT_16B);
#endif
	
	int32_t status;
	struct table_entry entry __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg3 = table_id;
	uint32_t arg2 = (uint32_t)&entry;
	arg3 = __e_rlwimi(arg3, key_size, 16, 0, 15);
	arg2 = __e_rlwimi(arg2, (uint32_t)key_desc, 16, 0, 15);
	__stqw(TABLE_RULE_QUERY_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* get HW status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);

	if (status == TABLE_HW_STATUS_SUCCESS) {
		/* Copy result and timestamp */
		switch (entry.type & TABLE_ENTRY_ENTYPE_FIELD_MASK) {
		case (TABLE_ENTRY_ENTYPE_EME16):
			*timestamp = entry.body.eme16.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.eme16.result;
			break;
		case (TABLE_ENTRY_ENTYPE_EME24):
			*timestamp = entry.body.eme24.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.eme24.result;
			break;
		case (TABLE_ENTRY_ENTYPE_LPM_RES):
			*timestamp = entry.body.lpm_res.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.lpm_res.result;
			break;
		case (TABLE_ENTRY_ENTYPE_MFLU_RES):
			*timestamp = entry.body.mflu_result.timestamp;
			/* STQW optimization is not done here so we do not force
			   alignment */
			*result = entry.body.mflu_result.result;
			break;
		default:
			/* Call fatal error handler */
			table_exception_handler_wrp(
					TABLE_RULE_QUERY_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_QUERY_INVAL_ENTYPE);
			break;
		} /* Switch */
	} else {
		/* Status Handling*/
		switch (status) {
		case (TABLE_HW_STATUS_MISS):
			/* A rule with the same match description is not found
			 * in the table. */
			break;

		/* Redirected to exception handler since aging is removed
		case (CTLU_HW_STATUS_TEMPNOR):
			* A rule with the same match description is found and
			 * rule is aged. *
			status = TABLE_STATUS_MISS;
			break;
		*/

		/* Redirected to exception handler since aging is removed - If
		aging is enabled once again, please check that it is indeed
		supported for MFLU, elsewhere it still needs to go to exception
		path.
		case (MFLU_HW_STATUS_TEMPNOR):
			/* A rule with the same match description is found and
			 * rule is aged. *
			status = TABLE_STATUS_MISS;
			break;
		*/

		default:
			/* Call fatal error handler */
			table_exception_handler_wrp(
					TABLE_RULE_QUERY_FUNC_ID,
					__LINE__,
					status);
			break;
		} /* Switch */
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
	DEBUG_ALIGN((uint32_t)rule, ALIGNMENT_16B);
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

	/* TODO Rev2
	if (rule->result.type == TABLE_RULE_RESULT_TYPE_CHAINING) {
		rule->result.op_rptr_clp.chain_parameters.reserved1 = 0;
		rule->result.op_rptr_clp.chain_parameters.reserved0 =
			CTLU_TLUR_TKIDV_BIT_MASK;
	}
	*/

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(TABLE_RULE_REPLACE_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Accelerator call */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		if (old_res)
			/* STQW optimization is not done here so we do not
			 * force alignment */
			*old_res = hw_old_res.result;
		break;
	case (TABLE_HW_STATUS_MISS):
		status = -EIO;
		break;
	default:
		/* Call fatal error handler */
		table_exception_handler_wrp(TABLE_RULE_REPLACE_FUNC_ID,
					    __LINE__,
					    status);
		break;
	} /* Switch */

	return status;
}

inline int table_create(enum table_hw_accel_id acc_id,
		 struct table_create_params *tbl_params,
		 uint16_t *table_id)
{
	int32_t           crt_status;
	int32_t           miss_status;
	struct table_rule *miss_rule;
	int               num_entries_per_rule;

	/* 16 Byte aligned for stqw optimization + HW requirements */
	struct table_create_input_message  tbl_crt_in_msg
		__attribute__((aligned(16)));

	struct table_create_output_message tbl_crt_out_msg
		__attribute__((aligned(16)));

	struct table_acc_context      *acc_ctx =
		(struct table_acc_context *)HWC_ACC_IN_ADDRESS;

	uint32_t arg2 = (uint32_t)&tbl_crt_out_msg; /* To be used in
						    Accelerator context __stqw
						    */

	/* Load frequent parameters into registers */
	uint8_t                           key_size = tbl_params->key_size;
	uint16_t                          attr = tbl_params->attributes;
	uint32_t                          max_rules = tbl_params->max_rules;
	uint32_t                          committed_rules =
		tbl_params->committed_rules;

	/* Calculate the number of entries each rule occupies */
	num_entries_per_rule = table_calc_num_entries_per_rule(
					attr & TABLE_ATTRIBUTE_TYPE_MASK,
					key_size);

	/* Prepare input message */
	tbl_crt_in_msg.attributes = attr;
	tbl_crt_in_msg.icid = TABLE_CREATE_INPUT_MESSAGE_ICID_BDI_MASK;
	tbl_crt_in_msg.max_rules = max_rules;
	tbl_crt_in_msg.max_entries =
		num_entries_per_rule * max_rules;
	tbl_crt_in_msg.committed_entries =
			num_entries_per_rule * committed_rules;
		/* Optimization
		 * Multiply on e200 is 2 clocks latency, 1 clock throughput */
	tbl_crt_in_msg.committed_rules = committed_rules;
	cdma_ws_memory_init(tbl_crt_in_msg.reserved,
			    TABLE_CREATE_INPUT_MESSAGE_RESERVED_SPACE,
			    0);

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)&tbl_crt_in_msg, 16, 0, 15);
	__stqw(TABLE_CREATE_MTYPE, arg2, 0, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Get status */
	crt_status = *((int32_t *)HWC_ACC_OUT_ADDRESS);

	/* Translate status */
	switch (crt_status) {
	case (TABLE_STATUS_SUCCESS):
		break;
	case (CTLU_HW_STATUS_NORSC):
		crt_status = -ENOMEM;
		break;
	case (MFLU_HW_STATUS_NORSC):
		crt_status = -ENOMEM;
		break;
	case (CTLU_HW_STATUS_TEMPNOR):
		/* TODO Rev2 - consider to change it to EAGAIN. it is
		 * now ENOMEM since in Rev1 it may take a very long
		 * time until rules are released. */
		crt_status = -ENOMEM;
		break;
	case (MFLU_HW_STATUS_TEMPNOR):
		/* TODO Rev2 - consider to change it to EAGAIN. it is
		 * now ENOMEM since in Rev1 it may take a very long
		 * time until rules are released. */
		crt_status = -ENOMEM;
		break;
	default:
		table_exception_handler_wrp(TABLE_CREATE_FUNC_ID,
					    __LINE__,
					    crt_status);
		break;
	}

	if (crt_status >= 0)
	/* Get new Table ID */
		*table_id = (((struct table_create_output_message *)acc_ctx->
			      output_pointer)->tid);

	/* Add miss result to the table if needed and if an error did not occur
	 * during table creation */
	if ((crt_status >= 0) &&
	    ((attr & TABLE_ATTRIBUTE_MR_MASK) == TABLE_ATTRIBUTE_MR_MISS)) {
		/* Re-assignment of the structure is done because of stack
		 * limitations of the service layer - assertion of sizes is
		 * done on table.h */
		miss_rule = (struct table_rule *)&tbl_crt_in_msg;
		miss_rule->options = TABLE_RULE_TIMESTAMP_NONE;

		/* Copy miss result  - Last 16 bytes */
		__stqw(*(((uint32_t *)&tbl_params->miss_result) + 1),
		       *(((uint32_t *)&tbl_params->miss_result) + 2),
		       *(((uint32_t *)&tbl_params->miss_result) + 3),
		       *(((uint32_t *)&tbl_params->miss_result) + 4),
		       0, ((uint32_t *)&(miss_rule->result) + 1));

		/* Copy miss result  - First 4 bytes */
		*((uint32_t *)(&(miss_rule->result))) =
				*((uint32_t *)&tbl_params->miss_result);
		miss_status = table_rule_create(acc_id,
						*table_id,
						miss_rule,
						0);

		if (miss_status)
			table_exception_handler_wrp(
					TABLE_CREATE_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_MISS_RES_CRT_FAIL);
	}
	return crt_status;
}

inline void table_delete(enum table_hw_accel_id acc_id,
		  uint16_t table_id)
{
	int32_t status;

	/* Prepare ACC context for CTLU accelerator call */
	__stqw(TABLE_DELETE_MTYPE, 0, table_id, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Check status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status)
		table_exception_handler_wrp(TABLE_DELETE_FUNC_ID,
					    __LINE__,
					    status);

	return;
}

inline void table_replace_miss_result(enum table_hw_accel_id acc_id,
			       uint16_t table_id,
			       struct table_result *new_miss_result,
			       struct table_result *old_miss_result)
{
	int32_t status;

	/* 16 Byte aligned for stqw optimization + HW requirements */
	struct table_rule new_miss_rule __attribute__((aligned(16)));
	new_miss_rule.options = TABLE_RULE_TIMESTAMP_NONE;

	/* Copy miss result  - Last 16 bytes */
	__stqw(*(((uint32_t *)new_miss_result) + 1),
	       *(((uint32_t *)new_miss_result) + 2),
	       *(((uint32_t *)new_miss_result) + 3),
	       *(((uint32_t *)new_miss_result) + 4),
	       0, ((uint32_t *)&(new_miss_rule.result) + 1));

	/* Copy miss result  - First 4 bytes */
	*((uint32_t *)(&(new_miss_rule.result))) =
			*((uint32_t *)new_miss_result);

	status = table_rule_replace(acc_id, table_id, &new_miss_rule, 0,
				       old_miss_result);
	if (status)
		table_exception_handler_wrp(TABLE_REPLACE_MISS_RESULT_FUNC_ID,
					    __LINE__,
					    TABLE_SW_STATUS_MISS_RES_RPL_FAIL);
	return;
}



#endif /* __TABLE_INLINE_H */
