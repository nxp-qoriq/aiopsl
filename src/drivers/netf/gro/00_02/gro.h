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
 @File          gro.h

 @Description   This file contains the AIOP SW internal TCP GRO API
*//***************************************************************************/
#ifndef __GRO_H
#define __GRO_H

#include "fsl_ldpaa.h"
#include "fsl_gro.h"
#include "fsl_fdma.h"



/** \addtogroup FSL_AIOP_GRO
 *  @{
 */


/**************************************************************************//**
@Group		AIOP_TCP_GRO_INTERNAL AIOP TCP GRO Internal

@Description	AIOP TCP GRO Internal

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		TCP_GRO_INTERNAL_STRUCTS TCP GRO Internal Structures

@Description	AIOP TCP GRO Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GRO Last Segment Header Fields.
*//***************************************************************************/
/* TCP GRO Last Segment Header Fields which needed to be updated in the
 * aggregated packet. */

struct tcp_gro_last_seg_header_fields {
		/** Acknowledgment number. */
	uint32_t acknowledgment_number;
		/** Data offset, reserved fields. */
	uint8_t  data_offset_reserved;
		/** TCP control bits */
	uint8_t  flags;
		/** Window size */
	uint16_t window_size;
};

/**************************************************************************//**
@Description	TCP GRO Internal Context.
*//***************************************************************************/
/* The fixed definition size is 128 bytes.
 * In case additional fields are added to the struct we can either:
 * Modify the fixed size
 * OR
 * Remove unneeded fields (e.g. the metadata structure takes 4 bytes for
 * alignment only). */

struct tcp_gro_context {
		/** Aggregated packet FD.
		 * This field must remain at the beginning of the structure due
		 * to alignment restrictions for FD in workspace (The FD adress
		 * in Workspace must be aligned to 32 bytes). */
	struct ldpaa_fd agg_fd;
		/** Address (in HW buffers) of the TCP GRO aggregation metadata
		 * buffer (\ref tcp_gro_context_metadata)
		 * Upper layer SW should always send a metadata buffer address
		 * to tcp_gro_aggregate_seg().
		 * After tcp_gro_aggregate_seg() returns \ref
		 * TCP_GRO_METADATA_USED bit in the status, the following call
		 * to tcp_gro_aggregate_seg() should send an address to a new
		 * metadata buffer.
		 * */
	uint64_t metadata_addr;
		/** Address (in HW buffers) of the TCP GRO statistics counters
		 *  (\ref tcp_gro_stats_cntrs).
		 *  The user should zero the statistics once it is allocated. */
	uint64_t stats_addr;
		/** Address (in HW buffers) of the callback function parameter
		 * argument on timeout.
		 * On timeout, GRO will call upper layer callback function with
		 * this parameter. */
	uint64_t gro_timeout_cb_arg;
		/** Aggregated packet metadata  */
	struct tcp_gro_context_metadata metadata;
		/** Last Segment header fields which we need to update in the
		 * aggregated packet. */
	struct tcp_gro_last_seg_header_fields last_seg_fields;
		/** Function to call upon Time Out occurrence.
			 * This function takes one argument. */
	gro_timeout_cb_t *gro_timeout_cb;
		/** Aggregated packet timestamp value. */
	uint32_t timestamp;
		/** Next expected sequence number. */
	uint32_t next_seq;
		/** TCP GRO aggregation flags */
	uint32_t flags;
		/** TMAN Instance ID. */
	uint32_t timer_handle;
		/** Internal TCP GRO flags */
	uint16_t internal_flags;
		/** First segment presentation size */
	uint16_t prc_segment_length;
		/** First segment presentation offset */
	uint16_t prc_segment_offset;
		/** First segment presentation address */
	uint16_t prc_segment_addr;
	/** Maximum aggregated packet size limit (The size refers to the
		 * packet headers + payload).
		 * A single segment size cannot oversize this limit. */
	uint16_t packet_size_limit;
		/* Network Interface ID */
	uint16_t	niid;
		/** Queueing Destination Priority */
	uint8_t qd_priority;
		/** Maximum aggregated segments per packet limit.
		 * 0/1 are an illegal values. */
	uint8_t	seg_num_limit;
		/* padding*/
	uint8_t pad[14];
};


