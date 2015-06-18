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
@File		table.c

@Description	This file contains the AIOP SW Table API implementation.

*//***************************************************************************/

#include "dplib/fsl_cdma.h"
#include "dplib/fsl_table.h"
#include "table_inline.h"
#include "fsl_stdio.h"

void table_get_params(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_get_params_output *tbl_params)
{
	int32_t status;

	struct table_params_query_output_message output
		__attribute__((aligned(16)));

	/* Prepare ACC context for TLU accelerator call */
	__stqw(TABLE_QUERY_MTYPE, (uint32_t)&output, table_id, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	/* Getting output parameters */
	tbl_params->current_rules = output.current_rules;
	tbl_params->committed_rules = output.committed_rules;
	tbl_params->max_rules = output.max_rules;
	tbl_params->attributes = output.attr;

	/* Check status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status)
		table_exception_handler_wrp(TABLE_GET_PARAMS_FUNC_ID,
					    __LINE__,
					    status);

	return;
}


void table_get_miss_result(enum table_hw_accel_id acc_id,
			   uint16_t table_id,
			   struct table_result *miss_result)
{
	int32_t status;
	uint32_t invalid_timestamp;
	status = table_rule_query(acc_id, table_id, 0, 0, miss_result,
				  &invalid_timestamp);

	if (status)
		table_exception_handler_wrp(TABLE_GET_MISS_RESULT_FUNC_ID,
					    __LINE__,
					    TABLE_SW_STATUS_MISS_RES_GET_FAIL);
	return;
}


int table_rule_create_or_replace(enum table_hw_accel_id acc_id,
				 uint16_t table_id,
				 struct table_rule *rule,
				 uint8_t key_size,
#ifdef REV2_RULEID
				 struct table_result *old_res,
				 uint64_t *rule_id)
#else
				 struct table_result *old_res)
#endif
{
	
#ifdef CHECK_ALIGNMENT
	DEBUG_ALIGN("table.c", (uint32_t)rule, ALIGNMENT_16B);
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

	/* TODO - Rev2
	if (rule->result.type == TABLE_RULE_RESULT_TYPE_CHAINING) {
		rule->result.op_rptr_clp.chain_parameters.reserved1 = 0;
		rule->result.op_rptr_clp.chain_parameters.reserved0 =
			TABLE_TLUR_TKIDV_BIT_MASK;
	}
	*/

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, (uint32_t)key_size, 16, 0, 15);
	__stqw(TABLE_RULE_CREATE_OR_REPLACE_MTYPE, arg2, arg3, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Accelerator call*/
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_SUCCESS) {
		/* Replace occurred */
		if (old_res)
			/* STQW optimization is not done here so we do not
			 * force alignment */
			*old_res = hw_old_res.result;
#ifdef REV2_RULEID
			*rule_id = hw_old_res.rule_id;
#endif
	}
	else if (status == TABLE_HW_STATUS_BIT_MISS){}
	else if (status & TABLE_HW_STATUS_BIT_TIDE) {
		table_exception_handler_wrp(
			TABLE_RULE_CREATE_OR_REPLACE_FUNC_ID,
			__LINE__,
			status);
	}
	else if (status & TABLE_HW_STATUS_BIT_NORSC) {
		status = -ENOMEM;
	/* TODO Rev2 - consider to check TEMPNOR for EAGAIN. it is
	 * now ENOMEM since in Rev1 it may take a very long  time until rules
	 * are released. */
	}
	else {
		/* Call fatal error handler */
		table_exception_handler_wrp(
				TABLE_RULE_CREATE_OR_REPLACE_FUNC_ID,
				__LINE__,
				status);
	}
	return status;
}


int table_lookup_by_keyid(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
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
		table_inline_exception_handler(TABLE_LOOKUP_BY_KEYID_FUNC_ID,
					       __LINE__,
					       status);
	}
	else if (status & TABLE_HW_STATUS_BIT_EOFH) {
		status = -EIO;
	}
	else {
		/* Call fatal error handler */
		table_exception_handler_wrp(TABLE_LOOKUP_BY_KEYID_FUNC_ID,
					    __LINE__,
					    status);
	}
	return status;
}


