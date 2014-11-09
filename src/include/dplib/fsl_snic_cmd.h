/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
#define SNIC_SEC_DECRYPT		0x0010
#define SNIC_SEC_ENCRYPT		0x0020

/* snic_cmds */
#define SNIC_SET_MTU               0
#define SNIC_IPR_CREATE_INSTANCE   1
#define SNIC_IPR_DELETE_INSTANCE   2
#define SNIC_ENABLE_FLAGS          3
#define SNIC_SET_QDID              4
#define SNIC_SET_SPID              5
#define SNIC_REGISTER              6
#define SNIC_UNREGISTER            7
#define SNIC_IPSEC_CREATE_INSTANCE 8
#define SNIC_IPSEC_DELETE_INSTANCE 9
#define SNIC_IPSEC_ADD_SA          10
#define SNIC_IPSEC_DEL_SA          11

/* todo cmd sizes */
#define SNIC_CMDSZ_SET_MTU             8
#define SNIC_CMDSZ_IPR_CREATE_INSTANCE (2*8)
#define SNIC_CMDSZ_IPR_DELETE_INSTANCE 8
#define SNIC_CMDSZ_ENABLE_FLAGS        8
#define SNIC_CMDSZ_SET_QDID            8
#define SNIC_CMDSZ_SET_SPID            8
#define SNIC_CMDSZ_REGISTER            8
#define SNIC_CMDSZ_UNREGISTER          8
#define SNIC_CMDSZ_IPSEC_CREATE_INSTANCE (4*8)

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

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_CREATE_INSTANCE_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,	snic_id); \
	_OP(0, 16,	8,	uint8_t,	fec_no); \
	_OP(0, 24,	8,	uint8_t,	key_size); \
	_OP(0, 32,	32,	uint32_t,	table_location); \
	_OP(1, 0,	32,	uint32_t,	dec_committed_sa_num); \
	_OP(1, 32,	32,	uint32_t, 	dec_max_sa_num); \
	_OP(2, 0,	32,	uint32_t,	enc_committed_sa_num); \
	_OP(2, 32,	32,	uint32_t, 	enc_max_sa_num); \
	_OP(3, 0,	8,	uint8_t, 	fec_array[0]); \
	_OP(3, 8,	8,	uint8_t, 	fec_array[1]); \
	_OP(3, 16,	8,	uint8_t, 	fec_array[2]); \
	_OP(3, 24,	8,	uint8_t, 	fec_array[3]); \
	_OP(3, 32,	8,	uint8_t, 	fec_array[4]); \
	_OP(3, 40,	8,	uint8_t, 	fec_array[5]); \
	_OP(3, 48,	8,	uint8_t, 	fec_array[6]); \
	_OP(3, 56,	8,	uint8_t, 	fec_array[7]); \
} while (0)

#endif /* _FSL_SNIC_CMD_H */
