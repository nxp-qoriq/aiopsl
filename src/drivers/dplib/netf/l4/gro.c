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
#include "dplib/fsl_tman.h"
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

/* New Aggregation */
int32_t tcp_gro_aggregate_seg(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		uint32_t flags)
{
	struct tcp_gro_context gro_ctx;
	struct tcphdr *tcp;
	struct ipv4hdr *ipv4;
	int32_t status;
	int32_t status1;
	uint16_t seg_size;
	uint8_t data_offset;
	
	seg_size = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	/* read GRO context*/
	status = cdma_read_with_mutex(tcp_gro_context_addr, 
			CDMA_PREDMA_MUTEX_WRITE_LOCK,
			(void *)(&gro_ctx), sizeof(struct tcp_gro_context));
	/* add segment to an existing aggregation */
	if (gro_ctx.metadata.seg_num != 0){
		status = tcp_gro_add_seg_to_aggregation(&gro_ctx);
		/* write gro context back to DDR + release mutex */
		status1 = cdma_write_with_mutex(tcp_gro_context_addr, 
					CDMA_POSTDMA_MUTEX_RM_BIT, 
					(void *)&gro_ctx, 
					sizeof(struct tcp_gro_context));
		return status;
	}
	/* read segment sizes address */
	if (flags & TCP_GRO_METADATA_SEGMENT_SIZES) {
		status = cdma_read(&(gro_ctx.metadata.seg_sizes_addr), 
				params->metadata, 
				METADATA_MEMBER1_SIZE); 
		status = cdma_write(gro_ctx.metadata.seg_sizes_addr, &seg_size, 
			sizeof(seg_size));
	}
	/* set metadada values */
	gro_ctx.metadata.seg_num = 1;
	gro_ctx.metadata.max_seg_size = seg_size;
	/* New aggregation - Initialize GRO Context */
	tcp = (struct tcphdr *)PARSER_GET_L4_POINTER_DEFAULT();
	/* Flush Aggregation */
	if (tcp->flags & NET_HDR_FLD_TCP_FLAGS_PSH) {
		/* write metadata to external memory */
		status = cdma_write((params->metadata + 
				METADATA_MEMBER1_SIZE), 
				&(gro_ctx.metadata.seg_num), 
				METADATA_MEMBER2_SIZE + METADATA_MEMBER3_SIZE);
		/* release the mutex */
		status = cdma_mutex_lock_release(tcp_gro_context_addr);
		/* update statistics */
		ste_inc_and_acc_counters(params->stats_addr + 
				GRO_STAT_AGG_NUM_CNTR_OFFSET, 1, 
				STE_MODE_COMPOUND_32_BIT_CNTR_SIZE | 
				STE_MODE_COMPOUND_32_BIT_ACC_SIZE |
				STE_MODE_COMPOUND_CNTR_SATURATE |
				STE_MODE_COMPOUND_ACC_SATURATE);
		return TCP_GRO_SEG_AGG_DONE_NEW_AGG;
	} else /* Aggregate */ {
		/* Todo - shouldn't we set the timer after we write the context 
		 * to DDR? so there will not be a case it expires before we even
		 * save the context the first time to DDR
		 * create timer for the aggregation */
		status = tman_create_timer(params->timeout_params.tmi_id, 
				gro_global_params.timeout_flags,
				params->limits.timeout_limit, 
				tcp_gro_context_addr, 
				(uint16_t)(uint32_t)&tcp_gro_timeout_callback, 
				gro_global_params.gro_timeout_epid,
				0, 
				&(gro_ctx.timer_handle));
		
		/* initialize gro context fields */
		gro_ctx.params = *params;
		gro_ctx.flags = flags;
		gro_ctx.last_ack = tcp->acknowledgment_number;
		data_offset = tcp->data_offset_reserved >> 
				TCP_DATA_OFFSET_OFFSET;
		gro_ctx.next_seq = tcp->sequence_number + seg_size - 
				(PARSER_GET_L4_OFFSET_DEFAULT() + data_offset);
		/* in case there is an option it must be a timestamp option */
		if (data_offset > TCP_HDR_LENGTH){
			gro_ctx.timestamp = *((uint32_t *)
				(PARSER_GET_L4_OFFSET_DEFAULT() + data_offset +
				TCP_TIMSTAMP_OPTION_VALUE_OFFSET));
			gro_ctx.internal_flags |= TCP_GRO_HAS_TIMESTAMP; 
		}
		if (PARSER_IS_OUTER_IPV4_DEFAULT()){
			ipv4 = (struct ipv4hdr *)
				PARSER_GET_OUTER_IP_POINTER_DEFAULT();
			/* Set ECN flags */
			gro_ctx.internal_flags |= (*((uint32_t *)ipv4) &
					TCP_GRO_ECN_MASK);
		}
		if (gro_ctx.flags & TCP_GRO_CALCULATE_TCP_CHECKSUM)
			gro_ctx.checksum = tcp_gro_calc_tcp_data_cksum();
		
		/* store aggregated frame */
		status = fdma_store_frame_data(PRC_GET_FRAME_HANDLE(), 
				*(uint8_t *)HWC_SPID_ADDRESS, 
				&(gro_ctx.agg_fd_isolation_attributes));
		/* copy default FD to gro context */
		gro_ctx.agg_fd = *((struct ldpaa_fd *)HWC_FD_ADDRESS);
		/* write gro context back to DDR + release mutex */
		status = cdma_write_with_mutex(tcp_gro_context_addr, 
				CDMA_POSTDMA_MUTEX_RM_BIT, 
				(void *)&gro_ctx, 
				sizeof(struct tcp_gro_context));
		/* update statistics */
		ste_inc_counter(gro_ctx.params.stats_addr + 
			GRO_STAT_SEG_NUM_CNTR_OFFSET, 
			1, STE_MODE_SATURATE | STE_MODE_32_BIT_CNTR_SIZE);
		
		return TCP_GRO_SEG_AGG_NOT_DONE_NEW_AGG;
	}	
}

