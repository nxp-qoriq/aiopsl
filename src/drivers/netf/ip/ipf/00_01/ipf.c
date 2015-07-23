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
@File		ipf.c

@Description	This file contains the AIOP SW IP Fragmentation implementation.

*//***************************************************************************/
#include "general.h"
#include "fsl_net.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_ldpaa.h"
#include "fsl_checksum.h"
#include "ipf.h"
#include "ip.h"
#include "fsl_stdlib.h"

extern __TASK struct aiop_default_task_params default_task_params;


int ipf_move_remaining_frame(struct ipf_context *ipf_ctx)
{
	int32_t	status;
	struct fdma_amq amq;
	
	status = fdma_store_default_frame_data();
	if (status < 0)
		return status; /* Received packet cannot be stored due to
				buffer pool depletion (status = (-ENOMEM)).*/
	
	/* Copy default FD to remaining_FD in IPF ctx */
	ipf_ctx->rem_fd = *((struct ldpaa_fd *)HWC_FD_ADDRESS);

	/* Present the remaining FD */
	status = fdma_present_frame_without_segments(&(ipf_ctx->rem_fd),
						FDMA_PRES_NO_FLAGS, 0,
						&(ipf_ctx->rem_frame_handle));
	
	/* Try to store the frame*/
	if (status == (-EIO)){
		if (fdma_store_frame_data(ipf_ctx->rem_frame_handle,
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


/*inline */void ipf_after_split_ipv4_fragment(struct ipf_context *ipf_ctx)
{
	struct fdma_present_segment_params present_segment_params;
	struct fdma_insert_segment_data_params insert_segment_data_params;
	struct ipv4hdr *ipv4_hdr;
	uint16_t header_length, frag_offset, ipv4_offset, payload_length;
	uint16_t ip_total_length;
	uint16_t flags_and_offset;

	if (ipf_ctx->first_frag) {
		ipv4_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv4_hdr = (struct ipv4hdr *)
				(ipv4_offset + PRC_GET_SEGMENT_ADDRESS());
		frag_offset = (ipv4_hdr->flags_and_offset &
						IPV4_HDR_FRAG_OFFSET_MASK);
		ip_total_length = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS)
				- ipv4_offset - ipf_ctx->prc_seg_offset;
		cksum_update_uint32(&ipv4_hdr->hdr_cksum,
				ipv4_hdr->total_length,
				ip_total_length);
		ipv4_hdr->total_length = ip_total_length;
		ipf_ctx->prev_frag_offset = frag_offset;

		/* Calculate frag offset for next fragment */
		if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS){
			header_length = (uint16_t)
			(ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK)<<2;
			payload_length = ip_total_length - header_length;
			ipf_ctx->prev_frag_offset += (payload_length>>3);
		}
	} else {
	/* Not first fragment */
		ipv4_offset = ipf_ctx->ip_offset;
		ipv4_hdr = (struct ipv4hdr *)
				(ipv4_offset + PRC_GET_SEGMENT_ADDRESS());
		if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
			/* Update Total length in header */
			ip_total_length = (uint16_t)
					LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS)
					- ipv4_offset - ipf_ctx->prc_seg_offset;
			cksum_update_uint32(&ipv4_hdr->hdr_cksum,
					ipv4_hdr->total_length,
					ip_total_length);
			ipv4_hdr->total_length = ip_total_length;

			header_length = (uint16_t)
			(ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK)<<2;
			payload_length = ip_total_length - header_length;
			frag_offset = ipf_ctx->prev_frag_offset;
			ipf_ctx->prev_frag_offset += (payload_length>>3);
		} else {
		frag_offset = ipf_ctx->prev_frag_offset +
					(ipf_ctx->mtu_payload_length>>3);
		ipf_ctx->prev_frag_offset = frag_offset;
		}
	}
	/* Update frag offset, M flag=1, checksum, length */
	flags_and_offset = frag_offset | IPV4_HDR_M_FLAG_MASK;
	cksum_update_uint32(&(ipv4_hdr->hdr_cksum),
			ipv4_hdr->flags_and_offset,
			flags_and_offset);
	ipv4_hdr->flags_and_offset = flags_and_offset;

	/* Run parser */
	parse_result_generate_default(PARSER_NO_FLAGS);

	/* Modify 12 first header fields in FDMA */
	fdma_modify_default_segment_data(ipv4_offset, 12);

	present_segment_params.flags = FDMA_PRES_NO_FLAGS;
	present_segment_params.frame_handle = ipf_ctx->rem_frame_handle;
	present_segment_params.offset = 0;
	present_segment_params.present_size = 0;
	/* present empty segment of the remaining frame */
	fdma_present_frame_segment(&present_segment_params);

	if (ipf_ctx->first_frag) {
		/* TODO Handle options */
		ipf_ctx->first_frag = 0;
	}
	header_length = ipv4_offset + ipf_ctx->prc_seg_offset +
		(uint16_t)((ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK) << 2);

	insert_segment_data_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
	insert_segment_data_params.frame_handle = ipf_ctx->rem_frame_handle;
	insert_segment_data_params.to_offset = 0;
	insert_segment_data_params.insert_size = header_length;
	insert_segment_data_params.from_ws_src =
					(void *)PRC_GET_SEGMENT_ADDRESS();
	insert_segment_data_params.seg_handle =
				present_segment_params.seg_handle;
	/* Insert the header to the remaining frame, close segment */
	fdma_insert_segment_data(&insert_segment_data_params);
}

