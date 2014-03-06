/* Copyright 2013 Freescale Semiconductor, Inc. */
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

#ifdef MC
struct dpni;
#else
#include <fsl_cmdif.h>
struct dpni {
	struct cmdif_desc cidesc; /*!< Descriptor for command portal */
};
#endif

/*!
 * @name General DPNI macros
 */
#define DPNI_MAX_TC				8
/*!< Max number of traffic classes */
#define DPNI_MAX_UNICAST_FILTERS		16
/*!< Maximum number of unicast filters */
#define DPNI_MAX_MULTICAST_FILTERS		64
/*!< Maximum number of multicast filters */
#define DPNI_MAX_VLAN_FILTERS			16
/*!< Maximum number of VLAN filters */
#define DPNI_MAX_QOS_ENTRIES			64
/*!< Maximum number of QoS entries */
#define DPNI_MAX_DPBP				8
/*!< Maximum number of bm-pools */

#define DPNI_ALL_TCS				(-1)
/*!< All traffic classes considered */
#define DPNI_ALL_TC_FLOWS			(-1)
/*!< All flows within traffic classes considered */
#define DPNI_NEW_FLOW_ID			(-1)
/*!< Generate new flow id */
#define DPNI_VFQID_NOT_VALID			(-1)
/*!< Invalid virtual FQID  */
/* @} */

/**
 * @brief   structure representing FLC parameters
 */
struct dpni_flc_cfg {
	int stash_en; /*!< either stash enabled or not */
	union {
		struct {
			uint8_t frame_annotation_size;
			/*!< Size of Frame  Annotation to be stashed */
			uint8_t frame_data_size;
			/*!< Size of Frame Data to be stashed. */
			uint8_t flow_context_size;
			/*!< Size of flow context to be stashed. */
			uint64_t flow_context_addr;
		/*!< 64/49 bit memory address containing the
		 flow context information to be stashed;
		 Must be cacheline-aligned */
		} stashing;
		uint64_t odp; /*!< value to be written for order-definition */
	} u;
};

/**
 *
 * @brief	Open object handle - Required before any operation on the object
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	dpni_id - DPNI unique ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpni_open(struct dpni *dpni, int dpni_id);

/**
 *
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_close(struct dpni *dpni);

/*!
 * @name DPNI configuration options
 *
 */
#define DPNI_OPT_ALLOW_DIST_KEY_PER_TC		0x00000001
/*!< allow different dist-key per TC */
#define DPNI_OPT_CHKSM_VALID_DISABLED		0x00000002
/*!< checksum validation disable */
#define DPNI_OPT_TX_CONF_DISABLED		0x00000004
/*!< tx-confirmation disable */
#define DPNI_OPT_DIST_HASH			0x00000010
/*!< hash based distribution support */
#define DPNI_OPT_DIST_FS			0x00000020
/*!< flow-steering based distribution support */
#define DPNI_OPT_POLICING			0x00000040
/*!< policing support */
#define DPNI_OPT_UNICAST_FILTER			0x00000080
/*!< unicast filtering support */
#define DPNI_OPT_MULTICAST_FILTER		0x00000100
/*!< multicast filtering support */
#define DPNI_OPT_VLAN_FILTER			0x00000200
/*!< vlan filtering support */
#define DPNI_OPT_MACSEC				0x00000400
/*!< MACSEC support */
#define DPNI_OPT_IPR				0x00000800
/*!< IP-reassembly support */
#define DPNI_OPT_IPF				0x00001000
/*!< IP-fragmentation support */
#define DPNI_OPT_RSC				0x00002000
/*!< RSC support */
#define DPNI_OPT_GSO				0x00004000
/*!< GSO support */
#define DPNI_OPT_IPSEC				0x00008000
/*!< IPSec transport support */
/* @} */

/**
 * @brief	DPNI types
 *
 */
enum dpni_type {
	DPNI_TYPE_NI = 1, /*!< DPNI of type NI */
	DPNI_TYPE_NIC
/*!< DPNI of type NIC */
};

/**
 * @brief	Structure representing DPNI configuration
 */
