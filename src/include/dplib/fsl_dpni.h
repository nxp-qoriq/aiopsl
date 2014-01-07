/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dpni.h
 *  @brief   Data Path Network Interface API
 */
#ifndef __FSL_DPNI_H
#define __FSL_DPNI_H


#include "common/types.h"
#include "net/fsl_net.h"
#include "dplib/fsl_ldpaa.h"
#include "common/fsl_cmdif.h"

//TODO #include "dplib/fsl_dpkg.h"


/**************************************************************************//**
 @Group		grp_dpni	Data Path Network Interface API

 @Description	Contains initialization APIs and runtime control APIs for DPNI

 @{
*//***************************************************************************/

#ifdef MC
struct dpni;
#else
struct dpni {
	struct cmdif_desc cidesc;
};
#endif

/**************************************************************************//**
 @Collection	General DPNI macros
 @{
*//***************************************************************************/
#define DPNI_MAX_NUM_OF_TC			8
				/**< Max number of traffic classes */
#define DPNI_MAX_DIST_SIZE			16
				/**< Maximum dist size */
#define DPNI_MAX_NUM_OF_UNICAST_FILTERS		16
				/**< Maximum number of unicast filters */
#define DPNI_MAX_NUM_OF_MULTICAST_FILTERS	64
				/**< Maximum number of multicast filters */
#define DPNI_MAX_NUM_OF_VLAN_FILTERS		16
				/**< Maximum number of VLAN filters */
#define DPNI_MAX_NUM_OF_QOS_ENTRIES		64
				/**< Maximum number of QoS entries */
#define DPNI_MAX_DIST_KEY_SIZE			124
				/**< Maximum key size for the distribution */
//TODO - should use TLU define
#define DPNI_MAX_QOS_KEY_SIZE			124
				/**< Maximum key size for the QoS look-up */
//TODO - should use TLU define
/* @} */


/**************************************************************************//**
 @Collection	DPNI configuration options
 @{
*//***************************************************************************/
#define DPNI_CFG_OPT_CHKSM_GEN_DISABLED		0x00000001
			/**< checksum generation disable  */
#define DPNI_CFG_OPT_CHKSM_VALID_DISABLED	0x00000002
			/**< checksum validation disable */
#define DPNI_CFG_OPT_TX_CONF_DISABLED		0x00000004
			/**< tx-confirmation disable */
#define DPNI_CFG_OPT_PASS_RX_TIMESTAMP		0x00000008
			/**< get time-stamp on rx */
#define DPNI_CFG_OPT_DIST_HASH			0x00000010
			/**< hash based distribution support */
#define DPNI_CFG_OPT_DIST_FS			0x00000020
			/**< flow-steering based distribution support */
#define DPNI_CFG_OPT_POLICING			0x00000040
			/**< policing support */
#define DPNI_CFG_OPT_UNICAST_FILTER		0x00000080
			/**< unicast filtering support */
#define DPNI_CFG_OPT_MULTICAST_FILTER		0x00000100
			/**< multicast filtering support */
#define DPNI_CFG_OPT_VLAN_FILTER		0x00000200
			/**< vlan filtering support */
#define DPNI_CFG_OPT_MACSEC			0x00000400
			/**< MACSEC support */
#define DPNI_CFG_OPT_IPR			0x00000800
			/**< IP-reassembly support */
#define DPNI_CFG_OPT_IPF			0x00001000
			/**< IP-fragmentation support */
#define DPNI_CFG_OPT_RSC			0x00002000
			/**< RSC support */
#define DPNI_CFG_OPT_GSO			0x00004000
			/**< GSO support */
#define DPNI_CFG_OPT_IPSEC			0x00008000
			/**< IPSec transport support */
#define DPNI_CFG_OPT_WFQ			0x00010000
			/**< WFQ support */
/* @} */

/**************************************************************************//**
 @Description	DPNI types
*//***************************************************************************/
enum dpni_type {
	DPNI_TYPE_NI = 0,	/**< DPNI of type NI */
	DPNI_TYPE_NIC		/**< DPNI of type NIC */
};

