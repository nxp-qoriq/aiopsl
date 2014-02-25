/**************************************************************************//**
@File		aiop_verification_gso.c

@Description	This file contains the AIOP GSO SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_gso.h"
#include "aiop_verification.h"

extern __TASK struct aiop_default_task_params default_task_params;

uint16_t  aiop_verification_gso(
		uint32_t data_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;

	opcode  = *((uint32_t *) data_addr);

	switch (opcode) {
	case TCP_GSO_CONTEXT_INIT_CMD_STR:
	{
		struct tcp_gso_init_command *str =
			(struct tcp_gso_init_command *)data_addr;
		
		tcp_gso_context_init(str->flags, str->mss, 
				(uint8_t *)(str->gso_ctx_addr));
		
		str_size = (uint16_t)sizeof(struct tcp_gso_init_command);
		break;
	}
	case TCP_GSO_GENERATE_SEG_CMD_STR:
	{
		struct tcp_gso_generate_seg_command *str =
			(struct tcp_gso_generate_seg_command *)data_addr;
		
		str->status = tcp_gso_generate_seg(
				(uint8_t *)(str->gso_ctx_addr));
		/* The segment that was generated is now the default frame of 
		 * the task */
		*((int32_t *)(str->gso_status_addr)) = str->status;
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str->default_task_params = default_task_params;
		
		str_size = (uint16_t)
				sizeof(struct tcp_gso_generate_seg_command);
		break;
	}
	case TCP_GSO_DISCARD_REMAINING_CMD_STR:
	{
		struct tcp_gso_discard_remainder_frame_command *str =
			(struct tcp_gso_discard_remainder_frame_command *)
				data_addr;
		
		str->status = tcp_gso_discard_frame_remainder(
				(uint8_t *)(str->gso_ctx_addr));
		
		*((int32_t *)(str->status_addr)) = str->status;
		str_size = (uint16_t)sizeof(
			struct tcp_gso_discard_remainder_frame_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}








