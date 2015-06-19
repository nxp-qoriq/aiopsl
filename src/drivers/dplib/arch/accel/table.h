/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

/**************************************************************************//**
@File		table.h

@Description	This file contains the AIOP SW Table API

@note		This file is intended for internal use only (it may not be 
		included elsewhere).
*//***************************************************************************/

#ifndef __TABLE_H
#define __TABLE_H

#include "general.h"
#include "dplib/fsl_table.h"

/**************************************************************************//**
@Group	TABLE Table (Internal)

@Description	AIOP Table API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	TABLE_MACROS Table Macros

@Description	AIOP Table Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	TABLE_RULE_RESULT_CONSTANTS Table Rule Result Constants
@{
*//***************************************************************************/
/** Uses for masking of opaque1 & opaque2 valid bits*/
#define TABLE_TLUR_OPAQUE_VALID_BITS_MASK	0x01FF

/** Uses for masking of Table ID, Key ID valid bit*/
#define TABLE_TLUR_TKIDV_BIT_MASK		0x02000000

/** Table old result reserved space */
#define TABLE_OLD_RESULT_RESERVED1_SPACE	8
#define TABLE_OLD_RESULT_RESERVED2_SPACE	28

/** @} */ /* end of TABLE_RULE_RESULT_CONSTANTS */

/**************************************************************************//**
@Group	TABLE_ENTRY_MACROS Table Entry Macros
@{
*//***************************************************************************/
/** Entry type STDY field mask */
#define TABLE_ENTRY_STDY_FIELD_MASK			0xC0

/** Entry type Valid bit */
#define TABLE_ENTRY_VALID_BIT				0x20

/** Entry type Valid bit */
#define TABLE_ENTRY_PREVV_BIT				0x10

/** Entry type Entype field mask */
#define TABLE_ENTRY_ENTYPE_FIELD_MASK			0x0F

/** Entry type Entype EME44 */
#define TABLE_ENTRY_ENTYPE_EME44			0x00

/** Entry type Entype EME24 */
#define TABLE_ENTRY_ENTYPE_EME24			0x01

/** Entry type Entype EME36 */
#define TABLE_ENTRY_ENTYPE_EME36			0x02

/** Entry type Entype EME16 */
#define TABLE_ENTRY_ENTYPE_EME16			0x03

/** Entry type Entype LPM Marker*/
#define TABLE_ENTRY_ENTYPE_LPM_MARKER			0x04

/** Entry type Entype LPM Result*/
#define TABLE_ENTRY_ENTYPE_LPM_RES			0x05

/** Entry type Entype MFLU Rule*/
#define TABLE_ENTRY_ENTYPE_MFLU_RULE			0x08

/** Entry type Entype MFLU Result*/
#define TABLE_ENTRY_ENTYPE_MFLU_RES			0x09

/** Entry type Entype MFLU Branch*/
#define TABLE_ENTRY_ENTYPE_MFLU_BRANCH			0x0A

/** EME16 Lookup Key field size. */
#define TABLE_ENTRY_EME44_LOOKUP_KEY_SIZE		44

/** EME24 Lookup Key field size. */
#define TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE		24

/** EME16 Lookup Key field size. */
#define TABLE_ENTRY_EME36_LOOKUP_KEY_SIZE		36

/** EME16 Lookup Key field size. */
#define TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE		16

/** LPM Marker Entry Internal Entry Structure size. */
#define TABLE_ENTRY_LPM_MRKR_INT_STRUCT_SIZE		32

/** LPM Result Entry Lookup Key field size. */
#define TABLE_ENTRY_LPM_RES_LOOKUP_KEY_SIZE		24

/** MFLU Rule Entry Lookup Key field size. */
#define TABLE_ENTRY_MFLU_RULE_LOOKUP_KEY_SIZE		32

/** MFLU Result Entry Lookup Key field size. */
#define TABLE_ENTRY_MFLU_RES_LOOKUP_KEY_SIZE		16

/** @} */ /* end of TABLE_ENTRY_MACROS */