/**************************************************************************//**
 @Description	Structure representing DPNI default parameters
*//***************************************************************************/
struct dpni_cfg {
	uint32_t	options;
		/**< Mask of available options */
	uint8_t		max_tcs;
		/**< maximum number of traffic-classes;
		     will affect both Tx & Rx; '0' will e treated as '1' */
	uint8_t		max_dist_per_tc[DPNI_MAX_NUM_OF_TC];
		/**< maximum distribution's size per Rx's traffic-class */
	int		dist_key_variety;
		/**< if dist-key can be different between the TCs */
	uint8_t		max_unicast_filters;
		/**< maximum number of unicast filters; '0' will be treated
		     as 'DPNI_MAX_NUM_OF_UNICAST_FILTERS' */
	uint8_t		max_multicast_filters;
		/**< maximum number of multicast filters; '0' will be treated
		     as 'DPNI_MAX_NUM_OF_MULTICAST_FILTERS' */
	uint8_t		max_vlan_filters;
		/**< maximum number of vlan filters; '0' will be treated
		     as 'DPNI_MAX_NUM_OF_VLAN_FILTERS' */
	uint8_t		max_qos_entries;
		/**< if 'max_tcs>1', declare the maximum entries for the
		     QoS table; '0' will be treated as
		     'DPNI_MAX_NUM_OF_QOS_ENTRIES' */
	uint16_t	max_qos_key_size;
		/**< maximum key size for the QoS look-up */
//	TODO - default?
	uint16_t	max_dist_key_size;
		/**< maximum key size for the distribution */
//	TODO - default? */
};

/**************************************************************************//**
 @Description	Structure representing DPNI initialization parameters
*//***************************************************************************/
struct dpni_init_params {
	enum dpni_type	type;	/**< DPNI Type */
	uint8_t		mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
				/**< Primary mac address */
	uint8_t		max_dpio_objs;
		/**< maximum number of dpio objects that will be associated
		     with this object */
// TODO - 'max_dpio_objs' & DPNI_CFG_OPT_WFQ will affect the channels allocation
#ifdef MC
	uint16_t	id;		/**< DPNI id */
	void		*qbman;		/**< Handle to QBMan */
	void		*device;	/**< Handle to resman-device  */
	void		*linkman;	/**< Handle to Linkman  */
	void		*secdcl;	/**< Handle to Sec-Dcl */
	void		*macsec;	/**< Handle to Macsec */
#endif /* MC */

};

/**************************************************************************//**
 @Function	dpni_defconfig

 @Description	Fill DPNI configuration structure with default parameters.
		User may then select to change these default values according
		to its needs.

 @Param[out]	cfg - Pointer to configuration parameters structure to fill

 @Return	'0' on Success; error code otherwise.
*//***************************************************************************/
int dpni_defconfig(struct dpni_cfg *cfg);

/**************************************************************************//**
 @Function	dpni_init

 @Description	Will allocate resources and preliminary initialization

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	cfg - Default configuration parameters
 @Param[in]	params - Initialization Parameters

 @Return	'0' on Success; Error code otherwise.

 @Cautions	This function should be called only once during
 	 	lifetime of the object
*//***************************************************************************/
int dpni_init(struct dpni			*dpni,
              const struct dpni_cfg		*cfg,
              const struct dpni_init_params	*params);

#ifdef MC
/**************************************************************************//**
 @Function	dpni_allocate

 @Description	TODO

 @Return	allocated memory for the object.
*//***************************************************************************/
struct dpni *dpni_allocate(void);
#endif /* MC */

/**************************************************************************//**
 @Function	dpni_free

 @Description	Free the DPNI object and all its resources.

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; error code otherwise.


*//***************************************************************************/
int dpni_free(struct dpni *dpni);

/**************************************************************************//**
 @Collection	DPNI Tx Queue modification options
 @{
*//***************************************************************************/
#define DPNI_TX_Q_MOD_OPT_TX_CONF		0x00000001
	/**< Modify the queue settings for dedicate tx-confirmation queue */
#define DPNI_TX_Q_MOD_OPT_TX_CONF_DPIO		0x00000002
	/**< Modify the tx-confirmation queue dpio parameters*/
#define DPNI_TX_Q_MOD_OPT_TX_CONF_DAN		0x00000004
	/**< Modify the tx-confirmation queue DAN attribute*/
#define DPNI_TX_Q_MOD_OPT_DEPTH_LIMIT		0x00000008
	/**< Modify the queue depth-limit */
#define DPNI_TX_Q_MOD_OPT_CKSUM_GEN		0x00000010
	/**< Modify the queue checksum generation */
