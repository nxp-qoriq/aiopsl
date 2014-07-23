/**************************************************************************//**
@File		table.h

@Description	This file contains the AIOP SW Table API

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
#define TABLE_OLD_RESULT_RESERVED_SPACE		44

/** @} */ /* end of TABLE_RULE_RESULT_CONSTANTS */

/**************************************************************************//**
@Group	TABLE_ENTRY_MACROS Table Entry Macros
@{
*//***************************************************************************/
/** Entry type Entype field mask */
#define TABLE_ENTRY_ENTYPE_FIELD_MASK			0x0F

/** Entry type Entype EME24 */
#define TABLE_ENTRY_ENTYPE_EME24			0x01

/** Entry type Entype EME16 */
#define TABLE_ENTRY_ENTYPE_EME16			0x03

/** Entry type Entype LPM Result*/
#define TABLE_ENTRY_ENTYPE_LPM_RES			0x05

/** Entry type Entype MFLU Result*/
#define TABLE_ENTRY_ENTYPE_MFLU_RES			0x09

/** EME16 Lookup Key field size. */
#define TABLE_ENTRY_EME44_LOOKUP_KEY_SIZE		44

/** EME16 Lookup Key field size. */
#define TABLE_ENTRY_EME36_LOOKUP_KEY_SIZE		36

/** EME16 Lookup Key field size. */
#define TABLE_ENTRY_EME16_LOOKUP_KEY_SIZE		16

/** EME24 Lookup Key field size. */
#define TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE		24

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

/** Number of entries needed for IPv4 LPM rule in worst case */
#define TABLE_LPM_IPV4_WC_ENTRIES_PER_RULE			5

/** Number of entries needed for IPv6 LPM rule in worst case */
#define TABLE_LPM_IPV6_WC_ENTRIES_PER_RULE			9

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
A general bit that is set in some errors conditions */
#define TABLE_HW_STATUS_MGCF	0x80000000

/** Miss Occurred.
 * This status is set when a matching rule is not found. Note that on chained
 * lookups this status is set only if the last lookup results in a miss. */
#define TABLE_HW_STATUS_MISS	TABLE_STATUS_MISS

/** Key Composition Error.
 * This status is set when a key composition error occurs, meaning one of the
 * following:
 * - Invalid Key Composition ID was used.
 * - Key Size Error.
 * */
#define TABLE_HW_STATUS_KSE	0x00000400

/** Extract Out Of Frame Header.
 * This status is set if key composition attempts to extract a field which is
 * not in the frame header either because it is placed beyond the first 256
 * bytes of the frame, or because the frame is shorter than the index evaluated
 * for the extraction. */
#define TABLE_HW_STATUS_EOFH	0x00000200

/** Maximum Number Of Chained Lookups Is Reached.
 * This status is set if the number of table lookups performed by the CTLU
 * reached the threshold. Not supported in Rev1 */
#define TABLE_HW_STATUS_MNLE	0x00000100

/** Policer Initialization Entry Error.
 * Might be used in CTLU/MFLU to indicate other stuff.
 * */
#define TABLE_HW_STATUS_PIEE	0x00000040 

/** Invalid Table ID.
 * This status is set if the lookup table associated with the TID is not
 * initialized. */
#define CTLU_HW_STATUS_TIDE	(0x00000080 | (TABLE_ACCEL_ID_CTLU << 24) | \
						TABLE_HW_STATUS_MGCF)

/** Resource is not available
 * */
#define CTLU_HW_STATUS_NORSC	(0x00000020 | (TABLE_ACCEL_ID_CTLU << 24) | \
						TABLE_HW_STATUS_MGCF)
/** Resource Is Temporarily Not Available.
 * Temporarily Not Available occurs if an other resource is in the process of
 * being freed up. Once the process ends, the resource may be available for new
 * allocation (availability is not guaranteed). */
#define CTLU_HW_STATUS_TEMPNOR	(0x00000010 | CTLU_HW_STATUS_NORSC)

/** Invalid Table ID.
 * This status is set if the lookup table associated with the TID is not
 * initialized. */
#define MFLU_HW_STATUS_TIDE	(0x00000080 | (TABLE_ACCEL_ID_MFLU << 24) | \
						TABLE_HW_STATUS_MGCF)

/** Resource is not available
 * */
#define MFLU_HW_STATUS_NORSC	(0x00000020 | (TABLE_ACCEL_ID_MFLU << 24) | \
						TABLE_HW_STATUS_MGCF)