/*inline*/ void ipf_after_split_ipv6_fragment(struct ipf_context *ipf_ctx,
						uint32_t last_ext_hdr_size)
{
	struct fdma_present_segment_params present_segment_params;
	struct fdma_insert_segment_data_params insert_segment_data_params;
	struct ipv6hdr *ipv6_hdr;
	struct ipv6fraghdr *ipv6_frag_hdr;
	uint16_t header_length, frag_payload_length, ipv6_offset;
	uint16_t frag_offset = 0;
	uint16_t seg_size_rs, modify_size;
	uint8_t *last_header;
	uint8_t orig_next_header;
	void *ws_dst_rs;

	if (ipf_ctx->first_frag) {
		ipv6_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv6_hdr = (struct ipv6hdr *)
				(ipv6_offset + PRC_GET_SEGMENT_ADDRESS());
		ipv6_hdr->payload_length =
		(uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) - ipv6_offset -
		ipf_ctx->prc_seg_offset - sizeof(struct ipv6hdr) + 
		IPV6_FRAGMENT_HEADER_LENGTH;

		if (!(ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS)){
			/* Calculate split size for next fragment */
			ipf_ctx->split_size += IPV6_FRAGMENT_HEADER_LENGTH;
		} else {
			/* Calculate frag offset for next fragment */
			frag_payload_length = (uint16_t)
				LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				(uint16_t)ipf_ctx->ipv6_frag_hdr_offset -
				ipf_ctx->prc_seg_offset;
			ipf_ctx->prev_frag_offset = (frag_payload_length>>3);
		}
		/* Replace the last "next header" of the unfragmentable
		 * part with 44 */
		ipv6_frag_hdr = (struct ipv6fraghdr *)
				(ipf_ctx->ipv6_frag_hdr_offset +
					PRC_GET_SEGMENT_ADDRESS());
		if (ipf_ctx->ipv6_frag_hdr_offset >
				(ipv6_offset + sizeof(struct ipv6hdr))) {
			/*ext headers exist */
			last_header = (uint8_t *)
				((uint32_t)ipv6_frag_hdr - last_ext_hdr_size);
			orig_next_header = *last_header;
			*(last_header) = IPV6_EXT_FRAGMENT;
		} else { /* no ext headers */
			orig_next_header = ipv6_hdr->next_header;
			ipv6_hdr->next_header = IPV6_EXT_FRAGMENT;
		}

			/* Build IPv6 fragment header */
			ipv6_frag_hdr->next_header = orig_next_header;
			ipv6_frag_hdr->reserved = 0;
			ipv6_frag_hdr->offset_and_flags = IPV6_HDR_M_FLAG_MASK;
			ipv6_frag_hdr->id = (uint16_t)fsl_os_rand();

			/* replace ip payload length, replace next header,
			 * insert IPv6 fragment header
			 */

			ws_dst_rs = (void *)PRC_GET_SEGMENT_ADDRESS();
			seg_size_rs = PRC_GET_SEGMENT_LENGTH();

/* Due to the CTLU HW requirement for alignment, presentation address should not 
 * change */
/*
			if ((PRC_GET_SEGMENT_ADDRESS() -
					(uint32_t)TLS_SECTION_END_ADDR) >=
					IPV6_FRAGMENT_HEADER_LENGTH){
				ws_dst_rs = (void *)((uint32_t)ws_dst_rs -
					(uint32_t)IPV6_FRAGMENT_HEADER_LENGTH);
				seg_size_rs = seg_size_rs +
						IPV6_FRAGMENT_HEADER_LENGTH;
			}
*/
			
			fdma_replace_default_segment_data(
				ipv6_offset,
				(uint16_t)
				((uint32_t)ipv6_frag_hdr - (uint32_t)ipv6_hdr),
				ipv6_hdr,
				(uint16_t)
				((uint32_t)ipv6_frag_hdr - (uint32_t)ipv6_hdr +
					IPV6_FRAGMENT_HEADER_LENGTH),
				ws_dst_rs,
				seg_size_rs,
				FDMA_REPLACE_SA_REPRESENT_BIT);
		
		ipf_ctx->first_frag = 0;
	} else {
	/* Not first fragment */
		ipv6_offset = ipf_ctx->ip_offset;
		ipv6_hdr = (struct ipv6hdr *)
				(ipv6_offset + PRC_GET_SEGMENT_ADDRESS());

		if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
			ipv6_hdr->payload_length =
			(uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			ipv6_offset - sizeof(struct ipv6hdr) - ipf_ctx->prc_seg_offset;
			frag_payload_length = (uint16_t)
				LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				(uint16_t)ipf_ctx->ipv6_frag_hdr_offset -
				IPV6_FRAGMENT_HEADER_LENGTH -
				ipf_ctx->prc_seg_offset;
			frag_offset = ipf_ctx->prev_frag_offset;
			ipf_ctx->prev_frag_offset += (frag_payload_length>>3);

			ipv6_frag_hdr = (struct ipv6fraghdr *)
					(ipf_ctx->ipv6_frag_hdr_offset +
						PRC_GET_SEGMENT_ADDRESS());
			ipv6_frag_hdr->offset_and_flags =
					(frag_offset<<3) | IPV6_HDR_M_FLAG_MASK;

			modify_size = (uint16_t)(ipf_ctx->ipv6_frag_hdr_offset)
				+ IPV6_FRAGMENT_HEADER_LENGTH - ipv6_offset;
			fdma_modify_default_segment_data
						(ipv6_offset, modify_size);

		} else {
			frag_offset = ipf_ctx->prev_frag_offset +
					(ipf_ctx->mtu_payload_length>>3);
			ipf_ctx->prev_frag_offset = frag_offset;
			ipv6_frag_hdr = (struct ipv6fraghdr *)
					(ipf_ctx->ipv6_frag_hdr_offset +
						PRC_GET_SEGMENT_ADDRESS());
			ipv6_frag_hdr->offset_and_flags =
					(frag_offset<<3) | IPV6_HDR_M_FLAG_MASK;
			/* Modify fragment header fields in FDMA */
			fdma_modify_default_segment_data
				((uint16_t)ipf_ctx->ipv6_frag_hdr_offset+2, 2);
		}
	}
	/* Run parser */
	parse_result_generate_default(PARSER_NO_FLAGS);

	present_segment_params.flags = FDMA_PRES_NO_FLAGS;
	present_segment_params.frame_handle = ipf_ctx->rem_frame_handle;
	present_segment_params.offset = 0;
	present_segment_params.present_size = 0;
	/* present empty segment of the remaining frame */
	fdma_present_frame_segment(&present_segment_params);

	header_length = (uint16_t)(ipf_ctx->ipv6_frag_hdr_offset) +
			ipf_ctx->prc_seg_offset + IPV6_FRAGMENT_HEADER_LENGTH;

	insert_segment_data_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
	insert_segment_data_params.frame_handle =
			ipf_ctx->rem_frame_handle;
	insert_segment_data_params.insert_size = header_length;
	insert_segment_data_params.to_offset = 0;
	insert_segment_data_params.from_ws_src =
			(void *)PRC_GET_SEGMENT_ADDRESS();
	insert_segment_data_params.seg_handle =
		present_segment_params.seg_handle;
	/* Insert the header to the remaining frame, close segment */
	fdma_insert_segment_data(&insert_segment_data_params);
}

