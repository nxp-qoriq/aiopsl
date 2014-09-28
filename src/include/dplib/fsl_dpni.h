/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
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
/*!
 *  @file    fsl_dpni.h
 *  @brief   Data Path Network Interface API
 *
 */
#ifndef __FSL_DPNI_H
#define __FSL_DPNI_H

#include <fsl_dpkg.h>

/*!
 * @Group grp_dpni	Data Path Network Interface API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPNI
 *
 * @{
 */

struct fsl_mc_io;

/*!
 * @name General DPNI macros
 */
#define DPNI_MAX_TC				8
/*!< Maximum number of traffic classes */
#define DPNI_MAX_DPBP				8
/*!< Maximum number of bm-pools */

#define DPNI_ALL_TCS				(uint8_t)(-1)
/*!< All traffic classes considered */
#define DPNI_ALL_TC_FLOWS			(uint16_t)(-1)
/*!< All flows within traffic classes considered */
#define DPNI_NEW_FLOW_ID			(uint16_t)(-1)
/*!< Generate new flow id */
#define DPNI_FQID_NOT_VALID			(-1)
/*!< Invalid FQID  */
/* @} */

/**
 *
 * @brief	Open object handle
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpni_id		DPNI unique ID
 * @param[out]	token		Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_open(struct fsl_mc_io *mc_io, int dpni_id, uint16_t *token);

/**
 *
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_close(struct fsl_mc_io *mc_io, uint16_t token);

/*!
 * @name DPNI configuration options
 *
 */
#define DPNI_OPT_ALLOW_DIST_KEY_PER_TC		0x00000001
/*!< Allow different distribution-key per TC */
#define DPNI_OPT_TX_CONF_DISABLED		0x00000002
/*!< No Tx-confirmation at all */
#define DPNI_OPT_PRIVATE_TX_CONF_ERROR_DISABLED	0x00000004
/*!< Private Tx-confirmation/error disable */
#define DPNI_OPT_DIST_HASH			0x00000010
/*!< Hash based distribution support */
#define DPNI_OPT_DIST_FS			0x00000020
/*!< Flow-steering based distribution support */
#define DPNI_OPT_UNICAST_FILTER			0x00000080
/*!< Unicast filtering support */
#define DPNI_OPT_MULTICAST_FILTER		0x00000100
/*!< Multicast filtering support */
#define DPNI_OPT_VLAN_FILTER			0x00000200
/*!< VLAN filtering support */
#define DPNI_OPT_IPR				0x00000800
/*!< IP-reassembly support */
#define DPNI_OPT_IPF				0x00001000
/*!< IP-fragmentation support */
#define DPNI_OPT_VLAN_MANIPULATION		0x00010000
/*!< VLAN manipulation support */
#define DPNI_OPT_QOS_MASK_SUPPORT		0x00020000
/*!< QoS mask support */
#define DPNI_OPT_FS_MASK_SUPPORT		0x00040000
/*!< Flow-steering mask support */
/* @} */

/**
 * @brief	Structure representing IPR configuration
 */
struct dpni_ipr_cfg {
	uint16_t max_reass_frm_size;
	uint16_t min_frag_size_ipv4;
	uint16_t min_frag_size_ipv6;
	uint16_t max_open_frames_ipv4;
	uint16_t max_open_frames_ipv6;
};

/**
 * @brief	Structure representing DPNI configuration
 */
