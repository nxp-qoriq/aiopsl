/**************************************************************************//*
 @File          fsl_dpsw_cmd.h

 @Description   defines portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef __FSL_DPSW_CMD_H
#define __FSL_DPSW_CMD_H

/* Command IDs */
#define DPSW_CMDID_INIT				0x01
#define DPSW_CMDID_GET_ATTR			0x02
#define DPSW_CMDID_DONE				0x03
#define DPSW_CMDID_ENABLE			0x04
#define DPSW_CMDID_DISABLE			0x05
#define DPSW_CMDID_RESET			0x06
#define DPSW_CMDID_SET_POLICER			0x07
#define DPSW_CMDID_SET_BUFFER_DEPLETION		0x09

#define DPSW_CMDID_SET_REFLECTION_IF		0x0D
#define DPSW_CMDID_SET_PARSER_ERROR_ACTION	0x0E
#define DPSW_CMDID_SET_PTP_V2			0x0F

#define DPSW_CMDID_IF_SET_TCI			0x10
#define DPSW_CMDID_IF_SET_STP_STATE		0x11
#define DPSW_CMDID_IF_SET_ACCEPTED_FRAMES	0x12
#define DPSW_CMDID_SET_IF_ACCEPT_ALL_VLAN	0x13
#define DPSW_CMDID_IF_GET_COUNTER		0x14
#define DPSW_CMDID_IF_SET_COUNTER		0x15
#define DPSW_CMDID_IF_TC_SET_MAP		0x16
#define DPSW_CMDID_IF_ADD_REFLECTION		0x17
#define DPSW_CMDID_IF_REMOVE_REFLECTION		0x18
#define DPSW_CMDID_IF_TC_SET_METERING_MARKING	0x19

#define DPSW_CMDID_IF_ADD_CUSTOM_TPID		0x1B
#define DPSW_CMDID_IF_SET_TRANSMIT_RATE		0x1C
#define DPSW_CMDID_IF_TC_SET_BW			0x1D
#define DPSW_CMDID_IF_ENABLE			0x1E
#define DPSW_CMDID_IF_DISABLE			0x1F
#define DPSW_CMDID_IF_TC_SET_Q_CONGESTION	0x20
#define DPSW_CMDID_IF_TC_SET_PFC		0x21
#define DPSW_CMDID_IF_TC_SET_CN			0x22
#define DPSW_CMDID_IF_GET_ATTR			0x23
#define DPSW_CMDID_IF_SET_MACSEC		0x24

#define DPSW_CMDID_VLAN_ADD			0x30
#define DPSW_CMDID_VLAN_ADD_IF			0x31
#define DPSW_CMDID_VLAN_ADD_IF_UNTAGGED		0x32
#define DPSW_CMDID_VLAN_ADD_IF_FLOODING		0x33
#define DPSW_CMDID_VLAN_REMOVE_IF		0x34
#define DPSW_CMDID_VLAN_REMOVE_IF_UNTAGGED	0x35
#define DPSW_CMDID_VLAN_REMOVE_IF_FLOODING	0x36
#define DPSW_CMDID_VLAN_REMOVE			0x37
#define DPSW_CMDID_FDB_ADD			0x40
#define DPSW_CMDID_FDB_REMOVE			0x41
#define DPSW_CMDID_FDB_ADD_UNICAST		0x42
#define DPSW_CMDID_FDB_REMOVE_UNICAST		0x43
#define DPSW_CMDID_FDB_ADD_MULTICAST		0x44
#define DPSW_CMDID_FDB_REMOVE_MULTICAST		0x45
#define DPSW_CMDID_FDB_SET_LEARNING_MODE	0x46
#define DPSW_CMDID_FDB_GET_ATTR			0x47
#define DPSW_CMDID_SET_IRQ				0x48
#define DPSW_CMDID_GET_IRQ				0x49
#define DPSW_CMDID_SET_IRQ_ENABLE			0x4A
#define DPSW_CMDID_GET_IRQ_ENABLE			0x4B
#define DPSW_CMDID_SET_IRQ_MASK				0x4C
#define DPSW_CMDID_GET_IRQ_MASK				0x4D
#define DPSW_CMDID_GET_IRQ_STATUS			0x4E
#define DPSW_CMDID_CLEAR_IRQ_STATUS			0x4F

/* Command sizes */
#define DPSW_CMDSZ_INIT				(8 * 6)
#define DPSW_CMDSZ_GET_ATTR			(8 * 3)
#define DPSW_CMDSZ_DONE				0
#define DPSW_CMDSZ_ENABLE			0
#define DPSW_CMDSZ_DISABLE			0
#define DPSW_CMDSZ_RESET			8
#define DPSW_CMDSZ_SET_POLICER			8
#define DPSW_CMDSZ_SET_BUFFER_DEPLETION		(8 * 2)

