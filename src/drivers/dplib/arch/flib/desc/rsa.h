/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_RSA_H__
#define __DESC_RSA_H__

#include "flib/rta.h"
#include "common.h"

/**
 * DOC: RSA Shared Descriptor Constructors
 *
 * Shared descriptors for RSA algorithm.
 */

/**
 * enum rsa_decrypt_form - Type selectors for decrypt forms in RSA protocol
 * @RSA_DECRYPT_FORM1: g, f, n, d
 * @RSA_DECRYPT_FORM2: g, f, d, p, q, tmp1, tmp2
 * @RSA_DECRYPT_FORM3: g, f, c, p, q, dp, dq, tmp1, tmp2
 */
enum rsa_decrypt_form {
	RSA_DECRYPT_FORM1 = 1,
	RSA_DECRYPT_FORM2,
	RSA_DECRYPT_FORM3
};

/*
 * RSA encryption/decryption PDB definitions.
 */

#define RSA_ENC_SGF_SHIFT	28
#define RSA_ENC_RSV_SHIFT	24
#define RSA_ENC_E_LEN_SHIFT	12

/**
 * struct rsa_encrypt_pdb_64b - RSA encryption PDB for 64 bits addresses
 * @header: contains sgf, rsv, #e, #n fields
 * @f_ref_high: reference to input (upper 32 bits)
 * @f_ref_low: reference to input (lower 32 bits)
 * @g_ref_high: reference to output (upper 32 bits)
 * @g_ref_low: reference to output (lower 32 bits)
 * @n_ref_high: reference to modulus (upper 32 bits)
 * @n_ref_low: reference to modulus (lower 32 bits)
 * @e_ref_high: reference to public key (upper 32 bits)
 * @e_ref_low: reference to public key (lower 32 bits)
 * @f_len: input length
 */
struct rsa_encrypt_pdb_64b {
	uint32_t header;
	uint32_t f_ref_high;
	uint32_t f_ref_low;
	uint32_t g_ref_high;
	uint32_t g_ref_low;
	uint32_t n_ref_high;
	uint32_t n_ref_low;
	uint32_t e_ref_high;
	uint32_t e_ref_low;
	uint32_t f_len;
};

/**
 * struct rsa_encrypt_pdb - RSA encryption PDB for 32 bits addresses
 * @header: contains sgf, rsv, #e, #n fields
 * @f_ref: reference to input
 * @g_ref: reference to output
 * @n_ref: reference to modulus
 * @e_ref: reference to public key
 * @f_len: input length
 */
struct rsa_encrypt_pdb {
	uint32_t header;
	uint32_t f_ref;
	uint32_t g_ref;
	uint32_t n_ref;
	uint32_t e_ref;
	uint32_t f_len;
};

#define RSA_DEC1_SGF_SHIFT	28
#define RSA_DEC1_RSV_SHIFT	24
#define RSA_DEC1_D_LEN_SHIFT	12

/**
 * struct rsa_dec_pdb_form1_64b - RSA decryption form1 PDB for 64 bits addresses
 * @header: contains sgf, rsv, #d, #n fields
 * @g_ref_high: reference to input (upper 32 bits)
 * @g_ref_low: reference to input (lower 32 bits)
 * @f_ref_high: reference to output (upper 32 bits)
 * @f_ref_low: reference to output (lower 32 bits)
 * @n_ref_high: reference to modulus (upper 32 bits)
 * @n_ref_low: reference to modulus (lower 32 bits)
 * @d_ref_high: reference to private key (upper 32 bits)
 * @d_ref_low: reference to private key (lower 32 bits)
 */
struct rsa_dec_pdb_form1_64b {
	uint32_t header;
	uint32_t g_ref_high;
	uint32_t g_ref_low;
	uint32_t f_ref_high;
	uint32_t f_ref_low;
	uint32_t n_ref_high;
	uint32_t n_ref_low;
	uint32_t d_ref_high;
	uint32_t d_ref_low;
};

