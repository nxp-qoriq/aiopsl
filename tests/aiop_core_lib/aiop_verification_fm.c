/**************************************************************************//**
@File		aiop_verification.c

@Description	This file contains the AIOP SW Verification Engine

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_fdma.h"

#include "aiop_verification.h"


void aiop_verification_fm()
{
	tcp_gso_ctx_t tcp_gso_context_addr;
	uint16_t data_addr;	/* Data Address in workspace*/
	uint16_t data_size;  	/* Data Size */
	uint16_t rem_data_size;	/* Remaining Data Size */
	uint16_t size = 0;	/* Data incremental read size */
	uint64_t ext_address;	/* External Data Address */
	uint16_t str_size;
	uint32_t opcode;
	uint32_t flags;

	/* Todo - read last 8 bytes from frame */

	/* Todo - read a new buffer from DDR with DATA_SIZE */
	data_size = DATA_SIZE;
	/* Todo - initialize parameters */
	
	rem_data_size = data_size;
	/* The condition is for back up only.
	In case the ASA was written correctly the Terminate command will
	finish the verification */
	while (size < data_size) {
		opcode  = *((uint32_t *) data_addr);
		flags = 0x0;

		switch ((opcode & ACCEL_ID_CMD_MASK) >> 16) {

		case TCP_GSO_MODULE_STATUS_ID:
		{
			str_size = aiop_verification_gso(tcp_gso_context_addr,
					data_addr, rem_data_size);
			break;
		}
		case CTLU_PARSE_CLASSIFY_ACCEL_ID:
		{
			str_size = aiop_verification_parser(data_addr);
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
		if (str_size == STR_SIZE_BIG){
			/* read a new buffer from DDR with DATA_SIZE */
			/* Todo -  if CDMA fails ?? perhaps write tghe error to 
			 * the last 8 bytes in the frame*/
			//ext_address = ??
			data_addr = WS_DATA_ADDR;
			data_size = DATA_SIZE;
			//cdma_read((void *)data_addr, ext_address, data_size);
			
			rem_data_size = data_size;
			size = 0;
		}
		else{
			data_addr += str_size;
			rem_data_size -= str_size;
			size += str_size;
		}
	}

	fdma_terminate_task();

	return;
}
