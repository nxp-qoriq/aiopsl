/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		ip.c

@Description	This file contains the ip header modification API
		implementation.

*//***************************************************************************/

#include "general.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_ip.h"
#include "fsl_cdma.h"
#include "fsl_checksum.h"
#include "net.h"
#include "fsl_time.h"

int ip_header_decapsulation(uint8_t flags)
{
	uint8_t inner_ip_offset, outer_ip_offset, size_to_be_removed;
	uint16_t etype_offset, mpls_offset;
	uint16_t *etype_ptr;
	uint32_t *mpls_ptr;
	struct ipv4hdr *inner_ipv4_ptr, *outer_ipv4_ptr;
	struct ipv6hdr *inner_ipv6_ptr, *outer_ipv6_ptr;
	void *inner_ip_ptr;
	uint32_t old_field;
	struct presentation_context *prc =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	inner_ip_offset = (uint8_t)(PARSER_GET_INNER_IP_OFFSET_DEFAULT());
	/* todo need parser to define PARSER_IS_INNER_IP macro */
	if (!PARSER_IS_TUNNELED_IP_DEFAULT())
		return NO_IP_ENCAPSULATION_FOUND_ERROR;
	outer_ip_offset = (uint8_t)(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	if (PARSER_IS_INNER_IPV4_DEFAULT()) {
		inner_ipv4_ptr = (struct ipv4hdr *)
			(inner_ip_offset + PRC_GET_SEGMENT_ADDRESS());
		/* For CS calculation */
		old_field = *((uint32_t *)inner_ipv4_ptr);

		if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
			/* Inner & Outer IPv4 */
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
			cksum_update_uint32(
				&inner_ipv4_ptr->hdr_cksum,
				old_field,
				*((uint32_t *)inner_ipv4_ptr));

			if (flags & IP_DECAP_MODE_TTL_HL) {
				old_field =
					*((uint32_t *)inner_ipv4_ptr+2);
				inner_ipv4_ptr->ttl =
						outer_ipv4_ptr->ttl;
				/* Update IP CS for TTL changes. */
				cksum_update_uint32(
					&inner_ipv4_ptr->hdr_cksum,
					old_field,
					*((uint32_t *)inner_ipv4_ptr+2));
			}
		} else {
			/* Inner IPv4 , outer IPv6
			 * Reset parser running sum in case of outer
			 * IPv6 as it does not contain checksum*/
			PARSER_CLEAR_RUNNING_SUM();

			/* Update Etype or MPLS label if needed */
			if (PARSER_IS_ONE_MPLS_DEFAULT()) {
				mpls_offset =
					PARSER_GET_LAST_MPLS_OFFSET_DEFAULT();
				mpls_ptr =
				  (uint32_t *) (mpls_offset + prc->seg_address);
				*mpls_ptr = (*mpls_ptr & MPLS_LABEL_MASK) |
							   NET_MPLS_LABEL_IPV4;
				fdma_modify_default_segment_data(mpls_offset,
								 3);
			} else {
				if (PARSER_IS_ETH_MAC_DEFAULT()) {
					etype_offset =
					 PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT();
					etype_ptr = (uint16_t *)
					      (etype_offset + prc->seg_address);
					*etype_ptr = NET_ETH_ETYPE_IPV4;
					fdma_modify_default_segment_data(
								   etype_offset,
								   2);
				}
			}

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
				cksum_update_uint32(
					&inner_ipv4_ptr->hdr_cksum,
					old_field,
					*((uint32_t *)inner_ipv4_ptr));

			if (flags & IP_DECAP_MODE_TTL_HL) {
				old_field =
					*((uint32_t *)inner_ipv4_ptr+2);
				inner_ipv4_ptr->ttl =
					outer_ipv6_ptr->hop_limit;
				/* Update IP CS for TTL changes. */
				cksum_update_uint32(
					&inner_ipv4_ptr->hdr_cksum,
					old_field,
					*((uint32_t *)inner_ipv4_ptr+2));
			}
		}
	} else {
		inner_ipv6_ptr = (struct ipv6hdr *)
			(inner_ip_offset + PRC_GET_SEGMENT_ADDRESS());

		if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
			/* Update Etype or MPLS label if needed */
			if (PARSER_IS_ONE_MPLS_DEFAULT()) {
				mpls_offset =
					  PARSER_GET_LAST_MPLS_OFFSET_DEFAULT();
				mpls_ptr = (uint32_t *)
					       (mpls_offset + prc->seg_address);
				*mpls_ptr = (*mpls_ptr & MPLS_LABEL_MASK) |
							   NET_MPLS_LABEL_IPV6;
				fdma_modify_default_segment_data(mpls_offset,
								 3);
			} else {
				if (PARSER_IS_ETH_MAC_DEFAULT()) {
					etype_offset =
					 PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT();
					etype_ptr = (uint16_t *)
					      (etype_offset + prc->seg_address);
					*etype_ptr = NET_ETH_ETYPE_IPV6;
					fdma_modify_default_segment_data(
								   etype_offset,
								   2);
				}
			}

			/* Inner IPv6, outer IPv4 */
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
			/* Inner & outer IPv6
			 * Reset parser running sum in case of outer
			 * IPv6 as it does not contain checksum*/
			/* Update Etype and MPLS label if needed */
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
					  ((outer_ipv6_ptr->vsn_traffic_flow &
							     ~IPV6_ECN_MASK));

			if (flags & IP_DECAP_MODE_TTL_HL)
				inner_ipv6_ptr->hop_limit =
					outer_ipv6_ptr->hop_limit;
		}

	}
	/* Update the frame presentation */
	size_to_be_removed = inner_ip_offset - outer_ip_offset;
	inner_ip_ptr = (void *)(inner_ip_offset + PRC_GET_SEGMENT_ADDRESS());
	fdma_replace_default_segment_data(
			(uint16_t)outer_ip_offset, /*to_offset*/
			(uint16_t)(size_to_be_removed + 12), /*to_size*/
			inner_ip_ptr, /*from_ws_src*/
			12, /*from_size*/
			(void *)prc->seg_address, /*ws_dst_rs*/
			prc->seg_length, /*size_rs*/
			FDMA_REPLACE_SA_REPRESENT_BIT);

	/* generate new parse information */
	parse_result_generate_default(0);
	return SUCCESS;
}