#define DPSW_CMDSZ_SET_REFLECTION_IF		8
#define DPSW_CMDSZ_SET_PARSER_ERROR_ACTION	8
#define DPSW_CMDSZ_SET_PTP_V2			8
#define DPSW_CMDSZ_IF_SET_TCI			8
#define DPSW_CMDSZ_IF_SET_STP_STATE		8
#define DPSW_CMDSZ_IF_SET_ACCEPTED_FRAMES	8
#define DPSW_CMDSZ_SET_IF_ACCEPT_ALL_VLAN	8
#define DPSW_CMDSZ_IF_GET_COUNTER		(8 * 2)
#define DPSW_CMDSZ_IF_SET_COUNTER		(8 * 2)
#define DPSW_CMDSZ_IF_TC_SET_MAP		8
#define DPSW_CMDSZ_IF_ADD_REFLECTION		8
#define DPSW_CMDSZ_IF_REMOVE_REFLECTION		8
#define DPSW_CMDSZ_IF_TC_SET_METERING_MARKING	(8 * 3)
#define DPSW_CMDSZ_IF_ADD_CUSTOM_TPID		8
#define DPSW_CMDSZ_IF_SET_TRANSMIT_RATE		(8 * 2)
#define DPSW_CMDSZ_IF_TC_SET_BW			8
#define DPSW_CMDSZ_IF_ENABLE			8
#define DPSW_CMDSZ_IF_DISABLE			8
#define DPSW_CMDSZ_IF_TC_SET_Q_CONGESTION	(8 * 3)
#define DPSW_CMDSZ_IF_TC_SET_PFC		(8 * 2)
#define DPSW_CMDSZ_IF_TC_SET_CN			(8 * 2)
#define DPSW_CMDSZ_IF_GET_ATTR			(8 * 4)
#define DPSW_CMDSZ_IF_SET_MACSEC		(8 * 2)

#define DPSW_CMDSZ_VLAN_ADD			8
#define DPSW_CMDSZ_VLAN_ADD_IF			8
#define DPSW_CMDSZ_VLAN_ADD_IF_UNTAGGED		8
#define DPSW_CMDSZ_VLAN_ADD_IF_FLOODING		8
#define DPSW_CMDSZ_VLAN_REMOVE_IF		8
#define DPSW_CMDSZ_VLAN_REMOVE_IF_UNTAGGED	8
#define DPSW_CMDSZ_VLAN_REMOVE_IF_FLOODING	8
#define DPSW_CMDSZ_VLAN_REMOVE			8
#define DPSW_CMDSZ_FDB_ADD			8
#define DPSW_CMDSZ_FDB_REMOVE			8
#define DPSW_CMDSZ_FDB_ADD_UNICAST		(8 * 2)
#define DPSW_CMDSZ_FDB_REMOVE_UNICAST		(8 * 2)
#define DPSW_CMDSZ_FDB_ADD_MULTICAST		(8 * 2)
#define DPSW_CMDSZ_FDB_REMOVE_MULTICAST		(8 * 2)
#define DPSW_CMDSZ_FDB_SET_LEARNING_MODE	8
#define DPSW_CMDSZ_FDB_GET_ATTR			8
#define DPSW_CMDSZ_SET_IRQ				(8 * 3)
#define DPSW_CMDSZ_GET_IRQ				(8 * 2)
#define DPSW_CMDSZ_SET_IRQ_ENABLE			8
#define DPSW_CMDSZ_GET_IRQ_ENABLE			8
#define DPSW_CMDSZ_SET_IRQ_MASK				8
#define DPSW_CMDSZ_GET_IRQ_MASK				8
#define DPSW_CMDSZ_GET_IRQ_STATUS			8
#define DPSW_CMDSZ_CLEAR_IRQ_STATUS			8

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_INIT(_OP) \
	_OP(0,  0,	16,	uint16_t,	cfg->adv.num_fdb_entries)\
	_OP(0,  16,	16,	uint16_t,	cfg->adv.fdb_aging_time)\
	_OP(0,  32,	16,	uint16_t,	cfg->num_ifs)\
	_OP(0,  48,	8,	uint8_t,	cfg->adv.max_vlans)\
	_OP(0,  56,	8,	uint8_t,	cfg->adv.max_fdbs)\
	_OP(1,  0,	64,	uint64_t,	cfg->adv.options)\
	_OP(2,  0,	32,	int,		cfg->if_cfg[0].iop_id)\
	_OP(2,  32,	32,	int,		cfg->if_cfg[1].iop_id)\
	_OP(3,  0,	1,	int,		cfg->if_cfg[0].external)\
	_OP(3,  1,	1,	int,		cfg->if_cfg[0].control)\
	_OP(3,  2,	8,	uint8_t,	cfg->if_cfg[0].num_tcs)\
	_OP(3,  10,	22,	int,		cfg->if_cfg[0].phys_if_id)\
	_OP(3,  32,	1,	int,		cfg->if_cfg[1].external)\
	_OP(3,  33,	1,	int,		cfg->if_cfg[1].control)\
	_OP(3,  34,	8,	uint8_t,	cfg->if_cfg[1].num_tcs)\
	_OP(3,  42,	22,	int,		cfg->if_cfg[1].phys_if_id)\
	_OP(4,  0,	64,	uint64_t,	cfg->if_cfg[0].options)\
	_OP(5,  0,	64,	uint64_t,	cfg->if_cfg[1].options)\