/* Add segment to an existing aggregation */
int32_t tcp_gro_add_seg_to_aggregation(struct tcp_gro_context *gro_ctx)
{
	struct tcphdr *tcp;
	struct ipv4hdr *ipv4;
	struct fdma_present_frame_params present_frame_params;
	struct fdma_concatenate_frames_params concat_params;
	uint32_t timestamp;
	int32_t  status;
	uint16_t headers_size;
	uint16_t seg_size;
	uint16_t aggregated_size;
	uint8_t  data_offset;
	
	tcp = (struct tcphdr *)PARSER_GET_L4_POINTER_DEFAULT();
	ipv4 = (struct ipv4hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT();
	
	/* Check for termination conditions due to the current segment.
	 * In case one of the following conditions is met, close the aggregation 
	 * and start a new aggregation with the current segment */
	/* 1. Segment sequence number is not the expected sequence number  */
	if (gro_ctx->next_seq != tcp->sequence_number){
		/* update statistics */
		ste_inc_counter(gro_ctx->params.stats_addr + 
			GRO_STAT_UNEXPECTED_SEQ_NUM_CNTR_OFFSET, 
			1, STE_MODE_SATURATE | STE_MODE_32_BIT_CNTR_SIZE);
		return tcp_gro_close_aggregation_and_open_new_aggregation(gro_ctx);
	}
	/* 2. IP ECN value of the new packet is different from previously 
	 * coalesced packet.
	 * 3. Aggregated timestamp value is different than the new segment 
	 * timestamp value.
	 * 4. Segment ACK number is less than the ACK number of the previously 
	 * coalesced segment. 
	 * 5. PHS flag is set for the aggregation from the previous segment */
	data_offset = tcp->data_offset_reserved >> TCP_DATA_OFFSET_OFFSET;
	if (data_offset > TCP_HDR_LENGTH)
		timestamp = *((uint32_t *)(PARSER_GET_L4_OFFSET_DEFAULT() + 
			TCP_HDR_LENGTH + TCP_TIMSTAMP_OPTION_VALUE_OFFSET));
	else
		timestamp = 0;
	
	if (((gro_ctx->internal_flags & TCP_GRO_ECN_MASK) != 
		(*((uint32_t *)ipv4) & TCP_GRO_ECN_MASK)) ||
		(gro_ctx->timestamp != timestamp) ||
		(gro_ctx->last_ack > tcp->acknowledgment_number) ||
		(gro_ctx->internal_flags & TCP_GRO_PSH_FLAG_SET))
		return tcp_gro_close_aggregation_and_open_new_aggregation(gro_ctx);
	
	/* Check for termination condition due to the current segment.
	 * In case one of the following conditions is met, add segment to 
	 * aggregation and close the aggregation. */
	if (tcp->flags & NET_HDR_FLD_TCP_FLAGS_PSH)
		return tcp_gro_add_seg_and_close_aggregation(gro_ctx);
	
	/* calculate data offset */
	headers_size = (uint16_t)(PARSER_GET_L4_OFFSET_DEFAULT() + data_offset);
	
	seg_size = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	aggregated_size = (uint16_t)(LDPAA_FD_GET_LENGTH(&(gro_ctx->agg_fd))) + 
			seg_size - headers_size;
	/* check whether aggregation limits are met */
	/* check segment number limit */
	if ((gro_ctx->metadata.seg_num + 1) == 
			gro_ctx->params.limits.seg_num_limit)
		return tcp_gro_add_seg_and_close_aggregation(gro_ctx);	
	/* check aggregated packet size limit */
	if (aggregated_size > gro_ctx->params.limits.packet_size_limit)
		return tcp_gro_close_aggregation_and_open_new_aggregation(
				gro_ctx);
	else if (aggregated_size == gro_ctx->params.limits.packet_size_limit)
		return tcp_gro_add_seg_and_close_aggregation(gro_ctx);
		
	/* segment can be aggregated */
	/* calculate tcp data checksum */
	if (gro_ctx->flags & TCP_GRO_CALCULATE_TCP_CHECKSUM)
		gro_ctx->checksum = tcp_gro_calc_tcp_data_cksum();	
	
	/* present aggregated frame */
	present_frame_params.fd_src = &(gro_ctx->agg_fd);
	present_frame_params.flags = FDMA_INIT_NDS_BIT;
	present_frame_params.asa_size = 0;
	present_frame_params.pta_dst = (void *)PRC_PTA_NOT_LOADED_ADDRESS; 
	status = fdma_present_frame(&present_frame_params);
	/* concatenate frames and store aggregated packet */
	concat_params.frame1 = present_frame_params.frame_handle;
	concat_params.frame2 = (uint16_t)PRC_GET_FRAME_HANDLE();
	concat_params.trim = (uint8_t)headers_size;
	/* Todo - when concatenate command support returning isolation context 
	 * when closing frame1: 
	 * 1. enable next 2 lines (spid and flags) instead of the next flags 
	 * assignment,  
	 * 2. remove next store, 
	 * 3. add isolation attributes to the concatenate command
	 * concat_params.spid = *((uint8_t *)HWC_SPID_ADDRESS);
	concat_params.flags = FDMA_CONCAT_PCA_BIT;*/
	concat_params.flags = FDMA_CONCAT_NO_FLAGS;
	fdma_concatenate_frames(&concat_params);
	fdma_store_frame_data(present_frame_params.frame_handle, 
			*((uint8_t *)HWC_SPID_ADDRESS), 
			&(gro_ctx->agg_fd_isolation_attributes));
			
	/* update gro context fields */
	gro_ctx->last_ack = tcp->acknowledgment_number;
	gro_ctx->next_seq = gro_ctx->next_seq + seg_size - headers_size;
	gro_ctx->last_seg_fields = *((struct tcp_gro_last_seg_header_fields *)
			(&(tcp->acknowledgment_number)));
	gro_ctx->metadata.seg_num++;
	if (gro_ctx->metadata.max_seg_size < seg_size)
		gro_ctx->metadata.max_seg_size = seg_size;
	
	/* write metadata segment size to external memory */
	if (gro_ctx->flags & TCP_GRO_METADATA_SEGMENT_SIZES) 
		status = cdma_write(gro_ctx->metadata.seg_sizes_addr + 
				sizeof(seg_size) * gro_ctx->metadata.seg_num, 
				&seg_size, sizeof(seg_size));
	
	/* update statistics */
	ste_inc_counter(gro_ctx->params.stats_addr + 
			GRO_STAT_SEG_NUM_CNTR_OFFSET, 
			1, STE_MODE_SATURATE | STE_MODE_32_BIT_CNTR_SIZE);
	
	return TCP_GRO_SEG_AGG_NOT_DONE;
}

/* Add segment to aggregation and close aggregation. */
int32_t tcp_gro_add_seg_and_close_aggregation(
		struct tcp_gro_context *gro_ctx)
{
	/* Todo - return valid status */
	return (int32_t)gro_ctx;
}

/* Close an existing aggregation and start a new aggregation with the new 
 * segment. */
int32_t tcp_gro_close_aggregation_and_open_new_aggregation(
		struct tcp_gro_context *gro_ctx)
{
	/* Todo - return valid status */
	return (int32_t)gro_ctx;
}

int32_t tcp_gro_flush_aggregation(
		uint64_t tcp_gro_context_addr)
{
	struct tcp_gro_context gro_ctx;
	struct tcphdr *tcp;
	struct ipv4hdr *ipv4;
	struct ipv6hdr *ipv6;
	int32_t status;
	uint16_t ip_length, outer_ip_offset;
	
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
	ste_inc_counter(gro_ctx.params.stats_addr + GRO_STAT_AGG_NUM_CNTR_OFFSET 
			, 1, STE_MODE_SATURATE | STE_MODE_32_BIT_CNTR_SIZE);	
	if (gro_ctx.flags & TCP_GRO_EXTENDED_STATS_EN)
		ste_inc_counter(gro_ctx.params.stats_addr + 
			GRO_STAT_AGG_FLUSH_REQUEST_NUM_CNTR_OFFSET, 
			1, STE_MODE_SATURATE | STE_MODE_32_BIT_CNTR_SIZE);
		
	return TCP_GRO_FLUSH_AGG_DONE;
}

/* Todo - fill function */
uint16_t tcp_gro_calc_tcp_data_cksum()
{
	/* Todo - return valid checksum */
	return 0;
}

/* Todo - fill function */
uint16_t tcp_gro_calc_tcp_header_cksum()
{
	/* Todo - return valid checksum */
	return 0;
}

/* Todo - fill function */
void tcp_gro_timeout_callback(uint64_t tcp_gro_context_addr)
{
	/* Todo - remove following statement when function is implemented */
	tcp_gro_context_addr = 0;
}
