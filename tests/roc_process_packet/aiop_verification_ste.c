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
@File          aiop_verification_ste.c

@Description   This file contains the AIOP STE SRs SW Verification
*//***************************************************************************/

#include "fsl_ste.h"

#include "aiop_verification.h"
#include "aiop_verification_ste.h"


uint16_t aiop_verification_ste(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;


	opcode  = *((uint32_t *) asa_seg_addr);


	switch (opcode) {
	/* STE set 4 byte counter Command Verification */
	case STE_SET_4B_CMD_STR:
	{
		struct ste_set_4byte_command *str =
		(struct ste_set_4byte_command *) asa_seg_addr;
		ste_set_32bit_counter(
				str->counter_addr,
				str->value);
		str_size = sizeof(struct ste_set_4byte_command);
		break;
	}

	/* STE set 8 byte counter Command Verification */
	case STE_SET_8B_CMD_STR:
	{
		struct ste_set_8byte_command *str =
		(struct ste_set_8byte_command *) asa_seg_addr;
		ste_set_64bit_counter( str->counter_addr, str->value);
		str_size = sizeof(struct ste_set_8byte_command);
		break;
	}

	/* STE inc counter Command Verification */
	case STE_ADD_CMD_STR:
	{
		struct ste_add_dec_command *str =
			(struct ste_add_dec_command *) asa_seg_addr;
		ste_inc_counter(
				str->counter_addr,
				str->value, str->flags);
		str_size = sizeof(struct ste_add_dec_command);
		break;
	}
	/* STE dec counter Command Verification */
	case STE_DEC_CMD_STR:
	{
		struct ste_add_dec_command *str =
			(struct ste_add_dec_command *) asa_seg_addr;
		ste_dec_counter(
				str->counter_addr,
				str->value, str->flags);
		str_size = sizeof(struct ste_add_dec_command);
		break;
	}

	/* STE inc and acc compound Command Verification */
	case STE_INC_ACC_CMD_STR:
	{
		struct ste_compound_command *str =
			(struct ste_compound_command *) asa_seg_addr;
		ste_inc_and_acc_counters(
				str->counter_addr,
				str->acc_value, str->flags);
		str_size = sizeof(struct ste_compound_command);
		break;
	}
	/* STE inc and sub compound Command Verification */
	case STE_INC_SUB_CMD_STR:
	{
		struct ste_compound_command *str =
			(struct ste_compound_command *) asa_seg_addr;
		ste_inc_and_sub_counters(
				str->counter_addr,
				str->acc_value, str->flags);
		str_size = sizeof(struct ste_compound_command);
		break;
	}
	/* STE dec and acc compound Command Verification */
	case STE_DEC_ACC_CMD_STR:
	{
		struct ste_compound_command *str =
			(struct ste_compound_command *) asa_seg_addr;
		ste_dec_and_acc_counters(
				str->counter_addr,
				str->acc_value, str->flags);
		str_size = sizeof(struct ste_compound_command);
		break;
	}
	/* STE dec and sub compound Command Verification */
	case STE_DEC_SUB_CMD_STR:
	{
		struct ste_compound_command *str =
			(struct ste_compound_command *) asa_seg_addr;
		ste_dec_and_sub_counters(
				str->counter_addr,
				str->acc_value, str->flags);
		str_size = sizeof(struct ste_compound_command);
		break;
	}

	/* STE barrier Command Verification */
	case STE_BARRIER_CMD_STR:
	{
		ste_barrier();
		str_size = sizeof(struct ste_barrier_command);
		break;
	}

#if REV2	
	/* STE Read Errors Command Verification */
	case STE_READ_ERRORS_CMD_STR:
	{
		struct ste_read_error_command *str =
				(struct ste_read_error_command *) asa_seg_addr;
		str->status_reg = STE_GET_STATUS_REGISTER();
		str->capture_attributes = STE_GET_ERR_CAP_ATTRIBUTES();
		str->acc_value = STE_GET_ERR_ACC_VALUE();
		str->counter_msb = STE_GET_ERR_MSB_COUNTER_ADDRESS();
		str->counter_lsb = STE_GET_ERR_LSB_COUNTER_ADDRESS();
		str_size = sizeof(struct ste_read_error_command);
		break;
	}

	/* STE clear Errors Command Verification */
	case STE_CLEAR_ERRORS_CMD_STR:
	{
		STE_CLEAR_CAPTURED_ERROR();
		str_size = sizeof(struct ste_clear_error_command);
		break;
	}
#endif
	
	default:
	{
		return STR_SIZE_ERR;
	}
	}


	return str_size;
}
