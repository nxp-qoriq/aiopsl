/**************************************************************************//**
@File		ip.c

@Description	This file contains the ip header modification API
		implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "general.h"
#include "net/fsl_net.h"
#include "dplib/fsl_parser.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_ip.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_ipv4_checksum.h"

#include "header_modification.h"


int32_t ip_header_decapsulation(uint8_t flags)
{
	uint8_t inner_ip_offset, outer_ip_offset, size_to_be_removed;
	struct ipv4hdr *inner_ipv4_ptr, *outer_ipv4_ptr;
	struct ipv6hdr *inner_ipv6_ptr, *outer_ipv6_ptr;
	void *inner_ip_ptr;
	uint16_t old_field;
	struct presentation_context *prc =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	inner_ip_offset = (uint8_t)(PARSER_GET_INNER_IP_OFFSET_DEFAULT());
	/* todo need parser to define PARSER_IS_INNER_IP macro */
	if (PARSER_IS_TUNNEL_IP_DEFAULT())
		return NO_IP_ENCAPSULATION_FOUND_ERROR;
	outer_ip_offset = (uint8_t)(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	if (flags) {
		if (PARSER_IS_INNER_IPV4_DEFAULT()) {
			inner_ipv4_ptr = (struct ipv4hdr *)
				(inner_ip_offset + PRC_GET_SEGMENT_ADDRESS());
			/* For CS calculation */
			old_field = *((uint16_t *)inner_ipv4_ptr);

			if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
				outer_ipv4_ptr = (struct ipv4hdr *)
				(outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());

				if (flags & IP_DECAP_MODE_TOS_TC_DS)
					inner_ipv4_ptr->tos =
					(inner_ipv4_ptr->tos &
							IPV4_DSCP_MASK) |
					(outer_ipv4_ptr->tos &
							~IPV4_DSCP_MASK);

				if (flags & IP_DECAP_MODE_TOS_TC_ECN)
					inner_ipv4_ptr->tos =
					(inner_ipv4_ptr->tos &
							IPV4_ECN_MASK) |
					(outer_ipv4_ptr->tos &
							~IPV4_ECN_MASK);

			/* Optimization calling cksum_calc_ipv4_header_checksum
			 * takes more than 25 cycles (assuming inlined
			 * functions) the bellow function takes 9 cycles
			 * including parameter preparation */
				/* Update IP CS for TOS changes. */
				cksum_update_uint16(
				    &inner_ipv4_ptr->hdr_cksum,
				    old_field,
				    *((uint16_t *)inner_ipv4_ptr+4));

				if (flags & IP_DECAP_MODE_TTL_HL) {
					old_field =
						*((uint16_t *)inner_ipv4_ptr+4);
					inner_ipv4_ptr->ttl =
							outer_ipv4_ptr->ttl;
					/* Update IP CS for TTL changes. */
					cksum_update_uint16(
					    &inner_ipv4_ptr->hdr_cksum,
					    old_field,
					    *((uint16_t *)inner_ipv4_ptr+4));
				}
			} else {
				/* Reset parser running sum in case of outer
				 * IPv6 as it does not contain checksum*/
				PARSER_CLEAR_RUNNING_SUM();

				outer_ipv6_ptr = (struct ipv6hdr *)
				(outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());

				if (flags & IP_DECAP_MODE_TOS_TC_DS)
					inner_ipv4_ptr->tos =
					(inner_ipv4_ptr->tos &
							IPV4_DSCP_MASK) |
				(((uint8_t)(outer_ipv6_ptr->vsn_traffic_flow
						>> 20) & ~IPV4_DSCP_MASK));

				if (flags & IP_DECAP_MODE_TOS_TC_ECN)
					inner_ipv4_ptr->tos =
					(inner_ipv4_ptr->tos &
							IPV4_ECN_MASK) |
				(((uint8_t)(outer_ipv6_ptr->vsn_traffic_flow
						>> 20) & ~IPV4_ECN_MASK));

					/* Update IP CS for TOS changes. */
					cksum_update_uint16(
					    &inner_ipv4_ptr->hdr_cksum,
					    old_field,
					    *((uint16_t *)inner_ipv4_ptr+4));

				if (flags & IP_DECAP_MODE_TTL_HL) {
					inner_ipv4_ptr->ttl =
						outer_ipv6_ptr->hop_limit;
					/* Update IP CS for TTL changes. */
					cksum_update_uint16(
					    &inner_ipv4_ptr->hdr_cksum,
					    old_field,
					    *((uint16_t *)inner_ipv4_ptr+4));
				}

			}
		} else {
			inner_ipv6_ptr = (struct ipv6hdr *)
				(inner_ip_offset + PRC_GET_SEGMENT_ADDRESS());

			if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
				outer_ipv4_ptr = (struct ipv4hdr *)
				(outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());

				if (flags & IP_DECAP_MODE_TOS_TC_DS)
					inner_ipv6_ptr->vsn_traffic_flow =
					(inner_ipv6_ptr->vsn_traffic_flow &
						IPV6_DSCP_MASK) |
					(((uint32_t)outer_ipv4_ptr->tos << 20)
							& ~IPV6_DSCP_MASK);

				if (flags & IP_DECAP_MODE_TOS_TC_ECN)
					inner_ipv6_ptr->vsn_traffic_flow =
					(inner_ipv6_ptr->vsn_traffic_flow &
						IPV6_ECN_MASK) |
					(((uint32_t)outer_ipv4_ptr->tos << 20)
							& ~IPV6_ECN_MASK);

				if (flags & IP_DECAP_MODE_TTL_HL)
					inner_ipv6_ptr->hop_limit =
							outer_ipv4_ptr->ttl;
			} else {
				/* Reset parser running sum in case of outer
				 * IPv6 as it does not contain checksum*/
				PARSER_CLEAR_RUNNING_SUM();
				outer_ipv6_ptr = (struct ipv6hdr *)
				(outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());

				if (flags & IP_DECAP_MODE_TOS_TC_DS)
					inner_ipv6_ptr->vsn_traffic_flow =
					(inner_ipv6_ptr->vsn_traffic_flow &
							IPV6_DSCP_MASK) |
					(outer_ipv6_ptr->vsn_traffic_flow
						& ~IPV6_DSCP_MASK);


				if (flags & IP_DECAP_MODE_TOS_TC_ECN)
					inner_ipv6_ptr->vsn_traffic_flow =
					(inner_ipv6_ptr->vsn_traffic_flow &
							IPV6_ECN_MASK) |
				(((uint8_t)(outer_ipv6_ptr->vsn_traffic_flow
						>> 20) & ~IPV6_ECN_MASK));

				if (flags & IP_DECAP_MODE_TTL_HL)
					inner_ipv6_ptr->hop_limit =
						outer_ipv6_ptr->hop_limit;
			}

		}
	}
	/* Update the frame presentation */
	size_to_be_removed = inner_ip_offset - outer_ip_offset;
	inner_ip_ptr = 	(void *)(inner_ip_offset + PRC_GET_SEGMENT_ADDRESS());
	if ((prc->seg_length - size_to_be_removed) >= MIN_REPRESENT_SIZE)
		fdma_replace_default_segment_data(
			(uint16_t)outer_ip_offset, /*to_offset*/
			(uint16_t)(size_to_be_removed + 12), /*to_size*/
			inner_ip_ptr, /*from_ws_src*/
			12, /*from_size*/
			(void *)prc->seg_address, /*ws_dst_rs*/
			prc->seg_length, /*size_rs*/
			FDMA_REPLACE_SA_REPRESENT_BIT);
	else
		fdma_replace_default_segment_data(
				(uint16_t)outer_ip_offset, /*to_offset*/
				(uint16_t)(size_to_be_removed + 12),/*to_size*/
				inner_ip_ptr, /*from_ws_src*/
				12, /*from_size*/
				(void *)prc->seg_address, /*ws_dst_rs*/
				MIN_REPRESENT_SIZE, /*size_rs*/
				FDMA_REPLACE_SA_REPRESENT_BIT);

	/* generate new parse information */
	parse_result_generate_default(0);
	return SUCCESS;
}

