/**************************************************************************//**
@File		table.c

@Description	This file contains the AIOP SW Table API implementation.

		Copyright 2013-2014 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_cdma.h"
#include "dplib/fsl_table.h"

#include "table.h"

int32_t table_create(enum table_hw_accel_id acc_id,
		     struct table_create_params *tbl_params,
		     uint16_t *table_id)
{
	int32_t status;
	struct table_rule * miss_rule;
	/* Initialized to one for the simple case where key_size <= 24 */
	int                               num_entries_per_rule = 1;

	/* 16 Byte aligned for stqw optimization + HW requirements */
	struct ctlu_table_create_input_message  tbl_crt_in_msg
		__attribute__((aligned(16)));

	struct ctlu_table_create_output_message tbl_crt_out_msg
		__attribute__((aligned(16)));
	int32_t                           cdma_status;

	struct ctlu_acc_context      *acc_ctx =
		(struct ctlu_acc_context *)HWC_ACC_IN_ADDRESS;

	uint32_t arg2 = (uint32_t)&tbl_crt_out_msg; /* To be used in
						    Accelerator context __stqw
						    */

	/* Load frequent parameters into registers */
	int                               key_size = tbl_params->key_size;
	uint16_t                          type = tbl_params->attributes;
	uint32_t                          max_rules = tbl_params->max_rules;
	uint32_t                          committed_rules =
		tbl_params->committed_rules;

	/* Calculate the number of entries each rule occupies */
	switch (type & TABLE_ATTRIBUTE_TYPE_MASK) {

	case TABLE_ATTRIBUTE_TYPE_EM:
		if (key_size > CTLU_SINGLE_ENTRY_RULE_KEY_SIZE) {
			num_entries_per_rule += ((((key_size -
			CTLU_MULTIPLE_ENTRY_RULE_LAST_ENTRY_KEY_SIZE) - 1)
			/ CTLU_MULTIPLE_ENTRY_RULE_NON_LAST_ENTRY_KEY_SIZE
			) + 1);
		}
		/* First, we remove the last entry key size which is already
		 * included in initialization. Then, we divide by the key
		 * size of a non-last entry (minus one) to get number of
		 * entries which are non last. and finally add one since we
		 * have reminder */

		/* TODO CONSIDER other methods if devision too costly.
		 * Devision operation on e200 is 4-34 cycles.	*/

		/* else, already initialized to one */
		break;

	case TABLE_ATTRIBUTE_TYPE_LPM:
		if (key_size > CTLU_MAX_LPM_EM4_IPV4_KEY_SIZE) {
			/* IPv6*/
			num_entries_per_rule =
					CTLU_LPM_IPV6_WC_ENTRIES_PER_RULE;
		} else {
			/* IPv4 */
			num_entries_per_rule =
					CTLU_LPM_IPV4_WC_ENTRIES_PER_RULE;
		}
		break;
/*
	case CTLU_TBL_ATTRIBUTE_TYPE_TCAM_ACL:
		break;

	case CTLU_TBL_ATTRIBUTE_TYPE_ALG_ACL:
		break;
*/
	default:
		break;

	}

	/* Prepare input message */
	tbl_crt_in_msg.type = type;
	tbl_crt_in_msg.icid = CTLU_TABLE_CREATE_INPUT_MESSAGE_ICID_BDI_MASK;
	tbl_crt_in_msg.max_rules = max_rules;
	tbl_crt_in_msg.max_entries =
		num_entries_per_rule * max_rules;
	tbl_crt_in_msg.committed_entries =
			num_entries_per_rule * committed_rules;
		/* Optimization
		 * Multiply on e200 is 2 clocks latency, 1 clock throughput */
	tbl_crt_in_msg.committed_rules = committed_rules;

	cdma_status = cdma_ws_memory_init(tbl_crt_in_msg.reserved,
			      CTLU_TABLE_CREATE_INPUT_MESSAGE_RESERVED_SPACE,
			      0);

	/* handle CDMA error */
	if (cdma_status != CDMA_WS_MEMORY_INIT__SUCCESS) {
		return cdma_status;
	}

	/* Prepare ACC context for CTLU accelerator call */
	__e_rlwimi(arg2, (uint32_t)&tbl_crt_in_msg, 16, 0, 15);
	__stqw(TABLE_CREATE_MTYPE, arg2, 0, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id); /*TODO*/

	/* Return Table ID */
	*table_id = (((struct ctlu_table_create_output_message *)acc_ctx->
		output_pointer)->tid);

	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);

	if (status) {
		return status;
	}

	/* TODO perform assertion of size here */
	/* Re-assignment of the structure is done because of stack limitations
	 * of the service layer */
	miss_rule = (struct table_rule *)&tbl_crt_in_msg;
	miss_rule->options = CTLU_RULE_TIMESTAMP_NONE;

	/* Copy miss result  - Last 16 bytes */
	__stqw(*(((uint32_t *)&tbl_params->miss_result) + 1),
	       *(((uint32_t *)&tbl_params->miss_result) + 2),
	       *(((uint32_t *)&tbl_params->miss_result) + 3),
	       *(((uint32_t *)&tbl_params->miss_result) + 4),
	       0, ((uint32_t *)&(miss_rule->result) + 1));

	/* Copy miss result  - First 4 bytes */
	*((uint32_t *)(&(miss_rule->result))) =
			*((uint32_t *)&tbl_params->miss_result);

	return table_rule_create(acc_id, *table_id, miss_rule, 0);
}


