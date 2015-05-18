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
#define SNIC_CMDSZ_IPSEC_DELETE_INSTANCE  8
#define SNIC_CMDSZ_IPSEC_DEL_SA       (6*8)

/* ipsec "options" mapping  (also used as ipsec SA "nic_options") */
#define SNIC_IPSEC_OPT_SEQ_NUM_ROLLOVER_EVENT		0x0001
#define SNIC_IPSEC_OPT_INCLUDE_IP_SRC_IN_SA_SELECT	0x0002
#define SNIC_IPSEC_OPT_INCLUDE_IP_DST_IN_SA_SELECT	0x0004

/* ipsec SA "direction" mapping */
#define SNIC_IPSEC_SA_ENCAP		0
#define SNIC_IPSEC_SA_DECAP		1

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

struct snic_ipsec_cfg {
	uint16_t snic_id;  /* non-user */
	uint16_t options;
	uint8_t num_sa_ipv4;
	uint8_t num_sa_ipv6;
};

struct snic_ipsec_sa_dec_cfg {
	uint8_t ip_src[16];	/* IP source for SA selection; required
				   only if DPNI_IPSEC_MATCH_IP_SRC is set;
				   Size of address depends on the setting of
				   DPNI_IPSEC_IPV6. */
	uint8_t ip_dst[16];	/* IP destination for SA selection; required
				   only if DPNI_IPSEC_MATCH_IP_SRC is set;
				   Size of address depends on the setting of
				   DPNI_IPSEC_IPV6. */
	uint8_t anti_replay; 	/* anti replay configuration */
	uint8_t reserved[3]; 	/* anti replay configuration */
	union {
		struct {
			uint8_t nonce[4];
		} ctr;
		struct {
			uint8_t salt[4];
		} gcm;
		struct {
			uint8_t salt[4];
		} ccm;
	} alg;
};

struct snic_ipsec_sa_enc_cfg {
	union {
		struct {
			uint8_t iv[16];
		} cbc;
		struct {
			uint64_t iv;
			uint8_t nonce[4];
		} ctr;
		struct {
			uint64_t iv;
			uint8_t salt[4];
		} ccm;
		struct {
			uint64_t iv;
			uint8_t salt[4];
		} gcm;
	} alg;
};

struct snic_ipsec_sa_cipher_info {
	uint8_t alg;
	uint8_t key_size;
	uint8_t key[64];
};

struct snic_ipsec_sa_auth_info {
	uint8_t alg;
	uint8_t key_size;
	uint8_t key[32];
};

/* IMPORTANT!!! this struct may only be changed in accordance with FLIB, DPNI driver
 * corresponding adjustments, as well as AIOP adjustments.
 */
struct snic_ipsec_sa_cfg {
	uint32_t options;
	uint32_t spi;
	uint32_t seq_num;
	uint32_t seq_num_ext;
	uint16_t nic_options;  /* non-user */
	uint16_t snic_id;  /* non-user */
	uint8_t sa_id;  /* non-user */
	uint8_t direction;
	uint8_t reserved[2];
	struct snic_ipsec_sa_cipher_info cipher;
	struct snic_ipsec_sa_auth_info auth;
	union {
		struct snic_ipsec_sa_dec_cfg dec;
		struct snic_ipsec_sa_enc_cfg enc;
	} dir;

};

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
#define SNIC_IPSEC_DELETE_INSTANCE_CMD(_OP) \
	_OP(0, 0,	16,	uint16_t,	snic_id)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_DEL_SA_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,	snic_id); \
	_OP(0, 16,	8,	uint8_t,	sa_id); \
	_OP(0, 24,	8,	uint8_t,	direction); \
	_OP(0, 32,	32,	uint32_t,	sa_options); \
	_OP(1, 0,	32,	uint32_t,	spi); \
	_OP(1, 32,	16,	uint16_t,	sa_nic_options); \
	_OP(2, 0,	8,	uint8_t,	ip_src[0]); \
	_OP(2, 8,	8,	uint8_t,	ip_src[1]); \
	_OP(2, 16,	8,	uint8_t,	ip_src[2]); \
	_OP(2, 24,	8,	uint8_t,	ip_src[3]); \
	_OP(2, 32,	8,	uint8_t,	ip_src[4]); \
	_OP(2, 40,	8,	uint8_t,	ip_src[5]); \
	_OP(2, 48,	8,	uint8_t,	ip_src[6]); \
	_OP(2, 56,	8,	uint8_t,	ip_src[7]); \
	_OP(3, 0,	8,	uint8_t,	ip_src[8]); \
	_OP(3, 8,	8,	uint8_t,	ip_src[9]); \
	_OP(3, 16,	8,	uint8_t,	ip_src[10]); \
	_OP(3, 24,	8,	uint8_t,	ip_src[11]); \
	_OP(3, 32,	8,	uint8_t,	ip_src[12]); \
	_OP(3, 40,	8,	uint8_t,	ip_src[13]); \
	_OP(3, 48,	8,	uint8_t,	ip_src[14]); \
	_OP(3, 56,	8,	uint8_t,	ip_src[15]); \
	_OP(4, 0,	8,	uint8_t,	ip_dst[0]); \
	_OP(4, 8,	8,	uint8_t,	ip_dst[1]); \
	_OP(4, 16,	8,	uint8_t,	ip_dst[2]); \
	_OP(4, 24,	8,	uint8_t,	ip_dst[3]); \
	_OP(4, 32,	8,	uint8_t,	ip_dst[4]); \
	_OP(4, 40,	8,	uint8_t,	ip_dst[5]); \
	_OP(4, 48,	8,	uint8_t,	ip_dst[6]); \
	_OP(4, 56,	8,	uint8_t,	ip_dst[7]); \
	_OP(5, 0,	8,	uint8_t,	ip_dst[8]); \
	_OP(5, 8,	8,	uint8_t,	ip_dst[9]); \
	_OP(5, 16,	8,	uint8_t,	ip_dst[10]); \
	_OP(5, 24,	8,	uint8_t,	ip_dst[11]); \
	_OP(5, 32,	8,	uint8_t,	ip_dst[12]); \
	_OP(5, 40,	8,	uint8_t,	ip_dst[13]); \
	_OP(5, 48,	8,	uint8_t,	ip_dst[14]); \
	_OP(5, 56,	8,	uint8_t,	ip_dst[15]); \
} while (0)

#endif /* _FSL_SNIC_CMD_H */
