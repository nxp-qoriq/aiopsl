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
@File		fsl_dpni_drv.h

@Description	Data Path Network Interface API
*//***************************************************************************/
#ifndef __FSL_DPNI_DRV_H
#define __FSL_DPNI_DRV_H

#include "fsl_ldpaa.h"
#include "fsl_net.h"
#include "fsl_dpkg.h"

#include "fsl_ep.h"
#include "fsl_fdma.h"


/**************************************************************************//**
@Group		dpni_drv_g DPNI DRV

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		DPNI_DRV_ORDER_SCOPE Order Scope options

 @Description	Set initial order scope to "No order scope".

 @{
*//***************************************************************************/
#define DPNI_DRV_NO_ORDER_SCOPE 0
/** @} end of group DPNI_DRV_ORDER_SCOPE */

/**************************************************************************//**
 @Group		DPNI_FS_TABLE_SIZE

 @Description	Number of Flow Steering entries per DPNI

 @{
*//***************************************************************************/
#define DPNI_FS_TABLE_SIZE 7
/** @} end of group DPNI_FS_TABLE_SIZE */

/**************************************************************************//**
 @Group		DPNI_DRV_LINK_OPT Link Options

 @Description	Available options to determine dpni link state.

 @{
*//***************************************************************************/
/** Enable auto-negotiation */
#define DPNI_DRV_LINK_OPT_AUTONEG		0x0000000000000001ULL
/** Enable half-duplex mode */
#define DPNI_DRV_LINK_OPT_HALF_DUPLEX		0x0000000000000002ULL
/** Enable pause frames */
#define DPNI_DRV_LINK_OPT_PAUSE			0x0000000000000004ULL
/** Enable a-symmetric pause frames */
#define DPNI_DRV_LINK_OPT_ASYM_PAUSE		0x0000000000000008ULL
/** Enable pause frames transmission */
#define DPNI_LINK_OPT_PFC_PAUSE			0x0000000000000010ULL
/** @} end of group DPNI_DRV_LINK_OPT */

/**************************************************************************//**
@Group		DPNI Attributes  DPNI Attributes

@Description	DPNI Driver Send Attributes

@{
*//***************************************************************************/
enum dpni_enqueue_attributes {
		/** Add transition to exclusive with increment
		 * scope ID to enqueue */
	DPNI_DRV_SEND_MODE_ORDERED			= 0x40000000,
		/** Add prestore then transition to exclusive
		 * with increment scope ID to FD enqueue */
	DPNI_DRV_SEND_MODE_PRESTORE_ORDERED	= 0x20000000
};

/**************************************************************************//**
 @Group         DPNI_DRV_SEND_MODE options

 @Description   Enqueue working frame modes bitmap values

 @{
*//***************************************************************************/
#define DPNI_DRV_SEND_MODE_NONE FDMA_ENWF_NO_FLAGS
#define DPNI_DRV_SEND_MODE_TERM FDMA_EN_TC_TERM_BITS
#define DPNI_DRV_SEND_MODE_ATTRIBUTE	dpni_enqueue_attributes
#ifndef LS2085A_REV1
#define DPNI_DRV_SEND_MODE_CONDTERM FDMA_EN_TC_CONDTERM_BITS
#define DPNI_DRV_SEND_MODE_RL FDMA_ENWF_RL_BIT
#endif
/** @} end of group DPNI_DRV_SEND_MODE */

#define DPNI_DRIVER_SEND_MODE_ATTRIBUTE_MASK \
	(DPNI_DRV_SEND_MODE_PRESTORE_ORDERED | DPNI_DRV_SEND_MODE_ORDERED)
#include "dpni_drv_rxtx_inline.h"

/**************************************************************************//**
@Description	Structure representing DPNI driver link state.

*//***************************************************************************/
struct dpni_drv_link_state {
	/** Rate */
	uint32_t rate;
	/** Mask of available options; use \ref DPNI_DRV_LINK_OPT values*/
	uint64_t options;
	/** Link state; '0' for down, '1' for up */
	int up;
};

/**************************************************************************//**
@Description	Structure representing DPNI driver link configuration.

*//***************************************************************************/
struct dpni_drv_link_cfg {
	/** Rate */
	uint32_t rate;
	/** Mask of available options; use \ref DPNI_DRV_LINK_OPT values */
	uint64_t options;
};

/**************************************************************************//**
@enum dpni_drv_counter

@Description	AIOP DPNI driver counter types

@{
*//***************************************************************************/

enum dpni_drv_counter {
	/** Counts ingress frames */
	DPNI_DRV_CNT_ING_FRAME,
	/** Counts ingress bytes */
	DPNI_DRV_CNT_ING_BYTE,
	/** Counts ingress frames dropped due to explicit 'drop' setting */
	DPNI_DRV_CNT_ING_FRAME_DROP,
	/** Counts ingress frames discarded due to errors */
	DPNI_DRV_CNT_ING_FRAME_DISCARD,
	/** Counts ingress multicast frames */
	DPNI_DRV_CNT_ING_MCAST_FRAME,
	/** Counts ingress multicast bytes */
	DPNI_DRV_CNT_ING_MCAST_BYTE,
	/** Counts ingress broadcast frames */
	DPNI_DRV_CNT_ING_BCAST_FRAME,
	/** Counts ingress broadcast bytes */
	DPNI_DRV_CNT_ING_BCAST_BYTES,
	/** Counts egress frames */
	DPNI_DRV_CNT_EGR_FRAME,
	/** Counts egress bytes */
	DPNI_DRV_CNT_EGR_BYTE,
	/** Counts egress frames discarded due to errors */
	DPNI_DRV_CNT_EGR_FRAME_DISCARD,
	/** Counts egress multicast frames */
	DPNI_DRV_CNT_EGR_MCAST_FRAME,
	/** Counts egress multicast bytes */
	DPNI_DRV_CNT_EGR_MCAST_BYTE,
	/** Counts egress broadcast frames */
	DPNI_DRV_CNT_EGR_BCAST_FRAME,
	/** Counts egress broadcast bytes */
	DPNI_DRV_CNT_EGR_BCAST_BYTES,
	/** Counts ingress frames discarded due to lack of buffers */
	DPNI_DRV_CNT_ING_FRAME_NO_BUFF_DISCARD,
	/** Counts egress confirmed frames */
	DPNI_DRV_CNT_EGR_CONF_FRAME
};
/* @} end of enum dpni_drv_counter */

/**************************************************************************//**
@enum dpni_drv_qos_counter

@Description	AIOP DPNI driver QoS counter types

@{
*//***************************************************************************/

enum dpni_drv_qos_counter {
	/** Counts egress bytes dequeued on a traffic class */
	DPNI_DRV_QOS_CNT_EGR_TC_DEQUEUE_BYTE,
	/** Counts egress frames dequeued on a traffic class */
	DPNI_DRV_QOS_CNT_EGR_TC_DEQUEUE_FRAME,
	/** Counts egress bytes in all frames whose enqueue was rejected, on a
	 * traffic class, due to either WRED or tail drop */
	DPNI_DRV_QOS_CNT_EGR_TC_REJECT_BYTE,
	/** Counts egress frame enqueues rejected, on a traffic class, due to
	 * either WRED or tail drop */
	DPNI_DRV_QOS_CNT_EGR_TC_REJECT_FRAME
};

/* @} end of enum dpni_drv_qos_counter */

/**************************************************************************//**
 @Group		DPNI_DRV_BUF_LAYOUT_OPT Buffer Layout modification options

 @Description	buffer layout modification options

 @{
*//***************************************************************************/
/** Select to modify the time-stamp setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_TIMESTAMP               0x00000001
/** Select to modify the parser-result setting; not applicable for Tx */
#define DPNI_DRV_BUF_LAYOUT_OPT_PARSER_RESULT           0x00000002
/** Select to modify the frame-status setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_FRAME_STATUS            0x00000004
/** Select to modify the private-data-size setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_PRIVATE_DATA_SIZE	0x00000008
/** Select to modify the data-alignment setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_DATA_ALIGN              0x00000010
/** Select to modify the data-head-room setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_DATA_HEAD_ROOM          0x00000020
/**!< Select to modify the data-tail-room setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_DATA_TAIL_ROOM          0x00000040

/**************************************************************************//**
@Description	 enum dpni_drv_request_frame_annotation - Frame annotation.

*//***************************************************************************/
enum dpni_drv_frame_annotation {
	/* Status word and Time stamp */
	DPNI_DRV_FA_STATUS_AND_TS = 0x01,
	/* Parser result */
	DPNI_DRV_FA_PARSER_RESULT = 0x02
};

