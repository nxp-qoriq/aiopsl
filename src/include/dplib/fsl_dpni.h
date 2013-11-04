/**************************************************************************//**
	Copyright 2013 Freescale Semiconductor, Inc.

 @File		  fsl_dpni.h

 @Description	This file contains LDPAA NI API.
*//***************************************************************************/
#ifndef __FSL_DPNI_H
#define __FSL_DPNI_H


#include "common/types.h"
#include "net/fsl_net.h"
#include "dplib/fsl_ldpaa.h"

/* TODO - should be removed and replaced by the real CTLU
 * extract_key structure */
struct extract_key {
	int tmp;
};

/* TODO - should be removed and replaced by the real CTLU
 * key_params structure */
struct key_params {
	int tmp;
};

/**************************************************************************//**
 @Collection	Command structures
 @{
*//***************************************************************************/
/*
struct cmdif_cmd {
	uint64_t header;
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};
*/
struct cmdif_dev {
	struct cmdif_cmd *regs;
	int auth_id;

};



/**************************************************************************//**
 @Group		ldpaa_g  LDPAA API

 @Description	TODO

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		dpni_g  LDPAA NI API

 @Description	TODO

 @{
*//***************************************************************************/

struct dpni;

/**************************************************************************//**
 @Collection	General DPNI defines
*//***************************************************************************/
#define DPNI_MAX_NUM_OF_TC			8
#define DPNI_MAX_DIST_SIZE			16
#define DPNI_MAX_NUM_OF_UNICAST_FILTERS		16
#define DPNI_MAX_NUM_OF_MULTICAST_FILTERS	64
#define DPNI_MAX_NUM_OF_VLAN_FILTERS		16
#define DPNI_MAX_NUM_OF_QOS_ENTRIES		64
#define DPNI_MAX_DIST_KEY_SIZE			124 /* should use TLU define */
#define DPNI_MAX_QOS_KEY_SIZE			124 /* should use TLU define */



/**************************************************************************//**
 @Collection	DPNI configuration options
*//***************************************************************************/
typedef uint32_t						dpni_cfg_opt_t;
		/**< a type to represent all dpni configuration options */

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
/* @} */

/**************************************************************************//**
 @Group		dpni_init_grp DPNI Initialization Unit

 @Description	DPNI Initialization Unit

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description	DPNI types
*//***************************************************************************/
enum dpni_type {
	DPNI_TYPE_NI = 0,
	DPNI_TYPE_NIC,
};

#ifdef MC
/**************************************************************************//**
 @Description	structure representing DPNI resource manager cbs
*//***************************************************************************/
struct dpni_resman_cbs {
	resource_allocate_cb		*allocate_cb;
	resource_deallocate_cb		*deallocate_cb;
};
#endif /* MC */

/**************************************************************************//**
 @Description	structure representing DPNI default parameters
*//***************************************************************************/
struct dpni_cfg {
	dpni_cfg_opt_t	options;	/**< TODO */
	uint8_t		max_tcs;	/**< TODO */
	uint8_t		max_dist_per_tc[DPNI_MAX_NUM_OF_TC];
			/**< TODO */
	int		dist_key_variety;
			/**< if dist-key can be different between the TCs */
	uint8_t		max_unicast_filters;
			/**< TODO */
	uint8_t		max_multicast_filters;
			/**< TODO */
	uint8_t		max_vlan_filters;
			/**< TODO */
	uint8_t		max_qos_entries;
			/**< TODO */
	uint16_t	max_qos_key_size;
			/**< TODO */
	uint16_t	max_dist_key_size;
			/**< TODO */
};

/**************************************************************************//**
 @Description	structure representing DPNI initialization parameters
*//***************************************************************************/
struct dpni_init_params {
	enum dpni_type	type;	/**< NI Type */
	uint8_t		mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
					/**< Primary mac address */
#ifdef MC
	void		*qman;		/**< Handle to QMAN */
	void		*resman_device;	/**< resman device identifier */
	struct dpni_resman_cbs	*resman_cbs;
	void		*secdcl;
	void		*macsec;
#endif /* MC */

};

