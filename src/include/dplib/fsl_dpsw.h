/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dpsw.h
 *  @brief   Data Path L2 Switch API
 */

#ifndef __FSL_DPSW_H
#define __FSL_DPSW_H

#include <fsl_net.h>

#ifdef MC
/*!< Forward declaration */
struct dpsw;
#else
#include <fsl_cmdif.h>
struct dpsw {
	struct cmdif_desc cidesc;
};
#endif

/*!
 * @Group grp_dprc	L2 switch API
 *
 * @brief	Contains API for handling L2 switch topology and functionality
 * @{
 */

/*!
 * @name L2 switch defines
 */
#define DPSW_MAX_TC		8	/*!< Maximum number of TC */
#define DPSW_MAX_PRI		8	/*!< Maximum number of priorities */
#define DPSW_MAX_VLAN		32	/*!< Maximum number of VLAN's */
#define DPSW_MAX_IF		256	/*!< Maximum number of interfaces */
#define DPSW_MAX_FDB		32	/*!< Maximum number of FDB */
#define DPSW_MAX_PROT		64	/*!< Max Protocols per rule */

/**
 * @brief	open L2 switch
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]    dpsw_id	Switch unique ID
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_open(struct dpsw *dpsw, int dpsw_id);

/**
 * @brief	Closes the object handle, no further operations on the object
 are allowed
 *
 * @param[in]	dpsw		L2 switch handle
 *
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_close(struct dpsw *dpsw);

/*!
 * @name L2 switch options
 */
#define DPSW_OPT_FLOODING_DIS	0x0000000000000001 /*!< Disable flooding */
#define DPSW_OPT_BROADCAST_DIS	0x0000000000000002 /*!< Disable Broadcast */
#define DPSW_OPT_MULTICAST_DIS	0x0000000000000004 /*!< Disable Multicast */
#define DPSW_OPT_TC_DIS		0x0000000000000008
/*!< Disable Traffic classes */
/* @} */

/**
 * @brief	L2 switch Interface configuration
 *
 */
struct dpsw_if_cfg {
	int external; /*!< Internal or external interface.
	 Remove when Link manager GPP mode
	 is supported */

	int phys_if_id; /*!< Physical port ID, relevant only
	 for external interface type.
	 Remove when Link manager GPP mode
	 is supported */

	int iop_id; /*!< WRIOP block identifier */
	int control; /*!< Control/Data Interface  */
	uint8_t num_tcs; /*!< The number of egress traffic
	 classes, Relevant only if traffic
	 classes are enabled. The valid value is
	 from 1-8 */
	uint64_t options; /*!< features (bitmap). DPSW_OPT_*/
};

/**
 * @brief	L2 switch configuration
 *
 */
struct dpsw_cfg {
	uint16_t num_ifs; /*!< Number of external and
	 internal interfaces */
	struct dpsw_if_cfg if_cfg[DPSW_MAX_IF];/*!< Array of interface
	 parameters */
	/*!< Advanced parameters. Default is zeros */
	struct {
		/*!< Enable/Disable L2 switch features (bitmap) */
		uint64_t options;
		/*!< Maximum Number of VLAN's. 0 - indicates default 16 */
		uint8_t max_vlans;
		/*!< Maximum Number of FDB's. 0 - indicates default 16 */
		uint8_t max_fdbs;
		/*!< Number of FDB entries for default FDB table.
		 * 0 - indicates default 1024 entries. */
		uint16_t num_fdb_entries;
		/*!< Default FDB aging time for default FDB table.
		 * 0 - indicates default 300 seconds */
		uint16_t fdb_aging_time;
	} adv;
};

/**
 * @brief	dpsw_init is a Topology function that initializes L2 switch and
 allocates L2 switch object. As a result of this call
 1.	FDB allocated having (FDB ID 0)
 2.	VLAN created (VLAN ID = 1)
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]    cfg		Configuration parameters
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_init(struct dpsw *dpsw, const struct dpsw_cfg *cfg);

/**
 * @brief	a topology Function frees L2 Switch resources
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_done(struct dpsw *dpsw);

/**
 * @brief	L2 switch attributes
 *
 */