/**************************************************************************//**
@Description	GRO Global parameters
*//***************************************************************************/

struct gro_global_parameters {
		/** GRO Timeout flags \ref GRO_INTERNAL_TIMEOUT_FLAGS. */
	uint32_t timeout_flags;
		/** Should got either as a global define or as a return
		 * parameter from a dedicated  ARENA function
		 * (epid = get_tmi_epid(tmi_id)).*/
	uint8_t  gro_timeout_epid;
};

/**************************************************************************//**
@Description	TCP GRO Header Structure

		Includes TCP header + Timestamp option.

*//***************************************************************************/
#pragma pack(push, 1)
struct tcphdr_gro {
	/** TCP structure */
	struct tcphdr tcp;
		/** TCP option kind */
	uint8_t  option_kind;
		/** TCP option length */
	uint8_t  option_length;
		/** TCP timestamp option value of the TCP sending the option. */
	uint32_t tsval;
		/** TCP timestamp echo reply option value of the TCP sending the
		 *  option. */
	uint32_t tsecr;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	TCP GRO Optimized Header Structure

		Includes TCP header + Optimized Timestamp option.

*//***************************************************************************/
#pragma pack(push, 1)
struct tcphdr_gro_opt {
	/** TCP structure */
	struct tcphdr tcp;
		/** TCP Timestamp first nop */
	uint8_t	nop1;
		/** TCP Timestamp second nop */
	uint8_t	nop2;
		/** TCP option kind */
	uint8_t	option_kind;
		/** TCP option length */
	uint8_t	option_length;
		/** TCP timestamp option value of the TCP sending the option. */
	uint32_t tsval;
		/** TCP timestamp echo reply option value of the TCP sending the
		 *  option. */
	uint32_t tsecr;
};
#pragma pack(pop)

/** @} */ /* end of TCP_GRO_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group	TCP_GRO_INTERNAL_MACROS TCP GRO Internal Macros

@Description	TCP GRO Internal Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GRO_GENERAL_INT_DEFINITIONS TCP GRO General Internal Definitions

 @Description TCP GRO General Internal Definitions.

 @{
*//***************************************************************************/