/**************************************************************************//**
 @Function	dpni_defconfig

 @Description	Obtain default configuration of NI

 @Param[out]	cfg - default configuration parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_defconfig(struct dpni_cfg *cfg);

/**************************************************************************//**
 @Function	dpni_init

 @Description	will allocate resources and preliminary initialization

 @Param[in]	dpni - dpni handle
 @Param[in]	cfg - configuration parameters
 @Param[in]	params - Initialization Parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_init(struct dpni *dpni,
              struct dpni_cfg *cfg,
              struct dpni_init_params *params);

#ifdef MC
/**************************************************************************//**
 @Function	dpni_allocate

 @Description	TODO

 @Return	allocated memory for the object.

 @Cautions	None.
*//***************************************************************************/
struct dpni *dpni_allocate(void);
#endif /* MC */

/**************************************************************************//**
 @Function	dpni_free

 @Description	Frees all allocated resources

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_free(struct dpni *dpni);

/** @} */ /* end of dpni_init_grp group */

/**************************************************************************//**
 @Group		dpni_runtime_control_grp DPNI Runtime Control Unit

 @Description	DPNI Runtime control unit API functions, definitions and enums.
		The DPNI driver provides a set of control routines.
		These routines may only be called after the module was fully
		initialized (both configuration and initialization routines were
		called). They are typically used to get information from
		hardware (status, counters/statistics, revision etc.), to modify
		a current state or to force/enable a required action. Run-time
		control may be called whenever necessary and as many times
		as needed.
 @{
*//***************************************************************************/

/**************************************************************************//**
 @Collection	DPNI Tx Queue modification options
*//***************************************************************************/
typedef uint32_t			dpni_tx_q_mod_opt_t;
	/**< a type to represent all the tx-queue modification options */

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
/* @} */

/**************************************************************************//**
 @Collection	DPNI Rx Queue modification options
*//***************************************************************************/
typedef uint32_t			dpni_rx_q_mod_opt_t;
	/**< a type to represent all the rx-queue modification options */

#define DPNI_RX_Q_MOD_OPT_USER_CTX		0x00000001
	/**< Modify the queue user's context parameters */
#define DPNI_RX_Q_MOD_OPT_FLC			0x00000002
	/**< Modify the queue flow-context (e.g. stashing) parameters */
#define DPNI_RX_Q_MOD_OPT_DPIO			0x00000004
	/**< Modify the queue dpio parameter */
#define DPNI_RX_Q_MOD_OPT_DAN			0x00000008
	/**< Modify the queue DAN attribute */
/* @} */

/**************************************************************************//**
 @Description	DPNI link types
*//***************************************************************************/
enum dpni_link_type {
	DPNI_LINK_TYPE_PHYS = 0,
	DPNI_LINK_TYPE_L2SW,
	DPNI_LINK_TYPE_DEMUX,
	DPNI_LINK_TYPE_LAG,
	DPNI_LINK_TYPE_AIOP,
	DPNI_LINK_TYPE_DPNI
};

#ifdef MC
/**************************************************************************//**
 @Description	TODO

 @Param[in]	handle	TODO
 @Param[in]	if_id	TODO
 @Param[out]	params	TODO
 *//***************************************************************************/
typedef int (dpni_cb_attach_link) (void			*handle,
				   uint8_t		if_id,
				   void			*params);

/**************************************************************************//**
 @Description	TODO

 @Param[in]	handle	TODO
 @Param[in]	if_id	TODO
 *//***************************************************************************/
typedef int (dpni_cb_detach_link) (void			*handle,
				   uint8_t		if_id);
#endif /* MC */

/**************************************************************************//**
 @Description	structure representing DPNI attach parameters
*//***************************************************************************/
struct dpni_attach_params {
	void			*dpio;	/**< TODO */
	void			*dpsp;	/**< TODO */
	struct ldpaa_flow_ctx	*flc;
	int			dan_en;/**< FQ data availability notification;
					default for all RX/TX-conf queues */
	uint64_t		rx_user_ctx;
	uint64_t		rx_err_user_ctx;
	uint64_t		tx_err_user_ctx;
	uint64_t		tx_conf_user_ctx;
};

/**************************************************************************//**
 @Description	structure representing DPNI attach link parameters
*//***************************************************************************/
struct dpni_attach_link_params {
	enum dpni_link_type link_type;
	union {
		struct {
			void			*phys_ni;
		} phys;
		struct {
			void			*l2sw;
			uint8_t			if_id;
#ifdef MC
			dpni_cb_attach_link	*attach_cb;
			dpni_cb_detach_link	*detach_cb;
#endif /* MC */
		} l2sw;
		struct {
			void			*demux;
			uint8_t			if_id;
#ifdef MC
			dpni_cb_attach_link	*attach_cb;
			dpni_cb_detach_link	*detach_cb;
#endif /* MC */
		} demux;
		struct {
			void			*lag;
#ifdef MC
			dpni_cb_attach_link	*attach_cb;
			dpni_cb_detach_link	*detach_cb;
#endif /* MC */
		} lag;
		struct {
			int tmp;
		} aiop;
		struct {
			void			*dpni;
		} dpni;
	} u;
};