/*inline*/int ipf_ipv4_last_frag(struct ipf_context *ipf_ctx)
{
	int32_t	status;
	struct fdma_amq isolation_attributes;
	struct ipv4hdr *ipv4_hdr;
	uint16_t ip_total_length;
	uint16_t frag_offset, ipv4_offset;
	uint8_t spid;

	spid = *((uint8_t *)HWC_SPID_ADDRESS);
	status = fdma_store_frame_data(ipf_ctx->rem_frame_handle, spid,
		 &isolation_attributes);
	if (status < 0)
		return status;/* Last fragment cannot be stored due to
				buffer pool depletion (status = (-ENOMEM)).*/
	
	/* Copy remaining_FD to default FD */
	*((struct ldpaa_fd *)HWC_FD_ADDRESS) = ipf_ctx->rem_fd;
	/* present fragment + header segment */
	fdma_present_default_frame();

	ipv4_offset = ipf_ctx->ip_offset;
	ipv4_hdr = (struct ipv4hdr *)
		(ipv4_offset + PRC_GET_SEGMENT_ADDRESS());
	if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
		frag_offset = ipf_ctx->prev_frag_offset;
	} else {
		frag_offset = ipf_ctx->prev_frag_offset +
			(ipf_ctx->mtu_payload_length>>3);
		}

	/* For fragment's fragmentation, last frag should have M bit set */
	if (ipf_ctx->flags & FRAGMENTATION_OF_FRAG)
		frag_offset |= IPV4_HDR_M_FLAG_MASK;

	/* Updating frag offset, M flag=0, checksum, length */
	cksum_update_uint32(&ipv4_hdr->hdr_cksum,
			ipv4_hdr->flags_and_offset,
			frag_offset);
	ipv4_hdr->flags_and_offset = frag_offset;

	ip_total_length = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
					ipv4_offset - ipf_ctx->prc_seg_offset;
	cksum_update_uint32(&ipv4_hdr->hdr_cksum,
			ipv4_hdr->total_length,
			ip_total_length);
	ipv4_hdr->total_length = ip_total_length;

	/* Run parser */
	parse_result_generate_default(PARSER_NO_FLAGS);

	/* Modify 12 first header fields in FDMA */
	fdma_modify_default_segment_data((uint16_t)ipv4_offset, 12);

	return IPF_GEN_FRAG_STATUS_DONE;
}