void ipv4_mangle(uint8_t flags, uint8_t dscp, uint8_t ttl)
{
	struct   ipv4hdr *ipv4hdr_ptr;
	uint16_t ipv4hdr_offset;
	uint32_t old_val, new_val;
	uint8_t tos;

	ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4hdr_ptr = (struct ipv4hdr *)
			(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());

	if (flags & IPV4_MANGLE_DSCP)
	{
		old_val = *(uint32_t *)(&ipv4hdr_ptr->vsn_and_ihl);
		/* clear DSCP field in TOS - 6 msb*/
		tos = ipv4hdr_ptr->tos & 0x3;
		tos |= dscp << 2;
		ipv4hdr_ptr->tos = tos;
		new_val = *(uint32_t *)(&ipv4hdr_ptr->vsn_and_ihl);
		/* update IP checksum */
		cksum_update_uint32(&ipv4hdr_ptr->hdr_cksum,
				    old_val,
				    new_val);
	}
	if (flags & IPV4_MANGLE_TTL)
	{
		old_val = *(uint32_t *)(&ipv4hdr_ptr->ttl);
		ipv4hdr_ptr->ttl = ttl;
		new_val = *(uint32_t *)(&ipv4hdr_ptr->ttl);
		/* update IP checksum */
		cksum_update_uint32(&ipv4hdr_ptr->hdr_cksum,
				    old_val,
				    new_val);
	}
	/* no need to invalidate gross running sum since IPv4 header
	 * checksum was also updated*/
	/* update IP header in FDMA */
	fdma_modify_default_segment_data(ipv4hdr_offset, 12);
}