/*TODO - if*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_RSP_GET_ATTR(_OP) \
	_OP(0,  0,	16,	uint16_t,		attr->num_ifs)\
	_OP(0,  16,	16,	uint16_t,		attr->mem_size)\
	_OP(0,  32,	8,	uint8_t,		attr->max_vlans)\
	_OP(0,  40,	8,	uint8_t,		attr->max_fdbs)\
	_OP(0,  48,	8,	uint8_t,		attr->num_vlans)\
	_OP(0,  56,	8,	uint8_t,		attr->num_fdbs)\
	_OP(1,  0,	32,	uint32_t,		attr->version)\
	_OP(2,  0,	64,	uint64_t,		attr->options)

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_SET_POLICER(_OP) \
	_OP(0,  0,	1, enum dpsw_policer_mode,		cfg->mode)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_SET_BUFFER_DEPLETION(_OP) \
	_OP(0,  0,	32,	uint32_t,	cfg->entrance_threshold)\
	_OP(0,  32,	32,	uint32_t,	cfg->exit_threshold)\
	_OP(0,  0,	64,	uint64_t,	cfg->wr_addr)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_SET_REFLECTION_IF(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_SET_PARSER_ERROR_ACTION(_OP) \
	_OP(0,  0,	8,	uint8_t,			cfg->num_prots)\
	_OP(0,  8,	2,	enum dpsw_action,		cfg->action)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_SET_PTP_V2(_OP) \
	_OP(0,  0,	32,	uint32_t,		cfg->options)\
	_OP(0,  32,	16,	uint16_t,		cfg->time_offset)\
	_OP(0,  48,	1,	int,			cfg->enable)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_SET_TCI(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	16,	uint16_t,			cfg->vlan_id)\
	_OP(0,  32,	8,	uint8_t,			cfg->dei)\
	_OP(0,  40,	8,	uint8_t,			cfg->pcp)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_SET_STP_STATE(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	16,	uint16_t,			cfg->vlan_id)\
	_OP(0,  32,	4,	enum dpsw_stp_state,		cfg->state)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_SET_ACCEPTED_FRAMES(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	4,	enum dpsw_accepted_frames,	cfg->type)\
	_OP(0,  20,	4,	enum dpsw_action,	cfg->unaccept_act)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_SET_ACCEPT_ALL_VLAN(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	1,	int,				accept_all)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_GET_COUNTER(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	5,	enum dpsw_counter,		type)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_RSP_IF_GET_COUNTER(_OP) \
	_OP(1,  0,	64,	uint64_t,			counter)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_SET_COUNTER(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	5,	enum dpsw_counter,		type)\
	_OP(1,  0,	64,	uint64_t,			counter)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_TC_SET_MAP(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	3,	enum dpsw_user_priority, \
						cfg->user_priority)\
	_OP(1,  0,	8,	uint8_t,	cfg->regenerated_priority[0])\
	_OP(1,  8,	8,	uint8_t,	cfg->regenerated_priority[1])\
	_OP(1,  16,	8,	uint8_t,	cfg->regenerated_priority[2])\
	_OP(1,  24,	8,	uint8_t,	cfg->regenerated_priority[3])\
	_OP(1,  32,	8,	uint8_t,	cfg->regenerated_priority[4])\
	_OP(1,  40,	8,	uint8_t,	cfg->regenerated_priority[5])\
	_OP(1,  48,	8,	uint8_t,	cfg->regenerated_priority[6])\
	_OP(1,  56,	8,	uint8_t,	cfg->regenerated_priority[7])\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_ADD_REFLECTION(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	16,	uint16_t,			cfg->vlan_id)\
	_OP(0,  32,	2,	enum dpsw_reflection_filter,	cfg->filter)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_REMOVE_REFLECTION(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	16,	uint16_t,			cfg->vlan_id)\
	_OP(0,  32,	2,	enum dpsw_reflection_filter,	cfg->filter)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_TC_SET_METERING_MARKING(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	8,	uint8_t,			tc_id)\
	_OP(0,  24,	4,	enum metering_algo,		cfg->algo)\
	_OP(0,  28,	4,	enum metering_mode,		cfg->mode)\
	_OP(0,  32,	32,	uint32_t,			cfg->cir)\
	_OP(1,  0,	32,	uint32_t,			cfg->eir)\
	_OP(1,  32,	32,	uint32_t,			cfg->cbs)\
	_OP(2,  0,	32,	uint32_t,			cfg->ebs)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_ADD_CUSTOM_TPID(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	16,	uint16_t,			cfg->tpid)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_SET_TRANSMIT_RATE(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(1,  0,	64,	uint64_t,			cfg->rate)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_TC_SET_BANDWIDTH(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	8,	uint8_t,			tc_id)\
	_OP(0,  24,	8,	uint8_t,		cfg->delta_bandwidth)\
	_OP(0,  31,	4,	enum dpsw_bw_algo,		cfg->algo)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_ENABLE(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_DISABLE(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_TC_SET_Q_CONGESTION(_OP) \
	_OP(0,  0,	16,	uint16_t,	if_id)\
	_OP(0,  16,	8,	uint8_t,	tc_id)\
	_OP(0,  32,	32,	uint32_t,	cfg->exit_threshold)\
	_OP(1,  0,	64,	uint64_t,	cfg->wr_addr)\
	_OP(2,  0,	32,	uint32_t,	cfg->entrance_threshold)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_TC_SET_PFC(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	8,	uint8_t,			tc_id)\
	_OP(0,  24,	1,	int,				cfg->receiver)\
	_OP(0,  25,	1,	int,				cfg->initiator)\
	_OP(0,  32,	32,	uint32_t,		cfg->initiator_trig)\
	_OP(0,  0,	16,	uint16_t,		cfg->pause_quanta)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_TC_SET_CN(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	8,	uint8_t,			tc_id)\
	_OP(0,  24,	1,	int,				cfg->enable)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_GET_ATTR(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_RSP_IF_GET_ATTR(_OP) \
	_OP(0,  16,	8,	uint8_t,			attr->num_tcs)\
	_OP(0,  27,	1,	int,				attr->external)\
	_OP(0,  28,	1,	int,				attr->control)\
	_OP(0,  32,	32,	int,			attr->phys_if_id)\
	_OP(1,  0,	64,	uint64_t,			attr->options)\
	_OP(2,  0,	32,	int,				attr->iop_id)\
	_OP(3,  0,	64,	uint64_t,			attr->rate)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_IF_SET_MACSEC(_OP) \
	_OP(0,  0,	16,	uint16_t,			if_id)\
	_OP(0,  16,	4,	enum dpsw_cipher_suite,	cfg->cipher_suite)\
	_OP(0,  20,	1,	int,				cfg->enable)\
	_OP(1,  0,	64,	uint64_t,			cfg->sci)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_VLAN_ADD(_OP) \
	_OP(0,  0,	16,	uint16_t,			cfg->fdb_id)\
	_OP(0,  16,	16,	uint16_t,			vlan_id)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_VLAN_ADD_IF(_OP) \
	_OP(0,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(0,  16,	16,	uint16_t,			vlan_id)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_VLAN_ADD_IF_UNTAGGED(_OP) \
	_OP(0,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(0,  16,	16,	uint16_t,			vlan_id)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_VLAN_ADD_IF_FLOODING(_OP) \
	_OP(0,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(0,  16,	16,	uint16_t,			vlan_id)\
/*TODO - array*/

