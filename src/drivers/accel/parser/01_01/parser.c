/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

	parse_profile->parse_profile.reserved1 = 0;
	parse_profile->parse_profile.reserved2 = 0;

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
	
	parse_profile->parse_profile.reserved1 = 0;
	parse_profile->parse_profile.reserved2 = 0;

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
	
	/* clear 8 first bytes (which include MTYPE and PRPID which are
	 * irrelevant for the user) */ 
	parse_profile->parse_profile.reserved1 = 0;
	parse_profile->parse_profile.reserved2 = 0;

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


