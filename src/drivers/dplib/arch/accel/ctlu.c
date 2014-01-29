/**************************************************************************//**
@File		ctlu.c

@Description	This file contains the AIOP SW CTLU API implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_cdma.h"
#include "dplib/fsl_ctlu.h"

#include "ctlu.h"
#include "system.h"
#include "id_pool.h"


extern uint64_t ext_keyid_pool_address;


int32_t ctlu_table_create(struct ctlu_table_create_params *tbl_params,
			  uint16_t *table_id)
{
	/* Initialized to one for the simple case where key_size <= 24 */
	int                               num_entries_per_rule = 1;

	/* 8 Byte aligned for stqw optimization */
	struct ctlu_table_create_input_message  tbl_crt_in_msg
		__attribute__((aligned(8)));

	struct ctlu_table_create_output_message tbl_crt_out_msg;
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
	switch (type & CTLU_TBL_ATTRIBUTE_TYPE_MASK) {

	case CTLU_TBL_ATTRIBUTE_TYPE_EM:
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

	case CTLU_TBL_ATTRIBUTE_TYPE_LPM:
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

	case CTLU_TBL_ATTRIBUTE_TYPE_TCAM_ACL:
		/* TODO */
		break;

	case CTLU_TBL_ATTRIBUTE_TYPE_ALG_ACL:
		/* TODO */
		break;

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
	if (cdma_status) { /* handle CDMA error TODO maybe use CR instead */
		return CTLU_CDMA_CALL_ERROR;
	}

	/* Copy miss result  - Last 16 bytes (Optimization - 2 clocks)*//*
	__stqw(*(((uint32_t *)&(tbl_params->miss_result)) + 1),
	       *(((uint32_t *)&(tbl_params->miss_result)) + 2),
	       *(((uint32_t *)&(tbl_params->miss_result)) + 3),
	       *(((uint32_t *)&(tbl_params->miss_result)) + 4),
	       0, ((uint32_t *)&(tbl_crt_in_msg.miss_lookup_fcv)) + 1);*/
	/* Copy miss result  - First 4 bytes *//*
	*((uint32_t *)(&(tbl_crt_in_msg.miss_lookup_fcv))) =
			*((uint32_t *)&tbl_params->miss_result);*/

	/*TODO temp fix until compiler issue with alignment is solved */
	tbl_crt_in_msg.miss_lookup_fcv = tbl_params->miss_result;

	/* Prepare ACC context for CTLU accelerator call */
	__e_rlwimi(arg2, (uint32_t)&tbl_crt_in_msg, 16, 0, 15);
	__stqw(CTLU_TABLE_CREATE_MTYPE, arg2, 0, 0, HWC_ACC_IN_ADDRESS,
	       0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return Table ID */
	*table_id = (((struct ctlu_table_create_output_message *)acc_ctx->
		output_pointer)->tid);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_update_miss_result(uint16_t table_id,
				      struct ctlu_table_rule_result *miss_rule)
{
	/* 8 Byte aligned for stqw optimization */
	struct ctlu_table_params_replace_input_message tbl_params_in_msg
		__attribute__((aligned(8)));

	/* TODO  potential compiler issue? *//*
	__stdw(0, 0, 0, tbl_params_in_msg.reserved);
	__stdw(0, 0, 0x8, tbl_params_in_msg.reserved);
	__stdw(0, 0, 0x10, tbl_params_in_msg.reserved);
	__stdw(0, 0, 0x18, tbl_params_in_msg.reserved);
	__stdw(0, 0, 0x20, tbl_params_in_msg.reserved);
	__stw(0, 0x28, tbl_params_in_msg.reserved);*/

	/* TODO potential compiler issue? *//*
	__stqw(0, 0, 0, 0, 0, tbl_params_in_msg.reserved);
	__stqw(0, 0, 0, 0, 0x10, tbl_params_in_msg.reserved);
	__stqw(0, 0, 0, 0, 0x20, tbl_params_in_msg.reserved);
	/* This overruns miss result field. The code later overruns it again
	with the actual miss result. */

	/* Prepare input message */
	*((uint64_t *)tbl_params_in_msg.reserved) = 0;
	*((uint64_t *)tbl_params_in_msg.reserved + 1) = 0;
	*((uint64_t *)tbl_params_in_msg.reserved + 2) = 0;
	*((uint64_t *)tbl_params_in_msg.reserved + 3) = 0;
	*((uint64_t *)tbl_params_in_msg.reserved + 4) = 0;
	*((uint32_t *)tbl_params_in_msg.reserved + 10) = 0;

	/* Copy miss result  - Last 16 bytes *//*
	__stqw(*(((uint32_t *)miss_rule) + 1),
	       *(((uint32_t *)miss_rule) + 2),
	       *(((uint32_t *)miss_rule) + 3),
	       *(((uint32_t *)miss_rule) + 4),
	       0, ((uint32_t *)&(tbl_params_in_msg.miss_lookup_fcv) + 1));*/

	/* Copy miss result  - First 4 bytes *//*
	*((uint32_t *)(&(tbl_params_in_msg.miss_lookup_fcv))) =
			*((uint32_t *)miss_rule);*/

	/*TODO temp fix until compiler issue with alignment is solved */
	tbl_params_in_msg.miss_lookup_fcv = *miss_rule;

	/* Prepare ACC context for CTLU accelerator call */
	__stqw(CTLU_TABLE_PARAMETERS_REPLACE_MTYPE,
	       ((uint32_t)&tbl_params_in_msg) << 16, table_id, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_get_params(uint16_t table_id,
			      struct ctlu_table_get_params_output *tbl_params)
{

	struct ctlu_table_params_query_output_message output;

	/* Prepare ACC context for TLU accelerator call */
	__stqw(CTLU_TABLE_QUERY_MTYPE, (uint32_t)&output, table_id, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Getting output parameters */
	tbl_params->current_rules = output.current_rules;
	tbl_params->committed_rules = output.committed_rules;
	tbl_params->max_rules = output.max_rules;
	tbl_params->attributes = output.type;

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_get_miss_result(uint16_t table_id,
				   struct ctlu_table_rule_result *miss_rule)
{
	struct ctlu_table_params_query_output_message output;

	/* Prepare ACC context for TLU accelerator call */
	__stqw(CTLU_TABLE_QUERY_REFERENCE_INC_MTYPE, (uint32_t)&output,
	       table_id, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Copy miss result */
	*((uint64_t *) miss_rule) = *((uint64_t *)&(output.miss_lookup_fcv));
	*(((uint64_t *) miss_rule) + 1) =
		*(((uint64_t *)&(output.miss_lookup_fcv)) + 1);
	*(((uint32_t *) miss_rule) + 4) =
			*(((uint32_t *)&(output.miss_lookup_fcv)) + 4);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_delete(uint16_t table_id)
{

	/* Prepare ACC context for CTLU accelerator call */
	__stqw(CTLU_TABLE_DELETE_MTYPE, 0, table_id, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return Status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_rule_create(uint16_t table_id,
			       struct ctlu_table_rule *rule,
			       uint8_t key_size)
{
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
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(CTLU_RULE_CREATE_MTYPE, ((uint32_t)rule) << 16,
	       arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return Status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_rule_create_or_replace(uint16_t table_id,
					  struct ctlu_table_rule *rule,
					  uint8_t key_size)
{
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
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(CTLU_RULE_CREATE_OR_REPLACE_MTYPE, ((uint32_t)rule) << 16,
	       arg3, 0,	HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return Status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_rule_replace(uint16_t table_id,
				struct ctlu_table_rule *rule,
				uint8_t key_size)
{
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
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(CTLU_RULE_REPLACE_MTYPE, ((uint32_t)rule) << 16,
	       arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_rule_delete(uint16_t table_id, union ctlu_key *key,
			       uint8_t key_size)
{
	/* Prepare HW context for TLU accelerator call */
	uint32_t arg3 = table_id;
	__e_rlwimi(arg3, key_size, 16, 0, 15);
	__stqw(CTLU_RULE_DELETE_MTYPE, ((uint32_t)key) << 16,
	       arg3, 0,	HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_lookup_by_keyid(uint16_t table_id, uint8_t keyid,
			struct ctlu_lookup_result *lookup_result)
{

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_LOOKUP_KEYID_EPRS_TMSTMP_RPTR_MTYPE,
	       (uint32_t)lookup_result, table_id | (((uint32_t)keyid) << 16),
	       0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU Lookup accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_table_lookup_by_key(uint16_t table_id, union ctlu_key *key,
				 uint8_t key_size,
				 struct ctlu_lookup_result *lookup_result)
{
	/* optimization 1 clock */
	uint32_t arg2 = (uint32_t)lookup_result;
	__e_rlwimi(arg2, key, 16, 0, 15);

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_LOOKUP_KEY_TMSTMP_RPTR_MTYPE, arg2,
	       table_id | (((uint32_t)key_size) << 16), 0, HWC_ACC_IN_ADDRESS,
	       0);

	/* Call CTLU Lookup accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

int32_t ctlu_kcr_builder_init(struct ctlu_kcr_builder *kb)
{
	/* clear NFEC (first byte in the KCR) */
	kb->kcr[CTLU_KCR_NFEC] = 0;
	/* Initialize KCR length to 1 */
	kb->kcr_length = 1;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_constant_fec(uint8_t constant, uint8_t num,
					       struct ctlu_kcr_builder *kb)
{

	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1;

	if ((curr_byte + CTLU_KCR_CONST_FEC_SIZE) > CTLU_KCR_MAX_KCR_SIZE)
		return CTLU_KCR_SIZE_ERR;

	if (num > 16)
		return CTLU_KCR_UDC_FEC_ERR;

	/* Build the FEC */
	/* User-defined FECID, no mask extension */
	fecid = CTLU_KCR_UDC_FECID << 1;
	/* OP0 = The number of times the user defined constant is repeated */
	op0 = (num-1);
	/* OP1 = User defined constant */
	op1 = constant;

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;

	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += 3;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_protocol_specific_field(enum
	kcr_builder_protocol_fecid protocol_fecid,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	mask_bytes = 0;
	uint8_t	fec_bytes_num = CTLU_KCR_PROTOCOL_SPECIFIC_FEC_SIZE;
	uint8_t fecid;

	/* Build the FEC */
	/* Protocol Specific FECID, mask extension indication */
	fecid = (uint8_t)protocol_fecid << 1;

	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */

		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+7] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+5] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+3] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+4] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+3] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+2] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+1] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}
	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_protocol_based_generic_fec(
	enum kcr_builder_parse_result_offset pr_offset,
	uint8_t extract_offset, uint8_t extract_size,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	mask_bytes = 0;
	uint8_t	fec_bytes_num = CTLU_KCR_PROTOCOL_BASED_GENERIC_FEC_SIZE;
	uint8_t fecid, op0, op1, op2;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = CTLU_KCR_GEC_FECID << 1;

	/* OP0 for Protocol based extraction */
	if (pr_offset > CTLU_KCR_PROTOCOL_MAX_OFFSET)
		return CTLU_KCR_PR_OFFSET_ERR;
	else
		op0 = CTLU_KCR_OP0_HET_PROTOCOL |
			  CTLU_KCR_PROTOCOL_HVT |
			  (uint8_t)pr_offset;

	/* OP1 = Extract Offset */
	if (extract_offset > CTLU_KCR_MAX_EXTRACT_OFFET)
		return CTLU_KCR_EXTRACT_OFFSET_ERR;
	else
		op1 = extract_offset;

	/* OP2 = Extract Size*/
	if (extract_size > CTLU_KCR_MAX_EXTRACT_SIZE)
		return CTLU_KCR_EXTRACT_SIZE_ERR;
	else
		op2 = extract_size;

	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


int32_t ctlu_kcr_builder_add_generic_extract_fec(uint8_t offset,
	uint8_t extract_size, uint32_t flags,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)

{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1, op2;
	uint8_t aligned_offset, extract_offset;
	uint8_t	fec_bytes_num = CTLU_KCR_GENERIC_FEC_SIZE;
	uint8_t	mask_bytes = 0;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = CTLU_KCR_GEC_FECID << 1;

	/* OP0 for General extraction */
	op0 = 0;
	aligned_offset = offset & CTLU_KCR_16_BYTES_ALIGNMENT;
	if (flags & CTLU_KCR_GEC_FRAME) {
		/*! Generic extraction from start of frame */
		switch (aligned_offset) {

		case (0x00):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x00;
			break;

		case (0x10):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x10;
			break;

		case (0x20):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x20;
			break;

		case (0x30):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x30;
			break;

		case (0x40):
			op0 = CTLU_KCR_OP0_HET_GEC |
			     CTLU_KCR_EOM_FRAME_OFFSET_0x40;
			break;

		case (0x50):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x50;
			break;

		case (0x60):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x60;
			break;

		case (0x70):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x70;
			break;

		case (0x80):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x80;
			break;

		case (0x90):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0x90;
			break;

		case (0xA0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xA0;
			break;

		case (0xB0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xB0;
			break;

		case (0xC0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xC0;
			break;

		case (0xD0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xD0;
			break;

		case (0xE0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xE0;
			break;

		case (0xF0):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_FRAME_OFFSET_0xF0;
			break;

		default:
			break;

		} /* switch */
	} /* if */
	else if (flags & CTLU_KCR_GEC_PARSE_RES) {
		if (aligned_offset > 0x30)
			return CTLU_KCR_EXTRACT_OFFSET_ERR;
		else
		switch (aligned_offset) {

		case (0x00):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x00;
			break;

		case (0x10):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x10;
			break;

		case (0x20):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x20;
			break;

		case (0x30):
			op0 = CTLU_KCR_OP0_HET_GEC |
			      CTLU_KCR_EOM_PARSE_RES_OFFSET_0x30;
			break;

		default:
			break;
		}
	} else
		return CTLU_KCR_PROTOCOL_GEC_ERR;


	/* OP1 = Extract Offset */
	extract_offset = offset & CTLU_KCR_OFFSET_WITHIN_16_BYTES;
	if (extract_offset > CTLU_KCR_MAX_EXTRACT_OFFET)
		return CTLU_KCR_EXTRACT_OFFSET_ERR;
	else
		op1 = extract_offset;

	/* OP2 = Extract Size*/
	if (extract_size > CTLU_KCR_MAX_EXTRACT_SIZE)
		return CTLU_KCR_EXTRACT_SIZE_ERR;
	else
		op2 = extract_size;

	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}

int32_t ctlu_kcr_builder_add_lookup_result_field_fec(uint8_t extract_field,
	uint8_t offset_in_opaque, uint8_t extract_size_in_opaque,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1, op2;
	uint8_t	fec_bytes_num = CTLU_KCR_LOOKUP_RES_FEC_SIZE;
	uint8_t	mask_bytes = 0;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = CTLU_KCR_GEC_FECID << 1;

	switch (extract_field) {

	case (CTLU_KCR_EXT_OPAQUE0):
		if ((offset_in_opaque + extract_size_in_opaque) > 8) {
			return CTLU_KCR_EXTRACT_OFFSET_ERR;
		} else {
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x00;
		op1 = CTLU_KCR_EXT_OPAQUE0_OFFSET + offset_in_opaque;
		op2 = extract_size_in_opaque - 1;
		}
		break;
	case (CTLU_KCR_EXT_OPAQUE1):
		if ((offset_in_opaque + extract_size_in_opaque) > 8) {
			return CTLU_KCR_EXTRACT_OFFSET_ERR;
		} else {
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x00;
		op1 = CTLU_KCR_EXT_OPAQUE1_OFFSET + offset_in_opaque;
		op2 = extract_size_in_opaque - 1;
		}
		break;
	case (CTLU_KCR_EXT_OPAQUE2):
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x10;
		op1 = CTLU_KCR_EXT_OPAQUE2_OFFSET;
		op2 = CTLU_KCR_EXT_OPAQUE2_SIZE;
		break;
	/*case (CTLU_KCR_EXT_UNIQUE_ID):
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x10;
		op1 = CTLU_KCR_EXT_UNIQUE_ID_OFFSET;
		op2 = CTLU_KCR_EXT_UNIQUE_ID_SIZE;
		break;
	case (CTLU_KCR_EXT_TIMESTAMP):
		op0 = CTLU_KCR_OP0_HET_GEC | CTLU_KCR_EOM_FCV_OFFSET_0x10;
		op1 = CTLU_KCR_EXT_TIMESTAMP_OFFSET;
		op2 = CTLU_KCR_EXT_TIMESTAMP_SIZE;
		break;*/
	default:
		return CTLU_KCR_BUILDER_EXT_LOOKUP_RES_ERR;
		break;
	}


	if (mask) {
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return CTLU_KCR_MASK_OFFSET_ERR;

		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		fecid = fecid | CTLU_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[CTLU_KCR_NFEC] += 1;
	kb->kcr_length += 4 + mask_bytes;

	return CTLU_KCR_SUCCESSFUL_OPERATION;
}


/* This function returns a deep copy of the key composition rule (kcr)
		created using the kcr builder. - OBSOLETE

int32_t ctlu_kcr_builder_get_kcr(struct ctlu_kcr_builder *kb, uint8_t *kcr)
{
	int i;

	/* Deep copy
	for (i = 0; i < CTLU_KCR_MAX_KCR_SIZE; i++)
		kcr[i] = kb->kcr[i];


	return CTLU_KCR_SUCCESSFUL_OPERATION;
}
*/
int32_t ctlu_kcr_builder_add_valid_field_fec(uint8_t mask,
					 struct ctlu_kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	nmsk_moff0 = 0x00; /* indicates 1 bytes mask from offset 0x00 */
	uint8_t	fec_bytes_num = CTLU_KCR_VALID_FIELD_FEC_SIZE;

	if (mask) {
		fec_bytes_num = fec_bytes_num + 2;
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		kb->kcr[curr_byte] = (CTLU_KCR_VF_FECID << 1) |
				     CTLU_KCR_MASK_EXT;
		kb->kcr[curr_byte+1] = 0x0;
		kb->kcr[curr_byte+2] = nmsk_moff0;
		kb->kcr[curr_byte+3] = mask;
	} else {
		if ((curr_byte + fec_bytes_num) > CTLU_KCR_MAX_KCR_SIZE)
			return CTLU_KCR_SIZE_ERR;

		kb->kcr[curr_byte] = CTLU_KCR_VF_FECID << 1;
		kb->kcr[curr_byte+1] = 0x0;
	}
	/* Build the FEC */
	/* Valid field FECID, mask extension indication*/

	/* Update kcr_builder struct */
	kb->kcr[0] += 1;
	kb->kcr_length += fec_bytes_num;

	return CTLU_KCR_SUCCESSFUL_OPERATION;

}


int32_t ctlu_kcr_create(uint8_t *kcr, uint8_t *keyid)
{
	int32_t status;
	uint16_t keyid_pool[SYS_KEYID_POOL_LENGTH];

	status = get_id(keyid_pool, SYS_KEYID_POOL_LENGTH,
		ext_keyid_pool_address, keyid);

	if (status != 0)		/* TODO check status ??? */
		return status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)kcr) << 16, ((uint32_t)(*keyid)) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (!status)
		return CTLU_KCR_CREATE_SUCCESS;
	else
		return CTLU_KCR_CREATE_STATUS_KCR_REPLACED;
}


int32_t ctlu_kcr_replace(uint8_t *kcr, uint8_t keyid)
{
	int32_t status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)kcr) << 16, ((uint32_t)keyid) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (!status) /* In this command status 0 means failure */
		return CTLU_KCR_REPLACE_FAIL_INVALID_KID;
	else
		return
			CTLU_KCR_REPLACE_SUCCESS;
}


int32_t ctlu_kcr_delete(uint8_t keyid)
{
	int32_t status;
	uint16_t keyid_pool[SYS_KEYID_POOL_LENGTH];
	uint8_t fake_kcr = 0;

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)&fake_kcr) << 16, ((uint32_t)keyid) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);

	if (!status)	/* In this command status 0 means failure */
		return CTLU_KCR_DELETE_FAIL_INVALID_KID;

	status = release_id(keyid, keyid_pool, SYS_KEYID_POOL_LENGTH,
		ext_keyid_pool_address);
	/*TODO check status ??? */
	return status;
}


int32_t ctlu_kcr_query(uint8_t keyid, uint8_t *kcr,
					uint8_t *size)
{
	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_COMPOSITION_RULE_QUERY_MTYPE, (uint32_t)kcr,
	       ((uint32_t)keyid) << 16, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	*size = *(((uint8_t *)HWC_ACC_OUT_ADDRESS) + 5);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_gen_key(uint8_t keyid, union ctlu_key *key, uint8_t *key_size)
{

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_KEY_GENERATE_EPRS_MTYPE, ((uint32_t)key),
	       ((uint32_t)keyid) << 16, 0, HWC_ACC_IN_ADDRESS, 0);


	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	*key_size = *(((uint8_t *)HWC_ACC_OUT_ADDRESS) + 5);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}


int32_t ctlu_gen_hash(union ctlu_key *key, uint8_t key_size, uint32_t *hash)
{

	/* Prepare HW context for TLU accelerator call */
	__stqw(CTLU_HASH_GEN_KEY_MTYPE, ((uint32_t)key) << 16,
	       ((uint32_t)key_size) << 16, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	*hash = *(((uint32_t *)HWC_ACC_OUT_ADDRESS) + 1);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

/*****************************************************************************/
/*				DEBUG CODE				     */
/*****************************************************************************/
int32_t ctlu_table_query_debug(uint16_t table_id,
			    struct ctlu_table_params_query_output_message
			    *output)
{
	/* Prepare ACC context for TLU accelerator call */
	__stqw(CTLU_TABLE_QUERY_MTYPE, (uint32_t)output, table_id, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(CTLU_ACCEL_ID);

	/* Return status */
	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

