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


#ifndef __DESC_SRTP_H__
#define __DESC_SRTP_H__

#include "flib/rta.h"
#include "common.h"

/**
 * @file                 srtp.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       SRTP Shared Descriptor Constructors
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
 * @defgroup sharedesc_group Shared Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of sharedesc_group */

/**
 * @details                  Function for creating a SRTP encapsulation
 *			     descriptor.
 * @ingroup                  sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *                           completion
 * @param[in] authdata       Pointer to authentication transform definitions.
 * @param[in] cipherdata     Pointer to block cipher transform definitions.
 * @param[in] n_tag          n_tag - value of ICV length
 * @param[in] roc            Rollover Counter
 * @param[in] cipher_salt    Salt value
 **/
static inline void cnstr_shdsc_srtp_encap(uint32_t *descbuf,
					  unsigned *bufsize,
					  struct alginfo *authdata,
					  struct alginfo *cipherdata,
					  uint8_t n_tag, uint32_t roc,
					  uint8_t *cipher_salt)
{
	struct program prg;
	struct program *program = &prg;
	uint32_t startidx;
	struct srtp_encap_pdb pdb;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);

	memset(&pdb, 0x00, sizeof(struct srtp_encap_pdb));
	pdb.n_tag = n_tag;
	pdb.roc = roc;
	memcpy(&pdb.salt, cipher_salt, SRTP_CIPHER_SALT_LEN);

	startidx = sizeof(struct srtp_encap_pdb) >> 2;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_SERIAL, ++startidx, 0);
	COPY_DATA((uint8_t *)&pdb, sizeof(struct srtp_encap_pdb));
	pkeyjmp = JUMP(IMM(keyjmp), LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
	KEY(MDHA_SPLIT_KEY, authdata->key_enc_flags, PTR(authdata->key),
	    authdata->keylen, IMMED);
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, IMMED);
	SET_LABEL(keyjmp);
	PROTOCOL(OP_TYPE_ENCAP_PROTOCOL,
		 OP_PCLID_SRTP,
		 OP_PCL_SRTP_AES_CTR | OP_PCL_SRTP_HMAC_SHA1_160);
	PATCH_JUMP(pkeyjmp, keyjmp);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                  Function for creating a SRTP decapsulation
 *			     descriptor.
 * @ingroup                  sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *                           completion
 * @param[in] authdata       Pointer to authentication transform definitions.
 * @param[in] cipherdata     Pointer to block cipher transform definitions.
 * @param[in] n_tag          n_tag - value of ICV length
 * @param[in] roc            Rollover Counter
 * @param[in] seq_num        Sequence Number
 * @param[in] cipher_salt    Salt value
 **/
static inline void cnstr_shdsc_srtp_decap(uint32_t *descbuf,
					  unsigned *bufsize,
					  struct alginfo *authdata,
					  struct alginfo *cipherdata,
					  uint8_t n_tag, uint32_t roc,
					  uint16_t seq_num,
					  uint8_t *cipher_salt)
{
	struct program prg;
	struct program *program = &prg;
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

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_SERIAL, ++startidx, 0);
	{
		COPY_DATA((uint8_t *)&pdb, sizeof(struct srtp_decap_pdb));
		pkeyjmp = JUMP(IMM(keyjmp), LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
		KEY(MDHA_SPLIT_KEY, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, IMMED);
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, IMMED);
		SET_LABEL(keyjmp);
		PROTOCOL(OP_TYPE_DECAP_PROTOCOL,
			 OP_PCLID_SRTP,
			 OP_PCL_SRTP_AES_CTR | OP_PCL_SRTP_HMAC_SHA1_160);
	}
	PATCH_JUMP(pkeyjmp, keyjmp);
	*bufsize = PROGRAM_FINALIZE();
}

#endif /* __DESC_SRTP_H__ */