struct dpsw_attr {
	uint32_t version; /* L2 switch Version */
	uint64_t options; /* Enable/Disable L2 switch features */
	uint8_t max_vlans; /* Maximum Number of VLANs */
	uint8_t max_fdbs; /* Maximum Number of FDBs */
	uint16_t mem_size; /* L2 Switch frame storage mem size */
	uint16_t num_ifs; /* Number of interfaces */
	uint8_t num_vlans; /* Current number of VLANs */
	uint8_t num_fdbs; /* Current number of FDBs */
};

/**
 * @brief	obtains L2 switch attributes
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[out]   attr		L2 switch attributes
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_get_attributes(struct dpsw *dpsw, struct dpsw_attr *attr);

/**
 * @brief	Function is used to enable L2 Switch functionality
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_enable(struct dpsw *dpsw);

/**
 * @brief	Function is used to disable L2 Switch functionality
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_disable(struct dpsw *dpsw);

/**
 * @brief	Function is used to reset L2 switch.
 It places L2 switch back to original configuration
 after dpsw_init. All attached interfaces and allocated
 resources are retained after reset.
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_reset(struct dpsw *dpsw);

/**
 * @brief	Policer configuration mode
 *
 */
enum dpsw_policer_mode {
	DPSW_POLICER_DIS = 0, /*!< Disable Policer */
	DPSW_POLICER_EN
/*!< Enable Policer */
};

/**
 * @brief	Policer configuration
 *
 */
struct dpsw_policer_cfg {
	enum dpsw_policer_mode mode; /*!< Enables/Disables policer (Ingress) */
};

/**
 * @brief	Function is used to enable/disable policer for L2 switch
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	cfg		Configuration parameters

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_policer(struct dpsw *dpsw, const struct dpsw_policer_cfg *cfg);

/**
 * @brief	Structure representing buffer depletion configuration
 *		parameters. Assuming only one buffer pool
 *		exist per switch.
 *
 */
struct dpsw_buffer_depletion_cfg {
	uint32_t entrance_threshold; /*!<Entrance threshold */
	uint32_t exit_threshold; /*!< Exit threshold */
	uint64_t wr_addr; /*!< Address in GPP to write
	 buffer depletion State Change
	 Notification Message */
};

/**
 * @brief	API is used to configure thresholds for buffer depletion state
 and establish buffer depletion State Change Notification Message
 (CSCNM) from Congestion Point (CP) to GPP trusted software
 This configuration is used to trigger PFC request or congestion
 notification if enabled. It is assumed one buffer pool defined
 per switch
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	cfg		Configuration parameters

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_buffer_depletion(struct dpsw *dpsw,
	const struct dpsw_buffer_depletion_cfg *cfg);

/**
 * @brief	Function sets target interface for reflected ports.
 Only one reflection receive port is allowed per switch
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Id

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_reflection_if(struct dpsw *dpsw, uint16_t if_id);

/**
 * @brief	Action
 *
 */
enum dpsw_action {
	DPSW_ACTION_DROP = 0, /*!< Drop frame */
	DPSW_ACTION_REDIRECT_TO_CTRL
/*!< Redirect to control port  */
};

/**
 * @brief	Parser error action configuration
 *
 */
struct dpsw_parser_error_action_cfg {
	uint8_t num_prots; /*!< Number of protocols */
	enum net_prot prot[DPSW_MAX_PROT]; /*!< Set of protocols */
	enum dpsw_action action; /*!< Action to take */
};

/**
 * @brief	Function is used to define frame errors that should be
 monitored and corresponding action to take
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	cfg		Error action description

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_parser_error_action(struct dpsw *dpsw,
	const struct dpsw_parser_error_action_cfg *cfg);

/*!
 * @name Precision Time Protocol (PTP) options
 */
#define DPSW_PTP_OPT_UPDATE_FCV	0x1	/*!< Indicate the need for UDP
						checksum update after PTP time
						correction field has been
						filled in */
/* @} */

