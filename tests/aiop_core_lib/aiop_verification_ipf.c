/**************************************************************************//**
@File		aiop_verification_ipf.c

@Description	This file contains the AIOP IPF SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#include "aiop_verification.h"

extern __TASK struct aiop_default_task_params default_task_params;
//extern __TASK ipf_ctx_t ipf_context_addr;
//extern __TASK int32_t status;
//extern __TASK int32_t status_ipf;

uint16_t  aiop_verification_ipf(
		uint32_t data_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;

	opcode  = *((uint32_t *) data_addr);
	
	switch (opcode) {
	/* IPF Init Context command */
	case IPF_CONTEXT_INIT_CMD :
	{
		struct ipf_init_command *str =
			(struct ipf_init_command *)data_addr;
		
		ipf_context_init(str->flags, str->mtu,
				(uint8_t *)(str->ipf_ctx_addr));
		str_size = sizeof(struct ipf_init_command);
		break;
	}
	/* IPF Generate Fragment Command */
	case IPF_GENERATE_FRAG_CMD:
	{
		struct ipf_generate_frag_command *str =
			(struct ipf_generate_frag_command *)data_addr;
		
		str->status = ipf_generate_frag(
				(uint8_t *)(str->ipf_ctx_addr));
		/* The fragment that was generated is now the default frame of 
		 * the task */
		*((int32_t *)(str->ipf_status_addr)) = str->status;
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str->default_task_params = default_task_params;
		
		str_size = sizeof(struct ipf_generate_frag_command);
		break;
	}
	/* IPF Discard Remaining Frame Command */
	case IPF_DISCARD_REMAINING_FRAME_CMD:
	{
		struct ipf_discard_remainder_frame_command *str =
			(struct ipf_discard_remainder_frame_command *)
				data_addr;
		
		str->status = ipf_discard_frame_remainder(
				(uint8_t *)(str->ipf_ctx_addr));
		*((int32_t *)(str->status_addr)) = str->status;
		
		str_size = sizeof(struct ipf_discard_remainder_frame_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}