/**************************************************************************//**
@Group	TABLE_RULE_ENTRY_MACROS Table Rule-Entry related Macros
@{
*//***************************************************************************/
/** MFLU small key (lead node takes 2 entries) max size */
#define TABLE_MFLU_SMALL_KEY_MAX_SIZE				42

/** Number of entries needed for a tiny small key */
#define TABLE_EM_KEYSIZE_1_TO_24_BYTES_ENTRIES_PER_RULE		1

/** Number of entries needed for a medium EM key */
#define TABLE_EM_KEYSIZE_25_TO_60_BYTES_ENTRIES_PER_RULE	2

/** Number of entries needed for a big EM key */
#define TABLE_EM_KEYSIZE_61_TO_96_BYTES_ENTRIES_PER_RULE	3

/** Number of entries needed for a huge EM key */
#define TABLE_EM_KEYSIZE_97_TO_124_BYTES_ENTRIES_PER_RULE	4

/** Number of entries needed for IPv4 LPM rule in worst case */
#define TABLE_LPM_IPV4_WC_ENTRIES_PER_RULE			5

/** Number of entries needed for IPv6 LPM rule in worst case */
#define TABLE_LPM_IPV6_WC_ENTRIES_PER_RULE			17

/** Number of entries needed for MFLU rule for small key in worst case */
#define TABLE_MFLU_SMALL_KEY_WC_ENTRIES_PER_RULE		3

/** Number of entries needed for MFLU rule for big key in worst case */
#define TABLE_MFLU_BIG_KEY_WC_ENTRIES_PER_RULE			4

/** @} */ /* end of TABLE_RULE_ENTRY_MACROS */

/**************************************************************************//**
@Group	TABLE_MTYPE HW Table Accelerator Message Types
@{
*//***************************************************************************/
	/** Table create message type */
#define TABLE_CREATE_MTYPE				0x004C
	/** Table query without reference counting message type */
#define TABLE_QUERY_MTYPE				0x0043
	/** Table delete message type */
#define TABLE_DELETE_MTYPE				0x0042
	/** Table Parameters replace message type */
#define TABLE_PARAMETERS_REPLACE_MTYPE			0x0046

	/** Table rule create with reference counter decrement (for aged entry)
	 * */
#define TABLE_RULE_CREATE_RPTR_DEC_MTYPE		0x006C
	/** Table rule create */
#define TABLE_RULE_CREATE_MTYPE				0x007C
	/** Table rule create or replace with reference counter decrement (for
	 * the old entry) */
#define TABLE_RULE_CREATE_OR_REPLACE_RPTR_DEC_MTYPE	0x006D
	/** Table rule create or replace */
#define TABLE_RULE_CREATE_OR_REPLACE_MTYPE		0x007D
	/** Table rule replace with reference counter decrement (for the old
	 * entry) */
#define TABLE_RULE_REPLACE_MTYPE_RPTR_DEC_MTYPE		0x0066
	/** Table rule replace */
#define TABLE_RULE_REPLACE_MTYPE			0x0076
	/** Table rule delete with reference counter decrement. */
#define TABLE_RULE_DELETE_MTYPE_RPTR_DEC_MTYPE		0x0062
	/** Table rule delete */
#define TABLE_RULE_DELETE_MTYPE				0x0072

	/** Table rule query
	Without timestamp update, without reference counter update */
#define TABLE_RULE_QUERY_MTYPE				0x0077

	/** Table lookup with keyID and explicit parse result.
	With timestamp and reference counter update */
#define TABLE_LOOKUP_KEYID_EPRS_TMSTMP_RPTR_MTYPE	0x0160
	/** Table lookup with explicit key
	With timestamp and reference counter update. Single search only. */
#define TABLE_LOOKUP_KEY_TMSTMP_RPTR_MTYPE		0x0069

	/** Table Entry create or replace */
#define TABLE_ENTRY_CREATE_OR_REPLACE_MTYPE			0x005C
	/** Table Entry create or replace UniqueID */
#define TABLE_ENTRY_CREATE_OR_REPLACE_UNIQUEID_MTYPE		0x005D
	/** Table Entry delete always */