/**
 * struct rsa_dec_pdb_form1 - RSA decryption form1 PDB for 32 bits addresses
 * @header: contains sgf, rsv, #d, #n fields
 * @g_ref: reference to input
 * @f_ref: reference to output
 * @n_ref: reference to modulus
 * @d_ref: reference to private key
 */
struct rsa_dec_pdb_form1 {
	uint32_t header;
	uint32_t g_ref;
	uint32_t f_ref;
	uint32_t n_ref;
	uint32_t d_ref;
};

#define RSA_DEC2_SGF_SHIFT	25
#define RSA_DEC2_D_LEN_SHIFT	12
#define RSA_DEC2_Q_LEN_SHIFT	12

/**
 * struct rsa_dec_pdb_form2_64b - RSA decryption form2 PDB for 64 bits addresses
 * @header: contains sgf, rsv, #d, #n fields
 * @g_ref_high: reference to input (upper 32 bits)
 * @g_ref_low: reference to input (lower 32 bits)
 * @f_ref_high: reference to output (upper 32 bits)
 * @f_ref_low: reference to output (lower 32 bits)
 * @d_ref_high: reference to private key (upper 32 bits)
 * @d_ref_low: reference to private key (lower 32 bits)
 * @p_ref_high: reference to prime p (upper 32 bits)
 * @p_ref_low: reference to prime p (lower 32 bits)
 * @q_ref_high: reference to prime q (upper 32 bits)
 * @q_ref_low: reference to prime q (lower 32 bits)
 * @tmp1_ref_high: reference to tmp1 (upper 32 bits)
 * @tmp1_ref_low: reference to tmp1 (lower 32 bits)
 * @tmp2_ref_high: reference to tmp2 (upper 32 bits)
 * @tmp2_ref_low: reference to tmp2 (lower 32 bits)
 * @trailer: contains rsv, #q, #p fields
 */
struct rsa_dec_pdb_form2_64b {
	uint32_t header;
	uint32_t g_ref_high;
	uint32_t g_ref_low;
	uint32_t f_ref_high;
	uint32_t f_ref_low;
	uint32_t d_ref_high;
	uint32_t d_ref_low;
	uint32_t p_ref_high;
	uint32_t p_ref_low;
	uint32_t q_ref_high;
	uint32_t q_ref_low;
	uint32_t tmp1_ref_high;
	uint32_t tmp1_ref_low;
	uint32_t tmp2_ref_high;
	uint32_t tmp2_ref_low;
	uint32_t trailer;
};

/**
 * struct rsa_dec_pdb_form2 - RSA decryption form2 PDB for 32 bits addresses
 * @header: contains sgf, rsv, #d, #n fields
 * @g_ref: reference to input
 * @f_ref: reference to output
 * @d_ref: reference to private key
 * @p_ref: reference to prime p
 * @q_ref: reference to prime q
 * @tmp1_ref: reference to tmp1
 * @tmp2_ref: reference to tmp2
 * @trailer: contains rsv, #q, #p fields
 */
struct rsa_dec_pdb_form2 {
	uint32_t header;
	uint32_t g_ref;
	uint32_t f_ref;
	uint32_t d_ref;
	uint32_t p_ref;
	uint32_t q_ref;
	uint32_t tmp1_ref;
	uint32_t tmp2_ref;
	uint32_t trailer;
};

#define RSA_DEC3_SGF_SHIFT	25
#define RSA_DEC3_Q_LEN_SHIFT	12

