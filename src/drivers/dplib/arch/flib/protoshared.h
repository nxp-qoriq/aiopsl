/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_PROTOSHARED_H__
#define __RTA_PROTOSHARED_H__

#include "rta.h"

/**
 * @file                 protoshared.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       Shared Descriptor Constructors
 */

/**
 * @defgroup descriptor_lib_group RTA Descriptors Library
 * @{
 */
/** @} end of descriptor_lib_group */

/**
 * @defgroup defines_group Auxiliary Defines
 * @ingroup descriptor_lib_group
 * @{
 */

/**
 * @enum      cipher_type_macsec protoshared.h
 * @details   Type selectors for cipher types in MACSEC protocol.
 */
enum cipher_type_macsec {
	MACSEC_CIPHER_TYPE_GCM,
	MACSEC_CIPHER_TYPE_GMAC
};

/**
 * @def CRC_8_ATM_POLY
 * This CRC Polynomial is used for the GMH Header Check Sequence.
 */
#define CRC_8_ATM_POLY			0x07000000

/**
 * @def WIMAX_GMH_EC_MASK
 * This mask is used in the WiMAX encapsulation/decapsulation descriptor
 * for setting/clearing the Encryption Control bit from the Generic Mac Header.
 */
#define WIMAX_GMH_EC_MASK		0x4000000000000000ull

/**
 * @def WIMAX_ICV_LEN
 * The length of the Integrity Check Value for WiMAX.
 */
#define WIMAX_ICV_LEN			0x0000000000000008ull

/**
 * @def WIMAX_FCS_LEN
 * The length of the Frame Check Sequence for WiMAX.
 */
#define WIMAX_FCS_LEN			0x00000000000000004ull

/**
 * @def WIMAX_PN_LEN
 * The length of the Packet Number for WiMAX.
 */
#define WIMAX_PN_LEN			0x0000000000000004ull

/**
 * @def PDCP_NULL_MAX_FRAME_LEN
 * The maximum frame frame length that is supported by PDCP NULL protocol.
 */
#define PDCP_NULL_MAX_FRAME_LEN		0x00002FFF

/**
 * @def PDCP_MAC_I_LEN
 * The length of the MAC-I for PDCP protocol operation.
 */
#define PDCP_MAC_I_LEN			0x00000004

/**
 * #def PDCP_MAX_FRAME_LEN_STATUS
 * The status returned in FD status/command field in case the input frame
 * is larger than PDCP_NULL_MAX_FRAME_LEN
 */
#define PDCP_MAX_FRAME_LEN_STATUS	0xF1

/**
 * @def PDCP_C_PLANE_SN_MASK
 * This mask is used in the PDCP descriptors for extracting the sequence
 * number (SN) from the PDCP Control Plane header. For PDCP Control Plane,
 * the SN is constant (5 bits) as opposed to PDCP Data Plane (7/12/15 bits).
 */
#define PDCP_C_PLANE_SN_MASK		0x0000001F

/**
 * @def PDCP_U_PLANE_15BIT_SN_MASK
 * This mask is used in the PDCP descriptors for extracting the sequence
 * number (SN) from the PDCP User Plane header. For PDCP Control Plane,
 * the SN is constant (5 bits) as opposed to PDCP Data Plane (7/12/15 bits).
 */
#define PDCP_U_PLANE_15BIT_SN_MASK	0x00007FFF

/**
 * @def PDCP_BEARER_MASK
 * This mask is used masking out the bearer for PDCP processing with SNOW f9
 * in LTE. The value on which this mask is applied is formatted as below:
 *
 *     Count-C (32 bit) | Bearer (5 bit) | Direction (1 bit) | 0 (26 bits)
 *
 * Applying this mask is done for creating the upper 64 bits of the IV needed
 * for SNOW f9.
 *
 * The lower 32 bits of the mask are used for masking the direction for AES
 * CMAC IV.
 */
#define PDCP_BEARER_MASK		0xFFFFFFFF04000000ull

/**
 * @def PDCP_DIR_MASK
 * This mask is used masking out the direction for PDCP processing with SNOW f9
 * in LTE. The value on which this mask is applied is formatted as below:
 *
 *     Bearer (5 bit) | Direction (1 bit) | 0 (26 bits)
 *
 * Applying this mask is done for creating the lower 32 bits of the IV needed
 * for SNOW f9.
 *
 * The upper 32 bits of the mask are used for masking the direction for AES
 * CMAC IV.
 */
#define PDCP_DIR_MASK			0xF800000000000000ull

/**
 * @def PDCP_NULL_INT_MAC_I_VAL
 * The value of the PDCP PDU MAC-I in case NULL integrity is used
 */

#define PDCP_NULL_INT_MAC_I_VAL		0x00000000

/**
 * @def PDCP_NULL_INT_ICV_CHECK_FAILED_STATUS
 * The status used to report ICV check failed in case of NULL integrity
 * Control Plane processing
 */
#define PDCP_NULL_INT_ICV_CHECK_FAILED_STATUS	0x0A
/**
 * @def PDCP_DPOVRD_HFN_OV_EN
 * Value to be used in the FD status/cmd field to indicate the HFN override
 * mechanism is active for the frame.
 */
#define PDCP_DPOVRD_HFN_OV_EN		0x80000000

/**
 * @def PDCP_P4080REV2_HFN_OV_BUFLEN
 * The length in bytes of the supplementary space that must be provided by the
 * user at the beginning of the input frame buffer for P4080 REV 2. The format
 * of the frame buffer is the following:
 *
 *  |<---PDCP_P4080REV2_HFN_OV_BUFLEN-->|
 * //===================================||============||==============\\
 * || PDCP_DPOVRD_HFN_OV_EN | HFN value || PDCP Header|| PDCP Payload ||
 * \\===================================||============||==============//
 *
 * If HFN override mechanism is not desired, then the MSB of the first 4 bytes
 * must be set to 0b.
 */
#define PDCP_P4080REV2_HFN_OV_BUFLEN	4

/**
 * @def MBMS_HEADER_POLY
 * CRC6 polynomial for MBMS PDU Header = D^6 + D^5 + D^3 + D^2 + D^1 + 1;
 */
#define MBMS_HEADER_POLY	0xBC000000

/**
 * @def MBMS_PAYLOAD_POLY
 * CRC10 polynomial for MBMS PDU Header = D^10 + D^9 + D^5 + D^4 + D^1+ 1;
 */
#define MBMS_PAYLOAD_POLY	0x8CC00000

/**
 * @def MBMS_TYPE0_HDR_LEN
 * The length of a MBMS Type 0 PDU header
 */
#define MBMS_TYPE0_HDR_LEN	18

/**
 * @def MBMS_TYPE1_HDR_LEN
 * The length of a MBMS Type 1 PDU header
 */
#define MBMS_TYPE1_HDR_LEN	11

/**
 * @def MBMS_TYPE3_HDR_LEN
 * The length of a MBMS Type 3 PDU header
 */
#define MBMS_TYPE3_HDR_LEN	19

/**
 * @def DUMMY_BUF_BASE
 * A dummy address used as immediate value when reading the parser result
 * from before the frame buffer
 */
#define DUMMY_BUF_BASE		0xDEADC000

/**
 * @def FM_RX_PRIV_SIZE
 * Size of the private part, reserved for DPA ETH in the buffer before the frame
 */
#define FM_RX_PRIV_SIZE		0x10

/**
 * @def FM_RX_EXTRA_HEADROOM
 * The size of the extra space reserved by Frame Manager at the beginning of
 * a data buffer on the receive path
 */
#define FM_RX_EXTRA_HEADROOM	0x40

/**
 * @def IC_PR_OFFSET
 * Offset of the Parser Results field in the Internal Context field
 */
#define IC_PR_OFFSET		0x20

/**
 * @def PR_L4_OFFSET
 * Offset of the L4 header offset result in the Parser Results field
 */
#define PR_L4_OFFSET		0x1E

/**
 * @def BUF_IC_OFFSET
 * Offset of the Internal Context in the buffer before the frame
 */
#define BUF_IC_OFFSET		(FM_RX_PRIV_SIZE + FM_RX_EXTRA_HEADROOM)

/**
 * @def BUF_PR_OFFSET
 * Offset of the Parser Results in the buffer before the frame
 */
#define BUF_PR_OFFSET		(BUF_IC_OFFSET + IC_PR_OFFSET)

/**
 * @def BUF_L4_OFFSET
 * Offset of the L4 header offset in the buffer before the frame
 */
#define BUF_L4_OFFSET		(BUF_PR_OFFSET + PR_L4_OFFSET)

/**
 * @def UDP_HDR_LEN
 * The length of the UDP header
 */
#define UDP_HDR_LEN		8

/**
 * @def GTP_HDR_LEN
 * The length of the GTP header with no options and no sequence number
 */
#define GTP_HDR_LEN		8

/**
 * @def MBMS_HDR_OFFSET
 * MBMS header offset in the frame buffer
 */
#define MBMS_HDR_OFFSET		(UDP_HDR_LEN + GTP_HDR_LEN)

/**
 * @def MBMS_CRC_HDR_FAIL
 * Status returned by SEC in case the header CRC of the MBMS PDU failed
 */
#define MBMS_CRC_HDR_FAIL	0xAA

/**
 * @def MBMS_CRC_PAYLOAD_FAIL
 * Status returned by SEC in case the payload CRC of the MBMS PDU failed
 */
#define MBMS_CRC_PAYLOAD_FAIL	0xAB

/** @} */ /* end of defines_group */
/**
 * @defgroup typedefs_group Auxiliary Data Structures
 * @ingroup descriptor_lib_group
 * @{
 */

/**
 * @struct    alginfo protoshared.h
 * @details   Container for IPsec algorithm details
 */
struct alginfo {
	uint32_t algtype;  /**< Algorithm selector. For valid values, see
				documentation of the functions where it is
				used */
	uint64_t key;      /**< Address where algorithm key resides */
	uint32_t keylen;   /**< Length of the provided key, in bytes */
	uint32_t key_enc_flags; /**< Key encryption flags */
};

/**
 * @struct    protcmd protoshared.h
 * @details   Container for Protocol Operation Command fields.
 */
struct protcmd {
	uint32_t optype;    /**< Command type. */
	uint32_t protid;    /**< Protocol Identifier */
	uint16_t protinfo;  /**< Protocol Information. */
};

/**
 * @struct    mbms_type_0_pdb protoshared.h
 * @details   Container for MBMS Type 0 PDB
 */
struct mbms_type_0_pdb {
	uint32_t crc_header_fail;
};

/**
 * @struct    mbms_type_1_3_pdb protoshared.h
 * @details   Container for MBMS Type 1 and Type 3 PDB
 */
struct mbms_type_1_3_pdb {
	uint32_t crc_header_fail;
	uint32_t crc_payload_fail;
};

/**
 * @enum      cipher_type_pdcp protoshared.h
 * @details   Type selectors for cipher types in PDCP protocol OP instructions.
 */
enum cipher_type_pdcp {
	PDCP_CIPHER_TYPE_NULL,
	PDCP_CIPHER_TYPE_SNOW,
	PDCP_CIPHER_TYPE_AES,
	PDCP_CIPHER_TYPE_ZUC,
	PDCP_CIPHER_TYPE_INVALID
};

/**
 * @enum       auth_type_pdcp protoshared.h
 * @details    Type selectors for integrity types in PDCP protocol OP
 *             instructions.
 */
enum auth_type_pdcp {
	PDCP_AUTH_TYPE_NULL,
	PDCP_AUTH_TYPE_SNOW,
	PDCP_AUTH_TYPE_AES,
	PDCP_AUTH_TYPE_ZUC,
	PDCP_AUTH_TYPE_INVALID
};

/**
 * @enum       pdcp_dir protoshared.h
 * @details    Type selectors for direction for PDCP protocol.
 */
enum pdcp_dir {
	PDCP_DIR_UPLINK = 0,
	PDCP_DIR_DOWNLINK = 1,
	PDCP_DIR_INVALID
};

/**
 * @enum     pdcp_plane protoshared.h
 * @details    PDCP domain selectors.
 */
enum pdcp_plane {
	PDCP_CONTROL_PLANE,
	PDCP_DATA_PLANE,
	PDCP_SHORT_MAC
};

/**
 * @enum     pdcp_sn_size protoshared.h
 * @details  Sequence Number Size selectors for PDCP protocol
 */
enum pdcp_sn_size {
	PDCP_SN_SIZE_5 = 5,
	PDCP_SN_SIZE_7 = 7,
	PDCP_SN_SIZE_12 = 12,
	PDCP_SN_SIZE_15 = 15
};

/**
 * @enum      rsa_decrypt_form protoshared.h
 * @details   Type selectors for decrypt forms in RSA protocol.
 */
enum rsa_decrypt_form {
	RSA_DECRYPT_FORM1 = 1,
	RSA_DECRYPT_FORM2,
	RSA_DECRYPT_FORM3
};

/**
 * @enum      ipsec_icv_size protoshared.h
 * @details   Type selectors for icv size in IPsec protocol.
 */
enum ipsec_icv_size {
	IPSEC_ICV_MD5_SIZE = 16,
	IPSEC_ICV_MD5_TRUNC_SIZE = 12
};

/**
 * @enum      mbms_pdu_type protoshared.h
 * @details   Type selectors for MBMS PDUs in SYNC protocol
 */
enum mbms_pdu_type {
	MBMS_PDU_TYPE0,
	MBMS_PDU_TYPE1,
	MBMS_PDU_TYPE2,	/* Should never reach SEC */
	MBMS_PDU_TYPE3,
	MBMS_PDU_TYPE_INVALID
};

/** @} */ /* end of typedefs_group */
/**
 * @defgroup sharedesc_group Shared Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of sharedesc_group */

/**
 * @details                  SNOW/f8 (UEA2) as a shared descriptor.
 * @ingroup                  sharedesc_group
 *
 * @param [in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param [in,out] bufsize   Points to size to be updated at completion.
 * @param [in] cipherdata    Pointer to block cipher transform definitions.
 * @param [in] dir           Cipher direction (DIR_ENCRYPT/DIR_DECRYPT).
 * @param [in] count         UEA2 count value (32 bits).
 * @param [in] bearer        UEA2 bearer ID (5 bits).
 * @param [in] direction     UEA2 direction (1 bit).
 *
 */
static inline void cnstr_shdsc_snow_f8(uint32_t *descbuf, unsigned *bufsize,
			 struct alginfo *cipherdata, uint8_t dir,
			 uint32_t count, uint8_t bearer, uint8_t direction)
{
	struct program prg;
	struct program *program = &prg;
	uint64_t ct = count;
	uint64_t br = bearer;
	uint64_t dr = direction;
	uint64_t context = (ct << 32) | (br << 27) | (dr << 26);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_ALWAYS, 1, 0);
	{
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, IMMED);
		MATHB(SEQINSZ, SUB, MATH2, VSEQINSZ, SIZE(4), 0);
		MATHB(SEQINSZ, SUB, MATH2, VSEQOUTSZ, SIZE(4), 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F8, OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL, 0, dir);
		LOAD(IMM(context), CONTEXT1, 0, SIZE(8), 0);
		SEQFIFOLOAD(MSG1, SIZE(32), WITH(VLF | LAST1 | LAST2));
		SEQFIFOSTORE(MSG, 0, SIZE(32), WITH(VLF));
	}
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                 SNOW/f9 (UIA2) as a shared descriptor.
 * @ingroup                 sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Points to size to be updated at completion.
 * @param[in] authdata      Pointer to authentication transform definitions.
 * @param[in] dir           Cipher direction (DIR_ENCRYPT/DIR_DECRYPT).
 * @param[in] count         UEA2 count value (32 bits).
 * @param[in] fresh         UEA2 fresh value ID (32 bits).
 * @param[in] direction     UEA2 direction (1 bit).
 * @param[in] datalen       Size of data.
 */
static inline void cnstr_shdsc_snow_f9(uint32_t *descbuf, unsigned *bufsize,
			 struct alginfo *authdata, uint8_t dir, uint32_t count,
			 uint32_t fresh, uint8_t direction, uint32_t datalen)
{
	struct program prg;
	struct program *program = &prg;
	uint64_t ct = count;
	uint64_t fr = fresh;
	uint64_t dr = direction;
	uint64_t context[2];

	context[0] = (ct << 32) | (dr << 26);
	context[1] = fr << 32;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_ALWAYS, 1, 0);
	{
		KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, IMMED);
		MATHB(SEQINSZ, SUB, MATH2, VSEQINSZ, SIZE(4), 0);
		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F9, OP_ALG_AAI_F9,
			      OP_ALG_AS_INITFINAL, 0, dir);
		LOAD(PTR((uintptr_t)context), CONTEXT2, 0, SIZE(16), IMMED);
		SEQFIFOLOAD(BIT_DATA, datalen, WITH(CLASS2 | LAST1 | LAST2));
		/* Save lower half of MAC out into a 32-bit sequence */
		SEQSTORE(CONTEXT2, 0, SIZE(4), 0);
	}
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                 CBC block cipher
 * @ingroup                 sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Limit/returned descriptor buffer size.
 * @param[in] cipherdata    Pointer to block cipher transform definitions.
 * @param[in] iv            IV data.
 * @param[in] ivlen         IV length.
 * @param[in] dir           DIR_ENCRYPT/DIR_DECRYPT.
 * @param[in] cipher        OP_ALG_ALGSEL_AES/DES/3DES.
 */
static inline void cnstr_shdsc_cbc_blkcipher(uint32_t *descbuf,
			       unsigned *bufsize, struct alginfo *cipherdata,
			       uint8_t *iv, uint32_t ivlen, uint8_t dir,
			       uint32_t cipher)
{
	struct program prg;
	struct program *program = &prg;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_ALWAYS, 1, SC);
	{
		/* Insert Key */
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, IMMED);
		MATHB(SEQINSZ, SUB, MATH2, VSEQINSZ, SIZE(4), 0);
		MATHB(SEQINSZ, SUB, MATH2, VSEQOUTSZ, SIZE(4), 0);
		ALG_OPERATION(cipher, OP_ALG_AAI_CBC, OP_ALG_AS_INIT, 0, dir);
		/* IV load, convert size */
		LOAD(PTR((uintptr_t)iv), CONTEXT1, 0, ivlen, IMMED);
		/* Insert sequence load/store with VLF */
		SEQFIFOLOAD(MSG1, SIZE(32), WITH(VLF | LAST1 | LAST2));
		SEQFIFOSTORE(MSG, 0, SIZE(32), WITH(VLF));
	}
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                HMAC shared
 * @ingroup                sharedesc_group
 *
 * @param[in,out] descbuf  Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize  Limit/returned descriptor buffer size.
 * @param[in] authdata     Pointer to authentication transform definitions.
 *                         Message digest algorithm: OP_ALG_ALGSEL_MD5/
 *                         SHA1-512.
 * @param[in] icv          HMAC comparison for ICV, NULL if no check desired.
 */
static inline void cnstr_shdsc_hmac(uint32_t *descbuf, unsigned *bufsize,
		      struct alginfo *authdata, uint8_t *icv)
{
	struct program prg;
	struct program *program = &prg;
	uint8_t storelen;
	uint8_t opicv;

	/* Compute fixed-size store based on alg selection */
	switch (authdata->algtype) {
	case OP_ALG_ALGSEL_MD5:
		storelen = 16;
		break;
	case OP_ALG_ALGSEL_SHA1:
		storelen = 20;
		break;
	case OP_ALG_ALGSEL_SHA224:
		storelen = 28;
		break;
	case OP_ALG_ALGSEL_SHA256:
		storelen = 32;
		break;
	case OP_ALG_ALGSEL_SHA384:
		storelen = 48;
		break;
	case OP_ALG_ALGSEL_SHA512:
		storelen = 64;
		break;
	default:
		return;
	}

	opicv = icv ? ICV_CHECK_ENABLE : ICV_CHECK_DISABLE;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_ALWAYS, 1, SC);
	{
		KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
		    storelen, IMMED);
		/* compute sequences */
		MATHB(SEQINSZ, SUB, MATH2, VSEQINSZ, SIZE(4), 0);
		MATHB(SEQINSZ, SUB, MATH2, VSEQOUTSZ, SIZE(4), 0);
		/* Do operation */
		ALG_OPERATION(authdata->algtype, OP_ALG_AAI_HMAC,
			      OP_ALG_AS_INITFINAL, opicv, DIR_ENC);
		/* Do load (variable length) */
		SEQFIFOLOAD(MSG2, SIZE(32), WITH(VLF | LAST1 | LAST2));
		SEQSTORE(CONTEXT2, 0, storelen, 0);
	}
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                 KASUMI F8 (Confidentiality) as a shared descriptor
 *                          (ETSI "Document 1: f8 and f9 specification").
 * @ingroup                 sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Points to size to be updated at completion.
 * @param[in] cipherdata    Pointer to block cipher transform definitions.
 * @param[in] dir           Cipher direction (DIR_ENCRYPT/DIR_DECRYPT).
 * @param[in] count         Count value (32 bits).
 * @param[in] bearer        Bearer ID (5 bits).
 * @param[in] direction     Direction (1 bit).
 */
static inline void cnstr_shdsc_kasumi_f8(uint32_t *descbuf, unsigned *bufsize,
			   struct alginfo *cipherdata, uint8_t dir,
			   uint32_t count, uint8_t bearer, uint8_t direction)
{
	struct program prg;
	struct program *program = &prg;
	uint64_t ct = count;
	uint64_t br = bearer;
	uint64_t dr = direction;
	uint64_t context = (ct << 32) | (br << 27) | (dr << 26);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_ALWAYS, 1, 0);
	{
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, IMMED);
		MATHB(SEQINSZ, SUB, MATH2, VSEQINSZ, SIZE(4), 0);
		MATHB(SEQINSZ, SUB, MATH2, VSEQOUTSZ, SIZE(4), 0);
		ALG_OPERATION(OP_ALG_ALGSEL_KASUMI, OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL, 0, dir);
		LOAD(IMM(context), CONTEXT1, 0, SIZE(8), 0);
		SEQFIFOLOAD(MSG1, SIZE(32), WITH(VLF | LAST1 | LAST2));
		SEQFIFOSTORE(MSG, 0, SIZE(32), WITH(VLF));
	}
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                 KASUMI F9 (Integrity) as a shared descriptor
 *                          (ETSI "Document 1: f8 and f9 specification").
 * @ingroup                 sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Points to size to be updated at completion.
 * @param[in] authdata      Pointer to authentication transform definitions.
 * @param[in] dir           Cipher direction (DIR_ENCRYPT/DIR_DECRYPT).
 * @param[in] count         Count value (32 bits).
 * @param[in] fresh         Fresh value ID (32 bits).
 * @param[in] direction     Direction (1 bit).
 * @param[in] datalen       Size of data.
 */
static inline void cnstr_shdsc_kasumi_f9(uint32_t *descbuf, unsigned *bufsize,
			   struct alginfo *authdata, uint8_t dir,
			   uint32_t count, uint32_t fresh, uint8_t direction,
			   uint32_t datalen)
{
	struct program prg;
	struct program *program = &prg;
	uint16_t ctx_offset = 16;
	uint64_t ct = count;
	uint64_t fr = fresh;
	uint64_t dr = direction;
	uint64_t context[3];

	context[0] = (ct << 32) | (dr << 26);
	context[1] = (fr << 32);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_ALWAYS, 1, 0);
	{
		KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, IMMED);
		MATHB(SEQINSZ, SUB, MATH2, VSEQINSZ, SIZE(4), 0);
		ALG_OPERATION(OP_ALG_ALGSEL_KASUMI, OP_ALG_AAI_F9,
			      OP_ALG_AS_INITFINAL, 0, dir);
		LOAD(PTR((uintptr_t)context), CONTEXT1, 0, SIZE(24), IMMED);
		SEQFIFOLOAD(BIT_DATA, datalen, WITH(CLASS1 | LAST1 | LAST2));
		/* Save output MAC of DWORD 2 into a 32-bit sequence */
		SEQSTORE(CONTEXT1, ctx_offset, SIZE(4), 0);
	}
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                CRC32 Accelerator (IEEE 802 CRC32 protocol mode)
 * @ingroup                sharedesc_group
 *
 * @param[in,out] descbuf  Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize  Limit of descriptor buffer size.
 */
