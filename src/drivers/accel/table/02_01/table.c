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

#include "fsl_cdma.h"
#include "fsl_table.h"


int table_create(enum table_hw_accel_id acc_id,
		 struct table_create_params *tbl_params,
		 t_tbl_id *table_id)
{
	int32_t           crt_status;
	int32_t           miss_status;
	struct table_rule *miss_rule;
	int               num_entries_per_rule;

	/* Table create input message AGT field */
	register uint16_t          agt;
	register uint32_t          timestamp_accuracy;

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
	t_rule_id rule_id;

	/* Calculate the number of entries each rule occupies */
	num_entries_per_rule = table_calc_num_entries_per_rule(
					attr & TABLE_ATTRIBUTE_TYPE_MASK,
					key_size);

	/*************************/
	/* Prepare input message */
	/*************************/
	/* Count leading zeroes */
	timestamp_accuracy = tbl_params->timestamp_accuracy;
	asm { cntlzw agt, timestamp_accuracy }

	/* Enable timestamp accuracy if needed*/
	if(agt < 30) {
		/* Calculate the correct power of 2 */
		agt = 31 - agt;
		tbl_crt_in_msg.attributes =
			(TABLE_CREATE_INPUT_MESSAGE_ATTE_AGTM_FLAG |
			 agt |
			 attr);
	}
	else
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
	if (crt_status == TABLE_STATUS_SUCCESS){}
	/* It's OK not to check TIDE here... */
	else if (crt_status & TABLE_HW_STATUS_BIT_NORSC)
		crt_status = -ENOMEM;
	/* consider to check TEMPNOR for EAGAIN. it is now ENOMEM since it may
	 * take a very long  time until rules are released. */
	else
		table_c_exception_handler(TABLE_CREATE_FUNC_ID,
					  __LINE__,
					  crt_status,
					  TABLE_ENTITY_HW);

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
						0,
						&rule_id);

		if (miss_status)
			table_c_exception_handler(
					TABLE_CREATE_FUNC_ID,
					__LINE__,
					TABLE_SW_STATUS_MISS_RES_CRT_FAIL,
					TABLE_ENTITY_SW);
	}
	return crt_status;
}


void table_get_params(enum table_hw_accel_id acc_id,
		      t_tbl_id table_id,
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
		table_c_exception_handler(TABLE_GET_PARAMS_FUNC_ID,
					  __LINE__,
					  status,
					  TABLE_ENTITY_HW);

	return;
}


void table_replace_miss_result(enum table_hw_accel_id acc_id,
			       t_tbl_id table_id,
			       struct table_result *new_miss_result,
			       struct table_result *replaced_miss_result)
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

	status = table_rule_replace_by_key_desc(acc_id,
						table_id,
						&new_miss_rule,
						0,
						replaced_miss_result);
	if (status)
		table_c_exception_handler(
			TABLE_REPLACE_MISS_RESULT_FUNC_ID,
			__LINE__,
			TABLE_SW_STATUS_MISS_RES_RPL_FAIL,
			TABLE_ENTITY_SW);
	return;
}


void table_get_miss_result(enum table_hw_accel_id acc_id,
			   t_tbl_id table_id,
			   struct table_result *miss_result)
{
	int32_t status;
	uint64_t invalid_8B;
	status = table_rule_query_by_key_desc(acc_id,
					      table_id,
					      0,
					      0,
					      miss_result,
					      ((uint32_t *)(&invalid_8B)),
					      ((uint32_t *)(&invalid_8B)),
					      &invalid_8B);

	if (status)
		table_c_exception_handler(TABLE_GET_MISS_RESULT_FUNC_ID,
					  __LINE__,
					  TABLE_SW_STATUS_MISS_RES_GET_FAIL,
					  TABLE_ENTITY_SW);
	return;
}


