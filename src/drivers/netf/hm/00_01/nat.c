/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		nat.c

@Description	This file contains the NAT header modification API
		implementation.

*//***************************************************************************/

#include "general.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_nat.h"
#include "fsl_l4.h"
#include "fsl_cdma.h"
#include "fsl_checksum.h"
#include "net.h"


int nat_ipv4(uint8_t flags, uint32_t ip_src_addr,
		uint32_t ip_dst_addr, uint16_t l4_src_port,
		uint16_t l4_dst_port, int16_t tcp_seq_num_delta,
		int16_t tcp_ack_num_delta)
{
	uint8_t l4_offset, ipv4_offset, modify_size;
	uint32_t old_header;
	struct tcphdr *tcp_ptr;
	struct ipv4hdr *ipv4_ptr;
	if (!PARSER_IS_TCP_OR_UDP_DEFAULT() || !PARSER_IS_OUTER_IPV4_DEFAULT())
		return NO_L4_IP_FOUND_ERROR;

	l4_offset = (uint8_t)(PARSER_GET_L4_OFFSET_DEFAULT());
	tcp_ptr = (struct tcphdr *)(l4_offset + PRC_GET_SEGMENT_ADDRESS());
	ipv4_offset = (uint8_t)(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	ipv4_ptr = (struct ipv4hdr *)(ipv4_offset + PRC_GET_SEGMENT_ADDRESS());

	modify_size = l4_offset - ipv4_offset + TCP_NO_OPTION_SIZE;

	PARSER_CLEAR_RUNNING_SUM();

	if (flags & NAT_MODIFY_MODE_L4_CHECKSUM) {
		if (flags & NAT_MODIFY_MODE_IPSRC) {
			old_header = ipv4_ptr->src_addr;
			ipv4_ptr->src_addr = ip_src_addr;
			cksum_update_uint32(&ipv4_ptr->hdr_cksum,
					old_header,
					ipv4_ptr->src_addr);
			if (!PARSER_IS_TUNNELED_IP_DEFAULT()) {
				if (PARSER_IS_TCP_DEFAULT())
					cksum_update_uint32(&tcp_ptr->checksum,
							old_header,
							ipv4_ptr->src_addr);
				else /* In case UDP header */
					cksum_update_uint32(
					  (uint16_t *)((uint16_t*)tcp_ptr+3),
					  old_header,
					  ipv4_ptr->src_addr);
			}
		}
		if (flags & NAT_MODIFY_MODE_IPDST) {
			old_header = ipv4_ptr->dst_addr;
			ipv4_ptr->dst_addr = ip_dst_addr;
			cksum_update_uint32(&ipv4_ptr->hdr_cksum,
					old_header,
					ipv4_ptr->dst_addr);
			if (!PARSER_IS_TUNNELED_IP_DEFAULT()) {
				if (PARSER_IS_TCP_DEFAULT())
					cksum_update_uint32(&tcp_ptr->checksum,
							    old_header,
							    ipv4_ptr->dst_addr);
				else /* In case UDP header */
					cksum_update_uint32(
					    (uint16_t *)((uint16_t*)tcp_ptr+3),
					    old_header,
					    ipv4_ptr->dst_addr);
			}
		}

		old_header = *(uint32_t *)tcp_ptr;
		if (flags & NAT_MODIFY_MODE_L4SRC)
			tcp_ptr->src_port = l4_src_port;

		if (flags & NAT_MODIFY_MODE_L4DST)
			tcp_ptr->dst_port = l4_dst_port;

		if (flags & (NAT_MODIFY_MODE_L4SRC |
				NAT_MODIFY_MODE_L4DST)) {
			if (PARSER_IS_TCP_DEFAULT())
				cksum_update_uint32(&tcp_ptr->checksum,
							old_header,
							*(uint32_t *)tcp_ptr);
			else /* In case UDP header */
				cksum_update_uint32(
					(uint16_t *)((uint16_t *)tcp_ptr+3),
					old_header,
					*(uint32_t *)tcp_ptr);
		}

		if (flags & NAT_MODIFY_MODE_TCP_SEQNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				fdma_modify_default_segment_data(ipv4_offset,
						modify_size);
				return NO_TCP_FOUND_ERROR;
			}
			old_header = tcp_ptr->sequence_number;
			/* todo need to verify if int16 is ok in the
			 * bellow addition */
			tcp_ptr->sequence_number += (int32_t)tcp_seq_num_delta;
			cksum_update_uint32(&tcp_ptr->checksum,
					old_header,
					tcp_ptr->sequence_number);
		}
		if (flags & NAT_MODIFY_MODE_TCP_ACKNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				fdma_modify_default_segment_data(ipv4_offset,
						modify_size);
				return NO_TCP_FOUND_ERROR;
			}
			old_header = tcp_ptr->acknowledgment_number;
			tcp_ptr->acknowledgment_number +=
					(int32_t)tcp_ack_num_delta;
			cksum_update_uint32(&tcp_ptr->checksum,
					old_header,
					tcp_ptr->acknowledgment_number);
		}

	} else {
		if (flags & NAT_MODIFY_MODE_IPSRC) {
			old_header = ipv4_ptr->src_addr;
			ipv4_ptr->src_addr = ip_src_addr;
			cksum_update_uint32(&ipv4_ptr->hdr_cksum,
					old_header,
					ipv4_ptr->src_addr);
		}
		if (flags & NAT_MODIFY_MODE_IPDST) {
			old_header = ipv4_ptr->dst_addr;
			ipv4_ptr->dst_addr = ip_dst_addr;
			cksum_update_uint32(&ipv4_ptr->hdr_cksum,
					old_header,
					ipv4_ptr->dst_addr);
		}

		if (flags & NAT_MODIFY_MODE_L4SRC)
			tcp_ptr->src_port = l4_src_port;

		if (flags & NAT_MODIFY_MODE_L4DST)
			tcp_ptr->dst_port = l4_dst_port;

		if (flags & NAT_MODIFY_MODE_TCP_SEQNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				fdma_modify_default_segment_data(ipv4_offset,
						modify_size);
				return NO_TCP_FOUND_ERROR;
			}
			/* todo need to verify if int16 is ok in the
			 * bellow addition */
			tcp_ptr->sequence_number += (int32_t)tcp_seq_num_delta;
		}
		if (flags & NAT_MODIFY_MODE_TCP_ACKNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				fdma_modify_default_segment_data(ipv4_offset,
						modify_size);
				return NO_TCP_FOUND_ERROR;
			}
			tcp_ptr->acknowledgment_number +=
					(int32_t)tcp_ack_num_delta;
		}
	}
	/* Modify the segment */
	fdma_modify_default_segment_data(ipv4_offset, modify_size);

	return SUCCESS;

}