static inline void cnstr_shdsc_crc(uint32_t *descbuf, unsigned *bufsize)
{
	struct program prg;
	struct program *program = &prg;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_ALWAYS, 1, 0);
	{
		MATHB(SEQINSZ, SUB, MATH2, VSEQINSZ, SIZE(4), 0);
		ALG_OPERATION(OP_ALG_ALGSEL_CRC,
			      OP_ALG_AAI_802 | OP_ALG_AAI_DOC,
			      OP_ALG_AS_FINALIZE, 0, DIR_ENC);
		SEQFIFOLOAD(MSG2, SIZE(32), WITH(VLF | LAST2));
		SEQSTORE(CONTEXT2, 0, SIZE(4), 0);
	}
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                 MACsec(802.1AE) encapsulation
 * @ingroup                 sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Points to size to be updated at completion.
 * @param[in] cipherdata    Pointer to block cipher transform definitions.
 * @param[in] sci           PDB Secure Channel Identifier.
 * @param[in] ethertype     PDB EtherType.
 * @param[in] tci_an        TAG Control Information and Association Number
 *                          are treated as a single field of 8 bits in PDB.
 * @param[in] pn            PDB Packet Number.
 **/
static inline void cnstr_shdsc_macsec_encap(uint32_t *descbuf,
					    unsigned *bufsize,
					    struct alginfo *cipherdata,
					    uint64_t sci, uint16_t ethertype,
					    uint8_t tci_an, uint32_t pn)
{
	struct program prg;
	struct program *program = &prg;
	struct macsec_encap_pdb pdb;
	uint32_t startidx;

	LABEL(keyjump);
	REFERENCE(pkeyjump);

	if ((cipherdata->algtype == MACSEC_CIPHER_TYPE_GMAC) &&
	    (rta_sec_era < RTA_SEC_ERA_5))
		pr_debug("MACsec GMAC available only for Era 5 or above\n");

	memset(&pdb, 0x00, sizeof(struct macsec_encap_pdb));
	pdb.sci_hi = high_32b(sci);
	pdb.sci_lo = low_32b(sci);
	pdb.ethertype = ethertype;
	pdb.tci_an = tci_an;
	pdb.pn = pn;

	startidx = sizeof(struct macsec_encap_pdb) >> 2;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_SERIAL, ++startidx, WITH(SC));
	{
		ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct macsec_encap_pdb));
		pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE,
				WITH(SHRD | SELF | BOTH));
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(IMMED));
		SET_LABEL(keyjump);
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_MACSEC,
			 WITH(OP_PCL_MACSEC));
	}
	PATCH_JUMP(pkeyjump, keyjump);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                 MACsec(802.1AE) decapsulation
 * @ingroup                 sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Points to size to be updated at completion.
 * @param[in] cipherdata    Pointer to block cipher transform definitions.
 * @param[in] sci           PDB Secure Channel Identifier.
 * @param[in] pn            PDB Packet Number.
 **/
static inline void cnstr_shdsc_macsec_decap(uint32_t *descbuf,
					    unsigned *bufsize,
					    struct alginfo *cipherdata,
					    uint64_t sci, uint32_t pn)
{
	struct program prg;
	struct program *program = &prg;
	struct macsec_decap_pdb pdb;
	uint32_t startidx;

	LABEL(keyjump);
	REFERENCE(pkeyjump);

	if ((cipherdata->algtype == MACSEC_CIPHER_TYPE_GMAC) &&
	    (rta_sec_era < RTA_SEC_ERA_5))
		pr_debug("MACsec GMAC available only for Era 5 or above\n");

	memset(&pdb, 0x00, sizeof(struct macsec_decap_pdb));
	pdb.sci_hi = high_32b(sci);
	pdb.sci_lo = low_32b(sci);
	pdb.pn = pn;

	startidx = sizeof(struct macsec_decap_pdb) >> 2;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_SERIAL, ++startidx, WITH(SC));
	{
		ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct macsec_decap_pdb));
		pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE,
				WITH(SHRD | SELF | BOTH));
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(IMMED));
		SET_LABEL(keyjump);
		PROTOCOL(OP_TYPE_DECAP_PROTOCOL, OP_PCLID_MACSEC,
			 WITH(OP_PCL_MACSEC));
	}
	PATCH_JUMP(pkeyjump, keyjump);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details  IPSec ESP encapsulation protocol-level shared descriptor.
 *           Requires an MDHA split key.
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *      completion
 * @param [in] ps            If 36/40bit addressing is desired, this parameter
 *      must be non-zero.
 * @param[in] pdb         Pointer to the PDB to be used with this descriptor.
 *      This structure will be copied inline to the descriptor under
 *      construction. No error checking will be made. Refer to the
 *      block guide for a details of the encapsulation PDB.
 * @param[in] cipherdata  Pointer to block cipher transform definitions. Valid
 *      algorithm values: one of OP_PCL_IPSEC_*
 * @param[in] authdata    Pointer to authentication transform definitions. Note
 *      that since a split key is to be used, the size of the split key itself
 *      is specified. Valid algorithm values: one of OP_PCL_IPSEC_*
 **/
static inline void cnstr_shdsc_ipsec_encap(uint32_t *descbuf,
					   unsigned *bufsize,
					   unsigned short ps,
					   struct ipsec_encap_pdb *pdb,
					   struct alginfo *cipherdata,
					   struct alginfo *authdata)
{
	struct program prg;
	struct program *program = &prg;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);
	LABEL(hdr);
	REFERENCE(phdr);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	phdr = SHR_HDR(SHR_SERIAL, hdr, 0);
	ENDIAN_DATA((uint8_t *)pdb,
		    sizeof(struct ipsec_encap_pdb) + pdb->ip_hdr_len);
	SET_LABEL(hdr);
	pkeyjmp = JUMP(IMM(keyjmp), LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
	KEY(MDHA_SPLIT_KEY, authdata->key_enc_flags, PTR(authdata->key),
	    authdata->keylen, IMMED);
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, IMMED);
	SET_LABEL(keyjmp);
	PROTOCOL(OP_TYPE_ENCAP_PROTOCOL,
		 OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | authdata->algtype));
	PATCH_JUMP(pkeyjmp, keyjmp);
	PATCH_HDR(phdr, hdr);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details IPSec ESP decapsulation protocol-level sharedesc
 *          Requires an MDHA split key.
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *      completion
 * @param [in] ps            If 36/40bit addressing is desired, this parameter
 *      must be non-zero.
 * @param[in] pdb         Pointer to the PDB to be used with this descriptor.
 *      This structure will be copied inline to the descriptor under
 *      construction. No error checking will be made. Refer to the
 *      block guide for details about the decapsulation PDB.
 * @param[in] cipherdata  Pointer to block cipher transform definitions. Valid
 *      algorithm values: one of OP_PCL_IPSEC_*
 * @param[in] authdata    Pointer to authentication transform definitions. Note
 *      that since a split key is to be used, the size of the split key itself
 *      is specified. Valid algorithm values: one of OP_PCL_IPSEC_*
 **/
static inline void cnstr_shdsc_ipsec_decap(uint32_t *descbuf,
					   unsigned *bufsize,
					   unsigned short ps,
					   struct ipsec_decap_pdb *pdb,
					   struct alginfo *cipherdata,
					   struct alginfo *authdata)
{
	struct program prg;
	struct program *program = &prg;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);
	LABEL(hdr);
	REFERENCE(phdr);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	phdr = SHR_HDR(SHR_SERIAL, hdr, 0);
	ENDIAN_DATA((uint8_t *)pdb, sizeof(struct ipsec_decap_pdb));
	SET_LABEL(hdr);
	pkeyjmp = JUMP(IMM(keyjmp), LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
	KEY(MDHA_SPLIT_KEY, authdata->key_enc_flags, PTR(authdata->key),
	    authdata->keylen, IMMED);
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, IMMED);
	SET_LABEL(keyjmp);
	PROTOCOL(OP_TYPE_DECAP_PROTOCOL,
		 OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | authdata->algtype));
	PATCH_JUMP(pkeyjmp, keyjmp);
	PATCH_HDR(phdr, hdr);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details  IPSec DES-CBC/3DES-CBC & AES-XCBC-MAC-96 ESP encapsulation
 * shared descriptor. Supported only for platforms with 32-bit address pointers,
 * and SEC ERA 4 or higher. The tunnel/transport mode of the IPsec ESP
 * is supported only if the Outer/Transport IP Header is present in
 * the encapsulation output packet.
 * The descriptor performs DES-CBC/3DES-CBC & HMAC-MD5-96 and then rereads
 * the input packet to do the AES-XCBC-MAC-96 calculation and to overwrite
 * the MD5 ICV.
 * The descriptor uses all the benefits of the built-in protocol by computing
 * the IPsec ESP with a hardware supported algorithms combination
 * (DES-CBC/3DES-CBC & HMAC-MD5-96). The HMAC-MD5 authentication algorithm
 * was chosen in order to speed up the computational time for this intermediate
 * step.
 * Warning: The user must allocate at least 32 bytes for the authentication key
 * (in order to use it also with HMAC-MD5-96),even when using a shorter key
 * for the AES-XCBC-MAC-96.
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *      completion
 * @param[in] pdb            Pointer to the PDB to be used with this descriptor.
 *      This structure will be copied inline to the descriptor under
 *      construction. No error checking will be made. Refer to the
 *      block guide for a details of the encapsulation PDB.
 * @param[in] cipherdata     Pointer to block cipher transform definitions.
 *      Valid algorithm values: OP_PCL_IPSEC_DES, OP_PCL_IPSEC_3DES.
 * @param[in] authdata       Pointer to authentication transform definition.
 *      Valid algorithm value: OP_PCL_IPSEC_AES_XCBC_MAC_96.
 **/
static inline void cnstr_shdsc_ipsec_encap_des_aes_xcbc(uint32_t *descbuf,
		unsigned *bufsize, struct ipsec_encap_pdb *pdb,
		struct alginfo *cipherdata, struct alginfo *authdata)
{
	struct program prg;
	struct program *program = &prg;

	LABEL(hdr);
	LABEL(shd_ptr);
	LABEL(keyjump);
	LABEL(outptr);
	LABEL(swapped_seqin_fields);
	LABEL(swapped_seqin_ptr);
	REFERENCE(phdr);
	REFERENCE(pkeyjump);
	REFERENCE(move_outlen);
	REFERENCE(move_seqout_ptr);
	REFERENCE(swapped_seqin_ptr_jump);
	REFERENCE(write_swapped_seqin_ptr);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	phdr = SHR_HDR(SHR_SERIAL, hdr, 0);
	ENDIAN_DATA((uint8_t *)pdb,
		    sizeof(struct ipsec_encap_pdb) + pdb->ip_hdr_len);
	SET_LABEL(hdr);
	pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE, SHRD | SELF);
	/*
	 * Hard-coded KEY arguments. The descriptor uses all the benefits of
	 * the built-in protocol by computing the IPsec ESP with a hardware
	 * supported algorithms combination (DES-CBC/3DES-CBC & HMAC-MD5-96).
	 * The HMAC-MD5 authentication algorithm was chosen with
	 * the keys options from below in order to speed up the computational
	 * time for this intermediate step.
	 * Warning: The user must allocate at least 32 bytes for
	 * the authentication key (in order to use it also with HMAC-MD5-96),
	 * even when using a shorter key for the AES-XCBC-MAC-96.
	 */
	KEY(MDHA_SPLIT_KEY, 0, PTR(authdata->key), 32, 0);
	SET_LABEL(keyjump);
	LOAD(IMM(LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
		 CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_RESET_CLS1_CHA),
	     CLRW, 0, 4, 0);
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, 0);
	PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | OP_PCL_IPSEC_HMAC_MD5_96));
	/* Swap SEQINPTR to SEQOUTPTR. */
	move_seqout_ptr = MOVE(DESCBUF, 0, MATH1, 0, IMM(16), WAITCOMP);
	MATHB(MATH1, AND, IMM(~(CMD_SEQ_IN_PTR ^ CMD_SEQ_OUT_PTR)), MATH1,
	      SIZE(8), IFB);
/*
 * TODO: RTA currently doesn't support creating a LOAD command
 * with another command as IMM.
 * To be changed when proper support is added in RTA.
 */
	LOAD(IMM(0xa00000e5), MATH3, 4, 4, 0);
	MATHB(MATH3, SHLD, MATH3, MATH3,  SIZE(8), 0);
	write_swapped_seqin_ptr = MOVE(MATH1, 0, DESCBUF, 0, IMM(20), WAITCOMP);
	swapped_seqin_ptr_jump = JUMP(IMM(swapped_seqin_ptr), LOCAL_JUMP,
				      ALL_TRUE, 0);
	LOAD(IMM(LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
		 CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_RESET_CLS1_CHA),
	     CLRW, 0, 4, 0);
	SEQOUTPTR(0, 65535, RTO);
	move_outlen = MOVE(DESCBUF, 0, MATH0, 4, IMM(8), WAITCOMP);
	MATHB(MATH0, SUB,
	      IMM((uint64_t)(pdb->ip_hdr_len + IPSEC_ICV_MD5_TRUNC_SIZE)),
	      VSEQINSZ, SIZE(4), 0);
	MATHB(MATH0, SUB, IMM(IPSEC_ICV_MD5_TRUNC_SIZE), VSEQOUTSZ, SIZE(4), 0);
	KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key), authdata->keylen,
	    0);
	ALG_OPERATION(OP_ALG_ALGSEL_AES, OP_ALG_AAI_XCBC_MAC,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE, OP_ALG_ENCRYPT);
	SEQFIFOLOAD(SKIP, pdb->ip_hdr_len, 0);
	SEQFIFOLOAD(MSG1, 0, VLF | FLUSH1 | LAST1);
	SEQFIFOSTORE(SKIP, 0, 0, VLF);
	SEQSTORE(CONTEXT1, 0, IPSEC_ICV_MD5_TRUNC_SIZE, 0);
/*
 * TODO: RTA currently doesn't support adding labels in or after Job Descriptor.
 * To be changed when proper support is added in RTA.
 */
	/* Label the Shared Descriptor Pointer */
	SET_LABEL(shd_ptr);
	shd_ptr += 1;
	/* Label the Output Pointer */
	SET_LABEL(outptr);
	outptr += 3;
	/* Label the first word after JD */
	SET_LABEL(swapped_seqin_fields);
	swapped_seqin_fields += 8;
	/* Label the second word after JD */
	SET_LABEL(swapped_seqin_ptr);
	swapped_seqin_ptr += 9;

	PATCH_HDR(phdr, hdr);
	PATCH_JUMP(pkeyjump, keyjump);
	PATCH_JUMP(swapped_seqin_ptr_jump, swapped_seqin_ptr);
	PATCH_MOVE(move_outlen, outptr);
	PATCH_MOVE(move_seqout_ptr, shd_ptr);
	PATCH_MOVE(write_swapped_seqin_ptr, swapped_seqin_fields);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details  IPSec DES-CBC/3DES-CBC & AES-XCBC-MAC-96 ESP decapsulation
 * shared descriptor. Supported only for platforms with 32-bit address pointers,
 * and SEC ERA 4 or higher. The tunnel/transport mode of the IPsec ESP
 * is supported only if the Outer/Transport IP Header is present in
 * the decapsulation input packet.
 * The descriptor computes the AES-XCBC-MAC-96 to check if the received ICV
 * is correct, rereads the input packet to compute the MD5 ICV, overwrites
 * the XCBC ICV, and then sends the modified input packet to the
 * DES-CBC/3DES-CBC & HMAC-MD5-96 IPsec.
 * The descriptor uses all the benefits of the built-in protocol by computing
 * the IPsec ESP with a hardware supported algorithms combination
 * (DES-CBC/3DES-CBC & HMAC-MD5-96). The HMAC-MD5 authentication algorithm
 * was chosen in order to speed up the computational time for this intermediate
 * step.
 * Warning: The user must allocate at least 32 bytes for the authentication key
 * (in order to use it also with HMAC-MD5-96),even when using a shorter key
 * for the AES-XCBC-MAC-96.
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *      completion
 * @param[in] pdb            Pointer to the PDB to be used with this descriptor.
 *      This structure will be copied inline to the descriptor under
 *      construction. No error checking will be made. Refer to the
 *      block guide for a details of the encapsulation PDB.
 * @param[in] cipherdata     Pointer to block cipher transform definitions.
 *      Valid algorithm values: OP_PCL_IPSEC_DES, OP_PCL_IPSEC_3DES.
 * @param[in] authdata       Pointer to authentication transform definition.
 *      Valid algorithm value: OP_PCL_IPSEC_AES_XCBC_MAC_96.
 **/
static inline void cnstr_shdsc_ipsec_decap_des_aes_xcbc(uint32_t *descbuf,
		unsigned *bufsize, struct ipsec_decap_pdb *pdb,
		struct alginfo *cipherdata, struct alginfo *authdata)
{
	struct program prg;
	struct program *program = &prg;

	LABEL(hdr);
	LABEL(jump_cmd);
	LABEL(keyjump);
	LABEL(outlen);
	LABEL(seqin_ptr);
	LABEL(seqout_ptr);
	LABEL(swapped_seqout_fields);
	LABEL(swapped_seqout_ptr);
	REFERENCE(seqout_ptr_jump);
	REFERENCE(phdr);
	REFERENCE(pkeyjump);
	REFERENCE(move_jump);
	REFERENCE(move_jump_back);
	REFERENCE(move_seqin_ptr);
	REFERENCE(swapped_seqout_ptr_jump);
	REFERENCE(write_swapped_seqout_ptr);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	phdr = SHR_HDR(SHR_SERIAL, hdr, 0);
	ENDIAN_DATA((uint8_t *)pdb, sizeof(struct ipsec_decap_pdb));
	SET_LABEL(hdr);
	pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE, SHRD | SELF);
	/*
	 * Hard-coded KEY arguments. The descriptor uses all the benefits of
	 * the built-in protocol by computing the IPsec ESP with a hardware
	 * supported algorithms combination (DES-CBC/3DES-CBC & HMAC-MD5-96).
	 * The HMAC-MD5 authentication algorithm was chosen with
	 * the keys options from bellow in order to speed up the computational
	 * time for this intermediate step.
	 * Warning: The user must allocate at least 32 bytes for
	 * the authentication key (in order to use it also with HMAC-MD5-96),
	 * even when using a shorter key for the AES-XCBC-MAC-96.
	 */
	KEY(MDHA_SPLIT_KEY, 0, PTR(authdata->key), 32, 0);
	SET_LABEL(keyjump);
	LOAD(IMM(LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
		 CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_RESET_CLS1_CHA),
	     CLRW, 0, 4, 0);
	KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key), authdata->keylen,
	    0);
	MATHB(SEQINSZ, SUB,
	      IMM((uint64_t)(pdb->ip_hdr_len + IPSEC_ICV_MD5_TRUNC_SIZE)),
	      MATH0, SIZE(4), 0);
	MATHB(MATH0, SUB, ZERO, VSEQINSZ, SIZE(4), 0);
	ALG_OPERATION(OP_ALG_ALGSEL_MD5, OP_ALG_AAI_HMAC_PRECOMP,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE, OP_ALG_ENCRYPT);
	ALG_OPERATION(OP_ALG_ALGSEL_AES, OP_ALG_AAI_XCBC_MAC,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_ENABLE, OP_ALG_DECRYPT);
	SEQFIFOLOAD(SKIP, pdb->ip_hdr_len, 0);
	SEQFIFOLOAD(MSG1, 0, VLF | FLUSH1);
	SEQFIFOLOAD(ICV1, IPSEC_ICV_MD5_TRUNC_SIZE, FLUSH1 | LAST1);
	/* Swap SEQOUTPTR to SEQINPTR. */
	move_seqin_ptr = MOVE(DESCBUF, 0, MATH1, 0, IMM(16), WAITCOMP);
	MATHB(MATH1, OR, IMM(CMD_SEQ_IN_PTR ^ CMD_SEQ_OUT_PTR), MATH1, SIZE(8),
	      IFB);
/*
 * TODO: RTA currently doesn't support creating a LOAD command
 * with another command as IMM.
 * To be changed when proper support is added in RTA.
 */
	LOAD(IMM(0xA00000e1), MATH3, 4, 4, 0);
	MATHB(MATH3, SHLD, MATH3, MATH3,  SIZE(8), 0);
	write_swapped_seqout_ptr = MOVE(MATH1, 0, DESCBUF, 0, IMM(20),
					WAITCOMP);
	swapped_seqout_ptr_jump = JUMP(IMM(swapped_seqout_ptr), LOCAL_JUMP,
				       ALL_TRUE, 0);
/*
 * TODO: To be changed when proper support is added in RTA (can't load
 * a command that is also written by RTA).
 * Change when proper RTA support is added.
 */
	SET_LABEL(jump_cmd);
	WORD(0xA00000f3);
	SEQINPTR(0, 65535, RTO);
	MATHB(MATH0, SUB, ZERO, VSEQINSZ, SIZE(4), 0);
	MATHB(MATH0, ADD, IMM(pdb->ip_hdr_len), VSEQOUTSZ, SIZE(4), 0);
	move_jump = MOVE(DESCBUF, 0, OFIFO, 0, IMM(8), WAITCOMP);
	move_jump_back = MOVE(OFIFO, 0, DESCBUF, 0, IMM(8), 0);
	SEQFIFOLOAD(SKIP, pdb->ip_hdr_len, 0);
	SEQFIFOLOAD(MSG2, 0, VLF | LAST2);
	SEQFIFOSTORE(SKIP, 0, 0, VLF);
	SEQSTORE(CONTEXT2, 0, IPSEC_ICV_MD5_TRUNC_SIZE, 0);
	seqout_ptr_jump = JUMP(IMM(seqout_ptr), LOCAL_JUMP, ALL_TRUE, CALM);

	LOAD(IMM(LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
		 CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_CLR_C2MODE |
		 CLRW_CLR_C2DATAS | CLRW_CLR_C2CTX | CLRW_RESET_CLS1_CHA),
	     CLRW, 0, 4, 0);
	SEQINPTR(0, 65535, RTO);
	MATHB(MATH0, ADD,
	      IMM((uint64_t)(pdb->ip_hdr_len + IPSEC_ICV_MD5_TRUNC_SIZE)),
	      SEQINSZ, SIZE(4), 0);
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, 0);
	PROTOCOL(OP_TYPE_DECAP_PROTOCOL, OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | OP_PCL_IPSEC_HMAC_MD5_96));