struct dpni_cfg {
	uint8_t mac_addr[6]; /*!< Primary mac address */
	struct {
		uint64_t options;
		/*!< Mask of available options; use 'DPNI_OPT_XXX' */
		enum net_prot start_hdr; /* If either 'NET_PROT_ETH' or
		 'NET_PROT_NONE' than will be treated as NIC otherwise as NI */
		uint8_t max_senders;
		/*!< maximum number of different senders; will be used as the
		 * number of dedicated tx flows; In case it isn't power-of-2 it
		 * will be ceiling to the next power-of-2 as HW demand it;
		 * '0' will be treated as '1' */
		uint8_t max_tcs;
		/*!< maximum number of traffic-classes;
		 will affect both Tx & Rx; '0' will e treated as '1' */
		uint8_t max_dist_per_tc[DPNI_MAX_TC];
		/*!< maximum distribution's size per Rx traffic-class;
		 * set it to the required value minus 1;
		 * i.e. 0->1, 1->2, ... ,255->256;
		 * In case it isn't power-of-2 it will
		 * be rounded up to the next power-of-2 as HW demand it */
		uint8_t max_unicast_filters;
		/*!< maximum number of unicast filters; '0' will be treated
		 as '16' */
		uint8_t max_multicast_filters;
		/*!< maximum number of multicast filters; '0' will be treated
		 as '64' */
		uint8_t max_vlan_filters;
		/*!< maximum number of vlan filters; '0' will be treated
		 as '16' */
		uint8_t max_qos_entries;
		/*!< if 'max_tcs>1', declare the maximum entries for the
		 QoS table; '0' will be treated as '64' */
		uint8_t max_qos_key_size;
		/*!< maximum key size for the QoS look-up; '0' will be treated
		 * as '24' which enough for IPv4 5-tuple */
		uint8_t max_dist_key_size;
		/*!< maximum key size for the distribution; '0' will be treated as
		 '24' which enough for IPv4 5-tuple */
		struct dpni_ipr_cfg ipr_cfg; /*!< IP reassembly configuration */
	} adv; /*!< use this structure to change default settings */
};

/**
 * @brief	Open object handle, allocate resources and preliminary initialization -
 *		required before any operation on the object
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]	cfg	Configuration structure
 * @param[out]  token	Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpni_create(struct fsl_mc_io *mc_io,
	const struct dpni_cfg *cfg,
	uint16_t *token);

/**
 *
 * @brief	Free the DPNI object and all its resources.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpni_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/*!
 * @name DPNI IRQ Index and Events
 */
#define DPNI_IRQ_INDEX				0
/*!< Irq index */
#define DPNI_IRQ_EVENT_LINK_CHANGED		0x00000001
/*!< irq event - Indicates that the link state changed */
/* @} */

/**
 * @brief	Sets IRQ information for the DPNI to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPNI.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_paddr and irq_val are valid);
 * @param[out]	irq_paddr	Physical address that must be written in order
 *				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 *				message-based interrupt
 * @param[out]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id);

/**
 * @brief	Sets overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_enable(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_enable(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq_mask(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_irq_mask(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpni_get_irq_status(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpni_clear_irq_status(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t irq_index,
	uint32_t status);

/**
 * @brief	Structure representing DPNI pools parameters
 */
struct dpni_pools_cfg {
	uint8_t num_dpbp; /*!< number of DPBPs */
	struct {
		uint16_t dpbp_id; /*!< DPBPs object id */
		uint16_t buffer_size; /*!< buffer size */
	} pools[DPNI_MAX_DPBP];
};

/**
 *
 * @brief	Set the pools
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	cfg	pools configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 *
 */
int dpni_set_pools(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_pools_cfg *cfg);

/**
 * @brief   DPNI destination types
 */
enum dpni_dest {
	DPNI_DEST_NONE,
	/*!< unassigned destination; i.e. queues will be set in parked mode;
	 * user should explict dequeue from this FQ */
	DPNI_DEST_DPIO,
	/*!< queues will generate notification to the dpio's channel;
	 i.e. will be set in schedule mode and FQDAN enable */
	DPNI_DEST_DPCON
/*!< queues won't generate notification, but will be connected to this
 channel object; i.e. will be set in schedule mode and FQDAN disable;
 user should explict dequeue from this channel */
};

/**
 * @brief	Structure representing DPNI destination parameters
 */
struct dpni_dest_cfg {
	enum dpni_dest dest_type; /*!< destination type */
	int dest_id;
	/*!< either DPIO id or DPCON id depends on the channel type */
	uint8_t priority;
/*!< 0-1 or 0-7 (depends on the channel type) to select the priority(work-queue)
 within the channel (not relevant for the 'NONE' case) */
};

/**
 *
 * @brief	Enable the NI, will allow sending and receiving frames.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 *
 * @brief	Disable the NI, will disallow sending and receiving frames.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Is DPNI enabled
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]  en		'1' for object enabled/'0' otherwise
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_is_enabled(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 *
 * @brief	Reset the NI, will return to initial state.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_reset(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing attributes parameters
 */