/**************************************************************************//**
 @Description	structure representing DPNI TX TC parameters
*//***************************************************************************/
struct dpni_tx_tc_cfg {
	uint8_t tcid;
};

/**************************************************************************//**
 @Description	structure representing DPNI TX Queue parameters
*//***************************************************************************/
struct dpni_tx_queue_ctx {
	dpni_tx_q_mod_opt_t	options;
	int 			tx_conf;
	uint64_t		user_ctx;
				/**< will be provided in case of tx-confirmation
				 *  or lossless condition */
	void 			*dpio;
	int 			dan_en;
	uint16_t		depth_limit;
	int			cksum_gen;
};

/**************************************************************************//**
 @Description	structure representing DPNI RX TC parameters
*//***************************************************************************/
struct dpni_rx_tc_cfg {
	uint8_t tcid;
	uint8_t dist_size;
//	struct policing_params *params;
};

/**************************************************************************//**
 @Description	structure representing DPNI RX Queue parameters
*//***************************************************************************/
struct dpni_rx_queue_ctx {
	uint8_t 			queue_idx;
	dpni_rx_q_mod_opt_t		options;
	uint64_t			user_ctx;
	struct ldpaa_flow_ctx		*flc;
	void				*dpio;
	int				dan_en;
};

/**************************************************************************//**
 @Description	structure representing configuration parameters
*//***************************************************************************/
struct dpni_int_cfg
{
	uint16_t	tx_qdid;
	uint16_t	tx_data_offset;
};

/**************************************************************************//**
 @Description	structure representing attributes parameters
*//***************************************************************************/
struct dpni_attributes
{
	uint64_t	options;
	uint8_t		max_tcs;
	uint8_t		max_dist_per_tc[DPNI_MAX_NUM_OF_TC];
};

/**************************************************************************//**
 @Description	structure representing statistic parameters
*//***************************************************************************/
struct dpni_stats
{
	uint64_t	egr_frame_cnt;
	uint64_t	egr_byte_cnt;
	uint64_t	egr_frame_discard_cnt;
	uint64_t	ing_frame_cnt;
	uint64_t	ing_byte_cnt;
	uint64_t	ing_fltr_frame_cnt;
	uint64_t	ing_frame_discard_cnt;
	uint64_t	ing_mcast_frame_cnt;
	uint64_t	ing_mcast_byte_cnt;
	uint64_t	ing_bcast_frame_cnt;
	uint64_t	ing_bcast_bytes_cnt;
};

/**************************************************************************//**
 @Description	structure representing QOS table parameters
*//***************************************************************************/
struct dpni_qos_tbl_params
{
	struct extract_key 	*source;
	int 			drop_frame;
	uint8_t			default_tc;
};

/**************************************************************************//**
 @Description	structure representing distribution parameters
*//***************************************************************************/
struct dpni_dist_params
{
	int 			dist_fs;
	struct extract_key	*extract_params; /* TODO */
};

/**************************************************************************//**
 @Description	structure representing FS table parameters
*//***************************************************************************/
struct dpni_fs_tbl_params
{
	uint8_t		tcid;
	int		drop_frame;
	uint8_t		default_flowid;
};


/**************************************************************************//**
 @Function	dpni_open

 @Description	Open object handle – required before any operation on the object

 @Param[in]	regs - pointer to MC portal registers address
 @Param[in]	id - dpni unique ID
 @Param[in]	icid - the software context’s icid

 @Return	Handle to this object

 @Cautions	None.
*//***************************************************************************/
struct dpni *dpni_open(void *regs, int id, uint16_t icid);

