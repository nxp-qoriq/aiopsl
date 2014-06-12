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
	int32_t           status;
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
	uint16_t                          type = tbl_params->attributes;
	uint32_t                          max_rules = tbl_params->max_rules;
	uint32_t                          committed_rules =
		tbl_params->committed_rules;

	/* Calculate the number of entries each rule occupies */
	num_entries_per_rule = table_calc_num_entries_per_rule(
					type & TABLE_ATTRIBUTE_TYPE_MASK,
					key_size);

	/* Prepare input message */
	tbl_crt_in_msg.type = type;
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
	__e_rlwimi(arg2, (uint32_t)&tbl_crt_in_msg, 16, 0, 15);
	__stqw(TABLE_CREATE_MTYPE, arg2, 0, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Get new Table ID */
	*table_id = (((struct table_create_output_message *)acc_ctx->
		output_pointer)->tid);

	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);

	/* Add miss result to the table if needed and if an error did not occur
	 * during table creation */
	if (!status && ((tbl_params->attributes & TABLE_ATTRIBUTE_MR_MASK) ==
			TABLE_ATTRIBUTE_MR_MISS)) {
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
		status = table_rule_create(acc_id, *table_id, miss_rule, 0);

		if (status)
			exception_handler(__FILE__,
					  __LINE__,
					  "Table miss rule creation failed. ");
	} else if (status) {
		/* HW Status from table creation might be non zero*/
		switch (status) {
		case (CTLU_HW_STATUS_NORSC):
			status = -ENOMEM;
			break;
		case (MFLU_HW_STATUS_NORSC):
			status = -ENOMEM;
			break;
		case (CTLU_HW_STATUS_TEMPNOR):
			/* TODO Rev2 - consider to change it to EAGAIN. it is
			 * now ENOMEM since in Rev1 it may take a very long
			 * time until rules are released. */
			status = -ENOMEM;
			break;
		case (MFLU_HW_STATUS_TEMPNOR):
			/* TODO Rev2 - consider to change it to EAGAIN. it is
			 * now ENOMEM since in Rev1 it may take a very long
			 * time until rules are released. */
			status = -ENOMEM;
			break;
		default:
			table_fatal_status_handler(status);
			break;
		}
	}

	return status;
}


void table_replace_miss_result(enum table_hw_accel_id acc_id,
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
		exception_handler(__FILE__,
				  __LINE__,
				  "Table replace miss result failed due to non"
				  "-existance of a miss result in the table. ");

	return;
}


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
	tbl_params->attributes = output.type;

	/* Check status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (!status)
		table_fatal_status_handler(status);

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
		exception_handler(__FILE__,
				  __LINE__,
				  "Table get miss result failed due to non"
				  "-existance of a miss result in the table. ");

	return;
}


void table_delete(enum table_hw_accel_id acc_id,
		  uint16_t table_id)
{
	int32_t status;

	/* Prepare ACC context for CTLU accelerator call */
	__stqw(TABLE_DELETE_MTYPE, 0, table_id, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

	/* Check status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (!status)
		table_fatal_status_handler(status);

	return;
}


int32_t table_rule_create(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_rule *rule,
			  uint8_t key_size)
{
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
	__e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(TABLE_RULE_CREATE_RPTR_DEC_MTYPE, arg2, arg3, 0,
	       HWC_ACC_IN_ADDRESS, 0); /* using RPTR DEC because aging would
	       have removed this entry with DEC if it would arrived on time */

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
	// TODO #DEFINES
	case (0x00000400):
		/* A rule with the same match description (and aged) is found
		 * in the table. The rule is replaced. */
		status = TABLE_STATUS_SUCCESS;
		break;
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
		table_fatal_status_handler(status);
		break;
	}
	return status;
}


int32_t table_rule_create_or_replace(enum table_hw_accel_id acc_id,
				     uint16_t table_id,
				     struct table_rule *rule,
				     uint8_t key_size,
				     struct table_result *old_res)
{
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
	__e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
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

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		break;
	case (TABLE_HW_STATUS_MISS):
		break;
	case (CTLU_HW_STATUS_NORSC):
		status = -ENOMEM;
		break;
	case (MFLU_HW_STATUS_NORSC):
		status = -ENOMEM;
		break;
	case (CTLU_HW_STATUS_TEMPNOR):
		status = -EAGAIN;
		break;
	case (MFLU_HW_STATUS_TEMPNOR):
		status = -EAGAIN;
		break;
	default:
		/* Call fatal error handler */
		table_fatal_status_handler(status);
		break;
	} /* Switch */

	return status;
}


int32_t table_rule_replace(enum table_hw_accel_id acc_id,
			   uint16_t table_id,
			   struct table_rule *rule,
			   uint8_t key_size,
			   struct table_result *old_res)
{
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
	__e_rlwimi(arg2, (uint32_t)rule, 16, 0, 15);
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

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		break;
	case (TABLE_HW_STATUS_MISS):
		status = -EIO;
		break;
	default:
		/* Call fatal error handler */
		table_fatal_status_handler(status);
		break;
	} /* Switch */

	return status;
}


