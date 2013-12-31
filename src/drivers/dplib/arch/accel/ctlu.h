/**************************************************************************//**
@File		ctlu.h

@Description	This file contains the AIOP SW CTLU API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __CTLU_H
#define __CTLU_H

#include "general.h"
#include "dplib/fsl_ctlu.h"

/**************************************************************************//**
@Group	CTLU CTLU (Internal)

@Description	AIOP CTLU API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	CTLU_MACROS CTLU Macros

@Description	AIOP CTLU Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	CTLU_RULE_RESULT_CONSTANTS CTLU Table Rule Result Constants
@{
*//***************************************************************************/
/** Rule Result Size Constant */
#define CTLU_FCV_RESULT_SIZE 20

/** Uses for masking of opaque1 & opaque2 valid bits*/
#define CTLU_TLUR_OPAQUE_VALID_BITS_MASK	0x01FF

/** Uses for masking of Table ID, Key ID valid bit*/
#define CTLU_TLUR_TKIDV_BIT_MASK		0x02000000

/** @} */ /* end of AIOP_CTLU_RULE_RESULT_CONSTANTS */

/**************************************************************************//**
@Group	CTLU_RULE_ENTRY_MACROS CTLU Rule-Entry related Macros
@{
*//***************************************************************************/
/** Max size of LPM IPv4 rule's key including the exact match field.
 * including prefix length. */
#define CTLU_MAX_LPM_EM4_IPV4_KEY_SIZE				9

/** Max size of LPM IPv6 rule's key including the exact match field.
 * including prefix length. */
#define CTLU_MAX_LPM_EM4_IPV6_KEY_SIZE				21

/** The key size available for a rule that occupies a single entry */
#define CTLU_SINGLE_ENTRY_RULE_KEY_SIZE				24

/** Key size in entry that is the last one in a chain of entries */
#define CTLU_MULTIPLE_ENTRY_RULE_LAST_ENTRY_KEY_SIZE		16

/** Key size in entry that isn't the last one in a chain of entries */
#define CTLU_MULTIPLE_ENTRY_RULE_NON_LAST_ENTRY_KEY_SIZE	36

/** Number of entries needed for IPv4 LPM rule in best scenario */
#define CTLU_LPM_IPV4_BC_ENTRIES_PER_RULE			2

/** Number of entries needed for IPv4 LPM rule in worst case */
#define CTLU_LPM_IPV4_WC_ENTRIES_PER_RULE			5

/** Number of entries needed for IPv6 LPM rule in best scenario */
#define CTLU_LPM_IPV6_BC_ENTRIES_PER_RULE			2

/** Number of entries needed for IPv4 LPM rule in worst case */
#define CTLU_LPM_IPV6_WC_ENTRIES_PER_RULE			9

/** @} */ /* end of CTLU_RULE_ENTRY_MACROS */


/**************************************************************************//**
@Group	CTLU_MTYPE CTLU Message Types
@{
*//***************************************************************************/
	/** Table create message type */
#define CTLU_TABLE_CREATE_MTYPE				0x004C
	/** Table Parameters replace message type */
#define CTLU_TABLE_PARAMETERS_REPLACE_MTYPE		0x0046
	/** Table query without reference counting message type */
#define CTLU_TABLE_QUERY_MTYPE				0x0043
	/** Table query with reference counting message type */
#define CTLU_TABLE_QUERY_REFERENCE_INC_MTYPE		0x0047
	/** Table delete message type */
#define CTLU_TABLE_DELETE_MTYPE				0x0042
	/** Table rule create */
#define CTLU_RULE_CREATE_MTYPE				0x006C
	/** Table rule create or replace */
#define CTLU_RULE_CREATE_OR_REPLACE_MTYPE		0x006D
	/** Table rule replace */
#define CTLU_RULE_REPLACE_MTYPE				0x0066
	/** Table rule delete */
#define CTLU_RULE_DELETE_MTYPE				0x0062

	/** Table rule query
	With timestamp and reference counter update */
#define CTLU_RULE_QUERY_TMSTMP_RPTR_MTYPE		0x0063
	/** Table rule query
	With timestamp update, without reference counter update */
