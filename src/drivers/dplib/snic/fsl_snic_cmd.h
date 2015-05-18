/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

#define SNIC_CMD_NUM_OF_PARAMS	24

struct snic_cmd_data {
	uint64_t params[SNIC_CMD_NUM_OF_PARAMS];
};

/* cmd IDs */

/** snic enable flags */
#define SNIC_IPR_EN			0x0001
#define SNIC_VLAN_REMOVE_EN		0x0002
#define SNIC_VLAN_ADD_EN		0x0004
#define SNIC_IPF_EN			0x0008
#define SNIC_IPSEC_EN			0x0010

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
#define SNIC_IPSEC_DEL_INSTANCE    9
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
#define SNIC_CMDSZ_IPSEC_CREATE_INSTANCE	8
#define SNIC_CMDSZ_IPSEC_ADD_SA		(19*8)
#define SNIC_CMDSZ_IPSEC_DELETE_INSTANCE  8
#define SNIC_CMDSZ_IPSEC_DEL_SA       (6*8)

/* ipsec "sa_selectors" mapping  (also used as ipsec SA "nic_options") */
#define SNIC_IPSEC_OPT_SEQ_NUM_ROLLOVER_EVENT		0x01
#define SNIC_IPSEC_OPT_INCLUDE_IP_SRC_IN_SA_SELECT	0x02
#define SNIC_IPSEC_OPT_INCLUDE_IP_DST_IN_SA_SELECT	0x04

/* ipsec SA "direction" mapping */
#define SNIC_IPSEC_SA_OUT		0
#define SNIC_IPSEC_SA_IN		1

/* ipsec SA "options" mapping */
#define SNIC_IPSEC_SA_OPT_EXT_SEQ_NUM		0x00000001
#define SNIC_IPSEC_SA_OPT_RND_GEN_IV		0x00000002
#define SNIC_IPSEC_SA_OPT_IPV6			0x00000004

/* ipsec SA "anti_replay" mapping */
#define SNIC_IPSEC_SA_ANTI_REPLAY_NONE		0x0
#define SNIC_IPSEC_SA_ANTI_REPLAY_WS_32		0x1
#define SNIC_IPSEC_SA_ANTI_REPLAY_WS_64		0x2
#define SNIC_IPSEC_SA_ANTI_REPLAY_WS_128	0x3


/* ipsec SA "cipher.alg" mapping */
#define SNIC_IPSEC_CIPHER_DES_IV64		0x1
#define SNIC_IPSEC_CIPHER_DES			0x2
#define SNIC_IPSEC_CIPHER_3DES			0x3
#define SNIC_IPSEC_CIPHER_NULL			0x4
#define SNIC_IPSEC_CIPHER_AES_CBC		0x5
#define SNIC_IPSEC_CIPHER_AES_CTR		0x6
#define SNIC_IPSEC_CIPHER_AES_CCM8		0x7
#define SNIC_IPSEC_CIPHER_AES_CCM12		0x8
#define SNIC_IPSEC_CIPHER_AES_CCM16		0x9
#define SNIC_IPSEC_CIPHER_AES_GCM8		0xA
#define SNIC_IPSEC_CIPHER_AES_GCM12		0xB
#define SNIC_IPSEC_CIPHER_AES_GCM16		0xC
#define SNIC_IPSEC_CIPHER_AES_NULL_WITH_GMAC	0xD

/* ipsec SA "auth_alg" mapping */
#define SNIC_IPSEC_AUTH_NULL			0x10
#define SNIC_IPSEC_AUTH_HMAC_MD5_96		0x11
#define SNIC_IPSEC_AUTH_HMAC_SHA1_96		0x12
#define SNIC_IPSEC_AUTH_AES_XCBC_MAC_96		0x13
#define SNIC_IPSEC_AUTH_HMAC_MD5_128		0x14
#define SNIC_IPSEC_AUTH_HMAC_SHA1_160		0x15
#define SNIC_IPSEC_AUTH_AES_CMAC_96		0x16
#define SNIC_IPSEC_AUTH_HMAC_SHA2_256_128	0x17
#define SNIC_IPSEC_AUTH_HMAC_SHA2_384_192	0x18
#define SNIC_IPSEC_AUTH_HMAC_SHA2_512_256	0x19

/* SA mode */
#define SNIC_IPSEC_SA_MODE_TRANSPORT		0x0
#define SNIC_IPSEC_SA_MODE_TUNNEL		0x1

struct snic_ipsec_cfg {
	uint8_t sa_selectors;
	uint8_t num_sa_ipv4;
	uint8_t num_sa_ipv6;
};

struct snic_ipsec_sa_in_cfg {
	uint8_t ip_src[16];	/* IP source for SA selection; required
				   only if DPNI_IPSEC_MATCH_IP_SRC is set;
				   Size of address depends on the setting of
				   DPNI_IPSEC_IPV6. */
	uint8_t ip_dst[16];	/* IP destination for SA selection; required
				   only if DPNI_IPSEC_MATCH_IP_SRC is set;
				   Size of address depends on the setting of
				   DPNI_IPSEC_IPV6. */
	uint8_t anti_replay; 	/* anti replay configuration */
};

struct snic_ipsec_sa_out_cfg {
	uint16_t frag_size;
	uint16_t outer_hdr_size; 	/**< Outer Header length in bytes
	                             	     (tunnel mode only).*/
	uint64_t outer_hdr_paddr; 	/**< Outer header content (tunnel mode only)*/
};

struct snic_ipsec_cipher_cfg {
	uint8_t alg;
	uint8_t iv[16];
	uint8_t nonce_or_salt[4];
	uint8_t key_size;
	uint64_t key_paddr; /* up to 64 byte key */
};