int ipf_split_ipv4_fragment(struct ipf_context *ipf_ctx)
{
	int32_t	status, split_status;
	struct fdma_split_frame_params split_frame_params;
	struct fdma_amq isolation_attributes;

	split_frame_params.fd_dst = (void *)HWC_FD_ADDRESS;
	split_frame_params.seg_dst = (void *)PRC_GET_SEGMENT_ADDRESS();
	split_frame_params.seg_offset = PRC_GET_SEGMENT_OFFSET();
	split_frame_params.present_size = PRC_GET_SEGMENT_LENGTH();
	split_frame_params.source_frame_handle =
					ipf_ctx->rem_frame_handle;
	split_frame_params.spid = *((uint8_t *)HWC_SPID_ADDRESS);

	if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
		split_frame_params.split_size_sf = 0;

		/* Split remaining frame, put split frame in default FD
		 * location*/
		
		/* Due to HW ticket TKT240996 */

		status = fdma_store_frame_data(
				split_frame_params.source_frame_handle,
				split_frame_params.spid,
				&isolation_attributes);
		status = fdma_present_frame_without_segments(
				&(ipf_ctx->rem_fd),
				FDMA_INIT_NO_FLAGS, 0,
				&(ipf_ctx->rem_frame_handle));
		split_frame_params.flags = FDMA_CFA_COPY_BIT |
					FDMA_SPLIT_SM_BIT|
					FDMA_SPLIT_PSA_NO_PRESENT_BIT;
		split_status = fdma_split_frame(&split_frame_params); /* TODO FDMA ERROR */
		if (split_status == (-EINVAL)) {
			/* last fragment, no split happened */
			status = ipf_ipv4_last_frag(ipf_ctx);
			return status;
		} else {
			status = fdma_store_default_frame_data();
			status = fdma_present_default_frame();

			ipf_after_split_ipv4_fragment(ipf_ctx);
				
			return IPF_GEN_FRAG_STATUS_IN_PROCESS;
		}
	} else { /* Split according to MTU */
		if (ipf_ctx->remaining_payload_length >
					ipf_ctx->mtu_payload_length) {
			/* Not last fragment, need to split */
			ipf_ctx->remaining_payload_length =
					ipf_ctx->remaining_payload_length -
					ipf_ctx->mtu_payload_length;

			split_frame_params.split_size_sf = ipf_ctx->split_size;

			/* Split remaining frame, put split frame in default FD
			 * location*/
			
			/* Due to HW ticket TKT240996 */
			status = fdma_store_frame_data(
					split_frame_params.source_frame_handle,
					split_frame_params.spid,
					&isolation_attributes);
			status = fdma_present_frame_without_segments(
					&(ipf_ctx->rem_fd),
					FDMA_INIT_NO_FLAGS, 0,
					&(ipf_ctx->rem_frame_handle));
			split_frame_params.flags = FDMA_CFA_COPY_BIT |
						FDMA_SPLIT_PSA_NO_PRESENT_BIT;
			split_status = fdma_split_frame(&split_frame_params); /* TODO FDMA ERROR */
			status = fdma_store_default_frame_data();
			status = fdma_present_default_frame();

			ipf_after_split_ipv4_fragment(ipf_ctx);

			return IPF_GEN_FRAG_STATUS_IN_PROCESS;

		} else {
		/* Last Fragment */
			status = ipf_ipv4_last_frag(ipf_ctx);
			return status;
		}
	}
}