#define CTLU_RULE_QUERY_TMSTMP_MTYPE			0x0067
	/** Table rule query
	Without timestamp update, with reference counter update */
#define CTLU_RULE_QUERY_RPTR_MTYPE			0x0073
	/** Table rule query
	Without timestamp update, without reference counter update */
#define CTLU_RULE_QUERY_MTYPE				0x0077

	/** Table lookup with keyID
	With timestamp and reference counter update */
#define CTLU_LOOKUP_KEYID_TMSTMP_RPTR_MTYPE		0x0060
	/** Table lookup with keyID
	With timestamp update, without reference counter update */
#define CTLU_LOOKUP_KEYID_TMSTMP_MTYPE			0x0064
	/** Table lookup with keyID
	Without timestamp update, with reference counter update */
#define CTLU_LOOKUP_KEYID_RPTR_MTYPE			0x0070
	/** Table lookup with keyID
	Without timestamp update, without reference counter update */
#define CTLU_LOOKUP_KEYID_MTYPE				0x0074

	/** Table lookup with keyID and explicit parse result.
	With timestamp and reference counter update */
#define CTLU_LOOKUP_KEYID_EPRS_TMSTMP_RPTR_MTYPE	0x0160
	/** Table lookup with keyID and explicit parse result.
	With timestamp update, without reference counter update */
#define CTLU_LOOKUP_KEYID_EPRS_TMSTMP_MTYPE		0x0164
	/** Table lookup with keyID and explicit parse result.
	Without timestamp update, with reference counter update */
#define CTLU_LOOKUP_KEYID_EPRS_RPTR_MTYPE		0x0170
	/** Table lookup with keyID and explicit parse result.
	Without timestamp update, without reference counter update */
#define CTLU_LOOKUP_KEYID_EPRS_MTYPE			0x0174

	/** Table lookup with explicit key
	With timestamp and reference counter update. Single search only. */
#define CTLU_LOOKUP_KEY_TMSTMP_RPTR_MTYPE		0x0069
	/** Table lookup with explicit key
	With timestamp update, without reference counter update. Single search
	only. */
#define CTLU_LOOKUP_KEY_TMSTMP_MTYPE			0x006D
	/** Table lookup with explicit key
	Without timestamp update, with reference counter update. Single search
	only. */
#define CTLU_LOOKUP_KEY_RPTR_MTYPE			0x0079
	/** Table lookup with explicit key
	Without timestamp update, without reference counter update. Single
	search only. */
#define CTLU_LOOKUP_KEY_MTYPE				0x007D

	/** Key composition rule create or replace */
#define CTLU_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE	0x003D
	/** Key composition rule query */
#define CTLU_KEY_COMPOSITION_RULE_QUERY_MTYPE			0x0037
	/** Key generate */
#define CTLU_KEY_GENERATE_MTYPE					0x0034
	/** Key generate with explicit parse result */
#define CTLU_KEY_GENERATE_EPRS_MTYPE				0x0035
	/** Generate hash (Parses the frame) */
#define CTLU_HASH_GEN_MTYPE					0x0095
	/** Generate hash from explicit parse result*/
#define CTLU_HASH_GEN_EPRS_MTYPE				0x0195
	/** Generate hash from explicit key */
#define CTLU_HASH_GEN_KEY_MTYPE					0x009D

/** @} */ /* end of CTLU_MTYPE */

/**************************************************************************//**
@Group	CTLU_TBL_CREATE CTLU Table Create specific constants
@{
*//***************************************************************************/
	/** Table create input message reserved space */
#define CTLU_TABLE_CREATE_INPUT_MESSAGE_RESERVED_SPACE	120

	/** BDI mask in table create ICID field */
#define CTLU_TABLE_CREATE_INPUT_MESSAGE_ICID_BDI_MASK	0x8000

/** @} */ /* end of CTLU_TBL_CREATE */

/**************************************************************************//**
@Group	TLU_KCR CTLU KCR Builder defines
@{
*//***************************************************************************/

	/** Number of FEC's (first byte in the KCR) */
