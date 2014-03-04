/**************************************************************************//**
@File		ctlu.h

@Description	This file contains the AIOP SW CTLU API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __CTLU_H
#define __CTLU_H

#include "general.h"
#include "dplib/fsl_table.h"

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

/** Table old result reserved space */
#define CTLU_TABLE_OLD_RESULT_RESERVED_SPACE	44

/** @} */ /* end of AIOP_CTLU_RULE_RESULT_CONSTANTS */

/**************************************************************************//**
@Group	CTLU_ENTRY_MACROS CTLU Entry Macros
@{
*//***************************************************************************/
/** Entry type Entype field mask */
#define CTLU_TABLE_ENTRY_ENTYPE_FIELD_MASK			0x0F

/** Entry type Entype EME24 */
#define CTLU_TABLE_ENTRY_ENTYPE_EME24				0x01

/** Entry type Entype EME16 */
#define CTLU_TABLE_ENTRY_ENTYPE_EME16				0x03

/** Entry type Entype LPM Result*/
#define CTLU_TABLE_ENTRY_ENTYPE_LPM_RES				0x05

/** EME24 Lookup Key field size. */
#define CTLU_TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE			24

/** EME16 Lookup Key field size. */
#define CTLU_TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE			16

/** LPM Result Entry Lookup Key field size. */
#define CTLU_TABLE_ENTRY_LPM_RES_LOOKUP_KEY_SIZE		24

/** MFLU Rule Entry Lookup Key field size. */
#define CTLU_TABLE_ENTRY_MFLU_RULE_LOOKUP_KEY_SIZE		32

/** MFLU Result Entry Lookup Key field size. */
#define CTLU_TABLE_ENTRY_MFLU_RES_LOOKUP_KEY_SIZE		16

/** @} */ /* end of CTLU_ENTRY_MACROS */

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

	/** CTLU Entry create or replace */
#define CTLU_ENTRY_CREATE_OR_REPLACE_MTYPE			0x005C
	/** CTLU Entry create or replace UniqueID */
#define CTLU_ENTRY_CREATE_OR_REPLACE_UNIQUEID_MTYPE		0x005D
	/** CTLU Entry delete always */
#define CTLU_ENTRY_DELETE_ALWAYS_MTYPE				0x0052
	/** CTLU Entry delete if aged */
#define CTLU_ENTRY_DELETE_IF_AGED_MTYPE				0x005A
	/** CTLU Entry query */
#define CTLU_ENTRY_QUERY_MTYPE					0x0053
	/** CTLU Entry query hash + uniqueID */
#define CTLU_ENTRY_QUERY_HASH_UNIQUEID_MTYPE			0x005B

	/** CTLU Acquire Semaphore */
#define TABLE_ACQUIRE_SEMAPHORE_MTYPE				0x00B0
	/** CTLU Release Semaphore */
#define TABLE_RELEASE_SEMAPHORE_MTYPE				0x00B1

/** @} */ /* end of CTLU_MTYPE */

/**************************************************************************//**
@Group	CTLU_TBL_CREATE CTLU Table Create specific constants
@{
*//***************************************************************************/
	/** Table create input message reserved space */
#define CTLU_TABLE_CREATE_INPUT_MESSAGE_RESERVED_SPACE	140

	/** BDI mask in table create ICID field */
#define CTLU_TABLE_CREATE_INPUT_MESSAGE_ICID_BDI_MASK	0x8000

/** @} */ /* end of CTLU_TBL_CREATE */

/**************************************************************************//**
@Group	CTLU_TBL_RULE_QUERY CTLU Table Query specific constants
@{
*//***************************************************************************/
	/** Table create input message reserved space */
#define CTLU_TABLE_QUERY_OUTPUT_MESSAGE_RESERVED_SPACE	16