	/** Size of GRO Context. */
#define SIZEOF_GRO_CONTEXT	(sizeof(struct tcp_gro_context))
	/* GRO internal struct size assertion check */
#pragma warning_errors on
ASSERT_STRUCT_SIZE(SIZEOF_GRO_CONTEXT, TCP_GRO_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of TCP_GRO_GENERAL_INT_DEFINITIONS */


/**************************************************************************//**
 @Group	TCP_GRO_INTERNAL_FLAGS TCP GRO Internal Flags

 @Description TCP GRO Internal Flags.

 @{
*//***************************************************************************/

	/** If set, TCP header of the GRO aggregation has timestamp.	*/
#define GRO_HAS_TIMESTAMP		0x00000001
	/** If set, TCP GRO is in exclusive mode. Otherwise, TCP GRO is in
	 * concurrent mode.	*/
#define GRO_OSM_EXLUSIVE_MODE		0x00000002
	/** If set, Flush aggregation immediately. */
#define GRO_FLUSH_AGG_SET		0x00000004
	/** If set, the segment was discarded. */
#define GRO_DISCARD_SEG_SET		0x00000008
	/** If set, there is a timer which already expired in process for the
	 * current aggregation. A new segment which arrives in this status will
	 * be returned as is since the gro context should be cleared by former
	 * aggregation. */
#define GRO_AGG_TIMER_IN_PROCESS	0x00000010
	/** IP header reserved1 ECN bit of the GRO aggregation.
#define GRO_ECN1			0x00010000
	* IP header reserved2 ECN bit of the GRO aggregation.
#define GRO_ECN2			0x00020000
*/
/** @} */ /* end of TCP_GRO_INTERNAL_FLAGS */

/**************************************************************************//**
 @Group	TCP_GRO_INTERNAL_MASKS TCP GRO Internal Masks

 @Description TCP GRO Internal Masks.

 @{
*//***************************************************************************/

/** ECN mask value of the IP header of the GRO aggregation.
 * The Mask should be used on the GRO internal flags \ref TCP_GRO_INTERNAL_FLAGS
 * in order to get the ECN value of the first segment. */
#define GRO_ECN_MASK			0x00003000

/** @} */ /* end of TCP_GRO_INTERNAL_MASKS */

/**************************************************************************//**
 @Group	TCP_GRO_INTERNAL_OFFSETS TCP GRO Internal Offsets

 @Description TCP GRO Internal Offsets.

 @{
*//***************************************************************************/

/** GRO ECN offset value. */
#define GRO_ECN_OFFSET	4

/** @} */ /* end of TCP_GRO_INTERNAL_OFFSETS */


/**************************************************************************//**
@Group	TCP_GRO_AGGREGATE_INTERNAL_STATUS  TCP GRO Aggregation Internal Statuses

@Description \ref tcp_gro_aggregate_seg() return internal values

@{
*//***************************************************************************/

	/** A segment has started a new aggregation and the aggregation has
	 * completed. */
#define	TCP_GRO_SEG_AGG_DONE_NEW_AGG				\
		(TCP_GRO_SEG_AGG_DONE | TCP_GRO_METADATA_USED)
	/** A segment has started a new aggregation and the aggregation is not
	 * completed. */
#define	TCP_GRO_SEG_AGG_NOT_DONE_NEW_AGG			\
		(TCP_GRO_SEG_AGG_NOT_DONE | TCP_GRO_METADATA_USED)
	/** A segment has started new aggregation, and the previous aggregation
	 * is completed. */
#define	TCP_GRO_SEG_AGG_DONE_AGG_OPEN_NEW_AGG			\
	(TCP_GRO_SEG_AGG_DONE_AGG_OPEN | TCP_GRO_METADATA_USED)


/** @} */ /* end of TCP_GRO_AGGREGATE_INTERNAL_STATUS */

/**************************************************************************//**
@Group	TCP_GRO_AGGREGATE_DEFINITIONS  TCP GRO Aggregation Internal Definitions

@Description TCP GRO Aggregation Internal Definitions

@{
*//***************************************************************************/

	/** Metadata 1st member size. */
#define METADATA_MEMBER1_SIZE (sizeof(					\
		((struct tcp_gro_context_metadata *)0)->seg_sizes_addr))
	/** Metadata 2nd member size. */
#define METADATA_MEMBER2_SIZE (sizeof(					\
		((struct tcp_gro_context_metadata *)0)->seg_num))
	/** Metadata 3rd member size. */
#define METADATA_MEMBER3_SIZE (sizeof(					\
		((struct tcp_gro_context_metadata *)0)->max_seg_size))
	/** Internal flags member size. */
#define INT_FLAGS_SIZE (sizeof(					\
		((struct tcp_gro_context *)0)->internal_flags))

	/* agg_num_cntr counter offset in statistics structure */
#define GRO_STAT_AGG_NUM_CNTR_OFFSET					\
	offsetof(struct tcp_gro_stats_cntrs, agg_num_cntr)
	/* seg_num_cntr counter offset in statistics structure */
#define GRO_STAT_SEG_NUM_CNTR_OFFSET					\
	offsetof(struct tcp_gro_stats_cntrs, seg_num_cntr)
	/* agg_timeout_cntr counter offset in statistics structure */
#define GRO_STAT_AGG_TIMEOUT_CNTR_OFFSET				\
	offsetof(struct tcp_gro_stats_cntrs, agg_timeout_cntr)
	/* agg_max_seg_num_cntr counter offset in statistics structure */
#define GRO_STAT_AGG_MAX_SEG_NUM_CNTR_OFFSET				\
	offsetof(struct tcp_gro_stats_cntrs, agg_max_seg_num_cntr)
	/* agg_max_packet_size_cntr counter offset in statistics structure */
#define GRO_STAT_AGG_MAX_PACKET_SIZE_CNTR_OFFSET			\
	offsetof(struct tcp_gro_stats_cntrs, agg_max_packet_size_cntr)
	/* unexpected_seq_num_cntr counter offset in statistics structure */