#define TABLE_ENTRY_DELETE_ALWAYS_MTYPE				0x0052
	/** Table Entry delete if aged */
#define TABLE_ENTRY_DELETE_IF_AGED_MTYPE			0x005A
	/** Table Entry query */
#define TABLE_ENTRY_QUERY_MTYPE					0x0053
	/** Table Entry query hash + uniqueID */
#define TABLE_ENTRY_QUERY_HASH_UNIQUEID_MTYPE			0x005B

	/** Table Acquire Semaphore */
#define TABLE_ACQUIRE_SEMAPHORE_MTYPE				0x00B0
	/** Table Release Semaphore */
#define TABLE_RELEASE_SEMAPHORE_MTYPE				0x00B1

#ifdef REV2_RULEID
	//** Table Rule Replace by Rule ID */
#define	TABLE_GET_NEXT_RULEID_MTYPE				0x004F
#define TABLE_GET_KEY_DESC_MTYPE				0x005F
#define TABLE_RULE_REPLACE_BY_RULEID_MTYPE			0x0075
#define	TABLE_RULE_DELETE_BY_RULEID_MTYPE			0x0071
#define TABLE_RULE_QUERY_BY_RULEID_MTYPE			0x0073
#endif

/** @} */ /* end of TABLE_MTYPE */

/**************************************************************************//**
@Group	TABLE_CREATE Table Create specific constants
@{
*//***************************************************************************/
	/** Table create input message reserved space */
#define TABLE_CREATE_INPUT_MESSAGE_RESERVED_SPACE	140

	/** BDI mask in table create ICID field */
#define TABLE_CREATE_INPUT_MESSAGE_ICID_BDI_MASK	0x8000

/** @} */ /* end of TABLE_CREATE */

/**************************************************************************//**
@Group	TABLE_QUERY Table Query specific constants
@{
*//***************************************************************************/
	/** Table query output message reserved space */
#define TABLE_QUERY_OUTPUT_MESSAGE_RESERVED_SPACE	20

/** @} */ /* end of TABLE_QUERY */

/**************************************************************************//**
@Group	TABLE_PARAMS_REPLACE Table Parameters Replace specific constants
@{
*//***************************************************************************/
	/** Table parameter replace input message reserved space */
#define TABLE_PARAMS_REPLACE_INPUT_MESSAGE_RESERVED_SPACE	44

/** @} */ /* end of TABLE_PARAMS_REPLACE */


/**************************************************************************//**
@Group	TABLE_STATUS Status returned to calling function
@{
*//***************************************************************************/
/** Command successful */
#define TABLE_HW_STATUS_SUCCESS	TABLE_STATUS_SUCCESS

/** Command failed general status bit.
 * A general bit that is set in some errors conditions */
#define TABLE_HW_STATUS_BIT_MGCF		0x80000000

/** Miss Occurred.
 * This bit is set when a matching rule is not found. Note that on chained
 * lookups this status is set only if the last lookup results in a miss. 
 * This bit is N/A if any other bit is set */
#define TABLE_HW_STATUS_BIT_MISS	TABLE_STATUS_MISS

/** Invalid Table ID.
 * This status is set if the lookup table associated with the TID is not
 * initialized. */
#define TABLE_HW_STATUS_BIT_TIDE	0x00000080

/** Resource is not available
 * This bit is N/A if #TABLE_HW_STATUS_BIT_TIDE is set.
 * */
#define TABLE_HW_STATUS_BIT_NORSC	0x00000020

/** Key Composition Error.
 * This bit is set when a key composition error occurs, meaning one of the
 * following:
 * - Invalid Key Composition ID was used.
 * - Key Size Error.
 * This bit is N/A if #TABLE_HW_STATUS_BIT_TIDE or #TABLE_HW_STATUS_BIT_NORSC
 * are set.
 * */
#define TABLE_HW_STATUS_BIT_KSE		0x00000400

/** Extract Out Of Frame Header.
 * This status is set if key composition attempts to extract a field which is
 * not in the frame header either because it is placed beyond the first 256
 * bytes of the frame, or because the frame is shorter than the index evaluated
 * for the extraction. 
 * This bit is N/A if #TABLE_HW_STATUS_BIT_TIDE, #TABLE_HW_STATUS_BIT_NORSC,
 * or #TABLE_HW_STATUS_BIT_KSE are set.*/