struct dpni_cfg {
	enum dpni_type type; /*!< DPNI Type */
	uint8_t mac_addr[6]; /*!< Primary mac address */
	struct {
		uint64_t options;
		/*!< Mask of available options; use 'DPNI_OPT_XXX' */
		uint8_t max_senders;
		/*!< maximum number of different senders; will be used as the
		 * number of dedicated tx flows; '0' will e treated as '1' */
		uint8_t max_tcs;
		/*!< maximum number of traffic-classes;
		 will affect both Tx & Rx; '0' will e treated as '1' */
		uint16_t max_dist_per_tc[DPNI_MAX_TC];
		/*!< maximum distribution's size per Rx traffic-class;
		 represent the maximum DPIO objects that will be
		 referenced by this TC; '0' will e treated as '1' */
		uint8_t max_unicast_filters;
		/*!< maximum number of unicast filters; '0' will be treated
		 as 'DPNI_MAX_UNICAST_FILTERS' */
		uint8_t max_multicast_filters;
		/*!< maximum number of multicast filters; '0' will be treated
		 as 'DPNI_MAX_MULTICAST_FILTERS' */
		uint8_t max_vlan_filters;
		/*!< maximum number of vlan filters; '0' will be treated
		 as 'DPNI_MAX_VLAN_FILTERS' */
		uint8_t max_qos_entries;
		/*!< if 'max_tcs>1', declare the maximum entries for the
		 QoS table; '0' will be treated as
		 'DPNI_MAX_QOS_ENTRIES' */
		uint8_t max_qos_key_size;
		/*!< maximum key size for the QoS look-up; '0' will be treate
		 * as '24' which enough for IPv4 5-tuple */
		uint8_t max_dist_key_size;
	/*!< maximum key size for the distribution; '0' will be treated as
	 '24' which enough for IPv4 5-tuple */
	} adv; /*!< use this structure to change default settings */
};

/**
 *
 * @brief	Will allocate resources and preliminary initialization
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	cfg - configuration parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	This function should be called only once during
 *		lifetime of the object
 */
int dpni_init(struct dpni *dpni, const struct dpni_cfg *cfg);

/**
 *
 * @brief	Free the DPNI object and all its resources.
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpni_done(struct dpni *dpni);

/**
 *
 * @brief	Sets IRQ information for the dpni object
 *		(required for DPNI to signal events).
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	irq_index - identifies the specific IRQ to configure
 * @param[in]	irq_paddr - physical IRQ address that must be written
 *		to signal the interrupt
 * @param[in]	irq_val - IRQ value to write into the IRQ address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_irq(struct dpni *dpni,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val);

/**
 *
 * @brief	Enable/Disable transmission of Pause-Frames.
 *		Will only affect the underlying MAC if exist.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]   priority -  the PFC class of service; use '0xff'
 *		to indicate legacy pause support (i.e. no PFC).
 * @param[in]   pause_time - Pause quanta value used with transmitted
 *		pause frames. Each quanta represents a 512 bit-times;
 *		Note that '0' as an input here will be used
 *		as disabling the transmission of the pause-frames.
 * @param[in]   thresh_time - Pause Threshold quanta value used by the MAC to
 *		retransmit pause frame. if the situation causing
 *		a pause frame to be sent didn't finish when
 *		the timer reached the threshold quanta, the MAC
 *		will retransmit the pause frame.
 *		Each quanta represents a 512 bit-times.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_pause_frames(struct dpni *dpni,
	uint8_t priority,
	uint16_t pause_time,
	uint16_t thresh_time);

/**
 *
 * @brief	Enable/Disable ignoring of Pause-Frames.
 *		Will only affect the underlying MAC if exist.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]   enable - indicates whether to ignore the incoming pause
 *		frames or not.
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_ignore_pause_frames(struct dpni *dpni, int enable);

/**
 * @brief   DPNI destination types
 */
