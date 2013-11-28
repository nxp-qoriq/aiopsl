/**************************************************************************//**
@File		ipf.c

@Description	This file contains the AIOP SW IP Fragmentation implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "fsl_ipf.h"
#include "ipf.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"

int32_t ipf_generate_frag(ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;

	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	/*struct adc * adc = (struct adc *) HWC_ADC_ADDRESS; */
	struct ipv4hdr *ipv4_hdr =
		(struct ipv4hdr *) pr->ip1_or_arp_offset +
		PRC_GET_SEGMENT_ADDRESS();
	uint8_t *ipv4_opt;	/* points to IPv4 options */
	uint8_t *ipv4_hdr_end;	/* points to IPv4 header end */
	int more_opt;	/* Indicates that there are more options to process */
	int opt_len;		/* Single option length */
	int ip_header_len;	/* Length in octates */
	uint8_t first_frag = ipf_ctx->first_frag; /* first fragment flag */
	int i;

	/* A LOT OF THINGS TO DO HERE */

	ip_header_len = ((ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK) >>
						IPV4_HDR_IHL_OFFSET) << 2;
	more_opt = ip_header_len > IPV4_HDR_LENGTH;
	if (more_opt) {
		ipv4_hdr_end = ((uint8_t *)ipv4_hdr) + ip_header_len;
		ipv4_opt = ((uint8_t *)ipv4_hdr) + IPV4_HDR_LENGTH;
	}
	/* Options Handling  - no change is made to the IP options length */
	/* !! THIS CODE DOES NOT PROTECT AGAINST MALICIOUS/MALFORMED OPTIONS !!
	 * (e.g. wrong length field in options */
	while (more_opt) {

		switch (*ipv4_opt) {

		case IPV4_EOOL_OPTION_TYPE:
			more_opt = FALSE;
			break;

		case IPV4_NOP_OPTION_TYPE:
			ipv4_opt++;
			break;

		case IPV4_RECORD_ROUTE_OPTION_TYPE:
			/* Preserve first fragment only */
			if (!first_frag) {
				/* fill in 7 NOPs instead */
				*((uint32_t *)ipv4_opt) = 0x01010101;
				ipv4_opt += 4;
				*((uint16_t *)ipv4_opt) = 0x0101;
				ipv4_opt += 2;
				*((uint8_t *)ipv4_opt) = 0x01;
				ipv4_opt += 1;
			} else {
				ipv4_opt += 7;
			}
			break;

		case IPV4_TIMESTAMP_OPTION_TYPE:
			/* Preserve first fragment only */
			if (!first_frag) {
				/* fill in NOPs instead*/
				opt_len = *(ipv4_opt + 1);
				for (i = 0; i < opt_len; i++) {
					*ipv4_opt = 0x01;
					ipv4_opt++;
				}
			}
			ipv4_opt = ipv4_opt + *(ipv4_opt + 1);
			break;

		case IPV4_SCURITY_OPTION_TYPE:
			/* Preserve */
			ipv4_opt += 11;
			break;

		case IPV4_LSRR_OPTION_TYPE:
			/* Preserve */
			ipv4_opt = ipv4_opt + *(ipv4_opt + 1);
			break;

		case IPV4_STREAM_IDENTIFIER_TYPE:
			/* Preserve */
			ipv4_opt += 4;
			break;

		case IPV4_SSRR_OPTION_TYPE:
			/* Preserve */
			ipv4_opt = ipv4_opt + *(ipv4_opt + 1);
			break;

		default:
			/* RAISE ERROR - OPTION IS NOT FAMILIER
			 * (length cannot be determined and
			 * thus copy-bit is irrelevant) */
			break;
		}
		more_opt = more_opt && (ipv4_opt < ipv4_hdr_end);
	}

	/* A LOT OF THINGS TO DO HERE */

	return 0;
}

int32_t ipf_discard_frame_remainder(ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;
	return fdma_discard_frame(
			ipf_ctx->rem_frame_handle, FDMA_DIS_NO_FLAGS);
}

void ipf_context_init(uint32_t flags, uint16_t mtu, ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;
	ipf_ctx->first_frag = 1;
	ipf_ctx->flags = flags;
	ipf_ctx->mtu = mtu;
}