void ipv4_dec_ttl_modification(void)
{
	struct   ipv4hdr *ipv4hdr_ptr;
	uint16_t ipv4hdr_offset;
	uint32_t old_ttl, new_ttl;

	ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4hdr_ptr = (struct ipv4hdr *)
			(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());

	old_ttl = *(uint32_t *)(&ipv4hdr_ptr->ttl);
	ipv4hdr_ptr->ttl = ipv4hdr_ptr->ttl - 1;
	new_ttl = *(uint32_t *)(&ipv4hdr_ptr->ttl);
	/* update IP checksum */
	cksum_update_uint32(&ipv4hdr_ptr->hdr_cksum,
			    old_ttl,
			    new_ttl);
	/* no need to invalidate gross running sum since IPv4 header
	 * checksum was also updated */
	/* update IP header in FDMA including check-sum */
	fdma_modify_default_segment_data(ipv4hdr_offset + 8, 4);
}

int ipv4_ts_opt_modification(struct ipv4hdr *ipv4_hdr, uint8_t *ip_opt_ptr, 
		uint32_t ip_address)
{
	uint8_t length = TS_OPT_GET_LENGTH();
	uint8_t ptr_next_ts = TS_OPT_GET_PTR();
	uint8_t overflow_flag = TS_OPT_GET_OVRFLOW_FLAG();
	uint16_t ipv4_offset = (uint16_t)((uint32_t)ipv4_hdr 
			- PRC_GET_SEGMENT_ADDRESS());
	uint32_t old_val, new_val, ut;
	uint16_t fdma_size;
	uint32_t *ts_ptr;

	if (length < 4)
		return -EIO;
		
	if (ptr_next_ts < 5)
		return -ENOSPC;
	fdma_size = (uint16_t)((uint32_t)ip_opt_ptr - (uint32_t)ipv4_hdr);
	if (ptr_next_ts > length)
	{
		if ((overflow_flag & 0xf0) == 0xf0)
			return -ENODEV;
		else
		{
			overflow_flag += 0x10;
			old_val = TS_OPT_GET_FIRST_WORD();
			TS_OPT_SET_OVRFLOW_FLAG();
			new_val = TS_OPT_GET_FIRST_WORD();
			/* need updating IPv4 header checksum */
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
					    old_val,
					    new_val);
			/* including first word from TS option */
			fdma_size += 4;
			fdma_modify_default_segment_data(ipv4_offset,
					fdma_size);
			return IP_TS_OPT_INC_OVERFLOW;
		}
	}
	else
	{
		fsl_get_time_ms(&ut);
		ts_ptr = (uint32_t *)(ip_opt_ptr + ptr_next_ts - 1);
		switch (TS_OPT_GET_FLAG())
		{
		case AIOP_IPOPT_TS_TSONLY:
			/* if length - (ptr_next_ts - 1) >= 4 it is ok */
			if ((length - ptr_next_ts) < 3 )
				return -ENOSPC;
			*ts_ptr = ut;
			old_val = TS_OPT_GET_FIRST_WORD();
			TS_OPT_SET_PTR(4);
			new_val = TS_OPT_GET_FIRST_WORD();
			/* need updating IPv4 header checksum */
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
					    old_val,
					    new_val);
			/* need updating IPv4 header checksum */
			/* rfc says old val must be 0 */
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
					    0,
					    ut);
			break;
		case AIOP_IPOPT_TS_TSANDADDR:
			/* if length - (ptr_next_ts - 1) >= 8 it is ok */
			if ((length - ptr_next_ts) < 7 )
				return -ENOSPC;
			*ts_ptr++ = ip_address;
			*ts_ptr = ut;
			old_val = TS_OPT_GET_FIRST_WORD();
			TS_OPT_SET_PTR(8);
			new_val = TS_OPT_GET_FIRST_WORD();
			/* need updating IPv4 header checksum */
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
					    old_val,
					    new_val);
			/* need updating IPv4 header checksum */
			/* rfc says old val must be 0 */
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
					    0,
					    ip_address);
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
					    0,
					    ut);
			break;
		case AIOP_IPOPT_TS_PRESPEC:
			/* if length - (ptr_next_ts - 1) >= 8 it is ok */
			if ((length - ptr_next_ts) < 7 )
				return -ENOSPC;
			if (*ts_ptr == ip_address)
			{
				ts_ptr++;
				*ts_ptr = ut;
				old_val = TS_OPT_GET_FIRST_WORD();
				TS_OPT_SET_PTR(8);
				new_val = TS_OPT_GET_FIRST_WORD();
				/* need updating IPv4 header checksum */
				cksum_update_uint32(&ipv4_hdr->hdr_cksum,
						    old_val,
						    new_val);
				/* need updating IPv4 header checksum */
				/* rfc says old val must be 0 */
				cksum_update_uint32(&ipv4_hdr->hdr_cksum,
						    0,
						    ut);
			}
			else
				return SUCCESS;
			break;
		default:
			break;
		}
	}
	
	fdma_size += ptr_next_ts - 1;
	fdma_modify_default_segment_data(ipv4_offset, fdma_size);
	return SUCCESS;
}

