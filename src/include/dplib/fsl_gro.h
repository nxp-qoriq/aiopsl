/**************************************************************************//**
 @File          fsl_gro.h

 @Description   This file contains the AIOP SW TCP GRO API
*//***************************************************************************/
#ifndef __FSL_GRO_H
#define __FSL_GRO_H

#include "general.h"

/**************************************************************************//**
@Group		FSL_AIOP_TCP_GRO FSL AIOP TCP GRO

@Description	FSL AIOP TCP GRO

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	TCP_GRO_MACROS TCP GRO Macros

@Description	TCP GRO Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GRO_GENERAL_DEFINITIONS TCP GRO General Definitions

 @Description TCP GRO General Definitions.

 @{
*//***************************************************************************/

	/** GRO context size definition. */
#define GRO_CONTEXT_SIZE	128
	/** GRO context definition. */
typedef uint8_t gro_ctx_t[GRO_CONTEXT_SIZE];



/**************************************************************************//**
@Description	User callback function, called after aggregation timeout is
		expired.
		The user provides this function and the GRO process invokes it.

@Param[in]	handle - The handle that was deleted.

 *//***************************************************************************/
typedef void (t_callback_on_timeout)(uint64_t handle);

/** @} */ /* end of TCP_GRO_GENERAL_DEFINITIONS */

/**************************************************************************//**
 @Group	TCP_GRO_FLAGS TCP GRO Flags

 @Description Flags for \ref gro_aggregate_seg() function.

 @{
*//***************************************************************************/

	/** GRO no flags indication. */
#define TCP_GRO_NO_FLAGS		0x00000000
	/** If set, extended statistics is enabled. 	*/
#define TCP_GRO_EXTENDED_STATS_EN	0x00000001
	/** If set, save the segment sizes in the metadata. */
#define TCP_GRO_METADATA_SEGMENT_SIZES	0x00000002
	/** If set, calculate TCP and IP checksum. */
#define TCP_GRO_CALCULATE_CHECKSUM	0x00000004


/** @} */ /* end of TCP_GRO_FLAGS */

/**************************************************************************//**
@Group	TCP_GRO_AGGREGATE_STATUS  TCP GRO Aggregation Statuses

@Description \ref gro_aggregate_seg() return values

@{
*//***************************************************************************/

	/** A segment was aggregated and the aggregation is not completed. */
#define	TCP_GRO_SEG_AGG_NOT_DONE	(TCP_GRO_MODULE_STATUS_ID | 0x1)
	/** A segment was aggregated and the aggregation is completed. */
#define	TCP_GRO_SEG_AGG_DONE		(TCP_GRO_MODULE_STATUS_ID | 0x2)
	/** A segment has started new aggregation, and the previous aggregation
	 * is completed. */
#define	TCP_GRO_SEG_AGG_DONE_AGG_OPEN	(TCP_GRO_MODULE_STATUS_ID | 0x3)

/** @} */ /* end of TCP_GRO_AGGREGATE_STATUS */

/**************************************************************************//**
@Group	TCP_GRO_FLUSH_STATUS  TCP GRO Flush Statuses

@Description \ref gro_flush_aggregation() return values

@{
*//***************************************************************************/

	/** The aggregation is flushed. */
#define	TCP_GRO_FLUSH_AGG_DONE	SUCCESS
	/** No aggregation exists for the session. */
#define	TCP_GRO_FLUSH_NO_AGG	(TCP_GRO_MODULE_STATUS_ID | 0x1)

/** @} */ /* end of TCP_GRO_FLUSH_STATUS */

/** @} */ /* end of TCP_GRO_MACROS */

/**************************************************************************//**
@Group		TCP_GRO_STRUCTS TCP GRO Structures

@Description	AIOP TCP GRO Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GRO statistics counters.
*//***************************************************************************/
struct gro_stats_cntrs {
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
};

