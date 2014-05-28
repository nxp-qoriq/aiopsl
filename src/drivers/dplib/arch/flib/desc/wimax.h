/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_WIMAX_H__
#define __DESC_WIMAX_H__

#include "flib/rta.h"
#include "common.h"

/**
 * @file                 wimax.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       WiMAX Shared Descriptor Constructors
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
 * @def WIMAX_PDBOPTS_FCS
 * Options Byte with FCS enabled.
 */
#define WIMAX_PDBOPTS_FCS		0x01

/**
 * @def WIMAX_PDBOPTS_AR
 * Options Byte with AR enabled.
 */
#define WIMAX_PDBOPTS_AR		0x40

/** @} */ /* end of defines_group */

/*
 * IEEE 802.16 WiMAX Protocol Data Block
 */

#define WIMAX_PDB_B0            0x19    /* Initial Block B0 Flags */
#define WIMAX_PDB_CTR           0x01    /* Counter Block Flags */

struct wimax_encap_pdb {
	uint8_t rsvd[3];                /* Reserved Bits */
	uint8_t options;                /* Options Byte */
	uint32_t nonce;                 /* Nonce Constant */
	uint8_t b0_flags;               /* Initial Block B0 */
	uint8_t ctr_flags;              /* Counter Block Flags */
	uint16_t ctr_init_count;
	/* begin DECO writeback region */
	uint32_t pn;                    /* Packet Number */
	/* end DECO writeback region */
};

struct wimax_decap_pdb {
	uint8_t rsvd[3];                /* Reserved Bits */
	uint8_t options;                /* Options Byte */
	uint32_t nonce;                 /* Nonce Constant */
	uint8_t iv_flags;               /* Initialization Vector Flags */
	uint8_t ctr_flags;              /* Counter Block Flags */
	uint16_t ctr_init_count;
	/* begin DECO writeback region */
	uint32_t pn;                    /* Packet Number */
	uint8_t rsvd1[2];               /* Reserved Bits */
	uint16_t antireplay_len;
	uint32_t antireplay_scorecard_hi;
	uint32_t antireplay_scorecard_lo;
	/** end DECO writeback region */
};

/**
 * @defgroup sharedesc_group Shared Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of sharedesc_group */

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
		COPY_DATA((uint8_t *)&pdb, sizeof(struct wimax_encap_pdb));
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
		COPY_DATA((uint8_t *)&pdb, sizeof(struct wimax_decap_pdb));
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

#endif /* __DESC_WIMAX_H__ */