/*
 * TODO: RTA currently doesn't support adding labels in or after Job Descriptor.
 * To be changed when proper support is added in RTA.
 */
	/* Label the SEQ OUT PTR */
	SET_LABEL(seqout_ptr);
	seqout_ptr += 2;
	/* Label the Output Length */
	SET_LABEL(outlen);
	outlen += 4;
	/* Label the SEQ IN PTR */
	SET_LABEL(seqin_ptr);
	seqin_ptr += 5;
	/* Label the first word after JD */
	SET_LABEL(swapped_seqout_fields);
	swapped_seqout_fields += 8;
	/* Label the second word after JD */
	SET_LABEL(swapped_seqout_ptr);
	swapped_seqout_ptr += 9;

	PATCH_HDR(phdr, hdr);
	PATCH_JUMP(pkeyjump, keyjump);
	PATCH_JUMP(seqout_ptr_jump, seqout_ptr);
	PATCH_JUMP(swapped_seqout_ptr_jump, swapped_seqout_ptr);
	PATCH_MOVE(move_jump, jump_cmd);
	PATCH_MOVE(move_jump_back, seqin_ptr);
	PATCH_MOVE(move_seqin_ptr, outlen);
	PATCH_MOVE(write_swapped_seqout_ptr, swapped_seqout_fields);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details  IPSec new mode ESP encapsulation protocol-level shared descriptor.
 *           Requires a MDHA split key.
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *      completion
 * @param[in] pdb         Pointer to the PDB to be used with this descriptor.
 *      This structure will be copied inline to the descriptor under
 *      construction. No error checking will be made. Refer to the
 *      block guide for details about the encapsulation PDB.
 * @param[in] cipherdata  Pointer to block cipher transform definitions. Valid
 *      algorithm values: one of OP_PCL_IPSEC_*
 * @param[in] authdata    Pointer to authentication transform definitions. Note
 *      that since a split key is to be used, the size of the split key itself
 *      is specified. Valid algorithm values: one of OP_PCL_IPSEC_*
 */
static inline void cnstr_shdsc_ipsec_new_encap(uint32_t *descbuf,
					       unsigned *bufsize,
					       struct ipsec_encap_pdb *pdb,
					       struct alginfo *cipherdata,
					       struct alginfo *authdata)
{
	if (rta_sec_era < RTA_SEC_ERA_8) {
		pr_debug("IPsec new mode encap: available only for Era %d or above\n",
			 USER_SEC_ERA(RTA_SEC_ERA_8));
		*bufsize = 0;
		return;
	}

	/* Dummy behaviour - until descriptor is ready */
	*bufsize = 0;
}

/**
 * @details IPSec new mode ESP decapsulation protocol-level shared descriptor.
 *          Requires a MDHA split key.
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *      completion
 * @param[in] pdb         Pointer to the PDB to be used with this descriptor.
 *      This structure will be copied inline to the descriptor under
 *      construction. No error checking will be made. Refer to the
 *      block guide for details about the decapsulation PDB.
 * @param[in] cipherdata  Pointer to block cipher transform definitions. Valid
 *      algorithm values: one of OP_PCL_IPSEC_*
 * @param[in] authdata    Pointer to authentication transform definitions. Note
 *      that since a split key is to be used, the size of the split key itself
 *      is specified. Valid algorithm values: one of OP_PCL_IPSEC_*
 */
static inline void cnstr_shdsc_ipsec_new_decap(uint32_t *descbuf,
					       unsigned *bufsize,
					       struct ipsec_decap_pdb *pdb,
					       struct alginfo *cipherdata,
					       struct alginfo *authdata)
{
	if (rta_sec_era < RTA_SEC_ERA_8) {
		pr_debug("IPsec new mode decap: available only for Era %d or above\n",
			 USER_SEC_ERA(RTA_SEC_ERA_8));
		*bufsize = 0;
		return;
	}

	/* Dummy behaviour - until descriptor is ready */
	*bufsize = 0;
}

/**
 * @details                 WiMAX(802.16) encapsulation
 * @ingroup                 sharedesc_group
 * @warning                 Descriptor valid on platforms
 *                          with support for SEC ERA 4.
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Points to size to be updated at completion.
 * @param[in] pdb_opts      PDB Options Byte.
 * @param[in] pn            PDB Packet Number.
 * @param[in] cipherdata    Pointer to block cipher transform definitions.
 * @param[in] protinfo      Protocol information: OP_PCL_WIMAX_OFDM/OFDMA.
 */
static inline void cnstr_shdsc_wimax_encap(uint32_t *descbuf, unsigned *bufsize,
					   uint8_t pdb_opts, uint32_t pn,
					   uint16_t protinfo,
					   struct alginfo *cipherdata)
{
	struct wimax_encap_pdb pdb;
	struct program prg;
	struct program *program = &prg;

	LABEL(hdr);
	LABEL(out_len);
	LABEL(keyjump);
	LABEL(local_offset);
	LABEL(seqout_ptr);
	LABEL(swapped_seqout_ptr);
	REFERENCE(phdr);
	REFERENCE(move_seqin_ptr);
	REFERENCE(move_seqout_ptr);
	REFERENCE(pkeyjump);
	REFERENCE(seqout_ptr_jump1);
	REFERENCE(seqout_ptr_jump2);
	REFERENCE(write_seqout_ptr);
	REFERENCE(write_swapped_seqout_ptr);

	memset(&pdb, 0x00, sizeof(struct wimax_encap_pdb));
	pdb.options = pdb_opts;
	pdb.pn = pn;
	pdb.b0_flags = WIMAX_PDB_B0;
	pdb.ctr_flags = WIMAX_PDB_CTR;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	phdr = SHR_HDR(SHR_SERIAL, hdr, WITH(0));
	{
		ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct wimax_encap_pdb));
		SET_LABEL(hdr);
		/* Save SEQOUTPTR, Output Pointer and Output Length. */
		move_seqout_ptr = MOVE(DESCBUF, 0, OFIFO, 0, IMM(16),
				       WITH(WAITCOMP));
/*
 * TODO: RTA currently doesn't support creating a LOAD command
 * with another command as IMM.
 * To be changed when proper support is added in RTA.
 */
		LOAD(IMM(0xa00000fa), OFIFO, 0, 4, WITH(0));

		/* Swap SEQOUTPTR to the SEQINPTR. */
		move_seqin_ptr = MOVE(DESCBUF, 0, MATH0, 0, IMM(20),
				      WITH(WAITCOMP));
		MATHB(MATH0, OR, IMM(CMD_SEQ_IN_PTR ^ CMD_SEQ_OUT_PTR), MATH0,
		      SIZE(8), IFB);
/*
 * TODO: RTA currently doesn't support creating a LOAD command
 * with another command as IMM.
 * To be changed when proper support is added in RTA.
 */
		LOAD(IMM(0xa00000dd), MATH2, 4, 4, WITH(0));
		write_swapped_seqout_ptr = MOVE(MATH0, 0, DESCBUF, 0, IMM(24),
						WITH(WAITCOMP));
		seqout_ptr_jump1 = JUMP(IMM(swapped_seqout_ptr), LOCAL_JUMP,
				       ALL_TRUE, WITH(0));

		write_seqout_ptr = MOVE(OFIFO, 0, DESCBUF, 0, IMM(20),
					WITH(WAITCOMP));

		SEQLOAD(MATH0, 0, 8, WITH(0));
		LOAD(IMM(LDST_SRCDST_WORD_CLRW |
			 CLRW_CLR_C1MODE |
			 CLRW_CLR_C2MODE |
			 CLRW_CLR_C2DATAS |
			 CLRW_CLR_C2CTX |
			 CLRW_CLR_C2KEY |
			 CLRW_RESET_CLS2_CHA |
			 CLRW_RESET_CLS1_CHA),
		     CLRW, 0, 4, WITH(0));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
		/* Set Encryption Control bit */
		MATHB(MATH0, OR, IMM(WIMAX_GMH_EC_MASK), MATH0, SIZE(8), 0);

		/*
		 * Update Generic Mac Header Length field.
		 * The left shift is used in order to update the GMH LEN field
		 * and nothing else.
		 */
		if (pdb_opts & WIMAX_PDBOPTS_FCS)
			MATHB(MATH0, ADD, IMM((WIMAX_PN_LEN << 0x28) +
					      (WIMAX_ICV_LEN << 0x28) +
					      (WIMAX_FCS_LEN << 0x28)),
			      MATH0, SIZE(8), 0);
		else
			MATHB(MATH0, ADD, IMM((WIMAX_PN_LEN << 0x28) +
					      (WIMAX_ICV_LEN << 0x28)),
			      MATH0, SIZE(8), 0);

		/*
		 * Compute the CRC-8-ATM value for the first five bytes
		 * of the header and insert the result into the sixth
		 * MATH0 byte field.
		 */
		KEY(KEY2, 0, IMM(CRC_8_ATM_POLY), 2, WITH(IMMED));
		ALG_OPERATION(OP_ALG_ALGSEL_CRC,
			      OP_ALG_AAI_CUST_POLY | OP_ALG_AAI_DIS,
			      OP_ALG_AS_UPDATE, ICV_CHECK_DISABLE,
			      OP_ALG_ENCRYPT);
		MOVE(MATH0, 0, IFIFOAB2, 0, IMM(5), WITH(LAST1));
		MOVE(CONTEXT2, 0, MATH1, 0, IMM(4), WITH(WAITCOMP));
		MOVE(MATH1, 0, MATH0, 5, IMM(1), WITH(WAITCOMP));
		SEQSTORE(MATH0, 0, 8, WITH(0));

		SEQINPTR(0, 8, WITH(RTO));
		LOAD(IMM(LDST_SRCDST_WORD_CLRW |
			 CLRW_CLR_C1MODE |
			 CLRW_CLR_C2MODE |
			 CLRW_CLR_C2DATAS |
			 CLRW_CLR_C2CTX |
			 CLRW_CLR_C2KEY |
			 CLRW_RESET_CLS2_CHA |
			 CLRW_RESET_CLS1_CHA),
		     CLRW, 0, 4, WITH(0));
		pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE,
				WITH(SHRD | SELF));
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(IMMED));
		SET_LABEL(keyjump);
		seqout_ptr_jump2 = JUMP(IMM(swapped_seqout_ptr), LOCAL_JUMP,
					ALL_TRUE, WITH(0));
		PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_WIMAX, protinfo);
/*
 * TODO: RTA currently doesn't support adding labels in or after Job Descriptor.
 * To be changed when proper support is added in RTA.
 */
		SET_LABEL(local_offset);
		local_offset += 1;

		SET_LABEL(swapped_seqout_ptr);
		swapped_seqout_ptr += 2;

		SET_LABEL(seqout_ptr);
		seqout_ptr += 3;
		SET_LABEL(out_len);
		out_len += 6;

	}
	PATCH_HDR(phdr, hdr);
	PATCH_JUMP(pkeyjump, keyjump);
	PATCH_JUMP(seqout_ptr_jump1, swapped_seqout_ptr);
	PATCH_JUMP(seqout_ptr_jump2, local_offset);
	PATCH_MOVE(move_seqin_ptr, out_len);
	PATCH_MOVE(move_seqout_ptr, seqout_ptr);
	PATCH_MOVE(write_seqout_ptr, local_offset);
	PATCH_MOVE(write_swapped_seqout_ptr, local_offset);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                 WiMAX(802.16) decapsulation
 * @ingroup                 sharedesc_group
 * @warning                 Descriptor valid on platforms
 *                          with support for SEC ERA 4.
 *
 * @param[in,out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in,out] bufsize   Points to size to be updated at completion.
 * @param[in] pdb_opts      PDB Options Byte.
 * @param[in] pn            PDB Packet Number.
 * @param[in] cipherdata    Pointer to block cipher transform definitions.
 * @param[in] protinfo      Protocol information: OP_PCL_WIMAX_OFDM/OFDMA.
 */
static inline void cnstr_shdsc_wimax_decap(uint32_t *descbuf, unsigned *bufsize,
					   uint8_t pdb_opts, uint32_t pn,
					   uint16_t ar_len, uint16_t protinfo,
					   struct alginfo *cipherdata)
{
	struct wimax_decap_pdb pdb;
	struct program prg;
	struct program *program = &prg;

	LABEL(gmh);
	LABEL(hdr);
	LABEL(keyjump);
	REFERENCE(load_gmh);
	REFERENCE(move_gmh);
	REFERENCE(phdr);
	REFERENCE(pkeyjump);

	memset(&pdb, 0x00, sizeof(struct wimax_decap_pdb));
	pdb.options = pdb_opts;
	pdb.pn = pn;
	pdb.antireplay_len = ar_len;
	pdb.iv_flags = WIMAX_PDB_B0;
	pdb.ctr_flags = WIMAX_PDB_CTR;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	phdr = SHR_HDR(SHR_SERIAL, hdr, WITH(0));
	{
		ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct wimax_decap_pdb));
		SET_LABEL(hdr);
		load_gmh = SEQLOAD(DESCBUF, 0, 8, WITH(0));
		LOAD(IMM(LDST_SRCDST_WORD_CLRW |
			 CLRW_CLR_C1MODE |
			 CLRW_CLR_C2MODE |
			 CLRW_CLR_C2DATAS |
			 CLRW_CLR_C2CTX |
			 CLRW_CLR_C2KEY |
			 CLRW_RESET_CLS2_CHA |
			 CLRW_RESET_CLS1_CHA),
		     CLRW, 0, 4, WITH(0));
		SEQINPTR(0, 8, WITH(RTO));

		pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE,
				WITH(SHRD | SELF));
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(IMMED));
		SET_LABEL(keyjump);
		PROTOCOL(OP_TYPE_DECAP_PROTOCOL, OP_PCLID_WIMAX, protinfo);

		SEQOUTPTR(0, 8, WITH(RTO));
		move_gmh = MOVE(DESCBUF, 0, MATH0, 0, IMM(8), WITH(WAITCOMP));

		/* Clear Encryption Control bit. */
		MATHB(MATH0, AND, IMM(~WIMAX_GMH_EC_MASK), MATH0, SIZE(8), 0);

		/*
		 * Update Generic Mac Header Length field.
		 * The left shift is used in order to update the GMH LEN field
		 * and nothing else.
		 */
		if (pdb_opts & WIMAX_PDBOPTS_FCS)
			MATHB(MATH0, SUB, IMM((WIMAX_PN_LEN << 0x28) +
					      (WIMAX_ICV_LEN << 0x28) +
					      (WIMAX_FCS_LEN << 0x28)),
			      MATH0, SIZE(8), 0);
		else
			MATHB(MATH0, SUB, IMM((WIMAX_PN_LEN << 0x28) +
					      (WIMAX_ICV_LEN << 0x28)),
			      MATH0, SIZE(8), 0);

		/*
		 * Compute the CRC-8-ATM value for the first five bytes
		 * of the header and insert the result into the sixth
		 * MATH0 byte field.
		 */
		LOAD(IMM(LDST_SRCDST_WORD_CLRW |
			 CLRW_CLR_C1MODE |
			 CLRW_CLR_C2MODE |
			 CLRW_CLR_C2DATAS |
			 CLRW_CLR_C2CTX |
			 CLRW_CLR_C2KEY |
			 CLRW_RESET_CLS2_CHA |
			 CLRW_RESET_CLS1_CHA),
		     CLRW, 0, 4, WITH(0));
		KEY(KEY2, 0, IMM(CRC_8_ATM_POLY), 2, WITH(IMMED));
		ALG_OPERATION(OP_ALG_ALGSEL_CRC,
			      OP_ALG_AAI_CUST_POLY | OP_ALG_AAI_DIS,
			      OP_ALG_AS_UPDATE, ICV_CHECK_DISABLE,
			      OP_ALG_ENCRYPT);
		MOVE(MATH0, 0, IFIFOAB2, 0, IMM(5), WITH(LAST1));
		MOVE(CONTEXT2, 0, MATH1, 0, IMM(4), WITH(WAITCOMP));
		MOVE(MATH1, 0, MATH0, 5, IMM(1), WITH(WAITCOMP));

		/* Rewrite decapsulation Generic Mac Header. */
		SEQSTORE(MATH0, 0, 6, WITH(0));
/*
 * TODO: RTA currently doesn't support adding labels in or after Job Descriptor.
 * To be changed when proper support is added in RTA.
 */
		SET_LABEL(gmh);
		gmh += 11;
	}
	PATCH_HDR(phdr, hdr);
	PATCH_JUMP(pkeyjump, keyjump);
	PATCH_LOAD(load_gmh, gmh);
	PATCH_MOVE(move_gmh, gmh);
	*bufsize = PROGRAM_FINALIZE();
}

/*
 * Function for appending the portion of a PDCP Control Plane shared descriptor
 * which performs NULL encryption and integrity (i.e. copies the input frame
 * to the output frame, appending 32 bits of zeros at the end (MAC-I for
 * NULL integrity).
 */
static inline int pdcp_insert_cplane_null_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	LABEL(local_offset);
	REFERENCE(move_cmd_read_descbuf);
	REFERENCE(move_cmd_write_descbuf);

	if (rta_sec_era > RTA_SEC_ERA_2) {
		MATHB(SEQINSZ, ADD, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		MATHB(SEQINSZ, dir == OP_TYPE_ENCAP_PROTOCOL ? ADD : SUB,
		      IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4), WITH(0));
	} else {
		MATHB(SEQINSZ, ADD, ONE, VSEQINSZ, SIZE(4), WITH(0));
		MATHB(VSEQINSZ, SUB, ONE, VSEQINSZ, SIZE(4), WITH(0));

		if (dir == OP_TYPE_ENCAP_PROTOCOL) {
			MATHB(SEQINSZ, ADD, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ,
			      SIZE(4), WITH(0));
			MATHB(VSEQINSZ, SUB, ONE, MATH0, SIZE(4), WITH(0));
		} else {
			MATHB(VSEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), VSEQINSZ,
			      SIZE(4), WITH(0));
			MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ,
			      SIZE(4), WITH(0));
			MATHB(VSEQOUTSZ, SUB, ONE, MATH0, SIZE(4), WITH(0));
		}

		MATHB(MATH0, ADD, ONE, MATH0, SIZE(4), WITH(0));

		/*
		 * Since MOVELEN is available only starting with
		 * SEC ERA 3, use poor man's MOVELEN: create a MOVE
		 * command dynamically by writing the length from M1 by
		 * OR-ing the command in the M1 register and MOVE the
		 * result into the descriptor buffer. Care must be taken
		 * wrt. the location of the command because of SEC
		 * pipelining. The actual MOVEs are written at the end
		 * of the descriptor due to calculations needed on the
		 * offset in the descriptor for the MOVE command.
		 */
		move_cmd_read_descbuf = MOVE(DESCBUF, 0, MATH0, 0, IMM(6),
					     WITH(0));
		move_cmd_write_descbuf = MOVE(MATH0, 0, DESCBUF, 0, IMM(8),
					      WITH(WAITCOMP));
	}
	MATHB(VSEQINSZ, SUB, IMM(PDCP_NULL_MAX_FRAME_LEN), NONE, SIZE(4),
	      WITH(0));
	JUMP(IMM(PDCP_MAX_FRAME_LEN_STATUS), HALT_STATUS, ALL_FALSE, MATH_N);

	if (rta_sec_era > RTA_SEC_ERA_2) {
		if (dir == OP_TYPE_ENCAP_PROTOCOL)
			MATHB(VSEQINSZ, ADD, ZERO, MATH0, SIZE(4), WITH(0));
		else
			MATHB(VSEQOUTSZ, ADD, ZERO, MATH0, SIZE(4), WITH(0));
	}
	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
	SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

	if (rta_sec_era > RTA_SEC_ERA_2) {
		MOVE(AB1, 0, OFIFO, 0, MATH0, WITH(0));
	} else {
		SET_LABEL(local_offset);

		/* Shut off automatic Info FIFO entries */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, WITH(0));
		/* Placeholder for MOVE command with length from M1 register */
		MOVE(IFIFOAB1, 0, OFIFO, 0, IMM(0), WITH(0));
		/* Enable automatic Info FIFO entries */
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, WITH(0));
	}

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		MATHB(MATH1, XOR, MATH1, MATH0, SIZE(8), WITH(0));
		MOVE(MATH0, 0, OFIFO, 0, IMM(4), WITH(0));
	}

	if (rta_sec_era < RTA_SEC_ERA_3) {
		PATCH_MOVE(move_cmd_read_descbuf, local_offset);
		PATCH_MOVE(move_cmd_write_descbuf, local_offset);
	}

	return 0;
}

static inline int pdcp_insert_uplane_null_op(struct program *program,
		struct alginfo *cipherdata,
		unsigned dir)
{
	LABEL(local_offset);
	REFERENCE(move_cmd_read_descbuf);
	REFERENCE(move_cmd_write_descbuf);

	if (rta_sec_era > RTA_SEC_ERA_2) {
		MATHB(SEQINSZ, ADD, ZERO, VSEQINSZ,  SIZE(4), WITH(0));
		MATHB(SEQINSZ, ADD, ZERO, VSEQOUTSZ,  SIZE(4), WITH(0));
	} else {
		MATHB(SEQINSZ, ADD, ONE, VSEQINSZ,  SIZE(4), WITH(0));
		MATHB(VSEQINSZ, SUB, ONE, VSEQINSZ,  SIZE(4), 0);
		MATHB(SEQINSZ, ADD, ONE, VSEQOUTSZ,  SIZE(4), WITH(0));
		MATHB(VSEQOUTSZ, SUB, ONE, VSEQOUTSZ,  SIZE(4), WITH(0));
		MATHB(VSEQINSZ, SUB, ONE, MATH0,  SIZE(4), WITH(0));
		MATHB(MATH0, ADD, ONE, MATH0,  SIZE(4), WITH(0));

		/*
		 * Since MOVELEN is available only starting with
		 * SEC ERA 3, use poor man's MOVELEN: create a MOVE
		 * command dynamically by writing the length from M1 by
		 * OR-ing the command in the M1 register and MOVE the
		 * result into the descriptor buffer. Care must be taken
		 * wrt. the location of the command because of SEC
		 * pipelining. The actual MOVEs are written at the end
		 * of the descriptor due to calculations needed on the
		 * offset in the descriptor for the MOVE command.
		 */
		move_cmd_read_descbuf = MOVE(DESCBUF, 0, MATH0, 0, IMM(6),
					     WITH(0));
		move_cmd_write_descbuf = MOVE(MATH0, 0, DESCBUF, 0, IMM(8),
					      WITH(WAITCOMP));
	}
	MATHB(SEQINSZ, SUB, IMM(PDCP_NULL_MAX_FRAME_LEN), NONE,  SIZE(4),
	      WITH(IFB));
	JUMP(IMM(PDCP_MAX_FRAME_LEN_STATUS), HALT_STATUS, ALL_FALSE, MATH_N);

	if (rta_sec_era > RTA_SEC_ERA_2)
		MATHB(VSEQINSZ, ADD, ZERO, MATH0,  SIZE(4), WITH(0));

	SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));
	if (rta_sec_era > RTA_SEC_ERA_2) {
		MOVE(AB1, 0, OFIFO, 0, MATH0, WITH(0));
	} else {
		SET_LABEL(local_offset);

		/* Shut off automatic Info FIFO entries */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, WITH(0));

		/* Placeholder for MOVE command with length from M0 register */
		MOVE(IFIFOAB1, 0, OFIFO, 0, IMM(0), WITH(0));

		/* Enable automatic Info FIFO entries */
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, WITH(0));
	}

	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

	if (rta_sec_era < RTA_SEC_ERA_3) {
		PATCH_MOVE(move_cmd_read_descbuf, local_offset);
		PATCH_MOVE(move_cmd_write_descbuf, local_offset);
	}
	return 0;
}