int32_t ipv4_header_modification(uint8_t flags, uint8_t tos, uint16_t id,
		uint32_t ip_src_addr, uint32_t ip_dst_addr)
{
	uint16_t ipv4hdr_offset;
	uint16_t udp_tcp_offset;
	struct   ipv4hdr *ipv4hdr_ptr;
	struct   udphdr *udphdr_ptr;
	struct   tcphdr *tcphdr_ptr;
	uint32_t l4_update;
	uint16_t sum;
	struct   parse_result *pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;


	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		l4_update = 0;
		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv4hdr_ptr = (struct ipv4hdr *)
				(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());

		if (flags & IPV4_MODIFY_MODE_IPTTL)
			ipv4hdr_ptr->ttl = ipv4hdr_ptr->ttl - 1;
		if (flags & IPV4_MODIFY_MODE_IPTOS)
			ipv4hdr_ptr->tos = tos;
		if (flags & IPV4_MODIFY_MODE_IPID)
			ipv4hdr_ptr->id = id;
		sum = 0;
		if (flags & IPV4_MODIFY_MODE_IPSRC) {
			if (flags & IPV4_MODIFY_MODE_L4_CHECKSUM) {
				/* Calculate delta for L4 checksum */
				sum = cksum_accumulative_update_uint32(
							  sum,
							  ipv4hdr_ptr->src_addr,
							  ip_src_addr);
				l4_update = 1;
			}

			ipv4hdr_ptr->src_addr = ip_src_addr;

		}
		if (flags & IPV4_MODIFY_MODE_IPDST) {
			if (flags & IPV4_MODIFY_MODE_L4_CHECKSUM) {
				/* Calculate delta for L4 checksum */
				sum = cksum_accumulative_update_uint32(
							  sum,
							  ipv4hdr_ptr->dst_addr,
							  ip_dst_addr);
				l4_update = 1;
			}

			ipv4hdr_ptr->dst_addr = ip_dst_addr;
		}

		/* update IP checksum */
		ipv4_cksum_calculate(ipv4hdr_ptr);

		/* update UDP/TCP checksum */
		if (l4_update) {
			udp_tcp_offset = PARSER_GET_L4_OFFSET_DEFAULT();
			if (PARSER_IS_UDP_DEFAULT()) {
				udphdr_ptr = (struct udphdr *)
				  ((uint16_t)udp_tcp_offset
				  + PRC_GET_SEGMENT_ADDRESS());
				if (udphdr_ptr->checksum != 0) {
					udphdr_ptr->checksum =
						cksum_ones_complement_sum16(
							   udphdr_ptr->checksum,
							   sum);
					fdma_modify_default_segment_data(
					    ipv4hdr_offset,
					    udp_tcp_offset-ipv4hdr_offset + 8);
					/* Invalidate gross running sum */
					pr->gross_running_sum = 0;
				} else{
				/* update IP header in FDMA */
					fdma_modify_default_segment_data(
							ipv4hdr_offset,
							20);

				}
			} else if (PARSER_IS_TCP_DEFAULT()) {
				tcphdr_ptr = (struct tcphdr *)
						((uint16_t)udp_tcp_offset +
						    PRC_GET_SEGMENT_ADDRESS());
				tcphdr_ptr->checksum =
						cksum_ones_complement_sum16(
							tcphdr_ptr->checksum,
							sum);
				fdma_modify_default_segment_data(
					    ipv4hdr_offset,
					    udp_tcp_offset-ipv4hdr_offset + 18);

				/* Invalidate gross running sum */
				pr->gross_running_sum = 0;
			}
		} else {
			/* update IP header in FDMA */
			fdma_modify_default_segment_data(ipv4hdr_offset,
							  20);
		}
	return SUCCESS;
	} else {
		return NO_IP_HDR_ERROR; }
}

