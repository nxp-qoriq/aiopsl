/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File		cwapf.c

@Description	This file contains the AIOP SW CAPWAP Fragmentation
		implementation.

*//***************************************************************************/
#include "general.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_ldpaa.h"
#include "fsl_checksum.h"
#include "cwapf.h"
#include "net.h"
#include "fsl_stdlib.h"

extern __TASK struct aiop_default_task_params default_task_params;

CWAPF_CODE_PLACEMENT int cwapf_move_remaining_frame(
		struct cwapf_context *cwapf_ctx)
{
	int32_t	status;
	struct fdma_amq amq;

	status = fdma_store_default_frame_data();
	if (status < 0)
		return status; /* Received packet cannot be stored due to
				buffer pool depletion (status = (-ENOMEM)).*/

	/* Copy default FD to remaining_FD in IPF ctx */
	cwapf_ctx->rem_fd = *((struct ldpaa_fd *)HWC_FD_ADDRESS);

	/* Present the remaining FD */
	status = fdma_present_frame_without_segments(&(cwapf_ctx->rem_fd),
						FDMA_PRES_NO_FLAGS, 0,
						&(cwapf_ctx->rem_frame_handle));

	/* Try to store the frame*/
	if (status == (-EIO)) {
		if (fdma_store_frame_data(cwapf_ctx->rem_frame_handle,
				*((uint8_t *) HWC_SPID_ADDRESS),
				&amq) < 0)
			return -ENOMEM;
		 else
			return status; /* Received packet FD contain errors
				(FD.err != 0). (status = (-EIO)).*/
	} else {
		return status;
	}

	return SUCCESS;
}

CWAPF_CODE_PLACEMENT void cwapf_after_split_fragment(
		struct cwapf_context *cwapf_ctx)
{
	struct fdma_present_segment_params present_segment_params;
	struct fdma_insert_segment_data_params insert_segment_data_params;
	struct ipv4hdr *ipv4_hdr;
	struct capwaphdr *capwap_hdr;
	struct udphdr *udp_hdr;
	uint16_t headers_length, frag_offset, ip_header_length, udp_length;
	uint16_t ip_total_length;
	uint8_t ws_dst_dummy;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	capwap_hdr = (struct capwaphdr *) (CWAPF_GET_CAPWAP_HDR_OFFSET() +
			PRC_GET_SEGMENT_ADDRESS());

	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		ipv4_hdr = (struct ipv4hdr *)
				(PARSER_GET_OUTER_IP_OFFSET_DEFAULT() +
				PRC_GET_SEGMENT_ADDRESS());
		ip_header_length = (uint16_t) (ipv4_hdr->vsn_and_ihl &
			IPV4_HDR_IHL_MASK) << 2;
	}

	if (cwapf_ctx->first_frag) {
		if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
			ip_total_length = (uint16_t)
				LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT() -
				cwapf_ctx->prc_seg_offset;
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
				ipv4_hdr->total_length,
				ip_total_length);
			ipv4_hdr->total_length = ip_total_length;
		}
		frag_offset = 0;
	} else {
		frag_offset = ( (cwapf_ctx->prev_frag_offset +
				cwapf_ctx->mtu_payload_length) >> 3);
	}

	cwapf_ctx->prev_frag_offset = frag_offset * 8;

	if (PARSER_IS_UDP_DEFAULT()) {
		udp_hdr = (struct udphdr *) ((uint16_t)(
			PARSER_GET_L4_OFFSET_DEFAULT() +
			PRC_GET_SEGMENT_ADDRESS()));
		udp_length = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			(uint16_t)PARSER_GET_L4_OFFSET_DEFAULT() -
			cwapf_ctx->prc_seg_offset;
		udp_hdr->length = udp_length;
		udp_hdr->checksum = 0;
	}

	capwap_hdr->offset_rsvd =
		( capwap_hdr->offset_rsvd &
			~NET_HDR_FLD_CAPWAP_FRAG_OFFSET_MASK ) |
			(frag_offset << NET_HDR_FLD_CAPWAP_FRAG_OFFSET_OFFSET);
	capwap_hdr->bits_flags = capwap_hdr->bits_flags |
			NET_HDR_FLD_CAPWAP_F;

	/* Run parser */
	parse_result_generate_default(PARSER_NO_FLAGS);

	/* Compute the total length between IP and CAPWAP headers, including
	 * CAPWAP header length */
	headers_length = (uint16_t)(CWAPF_GET_CAPWAP_HDR_OFFSET() +
				CWAPF_GET_CAPWAP_HDR_LENGTH(capwap_hdr));

	/* Modify all headers up to CAPWAP (including the latter) in FDMA.
	 * This ensures updating IP, UDP headers if present. At this tome, no
	 * performance penalty is known if we update more than it is strictly
	 * necessary */
	fdma_modify_default_segment_data(0, headers_length);

	present_segment_params.flags = FDMA_PRES_NO_FLAGS;
	present_segment_params.frame_handle = cwapf_ctx->rem_frame_handle;
	present_segment_params.offset = 0;

	/* Change presentation size to 1 as a w/a for TKT280408 */
	present_segment_params.present_size = 1;
	present_segment_params.ws_dst = &ws_dst_dummy;
	/* present empty segment of the remaining frame */
	fdma_present_frame_segment(&present_segment_params);

	if (cwapf_ctx->first_frag) {
		/* TODO Handle options */
		cwapf_ctx->first_frag = 0;
	}

	insert_segment_data_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
	insert_segment_data_params.frame_handle = cwapf_ctx->rem_frame_handle;
	insert_segment_data_params.to_offset = 0;
	insert_segment_data_params.insert_size = headers_length;
	insert_segment_data_params.from_ws_src =
					(void *)PRC_GET_SEGMENT_ADDRESS();
	insert_segment_data_params.seg_handle =
				present_segment_params.seg_handle;
	/* Insert the header to the remaining frame, close segment */
	fdma_insert_segment_data(&insert_segment_data_params);
}

