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
 @File          fsl_gro.h

 @Description   This file contains the AIOP SW TCP GRO API
*//***************************************************************************/
#ifndef __FSL_GRO_H
#define __FSL_GRO_H

#include "types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_L4 L4

 @Description	AIOP L4 related API

 @{
 *//***************************************************************************/

/**************************************************************************//**
@Group		FSL_AIOP_GRO GRO

@Description	FSL_AIOP_GRO

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	GRO_MACROS GRO Macros

@Description	GRO Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	GRO_GENERAL_DEFINITIONS GRO General Definitions

 @Description GRO General Definitions.

 @{
*//***************************************************************************/

	/** TCP GRO context size definition. */
#define TCP_GRO_CONTEXT_SIZE	128
	/** TCP GRO context definition. */
typedef uint8_t tcp_gro_ctx_t[TCP_GRO_CONTEXT_SIZE];



/**************************************************************************//**
@Description	User callback function, called after aggregation timeout is
		expired. The user provides this function and the GRO process
		invokes it.

@Param[in]	arg - Address (in HW buffers) of the argument to the callback
		function.

 *//***************************************************************************/
typedef void (gro_timeout_cb_t)(uint64_t arg);


	/** TCP GRO metadata segment sizes address setting macro.
	 * _metadata_addr - address (in HW buffers) of the TCP GRO aggregation
	 * metadata structure, in which the _seg_sizes_addr will be set.
	 * _seg_sizes_addr - segment sizes external address (in HW buffers) to
	 * be set.
	 * */
#define TCP_GRO_SET_METADATA_SEG_SIZES_ADDR(_metadata_addr, _seg_sizes_addr)\
		cdma_write(_metadata_addr, &_seg_sizes_addr, 8)
/** @} */ /* end of GRO_GENERAL_DEFINITIONS */

/**************************************************************************//**
 @Group	GRO_FLAGS GRO Flags

 @Description GRO Flags.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GRO_AGG_FLAGS TCP GRO Aggregation Flags

 @Description Flags for \ref tcp_gro_aggregate_seg() function.

 The flags are allowed to be changed per new session only.

 @{
*//***************************************************************************/

	/** GRO no flags indication. */
#define TCP_GRO_NO_FLAGS				0x00000000
	/** If set, extended statistics is enabled.	*/
#define TCP_GRO_EXTENDED_STATS_EN			0x00000001
	/** If set, save the segment sizes in the metadata. */
#define TCP_GRO_METADATA_SEGMENT_SIZES			0x00000002
	/** If set, calculate TCP checksum. */
#define TCP_GRO_CALCULATE_TCP_CHECKSUM			0x00000004
	/** If set, calculate IP checksum. */
#define TCP_GRO_CALCULATE_IP_CHECKSUM			0x00000008


/** @} */ /* end of TCP_GRO_AGG_FLAGS */


/**************************************************************************//**
 @Group	TCP_GRO_TIMEOUT_GRANULARITY_FLAGS TCP GRO Timeout Granularity Flags

 @Description Flags for the timer granularity value.

 The flags are allowed to be changed per new session only.

 @{
*//***************************************************************************/

/* The following defines will be used to set the TMAN timer tick size.*/

	/** 1 uSec timer ticks*/
#define TCP_GRO_CREATE_TIMER_MODE_USEC_GRANULARITY		0x00
	/** 10 uSec timer ticks*/
#define TCP_GRO_CREATE_TIMER_MODE_10_USEC_GRANULARITY		0x01
	/** 100 uSec timer ticks*/
#define TCP_GRO_CREATE_TIMER_MODE_100_USEC_GRANULARITY		0x02
	/** 1 mSec timer ticks*/
#define TCP_GRO_CREATE_TIMER_MODE_MSEC_GRANULARITY		0x03
	/** 10 mSec timer ticks*/
#define TCP_GRO_CREATE_TIMER_MODE_10_MSEC_GRANULARITY		0x04
	/** 100 mSec timer ticks*/
#define TCP_GRO_CREATE_TIMER_MODE_100_MSEC_GRANULARITY		0x05
	/** 1 Sec timer ticks*/
#define TCP_GRO_CREATE_TIMER_MODE_SEC_GRANULARITY		0x06

/** @} */ /* end of TCP_GRO_TIMEOUT_GRANULARITY_FLAGS */


/** @} */ /* end of TCP_GRO_FLAGS */

/**************************************************************************//**
@Group	TCP_GRO_AGGREGATE_STATUS  TCP GRO Aggregation Statuses

@Description \ref tcp_gro_aggregate_seg() return values

@{
*//***************************************************************************/

	/** A segment was aggregated and the aggregation is completed.
	 * The aggregated frame is located in the default frame location. */
#define	TCP_GRO_SEG_AGG_DONE		(TCP_GRO_MODULE_STATUS_ID | 0x1)
	/** A segment was aggregated and the aggregation is not completed. */
#define	TCP_GRO_SEG_AGG_NOT_DONE	(TCP_GRO_MODULE_STATUS_ID | 0x2)
	/** A segment has started new aggregation, and the previous aggregation
	 * is completed. The aggregated frame is located in the default frame
	 * location. */
#define	TCP_GRO_SEG_AGG_DONE_AGG_OPEN	(TCP_GRO_MODULE_STATUS_ID | 0x4)
	/** The aggregation was discarded due to buffer pool depletion. (This
	 * status is returned only when there was no other option to continue
	 * processing the aggregated frame due to the buffer pool depletion). */
#define	TCP_GRO_AGG_DISCARDED		(TCP_GRO_MODULE_STATUS_ID | 0x8)
	/** A flush call (\ref tcp_gro_flush_aggregation()) is required by the
	 * user when possible.
	 * This status bit can be return as a stand alone status, or as part of
	 * a combined status with one of the above statuses. */
#define	TCP_GRO_FLUSH_REQUIRED		(TCP_GRO_MODULE_STATUS_ID | 0x10)
/** The aggregation timer has expired.
	 * The aggregation will be returned via timer callback.
	 * This status is return when a timer has expired but has not yet got
	 * the gro context. Since the timer has already expired it is getting
	 * precedence in returning the aggregation.
	 * This status bit can be return as a stand alone status (in this case
	 * no aggregated frame exists in the default frame location), or as part
	 * of a combined status with one of the above statuses (in this case
	 * an aggregated frame may exists in the default frame location (depends
	 * on the combined status), and another frame exists, which will be
	 * flushed by the timeout callback). */
#define	TCP_GRO_SEG_AGG_TIMER_IN_PROCESS (TCP_GRO_MODULE_STATUS_ID | 0x20)


	/** A new aggregation has started with the current segment.
	 * The metadata address was used by tcp_gro_aggregate_seg().
	 * This status bit can be return only as part of a combined status with
	 * one of the above statuses. */
#define	TCP_GRO_METADATA_USED			0x40
	/** The segment could not start an aggregation since no timers are
	 * available.
	 * This status bit can be return only as part of a combined status with
	 * one of the above statuses. */
#define	TCP_GRO_TIMER_UNAVAIL			0x80
	/** The segment was discarded due to buffer pool depletion.
	 * This status bit can be return only as part of a combined status with
	 * one of the above statuses. */
#define	TCP_GRO_SEG_DISCARDED			0xA0


/** @} */ /* end of TCP_GRO_AGGREGATE_STATUS */

/**************************************************************************//**
@Group	TCP_GRO_FLUSH_STATUS  TCP GRO Flush Statuses

@Description \ref tcp_gro_flush_aggregation() return values

@{
*//***************************************************************************/

	/** The aggregation is flushed. */
#define	TCP_GRO_FLUSH_AGG_DONE	SUCCESS
	/** No aggregation exists for the session. */
#define	TCP_GRO_FLUSH_NO_AGG	(TCP_GRO_MODULE_STATUS_ID | 0x1)

#define	TCP_GRO_FLUSH_TIMER_IN_PROCESS	TCP_GRO_SEG_AGG_TIMER_IN_PROCESS

/** @} */ /* end of TCP_GRO_FLUSH_STATUS */

/** @} */ /* end of GRO_MACROS */

/**************************************************************************//**
@Group		GRO_STRUCTS GRO Structures

@Description	AIOP GRO Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GRO statistics counters.
*//***************************************************************************/
struct tcp_gro_stats_cntrs {
		/** Counts the number of aggregated packets. */
	uint32_t	agg_num_cntr;
		/** Counts the number of aggregated segments. */
	uint32_t	seg_num_cntr;
		/** Counts the number of aggregations due to timeout limit.
		 * This counter is valid when extended statistics mode is
		 * enabled (\ref TCP_GRO_EXTENDED_STATS_EN) */
	uint32_t	agg_timeout_cntr;
		/** Counts the number of aggregations due to segment count
		 * limit. This counter is valid when extended statistics mode is
		 * enabled (\ref TCP_GRO_EXTENDED_STATS_EN) */
	uint32_t	agg_max_seg_num_cntr;
		/** Counts the number of aggregations due to max aggregated
		 * packet size limit. This counter is valid when extended
		 * statistics mode is enabled (\ref
		 * TCP_GRO_EXTENDED_STATS_EN) */
	uint32_t	agg_max_packet_size_cntr;
		/** Counts the number of segments in which the sequence number
		 * is not expected. This counter is valid when extended
		 * statistics mode is enabled (\ref TCP_GRO_EXTENDED_STATS_EN)*/
	uint32_t	unexpected_seq_num_cntr;
		/** Counts the number of aggregations due to flush request.
		 * This counter does not count cases when the flush is triggered
		 * due to a \ref TCP_GRO_FLUSH_NO_AGG status flag.
		 * This counter is valid when extended statistics mode is
		 * enabled (\ref TCP_GRO_EXTENDED_STATS_EN)*/
	uint32_t	agg_flush_request_num_cntr;
		/** Counts the number of discarded segments.
		 * This counter is valid when extended statistics mode is
		 * enabled (\ref TCP_GRO_EXTENDED_STATS_EN)*/
	uint32_t	agg_discarded_seg_num_cntr;
};

/**************************************************************************//**
@Description	TCP GRO packet metadata.
*//***************************************************************************/
struct tcp_gro_context_metadata {
		/** Address (in HW buffers) for the segment sizes. This field
		 * will be used by GRO only if
		 * \ref TCP_GRO_METADATA_SEGMENT_SIZES is set.
		 * Upper SW should initialize this field at
		 * \ref tcp_gro_context_params.metadata_addr (first 8 bytes) and
		 * GRO reads it from that location.
		 * For each segment, upper SW should allocate 2 bytes (to
		 * support up to 64KB length segments) starting at this
		 * address. E.g. If max segments per aggregation is 10 segments,
		 * 20 bytes per aggregation should be allocated starting at this
		 * address.
		 * This field can be set by using
		 * \ref TCP_GRO_SET_METADATA_SEG_SIZES_ADDR macro.*/
	uint64_t seg_sizes_addr;
		/** Number of segments in the aggregation. */
	uint16_t seg_num;
		/** Largest segment size*/
	uint16_t max_seg_size;
};

/**************************************************************************//**
@Description	GRO aggregation limits.

		These limits are allowed to be changed per new session only.
*//***************************************************************************/
struct gro_context_limits {
		/** Timeout per packet aggregation limit.
		 * The timeout granularity is specified at
		 * \ref gro_context_timeout_params.granularity. */
	uint16_t timeout_limit;
		/** Maximum aggregated packet size limit (The size refers to the
		 * packet headers + payload).
		 * A single segment size cannot oversize this limit. */
	uint16_t packet_size_limit;
		/** Maximum aggregated segments per packet limit.
		 * 0/1 are an illegal values. */
	uint8_t	seg_num_limit;
};

/**************************************************************************//**
@Description	GRO aggregation Timeout Parameters.

		These parameters are allowed to be changed per new session only.
*//***************************************************************************/
struct gro_context_timeout_params {
		/** Address (in HW buffers) of the callback function parameter
		 * argument on timeout.
		 * On timeout, GRO will call upper layer callback function with
		 * this parameter. */
	uint64_t gro_timeout_cb_arg;
		/** Function to call upon Time Out occurrence.
		 * This function takes one argument. */
	gro_timeout_cb_t *gro_timeout_cb;
		/** GRO timer granularity
		 * (\ref TCP_GRO_TIMEOUT_GRANULARITY_FLAGS). */
	uint8_t granularity;
		/** TMAN Instance ID. */
	uint8_t tmi_id;
};

/**************************************************************************//**
@Description	TCP GRO aggregation parameters.
*//***************************************************************************/
struct tcp_gro_context_params {
		/** TCP GRO aggregation timeout parameters. */
	struct gro_context_timeout_params timeout_params;
		/** Aggregated packet limits. */
	struct gro_context_limits limits;
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
};


/** @} */ /* end of GRO_STRUCTS */

/**************************************************************************//**
@Group		GRO_FUNCTIONS GRO Functions

@Description	GRO Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	tcp_gro_aggregate_seg

@Description	This function aggregates TCP segments to one TCP
		packet.

		When the aggregation is completed the aggregated packet will be
		located at the default frame location in the workspace.

		Pre-condition - The segment to be aggregated should be located
		in the default frame location in workspace.

		Implicit input parameters in Task Defaults: spid.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context.
		The user should allocate \ref tcp_gro_ctx_t in
		this address.
@Param[in]	params - Pointer to the TCP GRO aggregation parameters \ref
		tcp_gro_context_params.
@Param[in]	flags - Please refer to \ref TCP_GRO_AGG_FLAGS.

@Return		GRO Status, or negative value on error.

@Retval		GRO Status - please refer to \ref TCP_GRO_AGGREGATE_STATUS.
@Retval		EIO - Received segment FD contain errors (FD.err != 0).
		Recommendation is to discard the frame or enqueue the frame.
		The frame was not aggregated.
@Retval		ENOMEM - Received segment cannot be stored/aggregated due to
		buffer pool depletion.
		Recommendation is to discard the frame.
		The frame was not aggregated.

@remark		When returning from this function, in case the aggregation is
		not done, no frame will be in the default frame area.
		Only when an aggregation is done, the aggregated frame will be
		in the default frame area.
@remark		It is assumed that the address of the presented segment is 
		aligned to 16 bytes.

@Cautions	The user should zero the \ref tcp_gro_ctx_t allocated space once
		a new session begins.
@Cautions	This function requires 1 CDMA mutex (out of 4 available per 
		task).
@Cautions	This function requires 1 FDMA working frame (out of 6 available 
		per task), and 1 FDMA segment (out of 8 available per task).
*//***************************************************************************/
int tcp_gro_aggregate_seg(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		uint32_t flags);


/**************************************************************************//**
@Function	tcp_gro_flush_aggregation

@Description	This function flush an open TCP packet
		aggregation.

		The aggregated packet will reside at the default frame location
		when this function returns.

		Implicitly updated values in Task Defaults:  frame handle,
		segment handle, segment address, segment offset, segment length.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context. The user should allocate \ref tcp_gro_ctx_t in
		this address. 

@Return		GRO Status - please refer to \ref TCP_GRO_FLUSH_STATUS.

@Cautions	No frame should reside at the default frame location in
		workspace before this function is called.
@Cautions	This function requires 1 CDMA mutex (out of 4 available per 
		task).
@Cautions	This function requires 1 FDMA working frame (out of 6 available 
		per task), and 1 FDMA segment (out of 8 available per task).
*//***************************************************************************/
int tcp_gro_flush_aggregation(
		uint64_t tcp_gro_context_addr);

/** @} */ /* end of GRO_Functions */
/** @} */ /* end of FSL_AIOP_GRO */
/** @} */ /* end of group AIOP_L4 */
/** @} */ /* end of NETF */


#endif /* __FSL_GRO_H */