int32_t ipv6_header_modification(uint8_t flags, uint8_t tc,
				    uint32_t flow_label, uint8_t *ip_src_addr,
				    uint8_t *ip_dst_addr)
{
	uint16_t ipv6hdr_offset;
	uint16_t udp_tcp_offset;
	struct   ipv6hdr *ipv6hdr_ptr;
	struct   udphdr *udphdr_ptr;
	struct   tcphdr *tcphdr_ptr;
	uint16_t l4_checksum, checksum;
	uint32_t l4_update;
	struct   parse_result *pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	if (PARSER_IS_OUTER_IPV6_DEFAULT()) {
		ipv6hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv6hdr_ptr = (struct ipv6hdr *)
				(ipv6hdr_offset + PRC_GET_SEGMENT_ADDRESS());

		l4_update = 0;
		if ((flags & IPV6_MODIFY_MODE_L4_CHECKSUM) &
				((flags & IPV6_MODIFY_MODE_IPSRC) ||
				 (flags & IPV6_MODIFY_MODE_IPDST))) {
			/* calculate checksum on old IPsrc+IPdst */
			fdma_calculate_default_frame_checksum(ipv6hdr_offset+8,
							      32,
							      &l4_checksum);
			l4_update = 1;
		}

		/* Update IP header in workspace */
		if (flags & IPV6_MODIFY_MODE_IPHL)
			ipv6hdr_ptr->hop_limit = ipv6hdr_ptr->hop_limit - 1;
		if (flags & IPV6_MODIFY_MODE_IPTC)
			ipv6hdr_ptr->vsn_traffic_flow =
			(ipv6hdr_ptr->vsn_traffic_flow & IPV6_TC_MASK) |
							(((uint32_t)tc)<<20);
		if (flags & IPV6_MODIFY_MODE_FLOW_LABEL)
			ipv6hdr_ptr->vsn_traffic_flow =
			(ipv6hdr_ptr->vsn_traffic_flow & IPV6_FLOW_MASK)
						| flow_label;

		if (flags & IPV6_MODIFY_MODE_IPSRC) {

			*((long long *)ipv6hdr_ptr->src_addr) =
					*((long long *)ip_src_addr);
			*((((long long *)ipv6hdr_ptr->src_addr)+1)) =
					*((((long long *)ip_src_addr)+1));
		}
		if (flags & IPV6_MODIFY_MODE_IPDST) {
			*((long long *)ipv6hdr_ptr->dst_addr) =
					*((long long *)ip_dst_addr);
			*((((long long *)ipv6hdr_ptr->dst_addr)+1)) =
					*((((long long *)ip_dst_addr)+1));
		}

		/* Update L4 checksum if needed */
		if (l4_update) {
			udp_tcp_offset = PARSER_GET_L4_OFFSET_DEFAULT();
			if (PARSER_IS_UDP_DEFAULT()) {
				udphdr_ptr = (struct udphdr *)
				  ((uint16_t)udp_tcp_offset
				  + PRC_GET_SEGMENT_ADDRESS());

				if (udphdr_ptr->checksum != 0) {
					/* Add checksum on old IPsrc+Ip dst */
					l4_checksum =
						cksum_ones_complement_sum16(
								l4_checksum,
						(uint16_t)udphdr_ptr->checksum);
				/* Calculate checksum on new IP src + IP dst */
					fdma_calculate_default_frame_checksum(
							      ipv6hdr_offset+8,
							      32,
							      &checksum);
				/* Substract checksum of new IP src+IP dst */
					l4_checksum =
						cksum_ones_complement_sum16(
							l4_checksum,
							(uint16_t)~checksum);
					udphdr_ptr->checksum = l4_checksum;

				/* Update FDMA with recalculated UDP checksum */
					fdma_modify_default_segment_data(
							      udp_tcp_offset+6,
							      2);
				}
			} else if (PARSER_IS_TCP_DEFAULT()) {
				tcphdr_ptr = (struct tcphdr *)
						((uint16_t)udp_tcp_offset +
						PRC_GET_SEGMENT_ADDRESS());
				l4_checksum = (uint16_t)tcphdr_ptr->checksum;
				/* Add checksum on old IPsrc+Ip dst */
				l4_checksum = cksum_ones_complement_sum16(
						l4_checksum,
						(uint16_t)tcphdr_ptr->checksum);
				/* Calculate checksum on new IP src + IP dst */
				fdma_calculate_default_frame_checksum(
						ipv6hdr_offset+8,
						32,
						&checksum);
				/* Subtract checksum of new IP src+IP dst */
				l4_checksum = cksum_ones_complement_sum16(
							   l4_checksum,
							   (uint16_t)~checksum);
				tcphdr_ptr->checksum = l4_checksum;

				/* Update FDMA with recalculated TCP checksum */
				fdma_modify_default_segment_data(
							     udp_tcp_offset+16,
							     2);
			}
		}

		/* Invalidate gross running sum */
		pr->gross_running_sum = 0;

		return SUCCESS;
	} else {
		return NO_IP_HDR_ERROR;
	     }
}

