/**************************************************************************//**
@File		aiop_verification.c

@Description	This file contains the AIOP SW Verification Engine

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_fdma.h"
#include "aiop_verification.h"


void aiop_verification_fm_temp()
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
	uint8_t  ipr_iteration = 0;


	/* initialize Additional Dequeue Context */
	PRC = (struct presentation_context *) HWC_PRC_ADDRESS;

	/* Initialize ASA variables */
	asa_seg_addr = (uint32_t)(PRC->asapa_asaps & PRC_ASAPA_MASK);
	init_asa_seg_addr = asa_seg_addr;
	/* shift size by 6 since the size is in 64bytes (2^6 = 64) quantities */
	asa_seg_size = (PRC->asapa_asaps & PRC_ASAPS_MASK) << 6;

	init_verif();

	/* The condition is for back up only.
	In case the ASA was written correctly the Terminate command will
	finish the verification */
	while (size < asa_seg_size) {
		opcode  = *((uint32_t *) asa_seg_addr);
		opcode = (opcode & ACCEL_ID_CMD_MASK) >> 16;

		switch (opcode) {

		case GRO_MODULE:
		{
			str_size = aiop_verification_gro(asa_seg_addr);
			if (str_size == sizeof(struct tcp_gro_agg_seg_command))
				gro_verif_create_next_frame(++gro_iteration);
			break;
		}
		case IPF_MODULE:
		{
			str_size = aiop_verification_ipf(asa_seg_addr);
			break;
		}
		case GSO_MODULE:
		{
			str_size = aiop_verification_gso(asa_seg_addr);
			break;
		}
		case IPR_MODULE:
		{
			ipr_verif_update_frame(ipr_iteration);
			str_size = aiop_verification_ipr(asa_seg_addr);
			ipr_iteration ++;
			break;
		}
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
		case CTLU_MODULE:
		{
			str_size = aiop_verification_ctlu(asa_seg_addr);
			break;
		}
		case CTLU_PARSE_CLASSIFY_MODULE:
		{
			str_size = aiop_verification_parser(asa_seg_addr);
			break;
		}
		case HM_MODULE:
		{
			str_size = aiop_verification_hm(asa_seg_addr);
			break;
		}
		case VPOOL_MODULE:
		{
			str_size = verification_virtual_pools(asa_seg_addr);
			break;
		}
		case IF_MODULE:
		{
			struct aiop_if_verif_command *str =
				(struct aiop_if_verif_command *)asa_seg_addr;
			uint32_t if_result;

			if_result = if_statement_result(
					str->compared_variable_addr,
					str->compared_value, str->cond);

			if (if_result == AIOP_TERMINATE_FLOW_CMD)
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
					str->compared_value, str->cond);

			if (if_result == AIOP_TERMINATE_FLOW_CMD)
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
		case TERMINATE_FLOW_MODULE:
		default:
		{
			fdma_terminate_task();
			return;
		}
		}


		if ((opcode == AIOP_IF_CMD) || (opcode == AIOP_IF_ELSE_CMD))
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