#define DPNI_TX_Q_MOD_OPT_REJECTIONS		0x00000020
	/**< Modify the rejection handling */
/* @} */

/**************************************************************************//**
 @Collection	DPNI Rx flow modification options
 @{
*//***************************************************************************/
#define DPNI_RX_FLOW_MOD_OPT_USER_CTX		0x00000001
	/**< Modify the user's context parameters */
//#define DPNI_RX_FLOW_MOD_OPT_FLC		0x00000002
	/**< Modify the flow-context (e.g. stashing) parameters */
#define DPNI_RX_FLOW_MOD_OPT_DAN		0x00000004
	/**< Modify the DAN attribute */
/* @} */

/**************************************************************************//**
 @Collection	DPNI Rx-Flow IO modification options
 @{
*//***************************************************************************/
#define DPNI_RX_FLOW_IO_MOD_OPT_DPIO		0x00000001
	/**< Modify the flow dpio parameter */
/* @} */

/**************************************************************************//**
 @Description	Structure representing DPNI key params
*//***************************************************************************/
struct dpni_key_params {
	uint8_t		*key;	/**< A pointer to the key */
	uint8_t		*mask;	/**< A pointer to the mask */
};

/**************************************************************************//**
 @Description	DPNI link types
*//***************************************************************************/
enum dpni_link_type {
	DPNI_LINK_TYPE_DPMAC = 0,	/**< Attached to DPMAC */
	DPNI_LINK_TYPE_DPSW,		/**< Attached to DPSW */
	DPNI_LINK_TYPE_DPDMUX,		/**< Attached to DPDMUX */
	DPNI_LINK_TYPE_DPLAG,		/**< Attached to DPLAG */
	DPNI_LINK_TYPE_DPNI,		/**< Attached to DPNI */
	DPNI_LINK_TYPE_AIOP		/**< Attached to AIOP */
};

/**************************************************************************//**
 @Description	Structure representing DPNI attach parameters
*//***************************************************************************/
struct dpni_attach_params {
#ifdef MC
	void			*dpio;		/**< Pointer to DPIO object */
	void			*dpsp;		/**< Pointer to DPSP object */
#else /* !MC */
	uint16_t		dpio_id;	/**< DPIO object id */
	uint16_t		dpsp_id;	/**< DPSP object id */
#endif /* MC */
//TODO - add struct ldpaa_flow_ctx	*flc; /**< valid only in case of flow-steering */
	int			dan_en;
		/**< FQ data availability notification;
		     default for all RX/TX-conf queues */
	uint64_t		rx_user_ctx;
		/**< User context; will be received with the FD in case of Rx
		     frame; can be override by calling
		     'dpni_set_rx_queue_ctx' */
	uint64_t		rx_err_user_ctx;
		/**< User context; will be received with the FD in case of Rx
		     error frame */
	uint64_t		tx_err_user_ctx;
		/**< User context; will be received with the FD in case of Tx
		     error frame and 'DPNI_CFG_OPT_TX_CONF_DISABLED' is set.
		     if not set, tx-error frames will received with
		     'tx_conf_user_ctx' */
	uint64_t		tx_conf_user_ctx;
		/**< User context; will be received with the FD in case of Tx
		     confirmation frame; can be override by calling
		     'dpni_set_tx_queue_ctx' */
};

/**************************************************************************//**
 @Description	Structure representing DPNI attach link parameters
*//***************************************************************************/
struct dpni_attach_link_params {
	enum dpni_link_type link_type;	/**< Linked object type */
	union {
		struct {
#ifdef MC
			void		*dpmac;	/**< TODO */
#else /* !MC */
			uint16_t	dpmac_id;/**< DPMAC object id */
#endif /* MC */
		} dpmac;
		struct {
#ifdef MC
			void		*dpsw;	/**< TODO */
#else /* !MC */
			uint16_t	dpsw_id;/**< DPSW object id */
#endif /* MC */
			uint8_t		if_id;
				/**< DPSW interface-id to be linked to */
		} dpsw;
		struct {
#ifdef MC
			void		*dpdmux;/**< TODO */
#else /* !MC */
			uint16_t	dpdmux_id;/**< DPDMUX object id */
#endif /* MC */
			uint8_t		if_id;
				/**< DPDMUX interface-id to be linked to */
		} dpdmux;
		struct {
#ifdef MC
			void		*dplag;/**< TODO */
#else /* !MC */
			uint16_t	dplag_id;/**< DPLAG object id */
#endif /* MC */
		} dplag;
		struct {
			int tmp;
		} aiop;
		struct {
#ifdef MC
			void		*dpni;/**< TODO */
#else /* !MC */
			uint16_t	dpni_id;/**< DPNI object id */
#endif /* MC */
		} dpni;
	} u;
};

