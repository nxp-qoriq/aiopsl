/**************************************************************************//**
@File		aiop_verification_gro.c

@Description	This file contains the AIOP GRO SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#include "aiop_verification.h"


uint16_t  aiop_verification_gro(uint32_t data_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;

	opcode  = *((uint32_t *) data_addr);
	flags = 0x0;

	switch (opcode) {
	case TCP_GRO_CONTEXT_AGG_SEG_CMD_STR:
	{
		struct tcp_gro_agg_seg_command *str =
			(struct tcp_gro_agg_seg_command *)data_addr;
		
		str->status = tcp_gro_aggregate_seg(
				str->tcp_gro_context_addr, 
				&(str->params),str->flags);
		/* If the aggregated packet was flushed it is now the 
		* default frame of the task */
		*((int32_t *)(str->gro_status_addr)) = str->status;
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = sizeof(struct tcp_gro_agg_seg_command);
		break;
	}
	case TCP_GRO_CONTEXT_FLUSH_AGG_CMD_STR:
	{
		struct tcp_gro_flush_agg_command *str =
			(struct tcp_gro_flush_agg_command *)data_addr;
		
		str->status = tcp_gro_flush_aggregation(
				str->tcp_gro_context_addr);
		/* If the aggregated packet was flushed it is now the 
		* default frame of the task */
		*((int32_t *)(str->gro_status_addr)) = str->status;
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str_size = sizeof(struct tcp_gro_flush_agg_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}