/*****************************************************************************/
/*				Internal API				     */
/*****************************************************************************/
int table_query_debug(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_params_query_output_message *output)
{
	/* Prepare ACC context for TLU accelerator call */
	__stqw(TABLE_QUERY_MTYPE, (uint32_t)output, table_id, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

/* TODO may not work in Rev1 due to HW issue - need to check with HW*/
int table_hw_accel_acquire_lock(enum table_hw_accel_id acc_id)
{
	__stqw(TABLE_ACQUIRE_SEMAPHORE_MTYPE, 0, 0, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


void table_hw_accel_release_lock(enum table_hw_accel_id acc_id)
{
	__stqw(TABLE_RELEASE_SEMAPHORE_MTYPE, 0, 0, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);
}

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

void table_exception_handler_wrp(enum table_function_identifier func_id,
				 uint32_t line,
				 int32_t status)  __attribute__ ((noreturn)) {
	table_exception_handler(__FILE__, func_id, line, status);
}

#pragma stackinfo_ignore on

void table_exception_handler(char *file_path,
			     enum table_function_identifier func_id,
			     uint32_t line,
			     int32_t status_id) __attribute__ ((noreturn)) {
	char *func_name, *status;
	fsl_os_print("File string address: 0x%x\n",(uint32_t)file_path);
	/* Translate function ID to function name string */
	switch(func_id) {
	case TABLE_CREATE_FUNC_ID:
		func_name = "table_create";
		break;
	case TABLE_REPLACE_MISS_RESULT_FUNC_ID:
		func_name = "table_replace_miss_result";
		break;
	case TABLE_GET_PARAMS_FUNC_ID:
		func_name = "table_get_params";
		break;
	case TABLE_GET_MISS_RESULT_FUNC_ID:
		func_name = "table_get_miss_result";
		break;
	case TABLE_DELETE_FUNC_ID:
		func_name = "table_delete";
		break;
	case TABLE_RULE_CREATE_FUNC_ID:
		func_name = "table_rule_create";
		break;
	case TABLE_RULE_CREATE_OR_REPLACE_FUNC_ID:
		func_name = "table_rule_create_or_replace";
		break;
	case TABLE_RULE_REPLACE_FUNC_ID:
		func_name = "table_rule_replace";
		break;
	case TABLE_RULE_QUERY_FUNC_ID:
		func_name = "table_rule_query";
		break;
	case TABLE_RULE_DELETE_FUNC_ID:
		func_name = "table_rule_delete";
		break;
#ifdef REV2_RULEID
	case TABLE_GET_NEXT_RULEID_FUNC_ID:
		func_name = "table_get_next_ruleid";
		break;
	case TABLE_GET_KEY_DESC_FUNC_ID:
		func_name = "table_get_key_desc";
		break;
	case TABLE_RULE_REPLACE_BY_RULEID_FUNC_ID:
		func_name = "table_rule_replace_by_ruleid";
		break;
	case TABLE_RULE_DELETE_BY_RULEID_FUNC_ID:
		func_name = "table_rule_delete_by_ruleid";
		break;
	case TABLE_RULE_QUERY_BY_RULEID_FUNC_ID:
		func_name = "table_rule_query_by_ruleid";
		break;
#endif //REV2_RULEID
	case TABLE_LOOKUP_BY_KEY_FUNC_ID:
		func_name = "table_rule_lookup_by_key";
		break;
	case TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_FUNC_ID:
		func_name = "table_rule_lookup_by_keyid_default_frame";
		break;
	case TABLE_LOOKUP_BY_KEYID_FUNC_ID:
		func_name = "table_rule_lookup_by_keyid";
		break;
	case TABLE_QUERY_DEBUG_FUNC_ID:
		func_name = "table_query_debug";
		break;
	case TABLE_HW_ACCEL_ACQUIRE_LOCK_FUNC_ID:
		func_name = "table_hw_accel_acquire_lock";
		break;
	case TABLE_HW_ACCEL_RELEASE_LOCK_FUNC_ID:
		func_name = "table_hw_accel_release_lock";
		break;
	case TABLE_EXCEPTION_HANDLER_WRP_FUNC_ID:
		func_name = "table_exception_handler_wrp";
		break;
	/* this function should not be recursive and can go to the exception
	 * handler directly */
	/* case TABLE_EXCEPTION_HANDLER_FUNC_ID:
		func_name = "table_exception_handler";
		break; */
	case TABLE_CALC_NUM_ENTRIES_PER_RULE_FUNC_ID:
		func_name = "table_calc_num_entries_per_rule";
		break;
	case TABLE_WORKAROUND_TKT226361_FUNC_ID:
		func_name = "table_workaround_tkt226361";
	default:
		/* create own exception */
		exception_handler(__FILE__,
				  "table_exception_handler",
				  __LINE__,
				  "table_exception_handler got unknown"
				  "function identifier.\n");
	}

	/* Call general exception handler */
	if (status_id & TABLE_HW_STATUS_BIT_TIDE) {
		status = "Invalid CTLU table ID.\n";
	} else if (status_id & TABLE_HW_STATUS_BIT_KSE) {
		status = "Key size error.\n";
	} else if (status_id & TABLE_HW_STATUS_BIT_MNLE) {
		status = "Maximum number of chained lookups reached.\n";
	}
	/*TODO*//*
	switch (status_id) {
	case (TABLE_HW_STATUS_MNLE):
		
		break;
	case(TABLE_SW_STATUS_MISS_RES_CRT_FAIL):
		status = "Table miss rule creation failed.\n";
		break;
	case(TABLE_SW_STATUS_MISS_RES_RPL_FAIL):
		status = "Table replace miss result failed due to non-existence"
			 " of a miss result in the table.\n";
		break;
	case(TABLE_SW_STATUS_MISS_RES_GET_FAIL):
		status = "Table get miss result failed due to non-existence of"
			  " a miss result in the table.\n";
		break;
	case(TABLE_SW_STATUS_QUERY_INVAL_ENTYPE):
		status = "Rule query failed due to unrecognized entry type"
			 " returned from HW.\n";
		break;
	case(TABLE_SW_STATUS_UNKNOWN_TBL_TYPE):
		status = "Unknown table type.\n";
		break;
	case(TABLE_SW_STATUS_TKT226361_ERR):
		status = "PDM TKT226361 Workaround failed.\n";
		break;
	default:
		status = "Unknown or Invalid status.\n";
		break;
	}
	*/
	exception_handler(file_path, func_name, line, status);
}
#pragma pop

int table_calc_num_entries_per_rule(uint16_t type, uint8_t key_size){
	int num_entries_per_rule;

	switch (type) {

	case TABLE_ATTRIBUTE_TYPE_EM:
		if (key_size <= TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE)
			num_entries_per_rule =
			    TABLE_EM_KEYSIZE_1_TO_24_BYTES_ENTRIES_PER_RULE;
		else if (key_size <= (TABLE_ENTRY_EME44_LOOKUP_KEY_SIZE +
				      TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE))
			num_entries_per_rule =
			    TABLE_EM_KEYSIZE_25_TO_60_BYTES_ENTRIES_PER_RULE;
		else if (key_size <= (TABLE_ENTRY_EME44_LOOKUP_KEY_SIZE +
				      TABLE_ENTRY_EME36_LOOKUP_KEY_SIZE +
				      TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE))
			num_entries_per_rule =
			    TABLE_EM_KEYSIZE_61_TO_96_BYTES_ENTRIES_PER_RULE;
		else
			num_entries_per_rule =
			    TABLE_EM_KEYSIZE_97_TO_124_BYTES_ENTRIES_PER_RULE;
		break;

	case TABLE_ATTRIBUTE_TYPE_LPM:
		if (key_size > TABLE_KEY_LPM_IPV4_SIZE) {
			/* IPv6*/
			num_entries_per_rule =
					TABLE_LPM_IPV6_WC_ENTRIES_PER_RULE;
		} else {
			/* IPv4 */
			num_entries_per_rule =
					TABLE_LPM_IPV4_WC_ENTRIES_PER_RULE;
		}
		break;

	case TABLE_ATTRIBUTE_TYPE_MFLU:
		if (key_size > TABLE_MFLU_SMALL_KEY_MAX_SIZE) {
			num_entries_per_rule =
				TABLE_MFLU_BIG_KEY_WC_ENTRIES_PER_RULE;
		} else {
			num_entries_per_rule =
				TABLE_MFLU_SMALL_KEY_WC_ENTRIES_PER_RULE;
		}
		break;

	default:
		table_exception_handler_wrp(
				TABLE_CALC_NUM_ENTRIES_PER_RULE_FUNC_ID,
				__LINE__,
				TABLE_SW_STATUS_UNKNOWN_TBL_TYPE);
		break;
	}

	return num_entries_per_rule;
}


void table_workaround_tkt226361(uint32_t mflu_peb_num_entries,
				uint32_t mflu_dp_ddr_num_entries,
				uint32_t mflu_sys_ddr_num_entries){

	uint16_t                   table_id;
	uint16_t                   table_loc = TABLE_ATTRIBUTE_LOCATION_PEB;
	struct table_create_params tbl_crt_prm;
	struct table_rule          rule1 __attribute__((aligned(16)));
	struct table_rule          rule2 __attribute__((aligned(16)));
	uint32_t                   i;
	uint32_t                   num_of_entries = mflu_peb_num_entries;
#ifdef REV2_RULEID
	uint64_t                   rule_id;
#endif
	/* Iterate for each memory region */
	for(i = 0; i < 3; i++){
		switch (i) {
		/* case 0 is already assigned at function init */
		case 1:
			table_loc = TABLE_ATTRIBUTE_LOCATION_DP_DDR;
			num_of_entries = mflu_dp_ddr_num_entries;
			break;
		case 2:
			table_loc = TABLE_ATTRIBUTE_LOCATION_SYS_DDR;
			num_of_entries = mflu_sys_ddr_num_entries;
			break;
		default:
			break;
		}

		/* At least 3 entries are needed for 2 rules creation, the
		 * number of entries is always a power of 2 */ 
		if (num_of_entries >= 4){

			/* Create table */
			tbl_crt_prm.attributes = TABLE_ATTRIBUTE_TYPE_MFLU |
						 table_loc |
						 TABLE_ATTRIBUTE_MR_NO_MISS;
			tbl_crt_prm.committed_rules = TABLE_TKT226361_RULES_NUM;
			tbl_crt_prm.max_rules = TABLE_TKT226361_RULES_NUM;
			tbl_crt_prm.key_size = TABLE_TKT226361_KEY_SIZE;
			
			if(table_create(TABLE_ACCEL_ID_MFLU,&tbl_crt_prm,
					&table_id)) {
				table_exception_handler_wrp(
					TABLE_WORKAROUND_TKT226361_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_TKT226361_ERR);
			}

			/* Create 2 rules */
			*((uint32_t *)(&rule1.key_desc.mflu.key[0])) =
					0x12345678;
			*((uint32_t *)(&rule1.key_desc.mflu.
				key[TABLE_TKT226361_KEY_SIZE])) =
					0x00000000; // priority
			*((uint32_t *)(&rule1.key_desc.mflu.mask[0])) =
					0xFFFFFFFF;
			rule1.options = TABLE_RULE_TIMESTAMP_NONE;
			rule1.result.type = TABLE_RESULT_TYPE_OPAQUES;
			if (table_rule_create(TABLE_ACCEL_ID_MFLU,
					table_id,
					&rule1,
					TABLE_TKT226361_KEY_SIZE +
#ifdef REV2_RULEID
					TABLE_KEY_MFLU_PRIORITY_FIELD_SIZE,
					&rule_id)){
#else
					TABLE_KEY_MFLU_PRIORITY_FIELD_SIZE)){
#endif
				table_exception_handler_wrp(
					TABLE_WORKAROUND_TKT226361_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_TKT226361_ERR);
			}

			*((uint32_t *)(&rule2.key_desc.mflu.key[0])) =
					0x87654321;
			*((uint32_t *)(&rule2.key_desc.mflu.key[TABLE_TKT226361_KEY_SIZE])) =
					0x00000000; // priority
			*((uint32_t *)(&rule2.key_desc.mflu.mask[0])) =
					0xFFFFFFFF;
			rule2.options = TABLE_RULE_TIMESTAMP_NONE;
			rule2.result.type = TABLE_RESULT_TYPE_OPAQUES;
			if (table_rule_create(TABLE_ACCEL_ID_MFLU,
					table_id,
					&rule2,
					TABLE_TKT226361_KEY_SIZE +
#ifdef REV2_RULEID
					TABLE_KEY_MFLU_PRIORITY_FIELD_SIZE,&rule_id)){
#else
					TABLE_KEY_MFLU_PRIORITY_FIELD_SIZE)){
#endif
				table_exception_handler_wrp(
					TABLE_WORKAROUND_TKT226361_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_TKT226361_ERR);
			}

			/* Delete the table */
			table_delete(TABLE_ACCEL_ID_MFLU, table_id);
		}
	}
}


#ifdef REV2_RULEID
int table_get_next_ruleid(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_rule_id_desc *rule_id_desc,
			  struct table_rule_id_desc *next_rule_id_desc)
{

#ifdef CHECK_ALIGNMENT
	DEBUG_ALIGN("table_inline.h",(uint32_t)rule_id_desc, ALIGNMENT_16B);
	DEBUG_ALIGN("table_inline.h",(uint32_t)next_rule_id_desc, ALIGNMENT_16B);
#endif

	int32_t status;

	uint32_t arg2 = (uint32_t)next_rule_id_desc;
	uint32_t arg3 = table_id;

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule_id_desc, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, 0x20, 16, 0, 15);
	__stqw(TABLE_GET_NEXT_RULEID_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Accelerator call */
	__e_hwaccel(acc_id);
	
	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_SUCCESS) {}
	else if (status == TABLE_HW_STATUS_MISS) {}
	else
		/* Call fatal error handler */
		table_exception_handler_wrp(TABLE_GET_NEXT_RULEID_FUNC_ID,
					    __LINE__,
					    status);

	return status;
}

int table_get_key_desc(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_rule_id_desc *rule_id_desc,
			  union table_key_desc *key_desc)
{
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("table_inline.h",(uint32_t)rule_id_desc, ALIGNMENT_16B);
	DEBUG_ALIGN("table_inline.h",(uint32_t)key_desc, ALIGNMENT_16B);
#endif
	int32_t status;

	uint32_t arg2 = (uint32_t)key_desc;
	uint32_t arg3 = table_id;

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule_id_desc, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, 0x20, 16, 0, 15);
	__stqw(TABLE_GET_KEY_DESC_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Accelerator call */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == TABLE_HW_STATUS_SUCCESS) {}
	else if (status == TABLE_HW_STATUS_MISS)
		status = -EIO;
	else
		/* Call fatal error handler */
		table_exception_handler_wrp(TABLE_GET_KEY_DESC_FUNC_ID,
					    __LINE__,
					    status);

	return status;

}