int32_t ipv4_header_encapsulation(uint8_t flags,
		struct ipv4hdr *ipv4_header_ptr, uint8_t ipv4_header_size)
{
	struct ipv4hdr *inner_ipv4hdr_ptr;
	struct ipv4hdr *outer_ipv4hdr_ptr;
	struct ipv6hdr *inner_ipv6hdr_ptr;
	uint32_t	fdma_flags;
	uint16_t	inner_ipv4_offset;
	uint16_t	outer_ipv4_offset;
	uint16_t	inner_ipv6_offset;
	uint16_t	fdma_size;
	uint16_t	*ethertype_ptr;
	void		*etype_ptr;
	uint32_t	*mpls_ptr;
	uint32_t	vsn_traffic_flow;
	uint8_t		tos, inner_tos;
	uint32_t	mpls_present;
	uint32_t	eth_present;
	struct presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;


	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		inner_ipv4_offset =
				(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		inner_ipv4hdr_ptr = (struct ipv4hdr *)
				(inner_ipv4_offset + PRC_GET_SEGMENT_ADDRESS());

		if (flags & IPV4_ENCAP_MODE_TTL)
			ipv4_header_ptr->ttl = inner_ipv4hdr_ptr->ttl;
		tos = ipv4_header_ptr->tos;
		inner_tos = inner_ipv4hdr_ptr->tos;
		if (flags & IPV4_ENCAP_MODE_TOS_DS)
			tos = (tos & IPV4_DSCP_MASK) |
				(inner_tos & ~IPV4_DSCP_MASK);
		if (flags & IPV4_ENCAP_MODE_TOS_ECN)
			tos = (tos & IPV4_ECN_MASK) |
				(inner_tos & ~IPV4_ECN_MASK);
		ipv4_header_ptr->tos = tos;
		if (flags & IPV4_ENCAP_MODE_DF)
			ipv4_header_ptr->flags_and_offset =
			 (ipv4_header_ptr->flags_and_offset & IPV4_DF_MASK) |
		       (inner_ipv4hdr_ptr->flags_and_offset & ~IPV4_DF_MASK);

		ipv4_header_ptr->total_length = inner_ipv4hdr_ptr->total_length
						+ (uint16_t)ipv4_header_size;

		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_OPEN_BIT |
					FDMA_REPLACE_SA_REPRESENT_BIT);