static inline int pdcp_insert_cplane_int_only_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	LABEL(local_offset);
	REFERENCE(move_cmd_read_descbuf);
	REFERENCE(move_cmd_write_descbuf);

	switch (authdata->algtype) {
	case PDCP_AUTH_TYPE_SNOW:
		/* Insert Auth Key */
		KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, 0);
		SEQLOAD(MATH0, 7, 1, WITH(0));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));

		if (rta_sec_era > RTA_SEC_ERA_2 ||
		    (rta_sec_era == RTA_SEC_ERA_2 &&
				   era_2_sw_hfn_override == 0)) {
			SEQINPTR(0, 1, WITH(RTO));
		} else {
			SEQINPTR(0, 5, WITH(RTO));
			SEQFIFOLOAD(SKIP, 4, WITH(0));
		}

		MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1,  SIZE(8),
		      WITH(IFB));
		MATHB(MATH1, SHLD, MATH1, MATH1,  SIZE(8), WITH(0));
		MOVE(DESCBUF, 8, MATH2, 0, IMM(8), WITH(WAITCOMP));
		MATHB(MATH2, AND, IMM(PDCP_BEARER_MASK), MATH2, SIZE(8),
		      WITH(0));
		MOVE(DESCBUF, 0x0C, MATH3, 0, IMM(4), WITH(WAITCOMP));
		MATHB(MATH3, AND, IMM(PDCP_DIR_MASK), MATH3, SIZE(8),
		      WITH(0));
		MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));
		MOVE(MATH2, 0, CONTEXT2, 0, IMM(0x0C), WITH(WAITCOMP));

		if (dir == OP_TYPE_DECAP_PROTOCOL) {
			MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), MATH1, SIZE(4),
			      WITH(0));
		} else {
			if (rta_sec_era > RTA_SEC_ERA_2) {
				MATHB(SEQINSZ, SUB, ZERO, MATH1, SIZE(4),
				      WITH(0));
			} else {
				MATHB(SEQINSZ, ADD, ONE, MATH1, SIZE(4),
				      WITH(0));
				MATHB(MATH1, SUB, ONE, MATH1, SIZE(4),
				      WITH(0));
			}
		}

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MATHB(MATH1, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
			MATHB(MATH1, SUB, ZERO, VSEQOUTSZ, SIZE(4), WITH(0));
		} else {
			MATHB(ZERO, ADD, MATH1, VSEQINSZ, SIZE(4), WITH(0));
			MATHB(ZERO, ADD, MATH1, VSEQOUTSZ, SIZE(4), WITH(0));

			/*
			 * Since MOVELEN is available only starting with
			 * SEC ERA 3, use poor man's MOVELEN: create a MOVE
			 * command dynamically by writing the length from M1 by
			 * OR-ing the command in the M1 register and MOVE the
			 * result into the descriptor buffer. Care must be taken
			 * wrt. the location of the command because of SEC
			 * pipelining. The actual MOVEs are written at the end
			 * of the descriptor due to calculations needed on the
			 * offset in the descriptor for the MOVE command.
			 */
			move_cmd_read_descbuf = MOVE(DESCBUF, 0, MATH1, 0,
						     IMM(6), WITH(0));
			move_cmd_write_descbuf = MOVE(MATH1, 0, DESCBUF, 0,
						     IMM(8), WITH(WAITCOMP));
		}

		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F9, OP_ALG_AAI_F9,
			      OP_ALG_AS_INITFINAL,
			      dir == OP_TYPE_ENCAP_PROTOCOL ?
				     ICV_CHECK_DISABLE : ICV_CHECK_ENABLE,
			      OP_ALG_ENCRYPT);

		if (rta_sec_era > RTA_SEC_ERA_2) {
			SEQFIFOLOAD(MSGINSNOOP, 0,
				    WITH(VLF | LAST1 | LAST2 | FLUSH1));
			MOVE(AB1, 0, OFIFO, 0, MATH1, WITH(0));
		} else {
			SEQFIFOLOAD(MSGINSNOOP, 0,
				    WITH(VLF | LAST1 | LAST2 | FLUSH1));
			SET_LABEL(local_offset);

			/* Shut off automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0,
			     WITH(0));
			/*
			 * Placeholder for MOVE command with length from M1
			 * register
			 */
			MOVE(IFIFOAB1, 0, OFIFO, 0, IMM(0), WITH(0));
			/* Enable automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0,
			     WITH(0));
		}

		if (dir == OP_TYPE_DECAP_PROTOCOL)
			SEQFIFOLOAD(ICV2, 4, WITH(LAST2));
		else
			SEQSTORE(CONTEXT2, 0, 4, WITH(0));

		break;

	case PDCP_AUTH_TYPE_AES:
		/* Insert Auth Key */
		KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		SEQLOAD(MATH0, 7, 1, WITH(0));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
		if (rta_sec_era > RTA_SEC_ERA_2 ||
		    (rta_sec_era == RTA_SEC_ERA_2 &&
		     era_2_sw_hfn_override == 0)) {
			SEQINPTR(0, 1, WITH(RTO));
		} else {
			SEQINPTR(0, 5, WITH(RTO));
			SEQFIFOLOAD(SKIP, 4, WITH(0));
		}

		MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8),
		      WITH(IFB));
		MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
		MOVE(DESCBUF, 8, MATH2, 0, IMM(8), WITH(WAITCOMP));
		MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));
		MOVE(MATH2, 0, IFIFOAB1, 0, IMM(8), WITH(0));
		if (dir == OP_TYPE_DECAP_PROTOCOL) {
			MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), MATH1,
			      SIZE(4), WITH(0));
		} else {
			if (rta_sec_era > RTA_SEC_ERA_2) {
				MATHB(SEQINSZ, SUB, ZERO, MATH1, SIZE(4),
				      WITH(0));
			} else {
				MATHB(SEQINSZ, ADD, ONE, MATH1, SIZE(4),
				      WITH(0));
				MATHB(MATH1, SUB, ONE, MATH1, SIZE(4),
				      WITH(0));
			}
		}

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MATHB(MATH1, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
			MATHB(MATH1, SUB, ZERO, VSEQOUTSZ, SIZE(4), WITH(0));
		} else {
			MATHB(ZERO, ADD, MATH1, VSEQINSZ, SIZE(4), WITH(0));
			MATHB(ZERO, ADD, MATH1, VSEQOUTSZ, SIZE(4), WITH(0));

			/*
			 * Since MOVELEN is available only starting with
			 * SEC ERA 3, use poor man's MOVELEN: create a MOVE
			 * command dynamically by writing the length from M1 by
			 * OR-ing the command in the M1 register and MOVE the
			 * result into the descriptor buffer. Care must be taken
			 * wrt. the location of the command because of SEC
			 * pipelining. The actual MOVEs are written at the end
			 * of the descriptor due to calculations needed on the
			 * offset in the descriptor for the MOVE command.
			 */
			move_cmd_read_descbuf = MOVE(DESCBUF, 0, MATH1, 0,
						     IMM(6), WITH(0));
			move_cmd_write_descbuf = MOVE(MATH1, 0, DESCBUF, 0,
						      IMM(8), WITH(WAITCOMP));
		}
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CMAC,
			      OP_ALG_AS_INITFINAL,
			      dir == OP_TYPE_ENCAP_PROTOCOL ?
				     ICV_CHECK_DISABLE : ICV_CHECK_ENABLE,
			      OP_ALG_ENCRYPT);

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MOVE(AB2, 0, OFIFO, 0, MATH1, WITH(0));
			SEQFIFOLOAD(MSGINSNOOP, 0,
				    WITH(VLF | LAST1 | LAST2 | FLUSH1));
		} else {
			SEQFIFOLOAD(MSGINSNOOP, 0,
				    WITH(VLF | LAST1 | LAST2 | FLUSH1));
			SET_LABEL(local_offset);

			/* Shut off automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0,
			     WITH(0));

			/*
			 * Placeholder for MOVE command with length from
			 * M1 register
			 * */
			MOVE(IFIFOAB2, 0, OFIFO, 0, IMM(0), WITH(0));

			/* Enable automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0,
			     WITH(0));
		}

		if (dir == OP_TYPE_DECAP_PROTOCOL)
			SEQFIFOLOAD(ICV1, 4, WITH(LAST1 | FLUSH1));
		else
			SEQSTORE(CONTEXT1, 0, 4, WITH(0));

		break;

	case PDCP_AUTH_TYPE_ZUC:
		if (rta_sec_era < RTA_SEC_ERA_5) {
			pr_debug("Invalid era for selected algorithm\n");
			return -1;
		}
		/* Insert Auth Key */
		KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		SEQLOAD(MATH0, 7, 1, WITH(0));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
		SEQINPTR(0, 1, WITH(RTO));
		MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8),
		      WITH(IFB));
		MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
		MOVE(DESCBUF, 8, MATH2, 8, IMM(8), WITH(WAITCOMP));
		MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));
		MOVE(MATH2, 0, CONTEXT2, 0, IMM(8), WITH(0));

		if (dir == OP_TYPE_DECAP_PROTOCOL)
			MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), MATH1,
			      SIZE(4), WITH(0));
		else
			MATHB(SEQINSZ, SUB, ZERO, MATH1, SIZE(4), WITH(0));

		MATHB(MATH1, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		MATHB(MATH1, SUB, ZERO, VSEQOUTSZ, SIZE(4), WITH(0));
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		ALG_OPERATION(OP_ALG_ALGSEL_ZUCA,
			      OP_ALG_AAI_F9,
			      OP_ALG_AS_INITFINAL,
			      dir == OP_TYPE_ENCAP_PROTOCOL ?
				     ICV_CHECK_DISABLE : ICV_CHECK_ENABLE,
			      OP_ALG_ENCRYPT);
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST1 | LAST2 | FLUSH1));
		MOVE(AB1, 0, OFIFO, 0, MATH1, WITH(0));

		if (dir == OP_TYPE_DECAP_PROTOCOL)
			SEQFIFOLOAD(ICV2, 4, WITH(LAST2));
		else
			SEQSTORE(CONTEXT2, 0, 4, WITH(0));

		break;

	default:
		pr_debug("%s: Invalid integrity algorithm selected: %d\n",
			 "pdcp_insert_cplane_int_only_op",
			 authdata->algtype);
		return -1;
	}

	if (rta_sec_era < RTA_SEC_ERA_3) {
		PATCH_MOVE(move_cmd_read_descbuf, local_offset);
		PATCH_MOVE(move_cmd_write_descbuf, local_offset);
	}

	return 0;
}

static inline int pdcp_insert_cplane_enc_only_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	/* Insert Cipher Key */
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(0));
	SEQLOAD(MATH0, 7, 1, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8), WITH(IFB));
	SEQSTORE(MATH0, 7, 1, WITH(0));
	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 8, MATH2, 8, IMM(8), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));

	switch (cipherdata->algtype) {
	case PDCP_CIPHER_TYPE_SNOW:
		MOVE(MATH2, 0, CONTEXT1, 0, IMM(8), WITH(WAITCOMP));

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		} else {
			MATHB(SEQINSZ, SUB, ONE, MATH1, SIZE(4), WITH(0));
			MATHB(MATH1, ADD, ONE, VSEQINSZ, SIZE(4), WITH(0));
		}

		MATHB(SEQINSZ, dir == OP_TYPE_ENCAP_PROTOCOL ? ADD : SUB,
		      IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4), WITH(0));
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F8,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE,
			      dir == OP_TYPE_ENCAP_PROTOCOL ?
					OP_ALG_ENCRYPT : OP_ALG_DECRYPT);
		break;

	case PDCP_CIPHER_TYPE_AES:
		MOVE(MATH2, 0, CONTEXT1, 0x10, IMM(0x10), WITH(WAITCOMP));

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		} else {
			MATHB(SEQINSZ, SUB, ONE, MATH1, SIZE(4), WITH(0));
			MATHB(MATH1, ADD, ONE, VSEQINSZ, SIZE(4), WITH(0));
		}

		MATHB(SEQINSZ,
		      dir == OP_TYPE_ENCAP_PROTOCOL ? ADD : SUB,
			     IMM(PDCP_MAC_I_LEN),
		      VSEQOUTSZ,
		      SIZE(4),
		      0);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CTR,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      dir == OP_TYPE_ENCAP_PROTOCOL ?
					OP_ALG_ENCRYPT : OP_ALG_DECRYPT);
		break;

	case PDCP_CIPHER_TYPE_ZUC:
		if (rta_sec_era < RTA_SEC_ERA_5) {
			pr_debug("Invalid era for selected algorithm\n");
			return -1;
		}

		MOVE(MATH2, 0, CONTEXT1, 0, IMM(0x08), WITH(0));
		MOVE(MATH2, 0, CONTEXT1, 0x08, IMM(0x08), WITH(WAITCOMP));
		MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		MATHB(SEQINSZ,
		      dir == OP_TYPE_ENCAP_PROTOCOL ? ADD : SUB,
			     IMM(PDCP_MAC_I_LEN),
		      VSEQOUTSZ,
		      SIZE(4),
		      0);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		ALG_OPERATION(OP_ALG_ALGSEL_ZUCE,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      dir == OP_TYPE_ENCAP_PROTOCOL ?
					OP_ALG_ENCRYPT : OP_ALG_DECRYPT);
		break;

	default:
		pr_debug("%s: Invalid encrypt algorithm selected: %d\n",
			 "pdcp_insert_cplane_enc_only_op",
			 cipherdata->algtype);
		return -1;
	}

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		SEQFIFOLOAD(MSG1, 0, WITH(VLF));
		FIFOLOAD(MSG1, IMM(PDCP_NULL_INT_MAC_I_VAL), 4,
			 WITH(LAST1 | FLUSH1));
	} else {
		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));
		MOVE(OFIFO, 0, MATH1, 4, IMM(PDCP_MAC_I_LEN),
		     WITH(WAITCOMP));
		MATHB(MATH1, XOR, IMM(PDCP_NULL_INT_MAC_I_VAL), NONE,
		      SIZE(4), WITH(0));
		JUMP(IMM(PDCP_NULL_INT_ICV_CHECK_FAILED_STATUS),
		     HALT_STATUS, ALL_FALSE, WITH(MATH_Z));
	}

	return 0;
}

static inline int pdcp_insert_cplane_acc_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_hfn_override)
{
	/* Insert Auth Key */
	KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key), authdata->keylen,
	    WITH(0));

	/* Insert Cipher Key */
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(0));
	PROTOCOL(dir, OP_PCLID_LTE_PDCP_CTRL, (uint16_t)cipherdata->algtype);

	return 0;
}

static inline int pdcp_insert_cplane_snow_aes_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	LABEL(back_to_sd_offset);
	LABEL(end_desc);
	LABEL(local_offset);
	LABEL(jump_to_beginning);
	LABEL(fifo_load_mac_i_offset);
	REFERENCE(seqin_ptr_read);
	REFERENCE(seqin_ptr_write);
	REFERENCE(seq_out_read);
	REFERENCE(jump_back_to_sd_cmd);
	REFERENCE(move_mac_i_to_desc_buf);

	SEQLOAD(MATH0, 7, 1, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8), WITH(IFB));
	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 4, MATH2, 0, IMM(0x08), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));
	SEQSTORE(MATH0, 7, 1, WITH(0));
	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		if (rta_sec_era > RTA_SEC_ERA_2 ||
		    (rta_sec_era == RTA_SEC_ERA_2 &&
				   era_2_sw_hfn_override == 0)) {
			SEQINPTR(0, 1, WITH(RTO));
		} else {
			SEQINPTR(0, 5, WITH(RTO));
			SEQFIFOLOAD(SKIP, 4, WITH(0));
		}
		KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		MOVE(MATH2, 0, IFIFOAB1, 0, IMM(0x08), WITH(0));

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
			MATHB(SEQINSZ, SUB, ZERO, MATH1, SIZE(4), WITH(0));
			MATHB(VSEQINSZ, ADD, IMM(PDCP_MAC_I_LEN - 1),
			      VSEQOUTSZ, SIZE(4), 0);
		} else {
			MATHB(SEQINSZ, SUB, MATH3, VSEQINSZ, SIZE(4), WITH(0));
			MATHB(VSEQINSZ, ADD, IMM(PDCP_MAC_I_LEN - 1), VSEQOUTSZ,
			      SIZE(4), 0);
			/*
			 * Note: Although the calculations below might seem a
			 * little off, the logic is the following:
			 *
			 * - SEQ IN PTR RTO below needs the full length of the
			 *   frame; in case of P4080_REV_2_HFN_OV_WORKAROUND,
			 *   this means the length of the frame to be processed
			 *   + 4 bytes (the HFN override flag and value).
			 *   The length of the frame to be processed minus 1
			 *   byte is in the VSIL register (because
			 *   VSIL = SIL + 3, due to 1 byte, the header being
			 *   already written by the SEQ STORE above). So for
			 *   calculating the length to use in RTO, I add one
			 *   to the VSIL value in order to obtain the total
			 *   frame length. This helps in case of P4080 which
			 *   can have the value 0 as an operand in a MATH
			 *   command only as SRC1 When the HFN override
			 *   workaround is not enabled, the length of the
			 *   frame is given by the SIL register; the
			 *   calculation is similar to the one in the SEC 4.2
			 *   and SEC 5.3 cases.
			 */
			if (era_2_sw_hfn_override)
				MATHB(VSEQOUTSZ, ADD, ONE, MATH1, SIZE(4),
				      WITH(0));
			else
				MATHB(SEQINSZ, ADD, MATH3, MATH1, SIZE(4),
				      WITH(0));
		}
		/*
		 * Placeholder for filling the length in
		 * SEQIN PTR RTO below
		 */
		seqin_ptr_read = MOVE(DESCBUF, 0, MATH1, 0, IMM(6), WITH(0));
		seqin_ptr_write = MOVE(MATH1, 0, DESCBUF, 0, IMM(8),
				       WITH(WAITCOMP));
		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CMAC,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_DECRYPT);
		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));
		MOVE(CONTEXT1, 0, MATH3, 0, IMM(4), WITH(WAITCOMP));
		if (rta_sec_era <= RTA_SEC_ERA_3)
			LOAD(IMM(CLRW_CLR_C1KEY |
				 CLRW_CLR_C1CTX |
				 CLRW_CLR_C1ICV |
				 CLRW_CLR_C1DATAS |
				 CLRW_CLR_C1MODE),
			     CLRW, 0, 4, WITH(0));
		else
			LOAD(IMM(CLRW_RESET_CLS1_CHA |
				 CLRW_CLR_C1KEY |
				 CLRW_CLR_C1CTX |
				 CLRW_CLR_C1ICV |
				 CLRW_CLR_C1DATAS |
				 CLRW_CLR_C1MODE),
			     CLRW, 0, 4, WITH(0));

		if (rta_sec_era <= RTA_SEC_ERA_3)
			LOAD(IMM(CCTRL_RESET_CHA_ALL), CCTRL, 0, 4, WITH(0));

		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(0));
		SET_LABEL(local_offset);
		MOVE(MATH2, 0, CONTEXT1, 0, IMM(8), WITH(0));
		SEQINPTR(0, 0, WITH(RTO));

		if (rta_sec_era == RTA_SEC_ERA_2 && era_2_sw_hfn_override) {
			SEQFIFOLOAD(SKIP, 5, WITH(0));
			MATHB(SEQINSZ, ADD, ONE, SEQINSZ, SIZE(4), WITH(0));
		}

		MATHB(SEQINSZ, SUB, ONE, VSEQINSZ, SIZE(4), WITH(0));
		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F8,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_ENCRYPT);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

		if (rta_sec_era > RTA_SEC_ERA_2 ||
		    (rta_sec_era == RTA_SEC_ERA_2 &&
				   era_2_sw_hfn_override == 0))
			SEQFIFOLOAD(SKIP, 1, WITH(0));

		SEQFIFOLOAD(MSG1, 0, WITH(VLF));
		MOVE(MATH3, 0, IFIFOAB1, 0, IMM(4), WITH(LAST1 | FLUSH1));
		PATCH_MOVE(seqin_ptr_read, local_offset);
		PATCH_MOVE(seqin_ptr_write, local_offset);
	} else {
		MOVE(MATH2, 0, CONTEXT1, 0, IMM(8), WITH(0));

		if (rta_sec_era >= RTA_SEC_ERA_5)
			MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(8), WITH(0));

		if (rta_sec_era > RTA_SEC_ERA_2)
			MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		else
			MATHB(SEQINSZ, SUB, MATH3, VSEQINSZ, SIZE(4), WITH(0));

		MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));
/*
 * TODO: To be changed when proper support is added in RTA (can't load a
 * command that is also written by RTA (or patch it for that matter).
 * Change when proper RTA support is added.
 */
		if (program->ps)
			WORD(0x168B0004);
		else
			WORD(0x16880404);

		jump_back_to_sd_cmd = JUMP(IMM(0), LOCAL_JUMP, ALL_TRUE,
					   WITH(0));
		/*
		 * Placeholder for command reading  the SEQ OUT command in
		 * JD. Done for rereading the decrypted data and performing
		 * the integrity check
		 */
/*
 * TODO: RTA currently doesn't support patching of length of a MOVE command
 * Thus, it is inserted as a raw word, as per PS setting.
 */
		if (program->ps)
			seq_out_read = MOVE(DESCBUF, 0, MATH1, 0, IMM(20),
					    WITH(WAITCOMP));
		else
			seq_out_read = MOVE(DESCBUF, 0, MATH1, 0, IMM(16),
					    WITH(WAITCOMP));

		MATHB(MATH1, XOR, IMM(CMD_SEQ_IN_PTR ^ CMD_SEQ_OUT_PTR), MATH1,
		      SIZE(4), WITH(0));
		/* Placeholder for overwriting the SEQ IN  with SEQ OUT */
