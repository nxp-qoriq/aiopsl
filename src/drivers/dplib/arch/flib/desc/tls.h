/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_TLS_H__
#define __DESC_TLS_H__

#include "flib/rta.h"
#include "common.h"

/**
 * @file                 tls.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       SSL/TLS/DTLS Shared Descriptor Constructors
 */

/**
 * @defgroup tls_pdb tls_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of tls_pdb */

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
 * @struct   tls_block_enc tls.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           SSL3.0/TLS1.0/TLS1.1/TLS1.2 block encapsulation PDB.
 */
struct tls_block_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   dtls_block_enc tls.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options, Epoch and Sequence Number fields for
 *           DTLS1.0 block encapsulation PDB.
 */
struct dtls_block_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_block_dec tls.h
 * @ingroup  tls_pdb
 * @details  Options and Sequence Number fields for SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           block decapsulation PDB.
 */
struct tls_block_dec {
	uint8_t rsvd[3];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   dtls_block_enc tls.h
 * @ingroup  tls_pdb
 * @details  Options, Epoch and Sequence Number fields for DTLS1.0 block
 *           decapsulation PDB.
 */
struct dtls_block_dec {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_block_pdb tls.h
 * @ingroup  tls_pdb
 * @details  SSL3.0/TLS1.0/TLS1.1/TLS1.2/DTLS1.0
 *           block encapsulation/decapsulation PDB.
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options byte.
 *           If SEC ERA is equal or greater than SEC ERA 5 and
 *           TLS_PDBOPTS_TR_ICV is set in the PDB Options Byte, the last member
 *           of the tls_block_pdb structure expands for ICVLen.
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
 * @struct   tls_stream_enc tls.h
 * @ingroup  tls_pdb
 * @details  Version and Options fields for SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           stream encapsulation PDB.
 */
struct tls_stream_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
};

/**
 * @struct   tls_stream_enc tls.h
 * @ingroup  tls_pdb
 * @details  Options field for SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           stream decapsulation PDB.
 */
struct tls_stream_dec {
	uint8_t rsvd[3];
	uint8_t options;
};

/**
 * @struct   tls_stream_pdb tls.h
 * @ingroup  tls_pdb
 * @details  SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           stream encapsulation/decapsulation PDB. The zero-length array
 *           expands for ICVLen if SEC ERA is equal or greater than SEC ERA 5
 *           and TLS_PDBOPTS_TR_ICV is set in the PDB Options Byte.
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
 * @struct   tls_ctr_enc tls.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           TLS1.1/TLS1.2 AES CTR encapsulation PDB.
 */
struct tls_ctr_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   tls_ctr tls.h
 * @ingroup  tls_pdb
 * @details  Options, Epoch and Sequence Number fields for
 *           TLS1.1/TLS1.2 AES CTR decapsulation PDB and
 *           DTLS1.0 AES CTR encapsulation/decapsulation PDB.
 */
struct tls_ctr {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_ctr_pdb tls.h
 * @ingroup  tls_pdb
 * @details  TLS1.1/TLS1.2/DTLS1.0 AES CTR encapsulation/decapsulation PDB.
 *           TLS1.1/TLS1.2/DTLS1.0 AES CTR encryption processing is supported
 *           starting with SEC ERA 5.
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options Byte.
 *           If TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte,
 *           the last member of the tls_block_pdb structure expands for ICVLen.
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
 * @struct   tls12_gcm_encap tls.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           TLS1.2 AES GCM encapsulation PDB.
 */
struct tls12_gcm_encap {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   tls12_gcm_decap tls.h
 * @ingroup  tls_pdb
 * @details  Options and Sequence Number fields for TLS1.2 AES GCM
 *           encapsulation PDB.
 */
struct tls12_gcm_decap {
	uint8_t rsvd[3];
	uint8_t options;
	uint32_t seq_num[2];
};
/**
 * @struct   dtls_gcm tls.h
 * @ingroup  tls_pdb
 * @details  Options and Epoch fields for DTLS1.0 AES GCM
 *           encapsulation/decapsulation PDB.
 */
struct dtls_gcm {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_gcm_pdb tls.h
 * @ingroup  tls_pdb
 * @details  TLS1.2/DTLS1.0 AES GCM encapsulation/decapsulation PDB
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options byte.
 *           If SEC ERA is equal or greater than SEC ERA 5 and
 *           TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte,
 *           the last member of the tls_block_pdb structure expands for ICVLen.
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
 * @struct   tls12_ccm_encap tls.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           TLS1.2 AES CCM encapsulation PDB.
 */
struct tls12_ccm_encap {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   tls_ccm tls.h
 * @ingroup  tls_pdb
 * @details  Options, Epoch and Sequence Number fields for
 *           TLS12 AES CCM decapsulation PDB and
 *           DTLS1.0 AES CCM encapsulation/decapsulation PDB.
 */
struct tls_ccm {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_ccm_pdb tls.h
 * @ingroup  tls_pdb
 * @details  TLS1.2/DTLS1.0 AES CCM encapsulation/decapsulation PDB.
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options byte.
 *           If SEC ERA is equal or greater than SEC ERA 5 and
 *           TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte,
 *           the last member of the tls_block_pdb structure expands for ICVLen.
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
 * @defgroup sharedesc_group Shared Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of sharedesc_group */

/**
 * @details  TLS family block cipher encapsulation/decapsulation
 *           shared descriptor.
 *           The following built-in protocols are supported:
 *           SSL3.0/TLS1.0/TLS1.1/TLS1.2/DTLS10.
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to buffer used for descriptor construction.
 * @param[in,out] bufsize   Pointer to descriptor size to be written back upon
 *                          completion.
 * @param[in] ps            If 36/40bit addressing is desired, this parameter
 *                          must be non-zero.
 * @param[in] pdb           Pointer to the PDB to be used in this descriptor.
 *                          This structure will be copied inline to the
 *                          descriptor under construction. No error checking
 *                          will be made. Refer to the block guide for details
 *                          of the PDB.
 * @param[in] pdb_len       The length of the Protocol Data Block in bytes.
 * @param[in] protcmd       Pointer to Protocol Operation Command definitions.
 * @param[in] cipherdata    Pointer to block cipher transform definitions.
 * @param[in] authdata      Pointer to authentication transform definitions.
 **/
static inline void cnstr_shdsc_tls(uint32_t *descbuf, unsigned *bufsize,
				   unsigned short ps, uint8_t *pdb,
				   unsigned pdb_len, struct protcmd *protcmd,
				   struct alginfo *cipherdata,
				   struct alginfo *authdata)
{
	struct program prg;
	struct program *program = &prg;
	unsigned startidx;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);

	startidx = pdb_len >> 2;
	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	SHR_HDR(SHR_SERIAL, ++startidx, 0);
	COPY_DATA(pdb, pdb_len);
	pkeyjmp = JUMP(IMM(keyjmp), LOCAL_JUMP, ALL_TRUE, BOTH|SHRD|SELF);
	/*
	 * SSL3.0 uses SSL-MAC (SMAC) instead of HMAC, thus MDHA Split Key
	 * does not apply.
	 */
	if (protcmd->protid == OP_PCLID_SSL30)
		KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, IMMED);
	else
		KEY(MDHA_SPLIT_KEY, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, IMMED);
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, IMMED);
	SET_LABEL(keyjmp);
	PROTOCOL(protcmd->optype, protcmd->protid, protcmd->protinfo);

	PATCH_JUMP(pkeyjmp, keyjmp);
	*bufsize = PROGRAM_FINALIZE();
}

#endif /* __DESC_TLS_H__ */