enum dpni_dest {
	DPNI_DEST_NONE = 0,
	/*!< unassigned destination; i.e. queues will be set in parked mode  */
	DPNI_DEST_DPIO,
	/*!< queues will generate notification to the dpio's channel;
	 i.e. will be set in schedule mode and FQDAN enable */
	DPNI_DEST_DPCON
/*!< queues won't generate notification, but will be connected to this
 channel object; i.e. will be set in schedule mode and FQDAN disable */
};

/**
 * @brief	Structure representing DPNI destination parameters
 */
struct dpni_dest_cfg {
	enum dpni_dest type; /*!< destination type */
	uint16_t dpio_id;
	/*!< DPIO object id; must have a notification channel
	 * (either local or remote) */
	uint16_t dpcon_id;
	/*!< DPCON object id */
	uint8_t priority;
/*!< 0-1 or 0-7 (depends on the channel type) to select the priority(work-queue)
 within the channel (not relevant for the 'NONE' case) */
};

/**
 * @brief	Structure representing DPNI attach parameters
 */
struct dpni_attach_cfg {
	uint8_t num_dpbp; /*!< number of DPBPs */
	uint16_t dpbp_id[DPNI_MAX_DPBP]; /*!< DPBPs object id */
/* TODO - add struct ldpaa_flow_ctx	*flc; */
	uint64_t rx_user_ctx;
	/*!< User context; will be received with the FD in case of Rx
	 frame; can be override by calling 'dpni_set_rx_flow' */
	int dest_apply_all; /*!< in case 'dest_apply_all'=1, 'dest_cfg' will
	 affect on all receive queues, otherwise it will affect only
	 on rx-err/tx-err queues. */
	struct dpni_dest_cfg dest_cfg; /*!< destination settings; will be
	 applied according to the 'dest_apply_all' */
	uint64_t rx_err_user_ctx;
	/*!< User context; will be received with the FD in case of Rx
	 error frame */
	uint64_t tx_err_user_ctx;
	/*!< User context; will be received with the FD in case of Tx
	 error frame and 'DPNI_OPT_TX_CONF_DISABLED' is set.
	 if not set, tx-error frames will received with 'tx_conf_user_ctx' */
	uint64_t tx_conf_user_ctx;
/*!< User context; will be received with the FD in case of Tx
 confirmation frame; can be override by calling 'dpni_set_tx_flow' */
};

/**
 *
 * @brief	Attach the NI to application
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	cfg - Attach configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_attach(struct dpni *dpni, const struct dpni_attach_cfg *cfg);

/**
 *
 * @brief	Detach the NI from application
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_detach(struct dpni *dpni);

/**
 * @brief	DPNI link types
 *
 */
enum dpni_link_type {
	DPNI_LINK_TYPE_DPMAC = 1, /*!< Attached to DPMAC */
	DPNI_LINK_TYPE_DPSW, /*!< Attached to DPSW */
	DPNI_LINK_TYPE_DPDMUX, /*!< Attached to DPDMUX */
	DPNI_LINK_TYPE_DPLAG, /*!< Attached to DPLAG */
	DPNI_LINK_TYPE_DPNI
/*!< Attached to DPNI */
};

/**
 * @brief	Structure representing DPNI attach link parameters
 *
 * To be removed!!!!
 */
struct dpni_attach_link_cfg {
	enum dpni_link_type link_type; /*!< Linked object type */
	union {
		struct {
			uint16_t id;/*!< DPMAC object id */
		} dpmac;
		struct {
			uint16_t id;/*!< DPSW object id */
			uint8_t if_id;
		/*!< DPSW interface-id to be linked to */
		} dpsw;
		struct {
			uint16_t id;/*!< DPDMUX object id */
			uint8_t if_id;
		/*!< DPDMUX interface-id to be linked to */
		} dpdmux;
		struct {
			uint16_t id;/*!< DPLAG object id */
		} dplag;
		struct {
			uint16_t id;/*!< DPNI object id */
		} dpni;
	} u;
};

