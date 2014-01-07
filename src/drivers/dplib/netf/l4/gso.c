/**************************************************************************//**
@File		gso.c

@Description	This file contains the AIOP SW TCP GSO API implementation

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_gso.h"
#include "gso.h"
#include "general.h"
#include "net/fsl_net.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "dplib/fsl_ipv4_checksum.h"
#include "fdma.h"

extern __TASK struct aiop_default_task_params default_task_params; // ??????

int32_t tcp_gso_generate_seg(
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	int32_t	status;
	uint8_t tcp_offset, outer_ip_offset, headers_size;
	uint16_t split_size, ip_header_length;
	struct tcphdr *tcp_ptr;
	struct ipv4hdr *outer_ipv4_ptr;
	struct ipv6hdr *outer_ipv6_ptr;
	struct fdma_present_frame_params present_rem_frame_params;
	
	tcp_offset = (uint8_t)(PARSER_GET_L4_OFFSET_DEFAULT());
	tcp_ptr = (struct tcphdr *)(tcp_offset + PRC_GET_SEGMENT_ADDRESS());
	outer_ip_offset = (uint8_t)(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	outer_ipv4_ptr = (struct ipv4hdr *)(outer_ip_offset + 
			PRC_GET_SEGMENT_ADDRESS());
	outer_ipv6_ptr = (struct ipv6hdr *)(outer_ip_offset + 
			PRC_GET_SEGMENT_ADDRESS());
	
	if (gso_ctx->first_seg) {
		if (tcp_ptr->flags & (NET_HDR_FLD_TCP_FLAGS_RST | 
				NET_HDR_FLD_TCP_FLAGS_SYN))
			/* RST/SYN flags set */
			return TCP_GSO_GEN_SEG_STATUS_SYN_RST_SET;
		if (tcp_ptr->flags & NET_HDR_FLD_TCP_FLAGS_URG)
			/* URG flag set */
			gso_ctx->urgent_pointer = tcp_ptr->urgent_pointer;
		if (tcp_ptr->flags & NET_HDR_FLD_TCP_FLAGS_FIN) {
			/* FIN flag set */
			gso_ctx->internal_flags = gso_ctx->internal_flags | TCP_GSO_FIN_BIT;
			tcp_ptr->flags = tcp_ptr->flags & ~TCP_GSO_FIN_BIT;  /* reset FIN */
		}
		if (tcp_ptr->flags & NET_HDR_FLD_TCP_FLAGS_PSH) {
			/* PSH flag set */
			gso_ctx->internal_flags = gso_ctx->internal_flags | TCP_GSO_PSH_BIT;
			tcp_ptr->flags = tcp_ptr->flags & ~TCP_GSO_PSH_BIT;  /* reset PSH */
		}
	}
	else {
		/* Restore parser's parameters  */
		default_task_params.parser_profile_id = gso_ctx->parser_profile_id;
		default_task_params.parser_starting_hxs = gso_ctx->parser_starting_hxs;
		/* Restore PRC parameters */
		PRC_SET_SEGMENT_ADDRESS(gso_ctx->seg_address);
		PRC_SET_SEGMENT_LENGTH(gso_ctx->seg_length);
		/* Call to tcp_gso_split_segment */	
		status = tcp_gso_split_segment(gso_ctx);
	}
		
		
	
	/* Keep parser's parameters from task defaults */
	gso_ctx->parser_profile_id = 
			default_task_params.parser_profile_id;
	gso_ctx->parser_starting_hxs = 
			default_task_params.parser_starting_hxs;
	/* Keep PRC parameters */
	gso_ctx->seg_address = PRC_GET_SEGMENT_ADDRESS();
	gso_ctx->seg_length = PRC_GET_SEGMENT_LENGTH();
	
	headers_size = (uint8_t)(PARSER_GET_L4_OFFSET_DEFAULT()) + 
			(tcp_ptr->data_offset_reserved & 0xf0); 
	split_size = (uint16_t)headers_size + gso_ctx->mss;
	ip_header_length = split_size - 
			(uint16_t)(PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	
	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		/* IPv4 */
		outer_ipv4_ptr->total_length = ip_header_length;
		ipv4_cksum_calculate(outer_ipv4_ptr);
		/* update IP checksum in FDMA */
		status = ipv4_cksum_calculate(outer_ipv4_ptr);  /* TODO FDMA ERROR */
	}
	else
		/* IPv6 */
		outer_ipv6_ptr->payload_length = ip_header_length - 40;
	
	/* Modify 12 first IPv4/IPv6 header fields in FDMA */
	status = fdma_modify_default_segment_data(outer_ip_offset,12); /* TODO FDMA ERROR */
	status = fdma_store_default_frame_data();
	if (status) 
		return status; /* TODO */
	
	/* Copy default FD to remaining_FD in GSO ctx */
	gso_ctx->rem_fd = *((struct ldpaa_fd*)HWC_FD_ADDRESS);
	
	/* Present the remaining FD */
	present_rem_frame_params.flags = FDMA_INIT_NDS_BIT;
	present_rem_frame_params.asa_size = 0;
	present_rem_frame_params.fd_src = &(gso_ctx->rem_fd);
	present_rem_frame_params.pta_dst = (void *)PRC_PTA_NOT_LOADED_ADDRESS;
	status = fdma_present_frame(&present_rem_frame_params);  /* TODO FDMA ERROR */
	gso_ctx->rem_frame_handle = present_rem_frame_params.frame_handle;
	
	/* Call to tcp_gso_split_segment */	
	status = tcp_gso_split_segment(gso_ctx);
	
	return 0; /* Todo - return valid status*/
}


int32_t tcp_gso_split_segment(struct tcp_gso_context *gso_ctx)
{
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct fdma_split_frame_params split_frame_params;
	
	/* Clear gross running sum in parse results */
	pr->gross_running_sum = 0;
	
	/* split_frame_params.flags = FDMA_CFA_COPY_BIT | 
						FDMA_SPLIT_PSA_PRESENT_BIT;
	split_frame_params.fd_dst = (void *)HWC_FD_ADDRESS;
	split_frame_params.seg_dst = (void *)PRC_GET_SEGMENT_ADDRESS();
	split_frame_params.seg_offset = PRC_GET_SEGMENT_OFFSET();
	split_frame_params.present_size = PRC_GET_SEGMENT_LENGTH();
	split_frame_params.split_size_sf = gso_ctx->ipf_params.
			mtu_params.split_size;
	split_frame_params.source_frame_handle =
			gso_ctx->rem_frame_handle;
	split_frame_params.spid = *((uint8_t *)HWC_SPID_ADDRESS); */

	/* Split remaining frame, put split frame in default FD location*/
	//status = fdma_split_frame(&split_frame_params);
	//if (status)
		//return status; /* TODO*/  
	
	
}


int32_t tcp_gso_discard_frame_remainder(
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	return fdma_discard_frame(
			gso_ctx->rem_frame_handle, FDMA_DIS_NO_FLAGS);
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
	gso_ctx->mss = mss;
	gso_ctx->urgent_pointer = 0;
}