struct dpni_attr {
	int id; /*!< DPNI ID */
	struct {
		uint16_t major; /*!< DPNI major version*/
		uint16_t minor; /*!< DPNI minor version*/
	} version; /*!< DPNI version */
	enum net_prot start_hdr;
	/*!< 'NET_PROT_ETH' for NIC else for NI */
	uint64_t options;
	/*!< Mask of available options; reflect the value as was given in the
	 initialization phase */
	uint8_t max_senders;
	/*!< maximum number of different senders; will be used as the
	 * number of dedicated tx flows; */
	uint8_t max_tcs;
	/*!< maximum number of traffic-classes;
	 will affect both Tx & Rx; */
	uint8_t max_dist_per_tc[DPNI_MAX_TC];
	/*!< maximum distribution's size per Rx traffic-class;
	 * add 1 to get the real value;
	 * i.e. 0->1, 1->2, ... ,255->256; */
	uint8_t max_unicast_filters;
	/*!< maximum number of unicast filters */
	uint8_t max_multicast_filters;
	/*!< maximum number of multicast filters */
	uint8_t max_vlan_filters;
	/*!< maximum number of vlan filters */
	uint8_t max_qos_entries;
	/*!< if 'max_tcs>1', declare the maximum entries for the  QoS table */
	uint8_t max_qos_key_size;
	/*!< maximum key size for the QoS look-up */
	uint8_t max_dist_key_size;
	/*!< maximum key size for the distribution look-up */
	struct dpni_ipr_cfg ipr_cfg; /*!< IP reassembly configuration */
};

/**
 *
 * @brief	Retrieve the object's attributes.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	attr	Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_attributes(struct fsl_mc_io *mc_io,
	uint16_t token,
	struct dpni_attr *attr);

/*!
 * @name DPNI errors
 *
 */
#define DPNI_ERROR_EOFHE	0x00020000
/*!< Extract out of frame header error */
#define DPNI_ERROR_FLE		0x00002000
/*!< Frame length error */
#define DPNI_ERROR_FPE		0x00001000
/*!< Frame physical error */
#define DPNI_ERROR_PHE		0x00000020
/*!< Parsing header error */
#define DPNI_ERROR_L3CE		0x00000004
/*!< Parser L3 checksum error */
#define DPNI_ERROR_L4CE		0x00000001
/*!< Parser L3 checksum error */

/*!
 * @brief   DPNI defining behaviour for errors
 *
 */
enum dpni_error_action {
	DPNI_ERROR_ACTION_DISCARD, /*!< Discard the frame */
	DPNI_ERROR_ACTION_CONTINUE, /*!< Continue with the flow */
	DPNI_ERROR_ACTION_SEND_TO_ERROR_QUEUE
/*!< Enqueue to error queue */
};

/**
 * @brief	Structure representing DPNI errors treatment
 */
struct dpni_error_cfg {
	uint32_t errors;
	/*!< Errors mask; use 'DPNI_ERROR_xxx' */
	enum dpni_error_action error_action;
	/*!< The desired action for the errors mask */
	int set_frame_annotation;
/*!< relevant only for the non-discard action;
 * if '1' those errors will be set in the FAS */
};

/**
 *
 * @brief	Set errors behaviour
 *
 * Can be called numerous times with different error masks
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	cfg - errors configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_errors_behaviour(struct fsl_mc_io *mc_io,
	uint16_t token, struct dpni_error_cfg *cfg);

/*!
 * @name DPNI buffer layout modification options
 *
 */
#define DPNI_BUF_LAYOUT_OPT_TIMESTAMP		0x00000001
/*!< Modify the time-stamp setting */
#define DPNI_BUF_LAYOUT_OPT_PARSER_RESULT	0x00000002
/*!< Modify the parser-result setting; Not applicable in TX */
#define DPNI_BUF_LAYOUT_OPT_FRAME_STATUS	0x00000004
/*!< Modify the frame-status setting */
#define DPNI_BUF_LAYOUT_OPT_PRIVATE_DATA_SIZE	0x00000008
/*!< Modify the private-data-size setting */
#define DPNI_BUF_LAYOUT_OPT_DATA_ALIGN		0x00000010
/*!< Modify the data-alignment setting */
/* @} */

/**
 * @brief	Structure representing DPNI buffer layout
 */