#define	CTLU_KCR_NFEC			0
	/** OP0 HET Protocol Based Extraction */
#define CTLU_KCR_OP0_HET_PROTOCOL	0x00
	/** OP0 HET Generic Extraction */
#define CTLU_KCR_OP0_HET_GEC		0x80
	/** Protocol based Extraction Header validation */
#define CTLU_KCR_PROTOCOL_HVT		0x40
	/** Mask extension exists */
#define CTLU_KCR_MASK_EXT		0x01

	/** Generic Extraction max. extract size */
#define CTLU_KCR_MAX_EXTRACT_SIZE	0xF
	/** Protocol Based Generic Extraction max. offset in Parse Result */
#define CTLU_KCR_PROTOCOL_MAX_OFFSET	0x3F
	/** Generic Extraction max. extract offset */
#define CTLU_KCR_MAX_EXTRACT_OFFET	0xF
	/** 16 Bytes alignment */
#define CTLU_KCR_16_BYTES_ALIGNMENT	0xF0
	/** Offset within 16 bytes */
#define CTLU_KCR_OFFSET_WITHIN_16_BYTES	0x0F


	/** Maximum KCR size */
#define CTLU_KCR_MAX_KCR_SIZE				64
	/** Constant FEC size */
#define CTLU_KCR_CONST_FEC_SIZE				3
	/** Protocol Specific FEC size (not including mask) */
#define CTLU_KCR_PROTOCOL_SPECIFIC_FEC_SIZE		1
	/** Protocol Based Generic FEC size (not including mask) */
#define CTLU_KCR_PROTOCOL_BASED_GENERIC_FEC_SIZE	4
	/** Generic FEC size (not including mask) */
#define CTLU_KCR_GENERIC_FEC_SIZE			4
	/** Lookup Result FEC size (not including mask) */
#define CTLU_KCR_LOOKUP_RES_FEC_SIZE			4
	/** Valid Field FEC size (not including mask) */
#define CTLU_KCR_VALID_FIELD_FEC_SIZE			2


	/** Generic Extraction offset 0x00 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x00		0x00
	/** Generic Extraction offset 0x10 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x10		0x01
	/** Generic Extraction offset 0x20 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x20		0x02
	/** Generic Extraction offset 0x30 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x30		0x03
	/** Generic Extraction offset 0x40 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x40		0x04
	/** Generic Extraction offset 0x50 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x50		0x05
	/** Generic Extraction offset 0x60 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x60		0x06
	/** Generic Extraction offset 0x70 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x70		0x07
	/** Generic Extraction offset 0x80 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x80		0x08
	/** Generic Extraction offset 0x90 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0x90		0x09
	/** Generic Extraction offset 0xA0 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0xA0		0x0A
	/** Generic Extraction offset 0xB0 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0xB0		0x0B
	/** Generic Extraction offset 0xC0 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0xC0		0x0C
	/** Generic Extraction offset 0xD0 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0xD0		0x0D
	/** Generic Extraction offset 0xE0 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0xE0		0x0E
	/** Generic Extraction offset 0xF0 from start of frame */
#define CTLU_KCR_EOM_FRAME_OFFSET_0xF0		0x0F
	/** Generic Extraction offset 0x00 from start of frame */
#define CTLU_KCR_EOM_PARSE_RES_OFFSET_0x00	0x10
	/** Generic Extraction offset 0x10 from Parse Result */
#define CTLU_KCR_EOM_PARSE_RES_OFFSET_0x10	0x11
	/** Generic Extraction offset 0x20 from Parse Result */
#define CTLU_KCR_EOM_PARSE_RES_OFFSET_0x20	0x12
	/** Generic Extraction offset 0x30 from Parse Result */
#define CTLU_KCR_EOM_PARSE_RES_OFFSET_0x30	0x13
	/** Todo Generic Extraction offset 0x00 from FCV */
#define CTLU_KCR_EOM_FCV_OFFSET_0x00		0x14
	/** Todo Generic Extraction offset 0x10 from FCV */
#define CTLU_KCR_EOM_FCV_OFFSET_0x10		0x15


	/** Todo Opaque0 Field offset from \ref CTLU_KCR_EOM_FCV_OFFSET_0x00 */