/**
 *
 * @brief	Attach the NI to the link side
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	cfg - Link attachment configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_attach_link(struct dpni *dpni, const struct dpni_attach_link_cfg *cfg);

/**
 *
 * @brief	Detach the NI from the link side
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_detach_link(struct dpni *dpni);

/**
 *
 * @brief	Enable the NI, will allow sending and receiving frames.
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_enable(struct dpni *dpni);

/**
 *
 * @brief	Disable the NI, will disallow sending and receiving frames.
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_disable(struct dpni *dpni);

/**
 *
 * @brief	Reset the NI, will return to initial state.
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_reset(struct dpni *dpni);

/**
 * @brief	Structure representing attributes parameters
 */
struct dpni_attr {
	uint64_t options; /*!< reflect the value as was given in the
	 initialization phase */
	uint8_t max_tcs; /*!< reflect the value as was given in the
	 initialization phase */
	uint16_t max_dist_per_tc[DPNI_MAX_TC]; /*!< reflect the value
	 as was given in the initialization phase */
};

/**
 *
 * @brief	Retrieve the object's attributes.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_attributes(struct dpni *dpni, struct dpni_attr *attr);

/*!
 * @name DPNI Rx buffer layout modification options
 *
 */
#define DPNI_RX_LAYOUT_MOD_OPT_TIMESTAMP		0x00000001
/*!< Modify the time-stamp setting */
#define DPNI_RX_LAYOUT_MOD_OPT_PARSER_RESULT		0x00000002
/*!< Modify the parser-result setting */
#define DPNI_RX_LAYOUT_MOD_OPT_FRAME_STATUS		0x00000004
/*!< Modify the frame-status setting */
#define DPNI_RX_LAYOUT_MOD_OPT_PRIVATE_DATA_SIZE	0x00000008
/*!< Modify the private-data-size setting */
#define DPNI_RX_LAYOUT_MOD_OPT_DATA_ALIGN		0x00000010
/*!< Modify the data-alignment setting */
/* @} */

/**
 * @brief	Structure representing DPNI RX buffer layout
 */
struct dpni_rx_buffer_layout {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for the buffer layout; use 'DPNI_RX_LAYOUT_MOD_OPT_xxx' */
	int pass_timestamp;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_TIMESTAMP */
	int pass_parser_result;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_PARSER_RESULT */
	int pass_frame_status;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_FRAME_STATUS */
	uint16_t private_data_size;
	/*!< This option maybe used when 'options' set
	 with DPNI_RX_LAYOUT_MOD_OPT_PRIVATE_DATA_SIZE */
	uint16_t data_align;
/*!< This option maybe used when 'options' set
 with DPNI_RX_LAYOUT_MOD_OPT_DATA_ALIGN */
};

/**
 *
 * @brief	Retrieve the RX buffer layout settings.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_buffer_layout(struct dpni *dpni,
	struct dpni_rx_buffer_layout *layout);

/**
 *
 * @brief	Set the RX buffer layout settings.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	layout - buffer's layout
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only when DPNI is disabled
 */
int dpni_set_rx_buffer_layout(struct dpni *dpni,
	struct dpni_rx_buffer_layout *layout);

/**
 *
 * @brief	Get the QDID used for enqueue
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	qdid - Qdid used for qneueue
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_enable().
 */
int dpni_get_qdid(struct dpni *dpni, uint16_t *qdid);

/**
 *
 * @brief	Get the tx data offset
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	data_offset - TX data offset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_data_offset(struct dpni *dpni, uint16_t *data_offset);

/**
 * @brief	DPNI Counter types
 *
 */
