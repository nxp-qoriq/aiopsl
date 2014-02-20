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
	/* shift size by 6 since the size is in 64bytes (2^6 = 64) quantities */
	asa_seg_size = (PRC->asapa_asaps & PRC_ASAPS_MASK) << 6;

	init_verif();
	
	/* The condition is for back up only.
	In case the ASA was written correctly the Terminate command will
	finish the verification */
	while (size < asa_seg_size) {
		opcode  = *((uint32_t *) asa_seg_addr);

		switch ((opcode & ACCEL_ID_CMD_MASK) >> 16) {

		case GRO_FM_ID:
		{
			str_size = aiop_verification_gro(asa_seg_addr);	
			if (str_size == sizeof(struct tcp_gro_agg_seg_command))
				gro_verif_create_next_frame(++gro_iteration);
			break;
		}
		case IPF_FM_ID:
		{
			str_size = aiop_verification_ipf(asa_seg_addr);
			break;
		}
		case (TCP_GSO_MODULE_STATUS_ID >> 16):
		{
			str_size = aiop_verification_gso(asa_seg_addr);
			break;
		}
		case IPR_VERIF_FM_ID:
		{
			ipr_verif_update_frame(ipr_iteration);
			str_size = aiop_verification_ipr(asa_seg_addr);
			ipr_iteration ++;
			break;
		}

		case FPDMA_ACCEL_ID:
		case FODMA_ACCEL_ID:
		{
			str_size = aiop_verification_fdma(asa_seg_addr);
			break;
		}
		case TMAN_ACCEL_ID:
		{
			str_size = aiop_verification_tman(asa_seg_addr);
			break;
		}
		case STE_VERIF_ACCEL_ID:
		{
			str_size = aiop_verification_ste(asa_seg_addr);
			break;
		}
		case CDMA_ACCEL_ID:
		{
			str_size = aiop_verification_cdma(asa_seg_addr);
			break;
		}
		case CTLU_ACCEL_ID:
		{
			str_size = aiop_verification_ctlu(asa_seg_addr);
			break;
		}
		case CTLU_PARSE_CLASSIFY_ACCEL_ID:
		{
			str_size = aiop_verification_parser(asa_seg_addr);
			break;
		}
		case HM_VERIF_ACCEL_ID:
		{
			str_size = aiop_verification_hm(asa_seg_addr);
			break;
		}
		case VPOOL_ACCEL_ID:
		{
			str_size = verification_virtual_pools(asa_seg_addr);
			break;
		}
		case AIOP_TERMINATE_FLOW_CMD:
		default:
		{
			fdma_terminate_task();
			return;
		}
		}

		if (str_size == STR_SIZE_ERR) {
			fdma_terminate_task();
			return;
		}
		asa_seg_addr += str_size;
		size += str_size;
	}

	fdma_terminate_task();

	return;
}
