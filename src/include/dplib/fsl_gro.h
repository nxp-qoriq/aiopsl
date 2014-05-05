/**************************************************************************//**
 @File          fsl_gro.h

 @Description   This file contains the AIOP SW TCP GRO API
*//***************************************************************************/
#ifndef __FSL_GRO_H
#define __FSL_GRO_H

#include "common/types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

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
		expired.
		The user provides this function and the GRO process invokes it.

@Param[in]	arg - Address (in HW buffers) of the argument to the callback
		function.

 *//***************************************************************************/
typedef void (gro_timeout_cb_t)(uint64_t arg);

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

	/** A segment was aggregated and the aggregation is completed. */
#define	TCP_GRO_SEG_AGG_DONE		(TCP_GRO_MODULE_STATUS_ID | 0x1)
	/** A segment was aggregated and the aggregation is not completed. */
#define	TCP_GRO_SEG_AGG_NOT_DONE	(TCP_GRO_MODULE_STATUS_ID | 0x2)
	/** A segment has started new aggregation, and the previous aggregation
	 * is completed. */
#define	TCP_GRO_SEG_AGG_DONE_AGG_OPEN	(TCP_GRO_MODULE_STATUS_ID | 0x3)


	/** A new aggregation has started with the current segment.
	 * The metadata address was used by tcp_gro_aggregate_seg().
	 * This status bit can be return only as part of a combined status with
	 * one of the above statuses. */
#define	TCP_GRO_METADATA_USED		0x10
	/** A flush call (\ref tcp_gro_flush_aggregation()) is required by the
	 * user when possible.
	 * This status bit can be return only as part of a combined status with
	 * one of the above statuses. */
#define	TCP_GRO_FLUSH_REQUIRED		0x20

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
};

/**************************************************************************//**
@Description	TCP GRO packet metadata.
*//***************************************************************************/
struct tcp_gro_context_metadata {
		/** Address (in HW buffers) of the segment sizes. This field
		 * will be updated if \ref TCP_GRO_METADATA_SEGMENT_SIZES is
		 * set. For each segment, upper SW should allocate 2 bytes (to
		 * support up to 64KB length segments). */
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
		/** Timeout per packet (in 1 mSec granularity) aggregation
		 * limit. */
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

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context.
		The user should allocate \ref tcp_gro_ctx_t in
		this address.
@Param[in]	params - Pointer to the TCP GRO aggregation parameters.
@Param[in]	flags - Please refer to \ref TCP_GRO_AGG_FLAGS.

@Return		Status, please refer to \ref TCP_GRO_AGGREGATE_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_errors or
		\ref TMANReturnStatus for more details.

@Cautions	The user should zero the \ref tcp_gro_ctx_t allocated space once
		a new session begins.

@remark		When returning from this function, in case the aggregation is
		not done, no frame will be in the default frame area.
		Only when an aggregation is done, the aggregated frame will be
		in the default frame area.

*//***************************************************************************/
int32_t tcp_gro_aggregate_seg(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		uint32_t flags);


/**************************************************************************//**
@Function	tcp_gro_flush_aggregation

@Description	This function flush an open TCP packet
		aggregation.

		The aggregated packet will reside at the default frame location
		when this function returns.

@Param[in]	tcp_gro_context_addr - Address (in HW buffers) of the TCP GRO
		internal context. The user should allocate \ref tcp_gro_ctx_t in
		this address.

@Return		Status, please refer to \ref TCP_GRO_FLUSH_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_errors or
		\ref TMANReturnStatus for more details.

@Cautions	No frame should reside at the default frame location in
		workspace before this function is called.
*//***************************************************************************/
int32_t tcp_gro_flush_aggregation(
		uint64_t tcp_gro_context_addr);

/** @} */ /* end of GRO_Functions */
/** @} */ /* end of FSL_AIOP_GRO */
/** @} */ /* end of NETF */


#endif /* __FSL_GRO_H */