struct dpni_buffer_layout {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for the buffer layout; use 'DPNI_BUF_LAYOUT_OPT_xxx' */
	int pass_timestamp;
	/*!< This option maybe used when 'options' set
	 with DPNI_BUF_LAYOUT_OPT_TIMESTAMP */
	int pass_parser_result;
	/*!< This option maybe used when 'options' set
	 with DPNI_BUF_LAYOUT_OPT_PARSER_RESULT */
	int pass_frame_status;
	/*!< This option maybe used when 'options' set
	 with DPNI_BUF_LAYOUT_OPT_FRAME_STATUS */
	uint16_t private_data_size;
	/*!< This option maybe used when 'options' set
	 with DPNI_BUF_LAYOUT_OPT_PRIVATE_DATA_SIZE */
	uint16_t data_align;
/*!< This option maybe used when 'options' set
 with DPNI_BUF_LAYOUT_OPT_DATA_ALIGN */
};

/**
 *
 * @brief	Retrieve the RX buffer layout settings.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	layout	buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_buffer_layout(struct fsl_mc_io *mc_io,
	uint16_t token,
	struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Set the RX buffer layout settings.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	layout	buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_rx_buffer_layout(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Retrieve the TX buffer layout settings.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	layout	buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_buffer_layout(struct fsl_mc_io *mc_io,
	uint16_t token,
	struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Set the TX buffer layout settings.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	layout	buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_buffer_layout(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Retrieve the TX-Conf buffer layout settings.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	layout	buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
	uint16_t token,
	struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Set the TX-Conf buffer layout settings.
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	layout	buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_buffer_layout *layout);

/**
 *
 * @brief	Enable/disable L3 checksum validation
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_l3_chksum_validation(struct fsl_mc_io *mc_io,
	uint16_t token,
	int en);

/**
 *
 * @brief	Get L3 checksum validation mode
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	en	enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_l3_chksum_validation(struct fsl_mc_io *mc_io,
	uint16_t token,
	int *en);

/**
 *
 * @brief	Enable/disable L4 checksum validation
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[in]	en	enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_l4_chksum_validation(struct fsl_mc_io *mc_io,
	uint16_t token,
	int en);

/**
 *
 * @brief	Get L4 checksum validation mode
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	en	enable/disable checksum validation
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_l4_chksum_validation(struct fsl_mc_io *mc_io,
	uint16_t token,
	int *en);

/**
 *
 * @brief	Get the QDID used for enqueue
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	qdid	Qdid used for qneueue
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_enable().
 */
int dpni_get_qdid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *qdid);

/**
 *
 * @brief	Get the AIOP's SPID that associate with this DPNI
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	spid	aiop's storage-profile
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Only relevant for DPNI that belongs to AIOP container.
 */
int dpni_get_spid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *spid);

/**
 *
 * @brief	Get the tx data offset
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	data_offset	TX data offset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_data_offset(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint16_t *data_offset);

/**
 * @brief	DPNI Counter types
 *
 */
enum dpni_counter {
	DPNI_CNT_ING_FRAME,
	/*!< Ingress frame count */
	DPNI_CNT_ING_BYTE,
	/*!< Ingress byte count */
	DPNI_CNT_ING_FRAME_DROP,
	/*!< Ingress frame dropped counter due to
	 explicit 'drop' setting */
	DPNI_CNT_ING_FRAME_DISCARD,
	/*!< Ingress frame discarded counter due to errors */
	DPNI_CNT_ING_MCAST_FRAME,
	/*!< Ingress multicast frame count */
	DPNI_CNT_ING_MCAST_BYTE,
	/*!< Ingress multicast byte count */
	DPNI_CNT_ING_BCAST_FRAME,
	/*!< Ingress broadcast frame count */
	DPNI_CNT_ING_BCAST_BYTES,
	/*!< Ingress broad bytes count */
	DPNI_CNT_EGR_FRAME,
	/*!< Egress frame count */
	DPNI_CNT_EGR_BYTE,
	/*!< Egress byte count */
	DPNI_CNT_EGR_FRAME_DISCARD
/*!< Egress frame discard counter due to errors */
};

/**
 *
 * @brief   Read one of the DPNI counters
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	counter - the requested counter
 * @param[out]	value - counter's current value
 *
 * @returns        '0' on Success; Error code otherwise.
 */