/**
 * @brief	Precision Time Protocol (PTP) configuration
 *
 */
struct dpsw_ptp_v2_cfg {
	int enable; /*!< Enable updating Correction time
	 filed in IEEE1588 V2 messages */
	uint16_t time_offset; /*!<	Time correction field offset
	 inside PTP from L2 start of the frame.
	 PTP messages can be transported over
	 different underlying protocols
	 IEEE802.3, IPv4/UDP, Ipv6/UDP and many
	 others. */

	uint32_t options; /*!<	Bitmap. An additional options
	 along with time correction
	 DPSW_PTP_OPT_XXX */
};

/**
 * @brief	Function is used to define IEEE1588 V2 Precision Time Protocol
 (PTP) parameters for time correction
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	cfg		IEEE1588 V2 Configuration parameters

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_set_ptp_v2(struct dpsw *dpsw, const struct dpsw_ptp_v2_cfg *cfg);

/**
 * @brief	Tag Contorl Information (TCI) configuration
 *
 */
struct dpsw_tci_cfg {
	uint8_t pcp;
	/*!< Priority Code Point (PCP): a 3-bit field which refers
	 * to the IEEE 802.1p priority.
	 */
	uint8_t dei;
	/*!< Drop Eligible Indicator (DEI): a 1-bit field. May be used
	 * separately or in conjunction with PCP to indicate frames
	 * eligible to be dropped in the presence of congestion.
	 */
	uint16_t vlan_id;
/*!< VLAN Identifier (VID): a 12-bit field specifying the VLAN
 * to which the frame belongs. The hexadecimal values
 * of 0x000 and 0xFFF are reserved.
 * All other values may be used as VLAN identifiers,allowing up
 * to 4,094 VLANs.
 */
};

/**
 * @brief	Function sets default VLAN Tag Control Information (TCI)
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Tag Control Information Configuration

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_tci(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_tci_cfg *cfg);

/**
 * @brief	Spanning Tree Protocol (STP) states
 *
 */
enum dpsw_stp_state {
	DPSW_STP_STATE_BLOCKING = 0, /*!< Blocking state */
	DPSW_STP_STATE_LISTENING, /*!< Listening state */
	DPSW_STP_STATE_LEARNING, /*!< Learning state */
	DPSW_STP_STATE_FORWARDING
/*!< Forwarding state */
};

/**
 * @brief	Spanning Tree Protocol (STP) Configuration
 *
 */
struct dpsw_stp_cfg {
	uint16_t vlan_id; /*!< VLAN ID STP state */
	enum dpsw_stp_state state; /*!< STP state */
};

/**
 * @brief	Function sets Spanning Tree Protocol (STP) state.
 The following STP states are supported -
 blocking, listening, learning, forwarding and disabled.
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		STP State configuration parameters

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_stp(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_stp_cfg *cfg);

/**
 * @brief	Types of frames to accept
 *
 */
enum dpsw_accepted_frames {
	DPSW_ADMIT_ALL = 0,
	/*!< The device will accept VALN tagged, untagged and
	 * priority tagged frames
	 **/
	DPSW_ADMIT_ONLY_VLAN_TAGGED,
	/*!< The device will discard untagged frames or
	 * Priority-Tagged frames received on this port.
	 */
	DPSW_ADMIT_ONLY_UNTAGGED
/*!< Untagged frames or Priority-Tagged frames received
 * on this port will be accepted and assigned to a VID
 * based on the PVID and VID Set for this port.
 */
};

/**
 * @brief	Types of frames to accept configuration
 *
 */
struct dpsw_accepted_frames_cfg {
	enum dpsw_accepted_frames type; /*!< Defines ingress accepted
	 frames */
	enum dpsw_action unaccept_act; /*!< When frame is not accepted,
	 it may be discarded or
	 redirected to control port
	 depending on this flag */
};