#define TABLE_HW_STATUS_BIT_EOFH	0x00000200

/** Maximum Number Of Chained Lookups Is Reached.
 * This bit is set if the number of table lookups performed by the CTLU
 * reached the threshold. Not supported in Rev1 
 * This bit is N/A if #TABLE_HW_STATUS_BIT_TIDE, #TABLE_HW_STATUS_BIT_NORSC,
 * #TABLE_HW_STATUS_BIT_KSE or #TABLE_HW_STATUS_BIT_EOFH are set.*/
#define TABLE_HW_STATUS_BIT_MNLE	0x00000100

/** Policer Initialization Entry Error.
 * Might be used in CTLU/MFLU to indicate other stuff.
 * */
#define TABLE_HW_STATUS_PIEE		0x00000040 

/**
 * Miss result creation failed in table creation
 */
#define TABLE_SW_STATUS_MISS_RES_CRT_FAIL	0xFF000001

/**
 * Miss result replacement failed due to non existence of miss result in the
 * table.
 */
#define TABLE_SW_STATUS_MISS_RES_RPL_FAIL	0xFF000002

/**
 * Miss result get failed due to non existence of miss result in the table.
 */
#define TABLE_SW_STATUS_MISS_RES_GET_FAIL	0xFF000003

/**
 * Rule query failed due to unrecognized entry type returned from HW.
 */
#define TABLE_SW_STATUS_QUERY_INVAL_ENTYPE	0xFF000004

/**
 * Unknown table type.
 */
#define TABLE_SW_STATUS_UNKNOWN_TBL_TYPE	0xFF000005

/**
 * Work around for TKT226361 error.
 */
#define TABLE_SW_STATUS_TKT226361_ERR		0xFF000006

/** @} */ /* end of TABLE_STATUS */

/**************************************************************************//**
@Group	TABLE_TKT226361 Defines for TABLE_TKT226361 WA
@{
*//***************************************************************************/
/* TODO remove section for non Rev1 */

/** Number of rules to be created in each table */
#define TABLE_TKT226361_RULES_NUM	2
/** Sufficient key size for each table */
#define TABLE_TKT226361_KEY_SIZE	1

/** @} */ /* end of TABLE_TKT226361 */

/** @} */ /* end of TABLE_MACROS */

/**************************************************************************//**
@Group		TABLE_Enumerations Table Enumerations

@Description	AIOP Table Enumerations

@{
*//***************************************************************************/

/**************************************************************************//**
@enum	table_function_identifier

@Description	IDs of table functions

@{
*//***************************************************************************/
enum table_function_identifier {
	TABLE_CREATE_FUNC_ID,
	TABLE_REPLACE_MISS_RESULT_FUNC_ID,
	TABLE_GET_PARAMS_FUNC_ID,
	TABLE_GET_MISS_RESULT_FUNC_ID,
	TABLE_DELETE_FUNC_ID,
	TABLE_RULE_CREATE_FUNC_ID,
	TABLE_RULE_CREATE_OR_REPLACE_FUNC_ID,
	TABLE_RULE_REPLACE_FUNC_ID,
	TABLE_RULE_QUERY_FUNC_ID,
	TABLE_RULE_DELETE_FUNC_ID,
#ifdef REV2_RULEID
	TABLE_GET_NEXT_RULEID_FUNC_ID,
	TABLE_GET_KEY_DESC_FUNC_ID,
	TABLE_RULE_REPLACE_BY_RULEID_FUNC_ID,
	TABLE_RULE_DELETE_BY_RULEID_FUNC_ID,
	TABLE_RULE_QUERY_BY_RULEID_FUNC_ID,
#endif
	TABLE_LOOKUP_BY_KEY_FUNC_ID,
	TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_FUNC_ID,
	TABLE_LOOKUP_BY_KEYID_FUNC_ID,
	TABLE_QUERY_DEBUG_FUNC_ID,
	TABLE_HW_ACCEL_ACQUIRE_LOCK_FUNC_ID,
	TABLE_HW_ACCEL_RELEASE_LOCK_FUNC_ID,
	TABLE_EXCEPTION_HANDLER_WRP_FUNC_ID,
	TABLE_EXCEPTION_HANDLER_FUNC_ID,
	TABLE_CALC_NUM_ENTRIES_PER_RULE_FUNC_ID,
	TABLE_WORKAROUND_TKT226361_FUNC_ID
};
/** @} */ /* end of table_function_identifier */