/*
 * TODO: RTA currently doesn't support patching of length of a MOVE command
 * Thus, it is inserted as a raw word, as per PS setting.
 */
		if (program->ps)
			MOVE(MATH1, 0, DESCBUF, 0, IMM(24), WITH(0));
		else
			MOVE(MATH1, 0, DESCBUF, 0, IMM(20), WITH(0));

		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(0));

		if (rta_sec_era >= RTA_SEC_ERA_5)
			MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(8), WITH(0));
		else
			MOVE(CONTEXT1, 0, MATH3, 0, IMM(8), WITH(0));

		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F8,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_DECRYPT);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

		if (rta_sec_era <= RTA_SEC_ERA_3)
			move_mac_i_to_desc_buf = MOVE(OFIFO, 0, DESCBUF, 0,
						      IMM(4), WITH(WAITCOMP));
		else
			MOVE(OFIFO, 0, MATH3, 0, IMM(4), WITH(0));

		if (rta_sec_era <= RTA_SEC_ERA_3)
			LOAD(IMM(CCTRL_RESET_CHA_ALL), CCTRL, 0, 4, WITH(0));
		else
			LOAD(IMM(CLRW_RESET_CLS1_CHA |
				 CLRW_CLR_C1KEY |
				 CLRW_CLR_C1CTX |
				 CLRW_CLR_C1ICV |
				 CLRW_CLR_C1DATAS |
				 CLRW_CLR_C1MODE),
			     CLRW, 0, 4, WITH(0));

		KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		/*
		 * Placeholder for jump in SD for executing the new SEQ IN PTR
		 * command (which is actually the old SEQ OUT PTR command
		 * copied over from JD.
		 */
		SET_LABEL(jump_to_beginning);
		JUMP(IMM(1 - jump_to_beginning), LOCAL_JUMP, ALL_TRUE, WITH(0));
		SET_LABEL(back_to_sd_offset);
		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CMAC,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_ENABLE,
			      OP_ALG_DECRYPT);

		if (rta_sec_era > RTA_SEC_ERA_2)
			MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		else
			MATHB(VSEQOUTSZ, ADD, ONE, VSEQINSZ, SIZE(4), WITH(0));

		if (rta_sec_era <= RTA_SEC_ERA_3)
			MOVE(MATH3, 0, IFIFOAB1, 0, IMM(8), WITH(0));
		else
			MOVE(CONTEXT2, 0, IFIFOAB1, 0, IMM(8), WITH(0));

		if (rta_sec_era == RTA_SEC_ERA_2 && era_2_sw_hfn_override)
			SEQFIFOLOAD(SKIP, 4, WITH(0));

		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

		if (rta_sec_era >= RTA_SEC_ERA_5) {
			LOAD(IMM(NFIFOENTRY_STYPE_ALTSOURCE |
				 NFIFOENTRY_DEST_CLASS1 |
				 NFIFOENTRY_DTYPE_ICV |
				 NFIFOENTRY_LC1 |
				 NFIFOENTRY_FC1 | 4), NFIFO_SZL, 0, 4, WITH(0));
			MOVE(MATH3, 0, ALTSOURCE, 0, IMM(4), WITH(0));
		} else {
			SET_LABEL(fifo_load_mac_i_offset);
			FIFOLOAD(ICV1, IMM(fifo_load_mac_i_offset), 4,
				 WITH(LAST1 | FLUSH1));
		}

		SET_LABEL(end_desc);

		if (!program->ps) {
			PATCH_MOVE(seq_out_read, end_desc + 1);
			PATCH_JUMP(jump_back_to_sd_cmd,
				   back_to_sd_offset + jump_back_to_sd_cmd - 5);

			if (rta_sec_era <= RTA_SEC_ERA_3)
				PATCH_MOVE(move_mac_i_to_desc_buf,
					   fifo_load_mac_i_offset + 1);
		} else {
			PATCH_MOVE(seq_out_read, end_desc + 2);
			PATCH_JUMP(jump_back_to_sd_cmd,
				   back_to_sd_offset + jump_back_to_sd_cmd - 5);

			if (rta_sec_era <= RTA_SEC_ERA_3)
				PATCH_MOVE(move_mac_i_to_desc_buf,
					   fifo_load_mac_i_offset + 1);
		}
	}

	return 0;
}

static inline int pdcp_insert_cplane_aes_snow_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	LABEL(local_offset);
	REFERENCE(read_load_nfifo);
	REFERENCE(write_load_nfifo);

	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(0));
	KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
	    authdata->keylen, WITH(0));

	if (rta_sec_era <= RTA_SEC_ERA_2)
		MATHB(SEQINSZ, SUB, ONE, VSEQINSZ, SIZE(4), WITH(0));

	SEQLOAD(MATH0, 7, 1, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MOVE(MATH0, 7, IFIFOAB2, 0, IMM(1), WITH(0));
	MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8), WITH(IFB));
	SEQSTORE(MATH0, 7, 1, WITH(0));
	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 4, MATH2, 0, IMM(8), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH1, SIZE(8), WITH(0));
	MOVE(MATH1, 0, CONTEXT1, 16, IMM(8), WITH(0));
	MOVE(MATH1, 0, CONTEXT2, 0, IMM(4), WITH(0));
	MATHB(MATH1, AND, IMM(low_32b(PDCP_BEARER_MASK)), MATH2,
	      SIZE(4), WITH(0));
	MATHB(MATH1, AND, IMM(high_32b(PDCP_DIR_MASK)), MATH3, SIZE(4),
	      WITH(0));
	MATHB(MATH3, SHLD, MATH3, MATH3, SIZE(8), WITH(0));
	MOVE(MATH2, 4, OFIFO, 0, IMM(12), WITH(0));
	MOVE(OFIFO, 0, CONTEXT2, 4, IMM(12), WITH(0));
	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		MATHB(SEQINSZ, ADD, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));
	} else {
		MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), MATH1, SIZE(4),
		      WITH(0));

		if (rta_sec_era <= RTA_SEC_ERA_2)
			MATHB(ZERO, ADD, MATH1, VSEQOUTSZ, SIZE(4), WITH(0));
		else
			MATHB(MATH1, SUB, ZERO, VSEQOUTSZ, SIZE(4), WITH(0));

		read_load_nfifo = MOVE(DESCBUF, 0, MATH1, 0, IMM(6),
				       WITH(WAITCOMP));
		write_load_nfifo = MOVE(MATH1, 0, DESCBUF, 0, IMM(8),
					WITH(WAITCOMP));
	}

	if (dir == OP_TYPE_ENCAP_PROTOCOL)
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
	else
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));

	if (rta_sec_era > RTA_SEC_ERA_2)
		MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));

	ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F9,
		      OP_ALG_AAI_F9,
		      OP_ALG_AS_INITFINAL,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     ICV_CHECK_DISABLE : ICV_CHECK_ENABLE,
		      OP_ALG_DECRYPT);
	SET_LABEL(local_offset);
	ALG_OPERATION(OP_ALG_ALGSEL_AES,
		      OP_ALG_AAI_CTR,
		      OP_ALG_AS_INITFINAL,
		      ICV_CHECK_DISABLE,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     OP_ALG_ENCRYPT : OP_ALG_DECRYPT);

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST2));
		MOVE(CONTEXT2, 0, IFIFOAB1, 0, IMM(4), WITH(LAST1 | FLUSH1));
	} else {
		SEQFIFOLOAD(MSGOUTSNOOP, 0, WITH(LAST2));
		SEQFIFOLOAD(MSG1, 4, WITH(LAST1 | FLUSH1));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CLASS1 | NOP | NIFP));

		if (rta_sec_era >= RTA_SEC_ERA_6)
			LOAD(IMM(0), DCTRL, 0, LDLEN_RST_CHA_OFIFO_PTR,
			     WITH(0));

		MOVE(OFIFO, 0, MATH0, 0, IMM(4), WITH(WAITCOMP));

		NFIFOADD(IFIFO, ICV2, 4, WITH(LAST2));

		if (rta_sec_era <= RTA_SEC_ERA_2) {
			/* Shut off automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0,
			     WITH(0));
			MOVE(MATH0, 0, IFIFOAB2, 0, IMM(4), WITH(WAITCOMP));
		} else {
			MOVE(MATH0, 0, IFIFO, 0, IMM(4), WITH(WAITCOMP));
		}

		PATCH_MOVE(read_load_nfifo, local_offset);
		PATCH_MOVE(write_load_nfifo, local_offset);
	}

	return 0;
}

static inline int pdcp_insert_cplane_snow_zuc_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	LABEL(keyjump);
	REFERENCE(pkeyjump);

	if (rta_sec_era < RTA_SEC_ERA_5) {
		pr_debug("Invalid era for selected algorithm\n");
		return -1;
	}

	pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE,
			WITH(SHRD | SELF | BOTH));
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(0));
	KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key), authdata->keylen,
	    WITH(0));

	SET_LABEL(keyjump);
	SEQLOAD(MATH0, 7, 1, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MOVE(MATH0, 7, IFIFOAB2, 0, IMM(1), WITH(0));
	MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8), WITH(IFB));
	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 4, MATH2, 0, IMM(8), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));
	MOVE(MATH2, 0, CONTEXT1, 0, IMM(8), WITH(0));
	MOVE(MATH2, 0, CONTEXT2, 0, IMM(8), WITH(WAITCOMP));

	if (dir == OP_TYPE_ENCAP_PROTOCOL)
		MATHB(SEQINSZ, ADD, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));
	else
		MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));

	MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
	SEQSTORE(MATH0, 7, 1, WITH(0));

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST2));
	} else {
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		SEQFIFOLOAD(MSGOUTSNOOP, 0, WITH(VLF | LAST1 | FLUSH1));
	}

	ALG_OPERATION(OP_ALG_ALGSEL_ZUCA,
		      OP_ALG_AAI_F9,
		      OP_ALG_AS_INITFINAL,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     ICV_CHECK_DISABLE : ICV_CHECK_ENABLE,
		      OP_ALG_ENCRYPT);

	ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F8,
		      OP_ALG_AAI_F8,
		      OP_ALG_AS_INITFINAL,
		      ICV_CHECK_DISABLE,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     OP_ALG_ENCRYPT : OP_ALG_DECRYPT);
	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		MOVE(CONTEXT2, 0, IFIFOAB1, 0, IMM(4), WITH(LAST1 | FLUSH1));
	} else {
		/* Save ICV */
		MOVE(OFIFO, 0, MATH0, 0, IMM(4), WITH(0));
		LOAD(IMM(NFIFOENTRY_STYPE_ALTSOURCE |
			 NFIFOENTRY_DEST_CLASS2 |
			 NFIFOENTRY_DTYPE_ICV |
			 NFIFOENTRY_LC2 | 4), NFIFO_SZL, 0, 4, WITH(0));
		MOVE(MATH0, 0, ALTSOURCE, 0, IMM(4), WITH(WAITCOMP));
	}

	/* Reset ZUCA mode and done interrupt */
	LOAD(IMM(CLRW_CLR_C2MODE), CLRW, 0, 4, WITH(0));
/* TODO: Add ICTRL definitions */
	LOAD(IMM(0x00001000), ICTRL, 0, 4, WITH(0));

	PATCH_JUMP(pkeyjump, keyjump);
	return 0;
}

static inline int pdcp_insert_cplane_aes_zuc_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	LABEL(keyjump);
	REFERENCE(pkeyjump);

	if (rta_sec_era < RTA_SEC_ERA_5) {
		pr_debug("Invalid era for selected algorithm\n");
		return -1;
	}

	pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE,
			WITH(SHRD | SELF | BOTH));
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(0));
	KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key), authdata->keylen,
	    WITH(0));

	SET_LABEL(keyjump);
	SEQLOAD(MATH0, 7, 1, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MOVE(MATH0, 7, IFIFOAB2, 0, IMM(1), WITH(0));
	MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8), WITH(IFB));

	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 4, MATH2, 0, IMM(8), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));
	MOVE(MATH2, 0, CONTEXT1, 16, IMM(8), WITH(0));
	MOVE(MATH2, 0, CONTEXT2, 0, IMM(8), WITH(WAITCOMP));

	if (dir == OP_TYPE_ENCAP_PROTOCOL)
		MATHB(SEQINSZ, ADD, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));
	else
		MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));

	MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
	SEQSTORE(MATH0, 7, 1, WITH(0));

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST2));
	} else {
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		SEQFIFOLOAD(MSGOUTSNOOP, 0, WITH(VLF | LAST1 | FLUSH1));
	}

	ALG_OPERATION(OP_ALG_ALGSEL_ZUCA,
		      OP_ALG_AAI_F9,
		      OP_ALG_AS_INITFINAL,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     ICV_CHECK_DISABLE : ICV_CHECK_ENABLE,
		      OP_ALG_ENCRYPT);

	ALG_OPERATION(OP_ALG_ALGSEL_AES,
		      OP_ALG_AAI_CTR,
		      OP_ALG_AS_INITFINAL,
		      ICV_CHECK_DISABLE,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     OP_ALG_ENCRYPT : OP_ALG_DECRYPT);

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		MOVE(CONTEXT2, 0, IFIFOAB1, 0, IMM(4), WITH(LAST1 | FLUSH1));
	} else {
		/* Save ICV */
		MOVE(OFIFO, 0, MATH0, 0, IMM(4), WITH(0));

		LOAD(IMM(NFIFOENTRY_STYPE_ALTSOURCE |
			 NFIFOENTRY_DEST_CLASS2 |
			 NFIFOENTRY_DTYPE_ICV |
			 NFIFOENTRY_LC2 | 4), NFIFO_SZL, 0, 4, WITH(0));
		MOVE(MATH0, 0, ALTSOURCE, 0, IMM(4), WITH(WAITCOMP));
	}

	/* Reset ZUCA mode and done interrupt */
	LOAD(IMM(CLRW_CLR_C2MODE), CLRW, 0, 4, WITH(0));
/* TODO: Add ICTRL definitions */
	LOAD(IMM(0x00001000), ICTRL, 0, 4, WITH(0));

	PATCH_JUMP(pkeyjump, keyjump);

	return 0;
}

static inline int pdcp_insert_cplane_zuc_snow_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	LABEL(keyjump);
	REFERENCE(pkeyjump);

	if (rta_sec_era < RTA_SEC_ERA_5) {
		pr_debug("Invalid era for selected algorithm\n");
		return -1;
	}

	pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE,
				WITH(SHRD | SELF | BOTH));
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(0));
	KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key), authdata->keylen,
	    WITH(0));

	SET_LABEL(keyjump);
	SEQLOAD(MATH0, 7, 1, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MOVE(MATH0, 7, IFIFOAB2, 0, IMM(1), WITH(0));
	MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8), WITH(IFB));
	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 4, MATH2, 0, IMM(8), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH1, SIZE(8), WITH(0));
	MOVE(MATH1, 0, CONTEXT1, 0, IMM(8), WITH(0));
	MOVE(MATH1, 0, CONTEXT2, 0, IMM(4), WITH(0));
	MATHB(MATH1, AND, IMM(low_32b(PDCP_BEARER_MASK)), MATH2,
	      SIZE(4), WITH(0));
	MATHB(MATH1, AND, IMM(high_32b(PDCP_DIR_MASK)), MATH3, SIZE(4),
	      WITH(0));
	MATHB(MATH3, SHLD, MATH3, MATH3, SIZE(8), WITH(0));
	MOVE(MATH2, 4, OFIFO, 0, IMM(12), WITH(0));
	MOVE(OFIFO, 0, CONTEXT2, 4, IMM(12), WITH(0));

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		MATHB(SEQINSZ, ADD, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));
		MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
	} else {
		MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));
		MATHB(VSEQOUTSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
	}

	SEQSTORE(MATH0, 7, 1, WITH(0));

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST2));
	} else {
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		SEQFIFOLOAD(MSGOUTSNOOP, 0, WITH(VLF | LAST2));
	}

	ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F9,
		      OP_ALG_AAI_F9,
		      OP_ALG_AS_INITFINAL,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     ICV_CHECK_DISABLE : ICV_CHECK_ENABLE,
		      OP_ALG_DECRYPT);

	ALG_OPERATION(OP_ALG_ALGSEL_ZUCE,
		      OP_ALG_AAI_F8,
		      OP_ALG_AS_INITFINAL,
		      ICV_CHECK_DISABLE,
		      dir == OP_TYPE_ENCAP_PROTOCOL ?
			     OP_ALG_ENCRYPT : OP_ALG_DECRYPT);

	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		MOVE(CONTEXT2, 0, IFIFOAB1, 0, IMM(4), WITH(LAST1 | FLUSH1));
	} else {
		SEQFIFOLOAD(MSG1, 4, WITH(LAST1 | FLUSH1));

		/* Save ICV */
		MOVE(OFIFO, 0, MATH0, 0, IMM(4), WITH(WAITCOMP));

		LOAD(IMM(NFIFOENTRY_STYPE_ALTSOURCE |
			 NFIFOENTRY_DEST_CLASS2 |
			 NFIFOENTRY_DTYPE_ICV |
			 NFIFOENTRY_LC2 | 4), NFIFO_SZL, 0, 4, WITH(0));
		MOVE(MATH0, 0, ALTSOURCE, 0, IMM(4), WITH(0));
	}

	PATCH_JUMP(pkeyjump, keyjump);
	return 0;
}

static inline int pdcp_insert_cplane_zuc_aes_op(struct program *program,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned dir,
		unsigned char era_2_sw_hfn_override)
{
	if (rta_sec_era < RTA_SEC_ERA_5) {
		pr_debug("Invalid era for selected algorithm\n");
		return -1;
	}

	SEQLOAD(MATH0, 7, 1, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MATHB(MATH0, AND, IMM(PDCP_C_PLANE_SN_MASK), MATH1, SIZE(8), WITH(IFB));
	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 4, MATH2, 0, IMM(0x08), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));
	SEQSTORE(MATH0, 7, 1, WITH(0));
	if (dir == OP_TYPE_ENCAP_PROTOCOL) {
		KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		MOVE(MATH2, 0, IFIFOAB1, 0, IMM(0x08), WITH(0));
		MOVE(MATH0, 7, IFIFOAB1, 0, IMM(1), WITH(0));

		MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		MATHB(VSEQINSZ, ADD, IMM(PDCP_MAC_I_LEN),
		      VSEQOUTSZ, SIZE(4), 0);

		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CMAC,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_DECRYPT);
		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));
		MOVE(CONTEXT1, 0, MATH3, 0, IMM(4), WITH(WAITCOMP));
		LOAD(IMM(CLRW_RESET_CLS1_CHA |
			 CLRW_CLR_C1KEY |
			 CLRW_CLR_C1CTX |
			 CLRW_CLR_C1ICV |
			 CLRW_CLR_C1DATAS |
			 CLRW_CLR_C1MODE),
		     CLRW, 0, 4, WITH(0));

		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(0));

		MOVE(MATH2, 0, CONTEXT1, 0, IMM(8), WITH(0));
		SEQINPTR(0, PDCP_NULL_MAX_FRAME_LEN, WITH(RTO));

		ALG_OPERATION(OP_ALG_ALGSEL_ZUCE,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_ENCRYPT);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

		SEQFIFOLOAD(SKIP, 1, WITH(0));

		SEQFIFOLOAD(MSG1, 0, WITH(VLF));
		MOVE(MATH3, 0, IFIFOAB1, 0, IMM(4), WITH(LAST1 | FLUSH1));
	} else {
		MOVE(MATH2, 0, CONTEXT1, 0, IMM(8), WITH(0));

		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(8), WITH(0));

		MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));

		MATHB(SEQINSZ, SUB, IMM(PDCP_MAC_I_LEN), VSEQOUTSZ, SIZE(4),
		      WITH(0));

		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, WITH(0));

		MOVE(CONTEXT1, 0, CONTEXT2, 0, IMM(8), WITH(0));

		ALG_OPERATION(OP_ALG_ALGSEL_ZUCE,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_DECRYPT);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF | CONT));
		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

		MOVE(OFIFO, 0, MATH3, 0, IMM(4), WITH(0));

		LOAD(IMM(CLRW_RESET_CLS1_CHA |
			 CLRW_CLR_C1KEY |
			 CLRW_CLR_C1CTX |
			 CLRW_CLR_C1ICV |
			 CLRW_CLR_C1DATAS |
			 CLRW_CLR_C1MODE),
			 CLRW, 0, 4, WITH(0));

		KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));

/* TODO: Add support in RTA for SOP bit in SEQINPTR command */
		WORD(0xF0080000);

		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CMAC,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_ENABLE,
			      OP_ALG_DECRYPT);

		MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));

		MOVE(CONTEXT2, 0, IFIFOAB1, 0, IMM(8), WITH(0));

		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

		LOAD(IMM(NFIFOENTRY_STYPE_ALTSOURCE |
			 NFIFOENTRY_DEST_CLASS1 |
			 NFIFOENTRY_DTYPE_ICV |
			 NFIFOENTRY_LC1 |
			 NFIFOENTRY_FC1 | 4), NFIFO_SZL, 0, 4, WITH(0));
		MOVE(MATH3, 0, ALTSOURCE, 0, IMM(4), WITH(0));
	}

	return 0;
}

static inline int pdcp_insert_uplane_15bit_op(struct program *program,
					      struct alginfo *cipherdata,
					      unsigned dir)
{
	int op;
	/* Insert Cipher Key */
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(0));
	SEQLOAD(MATH0, 6, 2, WITH(0));
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
	MATHB(MATH0, AND, IMM(PDCP_U_PLANE_15BIT_SN_MASK), MATH1, SIZE(8),
	      WITH(IFB));
	SEQSTORE(MATH0, 6, 2, WITH(0));
	MATHB(MATH1, SHLD, MATH1, MATH1, SIZE(8), WITH(0));
	MOVE(DESCBUF, 8, MATH2, 8, IMM(8), WITH(WAITCOMP));
	MATHB(MATH1, OR, MATH2, MATH2, SIZE(8), WITH(0));

	MATHB(SEQINSZ, SUB, MATH3, VSEQINSZ, SIZE(4), WITH(0));
	MATHB(SEQINSZ, SUB, MATH3, VSEQOUTSZ, SIZE(4), WITH(0));

	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

	op = dir == OP_TYPE_ENCAP_PROTOCOL ? OP_ALG_ENCRYPT : OP_ALG_DECRYPT;
	switch (cipherdata->algtype) {
	case PDCP_CIPHER_TYPE_SNOW:
		MOVE(MATH2, 0, CONTEXT1, 0, IMM(8), WITH(WAITCOMP));
		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F8,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      op);
		break;

	case PDCP_CIPHER_TYPE_AES:
		MOVE(MATH2, 0, CONTEXT1, 0x10, IMM(0x10), WITH(WAITCOMP));
		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CTR,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      op);
		break;

	case PDCP_CIPHER_TYPE_ZUC:
		if (rta_sec_era < RTA_SEC_ERA_5) {
			pr_debug("Invalid era for selected algorithm\n");
			return -1;
		}
		MOVE(MATH2, 0, CONTEXT1, 0, IMM(0x08), WITH(0));
		MOVE(MATH2, 0, CONTEXT1, 0x08, IMM(0x08), WITH(WAITCOMP));

		ALG_OPERATION(OP_ALG_ALGSEL_ZUCE,
			      OP_ALG_AAI_F8,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      op);
		break;

	default:
		pr_debug("%s: Invalid encrypt algorithm selected: %d\n",
			 "pdcp_insert_uplane_15bit_op",
			 cipherdata->algtype);
		return -1;
	}

	SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

	return 0;
}

/*
 * Function for inserting the snippet of code responsible for creating
 * the HFN override code via either DPOVRD or via the input frame.
 */
static inline int pdcp_insert_hfn_ov_op(struct program *program,
		uint32_t shift,
		enum pdb_type_e pdb_type,
		unsigned char era_2_sw_hfn_override)
{
	uint32_t imm = 0x80000000;
	uint16_t hfn_pdb_offset;

	if (rta_sec_era == RTA_SEC_ERA_2 && !era_2_sw_hfn_override)
		return 0;

	switch (pdb_type) {
	case PDCP_PDB_TYPE_NO_PDB:
		/*
		 * If there is no PDB, then HFN override mechanism does not
		 * make any sense, thus in this case the function will
		 * return the pointer to the current position in the
		 * descriptor buffer
		 */
		return 0;

	case PDCP_PDB_TYPE_REDUCED_PDB:
		hfn_pdb_offset = 4;
		break;

	case PDCP_PDB_TYPE_FULL_PDB:
		hfn_pdb_offset = 8;
		break;

	default:
		return -1;
	}

	if (rta_sec_era > RTA_SEC_ERA_2) {
		MATHB(DPOVRD, AND, IMM(imm), NONE, SIZE(8), WITH(IFB));
	} else {
		SEQLOAD(MATH0, 4, 4, WITH(0));
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));
		MATHB(MATH0, AND, IMM(imm), NONE, SIZE(8), WITH(IFB));
		SEQSTORE(MATH0, 4, 4, WITH(0));
	}

	JUMP(IMM(5), LOCAL_JUMP, ALL_TRUE, WITH(MATH_Z));

	if (rta_sec_era > RTA_SEC_ERA_2)
		MATHB(DPOVRD, LSHIFT, IMM(shift), MATH0, SIZE(4), WITH(0));
	else
		MATHB(MATH0, LSHIFT, IMM(shift), MATH0, SIZE(4), WITH(0));

	MATHB(MATH0, SHLD, MATH0, MATH0, SIZE(8), WITH(0));
	MOVE(MATH0, 0, DESCBUF, hfn_pdb_offset, IMM(4), WITH(0));

	return 0;
}