/**************************************************************************//**
 @Function	dpni_close

 @Description	Closes the object handle, no further operations on the object
		are allowed

 @Param[in]	dpni - dpni handle

 @Retur		Indicates completion status of the function

 @Cautions	None.
*//***************************************************************************/
int dpni_close(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_attach

 @Description	attach the NI to application

 @Param[in]	dpni - dpni handle
 @Param[in]	params - attach Parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_attach(struct dpni *dpni, struct dpni_attach_params *params);

/**************************************************************************//**
 @Function	dpni_detach

 @Description	detach the NI from application

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_detach(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_attach_link

 @Description	attach the NI to the link side

 @Param[in]	dpni - dpni handle
 @Param[in]	params - attach Parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_attach_link(struct dpni *dpni,
                     struct dpni_attach_link_params *params);

/**************************************************************************//**
 @Function	dpni_detach_link

 @Description	detach the NI from the link

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_detach_link(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_enable

 @Description	enable the NI, will allow sending and receiving frames.

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_enable(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_disable

 @Description	enable the NI, will disallow sending and receiving frames.

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_disable(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_tx_tc

 @Description	Set TX TC settings

 @Param[in]	dpni - dpni handle
 @Param[in]	tc_cfg - TC parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_set_tx_tc(struct dpni *dpni, struct dpni_tx_tc_cfg *tc_cfg);

/**************************************************************************//**
 @Function	dpni_set_rx_tc

 @Description	Set TX TC settings

 @Param[in]	dpni - dpni handle
 @Param[in]	tc_cfg - TC group parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_set_rx_tc(struct dpni *dpni, struct dpni_rx_tc_cfg *tc_cfg);

/**************************************************************************//**
 @Function	dpni_set_tx_queue_ctx

 @Description	Set TX queue context

 @Param[in]	dpni - dpni handle
 @Param[in]	tcid - traffic class id
 @Param[in]	params - queue parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_set_tx_queue_ctx(struct dpni *dpni,
				uint8_t	tcid,
				struct dpni_tx_queue_ctx *params);

/**************************************************************************//**
 @Function	dpni_set_rx_queue_ctx

 @Description	Set RX queue context

 @Param[in]	dpni - dpni handle
 @Param[in]	tcid - traffic class id
 @Param[in]	params - queue parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_set_rx_queue_ctx(struct dpni *dpni,
				uint8_t	tcid,
				struct dpni_rx_queue_ctx *params);

/**************************************************************************//**
 @Function	dpni_get_cfg

 @Description	Return the NI’s internal configuration

 @Param[in]	dpni - dpni handle
 @Param[out]	cfg - internal configuration

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_get_cfg(struct dpni *dpni, struct dpni_int_cfg *cfg);

/**************************************************************************//**
 @Function	dpni_get_attributes

 @Description	Retrieve the NI’s attributes.

 @Param[in]	dpni - dpni handle
 @Param[out]	cfg - internal configuration

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_get_attributes(struct dpni *dpni,
                        struct dpni_attributes *attributes);

/**************************************************************************//**
 @Function	dpni_get_stats

 @Description	Retrieve the statistics

 @Param[in]	dpni - dpni handle
 @Param[out]	stats - statics

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_get_stats(struct dpni *dpni, struct dpni_stats *stats);

/**************************************************************************//**
 @Function	dpni_reset_stats

 @Description	Reset the statistics.

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_reset_stats(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_get_link_state

 @Description	Reset the statistics.

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_get_link_state(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_mfl

 @Description	Set the maximum received frame length.

 @Param[in]	dpni - dpni handle
 @Param[in]	mfl - MFL length

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_set_mfl(struct dpni *dpni, uint16_t mfl);

/**************************************************************************//**
 @Function	dpni_set_mtu

 @Description	Set the MTU for this interface. Will have affect on IPF and
 	 	conditionally on GSO.

 @Param[in]	dpni - dpni handle
 @Param[in]	mtu - MTU length

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_set_mtu(struct dpni *dpni, uint16_t mtu);

/**************************************************************************//**
 @Function	dpni_enable_multicast_promisc

 @Description	Enable/Disable multicast promiscuous mode

 @Param[in]	dpni - dpni handle
 @Param[in]	en - ‘1’ for enabling/’0’ for disabling

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_enable_multicast_promisc(struct dpni *dpni, int en);

/**************************************************************************//**
 @Function	dpni_modify_primary_mac_addr

 @Description	Modify the primary mac address

 @Param[in]	dpni - dpni handle
 @Param[in]	addr - MAC address

 @Return	'0' on Success; error code otherwise.

 @Cautions	None.
*//***************************************************************************/
int dpni_modify_primary_mac_addr(struct dpni *dpni,
                                 uint8_t addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
 @Function	dpni_add_mac_addr

 @Description	Add unicast/multicast filter address

 @Param[in]	dpni - dpni handle
 @Param[in]	addr - MAC address

 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_mac_addr(struct dpni 	*dpni,
                      uint8_t 		addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
 @Function	dpni_remove_mac_addr

 @Description	Remove unicast/multicast filter address

 @Param[in]	dpni - dpni handle
 @Param[in]	addr - MAC address

 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_mac_addr(struct dpni 	*dpni,
                         uint8_t	addr[NET_HDR_FLD_ETH_ADDR_SIZE]);  

/**************************************************************************//**
 @Function	dpni_clear_mac_table

 @Description	Clear the mac filter table

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_mac_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_add_vlan_id

 @Description	Add VLAN-id filter

 @Param[in]	dpni - dpni handle
 @Param[in]	vid - VLAN ID
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_vlan_id(struct dpni *dpni, uint16_t vid);

/**************************************************************************//**
 @Function	dpni_remove_vlan_id

 @Description	Add VLAN-id filter

 @Param[in]	dpni - dpni handle
 @Param[in]	vid - VLAN ID
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_vlan_id(struct dpni *dpni, uint16_t vid);

/**************************************************************************//**
 @Function	dpni_clear_vlan_table

 @Description	Clear the VLAN filter table

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_vlan_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_qos_table

 @Description	Set QoS mapping table

 @Param[in]	dpni - dpni handle
 @Param[in]	params - QoS table parameters

 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_set_qos_table(struct dpni 			*dpni,
                       struct dpni_qos_tbl_params 	*params);

/**************************************************************************//**
 @Function	dpni_delete_qos_table

 @Description	Delete QoS mapping table

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_delete_qos_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_add_qos_entry

 @Description	Add QoS mapping entry

 @Param[in]	dpni - dpni handle
 @Param[in]	key_params - QoS key parameters
 @Param[in]	tcid - traffic class id
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_qos_entry(struct dpni 		*dpni,
                       struct key_params	*key_params,
                       uint8_t 			tcid);

/**************************************************************************//**
 @Function	dpni_remove_qos_entry

 @Description	Remove QoS mapping entry

 @Param[in]	dpni - dpni handle
 @Param[in]	key_params - QoS key parameters
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_qos_entry(struct dpni *dpni,
				struct key_params *key_params);

/**************************************************************************//**
 @Function	dpni_clear_qos_table

 @Description	Clear all QoS mapping entries

 @Param[in]	dpni - dpni handle

 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_qos_table(struct dpni *dpni);

/**************************************************************************//**
 @Function	dpni_set_dist

 @Description	Set the distribution method and key

 @Param[in]	dpni - dpni handle
 @Param[in]	dist - distribution params
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_set_dist(struct dpni 			*dpni,
                  struct dpni_dist_params 	dist[DPNI_MAX_NUM_OF_TC]);

/**************************************************************************//**
 @Function	dpni_set_fs_table

 @Description	Set FS mapping table for TC

 @Param[in]	dpni - dpni handle
 @Param[in]	params - FS table parameters
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_set_fs_table(struct dpni 		*dpni,
                      struct dpni_fs_tbl_params *params);

/**************************************************************************//**
 @Function	dpni_delete_fs_table

 @Description	Delete FS mapping table

 @Param[in]	dpni - dpni handle
 @Param[in]	tcid - traffic class id
  
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_delete_fs_table(struct dpni *dpni, uint8_t tcid);

/**************************************************************************//**
 @Function	dpni_add_fs_entry

 @Description	Add FS entry

 @Param[in]	dpni - dpni handle
 @Param[in]	tcid - traffic class id
 @Param[in]	key_params - key parameters
 @Param[in]	flowid - flow id
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_add_fs_entry(struct dpni 	*dpni,
                      uint8_t 		tcid,
                      struct key_params *key_params,
                      uint16_t 		flowid);

/**************************************************************************//**
 @Function	dpni_remove_fs_entry

 @Description	Remove FS entry

 @Param[in]	dpni - dpni handle
 @Param[in]	tcid - traffic class id
 @Param[in]	key_params - key parameters
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_remove_fs_entry(struct dpni 		*dpni,
                         uint8_t 		tcid,
                         struct key_params	*key_params);

/**************************************************************************//**
 @Function	dpni_clear_fs_table

 @Description	Clear all FS entries

 @Param[in]	dpni - dpni handle
 @Param[in]	tcid - traffic class id
 
 @Return	'0' on Success; error code otherwise.

 @Cautions	Allowed only following dpni_attach().
*//***************************************************************************/
int dpni_clear_fs_table(struct dpni *dpni, uint8_t tcid);

/** @} */ /* end of dpni_runtime_control_grp group */
/** @} */ /* end of dpni_g group */
/** @} */ /* end of ldpaa_g group */

#endif /* __FSL_DPNI_H */