/*inline*/int ipf_ipv6_last_frag(struct ipf_context *ipf_ctx)
{
	int32_t	status;
	struct fdma_amq isolation_attributes;
	struct ipv6hdr *ipv6_hdr;
	struct ipv6fraghdr *ipv6_frag_hdr;
	uint16_t ipv6_offset;
	uint16_t modify_size, frag_offset;
	uint8_t spid;

	spid = *((uint8_t *)HWC_SPID_ADDRESS);
	status = fdma_store_frame_data(ipf_ctx->rem_frame_handle, spid,
		 &isolation_attributes);
	if (status < 0)
		return status;/* Last fragment cannot be stored due to
				buffer pool depletion (status = (-ENOMEM)).*/
	
	/* Copy remaining_FD to default FD */
	*((struct ldpaa_fd *)HWC_FD_ADDRESS) = ipf_ctx->rem_fd;
	/* present fragment + header segment */
	fdma_present_default_frame();

	ipv6_offset = ipf_ctx->ip_offset;
	ipv6_hdr = (struct ipv6hdr *)
		(ipv6_offset + PRC_GET_SEGMENT_ADDRESS());
	ipv6_frag_hdr = (struct ipv6fraghdr *)
			(ipf_ctx->ipv6_frag_hdr_offset +
			PRC_GET_SEGMENT_ADDRESS());
	/* Update frag offset, M flag=0 */

	if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
		frag_offset = ipf_ctx->prev_frag_offset;
	} else {
		frag_offset = ipf_ctx->prev_frag_offset +
			(ipf_ctx->mtu_payload_length>>3);
	}
	ipv6_frag_hdr->offset_and_flags = frag_offset<<3;

	/* Update payload length in ipv6 header */
	ipv6_hdr->payload_length = (uint16_t)
			LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			ipv6_offset - sizeof(struct ipv6hdr) - ipf_ctx->prc_seg_offset;
	/* Run parser */
	parse_result_generate_default(PARSER_NO_FLAGS);

	/* Modify header fields in FDMA */
	modify_size = (uint16_t)(ipf_ctx->ipv6_frag_hdr_offset) +
		IPV6_FRAGMENT_HEADER_LENGTH - ipv6_offset;
	fdma_modify_default_segment_data(ipv6_offset, modify_size);

	return IPF_GEN_FRAG_STATUS_DONE;
}

