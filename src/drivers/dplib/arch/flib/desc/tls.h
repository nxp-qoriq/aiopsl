/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_TLS_H__
#define __DESC_TLS_H__

#include "flib/rta.h"
#include "common.h"

/**
 * DOC: SSL/TLS/DTLS Shared Descriptor Constructors
 *
 * Shared descriptors for SSL / TLS and DTLS protocols.
 */

/*
 * TLS family encapsulation/decapsulation PDB definitions.
 */

#define DTLS_PDBOPTS_ARS32	0x40	/* DTLS only */
#define DTLS_PDBOPTS_ARS64	0xc0	/* DTLS only */
#define TLS_PDBOPTS_OUTFMT	0x08
#define TLS_PDBOPTS_IV_WRTBK	0x02	/* TLS1.1/TLS1.2/DTLS only */
#define TLS_PDBOPTS_EXP_RND_IV	0x01	/* TLS1.1/TLS1.2/DTLS only */
#define TLS_PDBOPTS_TR_ICV	0x10	/* Available starting with SEC ERA 5 */

/**
 * struct tls_block_enc - SSL3.0/TLS1.0/TLS1.1/TLS1.2 block encapsulation PDB
 *                        part.
 * @type: protocol content type
 * @version: protocol version
 * @options: PDB options
 * @seq_num: protocol sequence number
 */
struct tls_block_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * struct dtls_block_enc - DTLS1.0 block encapsulation PDB part
 * @type: protocol content type
 * @version: protocol version
 * @options: PDB options
 * @epoch: protocol epoch
 * @seq_num: protocol sequence number
 */
struct dtls_block_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * struct tls_block_dec - SSL3.0/TLS1.0/TLS1.1/TLS1.2 block decapsulation PDB
 *                        part.
 * @rsvd: reserved, do not use
 * @options: PDB options
 * @seq_num: protocol sequence number
 */