/** @} end of group DPNI_DRV_BUF_LAYOUT_OPT */

/**************************************************************************//**
@Description	Structure representing DPNI buffer layout.

*//***************************************************************************/
struct dpni_drv_buf_layout {
	/** Flags representing the suggested modifications to the buffer
	 * layout; Use any combination of \ref DPNI_DRV_BUF_LAYOUT_OPT */
	uint32_t options;
	/** Pass timestamp value */
	int pass_timestamp;
	/** Pass parser results */
	int pass_parser_result;
	/** Pass frame status */
	int pass_frame_status;
	/** Size kept for private data (in bytes) */
	uint16_t private_data_size;
	/** Data alignment */
	uint16_t data_align;
	/** Data head room */
	uint16_t data_head_room;
	/** Data tail room */
	uint16_t data_tail_room;
};

/**************************************************************************//**
@Description	Structure representing DPNI ls_checksum.

*//***************************************************************************/
struct dpni_drv_tx_checksum {
	/* '1' to enable L3 checksum generation; '0' disable;*/
	uint16_t l3_checksum_gen;
	/* '1' to enable L4 checksum generation; '0' disable;*/
	uint16_t l4_checksum_gen;
};

/* Set to select color aware mode (otherwise - color blind) */
#define DPNI_DRV_POLICER_OPT_COLOR_AWARE         0x00000001
/* Set to discard frame with RED color */
#define DPNI_DRV_POLICER_OPT_DISCARD_RED         0x00000002

/**************************************************************************//**
@Description	 enum dpni_drv_policer_mode - selecting the policer mode

*//***************************************************************************/
enum dpni_drv_policer_mode {
	/* Policer is disabled */
	DPNI_DRV_POLICER_MODE_NONE = 0,
	/* Policer pass through */
	DPNI_DRV_POLICER_MODE_PASS_THROUGH,
	/* Policer algorithm RFC 2698 */
	DPNI_DRV_POLICER_MODE_RFC_2698,
	/* Policer algorithm RFC 4115 */
	DPNI_DRV_POLICER_MODE_RFC_4115
};

/**************************************************************************//**
@Description	 enum dpni_drv_policer_unit - DPNI policer units (bytes/packets)

*//***************************************************************************/
enum dpni_drv_policer_unit {
	DPNI_DRV_POLICER_UNIT_BYTES = 0,
	DPNI_DRV_POLICER_UNIT_PACKETS
};

/**************************************************************************//**
@Description	 enum dpni_drv_policer_color - selecting the policer color

*//***************************************************************************/
enum dpni_drv_policer_color {
	DPNI_DRV_POLICER_COLOR_GREEN = 0,
	DPNI_DRV_POLICER_COLOR_YELLOW,
	DPNI_DRV_POLICER_COLOR_RED
};

/**************************************************************************//**
@Description	Structure representing DPNI policer configuration.

*//***************************************************************************/
struct dpni_drv_rx_tc_policing_cfg{
	/* Mask of available options; use 'DPNI_DRV_POLICER_OPT_<X>' values */
	uint32_t options;
	/* Policer mode */
	enum dpni_drv_policer_mode mode;
	/* Bytes or Packets */
	enum dpni_drv_policer_unit unit;
	/* For pass-through mode the policer re-colors with this
	 * color any incoming packets. For Color aware non-pass-through mode:
	 * policer re-colors with this color all packets with FD[DROPP]>2. */
	enum dpni_drv_policer_color default_color;
	/* Committed information rate (CIR) in Kbps or packets/second */
	uint32_t cir;
	/* Committed burst size (CBS) in bytes or packets */
	uint32_t cbs;
	/* Peak information rate (PIR, rfc2698) in Kbps or packets/second */
	uint32_t eir;
	/* Peak burst size (PBS, rfc2698) in bytes or packets
	 * Excess burst size (EBS, rfc4115) in bytes or packets */
	uint32_t ebs;
};

/* Maximum number of traffic classes */
#define DPNI_DRV_MAX_TC                     8

/**************************************************************************//**
@Description	 enum dpni_drv_tx_schedule_mode - DPNI Tx scheduling mode

*//***************************************************************************/
enum dpni_drv_tx_schedule_mode {
	/* strict priority */
	DPNI_DRV_TX_SCHED_STRICT_PRIORITY = 0,
	/*  weighted based scheduling in group A */
	DPNI_DRV_TX_SCHED_WEIGHTED_A,
	/*  weighted based scheduling in group B */
	DPNI_DRV_TX_SCHED_WEIGHTED_B,
};

/**************************************************************************//**
@Description	struct dpni_drv_tx_schedule - Structure representing Tx
		scheduling configuration.

*//***************************************************************************/
struct dpni_drv_tx_schedule {
	/* scheduling mode */
	enum dpni_drv_tx_schedule_mode mode;
	/* Bandwidth represented in weights from 100 to 10000.
	 * Not applicable for 'strict-priority' mode*/
	uint16_t delta_bandwidth;
};

/**************************************************************************//**
@Description	struct dpni_drv_tx_selection - Structure representing
		transmission selection configuration.

*//***************************************************************************/
struct dpni_drv_tx_selection {
	/* An array of traffic-classes */
	struct dpni_drv_tx_schedule tc_sched[DPNI_DRV_MAX_TC];
	/* Priority of group A */
	uint32_t prio_group_A;
	/* Priority of group B */
	uint32_t prio_group_B;
	/* Treat A and B groups as separate */
	uint8_t separate_groups;
};

/**************************************************************************//**
@Description	struct dpni_drv_tx_shaping - Structure representing
		DPNI tx shaping configuration.

*//***************************************************************************/
struct dpni_drv_tx_shaping {
	/* rate in Mbps */
	uint32_t rate_limit;
	/* burst size in bytes (up to 64KB) */
	uint16_t max_burst_size;
};

/**************************************************************************//**
@Description	struct dpni_drv_qos_tbl - Structure representing
		QOS table configuration.

*//***************************************************************************/
struct dpni_drv_qos_tbl {
	/* I/O virtual address of 256 bytes DMA-able memory filled with
	 * key extractions to be used as the QoS criteria by calling
	 * dpni_prepare_key_cfg() */
	uint64_t key_cfg_iova;
	/* Set to '1' to discard frames in case of no match (miss);
	 * '0' to use the 'default_tc' in such cases. */
	uint8_t discard_on_miss;
	/* Used in case of no-match and 'discard_on_miss'= 0 */
	uint8_t default_tc;
};

/**************************************************************************//**
@Description	struct dpni_drv_qos_tbl - Structure representing
		Rule configuration for table lookup.

*//***************************************************************************/
struct dpni_drv_qos_rule {
	/* I/O virtual address of the key (must be in DMA-able memory) */
	uint64_t key_iova;
	/* I/O virtual address of the mask (must be in DMA-able memory) */
	uint64_t mask_iova;
	/* key and mask size (in bytes) */
	uint8_t key_size;
};

/**
 * DPNI driver errors
 */

/**************************************************************************//**
@Description	 DPNI driver errors

*//***************************************************************************/
/* Extract out of frame header error */
#define DPNI_DRV_EXTRACT_OUT_FRAME_HEADER_ERR	0x00020000

/* Frame length error */
#define DPNI_DRV_FRAME_LENGTH_ERR		0x00002000

/* Frame physical error */
#define DPNI_DRV_ERROR_FRAME_PHYSICAL_ERR	0x00001000

/* Parsing header error */
#define DPNI_DRV_PARSING_HEADER_ERR		0x00000020

/* Parser L3 checksum error */
#define DPNI_DRV_L3_CHECKSUM_ERR		0x00000004

/* Parser L4 checksum error */
#define DPNI_DRV_L4_CHECKSUM_ERR		0x00000001

/* All errors */
#define DPNI_DRV_ALL_ERR	(DPNI_DRV_EXTRACT_OUT_FRAME_HEADER_ERR | \
				DPNI_DRV_FRAME_LENGTH_ERR |		 \
				DPNI_DRV_ERROR_FRAME_PHYSICAL_ERR |	 \
				DPNI_DRV_PARSING_HEADER_ERR |		 \
				DPNI_DRV_L3_CHECKSUM_ERR |		 \
				DPNI_DRV_L4_CHECKSUM_ERR)