/**
 * @brief	When is admit_only_vlan_tagged- the device will discard untagged
 frames or Priority-Tagged frames received on this port.
 When admit_only_untagged- untagged frames or Priority-Tagged
 frames received on this port will be accepted and assigned
 to a VID based on the PVID and VID Set for this port.
 When admit_all - the device will accept VALN tagged, untagged
 and priority tagged frames.
 The default is admit_all
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Frame types configuration

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_accepted_frames(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_accepted_frames_cfg *cfg);

/**
 * @brief	When this is accept (FALSE), the device will discard incoming
 frames for VLANs that do not include this Port in its
 Member set. When accept (TRUE), the port will accept all
 incoming frames
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	accept_all	Accept or drop frames having different VLAN

 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_accept_all_vlan(struct dpsw *dpsw,
	uint16_t if_id,
	int accept_all);

/**
 * @brief	Counters types
 *
 */
enum dpsw_counter {
	DPSW_CNT_ING_FRAME, /*!< Ingress frame count */
	DPSW_CNT_ING_BYTE, /*!< Ingress byte count */
	DPSW_CNT_ING_FLTR_FRAME, /*!< Ingress filtered frame count */
	DPSW_CNT_ING_FRAME_DISCARD, /*!< Ingress frame discard count */
	DPSW_CNT_ING_MCAST_FRAME, /*!< Ingress multicast frame count */
	DPSW_CNT_ING_MCAST_BYTE, /*!< Ingress multicast byte count */
	DPSW_CNT_ING_BCAST_FRAME, /*!< Ingress broadcast frame count */
	DPSW_CNT_ING_BCAST_BYTES, /*!< Ingress broad bytes count */
	DPSW_CNT_EGR_FRAME, /*!< Egress frame count */
	DPSW_CNT_EGR_BYTE, /*!< Egress byte count */
	DPSW_CNT_EGR_FRAME_DISCARD
/*!< Egress frame discard counter */
};

/**
 * @brief	Functions obtains specific counter of particular interface
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	type		counter type
 *
 * @param[out]	counter	return value
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_counter(struct dpsw *dpsw,
	uint16_t if_id,
	enum dpsw_counter type,
	uint64_t *counter);

/**
 * @brief	Functions sets specific counter of particular interface
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	type		counter type
 *
 * @param[int]	counter	return value
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_counter(struct dpsw *dpsw,
	uint16_t if_id,
	enum dpsw_counter type,
	uint64_t counter);

/**
 * @brief	User priority
 *
 */
enum dpsw_user_priority {
	DPSW_UP_PCP = 0,
	/*!< Priority Code Point (PCP): a 3-bit field which
	 * refers to the IEEE 802.1p priority.
	 **/
	DPSW_UP_PCP_DEI,
	/*!< Priority Code Point (PCP) combined with
	 * Drop Eligible Indicator (DEI)
	 **/
	DPSW_UP_DSCP
/*!< Differentiated services Code Point (DSCP): 6 bit
 * field from IP header
 **/
};

/**
 * @brief	Mapping user priority into traffic class configuration
 *
 */
struct dpsw_tc_map_cfg {
	enum dpsw_user_priority user_priority; /*!< Source filed for user
	 priority regeneration */
	uint8_t regenerated_priority[DPSW_MAX_PRI]; /*!< The Regenerated User
	 Priority that the incoming
	 User Priority is mapped to
	 for this port **/
};

/**
 * @brief	Function is used for mapping variety of frame fields (DSCP, PCP)
 to Traffic Class. Traffic class is a number
 in the range from 0 to 7
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Traffic class mapping configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_map(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_tc_map_cfg *cfg);

/**
 * @brief	Filter type for frames to reflect
 *
 */
enum dpsw_reflection_filter {
	DPSW_REFLECTION_FILTER_INGRESS_ALL = 0,/*!< Reflect all frames */
	DPSW_REFLECTION_FILTER_INGRESS_VLAN
/*!< Reflect only frames belong to particular
 * VLAN defined by vid parameter **/
};

/**
 * @brief	Structure representing reflection information
 *
 */
struct dpsw_reflection_cfg {
	enum dpsw_reflection_filter filter; /*!< Filter type for
	 frames to reflect */
	uint16_t vlan_id;/*!< Vlan Id to reflect.
	 Valid only when filter type is
	 DPSW_INGRESS_VLAN **/
};

