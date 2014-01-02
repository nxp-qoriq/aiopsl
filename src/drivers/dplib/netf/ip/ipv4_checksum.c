/**************************************************************************//**
@File		ipv4_checksum.c

@Description	This file contains the AIOP SW IPv4 Checksum API
		implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "general.h"
#include "net/fsl_net.h"
#include "dplib/fsl_ipv4_checksum.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"


int32_t ipv4_cksum_calculate(struct ipv4hdr *ipv4header)
{

	uint16_t running_sum;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Calculate header size */
	uint16_t ihl = ((uint16_t)((*((uint8_t *)ipv4header) &
				    IPV4_HDR_IHL_MASK) << 2));

	/* Calculate header offset */
	uint16_t offset = ((uint16_t)
				(((uint8_t *)ipv4header)
				 - ((uint8_t *)PRC_GET_SEGMENT_ADDRESS()))
				+ PRC_GET_SEGMENT_OFFSET());

	/* Clear IPv4 checksum field */
	ipv4header->hdr_cksum = 0;

	/* Call FDMA for running sum computation */
	if (FDMA_CHECKSUM_SUCCESS != fdma_calculate_default_frame_checksum
								(offset,
								 ihl,
								 &running_sum)
								 ) {
		return IPV4_CKSUM_CALCULATE_STATUS_FDMA_FAILURE;
	}

	/* Invalidate Parser Result Gross Running Sum field */
	pr->gross_running_sum = 0;

	/* Write 1's complement of the 1's complement 16 bit sum into the
	IPv4 header */
	ipv4header->hdr_cksum = (uint16_t)~running_sum;

	/* TODO replace header if needed */
	
	return IPV4_CKSUM_CALCULATE_STATUS_SUCCESS;
}