/**************************************************************************//**
@Description	 enum dpni_drv_error_action - DPNI behavior for errors.

*//***************************************************************************/
enum dpni_drv_error_action {
	/* Discard the frame */
	DPNI_DRV_ERR_ACTION_DISCARD = 0,
	/* Continue with the normal flow */
	DPNI_DRV_ERR_ACTION_CONTINUE = 1,
	/* Send the frame to the error queue */
	DPNI_DRV_ERR_ACTION_SEND_TO_ERR_QUEUE = 2
};

/**************************************************************************//**
@Description	struct dpni_drv_error_cfg - Structure representing DPNI errors
		treatment.

*//***************************************************************************/
struct dpni_drv_error_cfg {
	/* Errors mask; use 'DPNI_DRV_ERROR_<X> */
	uint32_t			errors;
	/* The desired action for the errors mask */
	enum dpni_drv_error_action	error_action;
};

/**************************************************************************//**
@Description	 enum dpni_drv_early_drop_mode - DPNI early drop mode.

*//***************************************************************************/
enum dpni_drv_early_drop_mode {
	/* early drop is disabled */
	DPNI_DRV_EARLY_DROP_MODE_NONE = 0,
	/* early drop in taildrop mode */
	DPNI_DRV_EARLY_DROP_MODE_TAIL,
	/* early drop in WRED mode */
	DPNI_DRV_EARLY_DROP_MODE_WRED
};

/**************************************************************************//**
@Description	 enum dpni_drv_congestion_unit - DPNI congestion units.

*//***************************************************************************/
enum dpni_drv_congestion_unit {
	/* bytes units */
	DPNI_DRV_CONGESTION_UNIT_BYTES = 0,
	/* frames units */
	DPNI_DRV_CONGESTION_UNIT_FRAMES
};

/**************************************************************************//**
@Description	struct dpni_drv_wred - Structure representing
		WRED configuration.

*//***************************************************************************/
struct dpni_drv_wred {
	/* maximum threshold that packets may be discarded. Above this
	 * threshold all packets are discarded. The maximum threshold must be
	 * less than 2^39.
	 * Approximate to be expressed as (x+256)*2^(y-1) due to HW
	 * implementation. */
	uint64_t max_threshold;
	/* minimum threshold that packets may be discarded at. */
	uint64_t min_threshold;
	/* probability that a packet will be discarded (1-100, associated with
	 * the max_threshold). */
	uint8_t drop_probability;
};

/**************************************************************************//**
@Description	struct dpni_drv_early_drop_cfg - Structure representing
		early-drop configuration.

*//***************************************************************************/
struct dpni_drv_early_drop_cfg {
	/* drop mode */
	enum dpni_drv_early_drop_mode mode;
	/* units type */
	enum dpni_drv_congestion_unit units;
	/* WRED - 'green' configuration */
	struct dpni_drv_wred green;
	/* WRED - 'yellow' configuration */
	struct dpni_drv_wred yellow;
	/* WRED - 'red' configuration */
	struct dpni_drv_wred red;
	/* tail drop threshold */
	uint32_t tail_drop_threshold;
};

/** Get PEB free buffers */
#define DPNI_DRV_PEB_FREE_BUFS		0x01

/** Get backup (DDR) free buffers */
#define DPNI_DRV_BACKUP_FREE_BUFS	0x02

/**************************************************************************//**
@Description	struct dpni_drv_free_bufs - Structure representing a snapshot
		of the current fill level (number of free buffers) of the DPNI
		configured buffer pools.

*//***************************************************************************/
struct dpni_drv_free_bufs {
	/* Number of free buffers in PEB pools */
	uint32_t peb_bp_free_bufs;
	/* Number of free buffers in DDR backup pools */
	uint32_t backup_bp_free_bufs;
};

/**************************************************************************//**
@Description	struct dpni_drv_sparser_param - Structure representing the
		information needed to activate(enable) a Soft Parser.

*//***************************************************************************/
struct dpni_drv_sparser_param {
	/* The "custom_header_first" must be set if the custom header to parse
	 * is the first header in the packet, otherwise "custom_header_first"
	 * must be cleared. Only Rev2 platforms support the setting of
	 * "custom_header_first" field. */
	uint8_t				custom_header_first;
	/* Hard HXS on which a soft parser is activated. This must be configured
	 * if the header to parse is not the first header in the packet. */
	enum parser_starting_hxs_code	link_to_hard_hxs;
	/* Soft Sequence Start PC */
	uint16_t			start_pc;
	/* Soft Sequence byte-code */
	uint8_t				*byte_code;
	/* Soft Sequence size */
	uint16_t			size;
	/* Pointer to the Parameters Array of the SP */
	uint8_t				*param_array;
	/* Parameters offset */
	uint8_t				param_offset;
	/* Parameters size */
	uint8_t				param_size;
};

/**************************************************************************//**
@Description	struct dpni_drv_taildrop - Structure representing tail-drop
		configuration.

*//***************************************************************************/
struct dpni_drv_taildrop {
	/** Enable/Disable taildrop */
	uint8_t	enable;
	/** Congestion units type */
	enum dpni_drv_congestion_unit units;
	/** Taildrop threshold */
	uint32_t threshold;
	/** Overhead accounting length (range -2048 to +2047) */
	int16_t oal;
};

/**************************************************************************//**
@Description	struct dpni_drv_early_drop - Structure representing early drop
		configuration.

*//***************************************************************************/
struct dpni_drv_early_drop {
	/** Enable/Disable early drop */
	uint8_t	enable;
	/** Congestion units type */
	enum dpni_drv_congestion_unit units;
	/** WRED - 'green' configuration */
	struct dpni_drv_wred green;
	/** WRED - 'yellow' configuration */
	struct dpni_drv_wred yellow;
	/** WRED - 'red' configuration */
	struct dpni_drv_wred red;
};

/**************************************************************************//**
@enum	dpni_drv_confirmation_mode
@Description	enum dpni_drv_confirmation_mode - Identifies one of the
		supported TX confirmation modes.

*//***************************************************************************/
enum dpni_drv_confirmation_mode {
	/** Each TX queue has its own matching confirmation queue.
	 * Confirmation is received for every frame. */
	DPNI_DRV_CONF_AFFINE,
	/** The TX queues share a single confirmation queue per DPNI.
	 * Confirmation is received for every frame. */
	DPNI_DRV_CONF_SINGLE,
	/** TX confirmation is disabled. */
	DPNI_DRV_CONF_DISABLE,
	/** Each TX queue has its own matching confirmation queue.
	 * Confirmation is received only for frames encountering errors. */
	DPNI_DRV_CONF_AFFINE_ERR_ONLY,
	/** The TX queues share a single confirmation queue per DPNI.
	 * Confirmation is received only for frames encountering errors. */
	DPNI_DRV_CONF_SINGLE_ERR_ONLY,
};

#include "fsl_dpni.h"

/**************************************************************************//**
@Description	dpni_drv_pools_cfg - Structure representing
		buffer pools configuration.

*//***************************************************************************/
typedef struct dpni_pools_cfg dpni_drv_pools_cfg;

/**************************************************************************//**
@Description	dpni_drv_statistics - Structure containing the statistics.

*//***************************************************************************/
typedef union dpni_statistics dpni_drv_statistics;

/**************************************************************************//**
@Description	dpni_drv_queue_type - Identifies a type of queue.

*//***************************************************************************/
typedef enum dpni_queue_type dpni_drv_queue_type;

/**************************************************************************//**
@Description	struct dpni_drv_congestion_notification_cfg - Structure
		representing congestion notification configuration.

*//***************************************************************************/
struct dpni_drv_congestion_notification_cfg {
	/** Congestion units type */
	enum dpni_drv_congestion_unit units;
	/** Above this threshold we enter a congestion state */
	uint32_t threshold_entry;
	/** Below this threshold we exit the congestion state */
	uint32_t threshold_exit;
	/** Mask of available options;
	 * Use the following values:
	 * 'DPNI_CONG_OPT_NOTIFY_DEST_ON_ENTER',
	 * 'DPNI_CONG_OPT_NOTIFY_DEST_ON_EXIT',
	 * 'DPNI_CONG_OPT_FLOW_CONTROL' (This will have effect only if
	 * flow control is enabled with dpni_set_link_cfg()) */
	uint16_t notification_mode;
};

