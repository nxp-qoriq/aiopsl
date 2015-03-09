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

/* up to 256 bytes for segment for command data */
#define SNIC_CMD_NUM_OF_PARAMS	32

struct snic_cmd_data {
	uint64_t params[SNIC_CMD_NUM_OF_PARAMS];
};

/* cmd IDs */

/** snic enable flags */
#define SNIC_IPR_EN			0x0001
#define SNIC_VLAN_REMOVE_EN		0x0002
#define SNIC_VLAN_ADD_EN		0x0004
#define SNIC_IPF_EN			0x0008
#define SNIC_IPSEC_DECRYPT		0x0010
#define SNIC_IPSEC_ENCRYPT		0x0020

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
#define SNIC_CMDSZ_IPSEC_ADD_SA       (21*8)
#define SNIC_CMDSZ_IPSEC_DELETE_INSTANCE  8
#define SNIC_CMDSZ_IPSEC_DEL_SA       (7*8)

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

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_ADD_SA_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,	snic_id); \
	_OP(0, 16,	16,	uint16_t,	sa_id); \
	_OP(0, 32,	32,	int32_t,	ipsec_cfg->direction); \
	_OP(1, 0,	8,	uint8_t,	ipsec_encparams_cfg->ip_nh); \
	_OP(1, 8,	8,	uint8_t,	cipher_type); \
	_OP(1, 16,	16,	uint16_t,	ipsec_encparams_cfg->options); \
	_OP(1, 32,	32,	uint32_t,	ipsec_encparams_cfg->seq_num_ext_hi); \
	_OP(2, 0,	32,	uint32_t,	ipsec_encparams_cfg->seq_num); \
	_OP(2, 32,	32,	uint32_t,	ipsec_encparams_cfg->spi); \
	_OP(3, 0,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[0]); \
	_OP(3, 8,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[1]); \
	_OP(3, 16,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[2]); \
	_OP(3, 24,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[3]); \
	_OP(3, 32,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[4]); \
	_OP(3, 40,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[5]); \
	_OP(3, 48,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[6]); \
	_OP(3, 56,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[7]); \
	_OP(4, 0,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[8]); \
	_OP(4, 8,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[9]); \
	_OP(4, 16,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[10]); \
	_OP(4, 24,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[11]); \
	_OP(4, 32,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[12]); \
	_OP(4, 40,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[13]); \
	_OP(4, 48,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[14]); \
	_OP(4, 56,	8,	uint8_t,	ipsec_encap_cbc_cfg->iv[15]); \
	_OP(5, 0,	64,	uint64_t,	ipsec_encap_ctr_cfg->iv); \
	_OP(6, 0,	8,	uint8_t,	ipsec_encap_ctr_cfg->ctr_nonce[0]); \
	_OP(6, 8,	8,	uint8_t,	ipsec_encap_ctr_cfg->ctr_nonce[1]); \
	_OP(6, 16,	8,	uint8_t,	ipsec_encap_ctr_cfg->ctr_nonce[2]); \
	_OP(6, 24,	8,	uint8_t,	ipsec_encap_ctr_cfg->ctr_nonce[3]); \
	_OP(6, 32,	8,	uint8_t,	ipsec_encap_ccm_cfg->salt[0]); \
	_OP(6, 40,	8,	uint8_t,	ipsec_encap_ccm_cfg->salt[1]); \
	_OP(6, 48,	8,	uint8_t,	ipsec_encap_ccm_cfg->salt[2]); \
	_OP(6, 56,	8,	uint8_t,	ipsec_encap_ccm_cfg->salt[3]); \
	_OP(7, 0,	64,	uint64_t,	ipsec_encap_ccm_cfg->iv); \
	_OP(8, 0,	64,	uint64_t,	ipsec_encap_gcm_cfg->iv); \
	_OP(9, 0,	8,	uint8_t,	ipsec_encap_gcm_cfg->salt[0]); \
	_OP(9, 8,	8,	uint8_t,	ipsec_encap_gcm_cfg->salt[1]); \
	_OP(9, 16,	8,	uint8_t,	ipsec_encap_gcm_cfg->salt[2]); \
	_OP(9, 24,	8,	uint8_t,	ipsec_encap_gcm_cfg->salt[3]); \
	_OP(9, 32,	16,	uint16_t,	ipsec_decparams_cfg->options); \
	_OP(10, 0,	32,	uint32_t,	ipsec_decparams_cfg->seq_num_ext_hi); \
	_OP(10, 32,	32,	uint32_t,	ipsec_decparams_cfg->seq_num); \
	_OP(11, 0,	8,	uint8_t,	ipsec_decap_ctr_cfg->ctr_nonce[0]); \
	_OP(11, 8,	8,	uint8_t,	ipsec_decap_ctr_cfg->ctr_nonce[1]); \
	_OP(11, 16,	8,	uint8_t,	ipsec_decap_ctr_cfg->ctr_nonce[2]); \
	_OP(11, 24,	8,	uint8_t,	ipsec_decap_ctr_cfg->ctr_nonce[3]); \
	_OP(11, 32,	8,	uint8_t,	ipsec_decap_ccm_cfg->salt[0]); \
	_OP(11, 40,	8,	uint8_t,	ipsec_decap_ccm_cfg->salt[1]); \
	_OP(11, 48,	8,	uint8_t,	ipsec_decap_ccm_cfg->salt[2]); \
	_OP(11, 56,	8,	uint8_t,	ipsec_decap_ccm_cfg->salt[3]); \
	_OP(12, 0,	8,	uint8_t,	ipsec_decap_gcm_cfg->salt[0]); \
	_OP(12, 8,	8,	uint8_t,	ipsec_decap_gcm_cfg->salt[1]); \
	_OP(12, 16,	8,	uint8_t,	ipsec_decap_gcm_cfg->salt[2]); \
	_OP(12, 24,	8,	uint8_t,	ipsec_decap_gcm_cfg->salt[3]); \
	_OP(12, 32,	32,	uint32_t,	ipsec_cfg->cipherdata.algtype); \
	_OP(13, 0,	32,	uint32_t,	ipsec_cfg->cipherdata.keylen); \
	_OP(13, 32,	32,	uint32_t,	ipsec_cfg->cipherdata.key_enc_flags); \
	_OP(14, 0,	64,	uint64_t,	ipsec_cfg->cipherdata.key); \
	_OP(15, 0,	32,	uint32_t,	ipsec_cfg->authdata.algtype); \
	_OP(15, 32,	32,	uint32_t,	ipsec_cfg->authdata.keylen); \
	_OP(16, 0,	64,	uint64_t,	ipsec_cfg->authdata.key); \
	_OP(17, 0,	32,	uint32_t,	ipsec_cfg->authdata.key_enc_flags); \
	_OP(17, 32,	8,	uint8_t,	ipsec_dec_key[0]); \
	_OP(17, 40,	8,	uint8_t,	ipsec_dec_key[1]); \
	_OP(17, 48,	8,	uint8_t,	ipsec_dec_key[2]); \
	_OP(17, 56,	8,	uint8_t,	ipsec_dec_key[3]); \
	_OP(18, 0,	8,	uint8_t,	ipsec_dec_key[4]); \
	_OP(18, 8,	8,	uint8_t,	ipsec_dec_key[5]); \
	_OP(18, 16,	8,	uint8_t,	ipsec_dec_key[6]); \
	_OP(18, 24,	8,	uint8_t,	ipsec_dec_key[7]); \
	_OP(18, 32,	8,	uint8_t,	ipsec_dec_key[8]); \
	_OP(18, 40,	8,	uint8_t,	ipsec_dec_key[9]); \
	_OP(18, 48,	8,	uint8_t,	ipsec_dec_key[10]); \
	_OP(18, 56,	8,	uint8_t,	ipsec_dec_key[11]); \
	_OP(19, 0,	8,	uint8_t,	ipsec_dec_key[12]); \
	_OP(19, 8,	8,	uint8_t,	ipsec_dec_key[13]); \
	_OP(19, 16,	8,	uint8_t,	ipsec_dec_key[14]); \
	_OP(19, 24,	8,	uint8_t,	ipsec_dec_key[15]); \
	_OP(19, 32,	8,	uint8_t,	ipsec_dec_key[16]); \
	_OP(19, 40,	8,	uint8_t,	ipsec_dec_key[17]); \
	_OP(19, 48,	8,	uint8_t,	ipsec_dec_key[18]); \
	_OP(19, 56,	8,	uint8_t,	ipsec_dec_key[19]); \
	_OP(20, 0,	8,	uint8_t,	ipsec_dec_key[20]); \
	_OP(20, 8,	8,	uint8_t,	ipsec_dec_key[21]); \
	_OP(20, 16,	8,	uint8_t,	ipsec_dec_key[22]); \
	_OP(20, 24,	8,	uint8_t,	ipsec_dec_key[23]); \
	_OP(20, 32,	8,	uint8_t,	ipsec_dec_key[24]); \
	_OP(20, 40,	8,	uint8_t,	ipsec_dec_key[25]); \
	_OP(20, 48,	8,	uint8_t,	ipsec_dec_key[26]); \
	_OP(20, 56,	8,	uint8_t,	ipsec_dec_key[27]); \
	_OP(21, 0,	8,	uint8_t,	ipsec_dec_key[28]); \
	_OP(21, 8,	8,	uint8_t,	ipsec_dec_key[29]); \
	_OP(21, 16,	8,	uint8_t,	ipsec_dec_key[30]); \
	_OP(21, 24,	8,	uint8_t,	ipsec_dec_key[31]); \
	_OP(21, 32,	8,	uint8_t,	ipsec_dec_key[32]); \
	_OP(21, 40,	8,	uint8_t,	ipsec_dec_key[33]); \
	_OP(21, 48,	8,	uint8_t,	ipsec_dec_key[34]); \
	_OP(21, 56,	8,	uint8_t,	ipsec_dec_key[35]); \
	_OP(22, 0,	8,	uint8_t,	ipsec_dec_key[36]); \
	_OP(22, 8,	8,	uint8_t,	ipsec_dec_key[37]); \
	_OP(22, 16,	8,	uint8_t,	ipsec_dec_key[38]); \
	_OP(22, 24,	8,	uint8_t,	ipsec_dec_key[39]); \
	_OP(22, 32,	8,	uint8_t,	ipsec_dec_key[40]); \
	_OP(22, 40,	8,	uint8_t,	ipsec_dec_key[41]); \
	_OP(22, 48,	8,	uint8_t,	ipsec_dec_key[42]); \
	_OP(22, 56,	8,	uint8_t,	ipsec_dec_key[43]); \
	_OP(23, 0,	8,	uint8_t,	ipsec_dec_key[44]); \
	_OP(23, 8,	8,	uint8_t,	ipsec_dec_key[45]); \
	_OP(23, 16,	8,	uint8_t,	ipsec_dec_key[46]); \
	_OP(23, 24,	8,	uint8_t,	ipsec_dec_key[47]); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_DELETE_INSTANCE_CMD(_OP) \
	_OP(0, 0,	16,	uint16_t,	snic_id)

