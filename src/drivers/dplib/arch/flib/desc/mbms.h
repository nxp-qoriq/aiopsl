/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_MBMS_H__
#define __DESC_MBMS_H__

#include "flib/rta.h"
#include "common.h"

/**
 * @file                 mbms.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       MBMS Shared Descriptor Constructors
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
 * @def HDR_PAYLOAD_MASK
 * Mask to be used for extracting only the header CRC from the corresponding
 * field in the MBMS Type 1 & 3 PDUs SYNC headers
 */
#define HDR_CRC_MASK		0xFC00000000000000ll
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
 * @struct    mbms_type_0_pdb mbms.h
 * @details   Container for MBMS Type 0 PDB
 */
struct mbms_type_0_pdb {
	uint32_t crc_header_fail;
};

/**
 * @struct    mbms_type_1_3_pdb mbms.h
 * @details   Container for MBMS Type 1 and Type 3 PDB
 */
struct mbms_type_1_3_pdb {
	uint32_t crc_header_fail;
	uint32_t crc_payload_fail;
};

/**
 * @enum      mbms_pdu_type mbms.h
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
	COPY_DATA((uint8_t *)&pdb, sizeof(pdb));
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
	MOVE(DESCBUF, 0, MATH0, 0, IMM(8), WITH(WAITCOMP));

	/*
	 * Increment the read statistic with 1, while not mangling the header
	 * of the descriptor
	 */
	MATHB(MATH0, ADD, ONE, MATH0, SIZE(8), WITH(0));

	/* Write back the modifications in the descriptor buffer */
	MOVE(MATH0, 0, DESCBUF, 0, IMM(8), WITH(WAITCOMP));

	/* Store the updated statistic in external memory */
	STORE(SHAREDESCBUF_EFF, 4, IMM(DUMMY_BUF_BASE), 4, WITH(0));

	SET_LABEL(rto);

	/* Halt here with the appropriate status */
	JUMP(IMM(MBMS_CRC_HDR_FAIL), HALT_STATUS, ALL_FALSE, WITH(CALM));

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
	struct program part1_prg, part2_prg;
	struct program *program = &part1_prg;
	struct mbms_type_1_3_pdb pdb;
	uint32_t *part1_buf, *part2_buf;

	LABEL(pdb_end);
	LABEL(end_of_sd);
	LABEL(seq_in_ptr);
	LABEL(sd_ptr);
	LABEL(keyjmp);
	LABEL(hdr_crc_pass);
	LABEL(load_2nd_part);
	LABEL(all_crc_pass);
	LABEL(end_of_part2);
	REFERENCE(jump_chk_payload_crc);
	REFERENCE(jump_start_of_desc);
	REFERENCE(patch_load_2nd_part);
	REFERENCE(jump_all_crc_ok);
	REFERENCE(phdr);
	REFERENCE(seq_in_address);

	REFERENCE(move_sd_address);
	REFERENCE(patch_move_load_2nd_part);

	REFERENCE(patch_load);
	REFERENCE(pkeyjmp);
	REFERENCE(load_start_of_buf);

	part1_buf = descbuf;

	memset(&pdb, 0, sizeof(pdb));
	PROGRAM_CNTXT_INIT(part1_buf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();

	phdr = SHR_HDR(SHR_SERIAL, 0, 0);
	COPY_DATA((uint8_t *)&pdb, sizeof(pdb));
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
	patch_move_load_2nd_part = MOVE(MATH2, 0, DESCBUF, 0, IMM(7), WITH(0));

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
	pkeyjmp = JUMP(IMM(keyjmp), LOCAL_JUMP, ALL_TRUE, WITH(SHRD|SELF));

	/* Load the header polynomial to KEY2 register */
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
	MATHB(MATH3, AND, IMM(HDR_CRC_MASK), MATH2, SIZE(8), 0);

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
	MOVE(DESCBUF, 0, MATH0, 0, IMM(8), WITH(WAITCOMP));

	/*
	 * Increment the read statistic with 1, while not mangling the header
	 * of the descriptor
	 */
	MATHB(MATH0, ADD, ONE, MATH0, SIZE(8), WITH(0));

	/* Write back the modifications in the descriptor buffer */
	MOVE(MATH0, 0, DESCBUF, 0, IMM(8), WITH(WAITCOMP));

	/* Store the updated statistic in external memory */
	STORE(SHAREDESCBUF_EFF, 4, IMM(DUMMY_BUF_BASE), 4, WITH(0));

	/* Halt here with the appropriate status */
	JUMP(IMM(MBMS_CRC_HDR_FAIL), HALT_STATUS, ALL_TRUE, WITH(CALM));

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
	SET_LABEL(load_2nd_part);
	patch_load_2nd_part = LOAD(PTR(DUMMY_BUF_BASE), DESCBUF, 0, SIZE(8), 0);

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

	PATCH_MOVE(seq_in_address, seq_in_ptr);
	PATCH_JUMP(pkeyjmp, keyjmp);
	PATCH_MOVE(patch_load, load_start_of_buf);
	PATCH_MOVE(move_sd_address, sd_ptr);
	/*
	 * +1 here is needed because the PTR field (2WORDs) in the LOAD
	 * command needs to be updated by the MOVE command, not the LOAD command
	 * itself.
	 */
	PATCH_MOVE(patch_move_load_2nd_part, load_2nd_part + 1);
	PATCH_JUMP(jump_chk_payload_crc, hdr_crc_pass);
	PATCH_JUMP(jump_start_of_desc, pdb_end);
	PATCH_LOAD(patch_load_2nd_part, pdb_end);

	/*
	 * This patches the pointer in the load command so that it points after
	 * the "first part" SD.
	 * Note1: The +2 in the REFERENCE is needed because the least
	 *        significant byte in the PTR field of the LOAD command (the
	 *        offset from the base) needs to be updated; this resides 2
	 *        WORDS from the actual LOAD command
	 * Note2: The "+8" below is due to the preheader that is before the SD
	 */
	PATCH_RAW(patch_load_2nd_part + 2, 0xFF, end_of_sd * 4 + 8);

	PATCH_HDR(phdr, pdb_end);

	*bufsize = PROGRAM_FINALIZE();

	/* Here goes the 2nd part of the descriptor, as a separate program */
	program = &part2_prg;

	/*
	 * Start to write instructions in descriptor buffer after the
	 * instructions in the first program
	 */
	part2_buf = part1_buf + end_of_sd;

	/*
	 * The offset is set to the end of the PDB because the 2nd part of the
	 * descriptor is brought after the PDB in the SD (overwriting is done
	 * after the PDB).
	 */
	PROGRAM_CNTXT_INIT(part2_buf, pdb_end);

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
	MOVE(DESCBUF, 4, MATH0, 0, IMM(8), WITH(WAITCOMP));

	/*
	 * Increment the read statistic with 1, while not mangling the failed
	 * CRC header statistics
	 */
	MATHB(MATH0, ADD, ONE, MATH0, SIZE(8), WITH(0));

	/* Write back the modifications in the descriptor buffer */
	MOVE(MATH0, 0, DESCBUF, 4, IMM(8), WITH(WAITCOMP));

	/* Store the updated statistic in external memory */
	STORE(SHAREDESCBUF, 8, IMM(DUMMY_BUF_BASE), 4, WITH(0));

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

	SET_LABEL(end_of_part2);

	PATCH_JUMP(jump_all_crc_ok, all_crc_pass);
	PATCH_RAW_NON_LOCAL(&part1_prg, patch_load_2nd_part, 0xFF,
			    end_of_part2);

	*bufsize += PROGRAM_FINALIZE();

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
 *
 * @note This function can be called only for SEC ERA >= 5.
 *
 **/
static inline void cnstr_shdsc_mbms(uint32_t *descbuf,
				    unsigned *bufsize,
				    unsigned short ps,
				    unsigned *preheader_len,
				    enum mbms_pdu_type pdu_type)
{
	if (rta_sec_era < RTA_SEC_ERA_5) {
		*bufsize = 0;
		pr_err("MBMS protocol processing is available only for SEC ERA >= 5\n");
		return;
	}

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
		pr_err("Invalid MBMS PDU Type selected %d\n", pdu_type);
		return;
	}
}