void table_delete(enum table_hw_accel_id acc_id,
		  t_tbl_id table_id)
{
	int32_t status;

	/* Prepare ACC context for CTLU accelerator call */
	__stqw(TABLE_DELETE_MTYPE, 0, table_id, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Check status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status)
		table_c_exception_handler(TABLE_DELETE_FUNC_ID,
					  __LINE__,
					  status,
					  TABLE_ENTITY_HW);

	return;
}

int table_get_next_ruleid(enum table_hw_accel_id acc_id,
			  t_tbl_id table_id,
			  t_rule_id rule_id,
			  t_rule_id *next_rule_id)
{
	struct table_rule_id_desc rule_id_desc __attribute__((aligned(16)));
	struct table_rule_id_desc
				next_rule_id_desc __attribute__((aligned(16)));
	int32_t status;

	rule_id_desc.rule_id = rule_id;

	uint32_t arg2 = (uint32_t)&next_rule_id_desc;
	uint32_t arg3 = table_id;

	/* Prepare ACC context for CTLU accelerator call */
	arg2 = __e_rlwimi(arg2, (uint32_t)&rule_id_desc, 16, 0, 15);
	arg3 = __e_rlwimi(arg3, 0x20, 16, 0, 15);
	__stqw(TABLE_GET_NEXT_RULEID_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS,
	       0);

	/* Accelerator call */
	__e_hwaccel(acc_id);

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if ((status != TABLE_HW_STATUS_SUCCESS) &&
	    (status != TABLE_HW_STATUS_BIT_MISS))
		/* Call fatal error handler */
		table_c_exception_handler(TABLE_GET_NEXT_RULEID_FUNC_ID,
					  __LINE__,
					  status,
					  TABLE_ENTITY_HW);

	*next_rule_id = next_rule_id_desc.rule_id;
	return status;
}