int nat_ipv6(uint8_t flags, uint32_t *ip_src_addr,
		uint32_t *ip_dst_addr, uint16_t l4_src_port,
		uint16_t l4_dst_port, int16_t tcp_seq_num_delta,
		int16_t tcp_ack_num_delta)
{
	uint8_t l4_offset, ipv6_offset, i, modify_offset;
	uint32_t old_header;
	struct tcphdr *tcp_ptr;
	struct ipv6hdr *ipv6_ptr;
	if (!PARSER_IS_TCP_OR_UDP_DEFAULT() || !PARSER_IS_OUTER_IPV6_DEFAULT())
		return NO_L4_IP_FOUND_ERROR;

	l4_offset = (uint8_t)(PARSER_GET_L4_OFFSET_DEFAULT());
	tcp_ptr = (struct tcphdr *)(l4_offset + PRC_GET_SEGMENT_ADDRESS());
	ipv6_offset = (uint8_t)(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	ipv6_ptr = (struct ipv6hdr *)(ipv6_offset + PRC_GET_SEGMENT_ADDRESS());

	PARSER_CLEAR_RUNNING_SUM();

	if (flags & NAT_MODIFY_MODE_L4_CHECKSUM) {
		if (flags & NAT_MODIFY_MODE_IPSRC) {
			for (i = 0; i < 4; i++) {
				old_header = ipv6_ptr->src_addr[i];
				ipv6_ptr->src_addr[i] = ip_src_addr[i];
				if (!PARSER_IS_TUNNELED_IP_DEFAULT()) {
					if (PARSER_IS_TCP_DEFAULT())
					cksum_update_uint32(&tcp_ptr->checksum,
							old_header,
							ipv6_ptr->src_addr[i]);
					else /* In case UDP header */
					cksum_update_uint32(
						(uint16_t *)\
						((uint16_t *)tcp_ptr+3),
						old_header,
						ipv6_ptr->src_addr[i]);
				}
			}
		}
		if (flags & NAT_MODIFY_MODE_IPDST) {
			for (i = 0; i < 4; i++) {
				old_header = ipv6_ptr->dst_addr[i];
				ipv6_ptr->dst_addr[i] = ip_dst_addr[i];
				if (!PARSER_IS_TUNNELED_IP_DEFAULT()) {
					if (PARSER_IS_TCP_DEFAULT())
					cksum_update_uint32(&tcp_ptr->checksum,
							old_header,
							ipv6_ptr->dst_addr[i]);
					else /* In case UDP header */
					cksum_update_uint32(
						(uint16_t *)\
						((uint16_t *)tcp_ptr+3),
						old_header,
						ipv6_ptr->dst_addr[i]);
				}
			}

		}

		old_header = *(uint32_t *)tcp_ptr;
		if (flags & NAT_MODIFY_MODE_L4SRC)
			tcp_ptr->src_port = l4_src_port;
		if (flags & NAT_MODIFY_MODE_L4DST)
			tcp_ptr->dst_port = l4_dst_port;
		if (flags & (NAT_MODIFY_MODE_L4SRC |
				NAT_MODIFY_MODE_L4DST)) {
			if (PARSER_IS_TCP_DEFAULT())
				cksum_update_uint32(&tcp_ptr->checksum,
							old_header,
							*(uint32_t *)tcp_ptr);
			else /* In case UDP header */
				cksum_update_uint32(
					(uint16_t *)((uint16_t*)tcp_ptr+3),
					old_header,
					*(uint32_t *)tcp_ptr);
		}
		if (flags & NAT_MODIFY_MODE_TCP_SEQNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				/*from the IPv6 SRC addr */
				modify_offset = ipv6_offset + 8;
				fdma_modify_default_segment_data(modify_offset,
						IPV6_ADDR_SIZE);
				return NO_TCP_FOUND_ERROR;
			}
			old_header = tcp_ptr->sequence_number;
			/* todo need to verify if int16 is ok in the
			 * bellow addition */
			tcp_ptr->sequence_number += (int32_t)tcp_seq_num_delta;
			cksum_update_uint32(&tcp_ptr->checksum,
					old_header,
					tcp_ptr->sequence_number);
		}
		if (flags & NAT_MODIFY_MODE_TCP_ACKNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				/*from the IPv6 SRC addr */
				modify_offset = ipv6_offset + 8;
				fdma_modify_default_segment_data(modify_offset,
						IPV6_ADDR_SIZE);
				return NO_TCP_FOUND_ERROR;
			}
			old_header = tcp_ptr->acknowledgment_number;
			tcp_ptr->acknowledgment_number +=
					(int32_t)tcp_ack_num_delta;
			cksum_update_uint32(&tcp_ptr->checksum,
					old_header,
					tcp_ptr->acknowledgment_number);
		}

	} else {
		if (flags & NAT_MODIFY_MODE_IPSRC) {
			for (i = 0; i < 4; i++)
				ipv6_ptr->src_addr[i] = ip_src_addr[i];
		}
		if (flags & NAT_MODIFY_MODE_IPDST) {
			for (i = 0; i < 4; i++)
				ipv6_ptr->dst_addr[i] = ip_dst_addr[i];
		}

		if (flags & NAT_MODIFY_MODE_L4SRC)
			tcp_ptr->src_port = l4_src_port;
		if (flags & NAT_MODIFY_MODE_L4DST)
			tcp_ptr->dst_port = l4_dst_port;

		if (flags & NAT_MODIFY_MODE_TCP_SEQNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				/*from the IPv6 SRC addr */
				modify_offset = ipv6_offset + 8;
				fdma_modify_default_segment_data(modify_offset,
						IPV6_ADDR_SIZE);
				return NO_TCP_FOUND_ERROR;
			}
			/* todo need to verify if int16 is ok in the
			 * bellow addition */
			tcp_ptr->sequence_number += (int32_t)tcp_seq_num_delta;
		}
		if (flags & NAT_MODIFY_MODE_TCP_ACKNUM) {
			if (!PARSER_IS_TCP_DEFAULT()) {
				/*from the IPv6 SRC addr */
				modify_offset = ipv6_offset + 8;
				fdma_modify_default_segment_data(modify_offset,
						IPV6_ADDR_SIZE);
				return NO_TCP_FOUND_ERROR;
			}
			tcp_ptr->acknowledgment_number +=
					(int32_t)tcp_ack_num_delta;
		}
	}
	/*from the IPv6 SRC addr */
	modify_offset = ipv6_offset + 8;

	/* Modify the IPv6 header and TCP/UDP header */
	fdma_modify_default_segment_data(modify_offset,
			(uint16_t)((l4_offset-modify_offset)+TCP_NO_OPTION_SIZE));
	return SUCCESS;

}