#define CTLU_KCR_EXT_OPAQUE0_OFFSET	0x00
	/** Todo Opaque1 Field offset from \ref CTLU_KCR_EOM_FCV_OFFSET_0x00 */
#define CTLU_KCR_EXT_OPAQUE1_OFFSET	0x08
	/** Todo Opaque2 Field offset from \ref CTLU_KCR_EOM_FCV_OFFSET_0x10 */
#define CTLU_KCR_EXT_OPAQUE2_OFFSET	0x00
	/** Unique ID Field offset from \ref CTLU_KCR_EOM_FCV_OFFSET_0x10 */
#define CTLU_KCR_EXT_UNIQUE_ID_OFFSET	0x08
	/** Timestamp Field offset from \ref CTLU_KCR_EOM_FCV_OFFSET_0x10 */
#define CTLU_KCR_EXT_TIMESTAMP_OFFSET	0x0C

	/** Opaque2 Field size to extract (= real size - 1) */
#define CTLU_KCR_EXT_OPAQUE2_SIZE	0x00
	/** Unique ID Field size to extract (= real size - 1) */
#define CTLU_KCR_EXT_UNIQUE_ID_SIZE	0x03
	/** Timestamp Field size to extract (= real size - 1) */
#define CTLU_KCR_EXT_TIMESTAMP_SIZE	0x03

/** @} */ /* end of CTLU_KCR */


/**************************************************************************//**
@Group	CTLU_TBL_PARAMS_REPLACE CTLU Table Parameters Replace specific \
	 constants
@{
*//***************************************************************************/
	/** Table parameter replace input message reserved space */
#define TABLE_PARAMS_REPLACE_INPUT_MESSAGE_RESERVED_SPACE	44

/** @} */ /* end of CTLU_TBL_PARAMS_REPLACE */

/** @} */ /* end of CTLU_MACROS */


/**************************************************************************//**
@Group		CTLU_STRUCTS CTLU Structures

@Description	AIOP CTLU Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Table Create Input Message Structure

		Pointed by input pointer.

*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_table_create_input_message {
	/** Table Type
	Includes IEX, MRES & AGT bits */
	uint16_t type;

	/** ICID (including BDI) */
	uint16_t icid;

	/** Max Rules */
	uint32_t max_rules;

	/** Max Entries */
	uint32_t max_entries;

	/** Committed Entries */
	uint32_t committed_entries;

	/** Committed Rules */
	uint32_t committed_rules;

	/** Reserved */
	uint8_t  reserved[CTLU_TABLE_CREATE_INPUT_MESSAGE_RESERVED_SPACE];

	/** Miss Result */
	struct ctlu_table_rule_result miss_lookup_fcv;

};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Parameters Replace Input Message Structure

		Pointed by input pointer.