enum dpni_counter {
	DPNI_CNT_ING_FRAME = 1,
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
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	counter - the requested counter
 * @param[out]	value - counter's current value
 *
 * @returns        '0' on Success; Error code otherwise.
 */
int dpni_get_counter(struct dpni *dpni,
	enum dpni_counter counter,
	uint64_t *value);

/**
 *
 * @brief   Write to one of the DPNI counters
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	counter - the requested counter
 * @param[in]   value - New counter value.
 *		typically '0' for resetting the counter.
 *
 * @returns        '0' on Success; Error code otherwise.
 */
int dpni_set_counter(struct dpni *dpni,
	enum dpni_counter counter,
	uint64_t value);

/**
 *
 * @brief	Return the link state, either up or down
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	up - return '0' for down, '1' for up
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_link_state(struct dpni *dpni, int *up);

/**
 *
 * @brief	Set the maximum received frame length.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	mfl - MFL length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_mfl(struct dpni *dpni, uint16_t mfl);

/**
 *
 * @brief	Get the maximum received frame length.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	mfl - MFL length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_mfl(struct dpni *dpni, uint16_t *mfl);

/**
 *
 * @brief	Set the MTU for this interface. Will have affect on IPF and
 *		conditionally on GSO.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	mtu - MTU length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_mtu(struct dpni *dpni, uint16_t mtu);

/**
 *
 * @brief	Get the MTU.
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	mtu - MTU length
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_mtu(struct dpni *dpni, uint16_t *mtu);

/**
 *
 * @brief	Enable/Disable multicast promiscuous mode
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_multicast_promisc(struct dpni *dpni, int en);

/**
 *
 * @brief	Get multicast promiscuous mode
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[out]	en - '1' for enabling/'0' for disabling
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_multicast_promisc(struct dpni *dpni, int *en);

/**
 *
 * @brief	Set the primary mac address
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_primary_mac_addr(struct dpni *dpni, const uint8_t addr[6]);

/**
 *
 * @brief	Get the primary mac address
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_primary_mac_addr(struct dpni *dpni, uint8_t addr[6]);

/**
 *
 * @brief	Add unicast/multicast filter address
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_add_mac_addr(struct dpni *dpni, const uint8_t addr[6]);

/**
 *
 * @brief	Remove unicast/multicast filter address
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	addr - MAC address
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_remove_mac_addr(struct dpni *dpni, const uint8_t addr[6]);

/**
 *
 * @brief	Clear the mac filter table
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_clear_mac_table(struct dpni *dpni);

/**
 *
 * @brief	Add VLAN-id filter
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	vlan_id - VLAN ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_add_vlan_id(struct dpni *dpni, uint16_t vlan_id);

/**
 *
 * @brief	Add VLAN-id filter
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	vlan_id - VLAN ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_remove_vlan_id(struct dpni *dpni, uint16_t vlan_id);

/**
 *
 * @brief	Clear the VLAN filter table
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_clear_vlan_table(struct dpni *dpni);

/**
 * @brief	Structure representing DPNI TX TC parameters
 */
struct dpni_tx_tc_cfg {
	uint16_t depth_limit;
/*!<  if >0 than limit the depth of this queue which may result with
 * rejected frames */
/* TODO - support both bytes & frames??? if not what we prefer?? */
};

/**
 *
 * @brief	Set TX TC settings
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - TC parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_tc(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_tx_tc_cfg *cfg);

/**
 * @brief	Structure representing DPNI RX TC parameters
 */
struct dpni_rx_tc_cfg {
	uint16_t dist_size; /*!< set the distribution size */
/*	struct policing_cfg *params;*/
/*TODO - add struct ldpaa_flow_ctx	*flc;*/
};

/**
 *
 * @brief	Set RX TC settings
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - TC parameters
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpni_set_rx_tc(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_rx_tc_cfg *cfg);

/*!
 * @name DPNI Tx flow modification options
 *
 */
#define DPNI_TX_FLOW_MOD_OPT_TX_CONF_ERR	0x00000001
/*!< Modify the flow's settings for dedicate tx confirmation/error */
#define DPNI_TX_FLOW_MOD_OPT_ONLY_TX_ERR	0x00000002
/*!< Modify the tx confirmation/error behavior*/
#define DPNI_TX_FLOW_MOD_OPT_DEST		0x00000004
/*!< Modify the tx-confirmation/error queue destination parameters*/
#define DPNI_TX_FLOW_MOD_OPT_USER_CTX		0x00000008
/*!< Modify the tx-confirmation/error user-context*/
#define DPNI_TX_FLOW_MOD_OPT_L3_CKSUM_GEN	0x00000010
/*!< Modify the flow's l3 checksum generation */
#define DPNI_TX_FLOW_MOD_OPT_L4_CKSUM_GEN	0x00000020
/*!< Modify the flow's l4 checksum generation */
/* @} */

/**
 * @brief	Structure representing DPNI TX flow parameters
 */
struct dpni_tx_flow_cfg {
	uint32_t options;
	/*!< the flags that represent the modification that are required to be
	 done for this tx-flow; use 'DPNI_TX_FLOW_MOD_OPT_xxx' */
	int tx_conf_err;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_TX_CONF_ERR; Prefer this flow to
	 have its private tx confirmation/error settings */
	int only_error_frames; /*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_ONLY_TX_ERR and 'tx_conf_err' = 1;
	 if 'only_error_frames' = 1,  will send back only errors frames.
	 else send both confirmation and error frames */
	struct dpni_dest_cfg dest_cfg; /*!< This option maybe used
	 when 'options' set with DPNI_TX_FLOW_MOD_OPT_DEST; */
	uint64_t user_ctx;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_USER_CTX; will be provided in case
	 of 'tx_conf_err'= 1 or enqueue-rejection condition ("lossless") */
	int l3_chksum_gen;
	/*!< This option maybe used when 'options' set
	 with DPNI_TX_FLOW_MOD_OPT_L3_CKSUM_GEN; enable/disable checksum l3
	 generation */
	int l4_chksum_gen;
/*!< This option maybe used when 'options' set
 with DPNI_TX_FLOW_MOD_OPT_L4_CKSUM_GEN; enable/disable checksum l4
 generation */
};

/**
 *
 * @brief	Set TX flow configuration
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in,out] flow_id - this id is the sender index; should be used as the
 *		third argument of the enqueue command (QDBIN);
 *		for each new sender (flow) use 'DPNI_NEW_FLOW_ID'.
 *		driver will provide back a new flow_id that should
 *		be used for succeeding calls.
 * @param[in]	cfg - flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_tx_flow(struct dpni *dpni,
	uint16_t *flow_id,
	const struct dpni_tx_flow_cfg *cfg);

/**
 *
 * @brief	Get TX flow configuration and fqid
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	flow_id - this id is the sender index
 * @param[out]	cfg - flow configuration
 * @param[out]	fqid - virtual fqid to be used for dequeue operations;
 *		if equal to 'DPNI_VFQID_NOT_VALID' means you need to
 *		call this function after you enable the NI.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_tx_flow(struct dpni *dpni,
	uint16_t flow_id,
	struct dpni_tx_flow_cfg *cfg,
	uint32_t *fqid);

/*!
 * @name DPNI Rx flow modification options
 *
 */
#define DPNI_RX_FLOW_MOD_OPT_USER_CTX		0x00000001
/*!< Modify the user's context parameters */
#define DPNI_RX_FLOW_MOD_OPT_DEST		0x00000002
/*!< Modify the destination parameters */
/* #define DPNI_RX_Q_MOD_OPT_FLC			0x00000004 */
/*!< Modify the flow-context (e.g. stashing) parameters */
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
/* TODO - add struct ldpaa_flow_ctx	*flc;*/
/*!< valid only in case of flow-steering */
};