int ipf_split_ipv6_fragment(struct ipf_context *ipf_ctx,
					uint32_t last_ext_hdr_size)
{
	int32_t	status, split_status;
	struct fdma_split_frame_params split_frame_params;
	struct fdma_amq isolation_attributes;

	split_frame_params.fd_dst = (void *)HWC_FD_ADDRESS;
	split_frame_params.seg_dst = (void *)PRC_GET_SEGMENT_ADDRESS();
	split_frame_params.seg_offset = PRC_GET_SEGMENT_OFFSET();
	split_frame_params.present_size = PRC_GET_SEGMENT_LENGTH();
	split_frame_params.source_frame_handle =
					ipf_ctx->rem_frame_handle;
	split_frame_params.spid = *((uint8_t *)HWC_SPID_ADDRESS);

	/* In case of fragments restoration need to store the frame in order
	 * to get updated FD[length] */
	if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
		split_frame_params.split_size_sf = 0;

		/* Due to HW ticket TKT240996 */
		status = fdma_store_frame_data(
				split_frame_params.source_frame_handle,
				split_frame_params.spid,
				&isolation_attributes);
		status = fdma_present_frame_without_segments(
				&(ipf_ctx->rem_fd),
				FDMA_INIT_NO_FLAGS, 0,
				&(ipf_ctx->rem_frame_handle));
		split_frame_params.flags = FDMA_CFA_COPY_BIT |
					FDMA_SPLIT_SM_BIT|
					FDMA_SPLIT_PSA_NO_PRESENT_BIT;
		split_status = fdma_split_frame(&split_frame_params); /* TODO FDMA ERROR */
		if (split_status == (-EINVAL)) {
			/* last fragment, no split happened */
			status = ipf_ipv6_last_frag(ipf_ctx);
			return status;
		} else {
			status = fdma_store_default_frame_data();
			status = fdma_present_default_frame();

			ipf_after_split_ipv6_fragment(ipf_ctx,
							last_ext_hdr_size);
			
			return IPF_GEN_FRAG_STATUS_IN_PROCESS;
		}
	} else { /* Split according to MTU */
		if (ipf_ctx->remaining_payload_length >
				ipf_ctx->mtu_payload_length) {
			/* Not last fragment, need to split */
			ipf_ctx->remaining_payload_length =
					ipf_ctx->remaining_payload_length -
					ipf_ctx->mtu_payload_length;

			split_frame_params.split_size_sf = ipf_ctx->split_size;

			/* Split remaining frame, put split frame in default FD
			 * location*/

			/* Due to HW ticket TKT240996 */

			status = fdma_store_frame_data(
					split_frame_params.source_frame_handle,
					split_frame_params.spid,
					&isolation_attributes);
			status = fdma_present_frame_without_segments(
					&(ipf_ctx->rem_fd),
					FDMA_INIT_NO_FLAGS, 0,
					&(ipf_ctx->rem_frame_handle));
			split_frame_params.flags = FDMA_CFA_COPY_BIT |
						FDMA_SPLIT_PSA_NO_PRESENT_BIT;
			split_status = fdma_split_frame(&split_frame_params); /* TODO FDMA ERROR */
			status = fdma_store_default_frame_data();
			status = fdma_present_default_frame();

			ipf_after_split_ipv6_fragment(ipf_ctx,
							last_ext_hdr_size);

			return IPF_GEN_FRAG_STATUS_IN_PROCESS;
		} else {
			/* Last Fragment */
			status = ipf_ipv6_last_frag(ipf_ctx);
			return status;
		}
	}
}