void ipv6_mangle(uint8_t flags, uint8_t dscp, uint8_t hop_limit, 
		uint32_t flow_label)
{
	struct   ipv6hdr *ipv6hdr_ptr;
	uint16_t ipv6hdr_offset;
	uint32_t val;

	ipv6hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	struct   parse_result *pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	ipv6hdr_ptr = (struct ipv6hdr *)
			(ipv6hdr_offset + PRC_GET_SEGMENT_ADDRESS());

	if (flags & IPV6_MANGLE_DSCP)
	{
		val = ipv6hdr_ptr->vsn_traffic_flow;
		/* clear DSCP field in TOS - 6 msb in traffic class 
		 * field*/
		val &= ~0x0fc00000;
		val |= dscp << 22;
		ipv6hdr_ptr->vsn_traffic_flow = val;

	}
	if (flags & IPV6_MANGLE_HOP_LIMIT)
		ipv6hdr_ptr->hop_limit = hop_limit;
	if (flags & IPV6_MANGLE_FLOW_LABEL)
	{
		val = ipv6hdr_ptr->vsn_traffic_flow;
		/* set flow_label - 20 lsb */
		val &= 0xfff00000;
		val |= flow_label;
		ipv6hdr_ptr->vsn_traffic_flow = val;
	}
	/* update IP header in FDMA */
	fdma_modify_default_segment_data(ipv6hdr_offset, 8);
	/* Invalidate gross running sum */
	pr->gross_running_sum = 0;
}

void ipv6_dec_hop_limit_modification(void)
{
	struct   ipv6hdr *ipv6hdr_ptr;
	struct   parse_result *pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	uint16_t ipv6hdr_offset;

	ipv6hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv6hdr_ptr = (struct ipv6hdr *)
			(ipv6hdr_offset + PRC_GET_SEGMENT_ADDRESS());
	ipv6hdr_ptr->hop_limit = ipv6hdr_ptr->hop_limit - 1;
	/* update IP header in FDMA */
	fdma_modify_default_segment_data(ipv6hdr_offset + 7, 1);
	/* Invalidate gross running sum */
	pr->gross_running_sum = 0;
}

int ipv4_header_modification(uint8_t flags, uint8_t tos, uint16_t id,
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

		/* Reset IP checksum for re-calculation by FDMA */
		ipv4hdr_ptr->hdr_cksum = 0;

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
					/* calculate IP checksum and update
					   IP checksum in FDMA*/
					ip_cksum_calculate(ipv4hdr_ptr,
						IP_CKSUM_CALC_MODE_NONE);
					/* Invalidate gross running sum */
					pr->gross_running_sum = 0;
				} else{
				/* update IP header in FDMA */
					fdma_modify_default_segment_data(
							ipv4hdr_offset,
							20);
					/* calculate IP checksum and update
					   IP checksum in FDMA*/
					ip_cksum_calculate(ipv4hdr_ptr,
						IP_CKSUM_CALC_MODE_NONE);
					/* calculate IP checksum */
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
				/* calculate IP checksum and update
				   IP checksum in FDMA*/
				ip_cksum_calculate(ipv4hdr_ptr,
						IP_CKSUM_CALC_MODE_NONE);

				/* Invalidate gross running sum */
				pr->gross_running_sum = 0;
			}
		} else {
			/* update IP header in FDMA */
			fdma_modify_default_segment_data(ipv4hdr_offset,
							  20);
			/* calculate IP checksum and update
			   IP checksum in FDMA*/
			ip_cksum_calculate(ipv4hdr_ptr,
					IP_CKSUM_CALC_MODE_NONE);
		}
	return SUCCESS;
	} else {
		return NO_IP_HDR_ERROR; }
}