#define DPSW_CMD_VLAN_REMOVE_IF(_OP) \
	_OP(0,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(0,  16,	16,	uint16_t,			vlan_id)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_VLAN_REMOVE_IF_UNTAGGED(_OP) \
	_OP(0,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(0,  16,	16,	uint16_t,			vlan_id)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_VLAN_REMOVE_IF_FLOODING(_OP) \
	_OP(0,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(0,  16,	16,	uint16_t,			vlan_id)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_VLAN_REMOVE(_OP) \
	_OP(0,  16,	16,	uint16_t,			vlan_id)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_ADD(_OP) \
	_OP(0,  16,	16,	uint16_t,		cfg->num_fdb_entries)\
	_OP(0,  32,	16,	uint16_t,		cfg->fdb_aging_time)\
	_OP(0,  48,	16,	uint16_t,	cfg->num_fdb_static_entries)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_RSP_FDB_ADD(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)\


/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_REMOVE(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_ADD_UNICAST(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)\
	_OP(0,  16,	8,	uint8_t,		cfg->eth_addr[0])\
	_OP(0,  24,	8,	uint8_t,		cfg->eth_addr[1])\
	_OP(0,  32,	8,	uint8_t,		cfg->eth_addr[2])\
	_OP(0,  40,	8,	uint8_t,		cfg->eth_addr[3])\
	_OP(0,  48,	8,	uint8_t,		cfg->eth_addr[4])\
	_OP(0,  56,	8,	uint8_t,		cfg->eth_addr[5])\
	_OP(1,  0,	8,	uint16_t,			cfg->if_egress)\
	_OP(1,  16,	4,	enum dpsw_fdb_entry_type,	cfg->type)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_REMOVE_UNICAST(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)\
	_OP(0,  16,	8,	uint8_t,		cfg->eth_addr[0])\
	_OP(0,  24,	8,	uint8_t,		cfg->eth_addr[1])\
	_OP(0,  32,	8,	uint8_t,		cfg->eth_addr[2])\
	_OP(0,  40,	8,	uint8_t,		cfg->eth_addr[3])\
	_OP(0,  48,	8,	uint8_t,		cfg->eth_addr[4])\
	_OP(0,  56,	8,	uint8_t,		cfg->eth_addr[5])\
	_OP(1,  0,	16,	uint16_t,			cfg->if_egress)\
	_OP(1,  16,	4,	enum dpsw_fdb_entry_type,	cfg->type)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_ADD_MULTICAST(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)\
	_OP(0,  16,	8,	uint8_t,		cfg->eth_addr[0])\
	_OP(0,  24,	8,	uint8_t,		cfg->eth_addr[1])\
	_OP(0,  32,	8,	uint8_t,		cfg->eth_addr[2])\
	_OP(0,  40,	8,	uint8_t,		cfg->eth_addr[3])\
	_OP(0,  48,	8,	uint8_t,		cfg->eth_addr[4])\
	_OP(0,  56,	8,	uint8_t,		cfg->eth_addr[5])\
	_OP(1,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(1,  16,	4,	enum dpsw_fdb_entry_type,	cfg->type)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_REMOVE_MULTICAST(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)\
	_OP(0,  16,	8,	uint8_t,		cfg->eth_addr[0])\
	_OP(0,  24,	8,	uint8_t,		cfg->eth_addr[1])\
	_OP(0,  32,	8,	uint8_t,		cfg->eth_addr[2])\
	_OP(0,  40,	8,	uint8_t,		cfg->eth_addr[3])\
	_OP(0,  48,	8,	uint8_t,		cfg->eth_addr[4])\
	_OP(0,  56,	8,	uint8_t,		cfg->eth_addr[5])\
	_OP(1,  0,	16,	uint16_t,			cfg->num_ifs)\
	_OP(1,  16,	4,	enum dpsw_fdb_entry_type,	cfg->type)\
/*TODO - array*/

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_SET_LEARNING_MODE(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)\
	_OP(0,  16,	4,	enum dpsw_fdb_learning_mode,	mode)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_CMD_FDB_GET_ATTR(_OP) \
	_OP(0,  0,	16,	uint16_t,			fdb_id)\

/*	param, offset, width,	type,				arg_name */
#define DPSW_RSP_FDB_GET_ATTR(_OP) \
	_OP(0,  16,	16,	uint16_t,	attr->num_fdb_entries)\
	_OP(0,  32,	16,	uint16_t,	attr->fdb_aging_time)\
	_OP(0,  48,	16,	uint16_t,	attr->num_fdb_static_entries)\
	
/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_SET_IRQ(_OP) \
	_OP(0,  0,	8,	uint8_t,		irq_index)\
	_OP(0,  32,	32,	uint32_t,		irq_val)\
	_OP(0,  0,	64,	uint64_t,		irq_paddr)

/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPSW_RSP_GET_IRQ(_OP) \
	_OP(0,	0,	32,	uint32_t,		irq_val) \
	_OP(1,	0,	64,	uint64_t,		irq_paddr)

/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_SET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPSW_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)
	
/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_SET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPSW_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) 

/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPSW_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \

/*	param, offset, width,	type,			arg_name */
#define DPSW_CMD_CLEAR_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

#endif /* __FSL_DPSW_CMD_H */
