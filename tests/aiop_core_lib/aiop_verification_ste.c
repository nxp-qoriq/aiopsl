/**************************************************************************//**
@File          aiop_verification_ste.c

@Description   This file contains the AIOP STE SRs SW Verification
*//***************************************************************************/

#include "dplib/fsl_ste.h"

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

	default:
	{
		return STR_SIZE_ERR;
	}
	}


	return str_size;
}