int ipv6_header_modification(uint8_t flags, uint8_t tc,
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

		/* Update changes in FDMA */
		fdma_modify_default_segment_data(ipv6hdr_offset,
						 40);

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

int ipv4_header_encapsulation(uint8_t flags,
				  void *ipv4header, uint8_t ipv4_header_size)
{
	struct ipv4hdr *inner_ipv4hdr_ptr;
	struct ipv4hdr *outer_ipv4hdr_ptr;
	struct ipv6hdr *inner_ipv6hdr_ptr;
	uint32_t	fdma_flags;
	uint16_t	inner_ipv4_offset;
	uint16_t	outer_ipv4_offset;
	uint16_t	inner_ipv6_offset;
	uint16_t	mpls_offset;
	uint16_t	etype_offset;
	uint16_t	*etype_ptr;
	uint32_t	*mpls_ptr;
	uint32_t	vsn_traffic_flow;
	uint8_t		tos, inner_tos;
	uint64_t	orig_first_8_bytes;
	uint32_t	orig_ttl_and_chksum;
	struct presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	struct ipv4hdr *ipv4_header_ptr = ipv4header;

	/* Save original fields of input IP header */
	orig_first_8_bytes	= *((uint64_t *)ipv4_header_ptr);
	orig_ttl_and_chksum	= *(((uint32_t *)ipv4_header_ptr+2));

	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		/* Reset IP checksum for re-calculation by FDMA */
		ipv4_header_ptr->hdr_cksum = 0;

		ipv4_header_ptr->protocol = NET_IPV4_PROT_IPV4;

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

		/* Insert IPv4 header*/
		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_REPRESENT_BIT);

		fdma_insert_default_segment_data(
						    inner_ipv4_offset,
						    (void *)ipv4_header_ptr,
						    (uint16_t) ipv4_header_size,
						    fdma_flags);

		/* Restore original fields of input IP header */
		*((uint64_t *)ipv4_header_ptr)	   = orig_first_8_bytes;
		*(((uint32_t *)ipv4_header_ptr+2)) = orig_ttl_and_chksum;

		/* Re-run parser */
		parse_result_generate_default(0);

		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;

		/* Calculate outer IP checksum */
		outer_ipv4_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		outer_ipv4hdr_ptr = (struct ipv4hdr *)
				(outer_ipv4_offset + PRC_GET_SEGMENT_ADDRESS());

		/* calculate IP checksum and update
		   IP checksum in FDMA*/
		ip_cksum_calculate(outer_ipv4hdr_ptr,
				IP_CKSUM_CALC_MODE_NONE);
		return SUCCESS;

	} else if (PARSER_IS_OUTER_IPV6_DEFAULT()) {
		
		/* Update Etype or MPLS label if needed */
		if (PARSER_IS_ONE_MPLS_DEFAULT()) {
			mpls_offset = PARSER_GET_LAST_MPLS_OFFSET_DEFAULT();
			mpls_ptr = (uint32_t *)
					(mpls_offset + prc->seg_address);
			*mpls_ptr = (*mpls_ptr & MPLS_LABEL_MASK) |
					NET_MPLS_LABEL_IPV4;
			fdma_modify_default_segment_data(mpls_offset, 3);
		} else {
			if (PARSER_IS_ETH_MAC_DEFAULT()) {
				etype_offset =
					PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT();
				etype_ptr = (uint16_t *)
					      (etype_offset + prc->seg_address);
				*etype_ptr = NET_ETH_ETYPE_IPV4;
				fdma_modify_default_segment_data(etype_offset,
								 2);
			}
		}
		
		/* Reset IP checksum for re-calculation by FDMA */
		ipv4_header_ptr->hdr_cksum = 0;

		ipv4_header_ptr->protocol = NET_IPV4_PROT_IPV6;

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

		/* Insert IPv4 header*/
		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_REPRESENT_BIT);

		fdma_insert_default_segment_data(inner_ipv6_offset,
						 (void *)ipv4_header_ptr,
						 (uint16_t) ipv4_header_size,
						 fdma_flags);

		/* Restore original fields of input IP header */
		*((uint64_t *)ipv4_header_ptr)	   = orig_first_8_bytes;
		*(((uint32_t *)ipv4_header_ptr+2)) = orig_ttl_and_chksum;

		/* Re-run parser */
		parse_result_generate_default(0);

		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;

		/* Calculate outer IP checksum */
		outer_ipv4_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		outer_ipv4hdr_ptr = (struct ipv4hdr *) (outer_ipv4_offset +
							      prc->seg_address);

		/* calculate IP checksum and update
		   IP checksum in FDMA*/
		ip_cksum_calculate(outer_ipv4hdr_ptr,
				IP_CKSUM_CALC_MODE_NONE);

		return SUCCESS;
	} else { /* no inner IP */
		return NO_IP_HDR_ERROR;
	}
}

