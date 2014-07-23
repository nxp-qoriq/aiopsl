
#ifndef __DESC_MACSEC_H__
#define __DESC_MACSEC_H__

#include "flib/rta.h"
#include "common.h"

/**
 * @file                 macsec.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       MACsec Shared Descriptor Constructors
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
 * @enum      cipher_type_macsec macsec.h
 * @details   Type selectors for cipher types in MACSEC protocol.
 */
enum cipher_type_macsec {
	MACSEC_CIPHER_TYPE_GCM,
	MACSEC_CIPHER_TYPE_GMAC
};

/** @} */ /* end of defines_group */

/*
 * IEEE 801.AE MacSEC Protocol Data Block
 */
#define MACSEC_PDBOPTS_FCS	0x01
#define MACSEC_PDBOPTS_AR	0x40	/* used in decap only */

struct macsec_encap_pdb {
	uint16_t aad_len;
	uint8_t rsvd;
	uint8_t options;
	uint32_t sci_hi;
	uint32_t sci_lo;
	uint16_t ethertype;
	uint8_t tci_an;
	uint8_t rsvd1;
	/* begin DECO writeback region */
	uint32_t pn;
	/* end DECO writeback region */
};

struct macsec_decap_pdb {
	uint16_t aad_len;
	uint8_t rsvd;
	uint8_t options;
	uint32_t sci_hi;
	uint32_t sci_lo;
	uint8_t rsvd1[3];
	/* begin DECO writeback region */
	uint8_t antireplay_len;
	uint32_t pn;
	uint32_t antireplay_scorecard_hi;
	uint32_t antireplay_scorecard_lo;
	/* end DECO writeback region */
};

/**
 * @defgroup sharedesc_group Shared Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of sharedesc_group */

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
		pr_err("MACsec GMAC available only for Era 5 or above\n");

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
		COPY_DATA((uint8_t *)&pdb, sizeof(struct macsec_encap_pdb));
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
		pr_err("MACsec GMAC available only for Era 5 or above\n");

	memset(&pdb, 0x00, sizeof(struct macsec_decap_pdb));
	pdb.sci_hi = high_32b(sci);
	pdb.sci_lo = low_32b(sci);
	pdb.pn = pn;

	startidx = sizeof(struct macsec_decap_pdb) >> 2;

	PROGRAM_CNTXT_INIT(descbuf, 0);
	SHR_HDR(SHR_SERIAL, ++startidx, WITH(SC));
	{
		COPY_DATA((uint8_t *)&pdb, sizeof(struct macsec_decap_pdb));
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

#endif /* __DESC_MACSEC_H__ */