int32_t table_rule_query(enum table_hw_accel_id acc_id,
			 uint16_t table_id,
			 union table_key_desc *key_desc,
			 uint8_t key_size,
			 struct table_result *result,
			 uint32_t *timestamp)
{
	int32_t status;
	struct table_entry entry __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg3 = table_id;
	uint32_t arg2 = (uint32_t)&entry;
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__e_rlwimi(arg2, (uint32_t)key_desc, 16, 0, 15);
	__stqw(TABLE_RULE_QUERY_MTYPE, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call Table accelerator */
	__e_hwaccel(acc_id);

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
		/* Call fatal error handler TODO*/
		exception_handler(__FILE__,
				  __LINE__,
				  "Unknown result entry type. ");
		break;
	} /* Switch */

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		break;
	case (TABLE_HW_STATUS_MISS):
		/* A rule with the same match description is not found in the
		 * table. */
		break;
	case (CTLU_HW_STATUS_TEMPNOR):
		/* A rule with the same match description is found  and rule
		 * is aged. */
		status = TABLE_STATUS_MISS;
		break;
	case (MFLU_HW_STATUS_TEMPNOR):
		/* A rule with the same match description is found  and rule
		 * is aged. */
		status = TABLE_STATUS_MISS;
		break;
	default:
		/* Call fatal error handler */
		table_fatal_status_handler(status);
		break;
	} /* Switch */

	return status;
}


int32_t table_rule_delete(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  union table_key_desc *key_desc,
			  uint8_t key_size,
			  struct table_result *result)
{
	int32_t status;

	struct table_old_result old_res __attribute__((aligned(16)));
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg2 = (uint32_t)&old_res;
	uint32_t arg3 = table_id;
	__e_rlwimi(arg2, (uint32_t)key_desc, 16, 0, 15);
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

	/* Status Handling*/
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		break;
	case (TABLE_HW_STATUS_MISS):
		/* Rule was not found */
		status = -EIO;
		break;
	default:
		/* Call fatal error handler */
		table_fatal_status_handler(status);
		break;
	} /* Switch */

	return status;
}


int32_t table_lookup_by_key(enum table_hw_accel_id acc_id,
			    uint16_t table_id,
			    union table_lookup_key_desc key_desc,
			    uint8_t key_size,
			    struct table_lookup_result *lookup_result)
{
	int32_t status;
	/* optimization 1 clock */
	uint32_t arg2 = (uint32_t)lookup_result;
	__e_rlwimi(arg2, *((uint32_t *)(&key_desc)), 16, 0, 15);

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
		table_fatal_status_handler(status);
		break;
	} /* Switch */
	return status;
}


int32_t table_lookup_by_keyid_default_frame(enum table_hw_accel_id acc_id,
			      uint16_t table_id, uint8_t keyid,
			      struct table_lookup_result *lookup_result)
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
	default:
		/* Call fatal error handler */
		table_fatal_status_handler(status);
		break;
	} /* Switch */

	return status;
}


int32_t table_lookup_by_keyid(enum table_hw_accel_id acc_id,
			      uint16_t table_id,
			      uint8_t keyid,
			      uint32_t flags,
			      struct table_lookup_non_default_params
				     *ndf_params,
			      struct table_lookup_result *lookup_result)
{
	int32_t status;

	/* optimization 1 clock */
	uint32_t arg2 = (uint32_t)lookup_result;
	__e_rlwimi(arg2, *((uint32_t *)(ndf_params)), 16, 0, 15);

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
	switch (status) {
	case (TABLE_HW_STATUS_SUCCESS):
		break;
	case (TABLE_HW_STATUS_MISS):
		break;
	case (TABLE_HW_STATUS_EOFH):
		status = -EIO;
		break;
	default:
		/* Call fatal error handler */
		table_fatal_status_handler(status);
		break;
	} /* Switch */

	return status;
}


/*****************************************************************************/
/*				Internal API				     */
/*****************************************************************************/
int32_t table_query_debug(enum table_hw_accel_id acc_id,
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


int32_t table_hw_accel_acquire_lock(enum table_hw_accel_id acc_id)
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


void table_fatal_status_handler(int32_t status){

	switch (status) {
	case (TABLE_HW_STATUS_MNLE):
		handle_fatal_error("Maximum number of chained lookups reached"
				   ".\n");
		break;
	case (TABLE_HW_STATUS_KSE):
		handle_fatal_error("Key size error.\n");
		break;
	case (MFLU_HW_STATUS_TIDE):
		handle_fatal_error("Invalid MFLU table ID.\n");
		break;
	case (CTLU_HW_STATUS_TIDE):
		handle_fatal_error("Invalid CTLU table ID.\n");
		break;
	default:
		handle_fatal_error("Unknown or Invalid status.\n");
		break;
	}

	return;
}

int table_calc_num_entries_per_rule(uint16_t type, uint8_t key_size){
	/* Initialized to one for the EM simple case where key_size <= 24 */
	int num_entries_per_rule = 1;

	switch (type) {

	case TABLE_ATTRIBUTE_TYPE_EM:
		if (key_size > TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE) {
			num_entries_per_rule += ((((key_size -
			TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE) - 1)
			/ TABLE_ENTRY_EME36_LOOKUP_KEY_SIZE
			) + 1);
		}
		/* First, we remove the last entry key size which is already
		 * included in initialization. Then, we divide by the key
		 * size of a non-last entry (minus one) to get number of
		 * entries which are non last. and finally add one since we
		 * have reminder */

		/* CONSIDER other methods if devision too costly.
		 * Devision operation on e200 is 4-34 cycles.	*/

		/* else, already initialized to one */
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
		exception_handler(__FILE__,
				  __LINE__,
				  "UNknown table type.");
		break;
	}

	return num_entries_per_rule;
}