int ipv6_header_encapsulation(uint8_t flags, void *ipv6header,
				  uint8_t ipv6_header_size)
{
	struct ipv4hdr *inner_ipv4hdr_ptr;
	struct ipv6hdr *inner_ipv6hdr_ptr;
	uint32_t	fdma_flags;
	uint16_t	inner_ipv4_offset;
	uint16_t	inner_ipv6_offset;
	uint16_t	mpls_offset;
	uint16_t	etype_offset;
	uint16_t	*etype_ptr;
	uint32_t	*mpls_ptr;
	uint32_t	vsn_traffic_flow;
	uint8_t		inner_tos;
	uint32_t	orig_tc_and_flow_label;
	uint32_t	orig_length_and_hop_limit;
	struct presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	struct   ipv6hdr *ipv6_header_ptr = ipv6header;

	/* Save original fields of input IP header */
	orig_tc_and_flow_label = *((uint32_t *)ipv6_header_ptr);
	orig_length_and_hop_limit = *(((uint32_t *)ipv6_header_ptr+1));


	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		
		/* Update Etype or MPLS label if needed */
		if (PARSER_IS_ONE_MPLS_DEFAULT()) {
			mpls_offset = PARSER_GET_LAST_MPLS_OFFSET_DEFAULT();
			mpls_ptr = (uint32_t *)
					(mpls_offset + prc->seg_address);
			*mpls_ptr = (*mpls_ptr & MPLS_LABEL_MASK) |
							   NET_MPLS_LABEL_IPV6;
			fdma_modify_default_segment_data(mpls_offset, 3);
		} else {
			if (PARSER_IS_ETH_MAC_DEFAULT()) {
				etype_offset =
					PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT();
				etype_ptr = (uint16_t *)
					     (etype_offset + prc->seg_address);
				*etype_ptr = NET_ETH_ETYPE_IPV6;
				fdma_modify_default_segment_data(etype_offset,
								 2);
			}
		}

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
		(((uint32_t)(inner_tos & ~IPV4_DSCP_MASK))<<20);
		if (flags & IPV6_ENCAP_MODE_TC_ECN)
			vsn_traffic_flow =
				(vsn_traffic_flow & IPV6_ECN_MASK) |
			(((uint32_t)((inner_tos & ~IPV4_ECN_MASK)))<<20);

		ipv6_header_ptr->vsn_traffic_flow = vsn_traffic_flow;

		ipv6_header_ptr->payload_length =
				inner_ipv4hdr_ptr->total_length
					+ (uint16_t)ipv6_header_size - 40;

		/* Insert IPv6 header */
		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_REPRESENT_BIT);

		fdma_insert_default_segment_data(inner_ipv4_offset,
						 (void *)ipv6_header_ptr,
						 (uint16_t) ipv6_header_size,
						 fdma_flags);

		/* Restore original fields of input IP header */
		*((uint32_t *)ipv6_header_ptr) = orig_tc_and_flow_label;
		*(((uint32_t *)ipv6_header_ptr+1)) = orig_length_and_hop_limit;


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
	       (inner_ipv6hdr_ptr->vsn_traffic_flow & ~IPV6_ECN_MASK);

		ipv6_header_ptr->vsn_traffic_flow = vsn_traffic_flow;

		fdma_flags = (uint32_t)(FDMA_REPLACE_SA_REPRESENT_BIT);

		ipv6_header_ptr->payload_length =
					inner_ipv6hdr_ptr->payload_length +
					(uint16_t) ipv6_header_size;

		fdma_insert_default_segment_data(inner_ipv6_offset,
						 (void *)ipv6_header_ptr,
						 (uint16_t) ipv6_header_size,
						 fdma_flags);

		/* Restore original fields of input IP header */
		*((uint32_t *)ipv6_header_ptr) = orig_tc_and_flow_label;
		*(((uint32_t *)ipv6_header_ptr+1)) = orig_length_and_hop_limit;

		/* Re-run parser */
		parse_result_generate_default(0);

		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;

		return SUCCESS;
	} else { /* no inner IP */
		return NO_IP_HDR_ERROR;
	}
}