/*
 * PDCP Control PDB creation function
 */
static inline enum pdb_type_e cnstr_pdcp_c_plane_pdb(struct program *program,
		uint32_t hfn,
		unsigned char bearer,
		unsigned char direction,
		uint32_t hfn_threshold,
		struct alginfo *cipherdata,
		struct alginfo *authdata)
{
	struct pdcp_pdb pdb;
	enum pdb_type_e
		pdb_mask[PDCP_CIPHER_TYPE_INVALID][PDCP_AUTH_TYPE_INVALID] = {
			{	/* NULL */
				PDCP_PDB_TYPE_NO_PDB,		/* NULL */
				PDCP_PDB_TYPE_FULL_PDB,		/* SNOW f9 */
				PDCP_PDB_TYPE_FULL_PDB,		/* AES CMAC */
				PDCP_PDB_TYPE_FULL_PDB		/* ZUC-I */
			},
			{	/* SNOW f8 */
				PDCP_PDB_TYPE_FULL_PDB,		/* NULL */
				PDCP_PDB_TYPE_FULL_PDB,		/* SNOW f9 */
				PDCP_PDB_TYPE_REDUCED_PDB,	/* AES CMAC */
				PDCP_PDB_TYPE_REDUCED_PDB	/* ZUC-I */
			},
			{	/* AES CTR */
				PDCP_PDB_TYPE_FULL_PDB,		/* NULL */
				PDCP_PDB_TYPE_REDUCED_PDB,	/* SNOW f9 */
				PDCP_PDB_TYPE_FULL_PDB,		/* AES CMAC */
				PDCP_PDB_TYPE_REDUCED_PDB	/* ZUC-I */
			},
			{	/* ZUC-E */
				PDCP_PDB_TYPE_FULL_PDB,		/* NULL */
				PDCP_PDB_TYPE_REDUCED_PDB,	/* SNOW f9 */
				PDCP_PDB_TYPE_REDUCED_PDB,	/* AES CMAC */
				PDCP_PDB_TYPE_FULL_PDB		/* ZUC-I */
			},
	};

	switch (pdb_mask[cipherdata->algtype][authdata->algtype]) {
	case PDCP_PDB_TYPE_NO_PDB:
		break;

	case PDCP_PDB_TYPE_REDUCED_PDB:
		WORD((hfn << PDCP_C_PLANE_PDB_HFN_SHIFT));
		WORD((uint32_t)((bearer << PDCP_C_PLANE_PDB_BEARER_SHIFT) |
			(direction << PDCP_C_PLANE_PDB_DIR_SHIFT)));
		break;

	case PDCP_PDB_TYPE_FULL_PDB:
		memset(&pdb, 0x00, sizeof(struct pdcp_pdb));

		/* This is a HW issue. Bit 2 should be set to zero,
		 * but it does not work this way. Override here.
		 */
		pdb.opt_res.rsvd = 0x00000002;

		/* Copy relevant information from user to PDB */
		pdb.hfn_res = hfn << PDCP_C_PLANE_PDB_HFN_SHIFT;
		pdb.bearer_dir_res = (uint32_t)
			((bearer << PDCP_C_PLANE_PDB_BEARER_SHIFT) |
			 (direction << PDCP_C_PLANE_PDB_DIR_SHIFT));
		pdb.hfn_thr_res =
			hfn_threshold << PDCP_C_PLANE_PDB_HFN_THR_SHIFT;

		/* copy PDB in descriptor*/
		ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct pdcp_pdb));

		break;

	default:
		return PDCP_PDB_TYPE_INVALID;
	}

	return pdb_mask[cipherdata->algtype][authdata->algtype];
}

/**
 * @details                   Function for creating a PDCP Control Plane
 *                            encapsulation descriptor.
 * @ingroup                   sharedesc_group
 *
 * @param [in,out] descbuf    Pointer to buffer for descriptor construction.
 *
 * @param [in,out] bufsize    Size of descriptor written. Once the function
 *                            returns, the value of this parameter can be used
 *                            for reclaiming the space that wasn't used for the
 *                            descriptor.
 *
 * @param [in] ps             If 36/40bit addressing is desired, this parameter
 *                            must be non-zero.
 *
 * @param [in] hfn            Starting Hyper Frame Number to be used together
 *                            with the SN from the PDCP frames.
 *
 * @param [in] bearer         Radio bearer ID.
 *
 * @param [in] direction      The direction of the PDCP frame (UL/DL).
 *
 * @param [in] hfn_threshold  HFN value that once reached triggers a warning
 *                            from SEC that keys should be renegociated at the
 *                            earliest convenience.
 *
 * @param [in] cipherdata     Pointer to block cipher transform definitions.
 *                            Valid algorithm values are those from
 *                            cipher_type_pdcp enum.
 *
 * @param [in] authdata       Pointer to authentication transform definitions.
 *                            Valid algorithm values are those from
 *                            auth_type_pdcp enum.
 *
 * @param [in] era_2_sw_hfn_override    If software HFN override mechanism is
 *                                      desired for this descriptor.
 *                                      @note Can only be used for SEC ERA 2
 *
 * @note  @b descbuf must be large enough to contain a full 256 byte long
 *        descriptor; after the function returns, by subtracting the actual
 *        number of bytes used (using @b bufsize), the user can reuse the
 *        remaining buffer space for other purposes.
 *
 */
static inline void cnstr_shdsc_pdcp_c_plane_encap(uint32_t *descbuf,
		unsigned *bufsize,
		unsigned short ps,
		uint32_t hfn,
		unsigned char bearer,
		unsigned char direction,
		uint32_t hfn_threshold,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned char era_2_sw_hfn_override)
{
	static int
		(*pdcp_cp_fp[PDCP_CIPHER_TYPE_INVALID][PDCP_AUTH_TYPE_INVALID])
			(struct program*, struct alginfo *,
			 struct alginfo *, unsigned, unsigned char) = {
		{	/* NULL */
			pdcp_insert_cplane_null_op,	/* NULL */
			pdcp_insert_cplane_int_only_op,	/* SNOW f9 */
			pdcp_insert_cplane_int_only_op,	/* AES CMAC */
			pdcp_insert_cplane_int_only_op	/* ZUC-I */
		},
		{	/* SNOW f8 */
			pdcp_insert_cplane_enc_only_op,	/* NULL */
			pdcp_insert_cplane_acc_op,	/* SNOW f9 */
			pdcp_insert_cplane_snow_aes_op,	/* AES CMAC */
			pdcp_insert_cplane_snow_zuc_op	/* ZUC-I */
		},
		{	/* AES CTR */
			pdcp_insert_cplane_enc_only_op,	/* NULL */
			pdcp_insert_cplane_aes_snow_op,	/* SNOW f9 */
			pdcp_insert_cplane_acc_op,	/* AES CMAC */
			pdcp_insert_cplane_aes_zuc_op	/* ZUC-I */
		},
		{	/* ZUC-E */
			pdcp_insert_cplane_enc_only_op,	/* NULL */
			pdcp_insert_cplane_zuc_snow_op,	/* SNOW f9 */
			pdcp_insert_cplane_zuc_aes_op,	/* AES CMAC */
			pdcp_insert_cplane_acc_op	/* ZUC-I */
		},
	};
	static uint32_t
		desc_share[PDCP_CIPHER_TYPE_INVALID][PDCP_AUTH_TYPE_INVALID] = {
		{	/* NULL */
			SHR_WAIT,	/* NULL */
			SHR_ALWAYS,	/* SNOW f9 */
			SHR_ALWAYS,	/* AES CMAC */
			SHR_ALWAYS	/* ZUC-I */
		},
		{	/* SNOW f8 */
			SHR_ALWAYS,	/* NULL */
			SHR_ALWAYS,	/* SNOW f9 */
			SHR_WAIT,	/* AES CMAC */
			SHR_WAIT	/* ZUC-I */
		},
		{	/* AES CTR */
			SHR_ALWAYS,	/* NULL */
			SHR_ALWAYS,	/* SNOW f9 */
			SHR_ALWAYS,	/* AES CMAC */
			SHR_WAIT	/* ZUC-I */
		},
		{	/* ZUC-E */
			SHR_ALWAYS,	/* NULL */
			SHR_WAIT,	/* SNOW f9 */
			SHR_WAIT,	/* AES CMAC */
			SHR_ALWAYS	/* ZUC-I */
		},
	};
	enum pdb_type_e pdb_type;
	struct program prg;
	struct program *program = &prg;
	LABEL(pdb_end);

	if (rta_sec_era != RTA_SEC_ERA_2 && era_2_sw_hfn_override) {
		pr_debug("Cannot select SW HFN override for other era than 2");
		return;
	}

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();

	SHR_HDR(desc_share[cipherdata->algtype][authdata->algtype], 0, WITH(0));

	pdb_type = cnstr_pdcp_c_plane_pdb(program,
			hfn,
			bearer,
			direction,
			hfn_threshold,
			cipherdata,
			authdata);

	SET_LABEL(pdb_end);

	if (pdcp_insert_hfn_ov_op(program, PDCP_SN_SIZE_5, pdb_type,
				  era_2_sw_hfn_override))
		return;

	if (pdcp_cp_fp[cipherdata->algtype][authdata->algtype](program,
			cipherdata,
			authdata,
			OP_TYPE_ENCAP_PROTOCOL,
			era_2_sw_hfn_override))
		return;

	PATCH_HDR(0, pdb_end);

	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                   Function for creating a PDCP Control Plane
 *                            decapsulation descriptor.
 * @ingroup                   sharedesc_group
 *
 * @param [in,out] descbuf    Pointer to buffer for descriptor construction.
 *
 * @param [in,out] bufsize    Size of descriptor written. Once the function
 *                            returns, the value of this parameter can be used
 *                            for reclaiming the space that wasn't used for the
 *                            descriptor.
 *
 * @param [in] ps             If 36/40bit addressing is desired, this parameter
 *                            must be non-zero.
 *
 * @param [in] hfn            Starting Hyper Frame Number to be used together
 *                            with the SN from the PDCP frames.
 *
 * @param [in] bearer         Radio bearer ID.
 *
 * @param [in] direction      The direction of the PDCP frame (UL/DL).
 *
 * @param [in] hfn_threshold  HFN value that once reached triggers a warning
 *                            from SEC that keys should be renegociated at the
 *                            earliest convenience.
 *
 * @param [in] cipherdata     Pointer to block cipher transform definitions.
 *                            Valid algorithm values are those from
 *                            cipher_type_pdcp enum.
 *
 * @param [in] authdata       Pointer to authentication transform definitions.
 *                            Valid algorithm values are those from
 *                            auth_type_pdcp enum.
 *
 * @param [in] era_2_sw_hfn_override    If software HFN override mechanism is
 *                                      desired for this descriptor.
 *                                      @note Can only be used for SEC ERA 2
 *
 * @note  @b descbuf must be large enough to contain a full 256 byte long
 *        descriptor; after the function returns, by subtracting the actual
 *        number of bytes used (using @b bufsize), the user can reuse the
 *        remaining buffer space for other purposes.
 *
 */
static inline void cnstr_shdsc_pdcp_c_plane_decap(uint32_t *descbuf,
		unsigned *bufsize,
		unsigned short ps,
		uint32_t hfn,
		unsigned char bearer,
		unsigned char direction,
		uint32_t hfn_threshold,
		struct alginfo *cipherdata,
		struct alginfo *authdata,
		unsigned char era_2_sw_hfn_override)
{
	static int
		(*pdcp_cp_fp[PDCP_CIPHER_TYPE_INVALID][PDCP_AUTH_TYPE_INVALID])
			(struct program*, struct alginfo *,
			 struct alginfo *, unsigned, unsigned char) = {
		{	/* NULL */
			pdcp_insert_cplane_null_op,	/* NULL */
			pdcp_insert_cplane_int_only_op,	/* SNOW f9 */
			pdcp_insert_cplane_int_only_op,	/* AES CMAC */
			pdcp_insert_cplane_int_only_op	/* ZUC-I */
		},
		{	/* SNOW f8 */
			pdcp_insert_cplane_enc_only_op,	/* NULL */
			pdcp_insert_cplane_acc_op,	/* SNOW f9 */
			pdcp_insert_cplane_snow_aes_op,	/* AES CMAC */
			pdcp_insert_cplane_snow_zuc_op	/* ZUC-I */
		},
		{	/* AES CTR */
			pdcp_insert_cplane_enc_only_op,	/* NULL */
			pdcp_insert_cplane_aes_snow_op,	/* SNOW f9 */
			pdcp_insert_cplane_acc_op,	/* AES CMAC */
			pdcp_insert_cplane_aes_zuc_op	/* ZUC-I */
		},
		{	/* ZUC-E */
			pdcp_insert_cplane_enc_only_op,	/* NULL */
			pdcp_insert_cplane_zuc_snow_op,	/* SNOW f9 */
			pdcp_insert_cplane_zuc_aes_op,	/* AES CMAC */
			pdcp_insert_cplane_acc_op	/* ZUC-I */
		},
	};
	static uint32_t
		desc_share[PDCP_CIPHER_TYPE_INVALID][PDCP_AUTH_TYPE_INVALID] = {
		{	/* NULL */
			SHR_WAIT,	/* NULL */
			SHR_ALWAYS,	/* SNOW f9 */
			SHR_ALWAYS,	/* AES CMAC */
			SHR_ALWAYS	/* ZUC-I */
		},
		{	/* SNOW f8 */
			SHR_ALWAYS,	/* NULL */
			SHR_ALWAYS,	/* SNOW f9 */
			SHR_WAIT,	/* AES CMAC */
			SHR_WAIT	/* ZUC-I */
		},
		{	/* AES CTR */
			SHR_ALWAYS,	/* NULL */
			SHR_ALWAYS,	/* SNOW f9 */
			SHR_ALWAYS,	/* AES CMAC */
			SHR_WAIT	/* ZUC-I */
		},
		{	/* ZUC-E */
			SHR_ALWAYS,	/* NULL */
			SHR_WAIT,	/* SNOW f9 */
			SHR_WAIT,	/* AES CMAC */
			SHR_ALWAYS	/* ZUC-I */
		},
	};
	enum pdb_type_e pdb_type;
	struct program prg;
	struct program *program = &prg;
	LABEL(pdb_end);

	if (rta_sec_era != RTA_SEC_ERA_2 && era_2_sw_hfn_override) {
		pr_debug("Cannot select SW HFN override for other era than 2");
		return;
	}

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();

	SHR_HDR(desc_share[cipherdata->algtype][authdata->algtype], 0, WITH(0));

	pdb_type = cnstr_pdcp_c_plane_pdb(program,
			hfn,
			bearer,
			direction,
			hfn_threshold,
			cipherdata,
			authdata);

	SET_LABEL(pdb_end);

	if (pdcp_insert_hfn_ov_op(program, PDCP_SN_SIZE_5, pdb_type,
				  era_2_sw_hfn_override))
		return;

	if (pdcp_cp_fp[cipherdata->algtype][authdata->algtype](program,
			cipherdata,
			authdata,
			OP_TYPE_DECAP_PROTOCOL,
			era_2_sw_hfn_override))
		return;

	PATCH_HDR(0, pdb_end);

	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                   Function for creating a PDCP User Plane
 *                            encapsulation descriptor.
 * @ingroup                   sharedesc_group
 *
 * @param [in,out] descbuf    Pointer to buffer for descriptor construction.
 *
 * @param [in,out] bufsize    Size of descriptor written. Once the function
 *                            returns, the value of this parameter can be used
 *                            for reclaiming the space that wasn't used for the
 *                            descriptor.
 *
 * @param [in] ps             If 36/40bit addressing is desired, this parameter
 *                            must be non-zero.
 *
 * @param [in] sn_size        Selects Sequence Number Size: 7/12/15 bits.
 *
 * @param [in] hfn            Starting Hyper Frame Number to be used together
 *                            with the SN from the PDCP frames.
 *
 * @param [in] bearer         Radio bearer ID.
 *
 * @param [in] direction      The direction of the PDCP frame (UL/DL).
 *
 * @param [in] hfn_threshold  HFN value that once reached triggers a warning
 *                            from SEC that keys should be renegociated at the
 *                            earliest convenience.
 *
 * @param [in] cipherdata     Pointer to block cipher transform definitions.
 *                            Valid algorithm values are those from
 *                            cipher_type_pdcp enum.
 *
 * @param [in] era_2_sw_hfn_override    If software HFN override mechanism is
 *                                      desired for this descriptor.
 *                                      @note Can only be used for SEC ERA 2
 *
 * @note  @b descbuf must be large enough to contain a full 256 byte long
 *        descriptor; after the function returns, by subtracting the actual
 *        number of bytes used (using @b bufsize), the user can reuse the
 *        remaining buffer space for other purposes.
 *
 */
static inline void cnstr_shdsc_pdcp_u_plane_encap(uint32_t *descbuf,
		unsigned *bufsize,
		unsigned short ps,
		enum pdcp_sn_size sn_size,
		uint32_t hfn,
		unsigned short bearer,
		unsigned short direction,
		uint32_t hfn_threshold,
		struct alginfo *cipherdata,
		unsigned char era_2_sw_hfn_override)
{
	struct program prg;
	struct program *program = &prg;
	struct pdcp_pdb pdb;
	LABEL(pdb_end);

	if (rta_sec_era != RTA_SEC_ERA_2 && era_2_sw_hfn_override) {
		pr_debug("Cannot select SW HFN override for other era than 2");
		return;
	}

	PROGRAM_CNTXT_INIT(descbuf, 0);

	if (ps)
		PROGRAM_SET_36BIT_ADDR();

	SHR_HDR(SHR_ALWAYS, 0, WITH(0));

	/* Read options from user */
	/* Depending on sequence number lenght, the HFN and HFN threshold
	 * have different lengths.
	 */
	memset(&pdb, 0x00, sizeof(struct pdcp_pdb));

	switch (sn_size) {
	case PDCP_SN_SIZE_7:
		pdb.opt_res.opt |= PDCP_U_PLANE_PDB_OPT_SHORT_SN;
		pdb.hfn_res = hfn << PDCP_U_PLANE_PDB_SHORT_SN_HFN_SHIFT;
		pdb.hfn_thr_res =
			hfn_threshold<<PDCP_U_PLANE_PDB_SHORT_SN_HFN_THR_SHIFT;
		break;

	case PDCP_SN_SIZE_12:
		pdb.opt_res.opt &= (uint32_t)(~PDCP_U_PLANE_PDB_OPT_SHORT_SN);
		pdb.hfn_res = hfn << PDCP_U_PLANE_PDB_LONG_SN_HFN_SHIFT;
		pdb.hfn_thr_res =
			hfn_threshold<<PDCP_U_PLANE_PDB_LONG_SN_HFN_THR_SHIFT;
		break;

	case PDCP_SN_SIZE_15:
		pdb.opt_res.opt &= (uint32_t)(~PDCP_U_PLANE_PDB_OPT_SHORT_SN);
		pdb.hfn_res = hfn << PDCP_U_PLANE_PDB_15BIT_SN_HFN_SHIFT;
		pdb.hfn_thr_res =
			hfn_threshold<<PDCP_U_PLANE_PDB_15BIT_SN_HFN_THR_SHIFT;
		break;

	default:
		pr_debug("Invalid Sequence Number Size setting in PDB\n");
		return;
	}

	pdb.bearer_dir_res = (uint32_t)
				((bearer << PDCP_U_PLANE_PDB_BEARER_SHIFT) |
				 (direction << PDCP_U_PLANE_PDB_DIR_SHIFT));

	/* copy PDB in descriptor*/
	ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct pdcp_pdb));

	SET_LABEL(pdb_end);

	if (pdcp_insert_hfn_ov_op(program,
				  sn_size,
				  PDCP_PDB_TYPE_FULL_PDB,
				  era_2_sw_hfn_override))
		return;

	switch (sn_size) {
	case PDCP_SN_SIZE_7:
	case PDCP_SN_SIZE_12:
		switch (cipherdata->algtype) {
		case PDCP_CIPHER_TYPE_ZUC:
			if (rta_sec_era < RTA_SEC_ERA_5) {
				pr_debug("Invalid era for selected algorithm\n");
				return;
			}
		case PDCP_CIPHER_TYPE_AES:
		case PDCP_CIPHER_TYPE_SNOW:
			/* Insert Cipher Key */
			KEY(KEY1, cipherdata->key_enc_flags,
			    PTR((uint64_t)cipherdata->key), cipherdata->keylen,
			    WITH(0));
			PROTOCOL(OP_TYPE_ENCAP_PROTOCOL,
				 OP_PCLID_LTE_PDCP_USER,
				 (uint16_t)cipherdata->algtype);
			break;
		case PDCP_CIPHER_TYPE_NULL:
			pdcp_insert_uplane_null_op(program,
						   cipherdata,
						   OP_TYPE_ENCAP_PROTOCOL);
			break;
		default:
			pr_debug("%s: Invalid encrypt algorithm selected: %d\n",
				 "cnstr_pcl_shdsc_pdcp_u_plane_decap",
				 cipherdata->algtype);
			return;
		}
		break;

	case PDCP_SN_SIZE_15:
		switch (cipherdata->algtype) {
		case PDCP_CIPHER_TYPE_NULL:
			pdcp_insert_uplane_null_op(program,
						   cipherdata,
						   OP_TYPE_ENCAP_PROTOCOL);
			break;

		default:
			if (pdcp_insert_uplane_15bit_op(program, cipherdata,
							OP_TYPE_ENCAP_PROTOCOL))
				return;
			break;
		}
		break;

	default:
		break;
	}

	PATCH_HDR(0, pdb_end);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                   Function for creating a PDCP User Plane
 *                            decapsulation descriptor.
 * @ingroup                   sharedesc_group
 *
 * @param [in,out] descbuf    Pointer to buffer for descriptor construction.
 *
 * @param [in,out] bufsize    Size of descriptor written. Once the function
 *                            returns, the value of this parameter can be used
 *                            for reclaiming the space that wasn't used for the
 *                            descriptor.
 *
 * @param [in] ps             If 36/40bit addressing is desired, this parameter
 *                            must be non-zero.
 *
 * @param [in] sn_size        Selects Sequence Number Size: 7/12/15 bits.
 *
 * @param [in] hfn            Starting Hyper Frame Number to be used together
 *                            with the SN from the PDCP frames.
 *
 * @param [in] bearer         Radio bearer ID.
 *
 * @param [in] direction      The direction of the PDCP frame (UL/DL).
 *
 * @param [in] hfn_threshold  HFN value that once reached triggers a warning
 *                            from SEC that keys should be renegociated at the
 *                            earliest convenience.
 *
 * @param [in] cipherdata     Pointer to block cipher transform definitions.
 *                            Valid algorithm values are those from
 *                            cipher_type_pdcp enum.
 *
 * @param [in] era_2_sw_hfn_override    If software HFN override mechanism is
 *                                      desired for this descriptor.
 *                                      @note Can only be used for SEC ERA 2
 *
 * @note  @b descbuf must be large enough to contain a full 256 byte long
 *        descriptor; after the function returns, by subtracting the actual
 *        number of bytes used (using @b bufsize), the user can reuse the
 *        remaining buffer space for other purposes.
 *
 */
static inline void cnstr_shdsc_pdcp_u_plane_decap(uint32_t *descbuf,
		unsigned *bufsize,
		unsigned short ps,
		enum pdcp_sn_size sn_size,
		uint32_t hfn,
		unsigned short bearer,
		unsigned short direction,
		uint32_t hfn_threshold,
		struct alginfo *cipherdata,
		unsigned char era_2_sw_hfn_override)
{
	struct program prg;
	struct program *program = &prg;
	struct pdcp_pdb pdb;
	LABEL(pdb_end);

	if (rta_sec_era != RTA_SEC_ERA_2 && era_2_sw_hfn_override) {
		pr_debug("Cannot select SW HFN override for other era than 2");
		return;
	}

	PROGRAM_CNTXT_INIT(descbuf, 0);

	if (ps)
		PROGRAM_SET_36BIT_ADDR();

	SHR_HDR(SHR_ALWAYS, 0, WITH(0));

	/* Read options from user */
	/* Depending on Sequence Number Size, the HFN and HFN threshold
	 * have different lengths.
	 */
	memset(&pdb, 0x00, sizeof(struct pdcp_pdb));

	switch (sn_size) {
	case PDCP_SN_SIZE_7:
		pdb.opt_res.opt |= PDCP_U_PLANE_PDB_OPT_SHORT_SN;
		pdb.hfn_res = hfn << PDCP_U_PLANE_PDB_SHORT_SN_HFN_SHIFT;
		pdb.hfn_thr_res =
			hfn_threshold<<PDCP_U_PLANE_PDB_SHORT_SN_HFN_THR_SHIFT;
		break;

	case PDCP_SN_SIZE_12:
		pdb.opt_res.opt &= (uint32_t)(~PDCP_U_PLANE_PDB_OPT_SHORT_SN);
		pdb.hfn_res = hfn << PDCP_U_PLANE_PDB_LONG_SN_HFN_SHIFT;
		pdb.hfn_thr_res =
			hfn_threshold<<PDCP_U_PLANE_PDB_LONG_SN_HFN_THR_SHIFT;
		break;

	case PDCP_SN_SIZE_15:
		pdb.opt_res.opt &= (uint32_t)(~PDCP_U_PLANE_PDB_OPT_SHORT_SN);
		pdb.hfn_res = hfn << PDCP_U_PLANE_PDB_15BIT_SN_HFN_SHIFT;
		pdb.hfn_thr_res =
			hfn_threshold<<PDCP_U_PLANE_PDB_15BIT_SN_HFN_THR_SHIFT;
		break;

	default:
		pr_debug("Invalid Sequence Number Size setting in PDB\n");
		return;
	}

	pdb.bearer_dir_res = (uint32_t)
				((bearer << PDCP_U_PLANE_PDB_BEARER_SHIFT) |
				 (direction << PDCP_U_PLANE_PDB_DIR_SHIFT));

	/* copy PDB in descriptor*/
	ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct pdcp_pdb));

	SET_LABEL(pdb_end);

	if (pdcp_insert_hfn_ov_op(program,
				  sn_size,
				  PDCP_PDB_TYPE_FULL_PDB,
				  era_2_sw_hfn_override))
		return;

	switch (sn_size) {
	case PDCP_SN_SIZE_7:
	case PDCP_SN_SIZE_12:
		switch (cipherdata->algtype) {
		case PDCP_CIPHER_TYPE_ZUC:
			if (rta_sec_era < RTA_SEC_ERA_5) {
				pr_debug("Invalid era for selected algorithm\n");
				return;
			}
		case PDCP_CIPHER_TYPE_AES:
		case PDCP_CIPHER_TYPE_SNOW:
			/* Insert Cipher Key */
			KEY(KEY1, cipherdata->key_enc_flags,
			    PTR(cipherdata->key), cipherdata->keylen, WITH(0));
			PROTOCOL(OP_TYPE_DECAP_PROTOCOL,
				 OP_PCLID_LTE_PDCP_USER,
				 (uint16_t)cipherdata->algtype);
			break;
		case PDCP_CIPHER_TYPE_NULL:
			pdcp_insert_uplane_null_op(program,
						   cipherdata,
						   OP_TYPE_DECAP_PROTOCOL);
			break;
		default:
			pr_debug("%s: Invalid encrypt algorithm selected: %d\n",
				 "cnstr_pcl_shdsc_pdcp_u_plane_decap",
				 cipherdata->algtype);
			return;
		}
		break;

	case PDCP_SN_SIZE_15:
		switch (cipherdata->algtype) {
		case PDCP_CIPHER_TYPE_NULL:
			pdcp_insert_uplane_null_op(program,
						   cipherdata,
						   OP_TYPE_DECAP_PROTOCOL);
			break;

		default:
			if (pdcp_insert_uplane_15bit_op(program, cipherdata,
							OP_TYPE_DECAP_PROTOCOL))
				return;
			break;
		}
		break;

	default:
		break;
	}

	PATCH_HDR(0, pdb_end);
	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                   Function for creating a PDCP Short MAC descriptor.
 * @ingroup                   sharedesc_group
 *
 * @param [in,out] descbuf    Pointer to buffer for descriptor construction.
 *
 * @param [in,out] bufsize    Size of descriptor written. Once the function
 *                            returns, the value of this parameter can be used
 *                            for reclaiming the space that wasn't used for the
 *                            descriptor.
 *
 * @param [in] ps             If 36/40bit addressing is desired, this parameter
 *                            must be non-zero.
 *
 * @param [in] authdata       Pointer to authentication transform definitions.
 *                            Valid algorithm values are those from
 *                            auth_type_pdcp enum.
 *
 * @param [in] era_2_sw_hfn_override    If software HFN override mechanism is
 *                                      desired for this descriptor.
 *                                      @note Can only be used for SEC ERA 2
 *
 * @note  @b descbuf must be large enough to contain a full 256 byte long
 *        descriptor; after the function returns, by subtracting the actual
 *        number of bytes used (using @b bufsize), the user can reuse the
 *        remaining buffer space for other purposes.
 *
 */