#define GRO_STAT_UNEXPECTED_SEQ_NUM_CNTR_OFFSET				\
	offsetof(struct tcp_gro_stats_cntrs, unexpected_seq_num_cntr)
	/* agg_flush_request_num_cntr counter offset in statistics structure */
#define GRO_STAT_AGG_FLUSH_REQUEST_NUM_CNTR_OFFSET			\
	offsetof(struct tcp_gro_stats_cntrs, agg_flush_request_num_cntr)
	/* agg_discarded_seg_num_cntr counter offset in statistics structure */
#define GRO_STAT_AGG_DISCARDED_SEG_NUM_CNTR_OFFSET			\
	offsetof(struct tcp_gro_stats_cntrs, agg_discarded_seg_num_cntr)

	/* TCP Timestamp option kind */
#define TCP_GRO_TCP_TIMSTAMP_OPTION_KIND	8
	/* IPV6 ECN_OFFSET */
#define	TCP_GRO_IPV6_ECN_OFFSET			4
	/* Timer Handle Mask in TIMER FD */
#define TIMER_HANDLE_MASK			0x00FFFFFF
	/* TCP Timestamp option NOP value */
#define TIMESTAMP_NOP_VAL			1
	/* TCP GRO granularity shift value */
#define GRO_GRAN_OFFSET				16
	/* Invalid timer handle */
#define TCP_GRO_INVALID_TMAN_HANDLE		0xFFFFFFFF

/** @} */ /* end of TCP_GRO_AGGREGATE_DEFINITIONS */


/** @} */ /* end of TCP_GRO_AGGREGATE_INTERNAL_STATUS */


/** @} */ /* end of TCP_GRO_INTERNAL_MACROS */


/**************************************************************************//**
@Group		TCP_GRO_INTERNAL_FUNCTIONS TCP GRO Internal Functions

@Description	TCP GRO Internal Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	tcp_gro_add_seg_to_aggregation

@Description	Add segment to an existing aggregation.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context.
@Param[in]	params - Pointer to the TCP GRO aggregation parameters.
@Param[in]	gro_ctx - Pointer to the internal GRO context.

@Return		Status, please refer to \ref TCP_GRO_AGGREGATE_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_errors or
		\ref TMANReturnStatus for more details.

@Cautions	None.
*//***************************************************************************/
int tcp_gro_add_seg_to_aggregation(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		struct tcp_gro_context *gro_ctx);

/**************************************************************************//**
@Function	tcp_gro_add_seg_and_close_aggregation

@Description	Add segment to aggregation and close aggregation.

@Param[in]	gro_ctx - Pointer to the internal GRO context.

@Return		Status, please refer to \ref TCP_GRO_AGGREGATE_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_errors or
		\ref TMANReturnStatus for more details.

@Cautions	None.
*//***************************************************************************/
int tcp_gro_add_seg_and_close_aggregation(
		struct tcp_gro_context *gro_ctx);

/**************************************************************************//**
@Function	tcp_gro_close_aggregation_and_open_new_aggregation

@Description	Close an existing aggregation and start a new aggregation with
		the new segment.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context.
@Param[in]	params - Pointer to the TCP GRO aggregation parameters.
@Param[in]	gro_ctx - Pointer to the internal GRO context.

@Return		Status, please refer to \ref TCP_GRO_AGGREGATE_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_errors or
		\ref TMANReturnStatus for more details.

@Cautions	None.
*//***************************************************************************/
int tcp_gro_close_aggregation_and_open_new_aggregation(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		struct tcp_gro_context *gro_ctx);

/**************************************************************************//**
@Function	tcp_gro_timeout_callback

@Description	TCP GRO timeout callback.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void tcp_gro_timeout_callback(
		uint64_t tcp_gro_context_addr,
		uint16_t opaque2);

/**************************************************************************//**
@Function	tcp_gro_calc_tcp_header_cksum

@Description	Calculate the TCP pseudo header checksum.
		The result checksum will be placed in the TCP->checksum field.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void tcp_gro_calc_tcp_header_cksum();

/** @} */ /* end of TCP_GRO_INTERNAL_FUNCTIONS */


/** @} */ /* end of AIOP_TCP_GRO_INTERNAL */


/** @} */ /* end of FSL_AIOP_GRO */


#endif /* __GRO_H */