/**
 *
 * @brief	Set RX flow configuration
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id; use 'DPNI_ALL_TCS' for all TCs
 *		and flows
 * @param[in]	flow_id - flow id within the traffic class;
 *		use 'DPNI_ALL_TC_FLOWS' for all flows within this tc_id
 * @param[in]	cfg - flow configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_set_rx_flow(struct dpni *dpni,
	uint8_t tc_id,
	uint16_t flow_id,
	const struct dpni_rx_flow_cfg *cfg);

/**
 *
 * @brief	Get TX flow configuration and fqid
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	flow_id - flow id within the traffic class;
 * @param[out]	cfg - flow configuration
 * @param[out]	fqid - virtual fqid to be used for dequeue operations;
 *		if equal to 'DPNI_VFQID_NOT_VALID' means you need to
 *		call this function after you enable the NI.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpni_get_rx_flow(struct dpni *dpni,
	uint8_t tc_id,
	uint16_t flow_id,
	struct dpni_rx_flow_cfg *cfg,
	uint32_t *fqid);

/**
 * @brief	Structure representing QOS table parameters
 */
struct dpni_qos_tbl_cfg {
	struct dpkg_profile_cfg *extract_cfg;
	/*!< define the extractions to be used as the QoS criteria */
	int drop_frame;
	/*!< '1' for dropping the frame in case of no match;
	 '0' for using the 'default_tc' */
	uint8_t default_tc;
/*!< will be used in case of no-match and 'drop_frame'=0 */
};

