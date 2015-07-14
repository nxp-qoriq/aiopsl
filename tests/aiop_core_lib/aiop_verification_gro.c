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
@File		aiop_verification_gro.c

@Description	This file contains the AIOP GRO SW Verification Structures.

*//***************************************************************************/


#include "aiop_verification.h"
#include "fsl_cdma.h"

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
				&timeout_cb_verif;

		str->status = tcp_gro_aggregate_seg(
				str->tcp_gro_context_addr,
				&(str->params),str->flags);
		/* If the aggregated packet was flushed it is now the
		* default frame of the task */
		*((int32_t *)(str->gro_status_addr)) = str->status;
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





