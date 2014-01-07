/**************************************************************************//**
@File		aiop_verification.c

@Description	This file contains the AIOP SW Verification Engine

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_fdma.h"

#include "aiop_verification.h"
#include "dplib/fsl_cdma.h"

void aiop_verification_fm()
{
	tcp_gso_ctx_t tcp_gso_context_addr;
	ipf_ctx_t ipf_context_addr; /* intenral struct for IPF */
	uint8_t data_addr[DATA_SIZE];	/* Data Address in workspace*/
	uint64_t ext_address;	/* External Data Address */
	uint16_t str_size;	/* Command struct Size */
	uint32_t opcode;
	uint32_t flags;
	uint8_t seg_handle;
	uint16_t seg_length;

	/* Read last 8 bytes from frame */
	fdma_present_default_frame_segment(
			FDMA_PRES_SR_BIT, (void *)&ext_address, 0, 8, 
			&seg_length, &seg_handle);
	
	/* The Terminate command will finish the verification */
	do
	{
		/* Read a new buffer from DDR with DATA_SIZE */
		cdma_read((void *)data_addr, ext_address, (uint16_t)DATA_SIZE);
		
		opcode  = *((uint32_t *) data_addr);
		flags = 0x0;

		switch ((opcode & ACCEL_ID_CMD_MASK) >> 16) {

		case TCP_GSO_MODULE_STATUS_ID:
		{
			str_size = aiop_verification_gso(tcp_gso_context_addr,
					(uint32_t)data_addr);
			break;
		}
		case IPF_MODULE_STATUS_ID:
		{
			str_size = aiop_verification_ipf(
					ipf_context_addr,
					(uint32_t)data_addr);
		}
		case CTLU_PARSE_CLASSIFY_ACCEL_ID:
		{
			str_size = aiop_verification_parser(
					(uint32_t)data_addr);
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
		else{
			/* write command results back to DDR */
			cdma_write(ext_address, (void *)data_addr, str_size);
			/* Read next command from DDR */
			ext_address += str_size;
			//cdma_read((void *)data_addr, ext_address, DATA_SIZE);
		}
	} while (1);

	fdma_terminate_task();

	return;
}