struct tls_block_dec {
	uint8_t rsvd[3];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * struct dtls_block_dec - DTLS1.0 block decapsulation PDB part
 * @rsvd: reserved, do not use
 * @options: PDB options
 * @epoch: protocol epoch
 * @seq_num: protocol sequence number
 */
struct dtls_block_dec {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * struct tls_block_pdb - SSL3.0/TLS1.0/TLS1.1/TLS1.2/DTLS1.0 block
 *                        encapsulation / decapsulation PDB.
 * @iv: initialization vector
 * @end_index: the zero-length array expands with one/two words for the
 *             Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set in the
 *             DTLS1.0 decapsulation PDB Options byte.
 *             If SEC ERA is equal or greater than SEC ERA 5 and
 *             TLS_PDBOPTS_TR_ICV is set in the PDB Options Byte, it expands for
 *             ICVLen.
 */
struct tls_block_pdb {
	union {
		struct tls_block_enc tls_enc;
		struct dtls_block_enc dtls_enc;
		struct tls_block_dec tls_dec;
		struct dtls_block_dec dtls_dec;
	};
	uint32_t iv[4];
	uint32_t end_index[0];
};

/**
 * struct tls_stream_enc - SSL3.0/TLS1.0/TLS1.1/TLS1.2 stream encapsulation PDB
 *                         part.
 * @type: protocol content type
 * @version: protocol version
 * @options: PDB options
 */
struct tls_stream_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
};

/**
 * struct tls_stream_dec - SSL3.0/TLS1.0/TLS1.1/TLS1.2 stream decapsulation PDB
 *                         part.
 * @rsvd: reserved, do not use
 * @options: PDB options
 */
struct tls_stream_dec {
	uint8_t rsvd[3];
	uint8_t options;
};

/**
 * struct tls_stream_pdb - SSL3.0/TLS1.0/TLS1.1/TLS1.2 stream
 *                         encapsulation / decapsulation PDB.
 * @seq_num: protocol sequence number
 * @end_index: the zero-length array expands for ICVLen if SEC ERA is equal or
 *             greater than SEC ERA 5 and TLS_PDBOPTS_TR_ICV is set in the PDB
 *             Options Byte.
 */
struct tls_stream_pdb {
	union {
		struct tls_stream_enc enc;
		struct tls_stream_dec dec;
	};
	uint32_t seq_num[2];
	uint32_t end_index[0];
};

/**
 * struct tls_ctr_enc - TLS1.1/TLS1.2 AES CTR encapsulation PDB part
 * @type: protocol content type
 * @version: protocol version
 * @options: PDB options
 * @seq_num: protocol sequence number
 */
struct tls_ctr_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * struct tls_ctr - PDB part for TLS1.1/TLS1.2 AES CTR decapsulation and
 *                  DTLS1.0 AES CTR encapsulation/decapsulation.
 * @rsvd: reserved, do not use
 * @options: PDB options
 * @epoch: protocol epoch
 * @seq_num: protocol sequence number
 */
struct tls_ctr {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * struct tls_ctr_pdb - TLS1.1/TLS1.2/DTLS1.0 AES CTR
 *                      encapsulation / decapsulation PDB.
 * @write_iv: server write IV / client write IV
 * @constant: constant equal to 0x0000
 * @end_index: the zero-length array expands with one/two words for the
 *             Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set in the
 *             DTLS1.0 decapsulation PDB Options Byte.
 *             If TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte, it expands
 *             for ICVLen.
 *
 * TLS1.1/TLS1.2/DTLS1.0 AES CTR encryption processing is supported starting
 * with SEC ERA 5.
 */
struct tls_ctr_pdb {
	union {
		struct tls_ctr_enc tls_enc;
		struct tls_ctr ctr;
	};
	uint16_t write_iv[3];
	uint16_t constant;
	uint32_t end_index[0];
};

/**
 * struct tls12_gcm_encap - TLS1.2 AES GCM encapsulation PDB part
 * @type: protocol content type
 * @version: protocol version
 * @options: PDB options
 * @seq_num: protocol sequence number
 */
struct tls12_gcm_encap {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * struct tls12_gcm_decap - TLS1.2 AES GCM decapsulation PDB part
 * @rsvd: reserved, do not use
 * @options: PDB options
 * @seq_num: protocol sequence number
 */
struct tls12_gcm_decap {
	uint8_t rsvd[3];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * struct dtls_gcm - DTLS1.0 AES GCM encapsulation / decapsulation PDB part
 * @rsvd: reserved, do not use
 * @options: PDB options
 * @epoch: protocol epoch
 * @seq_num: protocol sequence number
 */
struct dtls_gcm {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * struct tls_gcm_pdb - TLS1.2/DTLS1.0 AES GCM encapsulation / decapsulation PDB
 * @salt: 4-byte salt
 * @end_index: the zero-length array expands with one/two words for the
 *             Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set in the
 *             DTLS1.0 decapsulation PDB Options byte.
 *             If SEC ERA is equal or greater than SEC ERA 5 and
 *             TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte, it expands for
 *             ICVLen.
 */
struct tls_gcm_pdb {
	union {
		struct tls12_gcm_encap tls12_enc;
		struct tls12_gcm_decap tls12_dec;
		struct dtls_gcm dtls;
	};
	uint32_t salt;
	uint32_t end_index[0];
};

/**
 * struct tls12_ccm_encap - TLS1.2 AES CCM encapsulation PDB part
 * @type: protocol content type
 * @version: protocol version
 * @options: PDB options
 * @seq_num: protocol sequence number
 */
struct tls12_ccm_encap {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * struct tls_ccm - PDB part for TLS12 AES CCM decapsulation PDB and
 *                  DTLS1.0 AES CCM encapsulation / decapsulation.
 * @rsvd: reserved, do not use
 * @options: PDB options
 * @epoch: protocol epoch
 * @seq_num: protocol sequence number
 */
struct tls_ccm {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * struct tls_ccm_pdb - TLS1.2/DTLS1.0 AES CCM encapsulation / decapsulation PDB
 * @write_iv: server write IV / client write IV
 * @b0_flags: use 0x5A for 8-byte ICV, 0x7A for 16-byte ICV
 * @ctr0_flags: equal to 0x2
 * @rsvd: reserved, do not use
 * @ctr0: CR0 lower 3 bytes, set to 0
 * @end_index: the zero-length array expands with one/two words for the
 *             Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set in the
 *             DTLS1.0 decapsulation PDB Options byte.
 *             If SEC ERA is equal or greater than SEC ERA 5 and
 *             TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte, it expands for
 *             ICVLen.
 */
struct tls_ccm_pdb {
	union {
		struct tls12_ccm_encap tls12;
		struct tls_ccm ccm;
	};
	uint32_t write_iv;
	uint8_t b0_flags;
	uint8_t ctr0_flags;
	uint8_t rsvd[3];
	uint8_t ctr0[3];
	uint32_t end_index[0];
};

/**
 * cnstr_shdsc_tls - TLS family block cipher encapsulation / decapsulation
 *                   shared descriptor.
 * @descbuf: pointer to buffer used for descriptor construction
 * @ps: if 36/40bit addressing is desired, this parameter must be true
 * @pdb: pointer to the PDB to be used in this descriptor
 *       This structure will be copied inline to the descriptor under
 *       construction. No error checking will be made. Refer to the block guide
 *       for details of the PDB.
 * @pdb_len: the length of the Protocol Data Block in bytes
 * @protcmd: pointer to Protocol Operation Command definitions
 * @cipherdata: pointer to block cipher transform definitions
 * @authdata: pointer to authentication transform definitions
 *
 * Return: size of descriptor written in words or negative number on error
 *
 * The following built-in protocols are supported:
 * SSL3.0 / TLS1.0 / TLS1.1 / TLS1.2 / DTLS10
 */
static inline int cnstr_shdsc_tls(uint32_t *descbuf, bool ps, uint8_t *pdb,
				  unsigned pdb_len, struct protcmd *protcmd,
				  struct alginfo *cipherdata,
				  struct alginfo *authdata)
{
	struct program prg;
	struct program *p = &prg;
	unsigned startidx;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);

	startidx = pdb_len >> 2;
	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR(p);
	SHR_HDR(p, SHR_SERIAL, ++startidx, 0);
	COPY_DATA(p, pdb, pdb_len);
	pkeyjmp = JUMP(p, keyjmp, LOCAL_JUMP, ALL_TRUE, BOTH|SHRD|SELF);
	/*
	 * SSL3.0 uses SSL-MAC (SMAC) instead of HMAC, thus MDHA Split Key
	 * does not apply.
	 */
	if (protcmd->protid == OP_PCLID_SSL30)
		KEY(p, KEY2, authdata->key_enc_flags, authdata->key,
		    authdata->keylen, INLINE_KEY(authdata));
	else
		KEY(p, MDHA_SPLIT_KEY, authdata->key_enc_flags, authdata->key,
		    authdata->keylen, INLINE_KEY(authdata));
	KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
	    cipherdata->keylen, INLINE_KEY(cipherdata));
	SET_LABEL(p, keyjmp);
	PROTOCOL(p, protcmd->optype, protcmd->protid, protcmd->protinfo);

	PATCH_JUMP(p, pkeyjmp, keyjmp);
	return PROGRAM_FINALIZE(p);
}

#endif /* __DESC_TLS_H__ */