CWAPF_CODE_PLACEMENT int cwapf_last_frag(struct cwapf_context *cwapf_ctx)
{
	int32_t	status;
	struct fdma_amq isolation_attributes;
	struct ipv4hdr *ipv4_hdr;
	struct capwaphdr *capwap_hdr;
	struct udphdr *udp_hdr;
	uint16_t ip_total_length, frag_offset, udp_length, ip_header_length;
	uint8_t spid;

	spid = *((uint8_t *)HWC_SPID_ADDRESS);
	status = fdma_store_frame_data(cwapf_ctx->rem_frame_handle, spid,
		 &isolation_attributes);
	if (status < 0)
		return status;/* Last fragment cannot be stored due to
				buffer pool depletion (status = (-ENOMEM)).*/

	/* Copy remaining_FD to default FD */
	*((struct ldpaa_fd *)HWC_FD_ADDRESS) = cwapf_ctx->rem_fd;
	/* present fragment + header segment */
	fdma_present_default_frame();

	frag_offset = ((cwapf_ctx->prev_frag_offset +
		cwapf_ctx->mtu_payload_length)>>3);

	capwap_hdr = (struct capwaphdr *)(
			(uint16_t)(CWAPF_GET_CAPWAP_HDR_OFFSET() +
			PRC_GET_SEGMENT_ADDRESS()));
	capwap_hdr->offset_rsvd =
		( capwap_hdr->offset_rsvd &
			~NET_HDR_FLD_CAPWAP_FRAG_OFFSET_MASK ) |
			(frag_offset << NET_HDR_FLD_CAPWAP_FRAG_OFFSET_OFFSET);
	capwap_hdr->bits_flags = capwap_hdr->bits_flags |
			NET_HDR_FLD_CAPWAP_F | NET_HDR_FLD_CAPWAP_L;

	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		ipv4_hdr = (struct ipv4hdr *)((uint16_t)(
			PARSER_GET_OUTER_IP_OFFSET_DEFAULT() +
			PRC_GET_SEGMENT_ADDRESS()));
		ip_header_length = (uint16_t) (ipv4_hdr->vsn_and_ihl &
			IPV4_HDR_IHL_MASK) << 2;
		ip_total_length = (uint16_t)
			LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT() -
			cwapf_ctx->prc_seg_offset;
		cksum_update_uint32(&ipv4_hdr->hdr_cksum,
			ipv4_hdr->total_length,
			ip_total_length);
		ipv4_hdr->total_length = ip_total_length;
	}

	if (PARSER_IS_UDP_DEFAULT()) {
		udp_hdr = (struct udphdr *) ((uint16_t)(
			PARSER_GET_L4_OFFSET_DEFAULT() +
			PRC_GET_SEGMENT_ADDRESS()));
		udp_length = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				(uint16_t)PARSER_GET_L4_OFFSET_DEFAULT() -
				cwapf_ctx->prc_seg_offset;
		udp_hdr->length = udp_length;
		udp_hdr->checksum = 0;
	}

	/* Run parser */
	parse_result_generate_default(PARSER_NO_FLAGS);

	/* Modify all headers in FDMA memory */
	fdma_modify_default_segment_data(0, (uint16_t)(
		CWAPF_GET_CAPWAP_HDR_OFFSET() +
		CWAPF_GET_CAPWAP_HDR_LENGTH(capwap_hdr)));

	return CWAPF_GEN_FRAG_STATUS_DONE;
}