/**************************************************************************//**
@Description	dpni_drv_statistics - Structure representing DPNI attributes.

*//***************************************************************************/
typedef struct dpni_attr dpni_drv_attr;

/**************************************************************************//**
@Description	Application Receive callback

		User provides this function. Driver invokes it when it gets a
		frame received on this interface.


@Return	OK on success; error code, otherwise.
*//***************************************************************************/
typedef void /*__noreturn*/ (rx_cb_t) (void);

/**************************************************************************//**
@Function	dpni_drv_register_rx_cb

@Description	Attaches a pointer to a call back function to a NI ID.

	The callback function will be called when the NI_ID receives a frame.

@Param[in]	ni_id  - The Network Interface ID
@Param[in]	cb - Callback function for Network Interface specified flow_id

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_register_rx_cb(uint16_t        ni_id,
			rx_cb_t		    *cb);

/**************************************************************************//**
@Function	dpni_drv_register_rx_cb_etype

@Description	Attaches a pointer to a call back function to a NI ID
		for a certain protocol defined by EtherType.
		The callback function will be called when the NI_ID receives
		a frame with the protocol defined by EtherType

@Param[in]	ni_id  - The Network Interface ID
@Param[in]	cb - Callback function for Network Interface specified flow_id
@Param[in]	etype - The etherType registered
			with dpni_drv_enable_etype_fs()

@Cautions	This method should be called only after
		dpni_drv_enable_etype_fs() for the same etherType

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_register_rx_cb_etype(uint16_t ni_id, rx_cb_t *cb, uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_unregister_rx_cb

@Description	Unregisters a NI callback function by replacing it with a
		pointer to a discard callback.
		The discard callback function will be called when the NI_ID
		receives a frame

@Param[in]	ni_id - The Network Interface ID

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_unregister_rx_cb(uint16_t		ni_id);

/**************************************************************************//**
@Function	dpni_drv_unregister_rx_cb_etype

@Description	Unregisters a NI callback function by replacing it with a
		pointer to a discard callback.
		The discard callback function will be called when the NI_ID
		receives a frame with a certain protocol defined by EtherType

@Param[in]	ni_id - The Network Interface ID
@Param[in]	etype - The etherType registered
			with dpni_drv_enable_etype_fs()

@Cautions	This method should be called only after
		dpni_drv_enable_etype_fs() for the same etherType

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_unregister_rx_cb_etype(uint16_t ni_id, uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_enable

@Description	Enable a NI_ID referenced by ni_id. Allows sending and
		receiving frames.

@Param[in]	ni_id   The Network Interface ID

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_enable(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_disable

@Description	Disables a NI_ID referenced by ni_id. Disallows sending and
		receiving frames

@Param[in]	ni_id	The Network Interface ID

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_disable(uint16_t ni_id);

/**************************************************************************//**
@Function	task_get_receive_niid

@Description	Get ID of NI on which the default packet arrived.

@Return	NI_IDs on which the default packet arrived.
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
inline uint16_t task_get_receive_niid(void);

/**************************************************************************//**
@Function	task_set_send_niid

@Description	Set the NI ID on which the packet should be sent.

@Param[in]	niid - The Network Interface ID

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int task_set_send_niid(uint16_t niid);

/**************************************************************************//**
@Function	task_get_send_niid

@Description	Get ID of NI on which the default packet should be sent.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int task_get_send_niid(void);


/**************************************************************************//**
@Function	dpni_drv_get_primary_mac_address

@Description	Get Primary MAC address of NI.

@Param[in]	ni_id - The Network Interface ID

@Param[out]	mac_addr - stores primary MAC address of the supplied NI.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_drv_get_primary_mac_addr(uint16_t ni_id,
		uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_set_primary_mac_addr

@Description	Set Primary MAC address of NI.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mac_addr - primary MAC address for given NI.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_set_primary_mac_addr(uint16_t ni_id,
                uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_add_mac_address

@Description	Adds unicast/multicast filter MAC address.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mac_addr - MAC address to be added to NI unicast/multicast
				filter.
@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_add_mac_addr(uint16_t ni_id,
          		const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_remove_mac_address

@Description	Removes unicast/multicast filter MAC address.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mac_addr - MAC address to be removed from NI
				unicast/multicast filter.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_remove_mac_addr(uint16_t ni_id,
          		const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_set_max_frame_length

@Description	Set the maximum received frame length.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mfl - MFL length.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_set_max_frame_length(uint16_t ni_id,
                          const uint16_t mfl);

/**************************************************************************//**
@Function	dpni_drv_get_max_frame_length

@Description	Get the maximum received frame length.

@Param[in]	ni_id - The Network Interface ID

@Param[out]	*mfl - pointer to store MFL length.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_get_max_frame_length(uint16_t ni_id,
                          uint16_t *mfl);

/**************************************************************************//**
@Function	sl_prolog

@Description	Network Interface SL prolog function. It is recommended to call
		this function at the beginning of the upper layer entry-point
		function, in this way it assures that HW presentation context
		is preserved (as needed for OSM functionality and ni_id
		resolution).
		It is also recommended that user AIOP entry-point function is
		declared with __declspec(entry_point) to assure it is not
		dead-stripped by the compiler.

@Retval		0 - Success.
		It is recommended that for any error value user should discard
		the frame and terminate the task.
@Retval		EIO - Parsing Error
@Retval		ENOSPC - Parser Block Limit Exceeds.
*//***************************************************************************/
inline int sl_prolog(void);

/**************************************************************************//**
@Function	sl_prolog_with_ref_take

@Description	Network Interface SL prolog function. It is recommended to call
		this function at the beginning of the upper layer entry-point
		function, in this way it assures that HW presentation context
		is preserved (as needed for OSM functionality and ni_id
		resolution).
		It is also recommended that user AIOP entry-point function is
		declared with __declspec(entry_point) to assure it is not
		dead-stripped by the compiler.
		
		Implied rcu_read_lock() on Rev2 platforms prior to accessing
		network interface parameters.

@Retval		0 - Success.
		It is recommended that for any error value user should discard
		the frame and terminate the task.
@Retval		EIO - Parsing Error
@Retval		ENOSPC - Parser Block Limit Exceeds.
*//***************************************************************************/
inline int sl_prolog_with_ref_take(void);

/**************************************************************************//**
@Function	sl_prolog_with_custom_header

@Description	Network Interface SL prolog function.
		It is mandatory to call this function if the first header in
		the packets arriving in AIOP must be parsed by a soft parser
		(custom header). This is supported only by the Rev2 platforms.
		It is recommended to call this function at the beginning of the
		upper layer entry-point function, in this way it assures that
		HW presentation context is preserved (as needed for OSM
		functionality and ni_id resolution).
		It is also recommended that user AIOP entry-point function is
		declared with __declspec(entry_point) to assure it is not
		dead-stripped by the compiler.

@Param[in]	start_hxs : The program counter of a soft parser loaded by the
		application in the instructions memory of the AIOP Parser.
		The program counter must be a value in the range 0x20..0x7fd.
		No checks are performed on the provided value.
		On Rev1 platforms the start_hxs is reset (Ethernet hard HXS).

@Retval		0 - Success.
		It is recommended that for any error value user should discard
		the frame and terminate the task.
@Retval		EIO - Parsing Error
@Retval		ENOSPC - Parser Block Limit Exceeds.
*//***************************************************************************/
inline int sl_prolog_with_custom_header(uint16_t start_hxs);

/**************************************************************************//**
@Function	sl_tman_expiration_task_prolog

@Description	Network Interface SL tman expiration task prolog function.
		This function initialize into WS the ICID as taken from the
		SPID and clear starting HXS and PRPID.
		It should be called from the beginning of user's timer
		expiration call-back function to assure that fdma create and
		fdma store functions will work properly.

@param[in]	spid - storage profile id.

*//***************************************************************************/
inline void sl_tman_expiration_task_prolog(uint16_t spid);

