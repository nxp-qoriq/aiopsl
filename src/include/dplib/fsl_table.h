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
@File		fsl_table.h

@Description	This file contains the AIOP Software Table API

*//***************************************************************************/

#ifndef __FSL_TABLE_H
#define __FSL_TABLE_H

#include "common/types.h"


/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_TABLE TABLE

@Description	Freescale AIOP Table API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_TABLE_MACROS TABLE Macros

@Description	Freescale AIOP Table Macros

@{
*//***************************************************************************/


/**************************************************************************//**
@Group	FSL_TABLE_ATTRIBUTES TABLE Attributes

@Description	Table Attributes

The table attributes are composed of the following sub fields:
- Type field (Exact Match, Longest Prefix Match, etc..)
- Location (External/PEB/Internal) field
- Miss result options field

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_TABLE_ATTRIBUTE_TYPE TABLE Type Attribute

@Description	Table Type

These macros specifies the table type sub field offset and mask and the
available table types. \n User should select one of the following defines
(excluding mask and offset defines):
@{
*//***************************************************************************/
	/** Exact Match table,
	 * Not available for MFLU Table HW Accelerator */
#define TABLE_ATTRIBUTE_TYPE_EM		0x0000

	/** Longest Prefix Match table,
	 * Not available for MFLU Table HW Accelerator */
#define TABLE_ATTRIBUTE_TYPE_LPM	0x1000

/*
	 Ternary match Table for ACL
#define TABLE_ATTRIBUTE_TYPE_TCAM_ACL	0x2000
*/
	/** MFLU (Previously called Algorithmic ACL table)
	 * Not available for CTLU Table HW Accelerator */
#define TABLE_ATTRIBUTE_TYPE_MFLU	0x4000

	/** Table type sub field mask */
#define TABLE_ATTRIBUTE_TYPE_MASK	0xF000

	/** Table type sub field offset */
#define TABLE_ATTRIBUTE_TYPE_OFFSET	12

/** @} */ /* end of FSL_CTLU_TABLE_TYPE */


/**************************************************************************//**
@Group	FSL_TABLE_ATTRIBUTE_LOCATION TABLE Location Attribute

@Description	Table Location

These macros specifies the table location sub field offset and mask and the
available table locations. \n User should select one of the following defines
(excluding mask and offset defines):
@{
*//***************************************************************************/
	/** Internal table (located in dedicated RAM), Not available for
	 * MFLU Table HW Accelerator. Not available for Rev1. */
#define TABLE_ATTRIBUTE_LOCATION_INT	0x0200

	/** Packet Express Buffer table (Exact memory region is specified by
	 * the user at the initialization time of the Table HW Accelerator) */
#define TABLE_ATTRIBUTE_LOCATION_PEB	0x0300

	/** External table 1 (Exact memory region is specified by the user at
	 * the initialization time of the Table HW Accelerator) */
#define TABLE_ATTRIBUTE_LOCATION_EXT1	0x0400

	/** External table 2 (Exact memory region is specified by the user at
	 * the initialization time of the Table HW Accelerator) */
#define TABLE_ATTRIBUTE_LOCATION_EXT2	0x0500

	/** Table Location sub field mask */
#define TABLE_ATTRIBUTE_LOCATION_MASK	0x0700

	/** Table Location sub field offset */
#define TABLE_ATTRIBUTE_LOCATION_OFFSET	8

/** @} */ /* end of FSL_CTLU_TABLE_ATTRIBUTE_LOCATION */


/**************************************************************************//**
@Group	FSL_TABLE_ATTRIBUTE_MR TABLE Miss Result Attribute

@Description	Table Miss Result Options

These macros specifies options of the table miss result plus mask and offset of
the sub filed.\n User should select one of the following defines (excluding
mask and offset defines):
@{
*//***************************************************************************/
	/** Table without miss result */
#define TABLE_ATTRIBUTE_MR_NO_MISS	0x0000

	/** Table with miss result */
#define TABLE_ATTRIBUTE_MR_MISS		0x0080

	/** Miss result options sub field mask */
#define TABLE_ATTRIBUTE_MR_MASK		0x00C0

	/** Miss result options sub field offset */
#define TABLE_ATTRIBUTE_MR_OFFSET	6

/** @} */ /* end of FSL_TABLE_ATTRIBUTE_MR */

/** @} */ /* end of FSL_TABLE_ATTRIBUTES */


/**************************************************************************//**
@Group		FSL_TABLE_RESULT_TYPES TABLE Results Types

@Description	Table Results Types

User should select one of the following defines:

@{
*//***************************************************************************/
/** Result is used for chaining of lookups, Contains 9B Opaque fields
 * Not available for Rev1 */
#define TABLE_RESULT_TYPE_CHAINING		0x81

/** Result is 9B of opaque data fields and 8B Slab/CDMA buffer pointer (which
 * has reference counter) */
#define TABLE_RESULT_TYPE_REFERENCE		0x91

/** Result is 17B of opaque data fields */
#define TABLE_RESULT_TYPE_OPAQUES		0xB1

/** @} *//* end of FSL_TABLE_RESULT_TYPES */


/**************************************************************************//**
@Group	FSL_TABLE_RULE_OPTIONS TABLE Rule Options
@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_TABLE_RULE_OPTIONS_TIMESTAMP TABLE Rule Timestamp Options

@Description	Table Rule Timestamp Options

User should select one of the following:

@{
*//***************************************************************************/

	/** Timestamp is disabled for this rule.
	Rule's timestamp field is cleared. */
#define	TABLE_RULE_TIMESTAMP_NONE	0x00

	/** Enables timestamp update per rule.
	Initial value of Rule's timestamp field is set when the rule is created
	or replaced. The rule's timestamp field is updated to the current
	timestamp when the rule is matched during lookup command.*/
#define	TABLE_RULE_TIMESTAMP_ENABLE	0x80

/** @} *//* end of FSL_TABLE_RULE_OPTIONS_TIMESTAMP */

/** @} *//* end of FSL_TABLE_RULE_OPTIONS */


/**************************************************************************//**
@Group	FSL_TABLE_KEY_DEFINES TABLE Rule Key
@{
*//***************************************************************************/
	/** Exact Match maximum key size in bytes */
#define TABLE_KEY_EXACT_MATCH_SIZE		124
	/** Exact Match Key Descriptor Reserved field size in bytes */
#define TABLE_KEY_EXACT_MATCH_RESERVED_SIZE	4

	/** IPv4 LPM key size */
#define TABLE_KEY_LPM_IPV4_SIZE			0x09
	/** IPv4 LPM Key Descriptor Reserved field size in bytes */
#define TABLE_KEY_LPM_IPV4_RESERVED_SIZE	119

	/** IPv6 LPM key size */
#define TABLE_KEY_LPM_IPV6_SIZE			0x15
	/** IPv6 LPM Key Descriptor Reserved field size in bytes */
#define TABLE_KEY_LPM_IPV6_RESERVED_SIZE	107

	/** MFLU maximum key size in bytes */
#define TABLE_KEY_MFLU_SIZE			0x38
	/** MFLU key mask field size in bytes */
#define TABLE_KEY_MFLU_MASK_SIZE		0x38
	/** MFLU priority field size in bytes */
#define TABLE_KEY_MFLU_PRIORITY_FIELD_SIZE	0x4
	/** MFLU Key Descriptor Reserved1 field size in bytes */
#define TABLE_KEY_MFLU_RESERVED1_SIZE		8

/** @} */ /* end of FSL_KEY_DEFINES */


/**************************************************************************//**
@Group	FSL_TABLE_LOOKUP_FLAG_DEFINES TABLE Lookup Flags
@{
*//***************************************************************************/
	/** Segment Address and Size Non Default - If set, the Segment given in
	the lookup function parameters is used instead of the default segment.
	Not available for Rev1  */
#define TABLE_LOOKUP_FLAG_SEG_NON_DEFAULT		0x80000000

	/** Parse Result Address Non Default - If set, the Parse Result Address
	given in the lookup function parameters is used instead of the default
	address. Not available for Rev1 */
#define TABLE_LOOKUP_FLAG_PRA_NON_DEFAULT		0x40000000

	/** Frame Descriptor Address Non Default - If set, the Frame Descriptor
	Address given in the lookup function parameters is used instead of the
	default address. Not available for Rev1 */
#define TABLE_LOOKUP_FLAG_FD_NON_DEFAULT		0x20000000

	/** Metadata Non Default - If set, the metadata given in the lookup
	function parameters is used instead of the default metadata which is
	zeroes. */
#define TABLE_LOOKUP_FLAG_MTDT_NON_DEFAULT		0x10000000

	/** No Lookup Flags */
#define TABLE_LOOKUP_FLAG_NONE				0x00000000

/** @} */ /* end of FSL_TABLE_LOOKUP_FLAG_DEFINES */


/**************************************************************************//**
@Group	FSL_TABLE_STATUS Status returned to calling function
@{
*//***************************************************************************/
/** Command successful */
#define TABLE_STATUS_SUCCESS	0x00000000

/** Miss Occurred.
 * This status is set when a matching rule is not found. Note that on chained
 * lookups this status is set only if the last lookup results in a miss. */
#define TABLE_STATUS_MISS	0x00000800

/** @} */ /* end of FSL_TABLE_STATUS */

/** @} */ /* end of FSL_TABLE_MACROS */

/**************************************************************************//**
@Group		FSL_TABLE_Enumerations TABLE Enumerations

@Description	Table Enumerations

@{
*//***************************************************************************/

/**************************************************************************//**
@enum	table_hw_accel_id

@Description	IDs of hardware table lookup accelerator

@{
*//***************************************************************************/
enum table_hw_accel_id {
	/** MFLU accelerator ID */
	TABLE_ACCEL_ID_MFLU = 0x02,
	/** CTLU accelerator ID */
	TABLE_ACCEL_ID_CTLU = 0x05
};

/** @} */ /* end of table_hw_accel_id */

/** @} */ /* end of FSL_TABLE_Enumerations */

/**************************************************************************//**
@Group		FSL_TABLE_STRUCTS TABLE Structures

@Description	Freescale AIOP Table Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Table Rule Result Chaining Parameters
*//***************************************************************************/
#pragma pack(push, 1)
struct table_result_chain_parameters {
	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint32_t reserved0;

	/** Table ID */
	uint16_t table_id;

	/** Key ID */
	uint8_t  keyid;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	\ref table_result structure ctlu_op0_refptr_clp field.

This field can be used either:
- As an opaque field of 8 bytes. \ref table_result type field should be set to
#TABLE_RESULT_TYPE_OPAQUES.\n Returned as part of lookup result.
- As a pointer to Slab/CDMA acquired buffer (which has reference counter).
\ref table_result type field should be set to #TABLE_RESULT_TYPE_REFERENCE.\n
Returned as part of lookup result.
- As a structure containing table ID and Key ID parameters for a chained
lookup. \ref table_result type field should be set to
#TABLE_RESULT_TYPE_CHAINING.
*//***************************************************************************/
#pragma pack(push, 1)
union table_result_op0_refptr_clp {
	/** Opaque Data - Returned as part of lookup result */
	uint64_t opaque0;

	/** Reference Pointer
	A pointer to Slab/CDMA acquired buffer (which has reference counter).
	The Table Hardware can increment the reference counter of the
	Slab/CDMA buffer on certain operations (please refer to \link
	FSL_TABLE_Functions Table functions documentation\endlink).\n
	Returned as part of lookup result. */
	uint64_t reference_pointer;

	/** A structure that contains table ID and key composition ID
	parameters for the chained lookups. Not available for Rev1 */
	struct table_result_chain_parameters chain_parameters;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Result

This structure represents the table result. Some of the fields defined here are
returned after lookup, see fields specification for more details.
*//***************************************************************************/
#pragma pack(push, 1)
struct table_result {
	/** Result Type - Should be set to one of the types specified in
	\ref FSL_TABLE_RESULT_TYPES macros. */
	uint8_t  type;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint16_t reserved;

	/** Opaque data - Returned as part of lookup result.
	Not valid when type is set to #TABLE_RESULT_TYPE_CHAINING */
	uint8_t  opaque2;

	/** Opaque0 or Reference Pointer or Chained Lookup Parameters.
	Chained Lookup is not available in Rev1.\n
	For more details please refer to \link table_result_op0_refptr_clp
	structure documentation\endlink. */
	union table_result_op0_refptr_clp op0_rptr_clp;

	/** Opaque data - Returned as part of lookup result.
	Not valid when type is set to #TABLE_RESULT_TYPE_CHAINING */
	uint64_t opaque1;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Exact Match Key Descriptor Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct table_key_desc_em {
	/** Exact Match key */
	uint8_t  key[TABLE_KEY_EXACT_MATCH_SIZE];

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved[TABLE_KEY_EXACT_MATCH_RESERVED_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	LPM IPv4 Key Descriptor Structure

The CTLU searches for the LPM of the concatenation of
{exact_match, full_ipv4_address}.
*//***************************************************************************/
#pragma pack(push, 1)
struct table_key_desc_lpm_ipv4 {
	/** Exact Match bytes */
	uint32_t exact_match;

	/** IPv4 Address */
	uint32_t addr;

	/** IPv4 Address Prefix length.
	 * Must be > 0. */
	uint8_t  prefix_length;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved[TABLE_KEY_LPM_IPV4_RESERVED_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	LPM IPv6 Key Descriptor Structure

The CTLU searches for the LPM of the concatenation of
{exact_match, full_ipv6_address}.
*//***************************************************************************/
#pragma pack(push, 1)
struct table_key_desc_lpm_ipv6 {
	/** Exact Match bytes */
	uint32_t exact_match;

	/** IPv6 Address (8 MSB) */
	uint64_t addr0;

	/** IPv6 Address (8 LSB) */
	uint64_t addr1;

	/** IPv6 Address Prefix length.
	 * Must be > 0. */
	uint8_t  prefix_length;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved[TABLE_KEY_LPM_IPV6_RESERVED_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	MFLU Key Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct table_key_desc_mflu {
	/** MFLU Lookup Key & Priority - This should point on a memory location
	containing concatenation of the	following fields (by the same order):
	 - Lookup Key - Size of this field must be within 1-56 byte.
	 - Priority - Priority determines the selection between two rule that
	match in the MFLU lookup. 0x00000000 is the highest priority. This
	field size is 4 bytes. */
	uint8_t  key[TABLE_KEY_MFLU_SIZE + TABLE_KEY_MFLU_PRIORITY_FIELD_SIZE];

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint32_t reserved0;

	/** Key mask -Each byte in the mask must have contiguous 1's in the
	MSbits. Therefore there are 9 valid values for each byte in the mask:
	 - 0x00: The entire byte is masked.
	 - 0x80: The MSbit in this byte is not masked.
	 - 0xC0: The 2 MSbits in this byte are not masked.
	 - ...
	 - 0xFE: The 7 MSbits in this byte are not masked.
	 - 0xFF: The entire byte is not masked. */
	uint8_t  mask[TABLE_KEY_MFLU_MASK_SIZE];

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1[TABLE_KEY_MFLU_RESERVED1_SIZE];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Key Descriptor
*//***************************************************************************/
union table_key_desc {
	/** Exact Match Key Descriptor,
	Should only be used with CTLU Hardware Table Accelerator */
	struct table_key_desc_em          em;

	/** LPM IPv4 Key Descriptor,
	Should only be used with CTLU Hardware Table Accelerator */
	struct table_key_desc_lpm_ipv4    lpm_ipv4;

	/** LPM IPv6 Key Descriptor,
	Should only be used with CTLU Hardware Table Accelerator */
	struct table_key_desc_lpm_ipv6    lpm_ipv6;

	/** MFLU Key Descriptor,
	Should only be used with MFLU Hardware Table Accelerator */
	struct table_key_desc_mflu        mflu;
};


/**************************************************************************//**
@Description	Table Rule
*//***************************************************************************/
#pragma pack(push, 1)
struct table_rule {
	/** Rule's key descriptor */
	union table_key_desc key_desc;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint64_t reserved0;

	/** Table Rule Options - Please refer to \ref FSL_TABLE_RULE_OPTIONS
	for more details.*/
	uint8_t  options;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1[3];

	/** Table Rule Result */
	struct table_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Lookup Result

This structure returned from the table accelerator upon a successful lookup.
*//***************************************************************************/
#pragma pack(push, 1)
struct table_lookup_result {
	/** Opaque0 or Reference Pointer - This field can be either:
	- 8 bytes of opaque data.
	- A pointer to Slab/CDMA acquired buffer (which has a reference counter)

	Depends on the matching rule result type. */
	uint64_t opaque0_or_reference;

	/** Opaque data */
	uint64_t opaque1;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint16_t reserved0;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1;

	/** Opaque data */
	uint8_t  opaque2;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint64_t reserved2;

	/** Timestamp of the rule that was matched in the lookup process.
	For this timestamp to be valid, suitable option should be set in
	\ref table_rule option field. */
	uint32_t timestamp;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	LPM IPv4 Lookup Key Descriptor Structure

The CTLU searches for the LPM of the concatenation of
{exact_match, full_ipv4_address}.
*//***************************************************************************/
#pragma pack(push, 1)
struct table_lookup_key_desc_lpm_ipv4 {
	/** Exact Match bytes */
	uint32_t exact_match;

	/** IPv4 Address */
	uint32_t addr;

	/** Maximum Prefix - Defines the maximum IP address prefix length for
	this search. The CTLU does not search for a prefix length larger than
	max_prefix. For lookup on all prefixes prefix_length = 0xFF. This field
	must be > 1 */
	uint8_t	 max_prefix;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	LPM IPv6 Key Descriptor Structure

The CTLU searches for the LPM of the concatenation of
{exact_match, full_ipv6_address}.
*//***************************************************************************/
#pragma pack(push, 1)
struct table_lookup_key_desc_lpm_ipv6 {
	/** Exact Match bytes */
	uint32_t exact_match;

	/** IPv6 Address (8 MSB) */
	uint64_t addr0;

	/** IPv6 Address (8 LSB) */
	uint64_t addr1;

	/** Maximum Prefix - Defines the maximum IP address prefix length for
	this search. The CTLU does not search for a prefix length larger than
	max_prefix. For lookup on all prefixes prefix_length = 0xFF. This field
	must be > 1 */
	uint8_t  max_prefix;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Lookup Key Descriptor
*//***************************************************************************/
union table_lookup_key_desc {
	/** Exact Match Key - Should only be used with CTLU Hardware Table
	Accelerator and tables of type #TABLE_ATTRIBUTE_TYPE_EM */
	void                                  *em_key;

	/** LPM IPv4 Key Descriptor - Should only be used with CTLU Hardware
	Table Accelerator and tables of type #TABLE_ATTRIBUTE_TYPE_LPM that
	were defined with \ref TABLE_KEY_LPM_IPV4_SIZE key size. */
	struct table_lookup_key_desc_lpm_ipv4 *lpm_ipv4;

	/** LPM IPv6 Key Descriptor - Should only be used with CTLU Hardware
	Table Accelerator and tables of type \ref TABLE_ATTRIBUTE_TYPE_LPM that
	were defined with \ref TABLE_KEY_LPM_IPV6_SIZE key size. */
	struct table_lookup_key_desc_lpm_ipv6 *lpm_ipv6;

	/** MFLU Lookup Key & Match Maximum Priority - This should point on a
	memory location containing concatenation of the following fields
	(by the same order):
	 - Lookup Key - Size of this field must be within 1-56 byte.
	 - Maximum Priority - defines the maximum priority to be matched in the
	lookup operation. Rules with lower priority will not be matched. 0 is
	the lowest priority, 0xFFFFFFFF is the highest priority. For lookup of
	all priorities assign 0 to this field. This field size is 4 bytes.

	Should only be used with MFLU Hardware Table Accelerator and tables
	of type #TABLE_ATTRIBUTE_TYPE_MFLU. */
	void                                  *mflu_key;
};


/**************************************************************************//**
@Description	Create Table Parameters
*//***************************************************************************/
struct table_create_params {
	/** The table committed number of rules, at any point in time the table
	can contain at least this number of rules.*/
	uint32_t committed_rules;

	/** The max number of rules this table can contain. This number is not
	guaranteed in contrast to committed_rules. Meaning, trying to add a
	rule to a table that already contains committed_rules might fail.
	NOTE: This field must not be 0. */
	uint32_t max_rules;

	/** A default rule that is chosen when no match is found. Available
	only for CTLU tables, This field should not be filled otherwise.*/
	struct table_result miss_result;

	/** Table Attributes - Please refer to \ref FSL_TABLE_ATTRIBUTES Table
	for more details. */
	uint16_t attributes;

	/** Table Key Size in bytes (fixed per table).
	In a case of EM table key size is limited to 1-124 Bytes
	In a case of LPM table:
	 - Should be set to #TABLE_KEY_LPM_IPV4_SIZE for IPv4.
	 - Should be set to #TABLE_KEY_LPM_IPV6_SIZE for IPv6.
	In a case of MFLU table, size should not include the priority field.

	Please note that this value is not returned through
	\ref table_get_params() function. */
	uint8_t  key_size;
};


/**************************************************************************//**
@Description	Get Table Parameters Output
*//***************************************************************************/
struct table_get_params_output {
	/** Table's current number of rules */
	uint32_t current_rules;

	/** The table committed number of rules, at any point in time the table
	can contain at least this number of rules.*/
	uint32_t committed_rules;

	/** The max number of rules this table can contain. This number is not
	guaranteed in contrast to committed_rules. Meaning, trying to add a
	rule to a table that already contains committed_rules might fail. */
	uint32_t max_rules;

	/** Table Attributes - Please refer to \ref FSL_TABLE_ATTRIBUTES
	for more details. */
	uint16_t attributes;
};


/**************************************************************************//**
@Description	Table Lookup Non Default Parameters Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct table_lookup_non_default_params {
	/** Segment Address
	This segment will usually contain the frame header. */
	uint16_t segment_addr;

	/** Segment Size */
	uint16_t segment_size;

	/** Parse Result Address */
	uint16_t parse_result_addr;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint16_t reserved0;

	/** Frame Descriptor Address */
	uint16_t fd_addr;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint32_t reserved1;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint16_t reserved2;

	/** User Metadata - This can contain metadata for the key creation
	process */
	uint64_t metadata;
};
#pragma pack(pop)

#ifdef REV2_RULEID
/**************************************************************************//**
@Description	Table Rule ID Descriptor Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct table_rule_id_desc {
	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint64_t reserved;
	
	/** Rule ID */
	uint64_t rule_id;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	Table Rule ID and Result Descriptor Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct table_ruleid_and_result_desc {
	/** Rule ID Descriptor */
	struct table_rule_id_desc rule_id_desc;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint64_t reserved0[3];

	/** Table Rule Options - Please refer to \ref FSL_TABLE_RULE_OPTIONS
	for more details.*/
	uint8_t  options;

	/** Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1[3];

	/** Table Rule Result */
	struct table_result result;
};
#pragma pack(pop)
#endif //REV2_RULEID

/** @} */ /* end of FSL_TABLE_STRUCTS */

#include "table_inline.h"

/**************************************************************************//**
@Group		FSL_TABLE_Functions TABLE Functions

@Description	Freescale AIOP Table Functions

@{
*//***************************************************************************/

/* ######################################################################### */
/* ########################### Table Operations ############################ */
/* ######################################################################### */

/**************************************************************************//**
@Function	table_create

@Description	Creates a new table.

@Param[in]	acc_id The ID of the Hardware Table Accelerator in which the
		the table will be created.
@Param[in]	tbl_params The table parameters.
@Param[out]	table_id Table ID. A unique (per Hardware Table Accelerator)
		table identification number to be used for future table
		references.

@Return		0 on success, or negative value on error.

@Retval		0 Success.
@Retval		ENOMEM Error, Not enough memory is available.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
inline int table_create(enum table_hw_accel_id acc_id,
		 struct table_create_params *tbl_params,
		 uint16_t *table_id);


/**************************************************************************//**
@Function	table_replace_miss_result

@Description	Replaces specific table miss result.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	new_miss_result A default result that is chosen when no match
		is found.
@Param[in, out]	old_miss_result The replaced miss result. If null the old
		miss result will not be returned. If not null, structure should
		be allocated by the caller to this function.

@Return		None.

@Cautions	Not available for MFLU table accelerator.
@Cautions	This function should only be called if the table was defined
		with a miss result (i.e. #TABLE_ATTRIBUTE_MR_MISS was set in
		table attributes).
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
inline void table_replace_miss_result(enum table_hw_accel_id acc_id,
			       uint16_t table_id,
			       struct table_result *new_miss_result,
			       struct table_result *old_miss_result);


/**************************************************************************//**
@Function	table_get_params

@Description	A getter for the table parameters.
		\n \n This function does not return the table miss result.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the query will be performed.
@Param[in]	table_id Table ID.
@Param[out]	tbl_params Table parameters. Structure should be allocated by
		the caller to this function.

@Return		None.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
void table_get_params(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_get_params_output *tbl_params);


/**************************************************************************//**
@Function	table_get_miss_result

@Description	A getter for the table miss result.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the query will be performed.
@Param[in]	table_id Table ID.
@Param[out]	miss_result A default rule data that is chosen when no match
		is found. Structure should be allocated by the caller to this
		function.

@Return		None.

@Cautions	Not available for MFLU table accelerator.
@Cautions	This function should only be called if the table was defined
		with a miss result (i.e. #TABLE_ATTRIBUTE_MR_MISS was set in
		table attributes).
@Cautions	NOTE: If the result is of type that contains pointer to
		Slab/CDMA buffer (refer to struct table_rule_result
		documentation) this function will not increment the reference
		counter of the buffer.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
void table_get_miss_result(enum table_hw_accel_id acc_id,
			   uint16_t table_id,
			   struct table_result *miss_result);


/**************************************************************************//**
@Function	table_delete

@Description	Deletes a specified table.
		\n \n When the function returns the table is logically deleted.
		It is not guaranteed that all the memory associated with
		the table is released when the function returns, as the process
		of releasing the memory may take some time.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.

@Return		None.

@Cautions	In this function the task yields.
@Cautions	If the table contains table_result of type \ref
		TABLE_RESULT_TYPE_REFERENCE the table should be empty when this
		function is called. In a case which the deleted table miss
		result is of type \ref TABLE_RESULT_TYPE_REFERENCE the user
		should decrement the reference counter of the buffer pointed by
		the miss result pointer after the table delete operation.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
inline void table_delete(enum table_hw_accel_id acc_id,
		  uint16_t table_id);


/* ######################################################################### */
/* ######################## Table Rule Operations ########################## */
/* ######################################################################### */
/**************************************************************************//**
@Function	table_rule_create

@Description	Adds a rule to a specified table.
		\n \n If the rule key already exists, the rule will not be
		added and a status will be returned.

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
inline int table_rule_create(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_rule *rule,
		      uint8_t key_size);


/**************************************************************************//**
@Function	table_rule_create_or_replace

@Description	Adds/replaces a rule to a specified table.
		\n \n If the rule key already exists, the rule will be replaced
		by the one specified in the function's parameters. Else, a new
		rule will be created in the table.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule The rule to be added. The structure pointed by this
		pointer must be in the task's workspace and must be aligned to
		16B boundary.
@Param[in]	key_size Key size in bytes.Should be equal to the key size the
		table was created with except for the following remark:
		 - the key size should be added priority field size (4 Bytes)
		for MFLU tables.
@Param[in, out]	old_res The result of the replaced rule. Valid only if
		replace took place. If set to null the replaced rule's result
		will not be returned. If not null, structure should be
		allocated by the caller to this function.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 Success.  A rule with the same key descriptor was found in
		the table. The rule was replaced.
@Retval		#TABLE_STATUS_MISS Success, A rule with the same key descriptor
		was not found in the table. A new rule is created.
@Retval		ENOMEM Error, Not enough memory is available.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int table_rule_create_or_replace(enum table_hw_accel_id acc_id,
				 uint16_t table_id,
				 struct table_rule *rule,
				 uint8_t key_size,
				 struct table_result *old_res);


/**************************************************************************//**
@Function	table_replace_rule

@Description	Replaces a specified rule in the table.
		\n \n The rule's key is not modifiable. Caller to this function
		supplies the key of the rule to be replaced.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule Table rule, contains the rule's key descriptor, with
		which the rule to be replaced will be found and contain the
		rule result to be replaced. The structure pointed by this
		pointer must be in the task's workspace and must be aligned to
		16B boundary.
@Param[in]	key_size Key size in bytes. Should be equal to the key size the
		table was created with except for the following remark:
		 - the key size should be added priority field size (4 Bytes)
		for MFLU tables.
@Param[in, out]	old_res The result of the replaced rule. If null the replaced
		rule's result will not be returned. If not null, structure
		should be allocated by the caller to this function.

@Return		0 on success or negative value on error.

@Retval		0 Success.
@Retval		EIO Error, a rule with the same key descriptor is not found
		in the table.

@Cautions	The key descriptor must be the exact same key descriptor that
		was used for the rule creation (not including
		reserved/priority fields).
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
inline int table_rule_replace(enum table_hw_accel_id acc_id,
		       uint16_t table_id,
		       struct table_rule *rule,
		       uint8_t key_size,
		       struct table_result *old_res);


/**************************************************************************//**
@Function	table_rule_query

@Description	Queries a rule in the table.
		\n \n This function does not update the matched result
		timestamp.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the query will be performed.
@Param[in]	table_id Table ID.
@Param[in]	key_desc Key Descriptor of the rule to be queried. The
		structure pointed by this pointer must be in the task's
		workspace and must be aligned to 16B boundary.
@Param[in]	key_size Key size in bytes. Should be equal to the key size the
		table was created with except for the following remark:
		 - the key size should be added priority field size (4 Bytes)
		for MFLU tables.
@Param[out]	result The result of the query. Structure should be allocated
		by the caller to this function.
@Param[out]	timestamp Timestamp of the result. Timestamp is not valid
		unless the rule queried for was created with suitable options
		(Please refer to \ref FSL_TABLE_RULE_OPTIONS for more
		details). Must be allocated by the caller to this function.

@Return		0 on success, #TABLE_STATUS_MISS on miss.

@Retval		0 Success.
@Retval		#TABLE_STATUS_MISS A rule with the same key descriptor is not
		found in the table.

@Cautions	NOTE: If the result is of type that contains pointer to
		Slab/CDMA buffer (refer to struct table_rule_result
		documentation) this function will not increment the reference
		counter of the buffer.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
inline int table_rule_query(enum table_hw_accel_id acc_id,
		     uint16_t table_id,
		     union table_key_desc *key_desc,
		     uint8_t key_size,
		     struct table_result *result,
		     uint32_t *timestamp);


/**************************************************************************//**
@Function	table_rule_delete

@Description	Deletes a specified rule in the table.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	key_desc Key Descriptor of the rule to be deleted. The
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
inline int table_rule_delete(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      union table_key_desc *key_desc,
		      uint8_t key_size,
		      struct table_result *result);


/* ######################################################################### */
/* ############################# Table Lookups ############################# */
/* ######################################################################### */


/**************************************************************************//**
@Function	table_lookup_by_key

@Description	Performs a lookup with a key built by the user.
		\n \n If opaque0_or_reference result field is a reference
		pointer, its reference counter will be incremented during this
		operation.

		This function updates the matched result timestamp.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	key_desc Lookup Key Descriptor of the rule to be queried. This 
		parameter is a union of pointers. The memory address pointed by this
		pointer must be in the task's workspace and must be aligned to 16B boundary.
@Param[in]	key_size Key size in bytes. Should be equal to the key size the
		table was created with except for the following remark:
		 - the key size should be added priority field size (4 Bytes)
		for MFLU tables.
@Param[out]	lookup_result Points to a user preallocated memory to which
		the table lookup result will be written. The structure pointed
		by this pointer must be in the task's workspace and must be
		aligned to 16B boundary.

@Return		0 on success, TABLE_STATUS_MISS on miss.

@Retval		0 Success.
@Retval		#TABLE_STATUS_MISS A match was not found during the lookup
		operation.

@Cautions	In this function the task yields.
@Cautions	This lookup cannot be used for chaining of lookups.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
inline int table_lookup_by_key(enum table_hw_accel_id acc_id,
			uint16_t table_id,
			union table_lookup_key_desc key_desc,
			uint8_t key_size,
			struct table_lookup_result *lookup_result);


/**************************************************************************//**
@Function	table_lookup_by_keyid_default_frame

@Description	Performs a lookup with a predefined key and the default frame.
		\n \n In this lookup process a lookup key will be built
		according to the Key Composition Rule associated with the Key
		ID supplied as a parameter to this functions. Default frame
		header (segment), Parse Result address, and FD address
		parameters are used in the key creation process.

		This function updates the matched result timestamp.

		If the lookup result contains a pointer to Slab/CDMA buffer
		(which has a reference counter) and the lookup result type
		\ref TABLE_RESULT_TYPE_REFERENCE, the pointer reference counter
		will be incremented during this operation.

		Implicit input parameters in Task Defaults: Segment Address,
		Segment Size, Frame Descriptor Address and Parse Results.

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
inline int table_lookup_by_keyid_default_frame(enum table_hw_accel_id acc_id,
					uint16_t table_id,
					uint8_t keyid,
					struct table_lookup_result
					       *lookup_result);


/**************************************************************************//**
@Function	table_lookup_by_keyid

@Description	Performs a lookup with a predefined key, a frame, and user
		metadata.

		In this lookup process a lookup key will be built according to
		the Key Composition Rule associated with the Key ID supplied as
		a parameter to this functions. Frame header (Segment), Parse
		result, FD address and Metadata can explicitly be passed to
		this function for the key creation process.

		This function updates the matched result timestamp.

		If the lookup result contains a pointer to Slab/CDMA buffer
		(which has a reference counter) and the lookup result type
		#TABLE_RESULT_TYPE_REFERENCE, the pointer reference counter
		will be incremented during this operation.

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
@Param[in]	flags Specifies options to this function, please refer to
		\ref FSL_TABLE_LOOKUP_FLAG_DEFINES.
@Param[in]	ndf_params Non defaults inputs to the key creation process.
		See structure documentation for more details. Some of the
		fields in this structures are only valid if appropriate flags
		were set to this function. The structure pointed by this
		pointer must be in the task's workspace and must be aligned to
		16B boundary.
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
int table_lookup_by_keyid(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  uint8_t keyid,
			  uint32_t flags,
			  struct table_lookup_non_default_params
				 *ndf_params,
			  struct table_lookup_result *lookup_result);

#ifdef REV2_RULEID
/**************************************************************************//**
@Function	table_get_next_ruleid

@Description	Retrieves the next Rule ID in a given table.

		This functions may be used by applications to iterate over
		CTLU/MFLU tables. The function returns a Rule ID value which
		exists in the given table and is equal or greater than the 
		value that was passed to it.
		To iterate over a table, application should initially call this
		function with Rule ID = 0. For subsequent calls, application 
		should increment by 1 the Rule ID value that was returned in 
		a previous call and provide it as input to the function.
		If there is no next Rule ID the function returns a miss and
		this may be used by application to terminate the table iteration.
		If the function returns the Rule ID 0xffffffff_ffffffff then this
		Rule ID exists in the table and it is the last Rule ID.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule_id_desc A Rule ID descriptor. The function returns
		a Rule ID which is equal or greater than the Rule ID within
		this descriptor. The structure pointed by this pointer must be in
		the task's workspace and must be aligned to 16B boundary.
@Param[out]	next_rule_id_desc The next Rule ID descriptor. The structure 
		pointed by this pointer must be in the task's workspace and must
		be aligned to 16B boundary.

@Return		0 on success or TABLE_STATUS_MISS if there is no next Rule ID.

@Retval		0 Success.
@Retval		#TABLE_STATUS_MISS In this table there is no Rule ID value which
		is equal or greater than the Rule ID value that was passed as input.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
@Cautions	In case that a rule is added or deleted (by another task) when 
		this function is called, the Rule ID of the added/deleted rule may
		or may not be returned by this function.
*//***************************************************************************/
int table_get_next_ruleid(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_rule_id_desc *rule_id_desc,
			  struct table_rule_id_desc *next_rule_id_desc);


/**************************************************************************//**
@Function	table_get_key_desc

@Description	Retrieves the key descriptor for a given Rule ID and Table ID

		This functions gets a Rule ID and returns the corresponding
		Key descriptor. 

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule_id_desc Rule ID descriptor. The structure pointed by this
		pointer must be in the task's workspace and must be aligned
		to 16B boundary.
@Param[out]	table_key_desc Key descriptor that corresponds to the input 
		Rule ID. The structure pointed by this pointer must be in the task's 
		workspace and must be aligned to 16B boundary.

@Return		0 on success or negative value on error.

@Retval		0 Success.
@Retval		EIO Error, a rule with the same Rule ID is not found
		in the table.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int table_get_key_desc(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_rule_id_desc *rule_id_desc,
			  union table_key_desc *key_desc);


/**************************************************************************//**
@Function	table_rule_replace_by_ruleid

@Description	Replaces a specified rule in the table.
		\n \n The rule's key is not modifiable. Caller to this function
		supplies the Rule ID of the rule to be replaced.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule Table rule, contains Rule ID and rule result to be
		replaced. The structure pointed by this pointer must be in the task's 
		workspace and must be aligned to 16B boundary.
@Param[in, out]	old_res The result of the replaced rule. If null the replaced
		rule's result will not be returned. If not null, structure
		should be allocated by the caller to this function.

@Return		0 on success or negative value on error.

@Retval		0 Success.
@Retval		EIO Error, a rule with the same Rule ID is not found
		in the table.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int table_rule_replace_by_ruleid(enum table_hw_accel_id acc_id,
			  uint16_t table_id,
			  struct table_ruleid_and_result_desc *rule,
			  struct table_result *old_res);


/**************************************************************************//**
@Function	table_rule_delete_by_ruleid

@Description	Deletes a specified rule in the table.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the operation will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule_id_desc Rule ID of the rule to be deleted. The
		structure pointed by this pointer must be in the task's
		workspace and must be aligned to 16B boundary.
@Param[in, out]	result The result of the deleted rule. If null the deleted
		rule's result will not be returned. If not null, structure
		should be allocated by the caller to this function.

@Return		0 on success or negative value on error.

@Retval		0 Success.
@Retval		EIO Error, a rule with the same Rule ID is not found
		in the table.


@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int table_rule_delete_by_ruleid(enum table_hw_accel_id acc_id,
		      uint16_t table_id,
		      struct table_rule_id_desc *rule_id_desc,
		      struct table_result *result);


/**************************************************************************//**
@Function	table_rule_query_by_ruleid

@Description	Queries a rule in the table.
		\n \n This function does not update the matched result
		timestamp.

@Param[in]	acc_id ID of the Hardware Table Accelerator that contains
		the table on which the query will be performed.
@Param[in]	table_id Table ID.
@Param[in]	rule_id_desc Rule ID of the rule to be queried. The
		structure pointed by this pointer must be in the task's
		workspace and must be aligned to 16B boundary.
@Param[out]	result The result of the query. Structure should be allocated
		by the caller to this function.
@Param[out]	timestamp Timestamp of the result. Timestamp is not valid
		unless the rule queried for was created with suitable options
		(Please refer to \ref FSL_TABLE_RULE_OPTIONS for more
		details). Must be allocated by the caller to this function.

@Return		0 on success, #TABLE_STATUS_MISS on miss.

@Retval		0 Success.
@Retval		#TABLE_STATUS_MISS A rule with the same Rule ID is not
		found in the table.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int table_rule_query_by_ruleid(enum table_hw_accel_id acc_id,
		     uint16_t table_id,
		     struct table_rule_id_desc *rule_id_desc,
		     struct table_result *result,
		     uint32_t *timestamp);

#endif //REV2_RULEID


/** @} */ /* end of FSL_TABLE_Functions */
/** @} */ /* end of FSL_TABLE */
/** @} */ /* end of ACCEL */


#endif /* __FSL_TABLE_H */
