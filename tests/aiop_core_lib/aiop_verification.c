/**************************************************************************//**
@File		aiop_verification.c

@Description	This file contains the AIOP SW Verification Engine

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_fdma.h"

#include "aiop_verification.h"


struct profile_sram {
	uint64_t ip_secific_sp_info; /**< IP-Specific SP Information 	 */
	uint16_t dl; /**<  DataLength(correction)	 */
	uint16_t reserved; /**< reserved 	 */
	uint16_t dhr; /**< DataHeadRoom(correction) */
	uint8_t  mode_bits1; /**< mode bits 	 */
	uint8_t  mode_bits2; /**< mode bits 	 */
	uint16_t pbs1; /**<  Pool Buffer Size	 */
	uint16_t bpid1; /**<  Bypass Memory Translation	 */
	uint16_t pbs2; /**<  Pool Buffer Size	 */
	uint16_t bpid2; /**<  Bypass Memory Translation	 */
	uint16_t pbs3; /**<  Pool Buffer Size	 */
	uint16_t bpid3; /**<  Bypass Memory Translation	 */
	uint16_t pbs4; /**<  Pool Buffer Size	 */
	uint16_t bpid4; /**<  Bypass Memory Translation	 */
};

	#define mode_bits1_PTAR_MASK  0x80
	#define mode_bits1_SGHR_MASK  0x10
	#define mode_bits1_ASAR_MASK  0x0f

	#define mode_bits2_BS_MASK  0x80
	#define mode_bits2_FF_MASK  0x30
	#define mode_bits2_VA_MASK  0x04
	#define mode_bits2_DLC_MASK 0x01

struct  profile_sram profile_sram1;

void aiop_verification()
{
	/* Presentation Context */
	struct presentation_context *PRC;
	uint32_t asa_seg_addr;	/* ASA Segment Address */
	uint16_t asa_seg_size;  /* ASA Segment Size */
	uint16_t size = 0;	/* ASA incremental read size */
	uint16_t str_size;
	uint32_t opcode;

	/* initialize profile sram */
	#define mode_bits1_PTAR  0x80
	#define mode_bits1_Rsrvd 0x00
	#define mode_bits1_SGHR  0x00
	#define mode_bits1_ASAR  0x0f
	
	#define mode_bits2_BS  0x00
	#define mode_bits2_FF  0x00
	#define mode_bits2_VA  0x00
	#define mode_bits2_DLC 0x00

	
	profile_sram1.ip_secific_sp_info = 0;
	profile_sram1.dl = 0;
	profile_sram1.reserved = 0;
	/* 0x0080 --> 0x8000 (little endian) */
	profile_sram1.dhr = 0x8000;  
	profile_sram1.mode_bits1 = (mode_bits1_PTAR | mode_bits1_SGHR | 
			mode_bits1_ASAR); 
	profile_sram1.mode_bits2 = (mode_bits2_BS | mode_bits2_FF | 
			mode_bits2_VA | mode_bits2_DLC);
	/* buffer size is 1024 bit, so PBS should be 2. 
	 * 0x0081 --> 0x8100 (little endian) */
	profile_sram1.pbs1 = 0x8100;  
	/* BPID=0 */
	profile_sram1.bpid1 = 0x0000; 
	/* buffer size is 1024 bit, so PBS should be 2. 
	 * 0x0081 --> 0x8100 (little endian) */
	profile_sram1.pbs2 = 0x8100;  
	/* BPID=1, 0x0001 --> 0x0100 (little endian) */
	profile_sram1.bpid2 = 0x0100; 
	profile_sram1.pbs3 = 0x0000; 
	profile_sram1.bpid3 = 0x0000;
	profile_sram1.pbs4 = 0x0000; 
	profile_sram1.bpid4 = 0x0000; 

	/* initialize Additional Dequeue Context */
	PRC = (struct presentation_context *) HWC_PRC_ADDRESS;

	/* Initialize ASA variables */
	asa_seg_addr = (uint32_t)(PRC->asapa_asaps & PRC_ASAPA_MASK);
	/* shift size by 6 since the size is in 64bytes (2^6 = 64) quantities */
	asa_seg_size = (PRC->asapa_asaps & PRC_ASAPS_MASK) << 6;

	/* The condition is for back up only.
	In case the ASA was written correctly the Terminate command will
	finish the verification */
	while (size < asa_seg_size) {
		opcode  = *((uint32_t *) asa_seg_addr);

		switch ((opcode & ACCEL_ID_CMD_MASK) >> 16) {

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