/**
 *
 * @brief	Set QoS mapping table
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	cfg - QoS table configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_set_qos_table(struct dpni *dpni, const struct dpni_qos_tbl_cfg *cfg);

/**
 *
 * @brief	Delete QoS mapping table
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_delete_qos_table(struct dpni *dpni);

/**
 * @brief	Structure representing DPNI key parameters
 */
struct dpni_key_cfg {
	uint8_t *key; /*!< A pointer to the key */
	uint8_t *mask;/*!< A pointer to the mask */
	uint8_t size; /*!< key/mask size */
};

/**
 *
 * @brief	Add QoS mapping entry
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	cfg - QoS key parameters
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_add_qos_entry(struct dpni *dpni,
	const struct dpni_key_cfg *cfg,
	uint8_t tc_id);

/**
 *
 * @brief	Remove QoS mapping entry
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	cfg - QoS key parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_remove_qos_entry(struct dpni *dpni, const struct dpni_key_cfg *cfg);

/**
 *
 * @brief	Clear all QoS mapping entries
 *
 * @param[in]	dpni - Pointer to dpni object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_clear_qos_table(struct dpni *dpni);

/**
 * @brief	Structure representing distribution parameters
 */
struct dpni_dist_cfg {
	int dist_fs;
	/*!< '1' for distribution based on flow-steering;
	 '0' for hash based */
	struct dpkg_profile_cfg *extract_cfg;
/*!< define the extractions to be used for the distribution key */
};

/**
 *
 * @brief	Set the distribution method and key
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	dist - distribution configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_set_dist(struct dpni *dpni,
	const struct dpni_dist_cfg dist[DPNI_MAX_TC]);

/**
 * @brief   DPNI Flow-Steering miss action
 */
enum dpni_fs_miss_action {
	DPNI_FS_MISS_DROP = 0,
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
/* TODO - add mask to select a subset of the hash result */
};

/**
 *
 * @brief	Set FS mapping table for a specific traffic class
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - FS table configuration
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_set_fs_table(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_fs_tbl_cfg *cfg);

/**
 *
 * @brief	Delete FS mapping table
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_delete_fs_table(struct dpni *dpni, uint8_t tc_id);

/**
 *
 * @brief	Add FS entry for a specific traffic-class
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - Key parameters
 * @param[in]	flow_id - Flow id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_add_fs_entry(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_key_cfg *cfg,
	uint16_t flow_id);

/**
 *
 * @brief	Remove FS entry from a specific traffic-class
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 * @param[in]	cfg - Key parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_remove_fs_entry(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_key_cfg *cfg);

/**
 *
 * @brief	Clear all FS entries
 *
 * @param[in]	dpni - Pointer to dpni object
 * @param[in]	tc_id - Traffic class id
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpni_attach().
 */
int dpni_clear_fs_table(struct dpni *dpni, uint8_t tc_id);


int dpni_get_irq(struct dpni *dpni,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val);

int dpni_set_irq_enable(struct dpni *dpni,
                          uint8_t irq_index,
                          uint8_t enable_state);

int dpni_get_irq_enable(struct dpni *dpni,
                          uint8_t irq_index,
                          uint8_t *enable_state);

int dpni_set_irq_mask(struct dpni *dpni,
                        uint8_t irq_index,
                        uint32_t mask);

int dpni_get_irq_mask(struct dpni *dpni,
                        uint8_t irq_index,
                        uint32_t *mask);

int dpni_get_irq_status(struct dpni *dpni,
                         uint8_t irq_index,
                         uint32_t *status);

int dpni_clear_irq_status(struct dpni *dpni,
                            uint8_t irq_index,
                            uint32_t status);

/** @} */

#endif /* __FSL_DPNI_H */
