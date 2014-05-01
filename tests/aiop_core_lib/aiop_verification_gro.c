/**************************************************************************//**
@File		aiop_verification_gro.c

@Description	This file contains the AIOP GRO SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#include "aiop_verification.h"
#include "dplib/fsl_cdma.h"

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

		str->params.timeout_params.tmi_id =
				*((uint8_t *)str->tmi_id_addr);
		str->params.timeout_params.gro_timeout_cb =
				&gro_timeout_cb_verif;

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

void gro_verif_create_next_frame(uint8_t gro_iteration)
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

	switch (gro_iteration)
	{
	case 1 :
	{
		tcp->sequence_number = sequence_number + seg_size -
				headers_size;
		break;
	}
	default:
	{
		tcp->sequence_number = sequence_number + seg_size -
						headers_size;
	}
	}


}

void gro_timeout_cb_verif(uint64_t arg)
{
	struct fdma_enqueue_wf_command str;
	struct fdma_queueing_destination_params qdp;
	int32_t status;
	uint32_t flags = 0;

	if (arg == 0)
		return;
	status = cdma_read((void *)&str,
			arg,
			(uint16_t)sizeof(struct fdma_enqueue_wf_command));
	if (status != CDMA_READ__SUCCESS)
		fdma_terminate_task();

	*(uint8_t *) HWC_SPID_ADDRESS = str.spid;
	flags |= ((str.TC == 1) ? (FDMA_EN_TC_TERM_BITS) :
	((str.TC == 2) ? (FDMA_EN_TC_CONDTERM_BITS) : 0x0));
	flags |= ((str.PS) ? FDMA_ENWF_PS_BIT : 0x0);

	if (str.EIS) {
		str.status = (int8_t)
			fdma_store_and_enqueue_default_frame_fqid(
				str.qd_fqid, flags);
	} else{
		qdp.qd = (uint16_t)(str.qd_fqid);
		qdp.qdbin = str.qdbin;
		qdp.qd_priority = str.qd_priority;
		str.status = (int8_t)
			fdma_store_and_enqueue_default_frame_qd(
					&qdp, flags);
	}
	fdma_terminate_task();
}