int table_rule_replace_by_ruleid(enum table_hw_accel_id acc_id,
		       uint16_t table_id,
		       struct table_ruleid_and_result_desc *rule,
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

	/* TODO Rev2
	if (rule->result.type == TABLE_RULE_RESULT_TYPE_CHAINING) {
		rule->result.op_rptr_clp.chain_parameters.reserved1 = 0;
		rule->result.op_rptr_clp.chain_parameters.reserved0 =
			CTLU_TLUR_TKIDV_BIT_MASK;
	}
	*/

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, 0x40, 16, 0, 15);
	__stqw(TABLE_RULE_REPLACE_BY_RULEID_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

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
	else if (status == TABLE_HW_STATUS_MISS)
		status = -EIO;
	else
		/* Call fatal error handler */
		table_exception_handler_wrp(TABLE_RULE_REPLACE_BY_RULEID_FUNC_ID,
					    __LINE__,
					    status);

	return status;
}

int table_rule_delete_by_ruleid(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_rule_id_desc *rule_id_desc,
		      struct table_result *result)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("table_inline.h",(uint32_t)rule_id_desc, ALIGNMENT_16B);
#endif

	int32_t status;

	struct table_old_result old_res __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg2 = (uint32_t)&old_res;
	uint32_t arg3 = table_id;
	arg2 = __e_rlwimi(arg2, (uint32_t)rule_id_desc, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, 0x20, 16, 0, 15);
	__stqw(TABLE_RULE_DELETE_BY_RULEID_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

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
	else if (status == TABLE_HW_STATUS_MISS)
		/* Rule was not found */
		status = -EIO;
	else
		/* Call fatal error handler */
		table_exception_handler_wrp(
				TABLE_RULE_DELETE_BY_RULEID_FUNC_ID,
				__LINE__,
				status);

	return status;
}