int dpni_get_counter(struct fsl_mc_io *mc_io,
	uint16_t token,
	enum dpni_counter counter,
	uint64_t *value);

/**
 *
 * @brief   Write to one of the DPNI counters
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	counter - the requested counter
 * @param[in]   value - New counter value.
 *		typically '0' for resetting the counter.
 *
 * @returns        '0' on Success; Error code otherwise.
 */
int dpni_set_counter(struct fsl_mc_io *mc_io,
	uint16_t token,
	enum dpni_counter counter,
	uint64_t value);

/**
 *
 * @brief	Return the link state, either up or down
 *
 * @param[in]	mc_io	Pointer to opaque I/O object
 * @param[in]   token	Token of DPNI object
 * @param[out]	up	return '0' for down, '1' for up
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_link_state(struct fsl_mc_io *mc_io, uint16_t token, int *up);

/**
 *
 * @brief	Set the maximum received frame length.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	max_frame_length - in Bytes, drop packet if length excedded
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_mfl(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint16_t max_frame_length);

/**
 *
 * @brief	Get the maximum received frame length.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	max_frame_length - Max frame length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_mfl(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint16_t *max_frame_length);

/**
 *
 * @brief	Set the MTU for this interface. Will have affect on IPF and
 *		conditionally on GSO.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	mtu - MTU length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t mtu);

/**
 *
 * @brief	Get the MTU.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	mtu - MTU length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *mtu);

/**
 *
 * @brief	Enable/Disable multicast promiscuous mode
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_multicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 *
 * @brief	Get multicast promiscuous mode
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_multicast_promisc(struct fsl_mc_io *mc_io,
	uint16_t token,
	int *en);

/**
 *
 * @brief	Enable/Disable unicast promiscuous mode
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 *
 * @brief	Get unicast promiscuous mode
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int *en);

/**
 *
 * @brief	Set the primary mac address
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	mac_addr	MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_primary_mac_addr(struct fsl_mc_io *mc_io,
	uint16_t token,
	const uint8_t mac_addr[6]);

/**
 *
 * @brief	Get the primary mac address
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[out]	mac_addr	MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_primary_mac_addr(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t mac_addr[6]);

/**
 *
 * @brief	Add unicast/multicast filter address
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	mac_addr	MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_mac_addr(struct fsl_mc_io *mc_io,
	uint16_t token,
	const uint8_t mac_addr[6]);

/**
 *
 * @brief	Remove unicast/multicast filter address
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	mac_addr	MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_mac_addr(struct fsl_mc_io *mc_io,
	uint16_t token,
	const uint8_t mac_addr[6]);

/**
 *
 * @brief	Clear the mac filters
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]   unicast		clear unicast table
 * @param[in]   multicast	clear multicast table
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_mac_filters(struct fsl_mc_io *mc_io,
	uint16_t token,
	int unicast,
	int multicast);

/**
 *
 * @brief	Enable/Disable VLAN filtering mode
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_vlan_filters(struct fsl_mc_io *mc_io, uint16_t token, int en);

/**
 *
 * @brief	Add VLAN-id filter
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	vlan_id - VLAN ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_vlan_id(struct fsl_mc_io *mc_io, uint16_t token, uint16_t vlan_id);

/**
 *
 * @brief	Remove VLAN-id filter
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	vlan_id - VLAN ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_vlan_id(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint16_t vlan_id);

/**
 *
 * @brief	Clear the VLAN filters
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_vlan_filters(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing DPNI TX TC parameters
 */
struct dpni_tx_tc_cfg {
	uint16_t depth_limit;
/*!<  if >0 than limit the depth of this queue which may result with
 * rejected frames */
};

/**
 *
 * @brief	Set TX TC settings
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - TC parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_tc(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t tc_id,
	const struct dpni_tx_tc_cfg *cfg);

/**
 * @brief	distribution mode
 */
enum dpni_dist_mode {
	DPNI_DIST_MODE_NONE,/*!< no distribution */
	DPNI_DIST_MODE_HASH, /*!< hash-distribution */
	DPNI_DIST_MODE_FS
/*!< flow-steering distribution */
};

/**
 * @brief   DPNI Flow-Steering miss action
 */
