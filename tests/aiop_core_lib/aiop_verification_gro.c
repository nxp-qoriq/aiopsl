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

	opcode  = *((uint32_t *) data_addr);

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
		str_size = (uint16_t)sizeof(struct tcp_gro_agg_seg_command);
		
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
		str_size = (uint16_t)sizeof(struct tcp_gro_flush_agg_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}
	
	return str_size;
}

void gro_verif_create_next_frame()
{
	struct tcphdr *tcp;
	uint32_t sequence_number;
	uint16_t headers_size, seg_size;
	uint8_t  data_offset;
	
	tcp = ((struct tcphdr *)PARSER_GET_L4_POINTER_DEFAULT());
	sequence_number = tcp->sequence_number;
	fdma_present_default_frame();
	
	/* calculate segment size + data offset + headers size*/
	seg_size = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	data_offset = (tcp->data_offset_reserved & 
				NET_HDR_FLD_TCP_DATA_OFFSET_MASK) >> 
				(NET_HDR_FLD_TCP_DATA_OFFSET_OFFSET - 
				 NET_HDR_FLD_TCP_DATA_OFFSET_SHIFT_VALUE);
	headers_size = (uint16_t)(PARSER_GET_L4_OFFSET_DEFAULT() + data_offset);
	tcp->sequence_number = sequence_number + seg_size - headers_size;
}