CWAPF_CODE_PLACEMENT int cwapf_split_fragment(struct cwapf_context *cwapf_ctx)
{
	int32_t	status, split_status;
	struct fdma_split_frame_params split_frame_params;
	struct fdma_amq isolation_attributes;

	split_frame_params.fd_dst = (void *)HWC_FD_ADDRESS;
	split_frame_params.seg_dst = (void *)PRC_GET_SEGMENT_ADDRESS();
	split_frame_params.seg_offset = PRC_GET_SEGMENT_OFFSET();
	split_frame_params.present_size = PRC_GET_SEGMENT_LENGTH();
	split_frame_params.source_frame_handle =
					cwapf_ctx->rem_frame_handle;
	split_frame_params.spid = *((uint8_t *)HWC_SPID_ADDRESS);

	if (cwapf_ctx->remaining_payload_length >
				cwapf_ctx->mtu_payload_length) {
		/* Not last fragment, need to split */
		cwapf_ctx->remaining_payload_length =
				cwapf_ctx->remaining_payload_length -
				cwapf_ctx->mtu_payload_length;

		split_frame_params.split_size_sf = cwapf_ctx->split_size;

		/* Split remaining frame, put split frame in default FD
		 * location*/

		/* Due to HW ticket TKT240996 */
		status = fdma_store_frame_data(
				split_frame_params.source_frame_handle,
				split_frame_params.spid,
				&isolation_attributes);
		status = fdma_present_frame_without_segments(
				&(cwapf_ctx->rem_fd),
				FDMA_INIT_NO_FLAGS, 0,
				&(cwapf_ctx->rem_frame_handle));

		split_frame_params.source_frame_handle =
				cwapf_ctx->rem_frame_handle;

		split_frame_params.flags = FDMA_CFA_COPY_BIT |
					FDMA_SPLIT_PSA_NO_PRESENT_BIT;
		/* TODO FDMA ERROR */
		split_status = fdma_split_frame(&split_frame_params);
		status = fdma_store_default_frame_data();
		status = fdma_present_default_frame();

		cwapf_after_split_fragment(cwapf_ctx);

		return CWAPF_GEN_FRAG_STATUS_IN_PROCESS;

	} else {
	/* Last Fragment */
		status = cwapf_last_frag(cwapf_ctx);
		return status;
	}

}