		if ((prc->seg_address - TLS_SECTION_END_ADDR) >=
							ipv4_header_size) {
			/* there is enough room in the head room */
			fdma_insert_default_segment_data(
						    inner_ipv4_offset,
						    (void *)ipv4_header_ptr,
						    (uint16_t) ipv4_header_size,
						    fdma_flags);
		} else {
			fdma_replace_default_segment_data(
						    inner_ipv4_offset,
						    0,
						    (void *)ipv4_header_ptr,
						    (uint16_t) ipv4_header_size,
						    (void *)prc->seg_address,
						    prc->seg_length,
						    fdma_flags);

		}

		/* Re-run parser */
		parse_result_generate_default(0);

		outer_ipv4_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		outer_ipv4hdr_ptr = (struct ipv4hdr *)
				(outer_ipv4_offset + PRC_GET_SEGMENT_ADDRESS());

		ipv4_cksum_calculate(outer_ipv4hdr_ptr);

		/* update IP checksum in FDMA */
		fdma_modify_default_segment_data(outer_ipv4_offset+10,
						 2);
		return SUCCESS;

	} else if (PARSER_IS_OUTER_IPV6_DEFAULT()) {
		inner_ipv6_offset =
				 (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		inner_ipv6hdr_ptr = (struct ipv6hdr *)
				(inner_ipv6_offset + PRC_GET_SEGMENT_ADDRESS());

		tos = ipv4_header_ptr->tos;
		vsn_traffic_flow = inner_ipv6hdr_ptr->vsn_traffic_flow;
		if (flags & IPV4_ENCAP_MODE_TTL)
			ipv4_header_ptr->ttl = inner_ipv6hdr_ptr->hop_limit;
		if (flags & IPV4_ENCAP_MODE_TOS_DS)
			tos = (tos & IPV4_DSCP_MASK) |
			  ((uint8_t)((vsn_traffic_flow &
					~IPV6_DSCP_MASK)>>20));
		if (flags & IPV4_ENCAP_MODE_TOS_ECN)
			tos = (tos & IPV4_ECN_MASK) |
			  ((uint8_t)((vsn_traffic_flow &
					  ~IPV6_ECN_MASK)>>20));

		ipv4_header_ptr->tos = tos;

		ipv4_header_ptr->total_length =
				inner_ipv6hdr_ptr->payload_length + 40
						+ (uint16_t) ipv4_header_size;

		/* Update Etype or MPLS label if needed */
		mpls_present = (uint32_t) PARSER_IS_ONE_MPLS_DEFAULT();
		if (mpls_present)
			etype_ptr = PARSER_GET_LAST_MPLS_POINTER_DEFAULT();
		else {
			eth_present = (uint32_t)PARSER_IS_ETH_MAC_DEFAULT();
			if (eth_present)
				etype_ptr = (uint16_t *)
					PARSER_GET_LAST_ETYPE_POINTER_DEFAULT();
			else
				etype_ptr = (void *)inner_ipv6hdr_ptr;
		}
		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_OPEN_BIT|
					FDMA_REPLACE_SA_REPRESENT_BIT);

		if ((prc->seg_address - TLS_SECTION_END_ADDR) >=
							ipv4_header_size) {
			/* there is enough room in the head room */
			fdma_insert_default_segment_data(
						   inner_ipv6_offset,
						   (void *)ipv4_header_ptr,
						   (uint16_t) ipv4_header_size,
						   fdma_flags);

			etype_ptr = (void *)(((uint32_t)etype_ptr) -
							ipv4_header_size);
		} else {
			fdma_replace_default_segment_data(
						   inner_ipv6_offset,
						   0,
						   (void *)ipv4_header_ptr,
						   (uint16_t) ipv4_header_size,
						   (void *)prc->seg_address,
						   prc->seg_length,
						   fdma_flags);
		}
		if (mpls_present) {
			mpls_ptr  = (uint32_t *) etype_ptr;
			*mpls_ptr = (*mpls_ptr & MPLS_LABEL_MASK) |
							   MPLS_LABEL_IPV4 ;
			fdma_size = 16;
		} else if (eth_present) {
			ethertype_ptr = (uint16_t *) etype_ptr;
			*ethertype_ptr = ETYPE_IPV4;
			fdma_size = 14;
		} else {
			fdma_size = 12;
		}

		/* Re-run parser */
		parse_result_generate_default(0);

		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;

		outer_ipv4hdr_ptr = (struct ipv4hdr *)
				PARSER_GET_OUTER_IP_POINTER_DEFAULT();

		ipv4_cksum_calculate(outer_ipv4hdr_ptr);

		/* update Checksum  & Etype in FDMA */
		fdma_modify_default_segment_data(
				      (uint16_t) ((uint32_t)etype_ptr -
					      (uint32_t)prc->seg_address),
				      fdma_size);

		return SUCCESS;
	} else { /* no inner IP */
		return NO_IP_HDR_ERROR;
	}
}