/**
 * @defgroup helper_group Shared Descriptor Helper Routines
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of helper_group */

/**
 * @details              Helper function for retrieving MBMS descriptor
 *                       statistics.
 * @ingroup              helper_group
 *
 * @param [in] descbuf   Pointer to descriptor buffer, previously populated
 *                       by the cnstr_shdsc_mbms() function.
 * @param [in,out] stats Points to a statistics structure matching the MBMS
 *                       PDU type, as specified by the @pdu_type parameter.
 * @param [in] pdu_type  MBMS PDU type.
 *
 */
static inline void get_mbms_stats(uint32_t *descbuf,
				  void *stats,
				  enum mbms_pdu_type pdu_type)
{
	uint32_t *pdb_ptr;

	/*
	 * The structure of the MBMS descriptor is the following:
	 * HEADER (1W)
	 * Header CRC failed (1W)
	 * Payload CRC failed (1W, valid only for MBMS Type 1 and Type 3)
	 */
	pdb_ptr = descbuf + 1;

	switch (pdu_type) {
	case MBMS_PDU_TYPE0:
		memcpy(stats, pdb_ptr, sizeof(struct mbms_type_0_pdb));
		break;

	case MBMS_PDU_TYPE1:
	case MBMS_PDU_TYPE3:
		memcpy(stats, pdb_ptr, sizeof(struct mbms_type_1_3_pdb));
		break;

	default:
		pr_err("Invalid MBMS PDU Type selected %d\n", pdu_type);
		break;
	}
}

#endif /* __DESC_MBMS_H__ */