/**
 * @brief	Function identifies port to be reflected or mirrored
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Reflection configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */

int dpsw_if_add_reflection(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_reflection_cfg *cfg);

/**
 * @brief	Function removes port to be reflected or mirrored
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Reflection configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_remove_reflection(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_reflection_cfg *cfg);

/**
 * @brief	Metering and marking algorithms
 *
 */
enum metering_algo {
	DPSW_METERING_ALGO_RFC2698 = 0, /*!< RFC 2698 */
	DPSW_METERING_ALGO_RFC4115
/*!< RFC 4115 */
};

/**
 * @brief	Metering and marking modes
 *
 */
enum metering_mode {
	DPSW_METERING_MODE_COLOR_BLIND = 0, /*!< Color blind mode */
	DPSW_METERING_MODE_COLOR_AWARE
/*!< Color aware mode */
};

/**
 * @brief	Metering and marking configuration
 *
 */
struct dpsw_metering_cfg {
	enum metering_algo algo; /*!< Implementation based on
	 Metering and marking algorithm */
	uint32_t cir; /*!< Committed information rate (CIR)
	 in bits/s */
	uint32_t eir; /*!< Excess information rate (EIR)
	 in bits/s */
	uint32_t cbs; /*!< Committed burst size (CBS)
	 in bytes */
	uint32_t ebs; /*!< Excess bust size (EBS) in bytes */
	enum metering_mode mode; /*!< Color awareness mode */
};

/**
 * @brief	Function sets metering and marking algorithm (coloring)
 and provides corresponding parameters
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Metering and marking parameters
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_metering_marking(struct dpsw *dpsw,
	uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_metering_cfg *cfg);

/**
 * @brief	Structure representing tag Protocol identifier
 *
 */
struct dpsw_custom_tpid_cfg {
	uint16_t tpid; /*!< An additional tag protocol identifier */
};

/**
 * @brief	API Configures a distinct Ethernet type value (or TPID value) to
 indicate a VLAN tag in addition to the common
 TPID values 0x8100 and 0x88A8.
 Two additional TPID's are supported
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Tag Protocol identifier
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_add_custom_tpid(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_custom_tpid_cfg *cfg);

/**
 * @brief	Structure representing transmit rate configuration
 *
 */
struct dpsw_transmit_rate_cfg {
	uint64_t rate; /*!< Interface Transmit rate in bits per second */
};

/**
 * @brief	API sets interface transmit rate.
 The setting mechanism is the same for internal and
 external (physical) interfaces.
 The rate set explicitly in bits per second
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Transmit rate configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_transmit_rate(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_transmit_rate_cfg *cfg);

/**
 * @brief	Transmission selection algorithm
 *
 */
enum dpsw_bw_algo {
	DPSW_BW_ALGO_STRICT_PRIORITY = 0, /*!< strict priority */
	DPSW_BW_ALGO_CREDIT_BASED
/*!< credit based shaper */
};

/**
 * @brief	Structure representing class bandwidth configuration
 *
 */
struct dpsw_bandwidth_cfg {
	enum dpsw_bw_algo algo; /*!< Transmission selection
	 algorithm */
	uint8_t delta_bandwidth; /*!< A percentage of
	 the interface transmit
	 rate. This parameter applied
	 only when using credit-based
	 shaper algorithm otherwise best
	 effort algorithm is applied **/
};

/**
 * @brief	API sets a percentage of the interface transmit rate;
 this is the bandwidth that can be reserved for use by the queue
 associated with traffic class. A percentage is relevant
 only when credit based shaping algorithm is selected for
 traffic class otherwise best effort (strict priority)
 algorithm is in place
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		Traffic class bandwidth configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_bandwidth(struct dpsw *dpsw,
	uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_bandwidth_cfg *cfg);

/**
 * @brief	Enable Interface
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_enable(struct dpsw *dpsw, uint16_t if_id);

/**
 * @brief	Disable Interface
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_disable(struct dpsw *dpsw, uint16_t if_id);

/**
 * @brief	Structure representing congestion queue
 *
 */