struct snic_ipsec_auth_cfg {
	uint8_t alg;
	uint8_t key_size;
	uint64_t key_paddr; /* up to 32 byte key */
};

struct snic_ipsec_sa_lifetime_limits {
	uint64_t soft_kb;	/**< Soft Kilobytes limit, in bytes. */
	uint64_t hard_kb; 	/**< Hard Kilobytes limit, in bytes. */
	uint64_t soft_packet; 	/**< Soft Packet count limit. */
	uint64_t hard_packet;		/**< Hard Packet count limit. */
	uint32_t soft_sec;	/**< Soft Seconds limit. */
	uint32_t hard_sec; 	/**< Hard Second limit. */
};

struct snic_ipsec_sa_cfg {
	uint32_t options;
	uint32_t spi;
	uint32_t seq_num;
	uint32_t seq_num_ext;
	uint8_t mode;
	uint8_t direction;
	struct snic_ipsec_sa_in_cfg in;
	struct snic_ipsec_sa_out_cfg out;
	struct snic_ipsec_cipher_cfg cipher;
	struct snic_ipsec_auth_cfg auth;
	struct snic_ipsec_sa_lifetime_limits lifetime;
};

struct snic_ipsec_sa_rmv_cfg {
	uint32_t options;
	uint32_t spi;
	uint8_t direction;
	uint8_t ip_src[16];	/* Relevant only if direction =
				   SNIC_IPSEC_SA_IN and
				   DPNI_IPSEC_MATCH_IP_SRC is set;
				   IP Source for SA selection;
				   Size of address depends on the setting of
				   DPNI_IPSEC_IPV6. */
	uint8_t ip_dst[16];	/* Relevant only if direction =
				   SNIC_IPSEC_SA_IN and
				   DPNI_IPSEC_MATCH_IP_DST is set;
				   IP Destination for SA selection;
				   Size of address depends on the setting of
				   DPNI_IPSEC_IPV6. */
} ;

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
	_OP(0, 8,	8,	uint8_t,	cfg->sa_selectors); \
	_OP(0, 24,	8,	uint8_t, 	cfg->num_sa_ipv4); \
	_OP(0, 32,	8,	uint8_t, 	cfg->num_sa_ipv6); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_ADD_SA_CMD(_OP, _OP2) \
do { \
	_OP(0, 0,	32,	uint32_t,	cfg->options); \
	_OP(0, 32,	32,	uint32_t,	cfg->spi); \
	_OP(1, 0,	32,	uint32_t,	cfg->seq_num); \
	_OP(1, 32,	32,	uint32_t,	cfg->seq_num_ext); \
	_OP(2, 0,	32,	uint32_t,	nic_options); \
	_OP(2, 32,	16,	uint16_t,	snic_id); \
	_OP(2, 48,	8,	uint8_t,	sa_id); \
	_OP(2, 56,	8,	uint8_t,	cfg->mode); \
	_OP(3, 0,	8,	uint8_t,	cfg->direction); \
	_OP(3, 8,	8,	uint8_t,	cfg->in.anti_replay); \
	_OP2\
	   (3, 16,	(16*8),	uint8_t,	cfg->in.ip_src); \
	_OP2\
	   (5, 16,	(16*8),	uint8_t,	cfg->in.ip_dst); \
	_OP(7, 16,	16,	uint16_t,	cfg->out.outer_hdr_size); \
	_OP(7, 32,	16,	uint16_t,	cfg->out.frag_size); \
	_OP(7, 48,	8,	uint8_t,	cfg->cipher.alg); \
	_OP(7, 56,	8,	uint8_t,	cfg->cipher.key_size); \
	_OP(8, 0,	64,	uint64_t,	cfg->cipher.key_paddr); \
	_OP(9, 0,	64,	uint64_t,	cfg->out.outer_hdr_paddr); \
	_OP2\
	   (10, 0,	(16*8),	uint8_t,	cfg->cipher.iv); \
	_OP2\
	   (12, 0,	(4*8),	uint8_t,	cfg->cipher.nonce_or_salt); \
        _OP(12, 32,	8,	uint8_t,	cfg->auth.alg); \
	_OP(12, 40,	8,	uint8_t,	cfg->auth.key_size); \
	_OP(13, 0,	64,	uint64_t,	cfg->auth.key_paddr); \
	_OP(14, 0,	64,	uint64_t,	cfg->lifetime.soft_kb); \
	_OP(15, 0,	64,	uint64_t,	cfg->lifetime.hard_kb); \
	_OP(16, 0,	64,	uint64_t,	cfg->lifetime.soft_packet); \
	_OP(17, 0,	64,	uint64_t,	cfg->lifetime.hard_packet); \
	_OP(18, 0,	32,	uint32_t,	cfg->lifetime.soft_sec); \
	_OP(18, 32,	32,	uint32_t,	cfg->lifetime.hard_sec); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_DELETE_INSTANCE_CMD(_OP) \
	_OP(0, 0,	16,	uint16_t,	snic_id)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_DEL_SA_CMD(_OP, _OP2) \
do { \
	_OP(0, 0,	16,	uint16_t,	snic_id); \
	_OP(0, 16,	8,	uint8_t,	sa_id); \
	_OP(0, 24,	8,	uint8_t,	cfg->direction); \
	_OP(0, 32,	32,	uint32_t,	cfg->options); \
	_OP(1, 0,	32,	uint32_t,	cfg->spi); \
	_OP(1, 32,	32,	uint32_t,	nic_options); \
	_OP2\
		(2, 0,	(16*8),	uint8_t,	cfg->ip_src); \
	_OP2\
		(4, 0,	(16*8),	uint8_t,	cfg->ip_dst); \
} while (0)

#endif /* _FSL_SNIC_CMD_H */