/**
 * struct rsa_dec_pdb_form3_64b - RSA decryption form3 PDB for 64 bits addresses
 * @header: contains sgf, rsv, #n fields
 * @g_ref_high: reference to input (upper 32 bits)
 * @g_ref_low: reference to input (lower 32 bits)
 * @f_ref_high: reference to output (upper 32 bits)
 * @f_ref_low: reference to output (lower 32 bits)
 * @c_ref_high: reference to c (upper 32 bits)
 * @c_ref_low: reference to c (lower 32 bits)
 * @p_ref_high: reference to prime p (upper 32 bits)
 * @p_ref_low: reference to prime p (lower 32 bits)
 * @q_ref_high: reference to prime q (upper 32 bits)
 * @q_ref_low: reference to prime q (lower 32 bits)
 * @dp_ref_high: reference to dp (upper 32 bits)
 * @dp_ref_low: reference to dp (lower 32 bits)
 * @dq_ref_high: reference to dq (upper 32 bits)
 * @dq_ref_low: reference to dq (lower 32 bits)
 * @tmp1_ref_high: reference to tmp1 (upper 32 bits)
 * @tmp1_ref_low: reference to tmp1 (lower 32 bits)
 * @tmp2_ref_high: reference to tmp2 (upper 32 bits)
 * @tmp2_ref_low: reference to tmp2 (lower 32 bits)
 * @trailer: contains rsv, #q, #p fields
 */
struct rsa_dec_pdb_form3_64b {
	uint32_t header;
	uint32_t g_ref_high;
	uint32_t g_ref_low;
	uint32_t f_ref_high;
	uint32_t f_ref_low;
	uint32_t c_ref_high;
	uint32_t c_ref_low;
	uint32_t p_ref_high;
	uint32_t p_ref_low;
	uint32_t q_ref_high;
	uint32_t q_ref_low;
	uint32_t dp_ref_high;
	uint32_t dp_ref_low;
	uint32_t dq_ref_high;
	uint32_t dq_ref_low;
	uint32_t tmp1_ref_high;
	uint32_t tmp1_ref_low;
	uint32_t tmp2_ref_high;
	uint32_t tmp2_ref_low;
	uint32_t trailer;
};

/**
 * struct rsa_dec_pdb_form3 - RSA decryption form3 PDB for 32 bits addresses
 * @header: contains sgf, rsv, #n fields
 * @g_ref: reference to input
 * @f_ref: reference to output
 * @c_ref: reference to c
 * @p_ref: reference to prime p
 * @q_ref: reference to prime q
 * @dp_ref: reference to dp
 * @dq_ref: reference to dq
 * @tmp1_ref: reference to tmp1
 * @tmp2_ref: reference to tmp2
 * @trailer: contains rsv, #q, #p fields
 */
struct rsa_dec_pdb_form3 {
	uint32_t header;
	uint32_t g_ref;
	uint32_t f_ref;
	uint32_t c_ref;
	uint32_t p_ref;
	uint32_t q_ref;
	uint32_t dp_ref;
	uint32_t dq_ref;
	uint32_t tmp1_ref;
	uint32_t tmp2_ref;
	uint32_t trailer;
};

/**
 * cnstr_shdsc_rsa - Function for creating a RSA encryption/decryption shared
 *                   descriptor. Supports decryption implemented in 3 forms.
 * @descbuf: pointer to buffer used for descriptor construction
 * @ps: if 36/40bit addressing is desired, this parameter must be true
 * @pdb: pointer to the Protocol Data Block to be used for descriptor
 *       construction. Must be mapped over a defined rsa structure. The PDB is
 *       assumed to be valid.
 * @pdb_len: size of the PDB in bytes
 * @protcmd: protocol Operation Command definitions
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_rsa(uint32_t *descbuf, bool ps, uint8_t *pdb,
				  unsigned pdb_len, struct protcmd *protcmd)
{
	struct program prg;
	struct program *p = &prg;

	LABEL(phdr);
	REFERENCE(pdbend);

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR(p);
	phdr = SHR_HDR(p, SHR_SERIAL, pdbend, 0);
	COPY_DATA(p, pdb, pdb_len);
	SET_LABEL(p, pdbend);
	PROTOCOL(p, protcmd->optype, protcmd->protid, protcmd->protinfo);
	PATCH_HDR(p, phdr, pdbend);
	return PROGRAM_FINALIZE(p);
}

#endif /* __DESC_RSA_H__ */