/**************************************************************************//**
@Description	TCP GRO packet metadata.
*//***************************************************************************/
struct gro_context_metadata {
		/** Address (in BMAN buffers) of the segment sizes. This field
		 * will be updated if \ref TCP_GRO_METADATA_SEGMENT_SIZES is
		 * set. */
	uint64_t seg_sizes;
		/** Number of segments in the aggregation. */
	uint16_t seg_num;
		/** Largest segment size*/
	uint16_t max_seg_size;
		/** Padding */
	uint8_t	pad[4];
};

/**************************************************************************//**
@Description	TCP GRO aggregation limits.
*//***************************************************************************/
struct gro_context_limits {
		/** Timeout per packet aggregation limit. */
	uint32_t timeout_limit;
		/** Maximum aggregated packet size limit. */
	uint16_t packet_size_limit;
		/** Maximum aggregated segments per packet limit. */
	uint8_t	seg_num_limit;
		/** Padding */
	uint8_t	pad[1];

};

/**************************************************************************//**
@Description	TCP GRO aggregation parameters.
*//***************************************************************************/
struct gro_context_params {
		/** TCP GRO aggregation metadata. */
	struct gro_context_metadata metadata;
		/** Address (in BMAN buffers) of the TCP GRO statistics counters
		 *  (\ref gro_stats_cntrs). */
	uint64_t statistics;
		/** Callback function parameter on timeout.
		 * On timeout, GRO will call upper layer callback function with
		 * this parameter. */
	uint64_t callback_param_on_timeout;
		/** Aggregated packet limits. */
	struct gro_context_limits limits;
		/** Function to call upon Time Out occurrence.
		 * This function takes one argument
		 * (\ref gro_context_params.callback_param_on_timeout). */
	t_callback_on_timeout	*callback_on_timeout;
		/** Padding */
	uint8_t	pad[4];
};


/** @} */ /* end of TCP_GRO_STRUCTS */

/**************************************************************************//**
@Group		TCP_GRO_Functions TCP GRO Functions

@Description	TCP GRO Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	gro_aggregate_seg

@Description	This function aggregates TCP segments to one TCP
		packet.

		When the aggregation is completed the aggregated packet will be
		located at the default frame location in the workspace.

		Pre-condition - The segment to be aggregated should be located
		in the default frame location in workspace.

@Param[in]	gro_ctx_addr - Address (in BMAN buffers) of the TCP GRO internal
		context. The user should allocate \ref gro_ctx_t in this
		address.
@Param[in]	params - Pointer to the aggregated packet metadata.
@Param[in]	flags - Please refer to \ref TCP_GRO_FLAGS.

@Return		Status, please refer to \ref TCP_GRO_AGGREGATE_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_hw_errors or
		\ref TMANReturnStatus for more details.

@Cautions	None.
*//***************************************************************************/
int32_t gro_aggregate_seg(
		uint64_t gro_ctx_addr,
		struct gro_context_params *params,
		uint32_t flags);


/**************************************************************************//**
@Function	gro_flush_aggregation

@Description	This function flush an open TCP packet
		aggregation.

		The aggregated packet will reside at the default frame location
		when this function returns.

@Param[in]	gro_ctx_addr - Address (in BMAN buffers) of the TCP GRO internal
		context. The user should allocate \ref gro_ctx_t in this
		address.

@Return		Status, please refer to \ref TCP_GRO_FLUSH_STATUS,
		\ref fdma_hw_errors, \ref fdma_sw_errors, \ref cdma_hw_errors or
		\ref TMANReturnStatus for more details.

@Cautions	No frame should reside at the default frame location in
		workspace before this function is called.
*//***************************************************************************/
int32_t gro_flush_aggregation(
		uint64_t gro_ctx_addr);




/** @} */ /* end of TCP_GRO_Functions */


/** @} */ /* end of FSL_AIOP_TCP_GRO */



#endif /* __FSL_GRO_H */
