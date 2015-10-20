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
@File		parser.c

@Description	This file contains the AIOP SW Parser API implementation.

*//***************************************************************************/
#include "general.h"
#include "fsl_types.h"
#include "fsl_fdma.h"
#include "fsl_parser.h"

#include "parser.h"
#include "system.h"
#include "fsl_id_pool.h"

extern uint64_t ext_prpid_pool_address;

extern __TASK struct aiop_default_task_params default_task_params;

int parser_profile_create(struct parse_profile_input *parse_profile,
				uint8_t *prpid)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser.c", (uint32_t)parse_profile, ALIGNMENT_16B);
#endif
	
	int32_t status;

	status = get_id(ext_prpid_pool_address, prpid);
	if (status != 0)
		return status;

	*((uint64_t *)(parse_profile->reserved)) = 0;
	*((uint16_t *)(parse_profile->reserved)) = PARSER_PRP_CREATE_MTYPE;
	parse_profile->reserved[4] = *prpid;

	__stqw(PARSER_PRP_CREATE_MTYPE,
		(((uint32_t)parse_profile) << 16), 0, 0,
		HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	return 0;
}

void parser_profile_replace(struct parse_profile_input *parse_profile,
				uint8_t prpid)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser.c", (uint32_t)parse_profile, ALIGNMENT_16B);
#endif
	
	*((uint64_t *)(parse_profile->reserved)) = 0;
	*((uint16_t *)(parse_profile->reserved)) = PARSER_PRP_CREATE_MTYPE;
	parse_profile->reserved[4] = prpid;

	__stqw(PARSER_PRP_CREATE_MTYPE,
		(((uint32_t)parse_profile) << 16), 0, 0,
		HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);
}

int parser_profile_delete(uint8_t prpid)
{

	struct parse_profile_delete_query_params parse_profile_delete_params
						__attribute__((aligned(16)));
	int32_t status;

	parse_profile_delete_params.mtype =
			((uint32_t)PARSER_PRP_DELETE_MTYPE) << 16;
	parse_profile_delete_params.prpid = ((uint32_t)prpid) << 24;

	__stqw(PARSER_PRP_DELETE_MTYPE,
		((uint32_t)&parse_profile_delete_params << 16), 0, 0,
		HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = release_id(prpid, ext_prpid_pool_address);
	return status;
}

void parser_profile_query(uint8_t prpid,
			struct parse_profile_input *parse_profile)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser.c", (uint32_t)parse_profile, ALIGNMENT_16B);
#endif
	
	struct parse_profile_delete_query_params parse_profile_query_params
						__attribute__((aligned(16)));

	parse_profile_query_params.mtype =
			((uint32_t)PARSER_PRP_QUERY_MTYPE) << 16;
	parse_profile_query_params.prpid = ((uint32_t)prpid) << 24;

	__stqw(PARSER_PRP_QUERY_MTYPE,
		(((uint32_t)&parse_profile_query_params << 16) |
		(uint32_t)parse_profile) , 0, 0, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);
	

}

int parse_result_generate_checksum(
		enum parser_starting_hxs_code starting_hxs,
		uint8_t starting_offset, uint16_t *l3_checksum,
		uint16_t *l4_checksum)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser.c", (uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg1, arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct parser_input_message_params input_struct
					__attribute__((aligned(16)));

	
	/* Check if Gross Running Sum calculation is needed */
	if (!pr->gross_running_sum) {
		fdma_calculate_default_frame_checksum(0, 0xFFFF,
					      &input_struct.gross_running_sum);
	} else {
		input_struct.gross_running_sum = pr->gross_running_sum;
	}

	arg1 = (uint32_t)default_task_params.parser_profile_id;
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_hxs, 13, 8, 18);
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_offset, 24, 0, 7);

	arg2 = ((uint32_t)(&input_struct) << 16);

	__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE), arg2, 0, arg1,
						HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
	if (status == PARSER_HW_STATUS_L3_L4_CHECKSUM_GENERATION_SUCCEEDED) {
		*l3_checksum = *((uint16_t *)HWC_ACC_OUT_ADDRESS2);
		*l4_checksum = *((uint16_t *)(HWC_ACC_OUT_ADDRESS2+2));
		return 0;
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_CHECKSUM,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_CHECKSUM,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
		return (-1);
	} else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
		return -ENOSPC;
	} else {
		return -EIO;
	}
}


#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

void parser_exception_handler(enum parser_function_identifier func_id,
		     uint32_t line,
		     int32_t status)
{
	char *func_name;
	char *err_msg;
	
	/* Translate function ID to function name string */
	switch(func_id) {
	case PARSE_RESULT_GENERATE_DEFAULT:
		func_name = "parse_result_generate_default";
		break;
	case PARSE_RESULT_GENERATE:
		func_name = "parse_result_generate";
		break;
	case PARSE_RESULT_GENERATE_CHECKSUM:
		func_name = "parse_result_generate_checksum";
		break;
	case PARSE_RESULT_GENERATE_BASIC:
		func_name = "parse_result_generate_basic";
		break;
	default:
		/* create own exception */
		func_name = "Unknown Function";
	}
	
	/* Translate error ID to error name string */
	if (status == PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		err_msg = "Parser SR failed due to Cycle limit exceeded.\n";
	} else if (status == PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		err_msg = "Parser SR failed due to invalid soft parse"
				"instruction.\n";
	} else {
		err_msg = "Unknown or Invalid status.\n";
	}
	
	exception_handler(__FILE__, func_name, line, err_msg);
}

#pragma pop


