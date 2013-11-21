/**************************************************************************//**
@File		l4_checksum.c

@Description	This file contains the AIOP SW Layer 4 Checksum API
		implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "fsl_l4_checksum.h"
// todo fix
#include "fsl_ipv4_checksum.h"
#include "fsl_parser.h"

int32_t cksum_calc_udp_tcp_checksum()
{
	uint16_t	l4checksum;
	struct tcphdr	*tcph;
	struct udphdr	*udph;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Check if TCP */
	if (PARSER_IS_TCP_DEFAULT()) {
		/* Check if Gross Running Sum calculation is needed */
		if (!pr->gross_running_sum) {
			if (!fdma_calculate_default_frame_checksum(0, 0xFFFF,
						&pr->gross_running_sum)) {
				return
				L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_FDMA_FAILURE;
			}
		}

		/* Call parser */
		if (PARSER_STATUS_PASS !=
		    parse_result_generate_default(0)) {
			return
			L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_PARSER_FAILURE;
		}
		/* Point to the TCP header */
		tcph = (struct tcphdr *)(PRC_GET_SEGMENT_ADDRESS() +
					 PARSER_GET_L4_OFFSET_DEFAULT());

		/* Subtract checksum field from parser running sum */
		l4checksum = cksum_ones_complement_sum16(pr->running_sum,
						(uint16_t)~(tcph->checksum));

		/* Invert checksum */
		l4checksum = (uint16_t)~l4checksum;

		/* Invalidate Parser Result Gross Running Sum field */
		pr->gross_running_sum = 0;

		/* Write checksum to TCP header */
		tcph->checksum = l4checksum;
	} /* TCP */

	/* If not, Check if UDP */
	else if (PARSER_IS_UDP_DEFAULT()) {
		/* Check if Gross Running Sum calculation is needed */
		if (!pr->gross_running_sum) {
			if (!fdma_calculate_default_frame_checksum(0, 0xFFFF,
						&pr->gross_running_sum)) {
				return
				L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_FDMA_FAILURE;
			}
		}

		/* Call parser */
		if (PARSER_STATUS_PASS !=
		    parse_result_generate_default(0)) {
			return 
			L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_PARSER_FAILURE;
		}

		/* Point to the UDP header */
		udph = (struct udphdr *)(PRC_GET_SEGMENT_ADDRESS() +
					 PARSER_GET_L4_OFFSET_DEFAULT());

		/* Subtract checksum field from parser running sum */
		l4checksum = cksum_ones_complement_sum16(pr->running_sum,
						(uint16_t)~(udph->checksum));

		/* Invert Checksum if needed */
		if (l4checksum != 0xFFFF)
			l4checksum = (uint16_t)~l4checksum;

		/* Invalidate Parser Result Gross Running Sum field */
		pr->gross_running_sum = 0;

		/* Write checksum to UDP header */
		udph->checksum = (uint16_t)l4checksum;
	} /* UDP */

	/* If not UDP/TCP frame, return an indication */
	else {
		return L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_NON_UDP_TCP;
	}

	return L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_SUCCESS;
}

