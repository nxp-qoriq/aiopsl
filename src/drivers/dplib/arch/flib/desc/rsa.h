
#ifndef __DESC_RSA_H__
#define __DESC_RSA_H__

#include "flib/rta.h"
#include "common.h"

/**
 * @file                 rsa.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       RSA Shared Descriptor Constructors
 */

/**
 * @defgroup descriptor_lib_group RTA Descriptors Library
 * @{
 */
/** @} end of descriptor_lib_group */

/**
 * @defgroup rsa_pdb rsa_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of rsa_pdb */

/**
 * @defgroup typedefs_group Auxiliary Data Structures
 * @ingroup descriptor_lib_group
 * @{
 */

/**
 * @enum      rsa_decrypt_form rsa.h
 * @details   Type selectors for decrypt forms in RSA protocol.
 */
enum rsa_decrypt_form {
	RSA_DECRYPT_FORM1 = 1,
	RSA_DECRYPT_FORM2,
	RSA_DECRYPT_FORM3
};

/** @} */ /* end of typedefs_group */

/*
 * RSA encryption/decryption PDB definitions.
 */

#define RSA_ENC_SGF_SHIFT	28
#define RSA_ENC_RSV_SHIFT	24
#define RSA_ENC_E_LEN_SHIFT	12

/**
 * @struct   rsa_encrypt_pdb_64b rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA encryption PDB for 64 bits addresses
 */
struct rsa_encrypt_pdb_64b {
	uint32_t header;	/* Contains sgf, rsv, #e, #n fields */
	uint32_t f_ref_high;	/* Reference to input */
	uint32_t f_ref_low;
	uint32_t g_ref_high;	/* Reference to output */
	uint32_t g_ref_low;
	uint32_t n_ref_high;	/* Reference to modulus */
	uint32_t n_ref_low;
	uint32_t e_ref_high;	/* Reference to public key */
	uint32_t e_ref_low;
	uint32_t f_len;		/* Input length */
};

/**
 * @struct   rsa_encrypt_pdb rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA encryption PDB for 32 bits addresses
 */
struct rsa_encrypt_pdb {
	uint32_t header;	/* Contains sgf, rsv, #e, #n fields */
	uint32_t f_ref;		/* Reference to input */
	uint32_t g_ref;		/* Reference to output */
	uint32_t n_ref;		/* Reference to modulus */
	uint32_t e_ref;		/* Reference to public key */
	uint32_t f_len;		/* Input length */
};

#define RSA_DEC1_SGF_SHIFT	28
#define RSA_DEC1_RSV_SHIFT	24
#define RSA_DEC1_D_LEN_SHIFT	12

/**
 * @struct   rsa_dec_pdb_form1_64b rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form1 PDB for 64 bits addresses
 */
struct rsa_dec_pdb_form1_64b {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref_high;	/* Reference to input */
	uint32_t g_ref_low;
	uint32_t f_ref_high;	/* Reference to output */
	uint32_t f_ref_low;
	uint32_t n_ref_high;	/* Reference to modulus */
	uint32_t n_ref_low;
	uint32_t d_ref_high;	/* Reference to private key */
	uint32_t d_ref_low;
};

/**
 * @struct   rsa_dec_pdb_form1 rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form1 PDB for 32 bits addresses
 */
struct rsa_dec_pdb_form1 {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref;		/* Reference to input */
	uint32_t f_ref;		/* Reference to output */
	uint32_t n_ref;		/* Reference to modulus */
	uint32_t d_ref;		/* Reference to private key */
};

#define RSA_DEC2_SGF_SHIFT	25
#define RSA_DEC2_D_LEN_SHIFT	12
#define RSA_DEC2_Q_LEN_SHIFT	12

/**
 * @struct   rsa_dec_pdb_form2_64b rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form2 PDB for 64 bits addresses
 */
