/**************************************************************************//**
@File		gro.c

@Description	This file contains the AIOP SW TCP GRO API implementation

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "net/fsl_net.h"
#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_parser.h"
#include "dplib/fsl_ste.h"
#include "gro.h"
#include "general.h"
#include "fdma.h"
#include "checksum.h"


	/* Shared memory global GRO parameters. */
struct gro_global_parameters gro_global_params;


void gro_init(uint32_t timeout_flags)
{
	gro_global_params.timeout_flags = timeout_flags;
}


int32_t tcp_gro_aggregate_seg(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		uint32_t flags)
{
	//struct tcp_gro_context gro_ctx;
	//cdma(&)
	/* Todo - remove next return statement */
	return (int32_t)(tcp_gro_context_addr+params+flags); 
}

int32_t tcp_gro_flush_aggregation(
		uint64_t tcp_gro_context_addr)
{
	struct tcp_gro_context gro_ctx;
	struct tcphdr *tcp;
	struct ipv4hdr *ipv4;
	struct ipv6hdr *ipv6;
	uint16_t ip_length, outer_ip_offset;
	int32_t status;
	
	/* read GRO context*/
	status = cdma_read_with_mutex(tcp_gro_context_addr, 
			CDMA_PREDMA_MUTEX_WRITE_LOCK,
			(void *)(&gro_ctx), sizeof(struct tcp_gro_context));
	/* no aggregation */
	if (gro_ctx.metadata.seg_num == 0)
		return TCP_GRO_FLUSH_NO_AGG;
	
	/* write metadata to external memory */
	status = cdma_write((gro_ctx.params.metadata + METADATA_MEMBER1_SIZE), 
			&(gro_ctx.metadata.seg_num), 
			METADATA_MEMBER2_SIZE + METADATA_MEMBER3_SIZE);
	/* release gro context mutex */
	status = cdma_mutex_lock_release(tcp_gro_context_addr);
	
	/* Copy aggregated FD to default FD location and prepare aggregated FD 
	 * parameters in Presentation Context */
	*((struct ldpaa_fd *)HWC_FD_ADDRESS) = gro_ctx.agg_fd;
	PRC_SET_SEGMENT_ADDRESS((uint32_t)TLS_SECTION_END_ADDR + 
			DEFAULT_SEGMENT_HEADOOM_SIZE);
	PRC_SET_SEGMENT_LENGTH(DEFAULT_SEGMENT_SIZE);
	PRC_SET_SEGMENT_OFFSET(0);
	PRC_RESET_SR_BIT();
	PRC_SET_ASA_SIZE(0);
	PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
	status = fdma_present_default_frame();
	
	/* run parser since we don't know which scenario preceded 
	 * the flush call */
	status = parse_result_generate_default(PARSER_NO_FLAGS);
	
	/* update IP length + checksum */
	outer_ip_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ip_length = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) - 
			outer_ip_offset;
	if (PARSER_IS_OUTER_IPV4_DEFAULT()){
		ipv4 = (struct ipv4hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT();
		if (gro_ctx.flags & TCP_GRO_CALCULATE_IP_CHECKSUM)
			cksum_update_uint32(&(ipv4->hdr_cksum),
					ipv4->total_length, ip_length);
		ipv4->total_length = ip_length;
	}
	else{
		ipv6 = (struct ipv6hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT();
		ipv6->payload_length = ip_length - IPV6_HDR_LENGTH;
	}
	
	/* update TCP length + checksum */
	/* update last segment header fields */
	tcp = (struct tcphdr *)PARSER_GET_L4_POINTER_DEFAULT();
	*((struct tcp_gro_last_seg_header_fields *)
		(&(tcp->acknowledgment_number))) = gro_ctx.last_seg_fields;
	
	if (gro_ctx.flags & TCP_GRO_CALCULATE_TCP_CHECKSUM)
		tcp_gro_calc_tcp_header_cksum();
	
	/* Save headers changes to FDMA */
	status = fdma_modify_default_segment_data(outer_ip_offset, (uint16_t)
	   (PARSER_GET_L4_OFFSET_DEFAULT() + TCP_HDR_LENGTH - outer_ip_offset));
	
	/* update statistics */
	ste_inc_counter(gro_ctx.params.stats_addr + AGG_NUM_CNTR_OFFSET, 
			1, STE_MODE_SATURATE | STE_MODE_32_BIT_CNTR_SIZE);	
	if (gro_ctx.flags & TCP_GRO_EXTENDED_STATS_EN)
		ste_inc_counter(gro_ctx.params.stats_addr + 
				AGG_FLUSH_REQUEST_NUM_CNTR_OFFSET, 
			1, STE_MODE_SATURATE | STE_MODE_32_BIT_CNTR_SIZE);
		
	return TCP_GRO_FLUSH_AGG_DONE;
}

/* Todo - fill function */
uint16_t tcp_gro_calc_tcp_header_cksum()
{
	/* Todo - return valid checksum */
	return 0;
}