/** Resource Is Temporarily Not Available.
 * Temporarily Not Available occurs if an other resource is in the process of
 * being freed up. Once the process ends, the resource may be available for new
 * allocation (availability is not guaranteed). */
#define MFLU_HW_STATUS_TEMPNOR	(0x00000010 | MFLU_HW_STATUS_NORSC)

/** @} */ /* end of TABLE_STATUS */

/** @} */ /* end of TABLE_MACROS */


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
@Description	Table Entry Body EME16
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme16 {
	/** Reserved */
	uint8_t  reserved[3];

	/**  Table internal usage */
	uint64_t internal_usage0;

	/** Unique ID */
	uint32_t unique_id;

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
@Description	Table Entry Body EME24
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_eme24 {
	/** Reserved */
	uint8_t  reserved[3];

	/** Table HW internal usage */
	uint64_t internal_usage;

	/** Unique ID */
	uint32_t unique_id;

	/** Timestamp */
	uint32_t timestamp;

	/** Part of lookup key */
	uint8_t  lookup_key_part[TABLE_ENTRY_EME24_LOOKUP_KEY_SIZE];

	/** Table result */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body LPM
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_lpm_res {
	/** Reserved */
	uint8_t  reserved[3];

	/** Table HW internal usage */
	uint64_t internal_usage0;

	/** Unique ID */
	uint32_t unique_id;

	/** Timestamp */
	uint32_t timestamp;

	/** Part of lookup key */
	uint8_t  lookup_key_part[TABLE_ENTRY_LPM_RES_LOOKUP_KEY_SIZE];

	/** Table result */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body MFLU Branch
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_branch {
	/** Reserved */
	uint8_t  reserved0[3];

	/** Table HW internal usage */
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

	/** Table HW internal usage */
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
	uint8_t  lookup_key_part[TABLE_ENTRY_MFLU_RULE_LOOKUP_KEY_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry Body MFLU Result
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry_body_mflu_res {
	/** Reserved */
	uint8_t  reserved[3];

	/** Table HW internal usage */
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
	uint8_t  lookup_key_part[TABLE_ENTRY_MFLU_RES_LOOKUP_KEY_SIZE];

	/** Table result */
	struct table_result result;
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

	/** MFLU Branch Entry - see MFLU specification for more details */
	struct table_entry_body_mflu_branch mflu_branch;

	/** MFLU Rule Entry - see MFLU specification for more details */
	struct table_entry_body_mflu_rule mflu_rule;

	/** MFLU Result Entry - see MFLU specification for more details */
	struct table_entry_body_mflu_res mflu_result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Entry
*//***************************************************************************/
#pragma pack(push, 1)
struct table_entry {
	/* Entry type (and some more things)
	 * Macros are available at: \ref TABLE_ENTRY_MACROS */
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
	uint8_t reserved[TABLE_OLD_RESULT_RESERVED_SPACE];

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

@Param[in]	acc_id - Table Accelerator ID.

@Param[in]	table_id - Table ID.

@Param[in]	output - All table parameter Table Hardware HW keeps.

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

@Param[in]	acc_id - Table Accelerator ID.

@Return		If the lock is already in use \ref CTLU_STATUS_MISS or
		\ref MFLU_STATUS_MISS are set.

@Cautions	This function performs a task switch.
*//***************************************************************************/
int table_hw_accel_acquire_lock(enum table_hw_accel_id acc_id);


/**************************************************************************//**
@Function	table_hw_accel_release_lock

@Description	Releases the binary lock of the given table hardware
		accelerator.

@Param[in]	acc_id - Table Accelerator ID.

@Return		None.

@Cautions	This function performs a task switch.
*//***************************************************************************/
void table_hw_accel_release_lock(enum table_hw_accel_id acc_id);


/**************************************************************************//**
@Function	table_fatal_status_handler

@Description	Handler for the status returned from the Table API functions.

@Param[in]	status - Status to be handled be this function.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void table_exception_handler(char *filename, uint32_t line, int32_t status);


/**************************************************************************//**
@Function	table_calc_num_entries_per_rule

@Description	This function calculates and returns the maximum number of
		table entries needed per rule for a specific key size and
		table type.

@Param[in]	type - Table.
@Param[in]	key_size - Key size.

@Return		The maximum number of entries needed per rule for a specific
		key size and table type.

@Cautions	This function calls the exception handler in a case of invalid
		table type.
*//***************************************************************************/
int table_calc_num_entries_per_rule(uint16_t type, uint8_t key_size);

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