int table_rule_query_by_ruleid(enum table_hw_accel_id acc_id,
		     uint16_t table_id,
		     struct table_rule_id_desc *rule_id_desc,
		     struct table_result *result,
		     uint32_t *timestamp)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("table_inline.h",(uint32_t)rule_id_desc, ALIGNMENT_16B);
#endif

	int32_t status;
	struct table_entry entry __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg3 = table_id;
	uint32_t arg2 = (uint32_t)&entry;
	uint8_t entry_type;
	arg3 = __e_rlwimi(arg3, 0x20, 16, 0, 15);
	arg2 = __e_rlwimi(arg2, (uint32_t)rule_id_desc, 16, 0, 15);
	__stqw(TABLE_RULE_QUERY_BY_RULEID_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

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
			table_exception_handler_wrp(
					TABLE_RULE_QUERY_BY_RULEID_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_QUERY_INVAL_ENTYPE);
	} else {
		/* Status Handling*/
		if (status == TABLE_HW_STATUS_MISS){}
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
			table_exception_handler_wrp(
					TABLE_RULE_QUERY_BY_RULEID_FUNC_ID,
					__LINE__,
					status);
	}

	return status;
}


#endif //REV2_RULEID

int table_lookup_by_keyid_default_frame_wrp(enum table_hw_accel_id acc_id,
					uint16_t table_id,
					uint8_t keyid,
					struct table_lookup_result
					       *lookup_result)
{
	return table_lookup_by_keyid_default_frame(acc_id,
						   table_id,
						   keyid,
						   lookup_result);
}

int table_rule_create_wrp(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_rule *rule,
#ifdef REV2_RULEID
			  uint8_t key_size,
			  uint64_t *rule_id)
#else
			  uint8_t key_size)
#endif
{
#ifdef REV2_RULEID
	return table_rule_create(acc_id, table_id, rule, key_size, rule_id);
#else
	return table_rule_create(acc_id, table_id, rule, key_size);
#endif
}

int table_rule_delete_wrp(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      union table_key_desc *key_desc,
		      uint8_t key_size,
		      struct table_result *result)
{
	return table_rule_delete(acc_id, table_id, key_desc, key_size, result);
}