/**************************************************************************//**
@Function	dpni_drv_send

@Description	Network Interface send (AIOP store and enqueue) function.
	Store and enqueue the default Working Frame.

@Param[in]	ni_id - The Network Interface ID
		flags - Flags for working frame enqueue, see DPNI_DRV_SEND_MODE
	Implicit: Queuing Destination Priority (qd_priority) in the TLS.

@Retval		0 - Success.
	It is recommended that for any error value user should discard
	the frame and terminate the task.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN or due to
	DPNI link down. It is recommended calling fdma_discard_fd()
	afterwards and then terminate task.
@Retval		ENOMEM - Failed due to buffer pool depletion. It is recommended
	calling fdma_discard_default_frame() afterwards and then terminate task.
@Cautions	The frame to be enqueued must be open (presented)
	when calling this function
*//***************************************************************************/
inline int dpni_drv_send(uint16_t ni_id, uint32_t flags);

/**************************************************************************//**
@Function	dpni_drv_explicit_send

@Description	Network Interface explicit send (AIOP enqueue) function.
	Enqueue the explicit closed frame.

@Param[in]	ni_id - The Network Interface ID
	Implicit: Queuing Destination Priority (qd_priority) in the TLS.

@Param[in]	fd - pointer to the explicit FD.

@Retval		0 - Success.
	It is recommended that for any error value user should discard
	the frame and terminate the task.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN or due to
	DPNI link down. It is recommended calling fdma_discard_fd()
	afterwards and then terminate task.

@Cautions	The frame to be enqueued must be closed (stored) when calling
	this function

*//***************************************************************************/
int dpni_drv_explicit_send(uint16_t ni_id, struct ldpaa_fd *fd);

/**************************************************************************//**
@Function	dpni_drv_set_multicast_promisc

@brief		Enable/Disable multicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[in]	en - '1' for enabling/'0' for disabling

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_set_multicast_promisc(uint16_t ni_id, int en);

/**************************************************************************//**
@Function	dpni_drv_get_multicast_promisc

@brief		Get multicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[out]	en - '1' for enabled/'0' for disabled

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_get_multicast_promisc(uint16_t ni_id, int *en);

/**************************************************************************//**
@Function	dpni_drv_set_unicast_promisc

@brief		Enable/Disable unicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[in]	en - '1' for enabling/'0' for disabling

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_set_unicast_promisc(uint16_t ni_id, int en);

/**************************************************************************//**
@Function	dpni_drv_get_unicast_promisc

@brief		Get unicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[out]	en - '1' for enabled/'0' for disabled

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_get_unicast_promisc(uint16_t ni_id, int *en);

/**************************************************************************//**
@Function	dpni_drv_get_spid

@Description	Function to receive PEB storage profile ID for specified NI.

@Param[in]	ni_id   The Network Interface ID
@Param[out]	spid - storage profile to use PEB buffer pool(for now using 1 byte).

@Return	'0' on Success;
*//***************************************************************************/
int dpni_drv_get_spid(uint16_t ni_id, uint16_t *spid);

/**************************************************************************//**
@Function	dpni_drv_get_spid_ddr

@Description	Function to receive DDR storage profile ID for specified NI.

@Param[in]	ni_id   The Network Interface ID
@Param[out]	spid_ddr - storage profile to use DDR buffer pool
		(for now using 1 byte).

@Return	'0' on Success;
*//***************************************************************************/
int dpni_drv_get_spid_ddr(uint16_t ni_id, uint16_t *spid_ddr);

/**************************************************************************//**
@Function	dpni_drv_get_num_of_nis

@Description	Returns the number of NI_IDs in the system.  Called by the AIOP
		applications to learn the maximum number of available network
		interfaces.

@Return	Number of NI_IDs in the system
*//***************************************************************************/
int dpni_drv_get_num_of_nis(void);