struct dpsw_queue_congestion_cfg {
	uint32_t entrance_threshold; /*!< Entrance threshold */
	uint32_t exit_threshold; /*!< Exit threshold */
	uint64_t wr_addr; /*!< Address in GPP to write
	 Connection State Change
	 Notification Message */
};

/*!
 * @name PFC source trigger
 */
#define DPSW_PFC_TRIG_QUEUE_CNG		0x01	/*!< Trigger for PFC
						initiation is traffic class
						queue congestion */
#define DPSW_PFC_TRIG_BUFFER_DPL	0x02	/*!< Trigger for PFC initiation
						is buffer depletion */
/* @} */

/**
 * @brief	API is used to configure thresholds for traffic class queue
 congestion state and to establish Congestion State Change
 Notification Message (CSCNM) from Congestion Point (CP) to GPP
 trusted software This configuration is used to trigger PFC
 request or congestion notification if enabled
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	tc_id		Traffic class Identifier
 *
 * @param[in]	cfg		Queue congestion configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_queue_congestion(struct dpsw *dpsw,
	uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_queue_congestion_cfg *cfg);

/**
 * @brief	Structure representing Priority Flow Control (PFC)
 *		configuration
 *
 */
struct dpsw_pfc_cfg {
	int receiver; /*!< Enable/Disable PFC receiver.
	 PFC receiver is responsible for accepting
	 PFC PAUSE messages and pausing transmission
	 for indicated in message PFC quanta */
	int initiator; /*!< Enable/Disable PFC initiator.
	 PFC initiator is responsible for sending
	 PFC request message when congestion has been
	 detected on specified TC queue  */
	uint32_t initiator_trig; /*!< Bitmap defining Trigger source or sources
	 for sending PFC request message out.  */
	uint16_t pause_quanta; /*!< Pause Quanta to indicate in PFC request
	 message the amount of quanta time to pause */
};

/**
 * @brief	Handles Priority Flow Control (PFC) configuration per
 Traffic Class (TC)
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	tc_id		Traffic class Identifier
 *
 * @param[in]	cfg		PFC configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_pfc(struct dpsw *dpsw,
	uint16_t if_id,
	uint8_t tc_id,
	struct dpsw_pfc_cfg *cfg);

/**
 * @brief	Structure representing Congestion Notification (CN)
 *		configuration
 *
 */
struct dpsw_cn_cfg {
	int enable; /*!< enable/disable Congestion State Change Notification
	 Message (CSCNM) from Congestion Point (CP) to GPP
	 trusted software */
};

/**
 * @brief	API is used to enable/disable Congestion State Change
 Notification Message (CSCNM) from Congestion Point (CP)
 to GPP trusted software
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	tc_id		Traffic class Identifier
 *
 * @param[in]	cfg		Congestion notification description
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_tc_set_cn(struct dpsw *dpsw,
	uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_cn_cfg *cfg);

/**
 * @brief	Structure representing L2 Switch interface attributes
 *
 */
struct dpsw_if_attr {
	int external; /*!< Internal or external Interface */
	int control; /*!< Control/Data Interface */
	int iop_id; /*!< WRIOP block identifier */
	uint8_t num_tcs; /*!< Number of traffic classes */
	int phys_if_id; /*!< Physical port ID */
	uint64_t rate; /*!< TX rate in bits per second */
	uint64_t options; /*!< features (bitmap) */
};

/**
 * @brief	Function obtains attributes of interface
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[out]	attr		Interface attributes
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_attributes(struct dpsw *dpsw,
	uint16_t if_id,
	struct dpsw_if_attr *attr);

/**
 * @brief	Cipher Suite
 *
 */
enum dpsw_cipher_suite {
	DPSW_MACSEC_GCM_AES_128 = 0, /*!< 128 bit */
	DPSW_MACSEC_GCM_AES_256
/*!< 256 bit */
};

/**
 * @brief	MACSec Configuration
 *
 */