/**************************************************************************//**
 @Description	Structure representing DPNI TX TC configuration
*//***************************************************************************/
struct dpni_tx_tc_cfg {
	int tmp;	/**< TODO */
};

/**************************************************************************//**
 @Description	Structure representing DPNI TX Queue configuration
*//***************************************************************************/
struct dpni_tx_queue_cfg {
	uint32_t	options;
        /**< the flags that represent the modification that are required to be
             done for the tx-queue */
	int 		tx_conf;
        /**< This option maybe used when 'options' set
             with DPNI_TX_Q_MOD_OPT_TX_CONF; Prefer this queue to
             have its private tx-confirmation settings */
#ifdef MC
	void		*tx_conf_dpio;	/**< TODO */
#else /* !MC */
	uint16_t	tx_conf_dpio_id;
        /**< This option maybe used when 'options' set
             with DPNI_TX_Q_MOD_OPT_TX_CONF_DPIO and this queue was set
             with private tx-confirmation settings;
             If dpio configured to cdan-support than use the
	     "cdan" channel else use the first channel;
	     In any case, will use highest priority within the channel */
#endif /* MC */
	int 		tx_conf_dan_en;
        /**< This option maybe used when 'options' set
             with DPNI_TX_Q_MOD_OPT_TX_CONF_DAN and this queue was set
             with private tx-confirmation settings; enable/disable of
	     data availability notification */
	uint16_t	depth_limit;
        /**< This option maybe used when 'options' set
             with DPNI_TX_Q_MOD_OPT_DEPTH_LIMIT; if >0 than limit
             the depth of this queue which may result with rejected frames */
//TODO - support both bytes & frames??? if not what we prefer??
	int		fetch_rejections;
        /**< This option maybe used when 'options' set
             with DPNI_TX_Q_MOD_OPT_REJECTIONS; enable/disable send
             enqueue-rejections back to software */
	uint64_t	user_ctx;
        /**< This option maybe used when 'options' set
             with DPNI_TX_Q_MOD_OPT_USER_CTX; will be provided in case
             of tx-confirmation or enqueue-reject condition ("lossless") */
	int		chksum_gen;
        /**< This option maybe used when 'options' set
             with DPNI_TX_Q_MOD_OPT_CKSUM_GEN; enable/disable checksum
             generation */
};

/**************************************************************************//**
 @Description	Structure representing DPNI RX TC configuration
*//***************************************************************************/
struct dpni_rx_tc_cfg {
	uint8_t dist_size;	/**< TODO */
//	struct policing_params *params;
//TODO - add struct ldpaa_flow_ctx	*flc; /**< valid only in case of flow-steering */
};

/**************************************************************************//**
 @Description	Structure representing DPNI RX flow configuration
*//***************************************************************************/
struct dpni_rx_flow_cfg {
	uint32_t	options;
        /**< the flags that represent the modification that are required to be
             done for the rx-flow */
	uint64_t	user_ctx;
        /**< This option maybe used when 'options' set
             with DPNI_RX_FLOW_MOD_OPT_USER_CTX; will be provided
             with each rx frame */
//TODO - add struct ldpaa_flow_ctx	*flc; /**< valid only in case of flow-steering */
	int		dan_en;
        /**< This option maybe used when 'options' set
             with DPNI_RX_FLOW_MOD_OPT_DAN; enable/disable of
	     data availability notification */
};

/**************************************************************************//**
 @Description	Structure representing DPNI RX Flow IO's parameters
*//***************************************************************************/
struct dpni_rx_flow_io {
	uint32_t	options;
        /**< the flags that represent the modification that are required to be
             done for the rx-flow-io */
#ifdef MC
	void		*dpio;	/**< TODO */
#else /* !MC */
	uint16_t	dpio_id;
        /**< This option maybe used when 'options' set
             with DPNI_RX_FLOW_IO_MOD_OPT_DPIO; DPIO object id */
#endif /* MC */
};

