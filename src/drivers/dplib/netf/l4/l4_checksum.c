/**************************************************************************//**
@File		l4_checksum.c

@Description	This file contains the AIOP SW Layer 4 Checksum API
		implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "general.h"
#include "net/fsl_net.h"
#include "dplib/fsl_l4_checksum.h"
#include "dplib/fsl_parser.h"
#include "parser.h"
#include "dplib/fsl_fdma.h"

extern __TASK struct aiop_default_task_params default_task_params;

int32_t cksum_calc_udp_tcp_checksum(uint32_t flags)
{
	uint16_t	l3checksum_dummy;
	uint16_t	l4checksum;
	struct tcphdr	*tcph;
	struct udphdr	*udph;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Check if TCP or UDP*/
	if (!PARSER_IS_TCP_OR_UDP_DEFAULT())
		return L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_NON_UDP_TCP;

	/* Check if Gross Running Sum calculation is needed */
	if (!pr->gross_running_sum) {
		if (FDMA_CHECKSUM_SUCCESS !=
		    fdma_calculate_default_frame_checksum(0, 0xFFFF,
						&pr->gross_running_sum)) {
			return L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_FDMA_FAILURE;
		}
	}

	/* Call parser */
	if (PARSER_STATUS_PASS != parse_result_generate_checksum(
	    (enum parser_starting_hxs_code)
	    default_task_params.parser_starting_hxs, 0, &l3checksum_dummy,
	    &l4checksum)) {
		return
		L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_PARSER_FAILURE;
	}

	if (PARSER_IS_TCP_DEFAULT()) {
		/* Point to the TCP header */
		tcph = (struct tcphdr *)(PRC_GET_SEGMENT_ADDRESS() +
					 PARSER_GET_L4_OFFSET_DEFAULT());

		/* Invalidate Parser Result Gross Running Sum field */
		pr->gross_running_sum = 0;

		/* Write checksum to TCP header */
		tcph->checksum = l4checksum;
	} /* TCP */
	else {
		/* Point to the UDP header */
		udph = (struct udphdr *)(PRC_GET_SEGMENT_ADDRESS() +
			 PARSER_GET_L4_OFFSET_DEFAULT());

		/* Invalidate Parser Result Gross Running Sum field */
		pr->gross_running_sum = 0;

		/* Write checksum to UDP header */
		udph->checksum = l4checksum;
	} /* UDP */

	/* TODO replace header if needed */

	return L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_SUCCESS;
}

