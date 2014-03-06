/**************************************************************************//*
 @File          fsl_dpni_cmd.h

 @Description   defines dpni portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPNI_CMD_H
#define _FSL_DPNI_CMD_H

/* cmd IDs */
#define DPNI_CMDID_INIT				0x123
#define DPNI_CMDID_DONE				0x124
#define DPNI_CMDID_ATTACH			0x125
#define DPNI_CMDID_DETACH			0x126
#define DPNI_CMDID_ATTACH_LINK			0x127
#define DPNI_CMDID_DETACH_LINK			0x128
#define DPNI_CMDID_SET_TX_TC			0x129
#define DPNI_CMDID_SET_RX_TC			0x12A
#define DPNI_CMDID_SET_TX_FLOW			0x12B
#define DPNI_CMDID_GET_TX_FLOW			0x12C
#define DPNI_CMDID_SET_RX_FLOW			0x154
#define DPNI_CMDID_GET_RX_FLOW			0x155
#define DPNI_CMDID_RESET			0x12D
#define DPNI_CMDID_GET_ATTR			0x12E
#define DPNI_CMDID_GET_QDID			0x12F
#define DPNI_CMDID_ENABLE			0x130
#define DPNI_CMDID_DISABLE			0x131
#define DPNI_CMDID_GET_COUNTER			0x132
#define DPNI_CMDID_SET_COUNTER			0x133
#define DPNI_CMDID_GET_LINK_STATE		0x134
#define DPNI_CMDID_SET_MFL			0x135
#define DPNI_CMDID_SET_MTU			0x136
#define DPNI_CMDID_SET_MCAST_PROMISC		0x137
#define DPNI_CMDID_GET_MCAST_PROMISC		0x138
#define DPNI_CMDID_SET_PRIM_MAC		0x139
#define DPNI_CMDID_ADD_MAC_ADDR			0x13A
#define DPNI_CMDID_REMOVE_MAC_ADDR		0x13B
#define DPNI_CMDID_CLR_MAC_TBL			0x13C
#define DPNI_CMDID_ADD_VLAN_ID			0x13D
#define DPNI_CMDID_REMOVE_VLAN_ID		0x13E
#define DPNI_CMDID_CLR_VLAN_TBL			0x13F
#define DPNI_CMDID_SET_QOS_TBL			0x140
#define DPNI_CMDID_DELETE_QOS_TBL		0x141
#define DPNI_CMDID_ADD_QOS_ENT			0x142
#define DPNI_CMDID_REMOVE_QOS_ENT		0x143
#define DPNI_CMDID_CLR_QOS_TBL			0x144
#define DPNI_CMDID_SET_DIST			0x145
#define DPNI_CMDID_SET_FS_TBL			0x146
#define DPNI_CMDID_DELETE_FS_TBL		0x147
#define DPNI_CMDID_ADD_FS_ENT			0x148
#define DPNI_CMDID_REMOVE_FS_ENT		0x149
#define DPNI_CMDID_CLR_FS_TBL			0x14A
#define DPNI_CMDID_SET_IRQ			0x14B
#define DPNI_CMDID_SET_TX_PAUSE_FRAMES		0x14C
#define DPNI_CMDID_SET_RX_IGNORE_PAUSE_FRAMES	0x14D

#define DPNI_CMDID_GET_TX_DATA_OFFSET		0x150
#define DPNI_CMDID_GET_PRIM_MAC		0x151
#define DPNI_CMDID_GET_MFL			0x152
#define DPNI_CMDID_GET_MTU			0x153
#define DPNI_CMDID_GET_RX_BUFFER_LAYOUT		0x154
#define DPNI_CMDID_SET_RX_BUFFER_LAYOUT		0x155

#define DPNI_CMDID_GET_IRQ				0x156
#define DPNI_CMDID_SET_IRQ_ENABLE			0x157
#define DPNI_CMDID_GET_IRQ_ENABLE			0x158
#define DPNI_CMDID_SET_IRQ_MASK				0x159
#define DPNI_CMDID_GET_IRQ_MASK				0x15A
#define DPNI_CMDID_GET_IRQ_STATUS			0x15B
#define DPNI_CMDID_CLEAR_IRQ_STATUS			0x15C