int ipf_generate_frag(ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;

	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	int32_t	status;
	uint32_t next_header, last_ext_hdr_size;
	uint16_t ip_header_length, mtu_payload_length, split_size;
	uint8_t last_ext_length;
	struct ipv4hdr *ipv4_hdr;
	struct ipv6hdr *ipv6_hdr;
/*	struct params_for_restoration restore_params; */

	if (ipf_ctx->first_frag) {
		/* First Fragment */
		/* Keep parser's parameters from task defaults */
		ipf_ctx->parser_profile_id =
				default_task_params.parser_profile_id;
		ipf_ctx->parser_starting_hxs =
				default_task_params.parser_starting_hxs;
		/* Keep PRC parameters */
		ipf_ctx->prc_seg_address = PRC_GET_SEGMENT_ADDRESS();
		ipf_ctx->prc_seg_length = PRC_GET_SEGMENT_LENGTH();
		ipf_ctx->prc_seg_offset = PRC_GET_SEGMENT_OFFSET();
		/* Keep frame's ip offset */
		ipf_ctx->ip_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();

		if (PARSER_IS_OUTER_IPV6_DEFAULT()) {
			/*
			ipf_ctx->ipv6_frag_hdr_offset =
				PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
			 */
			ipv6_hdr = (struct ipv6hdr *)(ipf_ctx->ip_offset
						+ PRC_GET_SEGMENT_ADDRESS());
			next_header = ipv6_last_header
					(ipv6_hdr, LAST_HEADER_BEFORE_FRAG);
			if (next_header & IPV6_NO_EXTENSION) {
				/* No ext. headers */
				ipf_ctx->ipv6_frag_hdr_offset =
						ipf_ctx->ip_offset +
						sizeof(struct ipv6hdr);
			} else {
				/* Ext. headers exist */
				last_ext_length =
					*((uint8_t *)(next_header + 1));
				last_ext_hdr_size =
				(uint16_t)((last_ext_length+1)<<3);
				ipf_ctx->ipv6_frag_hdr_offset =
					(uint8_t)(next_header -
					PRC_GET_SEGMENT_ADDRESS()
					+ last_ext_hdr_size);
			}

			if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
				/* Restore original fragments */
				status = ipf_move_remaining_frame(ipf_ctx);
				if (status)
					return status;
				/* Clear gross running sum in parse results */
				pr->gross_running_sum = 0;

				status = ipf_split_ipv6_fragment
						(ipf_ctx, last_ext_hdr_size);
				return status;
			} else {
				/* Split according to MTU */
				ip_header_length = (uint16_t)
						(ipf_ctx->ipv6_frag_hdr_offset -
						ipf_ctx->ip_offset +
						IPV6_FRAGMENT_HEADER_LENGTH);
				mtu_payload_length =
					(ipf_ctx->mtu_payload_length-
						ip_header_length) & ~0x7;
				ipf_ctx->mtu_payload_length =
						mtu_payload_length;
				split_size = mtu_payload_length +
				(uint16_t)(ipf_ctx->ipv6_frag_hdr_offset) +
				ipf_ctx->prc_seg_offset;
				ipf_ctx->split_size = split_size;
				ipf_ctx->remaining_payload_length =
					ipv6_hdr->payload_length +
					sizeof(struct ipv6hdr) -
					((uint16_t)
					(ipf_ctx->ipv6_frag_hdr_offset -
						ipf_ctx->ip_offset));
				status = ipf_move_remaining_frame(ipf_ctx);
				if (status)
					return status;
				/* Clear gross running sum in parse results */
				pr->gross_running_sum = 0;
				status = ipf_split_ipv6_fragment(
						ipf_ctx, last_ext_hdr_size);
				return status;
			}
		} else {
			/* IPv4 */
			ipf_ctx->ipv4 = 1;
			if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS) {
				/* Restore original fragments */
				status = ipf_move_remaining_frame(ipf_ctx);
				if (status)
					return status;
				/* Clear gross running sum in parse results */
				pr->gross_running_sum = 0;

				status = ipf_split_ipv4_fragment(ipf_ctx);
					return status;
			} else {
			/* Split according to MTU */
				ipv4_hdr =
					(struct ipv4hdr *)(ipf_ctx->ip_offset +
					PRC_GET_SEGMENT_ADDRESS());
			/* Check if "Don't Fragment" bit is set */
				if (ipv4_hdr->flags_and_offset &
						IPV4_HDR_D_FLAG_MASK)
					return IPF_GEN_FRAG_STATUS_DF_SET;
			/* Check if this is fragmentation of a fragment */	
				if (ipv4_hdr->flags_and_offset &
						IPV4_HDR_M_FLAG_MASK)
					ipf_ctx->flags |= FRAGMENTATION_OF_FRAG;

				ip_header_length = (uint16_t)
				(ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK)<<2;
				mtu_payload_length =
					(ipf_ctx->mtu_payload_length-
						ip_header_length) & ~0x7;
				ipf_ctx->mtu_payload_length =
						mtu_payload_length;
				split_size = mtu_payload_length +
						ip_header_length +
						(uint16_t)ipf_ctx->ip_offset +
						ipf_ctx->prc_seg_offset;
				ipf_ctx->split_size = split_size;
				ipf_ctx->remaining_payload_length =
						ipv4_hdr->total_length -
						ip_header_length;
				status = ipf_move_remaining_frame(ipf_ctx);
				if (status)
					return status;
				/* Clear gross running sum in parse results */
				pr->gross_running_sum = 0;
				status = ipf_split_ipv4_fragment(ipf_ctx);
				return status;
			}
		}
	} else {
		/* Not first Fragment */
		/* Restore original parser's parameters in task default */
		default_task_params.parser_profile_id =
					ipf_ctx->parser_profile_id;
		default_task_params.parser_starting_hxs =
					ipf_ctx->parser_starting_hxs;
		/* Restore original PRC parameters */
		PRC_SET_SEGMENT_ADDRESS(ipf_ctx->prc_seg_address);
		PRC_SET_SEGMENT_LENGTH(ipf_ctx->prc_seg_length);
		PRC_SET_SEGMENT_OFFSET(ipf_ctx->prc_seg_offset);

		/* Clear gross running sum in parse results */
		pr->gross_running_sum = 0;

		if (ipf_ctx->ipv4)
			status = ipf_split_ipv4_fragment(ipf_ctx);
		else
			status = ipf_split_ipv6_fragment(ipf_ctx, NULL);

		return status;
	}
}

int ipf_discard_frame_remainder(ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;
	fdma_discard_frame(
			ipf_ctx->rem_frame_handle, FDMA_DIS_NO_FLAGS);
	return SUCCESS;
}

void ipf_context_init(uint32_t flags, uint16_t mtu, ipf_ctx_t ipf_context_addr)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("ipf.c",(uint32_t *)ipf_context_addr, ALIGNMENT_32B);
#endif
	
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;

	ipf_ctx->first_frag = 1;
	ipf_ctx->flags = flags;
	ipf_ctx->mtu_payload_length = mtu;
	ipf_ctx->ipv4 = 0;
	ipf_ctx->prev_frag_offset = 0;

}