*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_table_params_replace_input_message {
	/** Reserved */
	uint8_t reserved[TABLE_PARAMS_REPLACE_INPUT_MESSAGE_RESERVED_SPACE];

	/** Miss Result */
	struct ctlu_table_rule_result miss_lookup_fcv;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Create Output Message Structure

		Pointed by output pointer.

*//***************************************************************************/
#pragma pack(push, 1)
struct	ctlu_table_create_output_message {
	/** Table ID */
	uint16_t  tid;

	/** Reserved */
	uint16_t  reserved;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Parameters Query Output Message Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_table_params_query_output_message {
	/** Table Type
	Includes IEX, MRES & AGT bits */
	uint16_t type;

	/* ICID (including BDI) */
	uint16_t icid;

	/** Max Rules */
	uint32_t max_rules;

	/** Max Entries */
	uint32_t max_entries;

	/** Committed Entries */
	uint32_t committed_entries;

	/** Committed Rules */
	uint32_t committed_rules;

	/** Current Number of rules */
	uint32_t current_rules;

	/** Current Number of entries */
	uint32_t current_entries;

	/** Reserved */
	uint64_t  reserved0;

	/** Reserved */
	uint64_t  reserved1;

	/** Miss Result */
	struct ctlu_table_rule_result miss_lookup_fcv;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	CTLU Accelerator Context Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_acc_context {
	/** IPFID + Reserved */
	uint16_t	flags_and_reserved;

	/** Message type */
	uint16_t	mtype;

	/** Input Pointer */
	uint16_t	input_pointer;

	/** Output Pointer */
	uint16_t	output_pointer;

	/** Message type In */
	uint64_t	mtypein;

	/** Status + Flags */
	uint32_t	status_and_flags;

	/** Message type Out */
	uint32_t	mtypeout;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	CTLU KCR Builder FEC Mask
*//***************************************************************************/
#pragma pack(push, 1)
struct	ctlu_hw_fec_mask{
	/** Number of mask bytes & Mask offset 0.
	First Nibble: Number of mask bytes, determines the size of the FEC mask
	extension.
	Second Nibble: Mask offset 0. */
	uint8_t  nmsk_moff0;

	/** Mask0.
	Bit-wise-mask which is applied to the extracted header at offset MOFF0
	from its beginning */
	uint8_t  mask0;

	/** Mask offset 1 & Mask offset 2.
	First Nibble: Mask offset 1.
	Second Nibble: Mask offset 2. */
	uint8_t  moff1_moff2;

	/** Mask 1.
	Bit-wise-mask which is applied to the extracted header at offset MOFF1
	from its beginning */
	uint8_t  mask1;

	/** Mask 2.
	Bit-wise-mask which is applied to the extracted header at offset MOFF2
	from its beginning */
	uint8_t  mask2;

	/** Mask offset 3.
	First Nibble: Reserved.
	Second Nibble: Mask offset 3. */
	uint8_t  res_moff3;

	/** Mask 3.
	Bit-wise-mask which is applied to the extracted header at offset MOFF3
	from its beginning */
	uint8_t  mask3;
};
#pragma pack(pop)

/** @} */ /* end of CTLU_STRUCTS */

/**************************************************************************//**
@Group		CTLU_Functions CTLU Functions

@Description	AIOP CTLU Functions

@{
*//***************************************************************************/
/*
@Function	ctlu_kcr_builder_build_fec_mask - OBSOLETE

@Description	This function builds the fec's mask structure as the HW expects.

@Param[in]	mask - The user's mask array.
@Param[out]	fec_mask - The fec's mask structure for the HW.
@Param[out]	mask_bytes - number of fec's mask bytes.

@Return		None.
*/
/*inline void ctlu_kcr_builder_build_fec_mask(
	struct ctlu_kcr_builder_fec_mask *mask,
	struct ctlu_hw_fec_mask *fec_mask, uint8_t *mask_bytes)
{
	/* build fec_mask
	fec_mask->nmsk_moff0 = ((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
	fec_mask->mask0 = mask->single_mask[0].mask;
	fec_mask->moff1_moff2 = (mask->single_mask[1].mask_offset<<4) |
					mask->single_mask[2].mask_offset;
	fec_mask->mask1 = mask->single_mask[1].mask;
	fec_mask->mask2 = mask->single_mask[2].mask;
	fec_mask->res_moff3 = mask->single_mask[3].mask_offset;
	fec_mask->mask3 = mask->single_mask[3].mask;

	*mask_bytes = ((mask->num_of_masks == 1) ? 2 :
			(mask->num_of_masks == 2) ? 4 :
			(mask->num_of_masks == 3) ? 5 : 7);
}*/

/**************************************************************************//**
@Function	ctlu_table_query_debug

@Description	Return all table parameters as kept in CTLU HW.

@Param[in]	table_id - Table ID.

@Param[in]	output - All table parameter CTLU HW keeps.

@Return		Please refer to \ref FSL_CTLU_STATUS_TBL_GET_PARAMS

@Cautions	This function performs a task switch.
		This function does not increment the reference count for the
		miss result returned.
*//***************************************************************************/
int32_t ctlu_table_query_debug(uint16_t table_id,
			       struct ctlu_table_params_query_output_message
			       *output);

/** @} */ /* end of CTLU_Functions */

/** @} */ /* end of CTLU */

#endif /* __CTLU_H */