/* cmd sizes */
#define DPNI_CMDSZ_INIT				(8 * 6)
#define DPNI_CMDSZ_DONE				0
#define DPNI_CMDSZ_RESET			0
#define DPNI_CMDSZ_ATTACH			(8 * 7)
#define DPNI_CMDSZ_DETACH			0
#define DPNI_CMDSZ_ATTACH_LINK			8
#define DPNI_CMDSZ_DETACH_LINK			0
#define DPNI_CMDSZ_SET_TX_TC			8
#define DPNI_CMDSZ_SET_RX_TC			8
#define DPNI_CMDSZ_SET_TX_FLOW			(8 * 3)
#define DPNI_CMDSZ_GET_TX_FLOW			(8 * 4)
#define DPNI_CMDSZ_SET_RX_FLOW			(8 * 3)
#define DPNI_CMDSZ_GET_RX_FLOW			(8 * 3)
#define DPNI_CMDSZ_GET_CFG			(8 * 2)
#define DPNI_CMDSZ_GET_ATTR			(8 * 4)
#define DPNI_CMDSZ_GET_QDID			8
#define DPNI_CMDSZ_GET_RX_BUFFER_LAYOUT		(8 * 2)
#define DPNI_CMDSZ_SET_RX_BUFFER_LAYOUT		(8 * 2)
#define DPNI_CMDSZ_ENABLE			0
#define DPNI_CMDSZ_DISABLE			0
#define DPNI_CMDSZ_GET_TX_DATA_OFFSET		8
#define DPNI_CMDSZ_GET_LINK_STATE		8
#define DPNI_CMDSZ_SET_MFL			8
#define DPNI_CMDSZ_GET_MFL			8
#define DPNI_CMDSZ_SET_MTU			8
#define DPNI_CMDSZ_GET_MTU			8
#define DPNI_CMDSZ_SET_MCAST_PROMISC		8
#define DPNI_CMDSZ_GET_MCAST_PROMISC		8
#define DPNI_CMDSZ_MODIFY_PRIM_MAC		8
#define DPNI_CMDSZ_ADD_MAC_ADDR			8
#define DPNI_CMDSZ_REMOVE_MAC_ADDR		8
#define DPNI_CMDSZ_CLR_MAC_TBL			0
#define DPNI_CMDSZ_ADD_VLAN_ID			8
#define DPNI_CMDSZ_REMOVE_VLAN_ID		8
#define DPNI_CMDSZ_CLR_VLAN_TBL			0
#define DPNI_CMDSZ_SET_QOS_TBL			8
#define DPNI_CMDSZ_DELETE_QOS_TBL		0
#define DPNI_CMDSZ_ADD_QOS_ENT			8
#define DPNI_CMDSZ_REMOVE_QOS_ENT		0
#define DPNI_CMDSZ_CLR_QOS_TBL			0
#define DPNI_CMDSZ_SET_DIST			0
#define DPNI_CMDSZ_SET_FS_TBL			8
#define DPNI_CMDSZ_DELETE_FS_TBL		8
#define DPNI_CMDSZ_ADD_FS_ENT			8
#define DPNI_CMDSZ_REMOVE_FS_ENT		8
#define DPNI_CMDSZ_CLR_FS_TBL			8
#define DPNI_CMDSZ_GET_MFL			8
#define DPNI_CMDSZ_SET_IRQ			(8 * 2)
#define DPNI_CMDSZ_SET_TX_PAUSE_FRAMES		8
#define DPNI_CMDSZ_SET_RX_IGNORE_PAUSE_FRAMES	8
#define DPNI_CMDSZ_GET_COUNTER			(8 * 2)
#define DPNI_CMDSZ_SET_COUNTER			(8 * 2)
#define DPNI_CMDSZ_SET_PRIM_MAC		8
#define DPNI_CMDSZ_GET_PRIM_MAC		8
#define DPNI_CMDSZ_GET_IRQ				(8 * 2)
#define DPNI_CMDSZ_SET_IRQ_ENABLE			8
#define DPNI_CMDSZ_GET_IRQ_ENABLE			8
#define DPNI_CMDSZ_SET_IRQ_MASK				8
#define DPNI_CMDSZ_GET_IRQ_MASK				8
#define DPNI_CMDSZ_GET_IRQ_STATUS			8
#define DPNI_CMDSZ_CLEAR_IRQ_STATUS			8

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_INIT(_OP) \
	_OP(0,	0,	8,	uint8_t,		cfg->adv.max_tcs) \
	_OP(0,  8,	8,	uint8_t,		cfg->adv.max_senders) \
	_OP(0,  16,	8,	uint8_t,		cfg->mac_addr[0]) \
	_OP(0,  24,	8,	uint8_t,		cfg->mac_addr[1]) \
	_OP(0,  32,	8,	uint8_t,		cfg->mac_addr[2]) \
	_OP(0,  40,	8,	uint8_t,		cfg->mac_addr[3]) \
	_OP(0,  48,	8,	uint8_t,		cfg->mac_addr[4]) \
	_OP(0,  56,	8,	uint8_t,		cfg->mac_addr[5]) \
	_OP(1,  0,	64,	uint64_t,		cfg->adv.options) \
	_OP(2,  0,	8,	uint8_t, \
					cfg->adv.max_unicast_filters) \
	_OP(2,  8,	8,	uint8_t, \
					cfg->adv.max_multicast_filters) \
	_OP(2,  16,	8,	uint8_t, \
					cfg->adv.max_vlan_filters) \
	_OP(2,  24,	8,	uint8_t, \
					cfg->adv.max_qos_entries) \
	_OP(2,  32,	8,	uint8_t, \
					cfg->adv.max_qos_key_size) \
	_OP(2,  48,	8,	uint8_t, \
					cfg->adv.max_dist_key_size) \
	_OP(3,  0,	1,	enum dpni_type,		cfg->type) \
	_OP(4,  0,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[0]) \
	_OP(4,  16,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[1]) \
	_OP(4,  32,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[2]) \
	_OP(4,  48,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[3]) \
	_OP(5,  0,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[4]) \
	_OP(5,  16,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[5]) \
	_OP(5,  32,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[6]) \
	_OP(5,  48,	16,	uint16_t, \
					cfg->adv.max_dist_per_tc[7])

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_IRQ(_OP) \
	_OP(0,	0,	8,	uint8_t,		irq_index) \
	_OP(0,  32,	32,	uint32_t,		irq_val) \
	_OP(1,  0,	64,	uint64_t,		irq_paddr) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_TX_PAUSE_FRAMES(_OP) \
	_OP(0,	0,	8,	uint8_t,		priority) \
	_OP(0,  16,	16,	uint16_t,		pause_time) \
	_OP(0,  32,	16,	uint16_t,		thresh_time) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_RX_IGNORE_PAUSE_FRAMES(_OP) \
	_OP(0,	0,	1,	int,			enable) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_ATTACH(_OP) \
	_OP(0,	0,	16,	uint16_t,	cfg->dest_cfg.dpio_id) \
	_OP(0,	16,	8,	uint8_t,	cfg->num_dpbp) \
	_OP(0,  32,	16,	uint16_t,	cfg->dest_cfg.dpcon_id) \
	_OP(0,  48,	8,	uint8_t,	cfg->dest_cfg.priority) \
	_OP(0,  56,	1,	int,		cfg->dest_apply_all) \
	_OP(0,  57,	2,	enum dpni_dest,	cfg->dest_cfg.type) \
	_OP(1,	0,	64,	uint64_t,	cfg->rx_user_ctx) \
	_OP(2,	0,	64,	uint64_t,	cfg->rx_err_user_ctx) \
	_OP(3,	0,	64,	uint64_t,	cfg->tx_err_user_ctx) \
	_OP(4,	0,	64,	uint64_t,	cfg->tx_conf_user_ctx) \
	_OP(5,	0,	16,	uint16_t,	cfg->dpbp_id[0]) \
	_OP(5,	16,	16,	uint16_t,	cfg->dpbp_id[1]) \
	_OP(5,	32,	16,	uint16_t,	cfg->dpbp_id[2]) \
	_OP(5,	48,	16,	uint16_t,	cfg->dpbp_id[3]) \
	_OP(6,	0,	16,	uint16_t,	cfg->dpbp_id[4]) \
	_OP(6,	16,	16,	uint16_t,	cfg->dpbp_id[5]) \
	_OP(6,	32,	16,	uint16_t,	cfg->dpbp_id[6]) \
	_OP(6,	48,	16,	uint16_t,	cfg->dpbp_id[7]) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_ATTACH_LINK(_OP) \
	_OP(0,	0,	3,	enum dpni_link_type,	cfg->link_type) \