struct dpsw_macsec_cfg {
	int enable; /*!< Enable MACSec */
	uint64_t sci; /*!< Secure Channel ID */
	enum dpsw_cipher_suite cipher_suite; /*!< Cipher Suite */
};

/**
 * @brief	Set MACSec configuration for physical port.
 Only point to point MACSec is supported
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	if_id		Interface Identifier
 *
 * @param[in]	cfg		MACSec configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
dpsw_if_set_macsec(struct dpsw *dpsw,
	uint16_t if_id,
	const struct dpsw_macsec_cfg *cfg);

/**
 * @brief	VLAN Configuration
 *
 */
struct dpsw_vlan_cfg {
	uint16_t fdb_id; /*!< Forwarding Data base */
};

/**
 * @brief	Adds VLAN to switch
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @param[in]	cfg		VLAN configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add(struct dpsw *dpsw,
	uint16_t vlan_id,
	const struct dpsw_vlan_cfg *cfg);

/**
 * @brief	Set of VLAN Interfaces
 *
 */
struct dpsw_vlan_if_cfg {
	uint16_t num_ifs; /*!< The number of ports that are
	 permanently assigned to the egress
	 list for this VLAN */
	uint16_t if_id[DPSW_MAX_IF]; /*!< The set of ports that are
	 permanently assigned to the egress
	 list for this VLAN */
};

/**
 * @brief	Adds set of interfaces to specified VLAN
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @param[in]	cfg		Set of interfaces to add
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if(struct dpsw *dpsw,
	uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg);

/**
 * @brief	Adds set of interfaces that should be
 *		transmitted as untagged
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @param[in]	cfg		set of interfaces that should be
 *				transmitted as untagged
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if_untagged(struct dpsw *dpsw,
	uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg);

/**
 * @brief	Add a Set of interfaces for flooding
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @param[in]	cfg		set of interfaces that should be
 *				used for flooding
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if_flooding(struct dpsw *dpsw,
	uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg);

/**
 * @brief	removes set of interfaces from specified VLAN
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @param[in]	cfg		set of interfaces that should be removed
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if(struct dpsw *dpsw,
	uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg);

/**
 * @brief	removes set of interfaces transmitting frames as un-tagged
 *		from specified VLAN
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @param[in]	cfg		set of interfaces that should be removed
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if_untagged(struct dpsw *dpsw,
	uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg);

/**
 * @brief	removes set of flooding interfaces from specified VLAN
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @param[in]	cfg		set of interfaces used for flooding
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if_flooding(struct dpsw *dpsw,
	uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg);

/**
 * @brief	removes specified VLAN
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	vlan_id		VLAN Identifier
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove(struct dpsw *dpsw, uint16_t vlan_id);

/**
 * @brief	FDB Configuration
 *
 */
struct dpsw_fdb_cfg {
	uint16_t num_fdb_entries; /*!< Number of FDB entries */
	uint16_t fdb_aging_time; /*!< Aging time in seconds */
	uint16_t num_fdb_static_entries;/*!< number of static entries */
};

/**
 * @brief	adds FDB to switch and Returns handle to FDB table for
 *		the reference
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[out]	fdb_id		Forwarding Database Identifier
 *
 * @param[out]	cfg		FDB Configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add(struct dpsw *dpsw,
	uint16_t *fdb_id,
	const struct dpsw_fdb_cfg *cfg);

/**
 * @brief	removes FDB from switch
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove(struct dpsw *dpsw, uint16_t fdb_id);

/**
 * @brief	FDB Entry type - Static/Dynamic
 *
 */
enum dpsw_fdb_entry_type {
	DPSW_FDB_ENTRY_STATIC = 0, /*!< Static entry */
	DPSW_FDB_ENTRY_DINAMIC
/*!< Dinamic entry */
};

/**
 * @brief	Structure representing uni-cast entry configuration
 *
 */
struct dpsw_fdb_unicast_cfg {
	enum dpsw_fdb_entry_type type; /*!< Static or Dinamic */
	uint8_t eth_addr[6]; /*!< MAC address */
	uint16_t if_egress; /*!< Egress interface ID */
};

