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
@File		aiop_verification.c

@Description	This file contains the AIOP SW Verification Engine

*//***************************************************************************/

#include "fsl_fdma.h"

#include "aiop_verification.h"

extern __VERIF_TLS uint32_t fatal_fqid;
extern __VERIF_TLS uint32_t sr_fm_flags;

void aiop_verification_sr()
{
	init_verif();
	aiop_verification_parse();
}

void aiop_verification_parse()
{
	/* Presentation Context */
	struct presentation_context *PRC;
	uint32_t asa_seg_addr;	/* ASA Segment Address */
	uint32_t init_asa_seg_addr;	/* Initial ASA Segment Address */
	uint16_t asa_seg_size;  /* ASA Segment Size */
	uint16_t size = 0;	/* ASA incremental read size */
	uint16_t str_size;
	uint32_t opcode;
	uint8_t gro_iteration = 0;
	uint8_t ipr_iteration = 0;


	/* initialize Additional Dequeue Context */
	PRC = (struct presentation_context *) HWC_PRC_ADDRESS;

	/* Initialize ASA variables */
	asa_seg_addr = (uint32_t)(PRC->asapa_asaps & PRC_ASAPA_MASK);
	init_asa_seg_addr = asa_seg_addr;
	/* shift size by 6 since the size is in 64bytes (2^6 = 64) quantities */
	asa_seg_size = (PRC->asapa_asaps & PRC_ASAPS_MASK) << 6;

	/* The condition is for back up only.
	In case the ASA was written correctly the Terminate command will
	finish the verification */
	while (size < asa_seg_size) {
		opcode  = *((uint32_t *) asa_seg_addr);
		opcode = (opcode & ACCEL_ID_CMD_MASK) >> 16;

		switch (opcode) {

		case FDMA_MODULE:
		{
			str_size = aiop_verification_fdma(asa_seg_addr);
			break;
		}
		case TMAN_MODULE:
		{
			str_size = aiop_verification_tman(asa_seg_addr);
			break;
		}
		case STE_MODULE:
		{
			str_size = aiop_verification_ste(asa_seg_addr);
			break;
		}
		case CDMA_MODULE:
		{
			str_size = aiop_verification_cdma(asa_seg_addr);
			break;
		}
		case TABLE_MODULE:
		{
			str_size = aiop_verification_table(asa_seg_addr);
			break;
		}
		case KEYGEN_MODULE:
		{
			str_size = aiop_verification_keygen(asa_seg_addr);
			break;
		}
		case PARSE_MODULE:
		{
			str_size = aiop_verification_parser(asa_seg_addr);
			break;
		}
		case HM_MODULE:
		{
			str_size = aiop_verification_hm(asa_seg_addr);
			break;
		}
#ifndef CDC_ROC		
		case VPOOL_MODULE:
		{
			str_size = verification_virtual_pools(asa_seg_addr);
			break;
		}
#endif
		case OSM_MODULE:
		{
			str_size = aiop_verification_osm(asa_seg_addr);
			break;
		}
		case WRITE_DATA_TO_WS_MODULE:
		{
			struct write_data_to_workspace_command *str =
				(struct write_data_to_workspace_command *)
					asa_seg_addr;
			uint16_t i;
			uint8_t *address = (uint8_t *)(str->ws_dst_rs);
			for (i = 0; i < str->size; i++)
				*address++ = str->data[i%32];

			str->status = 0;
			str_size = (uint16_t)
				sizeof(struct write_data_to_workspace_command);
			break;
		}
		case IF_MODULE:
		{
			struct aiop_if_verif_command *str =
				(struct aiop_if_verif_command *)asa_seg_addr;
			uint32_t if_result;

			if_result = if_statement_result(
					str->compared_variable_addr,
					str->compared_size,
					str->compared_value, str->cond);

			if (if_result == TERMINATE_FLOW_MODULE)
			{
				fdma_terminate_task();
				return;
			}
			else if (if_result)
				asa_seg_addr = init_asa_seg_addr +
							str->true_cmd_offset;
			else	/* jump to the next sequential command */
				asa_seg_addr = asa_seg_addr +
					sizeof(struct aiop_if_verif_command);

			break;
		}
		case IF_ELSE_MODULE:
		{
			struct aiop_if_else_verif_command *str =
				(struct aiop_if_else_verif_command *)
							asa_seg_addr;
			uint32_t if_result;

			if_result = if_statement_result(
					str->compared_variable_addr,
					str->compared_size,
					str->compared_value, str->cond);

			if (if_result == TERMINATE_FLOW_MODULE)
			{
				fdma_terminate_task();
				return;
			}
			else if (if_result)
				asa_seg_addr = init_asa_seg_addr +
							str->true_cmd_offset;
			else
				asa_seg_addr = init_asa_seg_addr +
							str->false_cmd_offset;

			break;
		}
		case EXCEPTION_MODULE:
		{
			struct fatal_error_command *str =
			    (struct fatal_error_command *)
						asa_seg_addr;
			fatal_fqid = str->fqid;
			sr_fm_flags = str->flags;
			str_size = (uint16_t)
			    sizeof
			     (struct fatal_error_command);
			break;
		}
		case UPDATE_ASA_VARIABLE:
		{
			struct update_asa_variable_command *str =
				(struct update_asa_variable_command *)
						asa_seg_addr;
			*((uint16_t *)(init_asa_seg_addr + str->asa_offset)) +=
					str->value;
			str_size = (uint16_t)
			       sizeof(struct update_asa_variable_command);
			break;
		}
		case TERMINATE_FLOW_MODULE:
		default:
		{
			fdma_terminate_task();
			return;
		}
		}


		if ((opcode == IF_MODULE) || (opcode == IF_ELSE_MODULE))
		{
			size = (uint16_t)(asa_seg_addr - init_asa_seg_addr);
		}
		else
		{
			if (str_size == STR_SIZE_ERR) {
				fdma_terminate_task();
				return;
			}
			asa_seg_addr += str_size;
			size += str_size;
		}
	}

	fdma_terminate_task();

	return;
}
