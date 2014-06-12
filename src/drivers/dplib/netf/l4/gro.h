/**************************************************************************//**
 @File          gro.h

 @Description   This file contains the AIOP SW internal TCP GRO API
*//***************************************************************************/
#ifndef __GRO_H
#define __GRO_H

#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_gro.h"
#include "dplib/fsl_fdma.h"



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
/* Currently the struct size is 128 bytes.
 * The fixed definition size is 128 bytes.
 * In case additional fields are added to the struct we can either:
 * Modify the fixed size
 * OR
 * Remove un-needed fields such as the limits (we get them every time)
 * and the timeout_flags and tmi_id of the timeout parameters (16 bytes
 * of un-needed bytes currently)*/

struct tcp_gro_context {
		/** Aggregated packet FD.
		 * This field must remain at the beginning of the structure due
		 * to alignment restrictions for FD in workspace (The FD adress
		 * in Workspace must be aligned to 32 bytes). */
	struct ldpaa_fd agg_fd
		__attribute__((aligned(sizeof(struct ldpaa_fd))));
		/** Aggregation parameters */
	struct tcp_gro_context_params params;
		/** Last Segment header fields which we need to update in the
		 * aggregated packet. */
	struct tcp_gro_last_seg_header_fields last_seg_fields;
		/** Aggregated packet metadata  */
	struct tcp_gro_context_metadata metadata;
		/** Internal TCP GRO flags */
	uint32_t internal_flags;
		/** Next expected sequence number. */
	uint32_t next_seq;
		/** Last received acknowledgment number. */
	uint32_t last_ack;
		/** Aggregated packet timestamp value. */
	uint32_t timestamp;
		/** TCP GRO aggregation flags */
	uint32_t flags;
		/** Aggregated packet isolation attributes.
		 * Todo - in case all the segments have the same isolation
		 * attributes there is no need to save this structure since the
		 * attributes can be taken from the new segment Additional
		 * Dequeue Context area. */
	struct fdma_amq agg_fd_isolation_attributes;
		/** TMAN Instance ID. */
	uint32_t timer_handle;
		/** Aggregation headers size */
	uint16_t agg_headers_size;
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
#define TCP_GRO_HAS_TIMESTAMP		0x00000001
	/** If set, TCP GRO is in exclusive mode. Otherwise, TCP GRO is in
	 * concurrent mode.	*/
#define TCP_GRO_OSM_EXLUSIVE_MODE	0x00000002
	/** If set, Flush aggregation immediately. */
#define TCP_GRO_FLUSH_AGG_SET		0x00000004
	/** IP header reserved1 ECN bit of the GRO aggregation. */
#define TCP_GRO_ECN1			0x00010000
	/** IP header reserved2 ECN bit of the GRO aggregation. */
#define TCP_GRO_ECN2			0x00020000

/** @} */ /* end of TCP_GRO_INTERNAL_FLAGS */

/**************************************************************************//**
 @Group	TCP_GRO_INTERNAL_MASKS TCP GRO Internal Masks

 @Description TCP GRO Internal Masks.

 @{
*//***************************************************************************/

/** ECN mask value of the IP header of the GRO aggregation.
 * The Mask should be used on the GRO internal flags \ref TCP_GRO_INTERNAL_FLAGS
 * in order to get the ECN value of the first segment. */
#define TCP_GRO_ECN_MASK	0x00030000

/** @} */ /* end of TCP_GRO_INTERNAL_MASKS */


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
int32_t tcp_gro_add_seg_to_aggregation(
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
int32_t tcp_gro_add_seg_and_close_aggregation(
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
int32_t tcp_gro_close_aggregation_and_open_new_aggregation(
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

@Description	Calculate the TCP pseudo header checksum and add it to the
		accumulated payload checksum (which was calculated previously).
		The result checksum will be placed in the TCP->checksum field.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void tcp_gro_calc_tcp_header_cksum();

/** @} */ /* end of TCP_GRO_INTERNAL_FUNCTIONS */


/** @} */ /* end of AIOP_TCP_GRO_INTERNAL */


/** @} */ /* end of FSL_AIOP_GRO */


#endif /* __GRO_H */