/**************************************************************************//**
 @Description	Structure representing attributes parameters
*//***************************************************************************/
struct dpni_attributes
{
	uint64_t	options;
	uint8_t		max_tcs;
	uint8_t		max_dist_per_tc[DPNI_MAX_NUM_OF_TC];
};

/**************************************************************************//**
 @Description	Structure representing statistic parameters
*//***************************************************************************/
struct dpni_stats
{
	uint64_t	egr_frame_cnt;
		/**< egress frame counter */
	uint64_t	egr_byte_cnt;
		/**< egress byte counter */
	uint64_t	egr_frame_discard_cnt;
		/**< egress frame discard counter due to errors */
	uint64_t	ing_frame_cnt;
	 	 /**< ingress frame counter */
	uint64_t	ing_byte_cnt;
	 	 /**< ingress frame counter */
	uint64_t	ing_dropped_frame_cnt;
	 	 /**< ingress frame dropped counter due to
	 	      explicit 'drop' setting*/
	uint64_t	ing_frame_discard_cnt;
	 	 /**< ingress frame discarded counter due to errors*/
	uint64_t	ing_mcast_frame_cnt;
		/**< ingress multicast frame counter */
	uint64_t	ing_mcast_byte_cnt;
		/**< ingress multicast byte counter */
	uint64_t	ing_bcast_frame_cnt;
		/**< ingress broadcast frame counter */
	uint64_t	ing_bcast_bytes_cnt;
		/**< ingress broadcast byte counter */
};

/**************************************************************************//**
 @Description	Structure representing QOS table parameters
*//***************************************************************************/
struct dpni_qos_tbl_params
{
	struct dpkg_profile_params	*extract_params;
		/**< define the extractions to be used as the QoS criteria */
	int 				drop_frame;
		/**< '1' for dropping the frame in case of no match;
		     '0' for using the 'default_tc' */
	uint8_t				default_tc;
		/**< will be used in case of no-match and 'drop_frame'=0 */
};

/**************************************************************************//**
 @Description	Structure representing distribution parameters
*//***************************************************************************/
struct dpni_dist_params
{
	int				dist_fs;
		/**< '1' for distribution based on flow-steering;
		     '0' for hash based */
	struct dpkg_profile_params	*extract_params;
	/**< define the extractions to be used for the distribution key */
};

/**************************************************************************//**
 @Description	Structure representing FS table parameters
*//***************************************************************************/
struct dpni_fs_tbl_params
{
	int		drop_frame;
		/**< '1' for dropping the frame in case of no match;
		     '0' for using the 'default_flowid' */
	uint8_t		default_flowid;
		/**< will be used in case of no-match and 'drop_frame'=0 */
};


/**************************************************************************//**
 @Function	dpni_open

 @Description	Open object handle - Required before any operation on the object

 @Param[in]	portal_vaddr - Pointer to MC portal registers
 @Param[in]	dpni_id - DPNI unique ID

 @Return	Pointer to the DPNI object, to be used in subsequent calls


*//***************************************************************************/
//struct dpni *dpni_open(void *portal_vaddr, int dpni_id);
int dpni_open(struct dpni *dpni, int id);

