/*
 * Copyright 2008-2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause or GPL-2.0+
 */

#ifndef __DESC_SRTP_H__
#define __DESC_SRTP_H__

#include "flib/rta.h"
#include "common.h"

/**
 * DOC: SRTP Shared Descriptor Constructors
 *
 * Shared descriptors for SRTP protocol.
 */

/*
 * SRTP Protocol Data Blocks
 */
#define SRTP_PDBOPTS_MKI	0x08
#define SRTP_PDBOPTS_AR		0x40
#define SRTP_CIPHER_SALT_LEN	14

struct srtp_encap_pdb {
	uint8_t x_len;			   /* RTP Extension length */
	uint8_t mki_len;		   /* MKI length */
	uint8_t n_tag;			   /* ICV length */
	uint8_t options;		   /* Options Byte */
	uint32_t cnst0;			   /* Constant Bits */
	uint8_t rsvd[2];		   /* Reserved Bits */
	uint16_t cnst1;			   /* Constant Bits */
	uint8_t salt[SRTP_CIPHER_SALT_LEN];/* Cipher Salt */
	uint16_t cnst2;			   /* Constant Bits */
	uint32_t rsvd1;			   /* Reserved Bits */
	uint32_t roc;			   /* Rollover Counter */
	uint32_t opt_mki;		   /* MKI */
};

struct srtp_decap_pdb {
	uint8_t x_len;			   /* RTP Extension length */
	uint8_t mki_len;		   /* MKI length */
	uint8_t n_tag;			   /* ICV length */
	uint8_t options;		   /* Options Byte */
	uint32_t cnst0;			   /* Constant Bits */
	uint8_t rsvd[2];		   /* Reserved Bits */
	uint16_t cnst1;			   /* Constant Bits */
	uint8_t salt[SRTP_CIPHER_SALT_LEN];/* Cipher Salt */
	uint16_t cnst2;			   /* Constant Bits */
	uint16_t rsvd1;			   /* Reserved Bits */
	uint16_t seq_num;		   /* Sequence Number */
	uint32_t roc;			   /* Rollover Counter */
	uint64_t antireplay_scorecard;	   /* Anti-replay Scorecard */
};

/**
 * cnstr_shdsc_srtp_encap - Function for creating a SRTP encapsulation
 *                          descriptor.
 * @descbuf: pointer to buffer used for descriptor construction
 * @authdata: pointer to authentication transform definitions
 * @cipherdata: pointer to block cipher transform definitions
 * @n_tag: value of ICV length
 * @roc: Rollover Counter
 * @cipher_salt: salt value
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_srtp_encap(uint32_t *descbuf,
					 struct alginfo *authdata,
					 struct alginfo *cipherdata,
					 uint8_t n_tag, uint32_t roc,
					 uint8_t *cipher_salt)
{
	struct program prg;
	struct program *p = &prg;
	uint32_t startidx;
	struct srtp_encap_pdb pdb;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);

	memset(&pdb, 0x00, sizeof(struct srtp_encap_pdb));
	pdb.n_tag = n_tag;
	pdb.roc = roc;
	memcpy(&pdb.salt, cipher_salt, SRTP_CIPHER_SALT_LEN);

	startidx = sizeof(struct srtp_encap_pdb) >> 2;

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	SHR_HDR(p, SHR_SERIAL, ++startidx, 0);
	COPY_DATA(p, (uint8_t *)&pdb, sizeof(struct srtp_encap_pdb));
	pkeyjmp = JUMP(p, keyjmp, LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
	KEY(p, MDHA_SPLIT_KEY, authdata->key_enc_flags, authdata->key,
	    authdata->keylen, INLINE_KEY(authdata));
	KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
	    cipherdata->keylen, INLINE_KEY(cipherdata));
	SET_LABEL(p, keyjmp);
	PROTOCOL(p, OP_TYPE_ENCAP_PROTOCOL,
		 OP_PCLID_SRTP,
		 OP_PCL_SRTP_AES_CTR | OP_PCL_SRTP_HMAC_SHA1_160);
	PATCH_JUMP(p, pkeyjmp, keyjmp);
	return PROGRAM_FINALIZE(p);
}

/**
 * cnstr_shdsc_srtp_decap - Function for creating a SRTP decapsulation
 *                          descriptor.
 * @descbuf: pointer to buffer used for descriptor construction
 * @authdata: pointer to authentication transform definitions
 * @cipherdata: pointer to block cipher transform definitions
 * @n_tag: value of ICV length
 * @roc: Rollover Counter
 * @seq_num: sequence number
 * @cipher_salt: salt value
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_srtp_decap(uint32_t *descbuf,
					 struct alginfo *authdata,
					 struct alginfo *cipherdata,
					 uint8_t n_tag, uint32_t roc,
					 uint16_t seq_num,
					 uint8_t *cipher_salt)
{
	struct program prg;
	struct program *p = &prg;
	struct srtp_decap_pdb pdb;
	uint32_t startidx;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);

	memset(&pdb, 0x00, sizeof(struct srtp_decap_pdb));
	pdb.n_tag = n_tag;
	pdb.seq_num = seq_num;
	pdb.roc = roc;
	memcpy(&pdb.salt, cipher_salt, SRTP_CIPHER_SALT_LEN);

	startidx = sizeof(struct srtp_decap_pdb) >> 2;

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	SHR_HDR(p, SHR_SERIAL, ++startidx, 0);
	{
		COPY_DATA(p, (uint8_t *)&pdb, sizeof(struct srtp_decap_pdb));
		pkeyjmp = JUMP(p, keyjmp, LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
		KEY(p, MDHA_SPLIT_KEY, authdata->key_enc_flags, authdata->key,
		    authdata->keylen, INLINE_KEY(authdata));
		KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
		    cipherdata->keylen, INLINE_KEY(cipherdata));
		SET_LABEL(p, keyjmp);
		PROTOCOL(p, OP_TYPE_DECAP_PROTOCOL,
			 OP_PCLID_SRTP,
			 OP_PCL_SRTP_AES_CTR | OP_PCL_SRTP_HMAC_SHA1_160);
	}
	PATCH_JUMP(p, pkeyjmp, keyjmp);
	return PROGRAM_FINALIZE(p);
}

#endif /* __DESC_SRTP_H__ */