/**************************************************************************//**
@Function	dpni_drv_set_concurrent

@Description	Function to set the initial ordering mode to concurrent for the given NI.

@Param[in]	ni_id   The Network Interface ID

@Cautions       This method should be called in boot mode only.

@Return	'0' on Success;
*//***************************************************************************/
int dpni_drv_set_concurrent(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_exclusive

@Description	Function to set the initial ordering mode to exclusive for the given NI.

@Param[in]	ni_id   The Network Interface ID

@Cautions       This method should be called in boot mode only.

@Return	'0' on Success;

*//***************************************************************************/
int dpni_drv_set_exclusive(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_concurrent_etype

@Description	Function to set the initial ordering mode to concurrent for
		the given NI for a certain protocol defined by EtherType

@Param[in]	ni_id   The Network Interface ID
@Param[in]	etype   The etherType registered
			with dpni_drv_enable_etype_fs()

@Cautions       This method should be called in boot mode only and after
		dpni_drv_enable_etype_fs() for the same etherType

@Return	'0' on Success;
	-ENOENT - When the EtherType was not registered
*//***************************************************************************/
int dpni_drv_set_concurrent_etype(uint16_t ni_id, uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_set_exclusive_etype

@Description	Function to set the initial ordering mode to exclusive for
		the given NI for a certain protocol defined by EtherType

@Param[in]	ni_id   The Network Interface ID
@Param[in]	etype   The etherType registered
			with dpni_drv_enable_etype_fs()

@Cautions       This method should be called in boot mode only and after
		dpni_drv_enable_etype_fs() for the same etherType

@Return	'0' on Success;
	-ENOENT - When the EtherType was not registered
*//***************************************************************************/
int dpni_drv_set_exclusive_etype(uint16_t ni_id, uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_get_ordering_mode

@Description	Returns the configuration in epid table for ordering mode.

@Param[in]	ni_id - Network Interface ID

@Return	Ordering mode for given NI
		0 - Concurrent
		1 - Exclusive
*//***************************************************************************/
int dpni_drv_get_ordering_mode(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_get_ordering_mode_etype

@Description	Returns the configuration in epid table for ordering mode
		for a certain protocol defined by EtherType

@Param[in]	ni_id - Network Interface ID
@Param[in]	etype - The etherType registered
			with dpni_drv_enable_etype_fs()

@Cautions	This method should be called only after
		dpni_drv_enable_etype_fs() for the same etherType

@Return	Ordering mode for given NI
		0 - Concurrent
		1 - Exclusive
		-ENOENT - When the EtherType was not registered
*//***************************************************************************/
int dpni_drv_get_ordering_mode_etype(uint16_t ni_id, uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_set_order_scope

@Description	Function to set order scope source for the specified NI.

@Param[in]	ni_id   The Network Interface ID

@Param[in]	key_cfg   A structure for defining a full Key Generation
		profile (rule)
		To disable order scope refer to \ref DPNI_DRV_ORDER_SCOPE

@Cautions	This method should be called in boot mode only.


@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_set_order_scope(uint16_t ni_id, struct dpkg_profile_cfg *key_cfg);

/**************************************************************************//**
@Function	dpni_drv_enable_etype_fs

@Description	Function to add a Flow Steering entry for the specified NI
		for a certain protocol defined by EtherType.
		For IPv4 see \ref NET_ETH_ETYPE_IPV4
		For IPv6 see \ref NET_ETH_ETYPE_IPV6
		For ARP see \ref ARP_ETHERTYPE

@Param[in]	ni_id   The Network Interface ID
@Param[in]	etype   EtherType is a two-octet field in an Ethernet
			frame. It is used to indicate which protocol is
			encapsulated in the payload of the frame

@Cautions	The flow steering table must be enabled for the NI
		('DPNI_OPT_DIST_FS' option was set at DPNI creation).
		This method should be called in boot mode only.
		In case dpni_drv_set_order_scope() is called,
		dpni_drv_enable_etype_fs() must be called after it.
		Maximum number of Flow Steering entries is
		min(\ref DPNI_FS_TABLE_SIZE,
		    \ref fs_entries set at DPNI creation)

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_enable_etype_fs(uint16_t ni_id, uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_get_connected_ni

@Description	Function to receive the connected AIOP NI ID for OBJ ID and type.

@Param[in]	id  object ID to find connection.

@Param[in]	type  The type of the give object ID ("dpni", dpmac").

@Param[out]	aiop_niid  Connected NI ID.

@Param[out]	state  link state on success: 1 - link is up, 0 - link is down;

@Return 0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_connected_ni(const int id, const char type[16], uint16_t *aiop_niid, int *state);

/**************************************************************************//**
@Function	dpni_drv_get_connected_obj

@Description	Function to receive the connected OBJ ID and type.

@Param[in]	aiop_niid  The AIOP Network Interface ID

@Param[out]	id  Connected object ID to the given NI ID

@Param[out]	type  The type of the connected object for given NI.

@Param[out]	state  link state on success: 1 - link is up, 0 - link is down;

@Return 0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_connected_obj(const uint16_t aiop_niid, int *id, char type[16], int *state);
/**************************************************************************//**
@Function	dpni_drv_set_rx_buffer_layout

@Description	Function to change SPs attributes (specify how many headroom)

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	layout  Structure representing DPNI buffer layout

@warning	Allowed only when DPNI is disabled

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_buffer_layout(uint16_t ni_id, const struct dpni_drv_buf_layout *layout);

/**************************************************************************//**
@Function	dpni_drv_get_rx_buffer_layout

@Description	Function to receive SPs attributes for RX buffer.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[out]	layout  Structure representing DPNI buffer layout

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_rx_buffer_layout(uint16_t ni_id, struct dpni_drv_buf_layout *layout);

/**************************************************************************//**
@Function	dpni_drv_register_rx_buffer_layout_requirements

@Description	register a request for DPNI requirement.

@Param[in]	head_room           Requested head room.
@Param[in]	tail_room           Requested tail room.
@Param[in]	private_data_size   Requested private data size.
@Param[in]	frame_anno          Requested frame annotation.
		OR-ed combination of dpni_drv_request_frame_annotation
		enumeration values. Hardware annotations are returned in the ASA
		presentation area as it follows :
			- Status Word at 0x00, 8 bytes,
			- Time Stamp at 0x08, 8 bytes,
			- Parser Result at 0x10, 48 bytes.
@Return		0        - on success,
		-ENAVAIL - resource not available or not found,
		-ENOMEM  - not enough memory.
 *//***************************************************************************/
int dpni_drv_register_rx_buffer_layout_requirements(uint16_t head_room,
						    uint16_t tail_room,
						    uint16_t private_data_size,
						    uint32_t frame_anno);

/**************************************************************************//**
@Function	dpni_drv_get_counter

@Description	Function to receive DPNI counter.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	counter Type of DPNI counter.

@Param[out]	value   Counter value for the requested type.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_counter(uint16_t ni_id, enum dpni_drv_counter counter,
			 uint64_t *value);

/**************************************************************************//**
@Function	dpni_drv_get_counter

@Description	Function to receive DPNI counter.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	tc      Traffic class.

@Param[in]	counter Type of DPNI QoS counter.

@Param[out]	value   Counter value for the requested type.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_qos_counter(uint16_t ni_id, uint8_t tc,
			     enum dpni_drv_qos_counter counter,
			     uint64_t *value);

/**************************************************************************//**
@Function	dpni_drv_get_statistics

@Description	Function to get DPNI statistics.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	page    Selects the statistics page to retrieve (0-3)

@Param[in]	param   Custom parameter for some pages used to select
                        a certain statistic source, for example the TC

@Param[in]	stat    Structure containing the statistics

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_statistics(uint16_t ni_id, uint8_t page, uint8_t param,
				dpni_drv_statistics *stat);

/**************************************************************************//**
@Function	dpni_drv_reset_statistics

@Description	Function to clear DPNI statistics.

@Param[in]	ni_id   The AIOP Network Interface ID

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_reset_statistics(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_get_dpni_id

@Description	Function to receive DPNI ID, known outside to AIOP.

@Param[in]	ni_id   The AIOP Network Interface ID.

@Param[out]	dpni_id DPNI ID known outside to AIOP.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_dpni_id(uint16_t ni_id, uint16_t *dpni_id);

/**************************************************************************//**
@Function	dpni_drv_get_ni_id

@Description	Function to receive AIOP internal NI ID.

@Param[in]	dpni_id DPNI ID known outside to AIOP.

@Param[out]	ni_id The AIOP Network Interface ID.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_ni_id(uint16_t dpni_id, uint16_t *ni_id);

/**************************************************************************//**
@Function	dpni_drv_get_link_state

@Description	Function to receive DPNI link state for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[out]	state Returned link state.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_link_state(uint16_t ni_id, struct dpni_drv_link_state *state);

/**************************************************************************//**
@Function	dpni_drv_set_link_cfg

@Description	Configures DPNI link for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	Link configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_link_cfg(uint16_t ni_id, struct dpni_drv_link_cfg *cfg);

/**************************************************************************//**
@Function	dpni_drv_clear_mac_filters

@Description	Function to clear DPNI unicast or multicast addresses.
		The primary MAC address is not cleared by this operation.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	unicast Set to '1' to clear unicast addresses.

@Param[in]	multicast Set to '1' to clear multicast addresses.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_clear_mac_filters(uint16_t ni_id, uint8_t unicast, uint8_t multicast);

/**************************************************************************//**
@Function	dpni_drv_clear_vlan_filters

@Description	Function to clear VLAN filters for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_clear_vlan_filters(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_vlan_filters

@Description	Function to set VLAN filters for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	en Set to '1' to enable; '0' to disable.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_vlan_filters(uint16_t ni_id, int en);

/**************************************************************************//**
@Function	dpni_drv_add_vlan_id

@Description	Function to add VLAN filters for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	vlan_id VLAN ID to add to given NI.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_add_vlan_id(uint16_t ni_id, uint16_t vlan_id);

/**************************************************************************//**
@Function	dpni_drv_remove_vlan_id

@Description	Function to remove VLAN filters in given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	vlan_id VLAN ID to remove in given NI.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_remove_vlan_id(uint16_t ni_id, uint16_t vlan_id);

/**************************************************************************//**
@Function	dpni_drv_get_initial_presentation

@Description	Function to get initial presentation settings from EPID table
		for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[out]	init_presentation Get initial presentation parameters
 	 	 \ref EP_INIT_PRESENTATION

@Cautions	PTA Presentation Address, ASA Presentation Address,
		ASA Presentation Offset, ASA Presentation Size:
		Those fields are not exposed in PRC on rev 2
		(Not copied from EPID), therefore it is not recommended to
		use them in order to present PTA, ASA.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_initial_presentation(
	uint16_t ni_id,
	struct ep_init_presentation* const init_presentation);

/**************************************************************************//**
@Function	dpni_drv_set_initial_presentation

@Description	Function to set initial presentation settings in EPID table for
		given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	init_presentation Set initial presentation parameters for given
		options and parameters \ref EP_INIT_PRESENTATION

@Cautions	1) PTA Presentation Address, ASA Presentation Address,
		   ASA Presentation Offset, ASA Presentation Size:
		   Those fields are not exposed in PRC on rev 2
		   (Not copied from EPID), therefore it is not recommended to
		   use them in order to present PTA, ASA.
		2) Data Segment, PTA Segment, ASA Segment must not reside
		   outside the bounds of the
		   presentation area. i.e. They must not fall within the HWC,
		   TLS or Stack areas.
		3) There should not be any overlap among the Segment, PTA & ASA.
		4) Minimum presented segment size must be configured.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_initial_presentation(
	uint16_t ni_id,
	const struct ep_init_presentation* const init_presentation);

/**************************************************************************//**
@Function	dpni_drv_get_initial_presentation_etype

@Description	Function to get initial presentation settings from EPID table
		for given NI for a certain protocol defined by EtherType

@Param[in]	ni_id   The AIOP Network Interface ID.
@Param[in]	etype   The etherType registered
			with dpni_drv_enable_etype_fs()

@Param[out]	init_presentation Get initial presentation parameters
		 \ref EP_INIT_PRESENTATION

@Cautions	PTA Presentation Address, ASA Presentation Address,
		ASA Presentation Offset, ASA Presentation Size:
		Those fields are not exposed in PRC on rev 2
		(Not copied from EPID), therefore it is not recommended to
		use them in order to present PTA, ASA.
		This method should be called only after
		dpni_drv_enable_etype_fs() for the same etherType

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_initial_presentation_etype(
	uint16_t ni_id, struct ep_init_presentation * const init_presentation,
	uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_set_initial_presentation_etype

@Description	Function to set initial presentation settings in EPID table for
		given NI for a certain protocol defined by EtherType

@Param[in]	ni_id   The AIOP Network Interface ID.
@Param[in]	etype   The etherType registered
			with dpni_drv_enable_etype_fs()

@Param[in]	init_presentation Set initial presentation parameters for given
		options and parameters \ref EP_INIT_PRESENTATION

@Cautions	1) PTA Presentation Address, ASA Presentation Address,
		   ASA Presentation Offset, ASA Presentation Size:
		   Those fields are not exposed in PRC on rev 2
		   (Not copied from EPID), therefore it is not recommended to
		   use them in order to present PTA, ASA.
		2) Data Segment, PTA Segment, ASA Segment must not reside
		   outside the bounds of the
		   presentation area. i.e. They must not fall within the HWC,
		   TLS or Stack areas.
		3) There should not be any overlap among the Segment, PTA & ASA.
		4) Minimum presented segment size must be configured.
		This method should be called only after
		dpni_drv_enable_etype_fs() for the same etherType

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_initial_presentation_etype(
	uint16_t ni_id,
	const struct ep_init_presentation * const init_presentation,
	uint16_t etype);

/**************************************************************************//**
@Function	dpni_drv_set_tx_checksum

@Description	Function to enable/disable l3/l4 check-sum for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	tx_checksum representing checksums to be enabled/disabled.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_tx_checksum(uint16_t ni_id,
                             const struct dpni_drv_tx_checksum * const tx_checksum);

/**************************************************************************//**
@Function	dpni_drv_get_tx_checksum

@Description	Function to get status of l3/l4 check-sum for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[out]	tx_checksum return the status for l3/l4 checksums.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_tx_checksum(uint16_t ni_id,
                            struct dpni_drv_tx_checksum * const tx_checksum);

/**************************************************************************//**
@Function	dpni_drv_set_rx_tc_policing

@Description	Function to set RX TC policing for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	tc_id Traffic class selection (0-7)

@Param[in]	cfg Traffic class policing configuration

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_tc_policing(uint16_t ni_id, uint8_t tc_id,
			    const struct dpni_drv_rx_tc_policing_cfg *cfg);

/**************************************************************************//**
@Function	dpni_drv_get_rx_tc_policing

@Description	Function to get RX TC policing for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	tc_id Traffic class selection (0-7)

@Param[in]	cfg Traffic class policing configuration

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_rx_tc_policing(uint16_t ni_id, uint8_t tc_id,
			    struct dpni_drv_rx_tc_policing_cfg * const cfg);

/**************************************************************************//**
@Function	dpni_drv_set_tx_selection

@Description	Sets the transmission priorities and the weighted scheduling
		mode for each traffic class of a given NI. All TCs are in the
		same priority group. Bandwidth (TX opportunities) that is made
		available to a priority group is fair shared among the TCs of
		that group in proportion to a configured "weight" value.
		The bandwidth weight of a TC it's a value between 100 and 24800.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	cfg Transmission selection configurations.

@Cautions	Allowed only when DPNI is disabled.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_tx_selection(uint16_t ni_id,
                          const struct dpni_drv_tx_selection *cfg);

/**************************************************************************//**
@Function	dpni_drv_set_tx_shaping

@Description	Set the transmit committed rate and excess rate shapers for
		a given NI. The shapers are token bucket based. Each shaper has
		an individually configured rate limit and maximum burst size.
		The rate is expressed in Mbps. The burst size is the maximum
		amount of data (in bytes) sent as a consecutive burst of back
		to back frames on the network. Burst size may be up to
		63487 bytes.

@Param[in]	ni_id : The AIOP Network Interface ID.

@Param[in]	cr_cfg : TX committed rate shaper configuration.

@Param[in]	er_cfg : TX excess rate shaper configuration.

@Param[in]	coupled : Credits to the CR shaper in excess of its token bucket
		limit is credited to the ER bucket.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_tx_shaping(uint16_t ni_id,
			    struct dpni_drv_tx_shaping *cr_cfg,
			    struct dpni_drv_tx_shaping *er_cfg,
			    uint8_t coupled);

/**************************************************************************//**
@Function	dpni_drv_set_qos_table

@Description	Function to set QoS mapping table for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	cfg QoS table configuration.

@Cautions	This function and all QoS-related functions require that
		'max_tcs > 1' was set at DPNI creation.
		Before calling this function, call dpni_drv_prepare_key_cfg() to
		prepare the key_cfg_iova parameter

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_qos_table(uint16_t ni_id,
                          const struct dpni_drv_qos_tbl *cfg);

/**************************************************************************//**
@Function	dpni_drv_add_qos_entry

@Description	Function to add QoS mapping entry for given NI and TC.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	cfg Rule configuration for table lookup.

@Param[in]	tc_id Traffic class selection (0-7)

@Param[in]	index Location in the QoS table where to insert the entry.
	Only relevant if MASKING is enabled for QoS classification on
	this DPNI, it is ignored for exact match. 
	Not supported on LS1088A.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_add_qos_entry(uint16_t ni_id,
                           const struct dpni_drv_qos_rule *cfg,
                           uint8_t tc_id,
                           uint16_t index);

/**************************************************************************//**
@Function	dpni_drv_remove_qos_entry

@Description	Function to remove QoS mapping entry for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	cfg Rule configuration for table lookup.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_remove_qos_entry(uint16_t ni_id,
                              const struct dpni_drv_qos_rule *cfg);

/**************************************************************************//**
@Function	dpni_drv_clear_qos_table

@Description	Function to clear all QoS mapping entries for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Cautions	Following this function call, all frames are directed to
		the default traffic class (0)

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_clear_qos_table(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_prepare_rx_tc_early_drop

@Description	Function to prepare an early drop.

@Param[in]	cfg Early-drop configuration.

@Param[out]	early_drop_buf Zeroed 256 bytes of memory before mapping it to
		DMA.

@Cautions	This function has to be called before
		dpni_drv_set_rx_tc_early_drop

*//***************************************************************************/
void dpni_drv_prepare_rx_tc_early_drop(
	const struct dpni_drv_early_drop_cfg *cfg,
	uint8_t *early_drop_buf);

/**************************************************************************//**
@Function	dpni_drv_set_rx_tc_early_drop

@Description	Function to set Rx traffic class early-drop configuration for
		given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	tc_id Traffic class selection (0-7)

@Param[in]	early_drop_iova I/O virtual address of 64 bytes;

@Cautions	Before calling this function, call
		dpni_drv_prepare_rx_tc_early_drop() to prepare the
		early_drop_iova parameter.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_tc_early_drop(uint16_t ni_id,
                                  uint8_t tc_id,
                                  uint64_t early_drop_iova);

/**************************************************************************//**
@Function	task_set_tx_tc

@Description	Set task TX traffic class.

@Param[in]	tc Traffic class.
*//***************************************************************************/
inline void task_set_tx_tc(uint8_t tc);

/**************************************************************************//**
@Function	task_get_tx_tc

@Description	Get task TX traffic class.

@Return	TX traffic class.
*//***************************************************************************/
inline uint8_t task_get_tx_tc(void);

/**************************************************************************//**
@Function	task_switch_to_egress_parse_profile

@Description	Switch to the "egress" parse profile.

@Param[in]	start_hxs Starting soft/hard parsing HXS.

@Return		None.
*//***************************************************************************/
inline void task_switch_to_egress_parse_profile(uint16_t start_hxs);

/**************************************************************************//**
@Function	dpni_drv_prepare_key_cfg

@Description	Function to prepare extract parameters.

@Param[in]	cfg defining a full Key Generation profile (rule)

@Param[out]	key_cfg_buf Zeroed 256 bytes of memory before mapping it to DMA

@Cautions	This function has to be called before the following functions:
		- dpni_drv_set_qos_table()
@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_prepare_key_cfg(struct dpkg_profile_cfg *cfg,
                             uint8_t *key_cfg_buf);

/**************************************************************************//**
@Function	dpni_drv_get_num_free_bufs

@Description	Get a snapshot of the current fill level (number of free
		buffers) of the DPNI configured buffer pools.

@Param[in]	flags :  OR-ed flags selecting the pool :
		DPNI_DRV_PEB_FREE_BUFS, DPNI_DRV_BACKUP_FREE_BUFS.

@Param[out]	free_bufs : Structure containing the current fill level.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_num_free_bufs(uint32_t flags,
			       struct dpni_drv_free_bufs *free_bufs);

/**************************************************************************//**
@Function	dpni_drv_set_errors_behavior

@Description	Set errors behavior.

@Param[in]	ni_id : The AIOP Network Interface ID.

@Param[in]	cfg :  Errors configuration.

@Return		0 on success;
		error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_errors_behavior(uint16_t ni_id,
				 const struct dpni_drv_error_cfg *cfg);

/**************************************************************************//**
@Function	dpni_drv_enable_ingress_soft_parser

@Description	Enable an AIOP "ingress" Soft Parser. The SP parameters (if
		exist) are stored in the Soft Examination Parameter Array of
		the AIOP "ingress" Parse Profile. All AIOP belonging DPNIs share
		the same "ingress" Parse Profile.
		The driver checks if the soft parse to be enabled is loaded and
		if its parameters (size and offset) coincides with the loading
		time declared parameters.

@Param[in]	param : Soft Parser activation parameters.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_enable_ingress_soft_parser
			(const struct dpni_drv_sparser_param *param);

/**************************************************************************//**
@Function	dpni_drv_enable_egress_soft_parser

@Description	Enable an AIOP "egress" Soft Parser. The SP parameters (if
		exist) are stored in the Soft Examination Parameter Array of
		the AIOP "egress" Parse Profile. All AIOP belonging DPNIs share
		the same "egress" Parse Profile.
		The driver checks if the soft parse to be enabled is loaded and
		if its parameters (size and offset) coincides with the loading
		time declared parameters.

@Param[in]	param : Soft Parser activation parameters.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_enable_egress_soft_parser
			(const struct dpni_drv_sparser_param *param);

/**************************************************************************//**
@Function	dpni_drv_load_wriop_ingress_soft_parser

@Description	Load a Soft Parser in the ingress WRIOP Parser instructions
		memory.

@Param[in]	param : Soft Parser loading parameters.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_load_wriop_ingress_soft_parser
			(const struct dpni_drv_sparser_param *param);

/**************************************************************************//**
@Function	dpni_drv_load_wriop_egress_soft_parser

@Description	Load a Soft Parser in the egress WRIOP Parser instructions
		memory.

@Param[in]	param : Soft Parser loading parameters.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_load_wriop_egress_soft_parser
			(const struct dpni_drv_sparser_param *param);

/**************************************************************************//**
@Function	dpni_drv_enable_wriop_ingress_soft_parser

@Description	Enable a Soft Parser onto the ingress path of the WRIOP parser
		for all AIOP belonging DPNIs.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	param : Soft Parser enable parameters.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_enable_wriop_ingress_soft_parser
		(uint16_t ni_id, const struct dpni_drv_sparser_param *param);

/**************************************************************************//**
@Function	dpni_drv_enable_wriop_egress_soft_parser

@Description	Enable a Soft Parser onto the egress path of the WRIOP parser
		for all AIOP belonging DPNIs.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	param : Soft Parser enable parameters.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_enable_wriop_egress_soft_parser
		(uint16_t ni_id, const struct dpni_drv_sparser_param *param);

/**************************************************************************//**
@Function	dpni_drv_set_tx_taildrop

@Description	Set Tx traffic class taildrop configuration for a given DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[in]	cfg : Structure representing taildrop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_tx_taildrop(uint16_t ni_id, uint8_t tc,
			     struct dpni_drv_taildrop *cfg);

/**************************************************************************//**
@Function	dpni_drv_get_tx_taildrop

@Description	Retrieve Tx traffic class taildrop configuration of a given
		DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[out]	cfg : Structure receiving the taildrop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_tx_taildrop(uint16_t ni_id, uint8_t tc,
			     struct dpni_drv_taildrop *cfg);

/**************************************************************************//**
@Function	dpni_drv_set_tx_early_drop

@Description	Set Tx traffic class early drop configuration for a given DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[in]	cfg : Structure representing early drop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_tx_early_drop(uint16_t ni_id, uint8_t tc,
			       struct dpni_drv_early_drop *cfg);

/**************************************************************************//**
@Function	dpni_drv_get_tx_early_drop

@Description	Retrieve Tx traffic class early drop configuration of a given
		DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[out]	cfg : Structure receiving the early drop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_tx_early_drop(uint16_t ni_id, uint8_t tc,
			       struct dpni_drv_early_drop *cfg);

/**************************************************************************//**
@Function	dpni_drv_set_rx_taildrop

@Description	Set Rx traffic class taildrop configuration for a given DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[in]	cfg : Structure representing taildrop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_taildrop(uint16_t ni_id, uint8_t tc,
			     struct dpni_drv_taildrop *cfg);

/**************************************************************************//**
@Function	dpni_drv_get_rx_taildrop

@Description	Retrieve Rx traffic class taildrop configuration of a given
		DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[out]	cfg : Structure receiving the taildrop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_rx_taildrop(uint16_t ni_id, uint8_t tc,
			     struct dpni_drv_taildrop *cfg);

/**************************************************************************//**
@Function	dpni_drv_set_rx_early_drop

@Description	Set Rx traffic class early drop configuration for a given DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[in]	cfg : Structure representing early drop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_early_drop(uint16_t ni_id, uint8_t tc,
			       struct dpni_drv_early_drop *cfg);

/**************************************************************************//**
@Function	dpni_drv_get_rx_early_drop

@Description	Retrieve Rx traffic class early drop configuration of a given
		DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[out]	cfg : Structure receiving the early drop configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_rx_early_drop(uint16_t ni_id, uint8_t tc,
			       struct dpni_drv_early_drop *cfg);

/**************************************************************************//**
@Function	dpni_drv_set_pools

@Description	Set buffer pools configuration of a given DPNI.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	cfg : Buffer pools configuration.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_pools(uint16_t ni_id, dpni_drv_pools_cfg *cfg);

/**************************************************************************//**
@Function	dpni_drv_set_congestion_notification

@Description	Set traffic class congestion notification configuration.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[in]	qtype : Type of queue - Rx, Tx are supported

@Param[in]	cfg : Congestion notification configuration

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_congestion_notification(uint16_t ni_id, uint8_t tc,
			dpni_drv_queue_type qtype,
			struct dpni_drv_congestion_notification_cfg *cfg);

/**************************************************************************//**
@Function	dpni_drv_get_congestion_notification

@Description	Get traffic class congestion notification configuration.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Param[in]	qtype : Type of queue - Rx, Tx are supported

@Param[out]	cfg : Congestion notification configuration

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_congestion_notification(uint16_t ni_id, uint8_t tc,
			dpni_drv_queue_type qtype,
			struct dpni_drv_congestion_notification_cfg *cfg);


/**************************************************************************//**
@Function	dpni_drv_set_rx_priorities

@Description	Sets the Rx priorities for each traffic class of a given NI.
		Lower index Rx TCs always take precedence over higher index TCs.
		4 strict priority levels: 0, 1, [2-5], [6-7].
		A DPCON must be associated with AIOP for the
		purpose of scheduling.

@Param[in]	ni_id The AIOP Network Interface ID.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_priorities(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_get_attributes

@Description	Retrieve the attributes of a given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[out]	attr Object's attributes.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_attributes(uint16_t ni_id, dpni_drv_attr *attr);

/**************************************************************************//**
@Function	dpni_drv_set_tx_confirmation_mode

@Description	Set the tx confirmation mode: disabled, single(one confirmation
		queue per dpni), affine(one confirmation queue for each tx
		queue), for all frames or error frames only.

@Param[in]	ni_id : The AIOP Network Interface ID.
@Param[in]	mode : The mode to be set. For the available modes,
		refer to \ref dpni_drv_confirmation_mode

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_tx_confirmation_mode(uint16_t ni_id,
				      enum dpni_drv_confirmation_mode mode);

#ifdef SL_DEBUG
/**************************************************************************//**
@Function	dpni_drv_dump_taildrop

@Description	Dumps taildrop configuration on specified DPNI and traffic
		class.
		Note : If application calls this function, application and
		AIOP_SL must be build with the SL_DEBUG macro defined.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Return		None

*//***************************************************************************/
void dpni_drv_dump_tx_taildrop(uint16_t ni_id, uint8_t tc_id);

/**************************************************************************//**
@Function	dpni_drv_dump_early_drop

@Description	Dumps early drop configuration on specified DPNI and traffic
		class.
		Note : If application calls this function, application and
		AIOP_SL must be build with the SL_DEBUG macro defined.

@Param[in]	ni_id : The Network Interface ID

@Param[in]	tc : Traffic class

@Return		None

*//***************************************************************************/
void dpni_drv_dump_tx_early_drop(uint16_t ni_id, uint8_t tc_id);

#endif	/* SL_DEBUG */

/** @} */ /* end of dpni_drv_g DPNI DRV group */
#endif /* __FSL_DPNI_DRV_H */