int32_t table_replace_miss_result(enum table_hw_accel_id acc_id,
				  uint16_t table_id,
				  struct table_rule_result
					 *new_miss_result,
				  struct table_rule_result
					 *old_miss_result)
{
	/* 16 Byte aligned for stqw optimization + HW requirements */
	struct table_rule new_miss_rule __attribute__((aligned(16)));
	new_miss_rule.options = CTLU_RULE_TIMESTAMP_NONE;

	/* Copy miss result  - Last 16 bytes */
	__stqw(*(((uint32_t *)new_miss_result) + 1),
	       *(((uint32_t *)new_miss_result) + 2),
	       *(((uint32_t *)new_miss_result) + 3),
	       *(((uint32_t *)new_miss_result) + 4),
	       0, ((uint32_t *)&(new_miss_rule.result) + 1));

	/* Copy miss result  - First 4 bytes */
	*((uint32_t *)(&(new_miss_rule.result))) =
			*((uint32_t *)new_miss_result);

	return table_rule_replace(acc_id, table_id, &new_miss_rule, 0,
				       old_miss_result);
}


int32_t table_get_params(enum table_hw_accel_id acc_id,
			 uint16_t table_id,
			 struct table_get_params_output *tbl_params)
{
	struct ctlu_table_params_query_output_message output
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
	tbl_params->attributes = output.type;

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_get_miss_result(enum table_hw_accel_id acc_id,
			      uint16_t table_id,
			      struct table_rule_result *miss_result)
{
	uint32_t invalid_timestamp;
	return
	  table_rule_query(acc_id, table_id, 0, 0, miss_result,
				&invalid_timestamp);

}


int32_t table_delete(enum table_hw_accel_id acc_id,
		     uint16_t table_id)
{
	/* Prepare ACC context for CTLU accelerator call */
	__stqw(TABLE_DELETE_MTYPE, 0, table_id, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	/* Return Status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_rule_create(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_rule *rule,
			  uint8_t key_size)
{
	struct table_old_result aged_res __attribute__((aligned(16)));
	uint32_t arg2 = (uint32_t)&aged_res;
	uint32_t arg3 = table_id;

	/* Set Opaque1, Opaque2 valid bits*/
	*(uint16_t *)(&(rule->result.type)) |= CTLU_TLUR_OPAQUE_VALID_BITS_MASK;

	/* Clear byte in offset 2*/
	*((uint8_t *)&(rule->result) + 2) = 0;

	if (rule->result.type == CTLU_RULE_RESULT_TYPE_CHAINING) {
		rule->result.op_rptr_clp.chain_parameters.reserved1 = 0;
		rule->result.op_rptr_clp.chain_parameters.reserved0 =
			CTLU_TLUR_TKIDV_BIT_MASK;
	}

	/* Prepare ACC context for CTLU accelerator call */
	__e_rlwimi(arg2, rule, 16, 0, 15);
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(TABLE_RULE_CREATE_RPTR_DEC_MTYPE, arg2, arg3, 0,
	       HWC_ACC_IN_ADDRESS, 0); /* using RPTR DEC because aging would
	       have removed this entry with DEC if it would arrived on time */

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	/* Return Status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_rule_create_or_replace(enum table_hw_accel_id acc_id,
				     uint16_t table_id,
				     struct table_rule *rule,
				     uint8_t key_size,
				     struct table_rule_result *old_res)
{
	struct table_old_result hw_old_res __attribute__((aligned(16)));
	uint32_t arg2 = (uint32_t)&hw_old_res;
	uint32_t arg3 = table_id;

	/* Set Opaque1, Opaque2 valid bits*/
	*(uint16_t *)(&(rule->result.type)) |= CTLU_TLUR_OPAQUE_VALID_BITS_MASK;

	/* Clear byte in offset 2*/
	*((uint8_t *)&(rule->result) + 2) = 0;

	if (rule->result.type == CTLU_RULE_RESULT_TYPE_CHAINING) {
		rule->result.op_rptr_clp.chain_parameters.reserved1 = 0;
		rule->result.op_rptr_clp.chain_parameters.reserved0 =
			CTLU_TLUR_TKIDV_BIT_MASK;
	}

	/* Prepare ACC context for CTLU accelerator call */
	__e_rlwimi(arg2, rule, 16, 0, 15);
	__e_rlwimi(arg3, key_size, 16, 0, 15);

	if (old_res) { /* Returning result and thus not decrementing RCOUNT */
		__stqw(TABLE_RULE_CREATE_OR_REPLACE_MTYPE, arg2, arg3, 0,
		       HWC_ACC_IN_ADDRESS, 0);
		__e_hwaccel(acc_id);
		/* STQW optimization is not done here so we do not force
		   alignment */
		*old_res = hw_old_res.result;
	} else {
		__stqw(TABLE_RULE_CREATE_OR_REPLACE_RPTR_DEC_MTYPE, arg2, arg3,
		       0, HWC_ACC_IN_ADDRESS, 0);
		__e_hwaccel(acc_id);
	}

	/* Return Status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_rule_replace(enum table_hw_accel_id acc_id,
			   uint16_t table_id,
			   struct table_rule *rule,
			   uint8_t key_size,
			   struct table_rule_result *old_res)
{
	struct table_old_result hw_old_res __attribute__((aligned(16)));
	uint32_t arg2 = (uint32_t)&hw_old_res;
	uint32_t arg3 = table_id;

	/* Set Opaque1, Opaque2 valid bits*/
	*(uint16_t *)(&(rule->result.type)) |= CTLU_TLUR_OPAQUE_VALID_BITS_MASK;

	/* Clear byte in offset 2*/
	*((uint8_t *)&(rule->result) + 2) = 0;

	if (rule->result.type == CTLU_RULE_RESULT_TYPE_CHAINING) {
		rule->result.op_rptr_clp.chain_parameters.reserved1 = 0;
		rule->result.op_rptr_clp.chain_parameters.reserved0 =
			CTLU_TLUR_TKIDV_BIT_MASK;
	}

	/* Prepare ACC context for CTLU accelerator call */
	__e_rlwimi(arg2, rule, 16, 0, 15);
	__e_rlwimi(arg3, key_size, 16, 0, 15);

	if (old_res) { /* Returning result and thus not decrementing RCOUNT */
		__stqw(TABLE_RULE_REPLACE_MTYPE, arg2, arg3, 0,
		       HWC_ACC_IN_ADDRESS, 0);
		__e_hwaccel(acc_id);
		/* STQW optimization is not done here so we do not force
		   alignment */
		*old_res = hw_old_res.result;
	} else {
		__stqw(TABLE_RULE_REPLACE_MTYPE_RPTR_DEC_MTYPE, arg2, arg3, 0,
		       HWC_ACC_IN_ADDRESS, 0);
		__e_hwaccel(acc_id);
	}

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_rule_query(enum table_hw_accel_id acc_id,
			 uint16_t table_id,
			 union ctlu_key *key,
			 uint8_t key_size,
			 struct table_rule_result *result,
			 uint32_t *timestamp)
{
	struct table_entry entry __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg3 = table_id;
	uint32_t arg2 = (uint32_t)&entry;
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__e_rlwimi(arg2, key, 16, 0, 15);
	__stqw(TABLE_RULE_QUERY_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	switch (entry.type & CTLU_TABLE_ENTRY_ENTYPE_FIELD_MASK) {
	case (CTLU_TABLE_ENTRY_ENTYPE_EME16):
		*timestamp = entry.body.eme16.timestamp;
		/* STQW optimization is not done here so we do not force
		   alignment */
		*result = entry.body.eme16.result;
		break;
	case (CTLU_TABLE_ENTRY_ENTYPE_EME24):
		*timestamp = entry.body.eme24.timestamp;
		/* STQW optimization is not done here so we do not force
		   alignment */
		*result = entry.body.eme24.result;
		break;
	case (CTLU_TABLE_ENTRY_ENTYPE_LPM_RES):
		*timestamp = entry.body.lpm_res.timestamp;
		/* STQW optimization is not done here so we do not force
		   alignment */
		*result = entry.body.lpm_res.result;
		break;
	default:
		break;
		/* todo */
	} /* Switch */

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_rule_delete(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  union ctlu_key *key,
			  uint8_t key_size,
			  struct table_rule_result *result)
{
	struct table_old_result old_res __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg2 = (uint32_t)&old_res;
	uint32_t arg3 = table_id;
	__e_rlwimi(arg2, key, 16, 0, 15);
	__e_rlwimi(arg3, key_size, 16, 0, 15);

	if (result) { /* Returning result and thus not decrementing RCOUNT */
		__stqw(TABLE_RULE_DELETE_MTYPE, arg2, arg3, 0,
		       HWC_ACC_IN_ADDRESS, 0);
		__e_hwaccel(acc_id);
		/* STQW optimization is not done here so we do not force
		   alignment */
		*result = old_res.result;
	} else {
		__stqw(TABLE_RULE_DELETE_MTYPE_RPTR_DEC_MTYPE, arg2, arg3, 0,
		       HWC_ACC_IN_ADDRESS, 0);
		__e_hwaccel(acc_id);
	}

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_lookup_by_keyid(enum table_hw_accel_id acc_id,
			      uint16_t table_id, uint8_t keyid,
			      struct table_lookup_result *lookup_result)
{

	/* Prepare HW context for TLU accelerator call */
	__stqw(TABLE_LOOKUP_KEYID_EPRS_TMSTMP_RPTR_MTYPE,
	       (uint32_t)lookup_result, table_id | (((uint32_t)keyid) << 16),
	       0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU Lookup accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_lookup_by_key(enum table_hw_accel_id acc_id,
			    uint16_t table_id,
			    union ctlu_key *key,
			    uint8_t key_size,
			    struct table_lookup_result *lookup_result)
{
	/* optimization 1 clock */
	uint32_t arg2 = (uint32_t)lookup_result;
	__e_rlwimi(arg2, key, 16, 0, 15);

	/* Prepare HW context for TLU accelerator call */
	__stqw(TABLE_LOOKUP_KEY_TMSTMP_RPTR_MTYPE, arg2,
	       table_id | (((uint32_t)key_size) << 16), 0, HWC_ACC_IN_ADDRESS,
	       0);

	/* Call CTLU Lookup accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

/*****************************************************************************/
/*				Internal API				     */
/*****************************************************************************/
int32_t table_query_debug(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct ctlu_table_params_query_output_message *output
			 )
{
	/* Prepare ACC context for TLU accelerator call */
	__stqw(TABLE_QUERY_MTYPE, (uint32_t)output, table_id, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t table_hw_accel_acquire_lock(enum table_hw_accel_id acc_id){
	__stqw(TABLE_ACQUIRE_SEMAPHORE_MTYPE, 0, 0, 0, HWC_ACC_IN_ADDRESS, 0);
	__e_hwaccel(acc_id);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


void table_hw_accel_release_lock(enum table_hw_accel_id acc_id){
	__stqw(TABLE_RELEASE_SEMAPHORE_MTYPE, 0, 0, 0, HWC_ACC_IN_ADDRESS, 0);
	__e_hwaccel(acc_id);
}