int32_t ipv6_header_encapsulation(uint8_t flags,
		struct ipv6hdr *ipv6_header_ptr, uint8_t ipv6_header_size)
{
	struct ipv4hdr *inner_ipv4hdr_ptr;
	struct ipv6hdr *inner_ipv6hdr_ptr;
	uint32_t 	fdma_flags;
	uint16_t	inner_ipv4_offset;
	uint16_t	inner_ipv6_offset;
	void		*etype_ptr;
	uint16_t	*ethertype_ptr;
	uint32_t	*mpls_ptr;
	uint32_t	vsn_traffic_flow;
	uint8_t		inner_tos;
	uint32_t	mpls_present;
	uint32_t	eth_present;
	struct presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;


	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {

		inner_ipv4_offset =
				(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		inner_ipv4hdr_ptr = (struct ipv4hdr *)
				(inner_ipv4_offset + PRC_GET_SEGMENT_ADDRESS());

		vsn_traffic_flow = ipv6_header_ptr->vsn_traffic_flow;
		inner_tos = inner_ipv4hdr_ptr->tos;
		if (flags & IPV6_ENCAP_MODE_HL)
			ipv6_header_ptr->hop_limit = inner_ipv4hdr_ptr->ttl;
		if (flags & IPV6_ENCAP_MODE_TC_DSCP)
			vsn_traffic_flow =
				(vsn_traffic_flow & IPV6_DSCP_MASK) |
		(((uint32_t)(inner_tos & ~IPV4_DSCP_MASK))<<22);
		if (flags & IPV6_ENCAP_MODE_TC_ECN)
			vsn_traffic_flow =
				(vsn_traffic_flow & IPV6_ECN_MASK) |
			(((uint32_t)((inner_tos & ~IPV4_ECN_MASK)))<<20);

		ipv6_header_ptr->vsn_traffic_flow = vsn_traffic_flow;

		ipv6_header_ptr->payload_length =
				inner_ipv4hdr_ptr->total_length
					+ (uint16_t)ipv6_header_size - 40;

		/* Update Etype or MPLS label if needed */
		mpls_present = (uint32_t) PARSER_IS_ONE_MPLS_DEFAULT();
		if (mpls_present)
			etype_ptr = PARSER_GET_LAST_MPLS_POINTER_DEFAULT();
		else {
			eth_present = (uint32_t)PARSER_IS_ETH_MAC_DEFAULT();
			if (eth_present)
				etype_ptr = (uint16_t *)
					PARSER_GET_LAST_ETYPE_POINTER_DEFAULT();
			else
				etype_ptr = (void *)inner_ipv4hdr_ptr;
		}

		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_OPEN_BIT|
					FDMA_REPLACE_SA_REPRESENT_BIT);

		if ((prc->seg_address - TLS_SECTION_END_ADDR) >=
							ipv6_header_size) {
			/* there is enough room in the head room */
			fdma_insert_default_segment_data(
						    inner_ipv4_offset,
						    (void *)ipv6_header_ptr,
						    (uint16_t) ipv6_header_size,
						    fdma_flags);
			etype_ptr = (void *)(((uint32_t)etype_ptr) -
							ipv6_header_size);

		} else {
			fdma_replace_default_segment_data(
						    inner_ipv4_offset,
						    0,
						    (void *)ipv6_header_ptr,
						    (uint16_t) ipv6_header_size,
						    (void *)prc->seg_address,
						    prc->seg_length,
						    fdma_flags);

		}

		if (mpls_present) {
			mpls_ptr  = (uint32_t *) etype_ptr;
			*mpls_ptr = (*mpls_ptr & MPLS_LABEL_MASK) |
							   MPLS_LABEL_IPV6 ;
			/* update MPLS label in FDMA */
			fdma_modify_default_segment_data(
					      (uint16_t) ((uint32_t)etype_ptr -
						    (uint32_t)prc->seg_address),
					      3);

		} else if (eth_present) {
			ethertype_ptr = (uint16_t *) etype_ptr;
			*ethertype_ptr = ETYPE_IPV6;
			/* update Etype in FDMA */
			fdma_modify_default_segment_data(
					(uint16_t) ((uint32_t)etype_ptr -
					      (uint32_t)prc->seg_address),
					      2);
			}

		/* Re-run parser */
		parse_result_generate_default(0);

		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;

		return SUCCESS;

	} else if (PARSER_IS_OUTER_IPV6_DEFAULT()) {
		inner_ipv6_offset =
				 (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		inner_ipv6hdr_ptr = (struct ipv6hdr *)
				(inner_ipv6_offset + PRC_GET_SEGMENT_ADDRESS());

		vsn_traffic_flow = ipv6_header_ptr->vsn_traffic_flow;
		if (flags & IPV6_ENCAP_MODE_HL)
			ipv6_header_ptr->hop_limit =
						  inner_ipv6hdr_ptr->hop_limit;
		if (flags & IPV6_ENCAP_MODE_TC_DSCP)
			vsn_traffic_flow =
				(vsn_traffic_flow & IPV6_DSCP_MASK) |
				(inner_ipv6hdr_ptr->vsn_traffic_flow &
					~IPV6_DSCP_MASK);
		if (flags & IPV6_ENCAP_MODE_TC_ECN)
			vsn_traffic_flow =
				(vsn_traffic_flow & IPV6_ECN_MASK) |
	       (inner_ipv6hdr_ptr->vsn_traffic_flow & ~IPV4_ECN_MASK);

		ipv6_header_ptr->vsn_traffic_flow = vsn_traffic_flow;

		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_OPEN_BIT|
					FDMA_REPLACE_SA_REPRESENT_BIT);

		ipv6_header_ptr->payload_length =
				inner_ipv6hdr_ptr->payload_length +
					(uint16_t) ipv6_header_size - 40;

		if ((prc->seg_address - TLS_SECTION_END_ADDR) >=
							ipv6_header_size) {
			/* there is enough room in the head room */
			fdma_insert_default_segment_data(
						   inner_ipv6_offset,
						   (void *)ipv6_header_ptr,
						   (uint16_t) ipv6_header_size,
						   fdma_flags);

		} else {
			fdma_replace_default_segment_data(
						   inner_ipv6_offset,
						   0,
						   (void *)ipv6_header_ptr,
						   (uint16_t) ipv6_header_size,
						   (void *)prc->seg_address,
						   prc->seg_length,
						   fdma_flags);

		}

		/* Re-run parser */
		parse_result_generate_default(0);

		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;

		return SUCCESS;
	} else { /* no inner IP */
		return NO_IP_HDR_ERROR;
	}
}