/*****************************************************************************/
/*				Internal API				     */
/*****************************************************************************/
int table_query_debug(enum table_hw_accel_id acc_id,
		      t_tbl_id table_id,
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


void table_hw_get_time_unit(enum table_hw_accel_id acc_id,
			    uint32_t *time_unit)
{
	uint32_t *reg_addr;
	if (acc_id == TABLE_ACCEL_ID_CTLU) {
		reg_addr = ((uint32_t *) 
				(TABLE_MEMMAP_CTLU_BASE_ADDR
				 |TABLE_MEMMAP_TIMESTAMP_WINDOW_BASE_OFFSET)
			   );
	} else {
		reg_addr = ((uint32_t *) 
				(TABLE_MEMMAP_MFLU_BASE_ADDR
				 |TABLE_MEMMAP_TIMESTAMP_WINDOW_BASE_OFFSET)
			   );
	}
	*time_unit = 1 <<  ((*reg_addr) &
			    TABLE_MEMMAP_TIMESTAMP_WINDOW_FIELD_MASK);
}


#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

void table_c_exception_handler(enum table_function_identifier func_id,
			       uint32_t line,
			       int32_t status,
			       enum table_entity entity)
					__attribute__ ((noreturn)) {
	table_exception_handler(__FILE__, func_id, line, status, entity);
}

#pragma stackinfo_ignore on

void table_exception_handler(char *file_path,
			     enum table_function_identifier func_id,
			     uint32_t line,
			     int32_t status_id,
			     enum table_entity entity)
				__attribute__ ((noreturn)) {
	char *func_name, *status;
	
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
	case TABLE_RULE_MODIFY_PRIORITY_FUNC_ID:
		func_name = "table_rule_modify_priority";
		break;
	case TABLE_RULE_QUERY_GET_RESULT_FUNC_ID:
		func_name = "table_rule_query_get_result";
		break;
	case TABLE_RULE_QUERY_GET_KEY_DESC_FUNC_ID:
		func_name = "table_rule_query_get_key_desc";
		break;
	case TABLE_RULE_DELETE_FUNC_ID:
		func_name = "table_rule_delete";
		break;
	case TABLE_RULE_REPLACE_BY_KEY_DESC_FUNC_ID:
		func_name = "table_rule_replace_by_key_desc";
		break;
	case TABLE_RULE_DELETE_BY_KEY_DESC_FUNC_ID:
		func_name = "table_rule_delete_by_key_desc";
		break;
	case TABLE_RULE_QUERY_BY_KEY_DESC_FUNC_ID:
		func_name = "table_rule_query_by_key_desc";
		break;
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
	case TABLE_CALC_NUM_ENTRIES_PER_RULE_FUNC_ID:
		func_name = "table_calc_num_entries_per_rule";
		break;
	case TABLE_GET_NEXT_RULEID_FUNC_ID:
		func_name = "table_get_next_ruleid";
		break;
	default:
		/* create own exception */
		exception_handler(__FILE__,
				  "table_exception_handler",
				  __LINE__,
				  "table_exception_handler got unknown"
				  "function identifier.\n");
	}
	if (entity == TABLE_ENTITY_HW){
		/* Call general exception handler */
		if (status_id & TABLE_HW_STATUS_BIT_TIDE) {
			status = "Invalid table ID.\n";
		} else if (status_id & TABLE_HW_STATUS_BIT_KSE) {
			status = "Key size error.\n";
		} else if (status_id & TABLE_HW_STATUS_BIT_MNLE) {
			status = "Maximum number of chained lookups reached.\n";
		} else {
			status = "Unknown or Invalid HW status.\n";
		}
	}
	else {
		switch (status_id) {
		case(TABLE_SW_STATUS_MISS_RES_CRT_FAIL):
			status = "Table miss rule creation failed.\n";
			break;
		case(TABLE_SW_STATUS_MISS_RES_RPL_FAIL):
			status = "Table replace miss result failed due to"
				 " non-existence of a miss result in the table."
				 "\n";
			break;
		case(TABLE_SW_STATUS_MISS_RES_GET_FAIL):
			status = "Table get miss result failed due to "
				 "non-existence of a miss result in the"
				 " table.\n";
			break;
		case(TABLE_SW_STATUS_QUERY_INVAL_ENTYPE):
			status = "Rule query failed due to unrecognized entry"
				 " type returned from HW.\n";
			break;
		case(TABLE_SW_STATUS_UNKNOWN_TBL_TYPE):
			status = "Unknown table type.\n";
			break;
		case(TABLE_SW_STATUS_SAME_PRIORITY):
			status = "Rule modify failed. New priority is the same"
				 " as the current.";
			break;
		case(TABLE_SW_STATUS_MFLU_DIFF_PRIORITY):
			/* TODO fill in Errata number */
			status = "Rule create failed to Errata No. TBD";
			break;
		default:
			status = "Unknown or Invalid SW status.\n";
			break;
		}
	}
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
		table_c_exception_handler(
				TABLE_CALC_NUM_ENTRIES_PER_RULE_FUNC_ID,
				__LINE__,
				TABLE_SW_STATUS_UNKNOWN_TBL_TYPE,
				TABLE_ENTITY_SW);
		break;
	}

	return num_entries_per_rule;
}


int table_lookup_by_keyid_default_frame_wrp(enum table_hw_accel_id acc_id,
					    t_tbl_id table_id,
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
			  t_tbl_id table_id,
			  struct table_rule *rule,
			  uint8_t key_size,
			  t_rule_id *rule_id)

{

	return table_rule_create(acc_id,
				 table_id,
				 rule,
				 key_size,
				 rule_id);
}

int table_rule_delete_wrp(enum table_hw_accel_id acc_id,
			  t_tbl_id table_id,
			  union table_key_desc *key_desc,
			  uint8_t key_size,
			  struct table_result *result)
{
	return table_rule_delete_by_key_desc(acc_id,
					     table_id,
					     key_desc,
					     key_size,
					     result);
}
