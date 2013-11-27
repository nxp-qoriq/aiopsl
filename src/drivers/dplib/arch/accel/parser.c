/**************************************************************************//**
@File		parser.c

@Description	This file contains the AIOP SW Parser API implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "dplib/fsl_parser.h"
#include "parser.h"
#include "system.h"
#include "id_pool.h"
#include "general.h"

int32_t parser_profile_create(struct parse_profile_record *parse_profile,
				uint8_t *prpid)
{
	struct parse_profile_create_params parse_profile_create_params;
	int32_t status;
	uint8_t prpid_pool[SYS_PRPID_POOL_LENGTH];

	status = get_id(prpid_pool, SYS_PRPID_POOL_LENGTH,
		ext_prpid_pool_address, prpid);
	if (status != 0)		/*TODO check status ??? */
		return status;

	parse_profile_create_params.parse_profile.reserved1 = 0;
	parse_profile_create_params.parse_profile.reserved2 = 0;

	parse_profile_create_params.reserved1[0] = 0;
	parse_profile_create_params.reserved1[1] = PARSER_PRP_CREATE_MTYPE;
	*(uint16_t *)(parse_profile_create_params.reserved1+2) = 0;
	parse_profile_create_params.prpid = *prpid;
	parse_profile_create_params.reserved2[0] = 0;
	*(uint16_t *)(parse_profile_create_params.reserved2+1) = 0;
	parse_profile_create_params.parse_profile = *parse_profile;

	__stqw(PARSER_PRP_CREATE_MTYPE,
		((uint32_t)&parse_profile_create_params << 16), 0, 0,
		HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

int32_t parser_profile_replace(struct parse_profile_record *parse_profile,
				uint8_t prpid)
{
	struct parse_profile_create_params parse_profile_create_params;

	parse_profile_create_params.parse_profile.reserved1 = 0;
	parse_profile_create_params.parse_profile.reserved2 = 0;

	parse_profile_create_params.reserved1[0] = 0;
	parse_profile_create_params.reserved1[1] = PARSER_PRP_CREATE_MTYPE;
	*(uint16_t *)(parse_profile_create_params.reserved1+2) = 0;
	parse_profile_create_params.prpid = prpid;
	parse_profile_create_params.reserved2[0] = 0;
	*(uint16_t *)(parse_profile_create_params.reserved2+1) = 0;
	parse_profile_create_params.parse_profile = *parse_profile;

	__stqw(PARSER_PRP_CREATE_MTYPE,
		((uint32_t)&parse_profile_create_params << 16), 0, 0,
		HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

int32_t parser_profile_delete(uint8_t prpid)
{

	struct parse_profile_delete_query_params parse_profile_delete_params;
	int32_t status;
	uint8_t prpid_pool[SYS_PRPID_POOL_LENGTH];

	parse_profile_delete_params.reserved1 = 0;
	parse_profile_delete_params.mtype = PARSER_PRP_DELETE_MTYPE;
	parse_profile_delete_params.reserved2 = 0;
	parse_profile_delete_params.prpid = prpid;
	parse_profile_delete_params.reserved3[0] = 0;
	*(uint16_t *)(parse_profile_delete_params.reserved3+1) = 0;

	__stqw(PARSER_PRP_DELETE_MTYPE,
		((uint32_t)&parse_profile_delete_params << 16), 0, 0,
		HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status != 0)
		return status;

	status = release_id(prpid, prpid_pool, SYS_PRPID_POOL_LENGTH,
		ext_prpid_pool_address);
	/*TODO check status ??? */
	return status;
}

int32_t parser_profile_query(uint8_t prpid,
			struct parse_profile_record *parse_profile)
{
	struct parse_profile_delete_query_params parse_profile_query_params;

	parse_profile_query_params.reserved1 = 0;
	parse_profile_query_params.mtype = PARSER_PRP_QUERY_MTYPE;
	parse_profile_query_params.reserved2 = 0;
	parse_profile_query_params.prpid = prpid;
	parse_profile_query_params.reserved3[0] = 0;
	*(uint16_t *)(parse_profile_query_params.reserved3+1) = 0;

	__stqw(PARSER_PRP_QUERY_MTYPE,
		(((uint32_t)&parse_profile_query_params << 16) |
		(uint32_t)parse_profile) , 0, 0, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

int32_t parse_result_generate_default(uint8_t flags)
{
	uint32_t arg1;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* If L4 checsum validation is required,
	 * Check if Gross Running Sum calculation is needed */
	if ((flags & PARSER_VALIDATE_L4_CHECKSUM) & (!pr->gross_running_sum)) {
		if (!fdma_calculate_default_frame_checksum(0, 0xFFFF,
					&pr->gross_running_sum))
			return PARSER_STATUS_FAIL_RUNNING_SUM_FDMA_FAILURE;
	}

	arg1 = (uint32_t)default_task_params.parser_profile_id |
		((uint32_t)flags << 8) |
		((uint32_t)default_task_params.parser_starting_hxs << 13);

	__stqw(PARSER_GEN_PARSE_RES_MTYPE, (uint32_t)HWC_PARSE_RES_ADDRESS, 0,
						arg1, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}

int32_t parse_result_generate(enum parser_starting_hxs_code starting_hxs,
	uint8_t starting_offset, uint8_t flags)
{
	uint32_t arg1;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* If L4 checksum validation is required,
	 * Check if Gross Running Sum calculation is needed */
	if ((flags & PARSER_VALIDATE_L4_CHECKSUM) & (!pr->gross_running_sum)) {
		if (!fdma_calculate_default_frame_checksum(0, 0xFFFF,
					&pr->gross_running_sum))
			return PARSER_STATUS_FAIL_RUNNING_SUM_FDMA_FAILURE;
	}

	arg1 = (uint32_t)default_task_params.parser_profile_id |
		((uint32_t)flags << 8) |
		((uint32_t)starting_hxs << 13) |
		((uint32_t)starting_offset << 24);

	__stqw(PARSER_GEN_PARSE_RES_MTYPE, (uint32_t)HWC_PARSE_RES_ADDRESS, 0,
						arg1, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	return *((int32_t *)HWC_ACC_OUT_ADDRESS);
}