/*TODO - How to insert union?*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_ATTR(_OP) \
	_OP(0,	0,	8,	uint8_t,		attr->max_tcs) \
	_OP(1,	0,	64,	uint64_t,		attr->options) \
	_OP(2,	0,	16,	uint16_t,	attr->max_dist_per_tc[0]) \
	_OP(2,	16,	16,	uint16_t,	attr->max_dist_per_tc[1]) \
	_OP(2,	32,	16,	uint16_t,	attr->max_dist_per_tc[2]) \
	_OP(2,	48,	16,	uint16_t,	attr->max_dist_per_tc[3]) \
	_OP(2,	0,	16,	uint16_t,	attr->max_dist_per_tc[4]) \
	_OP(2,	16,	16,	uint16_t,	attr->max_dist_per_tc[5]) \
	_OP(2,	32,	16,	uint16_t,	attr->max_dist_per_tc[6]) \
	_OP(2,	48,	16,	uint16_t,	attr->max_dist_per_tc[7]) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_RX_BUFFER_LAYOUT(_OP) \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size) \
	_OP(0,	16,	16,	uint16_t,	layout->data_align) \
	_OP(0,	32,	32,	uint32_t,	layout->options) \
	_OP(1,	0,	1,	int,		layout->pass_timestamp) \
	_OP(1,	1,	1,	int,		layout->pass_parser_result) \
	_OP(1,	2,	1,	int,		layout->pass_frame_status) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_RX_BUFFER_LAYOUT(_OP) \
	_OP(0,	0,	16,	uint16_t,	layout->private_data_size) \
	_OP(0,	16,	16,	uint16_t,	layout->data_align) \
	_OP(0,	32,	32,	uint32_t,	layout->options) \
	_OP(1,	0,	1,	int,		layout->pass_timestamp) \
	_OP(1,	1,	1,	int,		layout->pass_parser_result) \
	_OP(1,	2,	1,	int,		layout->pass_frame_status) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_QDID(_OP) \
	_OP(0,	0,	16,	uint16_t,		qdid) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_TX_DATA_OFFSET(_OP) \
	_OP(0,	0,	16,	uint16_t,		data_offset) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_GET_COUNTER(_OP) \
	_OP(0,	0,	16,	enum dpni_counter,	 counter) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_COUNTER(_OP) \
	_OP(1,	0,	64,	uint64_t,		value) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_LINK_STATE(_OP) \
	_OP(0,	0,	1,	int,			up) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_COUNTER(_OP) \
	_OP(0,	0,	16,	enum dpni_counter,	counter) \
	_OP(1,	0,	64,	uint64_t,		value) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_MFL(_OP) \
	_OP(0,	0,	16,	uint16_t,		mfl) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_MFL(_OP) \
	_OP(0,	0,	16,	uint16_t,		mfl) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_MTU(_OP) \
	_OP(0,	0,	16,	uint16_t,		mtu) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_MTU(_OP) \
	_OP(0,	0,	16,	uint16_t,		mtu) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_MULTICAST_PROMISC(_OP) \
	_OP(0,	0,	1,	int,			en) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_MULTICAST_PROMISC(_OP) \
	_OP(0,	0,	1,	int,			en) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_PRIMARY_MAC_ADDR(_OP) \
	_OP(0,	16,	8,	uint8_t,		addr[0]) \
	_OP(0,	24,	8,	uint8_t,		addr[1]) \
	_OP(0,	32,	8,	uint8_t,		addr[2]) \
	_OP(0,	40,	8,	uint8_t,		addr[3]) \
	_OP(0,	48,	8,	uint8_t,		addr[4]) \
	_OP(0,	56,	8,	uint8_t,		addr[5]) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_PRIMARY_MAC_ADDR(_OP) \
	_OP(0,	16,	8,	uint8_t,		addr[0]) \
	_OP(0,	24,	8,	uint8_t,		addr[1]) \
	_OP(0,	32,	8,	uint8_t,		addr[2]) \
	_OP(0,	40,	8,	uint8_t,		addr[3]) \
	_OP(0,	48,	8,	uint8_t,		addr[4]) \
	_OP(0,	56,	8,	uint8_t,		addr[5]) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_ADD_MAC_ADDR(_OP) \
	_OP(0,	16,	8,	uint8_t,		addr[0]) \
	_OP(0,	24,	8,	uint8_t,		addr[1]) \
	_OP(0,	32,	8,	uint8_t,		addr[2]) \
	_OP(0,	40,	8,	uint8_t,		addr[3]) \
	_OP(0,	48,	8,	uint8_t,		addr[4]) \
	_OP(0,	56,	8,	uint8_t,		addr[5]) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_REMOVE_MAC_ADDR(_OP) \
	_OP(0,	16,	8,	uint8_t,		addr[0]) \
	_OP(0,	24,	8,	uint8_t,		addr[1]) \
	_OP(0,	32,	8,	uint8_t,		addr[2]) \
	_OP(0,	40,	8,	uint8_t,		addr[3]) \
	_OP(0,	48,	8,	uint8_t,		addr[4]) \
	_OP(0,	56,	8,	uint8_t,		addr[5]) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_ADD_VLAN_ID(_OP) \
	_OP(0,	32,	16,	uint16_t,		vlan_id) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_REMOVE_VLAN_ID(_OP) \
	_OP(0,	32,	16,	uint16_t,		vlan_id) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_TX_TC(_OP) \
	_OP(0,	0,	16,	uint16_t,		cfg->depth_limit) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_RX_TC(_OP) \
	_OP(0,	0,	16,	uint16_t,		cfg->dist_size) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_TX_FLOW(_OP) \
	_OP(0,   0,	16,	uint16_t,		cfg->dest_cfg.dpio_id)\
	_OP(0,   16,	8,	uint8_t,		cfg->dest_cfg.priority)\
	_OP(0,   24,	1,	int,			cfg->only_error_frames)\
	_OP(0,   25,	2,	enum dpni_dest,		cfg->dest_cfg.type)\
	_OP(0,   27,	1,	int,			cfg->l3_chksum_gen)\
	_OP(0,   28,	1,	int,			cfg->l4_chksum_gen)\
	_OP(0,   32,	16,	uint16_t,		cfg->dest_cfg.dpcon_id)\
	_OP(0,   48,	16,	uint16_t,		*flow_id)\
	_OP(1,   0,	64,	uint64_t,		cfg->user_ctx)\
	_OP(2,   0,	32,	uint32_t,		cfg->options)\
	_OP(2,   32,	32,	int,			cfg->tx_conf_err)\
/*TODO - flow_id in/out????*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_GET_TX_FLOW(_OP) \
	_OP(0,   48,	16,	uint16_t,		flow_id)\

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_TX_FLOW(_OP) \
	_OP(0,   0,	16,	uint16_t,		cfg->dest_cfg.dpio_id)\
	_OP(0,   16,	8,	uint8_t,		cfg->dest_cfg.priority)\
	_OP(0,   24,	1,	int,			cfg->only_error_frames)\
	_OP(0,   25,	2,	enum dpni_dest,		cfg->dest_cfg.type)\
	_OP(0,   27,	1,	int,			cfg->l3_chksum_gen)\
	_OP(0,   28,	1,	int,			cfg->l4_chksum_gen)\
	_OP(0,   32,	16,	uint16_t,		cfg->dest_cfg.dpcon_id)\
	_OP(1,   0,	64,	uint64_t,		cfg->user_ctx)\
	_OP(2,   0,	32,	uint32_t,		cfg->options)\
	_OP(2,   32,	32,	int,			cfg->tx_conf_err)\
	_OP(3,   0,	32,	uint32_t,		*fqid)

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_RX_FLOW(_OP) \
	_OP(0,	0,	16,	uint16_t,		cfg->dest_cfg.dpio_id) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \
	_OP(0,	24,	16,	uint8_t,		cfg->dest_cfg.priority)\
	_OP(0,	32,	16,	uint16_t,		cfg->dest_cfg.dpcon_id)\
	_OP(0,	48,	16,	uint16_t,		flow_id) \
	_OP(1,	0,	64,	uint64_t,		cfg->user_ctx) \
	_OP(2,	0,	32,	uint32_t,		cfg->options) \
	_OP(2,	32,	2,	enum dpni_dest,		cfg->dest_cfg.type) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_GET_RX_FLOW(_OP) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \
	_OP(0,	48,	16,	uint16_t,		flow_id) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_RSP_GET_RX_FLOW(_OP) \
	_OP(0,	0,	16,	uint16_t,		cfg->dest_cfg.dpio_id) \
	_OP(0,	24,	16,	uint8_t,		cfg->dest_cfg.priority)\
	_OP(0,	32,	16,	uint16_t,		cfg->dest_cfg.dpcon_id)\
	_OP(1,	0,	64,	uint64_t,		cfg->user_ctx) \
	_OP(2,	0,	32,	uint32_t,		cfg->options) \
	_OP(2,	32,	2,	enum dpni_dest,		cfg->dest_cfg.type) \
	_OP(3,	0,	32,	uint32_t,		*fqid) \

/*	param, offset, width,	type,		arg_name */
#define DPNI_CMD_SET_QOS_TABLE(_OP) \
	_OP(0,	8,	1,		int,		cfg->drop_frame) \
	_OP(0,	16,	8,		uint8_t,	cfg->default_tc) \