static inline void cnstr_shdsc_pdcp_short_mac(uint32_t *descbuf,
		unsigned *bufsize,
		unsigned short ps,
		struct alginfo *authdata)
{
	struct program prg;
	struct program *program = &prg;
	uint32_t iv[3] = {0, 0, 0};
	LABEL(local_offset);
	REFERENCE(move_cmd_read_descbuf);
	REFERENCE(move_cmd_write_descbuf);

	PROGRAM_CNTXT_INIT(descbuf, 0);

	if (ps)
		PROGRAM_SET_36BIT_ADDR();

	SHR_HDR(SHR_ALWAYS, 1, WITH(0));

	if (rta_sec_era > RTA_SEC_ERA_2) {
		MATHB(SEQINSZ, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));
		MATHB(SEQINSZ, SUB, ZERO, MATH1, SIZE(4), WITH(0));
	} else {
		MATHB(SEQINSZ, ADD, ONE, MATH1, SIZE(4), WITH(0));
		MATHB(MATH1, SUB, ONE, MATH1, SIZE(4), WITH(0));
		MATHB(ZERO, ADD, MATH1, VSEQINSZ, SIZE(4), WITH(0));
		MOVE(MATH1, 0, MATH0, 0, IMM(8), WITH(0));

		/*
		 * Since MOVELEN is available only starting with
		 * SEC ERA 3, use poor man's MOVELEN: create a MOVE
		 * command dynamically by writing the length from M1 by
		 * OR-ing the command in the M1 register and MOVE the
		 * result into the descriptor buffer. Care must be taken
		 * wrt. the location of the command because of SEC
		 * pipelining. The actual MOVEs are written at the end
		 * of the descriptor due to calculations needed on the
		 * offset in the descriptor for the MOVE command.
		 */
		move_cmd_read_descbuf = MOVE(DESCBUF, 0, MATH0, 0, IMM(6),
					     WITH(0));
		move_cmd_write_descbuf = MOVE(MATH0, 0, DESCBUF, 0, IMM(8),
					      WITH(WAITCOMP));
	}
	MATHB(ZERO, ADD, MATH1, VSEQOUTSZ, SIZE(4), WITH(0));

	switch (authdata->algtype) {
	case PDCP_AUTH_TYPE_NULL:
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		if (rta_sec_era > RTA_SEC_ERA_2) {
			MOVE(AB1, 0, OFIFO, 0, MATH1, WITH(0));
		} else {
			SET_LABEL(local_offset);

			/* Shut off automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0,
			     WITH(0));

			/* Placeholder for MOVE command with length from M1
			 * register
			 */
			MOVE(IFIFOAB1, 0, OFIFO, 0, IMM(0), WITH(0));

			/* Enable automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0,
			     WITH(0));
		}

		LOAD(PTR((uintptr_t)iv), MATH0, 0, SIZE(8), IMMED);
		SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | LAST2 | FLUSH1));
		SEQSTORE(MATH0, 0, 4, WITH(0));

		break;

	case PDCP_AUTH_TYPE_SNOW:
		iv[0] = 0xFFFFFFFF;
		iv[1] = 0x04000000;
		iv[2] = 0xF8000000;

		KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		LOAD(PTR((uintptr_t)&iv), CONTEXT2, 0, SIZE(12), IMMED);
		ALG_OPERATION(OP_ALG_ALGSEL_SNOW_F9,
			      OP_ALG_AAI_F9,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_ENCRYPT);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MOVE(AB1, 0, OFIFO, 0, MATH1, WITH(0));
		} else {
			SET_LABEL(local_offset);


			/* Shut off automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0,
			     WITH(0));

			/* Placeholder for MOVE command with length from M1
			 * register */
			MOVE(IFIFOAB1, 0, OFIFO, 0, IMM(0), WITH(0));

			/* Enable automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0,
			     WITH(0));
		}
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST1 | LAST2 | FLUSH1));
		SEQSTORE(CONTEXT2, 0, 4, WITH(0));

		break;

	case PDCP_AUTH_TYPE_AES:
		iv[0] = 0xFFFFFFFF;
		iv[1] = 0xFC000000;
		iv[2] = 0x00000000; /* unused */

		KEY(KEY1, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		LOAD(PTR((uintptr_t)&iv), MATH0, 0, 8, IMMED);
		MOVE(MATH0, 0, IFIFOAB1, 0, IMM(8), WITH(0));
		ALG_OPERATION(OP_ALG_ALGSEL_AES,
			      OP_ALG_AAI_CMAC,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_ENCRYPT);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

		if (rta_sec_era > RTA_SEC_ERA_2) {
			MOVE(AB2, 0, OFIFO, 0, MATH1, WITH(0));
		} else {
			SET_LABEL(local_offset);

			/* Shut off automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0,
			     WITH(0));

			/* Placeholder for MOVE command with length from M1
			 * register */
			MOVE(IFIFOAB2, 0, OFIFO, 0, IMM(0), WITH(0));

			/* Enable automatic Info FIFO entries */
			LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0,
			     WITH(0));
		}
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST1 | LAST2 | FLUSH1));
		SEQSTORE(CONTEXT1, 0, 4, WITH(0));

		break;

	case PDCP_AUTH_TYPE_ZUC:
		if (rta_sec_era < RTA_SEC_ERA_5) {
			pr_debug("Invalid era for selected algorithm\n");
			return;
		}
		iv[0] = 0xFFFFFFFF;
		iv[1] = 0xFC000000;
		iv[2] = 0x00000000; /* unused */

		KEY(KEY2, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, WITH(0));
		LOAD(PTR((uintptr_t)&iv), CONTEXT2, 0, 12, IMMED);
		ALG_OPERATION(OP_ALG_ALGSEL_ZUCA,
			      OP_ALG_AAI_F9,
			      OP_ALG_AS_INITFINAL,
			      ICV_CHECK_DISABLE,
			      OP_ALG_ENCRYPT);
		SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));
		MOVE(AB1, 0, OFIFO, 0, MATH1, WITH(0));
		SEQFIFOLOAD(MSGINSNOOP, 0, WITH(VLF | LAST1 | LAST2 | FLUSH1));
		SEQSTORE(CONTEXT2, 0, 4, WITH(0));

		break;

	default:
		pr_debug("%s: Invalid integrity algorithm selected: %d\n",
			 "cnstr_shdsc_pdcp_short_mac",
			 authdata->algtype);
		return;
	}


	if (rta_sec_era < RTA_SEC_ERA_3) {
		PATCH_MOVE(move_cmd_read_descbuf, local_offset);
		PATCH_MOVE(move_cmd_write_descbuf, local_offset);
	}

	*bufsize = PROGRAM_FINALIZE();
}

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
	ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct srtp_encap_pdb));
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
		ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct srtp_decap_pdb));
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

/**
 * @details                  IEEE 802.11i WiFi encapsulation
 *
 * @ingroup                  sharedesc_group
 *
 * @param[in, out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in, out] bufsize   Pointer to descriptor size, updated at completion.
 *
 * @param [in] ps            If 36/40bit addressing is desired, this parameter
 *      must be non-zero.
 * @param[in] mac_hdr_len    PDB MAC header length (24 or 28 bytes).
 * @param[in] pn             PDB Packet Number.
 * @param[in] priority       PDB Packet priority.
 * @param[in] key_id         PDB Key ID.
 * @param[in] cipherdata     Block cipher transform definitions.
 **/

static inline void cnstr_shdsc_wifi_encap(uint32_t *descbuf, unsigned *bufsize,
		unsigned short ps, uint16_t mac_hdr_len, uint64_t pn,
		uint8_t priority, uint8_t key_id, struct alginfo *cipherdata)
{
	struct program prg;
	struct program *program = &prg;
	struct wifi_encap_pdb pdb;

	LABEL(pdbend);
	REFERENCE(phdr);
	LABEL(keyjump);
	REFERENCE(pkeyjump);
	LABEL(startloop);
	REFERENCE(pstartloop);

	memset(&pdb, 0, sizeof(struct wifi_encap_pdb));
	pdb.mac_hdr_len = mac_hdr_len;
	pdb.b0_flags = WIFI_B0_FLAGS;
	pdb.pri = priority;
	pdb.pn1 = (uint16_t)high_32b(pn);
	pdb.pn2 = low_32b(pn);
	pdb.frm_ctrl_mask = WIFI_FRM_CTRL_MASK;
	pdb.seq_ctrl_mask = WIFI_SEQ_CTRL_MASK;
	pdb.key_id = key_id;
	pdb.ctr_flags = WIFI_CTR_FLAGS;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	phdr = SHR_HDR(SHR_SERIAL, pdbend, SC);
	ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct wifi_encap_pdb));
	SET_LABEL(pdbend);
	pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE, WITH(SHRD | SELF));
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(IMMED));
	SET_LABEL(keyjump);
	PROTOCOL(OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_WIFI, OP_PCL_WIFI);

	/* Errata A-005487: PN is written in reverse order in CCM Header. */
	if (rta_sec_era <= RTA_SEC_ERA_5) {
		/*
		 * Copy MAC Header len in MATH2 and (Const + KeyID) in MATH3
		 * to be used later in CCMP header reconstruction.
		 */
		MATHB(ZERO, AND, MATH3, MATH3, SIZE(8), WITH(IFB|NFU));
		MOVE(DESCBUF, 0, MATH2, 0, IMM(6), 0);
		MOVE(DESCBUF, 20, MATH3, 0, IMM(4), 0);

		/*
		 * Protocol operation leaves in MATH0 the incremented PN as
		 * xx xx PN0 PN1 PN2 PN3 PN4 PN5. Before byte-reversing the
		 * MATH0, first the PN is decremented and transformed as
		 * PN0 PN1 PN2 PN3 00 00 PN4 PN5.
		 */
		MATHB(MATH0, SUB, ONE, MATH0, SIZE(8), WITH(IFB|NFU));
		MATHB(ZERO, ADD, MATH0, MATH1, SIZE(2), WITH(IFB|NFU));
		MATHB(MATH0, XOR, MATH1, MATH0, SIZE(8), WITH(IFB|NFU));
		MATHB(MATH0, LSHIFT, IMM(16), MATH0, SIZE(8), WITH(IFB|NFU));
		MATHB(MATH1, OR, MATH0, MATH0, SIZE(8), WITH(IFB|NFU));

		/*
		 * Prepare to byte-reverse MATH0: copy MATH0 input into Input
		 * DATA FIFO and place loop value (8) into MATH1.
		 */
		LOAD(IMM(0), DCTRL, LDOFF_DISABLE_AUTO_NFIFO, 0, 0);
		MOVE(MATH0, 0, IFIFOAB1, 0, IMM(8), 0);
		LOAD(IMM(0), DCTRL, LDOFF_ENABLE_AUTO_NFIFO, 0, 0);

		MOVE(IFIFOABD, 0, MATH0, 0, IMM(1), 0);
		MATHB(ZERO, ADD, IMM(8), MATH1, SIZE(4), WITH(IFB|NFU));
		MATHB(MATH1, SUB, ONE, VSEQINSZ, SIZE(4), WITH(IFB|NFU));

		/*
		 * loop to reverse MATH0 content from PN0 PN1 PN2 PN3 00 00
		 * PN4 PN5 --> PN5 PN4 00 00 PN3 PN2 PN1 PN0; during n-th
		 * iteration, right shift MATH0 with 8 bytes and copy into
		 * MATH0 at offset 0 the n-th byte from Input Data FIFO.
		 */
		SET_LABEL(startloop);
		MATHB(VSEQINSZ, SUB, ONE, VSEQINSZ, SIZE(4), WITH(IFB));
		MATHB(MATH0, RSHIFT, MATH1, MATH0, SIZE(8), WITH(IFB|NFU));
		MOVE(IFIFOABD, 0, MATH0, 0, IMM(1), 0);
		JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, 0);
		pstartloop = JUMP(IMM(startloop), LOCAL_JUMP, ALL_FALSE,
				  MATH_Z);

		/*
		 * Prepare to write the correct CCMP header to output frame.
		 * Copy MAC Header Len into VSOL and complete CCMP header in
		 * MATH3 with Const + keyID.
		 */
		MATHB(MATH2, RSHIFT, IMM(16), VSEQOUTSZ, SIZE(2),
		      WITH(IFB|NFU));
		MATHB(MATH0, OR, MATH3, MATH3, SIZE(8), WITH(IFB|NFU));

		/*
		 * write the correct CCMP header from MATH3 to output frame.
		 * set length for Ouput Sequence operation at 48B, larger than
		 * largest MAC header length(30) + CCMP header length(8)
		 */
		SEQOUTPTR(0, 48, WITH(RTO));
		SEQFIFOSTORE(SKIP, 0, 0, WITH(VLF));
		SEQSTORE(MATH3, 0, SIZE(8), 0);
	}
	PATCH_HDR(phdr, pdbend);
	PATCH_JUMP(pkeyjump, keyjump);
	PATCH_JUMP(pstartloop, startloop);

	*bufsize = PROGRAM_FINALIZE();
}

/**
 * @details                  IEEE 802.11 WiFi decapsulation
 *
 * @ingroup                  sharedesc_group
 *
 * @param[in, out] descbuf   Pointer to descriptor-under-construction buffer.
 * @param[in, out] bufsize   Pointer to descriptor size, updated at completion.
 *
 * @param [in] ps            If 36/40bit addressing is desired, this parameter
 *      must be non-zero.
 * @param[in] mac_hdr_len    PDB MAC header length (24 or 28 bytes).
 * @param[in] pn             PDB Packet Number.
 * @param[in] priority       PDB Packet priority.
 * @param[in] cipherdata     Block cipher transform definitions.
 **/
static inline void cnstr_shdsc_wifi_decap(uint32_t *descbuf, unsigned *bufsize,
		unsigned short ps, uint16_t mac_hdr_len, uint64_t pn,
		uint8_t priority, struct alginfo *cipherdata)
{
	struct program prg;
	struct program *program = &prg;
	struct wifi_decap_pdb pdb;

	LABEL(phdr);
	REFERENCE(pdbend);
	LABEL(keyjump);
	REFERENCE(pkeyjump);

	memset(&pdb, 0x00, sizeof(struct wifi_encap_pdb));
	pdb.mac_hdr_len = mac_hdr_len;
	pdb.b0_flags = WIFI_B0_FLAGS;
	pdb.pri = priority;
	pdb.pn1 = (uint16_t)high_32b(pn);
	pdb.pn2 = low_32b(pn);
	pdb.frm_ctrl_mask = WIFI_FRM_CTRL_MASK;
	pdb.seq_ctrl_mask = WIFI_SEQ_CTRL_MASK;
	pdb.ctr_flags = WIFI_CTR_FLAGS;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	phdr = SHR_HDR(SHR_SERIAL, pdbend, SC);
	ENDIAN_DATA((uint8_t *)&pdb, sizeof(struct wifi_decap_pdb));
	SET_LABEL(pdbend);
	pkeyjump = JUMP(IMM(keyjump), LOCAL_JUMP, ALL_TRUE, WITH(SHRD | SELF));
	KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
	    cipherdata->keylen, WITH(IMMED));
	SET_LABEL(keyjump);
	PROTOCOL(OP_TYPE_DECAP_PROTOCOL, OP_PCLID_WIFI, OP_PCL_WIFI);

	PATCH_HDR(phdr, pdbend);
	PATCH_JUMP(pkeyjump, keyjump);
	*bufsize = PROGRAM_FINALIZE();
}

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
	ENDIAN_DATA(pdb, pdb_len);
	SET_LABEL(pdbend);
	PROTOCOL(protcmd->optype, protcmd->protid, protcmd->protinfo);
	PATCH_HDR(phdr, pdbend);
	*bufsize = PROGRAM_FINALIZE();
}

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
	ENDIAN_DATA(pdb, pdb_len);
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