int32_t ip_set_nw_src(uint32_t src_addr)
{
	uint16_t ipv4hdr_offset;
	uint16_t udp_tcp_offset;
	struct   ipv4hdr *ipv4hdr_ptr;
	struct   udphdr *udphdr_ptr;
	struct   tcphdr *tcphdr_ptr;
	struct   parse_result *pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;


	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv4hdr_ptr = (struct ipv4hdr *)
				(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());

		/* update IP checksum */
		cksum_update_uint32(&ipv4hdr_ptr->hdr_cksum,
				    ipv4hdr_ptr->src_addr,
				    src_addr);

		ipv4hdr_ptr->src_addr = src_addr;

		/* update FDMA */
		fdma_modify_default_segment_data(ipv4hdr_offset+10, 6);

		/* update UDP/TCP checksum */
		udp_tcp_offset = PARSER_GET_L4_OFFSET_DEFAULT();
		if (PARSER_IS_UDP_DEFAULT()) {
			udphdr_ptr = (struct udphdr *) ((uint16_t)udp_tcp_offset
						+ PRC_GET_SEGMENT_ADDRESS());

			cksum_update_uint32(&udphdr_ptr->checksum,
					    ipv4hdr_ptr->src_addr,
					    src_addr);

			/* update FDMA */
			fdma_modify_default_segment_data(udp_tcp_offset + 6, 2);

			/* Invalidate gross running sum */
			pr->gross_running_sum = 0;

		} else if (PARSER_IS_TCP_DEFAULT()) {
			tcphdr_ptr = (struct tcphdr *) ((uint16_t)udp_tcp_offset
					+ PRC_GET_SEGMENT_ADDRESS());

			cksum_update_uint32(&tcphdr_ptr->checksum,
					    ipv4hdr_ptr->src_addr,
					    src_addr);

			/* update FDMA */
		       fdma_modify_default_segment_data(udp_tcp_offset + 16, 2);
			/* Invalidate gross running sum */
			pr->gross_running_sum = 0;

			}
	return SUCCESS;
	} else {
		return NO_IP_HDR_ERROR; }

}