/*	param, offset, width,	type,			arg_name */
#define SNIC_IPSEC_DEL_SA_CMD(_OP) \
do { \
	_OP(0, 0,	16,	uint16_t,	snic_id); \
	_OP(0, 16,	16,	uint16_t,	sa_id); \
	_OP(0, 32,	32,	int32_t,	direction); \
	_OP(1, 0,	8,	uint8_t,	ipsec_dec_key[0]); \
	_OP(1, 8,	8,	uint8_t,	ipsec_dec_key[1]); \
	_OP(1, 16,	8,	uint8_t,	ipsec_dec_key[2]); \
	_OP(1, 24,	8,	uint8_t,	ipsec_dec_key[3]); \
	_OP(1, 32,	8,	uint8_t,	ipsec_dec_key[4]); \
	_OP(1, 40,	8,	uint8_t,	ipsec_dec_key[5]); \
	_OP(1, 48,	8,	uint8_t,	ipsec_dec_key[6]); \
	_OP(1, 56,	8,	uint8_t,	ipsec_dec_key[7]); \
	_OP(2, 0,	8,	uint8_t,	ipsec_dec_key[8]); \
	_OP(2, 8,	8,	uint8_t,	ipsec_dec_key[9]); \
	_OP(2, 16,	8,	uint8_t,	ipsec_dec_key[10]); \
	_OP(2, 24,	8,	uint8_t,	ipsec_dec_key[11]); \
	_OP(2, 32,	8,	uint8_t,	ipsec_dec_key[12]); \
	_OP(2, 40,	8,	uint8_t,	ipsec_dec_key[13]); \
	_OP(2, 48,	8,	uint8_t,	ipsec_dec_key[14]); \
	_OP(2, 56,	8,	uint8_t,	ipsec_dec_key[15]); \
	_OP(3, 0,	8,	uint8_t,	ipsec_dec_key[16]); \
	_OP(3, 8,	8,	uint8_t,	ipsec_dec_key[17]); \
	_OP(3, 16,	8,	uint8_t,	ipsec_dec_key[18]); \
	_OP(3, 24,	8,	uint8_t,	ipsec_dec_key[19]); \
	_OP(3, 32,	8,	uint8_t,	ipsec_dec_key[20]); \
	_OP(3, 40,	8,	uint8_t,	ipsec_dec_key[21]); \
	_OP(3, 48,	8,	uint8_t,	ipsec_dec_key[22]); \
	_OP(3, 56,	8,	uint8_t,	ipsec_dec_key[23]); \
	_OP(4, 0,	8,	uint8_t,	ipsec_dec_key[24]); \
	_OP(4, 8,	8,	uint8_t,	ipsec_dec_key[25]); \
	_OP(4, 16,	8,	uint8_t,	ipsec_dec_key[26]); \
	_OP(4, 24,	8,	uint8_t,	ipsec_dec_key[27]); \
	_OP(4, 32,	8,	uint8_t,	ipsec_dec_key[28]); \
	_OP(4, 40,	8,	uint8_t,	ipsec_dec_key[29]); \
	_OP(4, 48,	8,	uint8_t,	ipsec_dec_key[30]); \
	_OP(4, 56,	8,	uint8_t,	ipsec_dec_key[31]); \
	_OP(5, 0,	8,	uint8_t,	ipsec_dec_key[32]); \
	_OP(5, 8,	8,	uint8_t,	ipsec_dec_key[33]); \
	_OP(5, 16,	8,	uint8_t,	ipsec_dec_key[34]); \
	_OP(5, 24,	8,	uint8_t,	ipsec_dec_key[35]); \
	_OP(5, 32,	8,	uint8_t,	ipsec_dec_key[36]); \
	_OP(5, 40,	8,	uint8_t,	ipsec_dec_key[37]); \
	_OP(5, 48,	8,	uint8_t,	ipsec_dec_key[38]); \
	_OP(5, 56,	8,	uint8_t,	ipsec_dec_key[39]); \
	_OP(6, 0,	8,	uint8_t,	ipsec_dec_key[40]); \
	_OP(6, 8,	8,	uint8_t,	ipsec_dec_key[41]); \
	_OP(6, 16,	8,	uint8_t,	ipsec_dec_key[42]); \
	_OP(6, 24,	8,	uint8_t,	ipsec_dec_key[43]); \
	_OP(6, 32,	8,	uint8_t,	ipsec_dec_key[44]); \
	_OP(6, 40,	8,	uint8_t,	ipsec_dec_key[45]); \
	_OP(6, 48,	8,	uint8_t,	ipsec_dec_key[46]); \
	_OP(6, 56,	8,	uint8_t,	ipsec_dec_key[47]); \
} while (0)

#endif /* _FSL_SNIC_CMD_H */