static inline void cnstr_shdsc_mbms_type0(uint32_t *descbuf,
					  unsigned *bufsize,
					  unsigned short ps)
{
	struct program prg;
	struct program *program = &prg;
	struct mbms_type_0_pdb pdb;

	LABEL(pdb_end);
	LABEL(end_of_sd);
	LABEL(seq_in_ptr);
	LABEL(rto);
	LABEL(crc_pass);
	LABEL(keyjmp);
	REFERENCE(jump_write_crc);
	REFERENCE(phdr);
	REFERENCE(seq_in_address);
	REFERENCE(patch_load);
	REFERENCE(pkeyjmp);
	REFERENCE(load_start_of_buf);
	REFERENCE(read_rto);
	REFERENCE(write_rto);

	memset(&pdb, 0, sizeof(pdb));
	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	phdr = SHR_HDR(SHR_SERIAL, 0, 0);
	ENDIAN_DATA((uint8_t *)&pdb, sizeof(pdb));
	SET_LABEL(pdb_end);

	/*
	 * Read the pointer to data from JD. The last byte is ignored. This
	 * is done for reading the IC & implicitly the PR portion of the IC.
	 */
	seq_in_address = MOVE(DESCBUF, 0, MATH0, 0, IMM(7), 0);
	patch_load = MOVE(MATH0, 0, DESCBUF, 0, IMM(7), 0);

	/*
	 * Next, do some stuff since the above commands overwrite the
	 * descriptor buffer and due to pipelining, it's possible that
	 * the modifications aren't taken into consideration.
	 */

	/*
	 * Set Non-SEQ LIODN equal to SEQ LIODN. This is needed for
	 * transferring data that is in the input buffer by the (non-SEQ) LOAD
	 * command below
	 */
	LOAD(IMM(0), DCTRL, LDOFF_CHG_NONSEQLIODN_SEQ, 0, WITH(0));

	pkeyjmp = JUMP(IMM(keyjmp), LOCAL_JUMP, ALL_TRUE, SHRD|SELF);

	/* Load the polynomial to KEY2 register */
	KEY(KEY2, 0, IMM(MBMS_HEADER_POLY), 1, WITH(IMMED));

	SET_LABEL(keyjmp);

	ALG_OPERATION(OP_ALG_ALGSEL_CRC,
		      OP_ALG_AAI_CUST_POLY |
		      OP_ALG_AAI_DIS | OP_ALG_AAI_DOS | OP_ALG_AAI_DOC,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE,
		      OP_ALG_ENCRYPT);

	/* Put UDP offset in least significant byte of M1 */
	load_start_of_buf = LOAD(PTR(DUMMY_BUF_BASE | BUF_L4_OFFSET), MATH1, 7,
				 SIZE(1), 0);
	load_start_of_buf++;

	/* Restore LIODN */
	LOAD(IMM(0), DCTRL, LDOFF_CHG_NONSEQLIODN_NON_SEQ, 0, WITH(0));

	/* Wait for transfer to end */
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, CALM);

	/* Calculate offset to MBMS SYNC header offset from start of frame */
	MATHB(MATH1, ADD, IMM(MBMS_HDR_OFFSET), VSEQINSZ, SIZE(4), WITH(0));

	/*
	 * Put the full input length in M1, used below to patch the rereading
	 * of the frame
	 */
	MATHB(VSEQINSZ, ADD, IMM(MBMS_TYPE0_HDR_LEN), MATH1, SIZE(4),
	      WITH(0));

	/* Calculate length of output frame to be stored (if CRC passes) */
	MATHB(MATH1, SUB, ZERO, VSEQOUTSZ, SIZE(4), WITH(0));

	/* SKIP all headers */
	SEQFIFOLOAD(SKIP, 0, WITH(VLF));

	/* Read the MBMS header, minus the CRC */
	SEQFIFOLOAD(MSG2,
		    MBMS_TYPE0_HDR_LEN - 1,
		    WITH(LAST2));

	/* READ CRC in MSB of M2 */
	SEQLOAD(MATH2, 0, 1, WITH(0));

	/* Restore VSIL before mangling MATH1 below */
	MATHB(MATH1, ADD, ZERO, VSEQINSZ, SIZE(4), WITH(0));

	/*
	 * Patch the SEQINPTR RTO command below to revert the frame input
	 * to the beginning.
	 * Note: One can remove these commands and use a large value for the
	 * length in SEQINPTR RTO.
	 */
	read_rto = MOVE(DESCBUF, 0, MATH1, 0, IMM(6), 0);
	write_rto = MOVE(MATH1, 0, DESCBUF, 0, IMM(8), 0);

	/*
	 * Wait here for CRCA to finish processing AND for the external transfer
	 * of the CRC to finish before proceeding in comparing the CRC
	 */
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM));

	/* Put in the MSB of M3 the CRC as calculated by CRCA */
	MOVE(CONTEXT2, 0, MATH3, 0, IMM(1), WITH(WAITCOMP));

	/* Do Frame_CRC XOR Calculated_CRC */
	MATHB(MATH2, XOR, MATH3, NONE, SIZE(8), WITH(0));

	/*
	 * If the last math operation sets the zero flag, it means the two CRCs
	 * match and the descriptor can start copying things into the OFIFO and
	 * subsequently write them to external memory.
	 */
	jump_write_crc = JUMP(IMM(0), LOCAL_JUMP, ALL_TRUE, WITH(MATH_Z));

	/*
	 * If here, then the two CRCs are different. Thus, the descriptor
	 * will increment the failed CRCs count and then halt execution with
	 * the status indicating the Header CRC failed.
	 */

	/*
	 * Read the first two words of the descriptor into M0 (the 2nd word
	 * contains the statistic to be incremented
	 */
	MOVE(DESCBUF, 0, MATH0, 0, IMM(8), 0);

	/*
	 * Increment the read statistic with 1, while not mangling the header
	 * of the descriptor
	 */
	MATHB(MATH0, ADD, ONE, MATH0, SIZE(8), WITH(0));

	/* Write back the modifications in the descriptor buffer */
	MOVE(MATH0, 0, DESCBUF, 0, IMM(8), WAITCOMP);

	/* Store the updated statistic in external memory */
	STORE(SHAREDESCBUF_EFF, 4, IMM(0xDEADC0DE), 4, WITH(0));

	SET_LABEL(rto);

	/* Halt here with the appropriate status */
	JUMP(IMM(MBMS_CRC_HDR_FAIL), HALT_STATUS, ALL_FALSE, WITH(0));

	/*
	 * If here, all is fine, so prepare the frame-copying. First revert
	 * the input frame
	 */
	SET_LABEL(crc_pass);
	SEQINPTR(0, 0, WITH(RTO));

	/* Store everything */
	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

	/*
	 * Move M1 bytes from IFIFO to OFIFO
	 * Note: Only bits 16:31 of M1 are used, so the fact that it's mangled
	 *       because of the RTO patching above is not relevant.
	 */
	MOVE(AB1, 0, OFIFO, 0, MATH1, WITH(0));

	/* Read all frame */
	SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

	SET_LABEL(end_of_sd);
	seq_in_ptr = end_of_sd + 8;

	PATCH_MOVE(seq_in_address, seq_in_ptr);
	PATCH_MOVE(patch_load, load_start_of_buf);
	PATCH_JUMP(pkeyjmp, keyjmp);
	PATCH_MOVE(read_rto, rto);
	PATCH_MOVE(write_rto, rto);
	PATCH_JUMP(jump_write_crc, crc_pass);

	PATCH_HDR(phdr, pdb_end);

	*bufsize = PROGRAM_FINALIZE();
}

static inline unsigned cnstr_shdsc_mbms_type1_3(uint32_t *descbuf,
						unsigned *bufsize,
						unsigned short ps,
						enum mbms_pdu_type pdu_type)
{
	struct program part1_prg;
	struct program *program = &part1_prg;
	struct mbms_type_1_3_pdb pdb;

	LABEL(pdb_end);
	LABEL(end_of_sd);
	LABEL(seq_in_ptr);
	LABEL(sd_ptr);
	LABEL(hdr_crc_pass);
	LABEL(all_crc_pass);
	REFERENCE(jump_chk_payload_crc);
	REFERENCE(jump_start_of_desc);
	REFERENCE(load_2nd_part);
	REFERENCE(jump_all_crc_ok);
	REFERENCE(phdr);
	REFERENCE(seq_in_address);

	REFERENCE(move_sd_address);
	REFERENCE(patch_load_2nd_part);

	REFERENCE(patch_load);
	REFERENCE(load_start_of_buf);

	memset(&pdb, 0, sizeof(pdb));
	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();

	phdr = SHR_HDR(SHR_SERIAL, 0, 0);
	ENDIAN_DATA((uint8_t *)&pdb, sizeof(pdb));
	SET_LABEL(pdb_end);

	/*
	 * Read the pointer to data from JD. The last byte is ignored. This
	 * is done for reading the IC & implicitly the PR portion of the IC.
	 */
	seq_in_address = MOVE(DESCBUF, 0, MATH0, 0, IMM(7), 0);
	patch_load = MOVE(MATH0, 0, DESCBUF, 0, IMM(7), 0);

	/*
	 * Next, do some stuff since the above commands overwrite the
	 * descriptor buffer and due to pipelining, it's possible that
	 * the modifications aren't taken into consideration.
	 */

	/*
	 * Set Non-SEQ LIODN equal to SEQ LIODN. This is needed for
	 * transferring data that is in the input buffer by the (non-SEQ) LOAD
	 * command below
	 */
	LOAD(IMM(0), DCTRL, LDOFF_CHG_NONSEQLIODN_SEQ, 0, WITH(0));

	/*
	 * Note: The assumption here is that the base adress where the preheader
	 * and the descriptor are allocated is 256B aligned.
	 */
	move_sd_address = MOVE(DESCBUF, 0, MATH2, 0, IMM(7), 0);
	patch_load_2nd_part = MOVE(MATH2, 0, DESCBUF, 0, IMM(7), WITH(0));

	/*
	 * This descriptor overwrites itself ("overlay methodology").
	 * The descriptor buffer is contiguous and the descriptor will
	 * bring from external memory into descriptor buffer the supplementary
	 * data which cannot fit in the descriptor buffer. In order to do that,
	 * the descriptor reads (LOAD) at SD_PTR + 51W (13W is the max
	 * JD size) from JD and brings data back into the descriptor buffer.
	 * The following instructions take care of patching the first before
	 * last command that can be pushed in the current descriptor buffer
	 */

	/* Load the header polynomial to KEY2 register */
	KEY(KEY2, 0, IMM(MBMS_HEADER_POLY), 1, WITH(IMMED));

	ALG_OPERATION(OP_ALG_ALGSEL_CRC,
		      OP_ALG_AAI_CUST_POLY |
		      OP_ALG_AAI_DIS | OP_ALG_AAI_DOS | OP_ALG_AAI_DOC,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE,
		      OP_ALG_ENCRYPT);

	/* Put UDP offset in least significant byte of M1 */
	load_start_of_buf = LOAD(PTR(DUMMY_BUF_BASE | BUF_L4_OFFSET), MATH1, 7,
				 SIZE(1), 0);
	load_start_of_buf++;

	/* Restore LIODN */
	LOAD(IMM(0), DCTRL, LDOFF_CHG_NONSEQLIODN_NON_SEQ, 0, WITH(0));

	/* Wait for transfer to end */
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, CALM);

	/* Calculate offset to MBMS SYNC header offset from start of frame */
	MATHB(MATH1, ADD, IMM(MBMS_HDR_OFFSET), VSEQINSZ, SIZE(4), WITH(0));

	/* Put full frame length into M0 */
	MATHB(SEQINSZ, SUB, ZERO, MATH0, SIZE(4), WITH(0));

	/* M1 will contain the offset to MBMS payload */
	if (pdu_type == MBMS_PDU_TYPE1)
		MATHB(VSEQINSZ, ADD, IMM(MBMS_TYPE1_HDR_LEN), MATH1, SIZE(4),
		      WITH(0));
	else
		MATHB(VSEQINSZ, ADD, IMM(MBMS_TYPE3_HDR_LEN), MATH1, SIZE(4),
		      WITH(0));

	/*
	 * Save frame length and MBMS Header Offset (all frame data to be
	 * skipped into Context1
	 */
	MOVE(MATH0, 0, CONTEXT1, 0, IMM(16), WITH(0));

	/* SKIP all headers */
	SEQFIFOLOAD(SKIP, 0, WITH(VLF));

	/* Read the MBMS header, minus the CRC */
	if (pdu_type == MBMS_PDU_TYPE1)
		SEQFIFOLOAD(MSG2,
			    MBMS_TYPE1_HDR_LEN - 2,
			    WITH(LAST2));
	else
		SEQFIFOLOAD(MSG2,
			    MBMS_TYPE3_HDR_LEN - 2,
			    WITH(LAST2));

	/* READ Header CRC and Payload CRC and save it in ... */
	SEQLOAD(MATH3, 0, SIZE(2), WITH(0));

	/*
	 * Wait here for CRCA to finish processing AND for the external transfer
	 * of the CRC to finish before proceeding in comparing the CRC
	 */
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, WITH(CALM | CLASS2));

	/* Clear the payload CRC */
	MATHB(MATH3, AND, IMM(0xFC00000000000000), MATH2, SIZE(8), 0);

	/* Put in M3 the payload CRC */
	MATHB(MATH3, XOR, MATH2, MATH3, SIZE(8), WITH(STL));

	/*
	 * Align the payload CRC properly (so it can be compared easily with
	 * the calculated CRC.
	 */
	MATHB(MATH3, LSHIFT, IMM(6), MATH3, SIZE(8), WITH(IFB));

	/* Save header & payload CRC for future checking and/or updating */
	MOVE(MATH2, 0, CONTEXT1, 16, IMM(16), WITH(0));

	/* Put in the MSB of M3 the header CRC as calculated by CRCA */
	MOVE(CONTEXT2, 0, MATH3, 0, IMM(8), WITH(WAITCOMP));

	/* Do Frame_CRC XOR Calculated_CRC */
	MATHB(MATH2, XOR, MATH3, NONE, SIZE(8), WITH(0));

	/*
	 * If the last math operation sets the zero flag, it means the two CRCs
	 * match and the descriptor can start copying things into the OFIFO and
	 * subsequently write them to external memory.
	 */
	jump_chk_payload_crc = JUMP(IMM(0), LOCAL_JUMP, ALL_TRUE, WITH(MATH_Z));

	/*
	 * If here, then the two CRCs are different. Thus, the descriptor
	 * will increment the failed CRCs count and then halt execution with
	 * the status indicating the Header CRC failed.
	 */

	/*
	 * Read the first two words of the descriptor into M0 (the 2nd word
	 * contains the statistic to be incremented
	 */
	MOVE(DESCBUF, 0, MATH0, 0, IMM(8), 0);

	/*
	 * Increment the read statistic with 1, while not mangling the header
	 * of the descriptor
	 */
	MATHB(MATH0, ADD, ONE, MATH0, SIZE(8), WITH(0));

	/* Write back the modifications in the descriptor buffer */
	MOVE(MATH0, 0, DESCBUF, 0, IMM(8), WAITCOMP);

	/* Store the updated statistic in external memory */
	STORE(SHAREDESCBUF_EFF, 4, IMM(0xDEADC0DE), 4, WITH(0));

	/* Halt here with the appropriate status */
	JUMP(IMM(MBMS_CRC_HDR_FAIL), HALT_STATUS, ALL_FALSE, WITH(0));

	/*
	 * If here, header is OK. Payload must be checked next
	 */
	SET_LABEL(hdr_crc_pass);

	/* Reset C2 related stuff */
	LOAD(IMM(LDST_SRCDST_WORD_CLRW |
		 CLRW_CLR_C2MODE |
		 CLRW_CLR_C2DATAS |
		 CLRW_CLR_C2CTX |
		 CLRW_CLR_C2KEY |
		 CLRW_RESET_CLS2_CHA),
		 CLRW, 0, 4, WITH(0));

	/*
	 * Set VSIL so that the length to be read is:
	 * original SIL - MBMS Hdr Offset - MBMS Header Length
	 */
	MATHB(MATH0, SUB, MATH1, VSEQINSZ, SIZE(4), WITH(0));

	/*
	 * Insert the overlaying procedure here. This is quite simple:
	 * - the LOAD command below was patched in the beginning
	 *   so that it reads from the SD_PTR + (total descriptor length -
	 *   <HERE>) and puts into the descriptor buffer AFTER the PDB
	 * - sharing is disabled (because the descriptor buffer needs to be
	 *   re-fetched
	 * - a jump back is done so the execution resumes after the PDB
	 */
	LOAD(IMM(0), DCTRL, LDOFF_CHG_SHARE_NEVER, 0, WITH(0));

	/*
	 * Note1: For now, RTA doesn't support to update the length of the LOAD
	 * So the length is hardcoded. If the descriptor get modified, this
	 * will have to be updated.
	 *
	 * Note2: The "+8" below is due to the preheader that is before the SD
	 */
	load_2nd_part = LOAD(PTR(DUMMY_BUF_BASE + 8 +
			     (program->current_pc + 4) * 4), DESCBUF, 0,
			     SIZE(136), 0);
	load_2nd_part++;

	jump_start_of_desc = JUMP(IMM(0), LOCAL_JUMP, ALL_TRUE, WITH(CALM));

	/*
	 * HERE ENDS THE FIRST PART OF THE DESCRIPTOR. ALL INSTRUCTIONS
	 * FOLLOWING THIS POINT ARE EXECUTED IN THE SECOND HALF OF THE
	 * DESCRIPTOR THAT HAS JUST BEEN TRANSFERED ABOVE
	 *
	 * Note: because of the above, all labels pointing to JD must be set
	 *       here
	 */
	SET_LABEL(end_of_sd);
	seq_in_ptr = end_of_sd + 8;
	sd_ptr = end_of_sd + 1;

	/* Load the payload polynomial to KEY2 register */
	KEY(KEY2, 0, IMM(MBMS_PAYLOAD_POLY), 2, WITH(IMMED));

	/* Request the CRC engine */
	ALG_OPERATION(OP_ALG_ALGSEL_CRC,
		      OP_ALG_AAI_CUST_POLY |
		      OP_ALG_AAI_DIS | OP_ALG_AAI_DOS | OP_ALG_AAI_DOC,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE,
		      OP_ALG_ENCRYPT);

	/* Get the payload CRC, saved previously */
	MOVE(CONTEXT1, 24, MATH2, 0, IMM(8), WITH(0));

	/* Read the payload data */
	SEQFIFOLOAD(MSG2, 0, WITH(LAST2 | VLF));

	/* Get the calculated CRC */
	MOVE(CONTEXT2, 0, MATH3, 0, IMM(8), WITH(WAITCOMP));

	/* Check if the two CRCs match */
	MATHB(MATH3, XOR, MATH2, NONE, SIZE(8), WITH(0));

	jump_all_crc_ok = JUMP(IMM(0), LOCAL_JUMP, ALL_TRUE, WITH(MATH_Z));

	/*
	 * If here, then the two CRCs are different. Thus, the descriptor
	 * will increment the failed payload CRCs count, copy the frame
	 * up to and including the MBMS Header, minus the Payload CRC which
	 * is going to be updated with the calculated CRC. Then the execution
	 * will be halted with the status indicating the Payload CRC failed.
	 */

	/* Revert the frame back to the beginning */
	SEQINPTR(0, 9600, WITH(RTO));

	/* Bytes to copy = MAC/VLAN/IP/UDP/GTP/MBMS minus CRC (2B) */
	MATHB(MATH1, SUB, IMM(2), MATH1, SIZE(4), WITH(0));
	MATHB(MATH1, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));

	/* Prepare the CRC Hdr to be written */
	MOVE(CONTEXT1, 16, MATH2, 0, IMM(8), WITH(0));

	/* Align the Calculated Payload CRC to be written properly */
	MATHB(MATH3, RSHIFT, IMM(6), MATH3, SIZE(8), WITH(IFB));

	/* Bytes to write in output memory =  MAC/VLAN/IP/UDP/GTP/MBMS */
	MATHB(MATH1, SUB, ZERO, VSEQOUTSZ, SIZE(4), WITH(0));

	/* Initiate writing to external memory */
	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

	/* Read everything w/o the CRCs */
	SEQFIFOLOAD(MSG1, 0, WITH(LAST1 | FLUSH1 | VLF));

	/* Move M1 bytes from IFIFO to OFIFO */
	MOVE(AB1, 0, OFIFO, 0, MATH1, WITH(0));

	/* Add the calculated payload CRC to the header CRC */
	MATHB(MATH2, OR, MATH3, MATH2, SIZE(8), WITH(0));

	/* Now store the updated CRCs to the output frame */
	SEQSTORE(MATH2, 0, SIZE(2), 0);

	/*
	 * Read the 2nd two words of the descriptor into M0 (the 3rd word
	 * contains the statistic to be incremented
	 */
	MOVE(DESCBUF, 4, MATH0, 0, IMM(8), 0);

	/*
	 * Increment the read statistic with 1, while not mangling the failed
	 * CRC header statistics
	 */
	MATHB(MATH0, ADD, ONE, MATH0, SIZE(8), WITH(0));

	/* Write back the modifications in the descriptor buffer */
	MOVE(MATH0, 4, DESCBUF, 0, IMM(8), WAITCOMP);

	/* Store the updated statistic in external memory */
	STORE(SHAREDESCBUF_EFF, 8, IMM(0xDEADC0DE), 4, WITH(0));

	/*
	 * Halt here with the appropriate status, but wait first for data
	 * to reach the memory
	 */
	JUMP(IMM(MBMS_CRC_PAYLOAD_FAIL), HALT_STATUS, ALL_TRUE, WITH(CALM));

	SET_LABEL(all_crc_pass);
	/* If here, both the header CRC and the payload CRC are correct */

	/* Revert the frame back to beginning */
	SEQINPTR(0, 9600, WITH(RTO));

	/* Bytes to read = MAC/VLAN/IP/UDP/GTP/MBMS + Payload*/
	MATHB(MATH0, SUB, ZERO, VSEQINSZ, SIZE(4), WITH(0));

	/* Bytes to write = bytes to read */
	MATHB(VSEQINSZ, SUB, ZERO, VSEQOUTSZ, SIZE(4), WITH(0));

	/* Store everything */
	SEQFIFOSTORE(MSG, 0, 0, WITH(VLF));

	/* Read all frame */
	SEQFIFOLOAD(MSG1, 0, WITH(VLF | LAST1 | FLUSH1));

	/* Move M1 bytes from IFIFO to OFIFO */
	MOVE(AB1, 0, OFIFO, 0, MATH0, WITH(0));

	/*
	 * Halt with 0 (i.e. no error).
	 * This is needed because the descriptor is overlayed, and otherwise
	 * the DECO will continue executing stuff that is leftover from the
	 * original descriptor buffer.
	 */
	JUMP(IMM(0x00), HALT_STATUS, ALL_TRUE, WITH(CALM));

	PATCH_MOVE(seq_in_address, seq_in_ptr);
	PATCH_MOVE(patch_load, load_start_of_buf);
	PATCH_MOVE(move_sd_address, sd_ptr);
	PATCH_MOVE(patch_load_2nd_part, load_2nd_part);
	PATCH_JUMP(jump_chk_payload_crc, hdr_crc_pass);
	PATCH_JUMP(jump_all_crc_ok, all_crc_pass);
	PATCH_JUMP(jump_start_of_desc, pdb_end);
	PATCH_LOAD(load_2nd_part - 1, pdb_end);

	PATCH_HDR(phdr, pdb_end);

	*bufsize = PROGRAM_FINALIZE();

	/*
	 * This is a temporary workaround till RTA has support for
	 * patching the length of the SD.
	 */
	*program->shrhdr &= (uint32_t)(~0x7F);
	*program->shrhdr |= end_of_sd;

	return end_of_sd;
}

/**
 * @details  MBMS PDU CRC checking descriptor.
 *
 * @ingroup sharedesc_group
 *
 * @param[in,out] descbuf   Pointer to buffer used for descriptor construction.
 * @param[in,out] bufsize   Pointer to descriptor size to be written back upon
 *                          completion.
 * @param[in] ps            If 36/40bit addressing is desired, this parameter
 *                          must be non-zero.
 * @param[in] preheader_len Length to be set in the corresponding preheader
 *                          field. Unless the descriptor is split in multiple
 *                          parts, this will be equal to bufsize.
 *                          This structure will be copied inline to the
 *                          descriptor under construction. No error checking
 *                          will be made. Refer to the block guide for details
 *                          of the PDB.
 * @param[in] pdu_type      Type of the MBMS PDU required to be processed
 *                          by this descriptor.
 **/
static inline void cnstr_shdsc_mbms(uint32_t *descbuf,
				    unsigned *bufsize,
				    unsigned short ps,
				    unsigned *preheader_len,
				    enum mbms_pdu_type pdu_type)
{
	switch (pdu_type) {
	case MBMS_PDU_TYPE0:
		cnstr_shdsc_mbms_type0(descbuf, bufsize, ps);
		*preheader_len = *bufsize;
		break;

	case MBMS_PDU_TYPE1:
		*preheader_len = cnstr_shdsc_mbms_type1_3(descbuf, bufsize, ps,
							  MBMS_PDU_TYPE1);
		break;

	case MBMS_PDU_TYPE3:
		*preheader_len = cnstr_shdsc_mbms_type1_3(descbuf, bufsize, ps,
							  MBMS_PDU_TYPE3);
		break;

	default:
		pr_debug("Invalid MBMS PDU Type selected %d\n", pdu_type);
		return;
	}
}

#endif /* __RTA_PROTOSHARED_H__ */