struct rsa_dec_pdb_form2_64b {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref_high;	/* Reference to input */
	uint32_t g_ref_low;
	uint32_t f_ref_high;	/* Reference to output */
	uint32_t f_ref_low;
	uint32_t d_ref_high;	/* Reference to private key */
	uint32_t d_ref_low;
	uint32_t p_ref_high;	/* Reference to prime p */
	uint32_t p_ref_low;
	uint32_t q_ref_high;	/* Reference to prime q */
	uint32_t q_ref_low;
	uint32_t tmp1_ref_high;	/* Reference to tmp1 */
	uint32_t tmp1_ref_low;
	uint32_t tmp2_ref_high;	/* Reference to tmp2 */
	uint32_t tmp2_ref_low;
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

/**
 * @struct   rsa_dec_pdb_form2 rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form2 PDB for 32 bits addresses
 */
struct rsa_dec_pdb_form2 {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref;		/* Reference to input */
	uint32_t f_ref;		/* Reference to output */
	uint32_t d_ref;		/* Reference to private key */
	uint32_t p_ref;		/* Reference to prime p */
	uint32_t q_ref;		/* Reference to prime q */
	uint32_t tmp1_ref;	/* Reference to tmp1 */
	uint32_t tmp2_ref;	/* Reference to tmp2 */
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

#define RSA_DEC3_SGF_SHIFT	25
#define RSA_DEC3_Q_LEN_SHIFT	12

/**
 * @struct   rsa_dec_pdb_form3_64b rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form3 PDB for 64 bits addresses
 */
struct rsa_dec_pdb_form3_64b {
	uint32_t header;	/* Contains sgf, rsv, #n fields */
	uint32_t g_ref_high;	/* Reference to input */
	uint32_t g_ref_low;
	uint32_t f_ref_high;	/* Reference to output */
	uint32_t f_ref_low;
	uint32_t c_ref_high;	/* Reference to c */
	uint32_t c_ref_low;
	uint32_t p_ref_high;	/* Reference to prime p */
	uint32_t p_ref_low;
	uint32_t q_ref_high;	/* Reference to prime q */
	uint32_t q_ref_low;
	uint32_t dp_ref_high;	/* Reference to dp */
	uint32_t dp_ref_low;
	uint32_t dq_ref_high;	/* Reference to dq */
	uint32_t dq_ref_low;
	uint32_t tmp1_ref_high;	/* Reference to tmp1 */
	uint32_t tmp1_ref_low;
	uint32_t tmp2_ref_high;	/* Reference to tmp2 */
	uint32_t tmp2_ref_low;
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

/**
 * @struct   rsa_dec_pdb_form3 rsa.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form3 PDB for 32 bits addresses
 */
struct rsa_dec_pdb_form3 {
	uint32_t header;	/* Contains sgf, rsv, #n fields */
	uint32_t g_ref;		/* Reference to input */
	uint32_t f_ref;		/* Reference to output */
	uint32_t c_ref;		/* Reference to c */
	uint32_t p_ref;		/* Reference to prime p */
	uint32_t q_ref;		/* Reference to prime q */
	uint32_t dp_ref;	/* Reference to dp */
	uint32_t dq_ref;	/* Reference to dq */
	uint32_t tmp1_ref;	/* Reference to tmp1 */
	uint32_t tmp2_ref;	/* Reference to tmp2 */
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

/**
 * @defgroup sharedesc_group Shared Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of sharedesc_group */

/**
 * @details                  Function for creating a RSA encryption/decryption
 *                           shared descriptor.
 *                           Supports decryption implemented in 3 forms
 * @ingroup                  sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *                           completion
 *
 * @param [in] ps            If 36/40bit addressing is desired, this parameter
 *                           must be non-zero
 * @param[in] pdb            Pointer to the Protocol Data Block to be used for
 *                           descriptor construction. Must be mapped over a
 *                           defined rsa structure
 *                           The PDB is assumed to be valid
 * @param[in] pdb_len        Size of the PDB in bytes
 * @param[in] protcmd        Protocol Operation Command definitions
 **/
static inline void cnstr_shdsc_rsa(uint32_t *descbuf, unsigned *bufsize,
				   unsigned short ps, uint8_t *pdb,
				   unsigned pdb_len, struct protcmd *protcmd)
{
	struct program prg;
	struct program *program = &prg;

	LABEL(phdr);
	REFERENCE(pdbend);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	phdr = SHR_HDR(SHR_SERIAL, pdbend, WITH(0));
	COPY_DATA(pdb, pdb_len);
	SET_LABEL(pdbend);
	PROTOCOL(protcmd->optype, protcmd->protid, protcmd->protinfo);
	PATCH_HDR(phdr, pdbend);
	*bufsize = PROGRAM_FINALIZE();
}

#endif /* __DESC_RSA_H__ */