/**************************************************************************//**
 @Function	dpni_close

 @Description	Closes the object handle, no further operations on the object
		are allowed

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_close(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_irq

 @Description	Sets IRQ information for the dpni object
 	 	(required for DPNI to signal events).

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	irq_index - identifies the specific IRQ to configure
 @Param[in]	irq_paddr - physical IRQ address that must be written
 	 	 	    to signal the interrupt
 @Param[in]	irq_val - IRQ value to write into the IRQ address

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_irq(struct dpni	*dpni,
                 uint8_t	irq_index,
                 uint64_t	irq_paddr,
                 uint32_t	irq_val);

/**************************************************************************//**
 @Function	dpni_attach

 @Description	Attach the NI to application

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	params - Attach Parameters

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_attach(struct dpni			*dpni,
                const struct dpni_attach_params	*params);

/**************************************************************************//**
 @Function	dpni_detach

 @Description	Detach the NI from application

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_detach(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_attach_link

 @Description	Attach the NI to the link side

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	params - Link attachment Parameters

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_attach_link(struct dpni				*dpni,
                     const struct dpni_attach_link_params	*params);

/**************************************************************************//**
 @Function	dpni_detach_link

 @Description	Detach the NI from the link side

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_detach_link(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_enable

 @Description	Enable the NI, will allow sending and receiving frames.

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_enable(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_disable

 @Description	Disable the NI, will disallow sending and receiving frames.

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_disable(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_get_attributes

 @Description	Retrieve the object's attributes.

 @Param[in]	dpni - Pointer to dpni object
 @Param[out]	attributes - Object's attributes

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_get_attributes(struct dpni		*dpni,
                        struct dpni_attributes	*attributes);

/**************************************************************************//**
 @Function	dpni_get_qdid

 @Description	Get the QDID used for enqueue

 @Param[in]	dpni - Pointer to dpni object
 @Param[out]	qdid - Qdid used for qneueue

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_get_qdid(struct dpni	*dpni,
                  uint16_t	*qdid);

/**************************************************************************//**
 @Function	dpni_get_tx_data_offset

 @Description	Get the tx data offset

 @Param[in]	dpni - Pointer to dpni object
 @Param[out]	data_offset - TX data offset

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_get_tx_data_offset(struct dpni	*dpni,
                            uint16_t	*data_offset);

/**************************************************************************//**
 @Function	dpni_get_stats

 @Description	Retrieve the statistics

 @Param[in]	dpni - Pointer to dpni object
 @Param[out]	stats - Statics

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_get_stats(struct dpni		*dpni,
                   struct dpni_stats	*stats);

/**************************************************************************//**
 @Function	dpni_reset_stats

 @Description	Reset the statistics.

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_reset_stats(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_get_link_state

 @Description	TODO

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_get_link_state(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_mfl

 @Description	Set the maximum received frame length.

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	mfl - MFL length

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_mfl(struct dpni	*dpni,
                 uint16_t	mfl);

/**************************************************************************//**
 @Function	dpni_get_mfl

 @Description	Get the maximum received frame length.

 @Param[in]	dpni - Pointer to dpni object
 @Param[out]	mfl - MFL length

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_get_mfl(struct dpni	*dpni,
                 uint16_t	*mfl);

/**************************************************************************//**
 @Function	dpni_set_mtu

 @Description	Set the MTU for this interface. Will have affect on IPF and
 	 	conditionally on GSO.

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	mtu - MTU length

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_mtu(struct dpni	*dpni,
                 uint16_t	mtu);

/**************************************************************************//**
 @Function	dpni_enable_multicast_promisc

 @Description	Enable/Disable multicast promiscuous mode

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	en - '1' for enabling/'0' for disabling

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_enable_multicast_promisc(struct dpni	*dpni,
                                  int		en);

/**************************************************************************//**
 @Function	dpni_set_primary_mac_addr

 @Description	Set the primary mac address

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	addr - MAC address

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_primary_mac_addr(
	struct dpni	*dpni,
	const uint8_t	addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
 @Function	dpni_get_primary_mac_addr

 @Description	Get the primary mac address

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	addr - MAC address

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_get_primary_mac_addr(
	struct dpni	*dpni,
	uint8_t		addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
 @Function	dpni_add_mac_addr

 @Description	Add unicast/multicast filter address

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	addr - MAC address

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_mac_addr(struct dpni 	*dpni,
                      const uint8_t	addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
 @Function	dpni_remove_mac_addr

 @Description	Remove unicast/multicast filter address

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	addr - MAC address

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_mac_addr(struct dpni	*dpni,
                         const uint8_t	addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
 @Function	dpni_clear_mac_table

 @Description	Clear the mac filter table

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_mac_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_add_vlan_id

 @Description	Add VLAN-id filter

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	vid - VLAN ID

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_vlan_id(struct dpni	*dpni,
                     uint16_t		vid);

/**************************************************************************//**
 @Function	dpni_remove_vlan_id

 @Description	Add VLAN-id filter

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	vid - VLAN ID

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_vlan_id(struct dpni	*dpni,
                        uint16_t	vid);

/**************************************************************************//**
 @Function	dpni_clear_vlan_table

 @Description	Clear the VLAN filter table

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_vlan_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_tx_tc

 @Description	Set TX TC settings

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tc_cfg - TC parameters

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_tx_tc(struct dpni			*dpni,
                   const struct dpni_tx_tc_cfg	*tc_cfg);

/**************************************************************************//**
 @Function	dpni_set_rx_tc

 @Description	Set RX TC settings

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tc_cfg - TC group parameters

 @Return	'0' on Success; error code otherwise.
*//***************************************************************************/
int dpni_set_rx_tc(struct dpni			*dpni,
                   const struct dpni_rx_tc_cfg	*tc_cfg);