enum dpni_fs_miss_action {
	DPNI_FS_MISS_DROP,
	/*!< in case of no-match drop the frame  */
	DPNI_FS_MISS_EXPLICIT_FLOWID,
	/*!< in case of no-match go to explicit flow-id */
	DPNI_FS_MISS_HASH
/*!< in case of no-match do a hashing to select a flow */
};

/**
 * @brief	Structure representing FS table parameters
 */
struct dpni_fs_tbl_cfg {
	enum dpni_fs_miss_action miss_action; /*!< miss action mode */
	uint16_t default_flow_id;
/*!< will be used in case 'DPNI_FS_MISS_EXPLICIT_FLOWID' */
};

/**
 * @brief	Structure representing DPNI RX TC parameters
 */
struct dpni_rx_tc_dist_cfg {
	uint8_t dist_size;
	/*!< set the distribution size;
	 * set it to the required value minus 1;
	 * i.e. 0->1, 1->2, ... ,255->256;
	 * In case it isn't power-of-2 it will
	 * be rounded up to the next power-of-2 as HW demand it */
	enum dpni_dist_mode dist_mode; /*!< distribution mode */
	struct dpkg_profile_cfg *dist_key_cfg;
	/*!< define the extractions to be used for the distribution key */
	struct dpni_fs_tbl_cfg fs_cfg;
/*!< FS table configuration; only relevant for 'DPNI_DIST_MODE_FS'; */
};

/**
 *
 * @brief	Set RX TC distribution settings
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - TC parameters
 *
 * @returns	'0' on Success; error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 *
 */
int dpni_set_rx_tc_dist(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t tc_id,
	const struct dpni_rx_tc_dist_cfg *cfg);

/*!
 * @name DPNI Tx flow modification options
 *
 */
#define DPNI_TX_FLOW_MOD_OPT_TX_CONF_ERROR	0x00000001
/*!< Modify the flow's settings for dedicate tx confirmation/error */
#define DPNI_TX_FLOW_MOD_OPT_ONLY_TX_ERROR	0x00000002
/*!< Modify the tx confirmation/error behavior*/
#define DPNI_TX_FLOW_MOD_OPT_DEST		0x00000004
/*!< Modify the tx-confirmation/error queue destination parameters*/
#define DPNI_TX_FLOW_MOD_OPT_USER_CTX		0x00000008
/*!< Modify the tx-confirmation/error user-context*/
#define DPNI_TX_FLOW_MOD_OPT_L3_CHKSUM_GEN	0x00000010
/*!< Modify the flow's l3 checksum generation */
#define DPNI_TX_FLOW_MOD_OPT_L4_CHKSUM_GEN	0x00000020
/*!< Modify the flow's l4 checksum generation */
/* @} */

/**
 * @brief	Structure representing DPNI TX flow parameters
 */
struct dpni_tx_flow_cfg {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for this tx-flow; use 'DPNI_TX_FLOW_MOD_OPT_xxx' */
	 struct {
		int use_default_queue;
		/*!< This option maybe used when 'options' set
		 with DPNI_TX_FLOW_MOD_OPT_TX_CONF_ERROR;
		 if 'use_default_queue' = 0 set this flow to
		 have its private tx confirmation/error settings */
		int errors_only; /*!< This option maybe used when 'options' set
		 with DPNI_TX_FLOW_MOD_OPT_ONLY_TX_ERROR and 'use_default_queue' = 0;
		 if 'errors_only' = 1,  will send back only errors frames.
		 else send both confirmation and error frames */
		struct dpni_dest_cfg dest_cfg; /*!< This option maybe used
		 when 'options' set with DPNI_TX_FLOW_MOD_OPT_DEST; */
		uint64_t user_ctx;
		/*!< This option maybe used when 'options' set
		 with DPNI_TX_FLOW_MOD_OPT_USER_CTX; will be provided in case
		 of 'tx_conf_err'= 1 or enqueue-rejection condition ("lossless") */
	} conf_err_cfg;
	int l3_chksum_gen;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_L3_CHKSUM_GEN; enable/disable checksum l3
	 generation */
	int l4_chksum_gen;
/*!< This option maybe used when 'options' set
 with DPNI_TX_FLOW_MOD_OPT_L4_CHKSUM_GEN; enable/disable checksum l4
 generation */
};

