/**************************************************************************//*
 @File          fsl_snic_cmd.h

 @Description   defines sNIC commands between MC(client) & AIOPSL(server)

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_SNIC_CMD_H
#define _FSL_SNIC_CMD_H

#define SNIC_CMD_NUM_OF_PARAMS	16

struct snic_cmd_data {
	uint64_t params[SNIC_CMD_NUM_OF_PARAMS];
};

/* cmd IDs */

/** snic enable flags */
#define SNIC_IPR_EN			0x0001
#define SNIC_VLAN_REMOVE_EN		0x0002
#define SNIC_VLAN_ADD_EN		0x0004
#define SNIC_IPF_EN			0x0008

/* snic_cmds */
#define SNIC_SET_MTU             0
#define SNIC_IPR_CREATE_INSTANCE 1
#define SNIC_IPR_DELETE_INSTANCE 2
#define SNIC_ENABLE_FLAGS        3
#define SNIC_SET_QDID            4
#define SNIC_REGISTER            5
#define SNIC_UNREGISTER          6

/* todo cmd sizes */

/*	param, offset, width,	type,			arg_name */
#define SNIC_CMD_MTU(_OP) \
do { \
	_OP(0,  0,	16,	uint16_t,			snic_id); \
	_OP(0,  16,	16,	uint16_t,			ipf_mtu); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPR_CREATE_INSTANCE_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,			snic_id); \
	_OP(0, 16,	16,	uint16_t, ipr_params.max_reass_frm_size); \
	_OP(0, 32,	16,	uint16_t, ipr_params.min_frag_size_ipv4); \
	_OP(0, 48,	16,	uint16_t, ipr_params.min_frag_size_ipv6); \
	_OP(1, 0,	64,	uint64_t, ipr_params.extended_stats_addr); \
	_OP(2, 0,	32,	uint32_t, ipr_params.max_open_frames_ipv4 ); \
	_OP(2, 32,	32,	uint32_t, ipr_params.max_open_frames_ipv6 ); \
	_OP(3, 0,	16,	uint16_t, ipr_params.timeout_value_ipv4); \
	_OP(3, 16,	16,	uint16_t, ipr_params.timeout_value_ipv6); \
	_OP(3, 32,	32, ipr_timeout_cb_t *, ipr_params.ipv4_timeout_cb); \
	_OP(4, 0,	32, ipr_timeout_cb_t *, ipr_params.ipv6_timeout_cb); \
	_OP(4, 32,	32, 	uint32_t, ipr_params.flags); \
	_OP(5, 0,	64, ipr_timeout_arg_t, ipr_params.cb_timeout_ipv4_arg); \
	_OP(6, 0,	64, ipr_timeout_arg_t, ipr_params.cb_timeout_ipv6_arg); \
	_OP(7, 0,	0,	uint8_t, ipr_params.tmi_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPR_DELETE_INSTANCE_CMD(_OP) \
	_OP(0,  0,	16,	uint16_t,			snic_id)

/*	param, offset, width,	type,			arg_name */
#define SNIC_ENABLE_FLAGS_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,			snic_id); \
	_OP(0, 16,	16,	uint16_t,			snic_flags); \
} while (0)

#define SNIC_SET_QDID_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,			snic_id); \
	_OP(0, 16,	16,	uint16_t,			qdid); \
} while (0)

#define SNIC_REGISTER_CMD(_OP) \
do { \
	_OP(0, 0,	32,	uint32_t,		snic_ep_pc); \
	_OP(0, 32,	16,	uint16_t,		snic_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_UNREGISTER_CMD(_OP) \
	_OP(0,  0,	16,	uint16_t,			snic_id)

#endif /* _FSL_SNIC_CMD_H */