int ip_set_nw_src(uint32_t src_addr)
{
	uint16_t ipv4hdr_offset;
	uint16_t udp_tcp_offset;
	uint16_t udp_checksum;
	uint32_t old_src_add;
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

		old_src_add = ipv4hdr_ptr->src_addr;
		ipv4hdr_ptr->src_addr = src_addr;

		/* update FDMA */
		fdma_modify_default_segment_data(ipv4hdr_offset+10, 6);

		/* update UDP/TCP checksum */
		udp_tcp_offset = PARSER_GET_L4_OFFSET_DEFAULT();
		if (PARSER_IS_UDP_DEFAULT()) {
			udphdr_ptr = (struct udphdr *) ((uint16_t)udp_tcp_offset
						+ PRC_GET_SEGMENT_ADDRESS());

			if (udphdr_ptr->checksum != 0) {
				udp_checksum = udphdr_ptr->checksum;
				udp_checksum = cksum_accumulative_update_uint32(
								 udp_checksum,
								 old_src_add,
								 src_addr);

			if (udp_checksum == 0)
				udp_checksum = (uint16_t) ~udp_checksum;
			udphdr_ptr->checksum = udp_checksum;

			/* update FDMA */
			fdma_modify_default_segment_data(udp_tcp_offset + 6, 2);

			/* Invalidate gross running sum */
			pr->gross_running_sum = 0;
			}
		} else if (PARSER_IS_TCP_DEFAULT()) {
			tcphdr_ptr = (struct tcphdr *) ((uint16_t)udp_tcp_offset
					+ PRC_GET_SEGMENT_ADDRESS());

			cksum_update_uint32(&tcphdr_ptr->checksum,
								old_src_add,
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

int ip_set_nw_dst(uint32_t dst_addr)
{
	uint16_t ipv4hdr_offset;
	uint16_t udp_tcp_offset;
	uint16_t udp_checksum;
	uint32_t old_dst_addr;
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

		old_dst_addr = ipv4hdr_ptr->dst_addr;
		ipv4hdr_ptr->dst_addr = dst_addr;

		/* update FDMA */
		fdma_modify_default_segment_data(ipv4hdr_offset+10, 10);

		/* update UDP/TCP checksum */
		udp_tcp_offset = PARSER_GET_L4_OFFSET_DEFAULT();
		if (PARSER_IS_UDP_DEFAULT()) {
			udphdr_ptr = (struct udphdr *) ((uint16_t)udp_tcp_offset
						+ PRC_GET_SEGMENT_ADDRESS());

			if (udphdr_ptr->checksum != 0) {
				udp_checksum = udphdr_ptr->checksum;
				udp_checksum = cksum_accumulative_update_uint32(
								 udp_checksum,
								 old_dst_addr,
								 dst_addr);

			if (udp_checksum == 0)
				udp_checksum = (uint16_t) ~udp_checksum;
			udphdr_ptr->checksum = udp_checksum;
			/* update FDMA */
			fdma_modify_default_segment_data(udp_tcp_offset + 6, 2);

			/* Invalidate gross running sum */
			pr->gross_running_sum = 0;
			}

		} else if (PARSER_IS_TCP_DEFAULT()) {
			tcphdr_ptr = (struct tcphdr *) ((uint16_t)udp_tcp_offset
					+ PRC_GET_SEGMENT_ADDRESS());

			cksum_update_uint32(&tcphdr_ptr->checksum,
								old_dst_addr,
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



HM_CODE_PLACEMENT void ip_cksum_calculate(struct ipv4hdr *ipv4header, uint8_t flags)
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

	/* Call FDMA for running sum computation */
	fdma_calculate_default_frame_checksum(offset, ihl, &running_sum);

	/* Invalidate Parser Result Gross Running Sum field */
	pr->gross_running_sum = 0;

	/* Subtract old checksum and invert (this can result in 0x0000 !) */
	running_sum = cksum_ones_complement_sum16((uint16_t)~running_sum,
						  ipv4header->hdr_cksum);

	/* Write 1's complement of the 1's complement 16 bit sum into the
	IPv4 header */
	ipv4header->hdr_cksum = running_sum;

	/* Update FDMA */
	if (!(flags & IP_CKSUM_CALC_MODE_DONT_UPDATE_FDMA)) {
		fdma_modify_default_segment_data(offset +
				offsetof(struct ipv4hdr, hdr_cksum),
				sizeof((struct ipv4hdr *)0)->hdr_cksum);
	}

	return;
}

HM_CODE_PLACEMENT uint32_t ipv6_last_header(struct ipv6hdr *ipv6_hdr, uint8_t flag)
{
	uint32_t current_hdr_ptr;
	uint16_t current_hdr_size;
	uint16_t last_hdr_size;
	uint8_t current_ver;
	uint8_t next_hdr;
	uint8_t ah_ext;
	uint8_t dst_ext;
	uint8_t frag_ext;
	uint8_t no_extension;

	no_extension = 0;

	if (flag == LAST_HEADER_BEFORE_FRAG) {
		ah_ext = no_extension;
		frag_ext = no_extension;
		last_hdr_size = 0;

	/* flag == LAST_HEADER */
	} else {
		ah_ext = IPV6_EXT_AH;
		frag_ext = IPV6_EXT_FRAGMENT;
	}

	/* Destination extension can appear only once on fragment request */
	dst_ext = IPV6_EXT_DESTINATION;

	/* Copy initials IPv6 header */
	current_hdr_ptr = (uint32_t)ipv6_hdr;
	current_hdr_size = sizeof(struct ipv6hdr);
	next_hdr = ipv6_hdr->next_header;

	/* Skip to next extension header until extension isn't ipv6 header
	 * or until extension is the fragment position (depend on flag) */
	while ((next_hdr == IPV6_EXT_HOP_BY_HOP) ||
		(next_hdr == IPV6_EXT_ROUTING) || (next_hdr == dst_ext) ||
		(next_hdr == ah_ext) ||
		(next_hdr == frag_ext)) {

		current_ver = next_hdr;
		current_hdr_ptr += current_hdr_size;
		next_hdr = *((uint8_t *)(current_hdr_ptr));
		current_hdr_size = *((uint8_t *)(current_hdr_ptr + 1));

		/* Calculate current extension size  */
		switch (current_ver) {

		case IPV6_EXT_DESTINATION:
		{
			current_hdr_size = ((current_hdr_size + 1) << 3);

			if (flag == LAST_HEADER_BEFORE_FRAG) {
				/* After destination IS NOT routing extension */
				if (next_hdr != IPV6_EXT_ROUTING) {
				/* Hop by Hop or Routing extensions found */
					if (last_hdr_size) {
						return current_hdr_ptr -\
								last_hdr_size;
					} else {
						return ((uint32_t)ipv6_hdr +\
							IPV6_NEXT_HDR_OFFSET) \
							| IPV6_NO_EXTENSION;
					}
				}

				/* Disable destination extension */
				dst_ext = no_extension;
			}

			break;
		}

		case IPV6_EXT_AH:
		{
			current_hdr_size = ((current_hdr_size + 2) << 2);
			break;
		}

		case IPV6_EXT_FRAGMENT:
		{
			current_hdr_size = IPV6_FRAGMENT_HEADER_LENGTH;
			break;
		}

		/* Routing or Hop By Hop */
		default:
		{
			current_hdr_size = ((current_hdr_size + 1) << 3);
			last_hdr_size = current_hdr_size;
			break;
		}
		}
	}

	/* return last extension pointer and extension indicator */
	if (current_hdr_ptr == (uint32_t)ipv6_hdr) {
		current_hdr_ptr = ((current_hdr_ptr + IPV6_NEXT_HDR_OFFSET) \
							| IPV6_NO_EXTENSION);
	}

	return current_hdr_ptr;
}
