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
@File		gso.c

@Description	This file contains the AIOP SW TCP GSO API implementation

*//***************************************************************************/

#include "dplib/fsl_gso.h"
#include "gso.h"
#include "general.h"
#include "net/fsl_net.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "dplib/fsl_l4.h"
#include "checksum.h"
#include "common/fsl_stdlib.h"
////////#include "gro.h"

extern __TASK struct aiop_default_task_params default_task_params;

int tcp_gso_generate_seg(
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	int32_t	sr_status;
	uint8_t outer_ip_offset, tcp_offset;
	struct tcphdr *tcp_ptr;

	tcp_ptr = (struct tcphdr *)(PARSER_GET_L4_POINTER_DEFAULT());
	tcp_offset = (uint8_t)(PARSER_GET_L4_OFFSET_DEFAULT());
	outer_ip_offset = (uint8_t)(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	if (!(gso_ctx->first_seg)) {
		/* Restore parser's parameters  */
		default_task_params.parser_profile_id =
				gso_ctx->parser_profile_id;
		default_task_params.parser_starting_hxs =
				gso_ctx->parser_starting_hxs;
		/* Restore PRC parameters */
		PRC_SET_SEGMENT_ADDRESS(gso_ctx->seg_address);
		PRC_SET_SEGMENT_LENGTH(gso_ctx->seg_length);
		PRC_SET_SEGMENT_OFFSET(gso_ctx->seg_offset);

		/* Call to tcp_gso_split_segment */
		return tcp_gso_split_segment(gso_ctx);
	}

	/* save ip_offset */
	gso_ctx->ip_offset = outer_ip_offset;
	/* save tcp_offset */
	gso_ctx->tcp_offset = tcp_offset;

	if (tcp_ptr->flags & (NET_HDR_FLD_TCP_FLAGS_RST |
			NET_HDR_FLD_TCP_FLAGS_SYN))
		/* RST/SYN flags set */
		return TCP_GSO_GEN_SEG_STATUS_SYN_RST_SET;
	if (tcp_ptr->flags & NET_HDR_FLD_TCP_FLAGS_URG)
		/* URG flag set */
		gso_ctx->urgent_pointer = tcp_ptr->urgent_pointer;
	if (tcp_ptr->flags & NET_HDR_FLD_TCP_FLAGS_FIN) {
		/* FIN flag set */
		gso_ctx->internal_flags = gso_ctx->internal_flags |
				TCP_GSO_FIN_BIT;
		/* reset FIN */
		tcp_ptr->flags = tcp_ptr->flags & ~TCP_GSO_FIN_BIT;
	}
	if (tcp_ptr->flags & NET_HDR_FLD_TCP_FLAGS_PSH) {
		/* PSH flag set */
		gso_ctx->internal_flags = gso_ctx->internal_flags |
				TCP_GSO_PSH_BIT;
		/* reset PSH */
		tcp_ptr->flags = tcp_ptr->flags & ~TCP_GSO_PSH_BIT;
	}

	/* Modify default segment (updated TCP flags if needed) */
	if (gso_ctx->internal_flags != 0)
		fdma_modify_default_segment_data(
			(uint16_t)PARSER_GET_L4_OFFSET_DEFAULT() +
			(uint16_t)offsetof(struct tcphdr, flags), (uint16_t)(
			sizeof(tcp_ptr->flags)));

	/* Keep parser's parameters from task defaults */
	gso_ctx->parser_profile_id =
			default_task_params.parser_profile_id;
	gso_ctx->parser_starting_hxs =
			default_task_params.parser_starting_hxs;
	/* Keep PRC parameters */
	gso_ctx->seg_address = PRC_GET_SEGMENT_ADDRESS();
	gso_ctx->seg_length = PRC_GET_SEGMENT_LENGTH();
	gso_ctx->seg_offset = PRC_GET_SEGMENT_OFFSET();

	gso_ctx->headers_size = (uint16_t)
		((uint8_t)(PARSER_GET_L4_OFFSET_DEFAULT()) +
				((tcp_ptr->data_offset_reserved &
				NET_HDR_FLD_TCP_DATA_OFFSET_MASK) >>
				(NET_HDR_FLD_TCP_DATA_OFFSET_OFFSET -
				NET_HDR_FLD_TCP_DATA_OFFSET_SHIFT_VALUE)));
	gso_ctx->split_size = gso_ctx->headers_size + gso_ctx->mss;

	sr_status = fdma_store_default_frame_data();
	if (sr_status == (-ENOMEM))
		return sr_status; /* Received packet cannot be stored due to
		buffer pool depletion.*/

	/* Copy default FD to remaining_FD in GSO ctx */
	gso_ctx->rem_fd = *((struct ldpaa_fd *)HWC_FD_ADDRESS);

	sr_status = fdma_present_frame_without_segments(&(gso_ctx->rem_fd),
			FDMA_INIT_NO_FLAGS, 0, &(gso_ctx->rem_frame_handle));
	if (sr_status == (-EIO))
		return sr_status; /* Received packet FD contain errors
		(FD.err != 0).*/

	/* Call to tcp_gso_split_segment */
	return tcp_gso_split_segment(gso_ctx);
}

int32_t tcp_gso_split_segment(struct tcp_gso_context *gso_ctx)
{
	int32_t	status, sr_status, split_sr_status;
	uint16_t updated_ipv4_outer_total_length, l3checksum, ip_header_length;
	uint8_t spid, outer_ip_offset, outer_tcp_offset;
	struct tcphdr *tcp_ptr;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct fdma_split_frame_params split_frame_params;
	struct fdma_amq isolation_attributes;
	struct ipv4hdr *outer_ipv4_ptr;
	struct ipv6hdr *outer_ipv6_ptr;
	struct fdma_present_segment_params present_segment_params;
	struct fdma_insert_segment_data_params insert_segment_data_params;

	/* params for Split remaining frame */
	split_frame_params.flags = FDMA_CFA_COPY_BIT |
					FDMA_SPLIT_PSA_PRESENT_BIT;
	split_frame_params.fd_dst = (void *)HWC_FD_ADDRESS;
	split_frame_params.seg_dst = (void *)PRC_GET_SEGMENT_ADDRESS();
	split_frame_params.seg_offset = PRC_GET_SEGMENT_OFFSET();
	split_frame_params.present_size = PRC_GET_SEGMENT_LENGTH();
	split_frame_params.split_size_sf = gso_ctx->split_size;
	split_frame_params.source_frame_handle = gso_ctx->rem_frame_handle;
	split_frame_params.spid = *((uint8_t *) HWC_SPID_ADDRESS);

	/* Split remaining frame, put split frame in default FD location*/
#ifndef REV2
	sr_status = fdma_store_frame_data(split_frame_params.source_frame_handle,
			split_frame_params.spid, &isolation_attributes);
	sr_status = fdma_present_frame_without_segments(&(gso_ctx->rem_fd),
			FDMA_INIT_NO_FLAGS, 0, &(gso_ctx->rem_frame_handle));
	split_frame_params.flags = FDMA_CFA_COPY_BIT |
					FDMA_SPLIT_PSA_NO_PRESENT_BIT;
	split_sr_status = fdma_split_frame(&split_frame_params); /* TODO FDMA ERROR */
	if (split_sr_status != (-EINVAL)) {
		sr_status = fdma_store_default_frame_data();
		sr_status = fdma_present_default_frame();
	}
#else
	split_sr_status = fdma_split_frame(&split_frame_params); /* TODO FDMA ERROR */
#endif
	if (split_sr_status == (-EINVAL)) {
		/* last segment */
		spid = *((uint8_t *)HWC_SPID_ADDRESS);
		/* store remaining FD */
		sr_status = fdma_store_frame_data(gso_ctx->rem_frame_handle,
				spid, &isolation_attributes);
				/* TODO FDMA ERROR */
		/* Copy remaining FD to default FD */
		*((struct ldpaa_fd *)HWC_FD_ADDRESS) = gso_ctx->rem_fd;
		/* present frame + header segment */
		fdma_present_default_frame();

		status = TCP_GSO_GEN_SEG_STATUS_DONE;

		/* We didn't run the parser yet so the outer_ip_offset and
		 outer_tcp_offset are taken from gso_ctx. The parser will be
		 run only after the IP length is updated.  */
		outer_ip_offset = gso_ctx->ip_offset;
		outer_tcp_offset = gso_ctx->tcp_offset;
		outer_ipv4_ptr = (struct ipv4hdr *)(
				outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());
		outer_ipv6_ptr = (struct ipv6hdr *)(
				outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());
		tcp_ptr = (struct tcphdr *)(
				outer_tcp_offset + PRC_GET_SEGMENT_ADDRESS());

		/* update IP length */
		if (gso_ctx->ipv4) {
			/* IPv4 - update IP length */
			updated_ipv4_outer_total_length =
				(uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				outer_ip_offset);
			outer_ipv4_ptr->total_length =
					updated_ipv4_outer_total_length;
		} else {
			/* IPv6 - update IP length */
			outer_ipv6_ptr->payload_length =
				(uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
					outer_ip_offset - IPV6_HDR_LENGTH);
		}

		/* update TCP header flags */
		tcp_ptr->flags |= (gso_ctx->internal_flags & (TCP_GSO_FIN_BIT |
						TCP_GSO_PSH_BIT));

		/* sequence number calculation */
		tcp_ptr->sequence_number += gso_ctx->mss;

		/* urgent pointer calculation */
		if (tcp_ptr->urgent_pointer) {
			tcp_ptr->urgent_pointer = MIN(gso_ctx->mss,
						gso_ctx->urgent_pointer);
			if (!(tcp_ptr->urgent_pointer))
				/* reset URG */
				tcp_ptr->flags = tcp_ptr->flags &
				~NET_HDR_FLD_TCP_FLAGS_URG;
		}
	} else {
		/* First/middle segment */
		status = TCP_GSO_GEN_SEG_STATUS_IN_PROCESS;

		if (gso_ctx->first_seg) {
			/* Parser results are already updated */
			gso_ctx->first_seg = 0;

			outer_ip_offset = (uint8_t)
					(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

			tcp_ptr = (struct tcphdr *)(
					PARSER_GET_L4_POINTER_DEFAULT());

			ip_header_length = gso_ctx->split_size - (uint16_t)
					(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

			if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
				/* IPv4 - update IP length */
				gso_ctx->ipv4 = 1;
				outer_ipv4_ptr = (struct ipv4hdr *)(
					PARSER_GET_OUTER_IP_POINTER_DEFAULT());
				outer_ipv4_ptr->total_length = ip_header_length;
			} else {
				/* IPv6 - update IP length*/
				outer_ipv6_ptr = (struct ipv6hdr *)(
					PARSER_GET_OUTER_IP_POINTER_DEFAULT());
				outer_ipv6_ptr->payload_length =
					ip_header_length -IPV6_HDR_LENGTH;
			}

		} else {
			/* We didn't run the parser yet so the outer_ip_offset
			and outer_tcp_offset are taken from gso_ctx. The parser
			will be run only after the IP length is updated.  */
			outer_ip_offset = gso_ctx->ip_offset;
			outer_tcp_offset = gso_ctx->tcp_offset;
			outer_ipv4_ptr = (struct ipv4hdr *)(
				outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());
			outer_ipv6_ptr = (struct ipv6hdr *)(
				outer_ip_offset + PRC_GET_SEGMENT_ADDRESS());
			tcp_ptr = (struct tcphdr *)(
				outer_tcp_offset + PRC_GET_SEGMENT_ADDRESS());

			/* sequence number calculation */
			tcp_ptr->sequence_number += gso_ctx->mss;
			}

		/* urgent pointer calculation */
		if (tcp_ptr->urgent_pointer) {
			tcp_ptr->urgent_pointer = MIN(gso_ctx->mss,
						gso_ctx->urgent_pointer);
			if (tcp_ptr->urgent_pointer)
				gso_ctx->urgent_pointer -=
						tcp_ptr->urgent_pointer;
			else
				/* reset URG */
				tcp_ptr->flags = tcp_ptr->flags &
				~NET_HDR_FLD_TCP_FLAGS_URG;
		}

		/* Present empty segment of the remaining FD */
		present_segment_params.flags = FDMA_PRES_NO_FLAGS;
		present_segment_params.frame_handle = gso_ctx->rem_frame_handle;
		present_segment_params.offset = 0;
		present_segment_params.present_size = 0;
		fdma_present_frame_segment(&present_segment_params);

		/* Insert header to the remaining frame + close segment  */
		insert_segment_data_params.from_ws_src =
				(void *)PRC_GET_SEGMENT_ADDRESS();
		insert_segment_data_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
		insert_segment_data_params.to_offset = 0;
		insert_segment_data_params.frame_handle =
				gso_ctx->rem_frame_handle;
		insert_segment_data_params.insert_size = gso_ctx->headers_size;
		insert_segment_data_params.seg_handle =
				present_segment_params.seg_handle;
		/* TODO FDMA ERROR */
		sr_status = fdma_insert_segment_data(
				&insert_segment_data_params);

		/* IPv4 - ID generation */
		if (gso_ctx->ipv4)
			outer_ipv4_ptr->id = (uint16_t)fsl_os_rand();
		}

	/* Modify default segment */
	fdma_modify_default_segment_data((uint16_t)outer_ip_offset,
			(uint16_t)(gso_ctx->headers_size - outer_ip_offset));

	/* update TCP checksum */
	/////sr_status = l4_udp_tcp_cksum_calc(
	/////	L4_UDP_TCP_CKSUM_CALC_MODE_DONT_UPDATE_FDMA);


	/* Calculation of Gross Running Sum*/
	fdma_calculate_default_frame_checksum(
			0, 0xFFFF, &pr->gross_running_sum);

	/* Run parser */
	/* Update TCP checksum + Calculate IPv4 checksum */
	parse_result_generate_checksum(
			(enum parser_starting_hxs_code)
				   default_task_params.parser_starting_hxs,
			0,
			&l3checksum,
			&(tcp_ptr->checksum));

	/* Invalidate Parser Result Gross Running Sum field */
	pr->gross_running_sum = 0;

	/* Update IPv4 checksum */
	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
	/* IPv4 */
		outer_ipv4_ptr->hdr_cksum = l3checksum;
	/* Modify default segment (update IP checksum + TCP checksum) */
		fdma_modify_default_segment_data(
			(uint16_t)outer_ip_offset,(uint16_t)(
			gso_ctx->headers_size - outer_ip_offset));
	} else {
	/* IPv6 */
	/* Modify default segment (update TCP checksum) */
		fdma_modify_default_segment_data(
			(uint16_t)PARSER_GET_L4_OFFSET_DEFAULT() +
			(uint16_t)offsetof(struct tcphdr, checksum), (uint16_t)(
			sizeof(tcp_ptr->checksum)));
		}

	return status; /* Todo - return valid status*/
	}

int tcp_gso_discard_frame_remainder(
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	fdma_discard_frame(gso_ctx->rem_frame_handle, FDMA_DIS_NO_FLAGS);
	return SUCCESS;
}


void tcp_gso_context_init(
		uint32_t flags,
		uint16_t mss,
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	gso_ctx->first_seg = 1;
	gso_ctx->flags = flags;
	gso_ctx->split_size = 0;
	gso_ctx->urgent_pointer = 0;
	gso_ctx->mss = mss;
	gso_ctx->internal_flags = 0;
	gso_ctx->ipv4 = 0;
}