/**************************************************************************//**
@enum	table_entity

@Description	Distinguishes between entities that initiate fatal 

@{
*//***************************************************************************/
enum table_entity {
	TABLE_ENTITY_SW,
	TABLE_ENTITY_HW,
};
/** @} */ /* end of table_entity */

/** @} */ /* end of TABLE_Enumerations */


/**************************************************************************//**
@Group		TABLE_STRUCTS Table Structures

@Description	AIOP Table Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Table Create Input Message Structure

		Pointed by input pointer.

*//***************************************************************************/
#pragma pack(push, 1)
struct table_create_input_message {
	/** Table Attributes
	Includes IEX, MRES & AGT bits */
	uint16_t attributes;

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
	uint8_t  reserved[TABLE_CREATE_INPUT_MESSAGE_RESERVED_SPACE];

};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Create Output Message Structure

		Pointed by output pointer.

*//***************************************************************************/
#pragma pack(push, 1)
struct	table_create_output_message {
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
struct table_params_query_output_message {
	/** Table attributes
	Includes Type IEX, MRES & AGT bits */
	uint16_t attr;

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
	uint8_t  reserved[TABLE_QUERY_OUTPUT_MESSAGE_RESERVED_SPACE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body EME44
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme44 {

	/** Number of Sons */
	uint32_t number_of_sons;

	/** Part of lookup key */
	uint8_t  lookup_key_part[TABLE_ENTRY_EME44_LOOKUP_KEY_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body EME24
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme24 {

	/** Timestamp */
	uint32_t timestamp;

	/** Part of lookup key */
	uint8_t  lookup_key_part[TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE];

	/** Table result */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body EME36
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme36 {

	/** Number of Sons */
	uint32_t number_of_sons;

	/* PrevUniqueID*/
	uint32_t prev_unique_ID;

	/* PrevHash*/
	uint32_t prev_hash;

	/** Part of lookup key */
	uint8_t  lookup_key_part[TABLE_ENTRY_EME36_LOOKUP_KEY_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body EME16
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme16 {

	/** Timestamp */
	uint32_t timestamp;

	/** Table HW internal usage */
	uint64_t internal_usage1;

	/** Part of lookup key */
	uint8_t  lookup_key_part[TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE];

	/** Table result */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body LPM Marker
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_lpm_marker {

	/** LPM things */ /* Todo split this field*/
	uint32_t fallback_length_and_subsequent_merker_and_reserved;

	/** Lookup Key or PrevUnique ID */
	uint32_t lookup_key_prev_unique_ID;

	/** Lookup Key or PrevHash */
	uint32_t lookup_key_prev_hash;

	/** Lookup key */
	uint32_t  lookup_key;

	/** Table result */
	uint8_t internal_entry_structure[TABLE_ENTRY_LPM_MRKR_INT_STRUCT_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body LPM Result
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_lpm_res {

	/** Timestamp */
	uint32_t timestamp;

	/** Part of lookup key */
	uint8_t  lookup_key_part[TABLE_ENTRY_LPM_RES_LOOKUP_KEY_SIZE];

	/** Table result */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body MFLU Rule
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_rule {

	/** Priority */
	uint32_t priority;

	/* TODO */
	uint32_t prevUniqueID;

	/* TODO */
	uint32_t prevHashValue;

	/* TODO */
	uint16_t what;

	/** Reserved */
	uint16_t reserved1;

	/** Part of lookup key, each 9 bits describes a byte */
	uint8_t  lookup_key_part[TABLE_ENTRY_MFLU_RULE_LOOKUP_KEY_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body MFLU Result
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_res {

	/** Timestamp */
	uint32_t timestamp;

	/* TODO */
	uint32_t prevUniqueID;

	/* TODO */
	uint32_t prevHashValue;

	/** Part of lookup key, each 9 bits describes a byte */
	uint8_t  lookup_key_part[TABLE_ENTRY_MFLU_RES_LOOKUP_KEY_SIZE];

	/** Table result */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body MFLU Branch
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_branch {

	/** Index of the byte to be compared in the key */
	uint8_t  keyByteNum;

	/** Reserved */
	uint8_t  reserved1[3]; /* TODO magic */

	/* TODO */
	uint32_t prevUniqueID;

	/* TODO */
	uint32_t prevHashValue;

	/* TODO */
	uint16_t what;

	/** Exact match children enable bits */
	uint16_t em_children_enable;

	/** Child with wild cards exists flags */
	uint8_t  child_with_wildcards[32]; /* TODO */
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body Union
*//***************************************************************************/
#pragma pack(push, 1)
union table_entry_body {
	/** EME44 Entry - see CTLU specification for more details */
	struct table_entry_body_eme44 eme44;

	/** EME24 Entry - see CTLU specification for more details */
	struct table_entry_body_eme24 eme24;

	/** EME36 Entry - see CTLU specification for more details */
	struct table_entry_body_eme36 eme36;

	/** EME16 Entry - see CTLU specification for more details */
	struct table_entry_body_eme16 eme16;

	/** LPM Marker Entry - see CTLU specification for more details */
	struct table_entry_body_lpm_marker lpm_marker;

	/** LPM Result Entry - see CTLU specification for more details */
	struct table_entry_body_lpm_res lpm_res;

	/** MFLU Rule Entry - see MFLU specification for more details */
	struct table_entry_body_mflu_rule mflu_rule;

	/** MFLU Result Entry - see MFLU specification for more details */
	struct table_entry_body_mflu_res mflu_result;

	/** MFLU Branch Entry - see MFLU specification for more details */
	struct table_entry_body_mflu_branch mflu_branch;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry {
	/** Entry type (and some more things)
	 * Macros are available at: \ref TABLE_ENTRY_MACROS */
	uint8_t type;

	/** Entry key size */
	uint8_t entry_key_size;

	/** Table ID */
	uint16_t table_id;

	/** HW Internal use */
	uint32_t next_index;

	/** HW Internal use */
	uint32_t prev_index;

	/** Unique ID */
	uint32_t unique_id;

	/** The body of the entry (varies per type) */
	union table_entry_body body;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Old Result
*//***************************************************************************/
#pragma pack(push, 1)
struct table_old_result {
	/* Reserved */
	uint8_t reserved1[TABLE_OLD_RESULT_RESERVED1_SPACE];
	
	/* Rule ID */
	uint64_t rule_id;
	
	/* Reserved */
	uint8_t reserved2[TABLE_OLD_RESULT_RESERVED2_SPACE];

	/** The body of the entry (varies per type) */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Accelerator Context Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct table_acc_context {
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

/** @} */ /* end of TABLE_STRUCTS */

/**************************************************************************//**
@Group		TABLE_Functions Table Functions

@Description	AIOP Table Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	table_query_debug

@Description	Return all table parameters as kept in the given table
		accelerator HW.

@Param[in]	acc_id Table Accelerator ID.

@Param[in]	table_id Table ID.

@Param[in]	output All table parameter Table Hardware HW keeps.

@Return		Please refer to \ref FSL_TABLE_STATUS

@Cautions	This function performs a task switch.
		This function does not increment the reference count for the
		miss result returned.
*//***************************************************************************/
int table_query_debug(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_params_query_output_message *output);

/**************************************************************************//**
@Function	table_hw_accel_acquire_lock

@Description	Tries to acquire the binary lock of the given table hardware
		accelerator.

@Param[in]	acc_id Table Accelerator ID.

@Return		If the lock is already in use \ref CTLU_STATUS_MISS or
		\ref MFLU_STATUS_MISS are set.

@Cautions	This function performs a task switch.
*//***************************************************************************/
int table_hw_accel_acquire_lock(enum table_hw_accel_id acc_id);


/**************************************************************************//**
@Function	table_hw_accel_release_lock

@Description	Releases the binary lock of the given table hardware
		accelerator.

@Param[in]	acc_id Table Accelerator ID.

@Return		None.

@Cautions	This function performs a task switch.
*//***************************************************************************/
void table_hw_accel_release_lock(enum table_hw_accel_id acc_id);

/**************************************************************************//**
@Function	table_exception_handler_wrp

@Description	Wrapper for the handler of the error status returned from the
		Table API functions.
		This wrapper adds the file path in which the error occurred. It
		should be implemented inside a special compiler pragma which
		tells the compiler to store data (e.g. strings) in a dedicated
		location, so there will be good memory usage.
		This wrapper eases the process of relocation functions if
		needed (they can call another wrapper instead (i.e. wrapper is
		per file but the exception handling function of table API is
		located in one place and should not be changed over time.

@Param[in]	func_id The function in which the error occurred.
@Param[in]	line The line in which the error occurred.
@Param[in]	status Status to be handled in this function.
@Param[in]	entity Distinguishes between entities that initiate the
		exception request.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void table_exception_handler_wrp(enum table_function_identifier func_id,
				 uint32_t line,
				 int32_t status,
				 enum table_entity entity)
					__attribute__ ((noreturn));

/**************************************************************************//**
@Function	table_exception_handler_wrp

@Description	Wrapper for the handler of the error status returned from the
		Table API functions.
		This wrapper adds the file path in which the error occurred. It
		should be implemented inside a special compiler pragma which
		tells the compiler to store data (e.g. strings) in a dedicated
		location, so there will be good memory usage.
		This wrapper eases the process of relocation functions if
		needed (they can call another wrapper instead (i.e. wrapper is
		per file but the exception handling function of table API is
		located in one place and should not be changed over time.

@Param[in]	func_id The function in which the error occurred.
@Param[in]	line The line in which the error occurred.
@Param[in]	status Status to be handled in this function.
@Param[in]	entity Distinguishes between entities that initiate the
		exception request.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
inline void table_inline_exception_handler(
				 enum table_function_identifier func_id,
				 uint32_t line,
				 int32_t status,
				 enum table_entity entity)
					__attribute__ ((noreturn));

/**************************************************************************//**
@Function	table_exception_handler

@Description	Handler for the error status returned from the Table API
		functions.

@Param[in]	file_path The path of the file in which the error occurred.
@Param[in]	func_id The function in which the error occurred.
@Param[in]	line The line in which the error occurred.
@Param[in]	status_id Status ID to be handled in this function.
@Param[in]	entity Distinguishes between entities that initiate the
		exception request.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void table_exception_handler(char *file_path,
			     enum table_function_identifier func_id,
			     uint32_t line,
			     int32_t status_id,
			     enum table_entity entity)
				__attribute__ ((noreturn));

/**************************************************************************//**
@Function	table_calc_num_entries_per_rule

@Description	This function calculates and returns the maximum number of
		table entries needed per rule for a specific key size and
		table type.

@Param[in]	type Table.
@Param[in]	key_size Key size.

@Return		The maximum number of entries needed per rule for a specific
		key size and table type.

@Cautions	This function calls the exception handler in a case of invalid
		table type.
*//***************************************************************************/
int table_calc_num_entries_per_rule(uint16_t type, uint8_t key_size);

/**************************************************************************//**
@Function	table_workaround_tkt226361

@Description	Work around for tkt226361. Create a table in each memory region
		with 2 rules and then deletes the table.

@Param[in]	mflu_peb_num_entries number of entries that was defined for
		PEB memory region.
@Param[in]	mflu_dp_ddr_num_entries number of entries that was defined
		for DPDDR memory region.
@Param[in]	mflu_sys_ddr_num_entries number of entries that was defined
		for System DDR memory region.

@Cautions	This function calls the exception handler if the work around
		failed.
		This function performs a task switch.
*//***************************************************************************/
void table_workaround_tkt226361(uint32_t mflu_peb_num_entries,
				uint32_t mflu_dp_ddr_num_entries,
				uint32_t mflu_sys_ddr_num_entries);

/**************************************************************************//**
@Function	table_lookup_by_keyid_default_frame_wrp

@Description	Wrapper of the function table_lookup_by_keyid_default_frame.
		See description of the function:
		table_lookup_by_keyid_default_frame

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	keyid A Key Composition Rule (KCR) ID for the table lookups
		(Key Composition Rule specifies how to build a key). The key
		built by this KCR should fit \ref table_lookup_key_desc union
		and it's size should be equal to the key size the table was
		created with except for the following remark:
		 - the key size should be added priority field size (4 Bytes)
		for MFLU tables.
@Param[out]	lookup_result Points to a user preallocated memory to which
		the table lookup result will be written. The structure pointed
		by this pointer must be in the task's workspace and must be
		aligned to 16B boundary.

@Return		0 on success, TABLE_STATUS_MISS on miss or negative value if an
		error occurred.

@Retval		0 Success.
@Retval		#TABLE_STATUS_MISS A match was not found during the lookup
		operation.
@Retval		EIO Error, Key composition attempted to extract a field which
		is not in the frame header either because it is placed beyond
		the first 256 bytes of the frame, or because the frame is
		shorter than the index evaluated for the extraction.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
@Cautions	Presented header address in the workspace must be aligned to
			16 bytes.

*//***************************************************************************/
int table_lookup_by_keyid_default_frame_wrp(
				        enum table_hw_accel_id acc_id,
					uint16_t table_id,
					uint8_t keyid,
					struct table_lookup_result
					       *lookup_result);

/**************************************************************************//**
@Function	table_rule_create_wrp

@Description	Wrapper of function table_rule_create.
		See description of the function table_rule_create.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule The rule to be added. The structure pointed by this
		pointer must be in the task's workspace and must be aligned to
		16B boundary.
@Param[in]	key_size Key size in bytes. Should be equal to the key size the
		table was created with except for the following remark:
		 - the key size should be added priority field size (4 Bytes)
		for MFLU tables.

@Return		0 on success, or negative value on error.

@Retval		0 Success.
@Retval		ENOMEM Error, Not enough memory is available.
@Retval		EIO Error, A valid rule with the same key descriptor is found
		in the table. No change was made to the table.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int table_rule_create_wrp(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_rule *rule,
#ifdef REV2_RULEID
		      uint8_t key_size,
		      uint64_t *rule_id);
#else
			  uint8_t key_size);
#endif

/**************************************************************************//**
@Function	table_rule_delete_wrp

@Description	Wrapper of the function table_rule_delete.
		see the description of the function table_rule_delete.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	key_desc Key Descriptor of the rule to be queried. The
		structure pointed by this pointer must be in the task's
		workspace and must be aligned to 16B boundary.
@Param[in]	key_size Key size in bytes. Should be equal to the key size the
		table was created with except for the following remark:
		 - the key size should be added priority field size (4 Bytes)
		for MFLU tables.
@Param[in, out]	result The result of the deleted rule. If null the deleted
		rule's result will not be returned. If not null, structure
		should be allocated by the caller to this function.

@Return		0 on success or negative value on error.

@Retval		0 Success.
@Retval		EIO Error, a rule with the same key descriptor is not found
		in the table.

@Cautions	The key descriptor must be the exact same key descriptor that
		was used for the rule creation (not including reserved/priority
		fields).
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int table_rule_delete_wrp(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      union table_key_desc *key_desc,
		      uint8_t key_size,
		      struct table_result *result);


/** @} */ /* end of TABLE_Functions */

/** @} */ /* end of TABLE */

/* Asserting structure size for re-using of space allocated on stack in table
 * create function - macro appears twice because its checking < and not == */
#pragma warning_errors on
ASSERT_STRUCT_SIZE(sizeof(struct table_create_input_message), \
		   sizeof(struct table_rule));
ASSERT_STRUCT_SIZE(sizeof(struct table_rule), \
		   sizeof(struct table_create_input_message));
#pragma warning_errors off

#endif /* __TABLE_H */