/**
 * @brief	Function adds an unicast entry into MAC lookup table
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @param[in]	cfg		unicast entry configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_unicast(struct dpsw *dpsw,
	uint16_t fdb_id,
	const struct dpsw_fdb_unicast_cfg *cfg);

/**
 * @brief	removes an entry from MAC lookup table
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @param[in]	cfg		unicast entry configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_unicast(struct dpsw *dpsw,
	uint16_t fdb_id,
	const struct dpsw_fdb_unicast_cfg *cfg);

/**
 * @brief	Structure representing multi-cast entry configuration
 *
 */
struct dpsw_fdb_multicast_cfg {
	enum dpsw_fdb_entry_type type; /*!< Static or Dinamic */
	uint8_t eth_addr[6]; /*!< MAC Address */
	uint16_t num_ifs; /*!< Number of external and internal interfaces */
	uint16_t if_id[DPSW_MAX_IF]; /*!< Egress interface IDs */
};

/**
 * @brief	adds multicast entry to MAC lookup table
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @param[in]	cfg		multicast entry configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_multicast(struct dpsw *dpsw,
	uint16_t fdb_id,
	const struct dpsw_fdb_multicast_cfg *cfg);

/**
 * @brief	removes multicast entry to MAC lookup table
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @param[in]	cfg		multicast entry configuration
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_multicast(struct dpsw *dpsw,
	uint16_t fdb_id,
	const struct dpsw_fdb_multicast_cfg *cfg);

/**
 * @brief	Auto-learning modes
 *
 */
enum dpsw_fdb_learning_mode {
	DPSW_FDB_LEARNING_MODE_DIS = 0, /*!< Disable Auto-learning */
	DPSW_FDB_LEARNING_MODE_HW, /*!< Enable HW auto-Learning */
	DPSW_FDB_LEARNING_MODE_NON_SECURE, /*!< Enable None secure learning
	 by CPU */
	DPSW_FDB_LEARNING_MODE_SECURE
/*!< Enable secure learning
 by CPU */
};

/**
 * @brief        defines FDB learning mode
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @param[in]	mode		learning mode
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_set_learning_mode(struct dpsw *dpsw,
	uint16_t fdb_id,
	enum dpsw_fdb_learning_mode mode);

/**
 * @brief	FDB Attributes
 *
 */
struct dpsw_fdb_attr {
	uint16_t num_fdb_entries; /*!< Number of FDB entries */
	uint16_t fdb_aging_time; /*!< Aging time in seconds */
	uint16_t num_fdb_static_entries;/*!< number of static entries */
};

/**
 * @brief        obtains FDB attributes
 *
 * @param[in]	dpsw		L2 switch handle
 *
 * @param[in]	fdb_id		Forwarding Database Identifier
 *
 * @param[out]	attr		FDB attributes
 *
 * @returns      Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_get_attributes(struct dpsw *dpsw,
	uint16_t fdb_id,
	struct dpsw_fdb_attr *attr);

int dpsw_get_irq(struct dpsw *dpsw,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val);

int dpsw_set_irq(struct dpsw *dpsw,
                 uint8_t irq_index,
                 uint64_t irq_paddr,
                 uint32_t irq_val);

int dpsw_set_irq_enable(struct dpsw *dpsw,
                          uint8_t irq_index,
                          uint8_t enable_state);

int dpsw_get_irq_enable(struct dpsw *dpsw,
                          uint8_t irq_index,
                          uint8_t *enable_state);

int dpsw_set_irq_mask(struct dpsw *dpsw,
                        uint8_t irq_index,
                        uint32_t mask);

int dpsw_get_irq_mask(struct dpsw *dpsw,
                        uint8_t irq_index,
                        uint32_t *mask);

int dpsw_get_irq_status(struct dpsw *dpsw,
                         uint8_t irq_index,
                         uint32_t *status);

int dpsw_clear_irq_status(struct dpsw *dpsw,
                            uint8_t irq_index,
                            uint32_t status);

/*! @} */

#endif /* __FSL_DPSW_H */