/**
 *
 * @brief	Set TX flow configuration
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in,out] flow_id - this id is the sender index; should be used as the
 *		third argument of the enqueue command (QDBIN);
 *		for each new sender (flow) use 'DPNI_NEW_FLOW_ID'.
 *		driver will provide back a new flow_id that should
 *		be used for succeeding calls.
 * @param[in]	cfg - flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_flow(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint16_t *flow_id,
	const struct dpni_tx_flow_cfg *cfg);

/**
 * @brief	Structure representing DPNI TX flow attributes
 */
struct dpni_tx_flow_attr {
	 struct {
		int use_default_queue;
		/*!< This option maybe used when 'options' set
		 with DPNI_TX_FLOW_MOD_OPT_TX_CONF_ERR; Prefer this flow to
		 have its private tx confirmation/error settings */
		int errors_only; /*!< This option maybe used when 'options' set
		 with DPNI_TX_FLOW_MOD_OPT_ONLY_TX_ERR and 'tx_conf_err' = 1;
		 if 'only_error_frames' = 1,  will send back only errors frames.
		 else send both confirmation and error frames */
		struct dpni_dest_cfg dest_cfg; /*!< This option maybe used
		 when 'options' set with DPNI_TX_FLOW_MOD_OPT_DEST; */
		uint64_t user_ctx;
		/*!< This option maybe used when 'options' set
		 with DPNI_TX_FLOW_MOD_OPT_USER_CTX; will be provided in case
		 of 'tx_conf_err'= 1 or enqueue-rejection condition ("lossless") */
	} conf_err_cfg;
	int l3_chksum_gen;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_L3_CHKSUM_GEN; enable/disable checksum l3
	 generation */
	int l4_chksum_gen;
/*!< This option maybe used when 'options' set
 with DPNI_TX_FLOW_MOD_OPT_L4_CHKSUM_GEN; enable/disable checksum l4
 generation */
	uint32_t fqid; /*!< Virtual fqid to be used for dequeue operations;
	if equal to 'DPNI_FQID_NOT_VALID' means you need to 
	call this function after you enable the NI. */
};

/**
 *
 * @brief	Get TX flow configuration and fqid
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	flow_id - this id is the sender index
 * @param[out]	attr - flow attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_flow(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint16_t flow_id,
	struct dpni_tx_flow_attr *attr);

/*!
 * @name DPNI Rx flow modification options
 *
 */
#define DPNI_RX_FLOW_MOD_OPT_USER_CTX		0x00000001
/*!< Modify the user's context parameters */
#define DPNI_RX_FLOW_MOD_OPT_DEST		0x00000002
/*!< Modify the destination parameters */
/* @} */

/**
 * @brief	Structure representing DPNI RX flow parameters
 */
struct dpni_rx_flow_cfg {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for the rx-flow; use 'DPNI_RX_FLOW_MOD_OPT_xxx' */
	uint64_t user_ctx;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_FLOW_MOD_OPT_USER_CTX; will be provided
	 with each rx frame */
	struct dpni_dest_cfg dest_cfg; /*!< This option maybe used
	 when 'options' set with DPNI_RX_FLOW_MOD_OPT_DEST; */
};

/**
 * @brief	Structure representing DPNI RX flow parameters
 */
struct dpni_rx_flow_attr {
	uint64_t user_ctx;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_FLOW_MOD_OPT_USER_CTX; will be provided
	 with each rx frame */
	struct dpni_dest_cfg dest_cfg; /*!< This option maybe used
	 when 'options' set with DPNI_RX_FLOW_MOD_OPT_DEST; */
	uint32_t fqid; /*!< Virtual fqid to be used for dequeue operations;
 *		if equal to 'DPNI_FQID_NOT_VALID' means you need to
 *		call this function after you enable the NI. */
};

