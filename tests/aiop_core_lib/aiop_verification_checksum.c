/**************************************************************************//**
@File		aiop_verification_checksum.c

@Description	This file contains the AIOP Checksum SRs SW Verification

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "net/fsl_net.h"
#include "dplib/fsl_ipv4_checksum.h"
#include "dplib/fsl_l4_checksum.h"

#include "aiop_verification.h"
#include "aiop_verification_checksum.h"



uint16_t aiop_verification_checksum(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	

	opcode  = *((uint32_t *) asa_seg_addr);


	switch (opcode) {

	case CKSUM_CALC_IPV4_HEADER_CHECKSUM_STR:
	{
		struct cksum_calc_ipv4_header_checksum_command *cmd =
			(struct cksum_calc_ipv4_header_checksum_command *)
			asa_seg_addr;
		cmd->status = ipv4_cksum_calculate(
				(struct ipv4hdr *)cmd->ipv4header,
				cmd->options);
		str_size =
		   sizeof(struct cksum_calc_ipv4_header_checksum_command);
		break;
	}
	case CKSUM_CALC_UDP_TCP_CHECKSUM_STR:
	{
		struct cksum_calc_udp_tcp_checksum_command *cmd =
				(struct cksum_calc_udp_tcp_checksum_command *)
				asa_seg_addr;
		cmd->status = cksum_calc_udp_tcp_checksum(cmd->options);
		str_size = sizeof(struct cksum_calc_udp_tcp_checksum_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}

