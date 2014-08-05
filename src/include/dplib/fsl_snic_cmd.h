/**************************************************************************//*
 @File          fsl_snic_cmd.h

 @Description   defines sNIC commands between MC(client) & AIOPSL(server)

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_SNIC_CMD_H
#define _FSL_SNIC_CMD_H

#define SNIC_CMD_NUM_OF_PARAMS	8

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
#define SNIC_SET_SPID            5
#define SNIC_REGISTER            6
#define SNIC_UNREGISTER          7

/* todo cmd sizes */
#define SNIC_CMDSZ_SET_MTU             8
#define SNIC_CMDSZ_IPR_CREATE_INSTANCE (2*8)
#define SNIC_CMDSZ_IPR_DELETE_INSTANCE 8
#define SNIC_CMDSZ_ENABLE_FLAGS        8
#define SNIC_CMDSZ_SET_QDID            8
#define SNIC_CMDSZ_SET_SPID            8
#define SNIC_CMDSZ_REGISTER            8
#define SNIC_CMDSZ_UNREGISTER          8

/*	param, offset, width,	type,			arg_name */
#define SNIC_CMD_MTU(_OP) \
do { \
	_OP(0,  0,	16,	uint16_t,		snic_id); \
	_OP(0,  16,	16,	uint16_t,		ipf_mtu); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPR_CREATE_INSTANCE_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,		snic_id); \
	_OP(0, 16,	16,	uint16_t, cfg->max_reass_frm_size); \
	_OP(0, 32,	16,	uint16_t, cfg->min_frag_size_ipv4); \
	_OP(0, 48,	16,	uint16_t, cfg->min_frag_size_ipv6); \
	_OP(1, 0,	32,	uint32_t, cfg->max_open_frames_ipv4 ); \
	_OP(1, 32,	32,	uint32_t, cfg->max_open_frames_ipv6 ); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPR_DELETE_INSTANCE_CMD(_OP) \
	_OP(0,  0,	16,	uint16_t,		snic_id)

/*	param, offset, width,	type,			arg_name */
#define SNIC_ENABLE_FLAGS_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,		snic_id); \
	_OP(0, 16,	16,	uint16_t,		snic_flags); \
} while (0)

#define SNIC_SET_QDID_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,		snic_id); \
	_OP(0, 16,	16,	uint16_t,		qdid); \
} while (0)

#define SNIC_SET_SPID_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,		snic_id); \
	_OP(0, 16,	16,	uint16_t,		spid); \
} while (0)

#define SNIC_REGISTER_CMD(_OP) \
do { \
	_OP(0, 0,	32,	uint32_t,		snic_ep_pc); \
	_OP(0, 32,	16,	uint16_t,		snic_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_UNREGISTER_CMD(_OP) \
	_OP(0,  0,	16,	uint16_t,		snic_id)

#endif /* _FSL_SNIC_CMD_H */
