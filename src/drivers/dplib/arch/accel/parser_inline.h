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
@File		parser_inline.h

@Description	This file contains the AIOP SW Parser Inline API implementation.

*//***************************************************************************/

#ifndef __PARSER_INLINE_H
#define __PARSER_INLINE_H


#include "parser.h"
#include "general.h"
#include "common/types.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"

#include "system.h"
#include "id_pool.h"

inline int parse_result_generate(enum parser_starting_hxs_code starting_hxs,
	uint8_t starting_offset, uint8_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser_inline.h",(uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg1, arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	/* 8 Byte aligned for stqw optimization */
	struct parser_input_message_params input_struct
				__attribute__((aligned(16)));
	

	arg1 = (uint32_t)default_task_params.parser_profile_id;
	arg1 = __e_rlwimi(arg1, (uint32_t)flags, 8, 16, 23);
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_hxs, 13, 8, 18);
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_offset, 24, 0, 7);

	/* If Gross Running Sum != 0 then it is valid */
	if (pr->gross_running_sum) {
		input_struct.gross_running_sum = pr->gross_running_sum;
		
		arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;
		
		__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
			arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
	} else {
		/* If L4 checksum validation is required, calculate it first */
		if (flags & PARSER_VALIDATE_L4_CHECKSUM) {
			fdma_calculate_default_frame_checksum(0, 0xFFFF,
					&(input_struct.gross_running_sum));
						
			arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;
		
			__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
		} else {
			/* Gross Running Sum == 0 and validation is not
			 * required */
			__stqw(PARSER_GEN_PARSE_RES_MTYPE,
			(uint32_t)HWC_PARSE_RES_ADDRESS, 0, arg1,
			HWC_ACC_IN_ADDRESS, 0);
		}
	}

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
	if (status == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if ((status ==
			PARSER_HW_STATUS_L3_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L4_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L3_L4_CHECKSUM_VALIDATION_SUCCEEDED)) {
		return 0;
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE,
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

inline int parse_result_generate_default(uint8_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser_inline.h",(uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg1, arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct parser_input_message_params input_struct
					__attribute__((aligned(16)));

	
	arg1 = (uint32_t)default_task_params.parser_profile_id;
	arg1 = __e_rlwimi(arg1, (uint32_t)flags, 8, 16, 23);
	arg1 = __e_rlwimi(arg1,
			(uint32_t)default_task_params.parser_starting_hxs,
			13, 8, 18);

	/* If Gross Running Sum != 0 then it is valid */
	if (pr->gross_running_sum) {
		input_struct.gross_running_sum = pr->gross_running_sum;

		arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;

		__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
	} else {
		/* If L4 checksum validation is required, calculate it first */
		if (flags & PARSER_VALIDATE_L4_CHECKSUM) {

			fdma_calculate_default_frame_checksum(0, 0xFFFF,
					&(input_struct.gross_running_sum));

			arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;
			
			__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
		} else {
			/* Gross Running Sum == 0 and validation is not
			 * required */
			__stqw(PARSER_GEN_PARSE_RES_MTYPE,
			(uint32_t)HWC_PARSE_RES_ADDRESS, 0, arg1,
			HWC_ACC_IN_ADDRESS, 0);
		}
	}
	
	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
	/* implementation of errors is priority based (if-else) since multiple
	 * error indications may appear at the same time */
	if (status == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if ((status ==
			PARSER_HW_STATUS_L3_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L4_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L3_L4_CHECKSUM_VALIDATION_SUCCEEDED)) {
		return 0;
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_DEFAULT,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_DEFAULT,
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

inline int parse_result_generate_basic(void)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser_inline.h",(uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct parser_input_message_params input_struct
					__attribute__((aligned(16)));

	
	input_struct.gross_running_sum = pr->gross_running_sum;

	arg2 = ((uint32_t)(&input_struct) << 16) |
		(uint32_t)HWC_PARSE_RES_ADDRESS;
	__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
		arg2, 0, 0, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);


	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
       if (status == PARSER_HW_STATUS_SUCCESS) {
             return 0;
       } else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_BASIC,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
             return (-1);
       } else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_BASIC,
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


#endif /* __PARSER_INLINE_H */