/*TODO- dpkg_profile_cfg*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_ADD_QOS_ENTRY(_OP) \
	/*_OP(0,	0,	16,	uint8,		cfg->key) */\
	_OP(0,	16,	8,	uint8_t,		tc_id) \
	_OP(0,	24,	8,	uint8_t,		cfg->size) \
	/*_OP(0,	32,	16,	uint8,		cfg->mask)*/ \
/*TODO - key_params*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_REMOVE_QOS_ENTRY(_OP) \
	/*_OP(0, 0,	16,	(uint8 *),		cfg->key)*/ \
	_OP(0,	24,	8,	uint8_t,		cfg->size) \
	/*_OP(0, 32,	16,	(uint8 *),		cfg->mask) */\
/*TODO - key_params*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_DIST(_OP) \
	/*_OP(0, 0,	8,	uint8_t,dist->extract_params->num_extracts)*/\
	_OP(0,	8,	1,	int,		dist->dist_fs) \
/*TODO*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_SET_FS_TABLE(_OP) \
	_OP(0,	0,	4,	enum dpni_fs_miss_action, cfg->miss_action) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \
	_OP(0,	48,	16,	uint16_t,		cfg->default_flow_id) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_DELETE_FS_TABLE(_OP) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_ADD_FS_ENTRY(_OP) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \
	_OP(0,	48,	16,	uint16_t,		flow_id) \
	/*_OP(0, 0,	16,	(uint8 *),		cfg->key)*/ \
	_OP(0,	24,	8,	uint8_t,		cfg->size) \
	/*_OP(0, 32,	16,	(uint8 *),		cfg->mask) */\
/*TODO - key_params*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_REMOVE_FS_ENTRY(_OP) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \
	/*_OP(0, 0,	16,	(uint8 *),		cfg->key)*/ \
	_OP(0,	24,	8,	uint8_t,		cfg->size) \
	/*_OP(0, 32,	16,	(uint8 *),		cfg->mask) */\
/*TODO - key_params*/

/*	param, offset, width,	type,				arg_name */
#define DPNI_CMD_CLEAR_FS_TABLE(_OP) \
	_OP(0,	16,	8,	uint8_t,		tc_id) \

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ(_OP) \
	_OP(0,	0,	32,	uint32_t,		irq_val) \
	_OP(1,	0,	64,	uint64_t,		irq_paddr)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)
	
/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_SET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) 

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPNI_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \

/*	param, offset, width,	type,			arg_name */
#define DPNI_CMD_CLEAR_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

#endif /* _FSL_DPNI_CMD_H */