CWAPF_CODE_PLACEMENT int cwapf_generate_frag(cwapf_ctx_t cwapf_context_addr)
{
	struct cwapf_context *cwapf_ctx =
		(struct cwapf_context *)cwapf_context_addr;

	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	int32_t	status;
	uint16_t ip_header_length = 0, mtu_payload_length,
		udp_header_length = 0;
	struct ipv4hdr *ipv4_hdr;
	struct capwaphdr *capwap_hdr;

	if (cwapf_ctx->first_frag) {
		/* First Fragment */

		/* Keep parser's parameters from task defaults */
		cwapf_ctx->parser_profile_id =
				default_task_params.parser_profile_id;
		cwapf_ctx->parser_starting_hxs =
				default_task_params.parser_starting_hxs;
		/* Keep PRC parameters */
		cwapf_ctx->prc_seg_address = PRC_GET_SEGMENT_ADDRESS();
		cwapf_ctx->prc_seg_length = PRC_GET_SEGMENT_LENGTH();
		cwapf_ctx->prc_seg_offset = PRC_GET_SEGMENT_OFFSET();

		capwap_hdr = (struct capwaphdr *)((uint16_t)(
			CWAPF_GET_CAPWAP_HDR_OFFSET() +
			PRC_GET_SEGMENT_ADDRESS()));

		mtu_payload_length = cwapf_ctx->mtu_payload_length -
			(uint16_t)CWAPF_GET_CAPWAP_HDR_LENGTH(capwap_hdr);

		cwapf_ctx->remaining_payload_length = (uint16_t)(
			LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			CWAPF_GET_CAPWAP_HDR_OFFSET() -
			CWAPF_GET_CAPWAP_HDR_LENGTH(capwap_hdr));

		if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
			ipv4_hdr = (struct ipv4hdr *)((uint16_t)(
				PARSER_GET_OUTER_IP_OFFSET_DEFAULT() +
				PRC_GET_SEGMENT_ADDRESS()));
			ip_header_length = (uint16_t) (ipv4_hdr->vsn_and_ihl &
				IPV4_HDR_IHL_MASK) << 2;
			mtu_payload_length -= ip_header_length;
		}

		if (PARSER_IS_UDP_DEFAULT()) {
			udp_header_length = (uint16_t)(
				CWAPF_GET_CAPWAP_HDR_OFFSET() -
				PARSER_GET_L4_OFFSET_DEFAULT());
			mtu_payload_length -= udp_header_length;
		}
		mtu_payload_length = mtu_payload_length & ~0x7;
		cwapf_ctx->mtu_payload_length = mtu_payload_length;

		cwapf_ctx->split_size = (uint16_t)(PRC_GET_SEGMENT_OFFSET() +
				CWAPF_GET_CAPWAP_HDR_OFFSET() +
				CWAPF_GET_CAPWAP_HDR_LENGTH(capwap_hdr) +
				mtu_payload_length);

		status = cwapf_move_remaining_frame(cwapf_ctx);
		if (status)
			return status;

		/* Clear gross running sum in parse results */
		pr->gross_running_sum = 0;
		status = cwapf_split_fragment(cwapf_ctx);
		return status;

	} else {
		/* Not first Fragment */
		/* Restore original parser's parameters in task default */
		default_task_params.parser_profile_id =
					cwapf_ctx->parser_profile_id;
		default_task_params.parser_starting_hxs =
					cwapf_ctx->parser_starting_hxs;
		/* Restore original PRC parameters */
		PRC_SET_SEGMENT_ADDRESS(cwapf_ctx->prc_seg_address);
		PRC_SET_SEGMENT_LENGTH(cwapf_ctx->prc_seg_length);
		PRC_SET_SEGMENT_OFFSET(cwapf_ctx->prc_seg_offset);

		/* Clear gross running sum in parse results */
		pr->gross_running_sum = 0;

		status = cwapf_split_fragment(cwapf_ctx);

		return status;
	}
}

int cwapf_discard_frame_remainder(cwapf_ctx_t cwapf_context_addr)
{
	struct cwapf_context *cwapf_ctx =
		(struct cwapf_context *)cwapf_context_addr;
	fdma_discard_frame(
			cwapf_ctx->rem_frame_handle, FDMA_DIS_NO_FLAGS);
	return SUCCESS;
}

CWAPF_CODE_PLACEMENT void cwapf_context_init(uint16_t mtu,
					     cwapf_ctx_t cwapf_context_addr)
{

#ifdef CHECK_ALIGNMENT
	DEBUG_ALIGN("ipf.c", (uint32_t *)cwapf_context_addr, ALIGNMENT_32B);
#endif

	struct cwapf_context *cwapf_ctx =
		(struct cwapf_context *)cwapf_context_addr;

	cwapf_ctx->first_frag = 1;
	cwapf_ctx->mtu_payload_length = mtu;
	cwapf_ctx->prev_frag_offset = 0;

}