/** @} */ /* end of CTLU_TBL_RULE_QUERY */

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
	struct table_rule_result miss_lookup_fcv;
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
	uint8_t  reserved[CTLU_TABLE_QUERY_OUTPUT_MESSAGE_RESERVED_SPACE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body EME16
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme16 {
	/** Reserved */
	uint8_t  reserved[3];

	/** CTLU HW internal usage */
	uint64_t internal_usage0;

	/** Unique ID */
	uint32_t unique_id;

	/** Timestamp */
	uint32_t timestamp;

	/** CTLU HW internal usage */
	uint64_t internal_usage1;

	/** Part of lookup key */
	uint8_t  lookup_key_part[CTLU_TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE];

	/** CTLU result */
	struct table_rule_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body EME24
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme24 {
	/** Reserved */
	uint8_t  reserved[3];

	/** CTLU HW internal usage */
	uint64_t internal_usage;

	/** Unique ID */
	uint32_t unique_id;

	/** Timestamp */
	uint32_t timestamp;

	/** Part of lookup key */
	uint8_t  lookup_key_part[CTLU_TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE];

	/** CTLU result */
	struct table_rule_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body LPM
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_lpm_res {
	/** Reserved */
	uint8_t  reserved[3];

	/** CTLU HW internal usage */
	uint64_t internal_usage0;

	/** Unique ID */
	uint32_t unique_id;

	/** Timestamp */
	uint32_t timestamp;

	/** Part of lookup key */
	uint8_t  lookup_key_part[CTLU_TABLE_ENTRY_LPM_RES_LOOKUP_KEY_SIZE];

	/** CTLU result */
	struct table_rule_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body MFLU Branch
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_branch {
	/** Reserved */
	uint8_t  reserved0[3];

	/** CTLU HW internal usage */
	uint64_t internal_usage0;

	/** Unique ID */
	uint32_t unique_id;

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
@Description	Table Entry Body MFLU Branch
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_rule {
	/** Reserved */
	uint8_t  reserved0[3];

	/** CTLU HW internal usage */
	uint64_t internal_usage0;

	/** Unique ID */
	uint32_t unique_id;

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
	uint8_t  lookup_key_part[CTLU_TABLE_ENTRY_MFLU_RULE_LOOKUP_KEY_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body MFLU Result
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_res {
	/** Reserved */
	uint8_t  reserved[3];

	/** CTLU HW internal usage */
	uint64_t internal_usage0;

	/** Unique ID */
	uint32_t unique_id;

	/** Timestamp */
	uint32_t timestamp;

	/* TODO */
	uint32_t prevUniqueID;

	/* TODO */
	uint32_t prevHashValue;

	/** Part of lookup key, each 9 bits describes a byte */
	uint8_t  lookup_key_part[CTLU_TABLE_ENTRY_MFLU_RES_LOOKUP_KEY_SIZE];

	/** CTLU result */
	struct table_rule_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body Union
*//***************************************************************************/
#pragma pack(push, 1)
union table_entry_body {
	/** EME16 Entry - see CTLU specification for more details */
	struct table_entry_body_eme16 eme16;

	/** EME24 Entry - see CTLU specification for more details */
	struct table_entry_body_eme24 eme24;

	/** LPM Result Entry - see CTLU specification for more details */
	struct table_entry_body_lpm_res lpm_res;

	/** MFLU Branch Entry - see CTLU specification for more details */
	struct table_entry_body_mflu_branch mflu_branch;

	/** MFLU Rule Entry - see CTLU specification for more details */
	struct table_entry_body_mflu_rule mflu_rule;

	/** MFLU Result Entry - see CTLU specification for more details */
	struct table_entry_body_mflu_res mflu_result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry {
	/* Entry type (and some more things)
	 * Macros are available at: \ref CTLU_ENTRY_MACROS */
	uint8_t type;

	/** The body of the entry (varies per type) */
	union table_entry_body body;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry
*//***************************************************************************/
#pragma pack(push, 1)
struct table_old_result {
	/* Reserved */
	uint8_t reserved[CTLU_TABLE_OLD_RESULT_RESERVED_SPACE];

	/** The body of the entry (varies per type) */
	struct table_rule_result result;
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

/** @} */ /* end of CTLU_STRUCTS */

/**************************************************************************//**
@Group		CTLU_Functions CTLU Functions

@Description	AIOP CTLU Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	table_query_debug

@Description	Return all table parameters as kept in the given table
		accelerator HW.

@Param[in]	acc_id - Table Accelerator ID.

@Param[in]	table_id - Table ID.

@Param[in]	output - All table parameter Table Hardware HW keeps.

@Return		Please refer to \ref FSL_CTLU_STATUS_TBL_GET_PARAMS

@Cautions	This function performs a task switch.
		This function does not increment the reference count for the
		miss result returned.
*//***************************************************************************/
int32_t table_query_debug(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct ctlu_table_params_query_output_message *output
			 );

/**************************************************************************//**
@Function	table_hw_accel_acquire_lock

@Description	Tries to acquire the binary lock of the given table hardware
		accelerator.

@Param[in]	acc_id - Table Accelerator ID.

@Return		If the lock is already in use \ref TABLE_STATUS_MISS is set.

@Cautions	This function performs a task switch.
*//***************************************************************************/
int32_t table_hw_accel_acquire_lock(enum table_hw_accel_id acc_id);


/**************************************************************************//**
@Function	table_hw_accel_release_lock

@Description	Releases the binary lock of the given table hardware
		accelerator.

@Param[in]	acc_id - Table Accelerator ID.

@Return		None.

@Cautions	This function performs a task switch.
*//***************************************************************************/
void table_hw_accel_release_lock(enum table_hw_accel_id acc_id);

/** @} */ /* end of CTLU_Functions */

/** @} */ /* end of CTLU */

#endif /* __CTLU_H */