/**
 *
 * @brief	Set RX flow configuration
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	tc_id - Traffic class id; use 'DPNI_ALL_TCS' for all TCs
 *		and flows
 * @param[in]	flow_id - flow id within the traffic class; it is ignored if
 * 		tc_id was set to 'DPNI_ALL_TCS';
 *		use 'DPNI_ALL_TC_FLOWS' for all flows within this tc_id
 * @param[in]	cfg - flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_flow(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t tc_id,
	uint16_t flow_id,
	const struct dpni_rx_flow_cfg *cfg);

/**
 *
 * @brief	Get RX flow configuration and fqid
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	flow_id - flow id within the traffic class;
 * @param[out]	attr - flow attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_flow(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t tc_id,
	uint16_t flow_id,
	struct dpni_rx_flow_attr *attr);

/**
 *
 * @brief	Set RX error queue's configuration
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token		Token of DPNI object
 * @param[in]	cfg - queue configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_err_queue(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_rx_flow_cfg *cfg);

/**
 *
 * @brief	Get RX error queue's configuration and id
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[out]	attr - queue attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_err_queue(struct fsl_mc_io *mc_io,
	uint16_t token,
	struct dpni_rx_flow_attr *attr);

/**
 *
 * @brief	Set TX conf/error queue's configuration
 *
 * If 'DPNI_OPT_TX_CONF_DISABLED' is set, this fqid will be treated as tx-err
 * and received only errors (confirmation is disabled).
 * Otherwise this fqid will be used for both errors and confirmation
 * (except when a private fqid is used)
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[in]	cfg - queue configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_conf_err_queue(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_rx_flow_cfg *cfg);

/**
 *
 * @brief	Get TX conf/error queue's configuration and id
 *
 * If 'DPNI_OPT_TX_CONF_DISABLED' is set, this fqid will be treated as tx-error
 * and received only errors (confirmation is disabled).
 * Otherwise this fqid will be used for both errors and confirmation
 * (except when a private fqid is used)
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	attr - queue attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_conf_err_queue(struct fsl_mc_io *mc_io,
	uint16_t token,
	struct dpni_rx_flow_attr *attr);

/**
 * @brief	Structure representing QOS table parameters
 */
struct dpni_qos_tbl_cfg {
	struct dpkg_profile_cfg *qos_key_cfg;
	/*!< define the extractions to be used as the QoS criteria */
	int discard_on_miss;
	/*!< '1' for discard the frame in case of no match (miss);
	 '0' for using the 'default_tc' */
	uint8_t default_tc;
/*!< will be used in case of no-match and 'discard_on_miss'= 0 */
};

/**
 *
 * @brief	Set QoS mapping table
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[in]	cfg - QoS table configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_set_qos_table(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_qos_tbl_cfg *cfg);

/**
 * @brief	Structure representing DPNI key parameters
 */
struct dpni_rule_cfg {
	uint64_t key_iova; /*!< IO virtual address to the key */
	uint64_t mask_iova;/*!< IO virtual address to the mask */
	uint8_t key_size; /*!< key/mask size */
};

/**
 *
 * @brief	Add QoS mapping entry
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[in]	cfg - QoS key parameters
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_qos_entry(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_rule_cfg *cfg,
	uint8_t tc_id);

/**
 *
 * @brief	Remove QoS mapping entry
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[in]	cfg - QoS key parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_qos_entry(struct fsl_mc_io *mc_io,
	uint16_t token,
	const struct dpni_rule_cfg *cfg);

/**
 *
 * @brief	Clear all QoS mapping entries
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_qos_table(struct fsl_mc_io *mc_io, uint16_t token);

/**
 *
 * @brief	Add FS entry for a specific traffic-class
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - Key parameters
 * @param[in]	flow_id - Flow id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_add_fs_entry(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t tc_id,
	const struct dpni_rule_cfg *cfg,
	uint16_t flow_id);

/**
 *
 * @brief	Remove FS entry from a specific traffic-class
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - Key parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_remove_fs_entry(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t tc_id,
	const struct dpni_rule_cfg *cfg);

/**
 *
 * @brief	Clear all FS entries
 *
 * @param[in]	mc_io - Pointer to opaque I/O object
 * @param[in]   token - Token of DPNI object
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpni_clear_fs_entries(struct fsl_mc_io *mc_io,
	uint16_t token,
	uint8_t tc_id);

int dpni_set_vlan_insertion(struct fsl_mc_io *mc_io, uint16_t token, int en);
int dpni_set_vlan_removal(struct fsl_mc_io *mc_io, uint16_t token, int en);
int dpni_set_ipr(struct fsl_mc_io *mc_io, uint16_t token, int en);
int dpni_set_ipf(struct fsl_mc_io *mc_io, uint16_t token, int en);

/** @} */

#endif /* __FSL_DPNI_H */