int32_t ip_set_nw_dst(uint32_t dst_addr)
{
	uint16_t ipv4hdr_offset;
	uint16_t udp_tcp_offset;
	struct   ipv4hdr *ipv4hdr_ptr;
	struct   udphdr *udphdr_ptr;
	struct   tcphdr *tcphdr_ptr;
	struct   parse_result *pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;


	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv4hdr_ptr = (struct ipv4hdr *)
				(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());

		/* update IP checksum */
		cksum_update_uint32(&ipv4hdr_ptr->hdr_cksum,
				    ipv4hdr_ptr->dst_addr,
				    dst_addr);

		ipv4hdr_ptr->dst_addr = dst_addr;

		/* update FDMA */
		fdma_modify_default_segment_data(ipv4hdr_offset+10, 10);

		/* update UDP/TCP checksum */
		udp_tcp_offset = PARSER_GET_L4_OFFSET_DEFAULT();
		if (PARSER_IS_UDP_DEFAULT()) {
			udphdr_ptr = (struct udphdr *) ((uint16_t)udp_tcp_offset
						+ PRC_GET_SEGMENT_ADDRESS());

			cksum_update_uint32(&udphdr_ptr->checksum,
					    ipv4hdr_ptr->dst_addr,
					    dst_addr);

			/* update FDMA */
			fdma_modify_default_segment_data(udp_tcp_offset + 6, 2);

			/* Invalidate gross running sum */
			pr->gross_running_sum = 0;

		} else if (PARSER_IS_TCP_DEFAULT()) {
			tcphdr_ptr = (struct tcphdr *) ((uint16_t)udp_tcp_offset
					+ PRC_GET_SEGMENT_ADDRESS());

			cksum_update_uint32(&tcphdr_ptr->checksum,
					    ipv4hdr_ptr->dst_addr,
					    dst_addr);

			/* update FDMA */
		       fdma_modify_default_segment_data(udp_tcp_offset + 16, 2);
			/* Invalidate gross running sum */
			pr->gross_running_sum = 0;

			}
	return SUCCESS;
	} else {
		return NO_IP_HDR_ERROR; }


}