/**************************************************************************//**
 @Function	dpni_set_tx_queue_cfg

 @Description	Set TX queue configuration

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id
 @Param[in]	params - Queue parameters

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_tx_queue_cfg(struct dpni				*dpni,
                          uint8_t				tcid,
                          const struct dpni_tx_queue_cfg	*params);

/**************************************************************************//**
 @Function	dpni_set_rx_flow_io

 @Description	Set RX flow IO configuration

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id
 @Param[in]	flowid - flow id within the traffic class
 @Param[in]	params - io parameters

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_rx_flow_io(struct dpni			*dpni,
                        uint8_t				tcid,
                        uint8_t				flowid,
                        const struct dpni_rx_flow_io	*params);

/**************************************************************************//**
 @Function	dpni_set_rx_flow_cfg

 @Description	Set RX flow configuration

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id
 @Param[in]	flowid - flow id within the traffic class
 @Param[in]	params - flow configuration parameters

 @Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_set_rx_flow_cfg(struct dpni			*dpni,
                          uint8_t			tcid,
                          uint8_t			flowid,
                          const struct dpni_rx_flow_cfg	*params);

/**************************************************************************//**
 @Function	dpni_set_qos_table

 @Description	Set QoS mapping table

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	params - QoS table parameters

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_set_qos_table(struct dpni 			*dpni,
                       const struct dpni_qos_tbl_params	*params);

/**************************************************************************//**
 @Function	dpni_delete_qos_table

 @Description	Delete QoS mapping table

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_delete_qos_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_add_qos_entry

 @Description	Add QoS mapping entry

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	key_params - QoS key parameters
 @Param[in]	tcid - Traffic class id

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_qos_entry(struct dpni 			*dpni,
                       const struct dpni_key_params	*key_params,
                       uint8_t 				tcid);

/**************************************************************************//**
 @Function	dpni_remove_qos_entry

 @Description	Remove QoS mapping entry

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	key_params - QoS key parameters

 @Return	'0' on Success; Error code otherwise.
j
 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_qos_entry(struct dpni			*dpni,
                          const struct dpni_key_params	*key_params);

/**************************************************************************//**
 @Function	dpni_clear_qos_table

 @Description	Clear all QoS mapping entries

 @Param[in]	dpni - Pointer to dpni object

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_qos_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_dist

 @Description	Set the distribution method and key

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	dist - distribution params

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_set_dist(struct dpni 			*dpni,
                  const struct dpni_dist_params	dist[DPNI_MAX_NUM_OF_TC]);

/**************************************************************************//**
 @Function	dpni_set_tc_fs_table

 @Description	Set FS mapping table for a specific traffic class

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id
 @Param[in]	params - FS table parameters

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_set_fs_table(struct dpni			*dpni,
                      uint8_t 				tcid,
                      const struct dpni_fs_tbl_params	*params);

/**************************************************************************//**
 @Function	dpni_delete_fs_table

 @Description	Delete FS mapping table

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_delete_fs_table(struct dpni	*dpni,
                         uint8_t	tcid);

/**************************************************************************//**
 @Function	dpni_add_fs_entry

 @Description	Add FS entry for a specific traffic-class

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id
 @Param[in]	key_params - Key parameters
 @Param[in]	flowid - Flow id

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_fs_entry(struct dpni			*dpni,
                      uint8_t 				tcid,
                      const struct dpni_key_params	*key_params,
                      uint16_t				flowid);

/**************************************************************************//**
 @Function	dpni_remove_fs_entry

 @Description	Remove FS entry from a specific traffic-class

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id
 @Param[in]	key_params - Key parameters

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_fs_entry(struct dpni 			*dpni,
                         uint8_t 			tcid,
                         const struct dpni_key_params	*key_params);

/**************************************************************************//**
 @Function	dpni_clear_fs_table

 @Description	Clear all FS entries

 @Param[in]	dpni - Pointer to dpni object
 @Param[in]	tcid - Traffic class id

 @Return	'0' on Success; Error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_fs_table(struct dpni	*dpni,
                        uint8_t		tcid);

/** @} */

#endif /* __FSL_DPNI_H */
