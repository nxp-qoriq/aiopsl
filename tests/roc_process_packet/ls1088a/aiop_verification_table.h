/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		aiop_verification_table.h

@Description	This file contains the AIOP Table SW Verification Structures

*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_TABLE_H_
#define __AIOP_VERIFICATION_TABLE_H_

#include "fsl_table.h"

/** \enum table_verif_cmd_type defines the parser verification CMDTYPE
 * field. */
enum table_verif_cmd_type {
	TABLE_CREATE_VERIF_CMDTYPE,
	TABLE_REPLACE_MISS_RESULT_VERIF_CMDTYPE,
	TABLE_GET_PARAMS_VERIF_CMDTYPE,
	TABLE_GET_MISS_RESULT_VERIF_CMDTYPE,
	TABLE_DELETE_VERIF_CMDTYPE,
	TABLE_RULE_CREATE_VERIF_CMDTYPE,
	TABLE_RULE_CREATE_OR_REPLACE_VERIF_CMDTYPE,
	TABLE_RULE_REPLACE_VERIF_CMDTYPE,
	TABLE_RULE_MODIFY_PRIORITY_VERIF_CMDTYPE,
	TABLE_RULE_QUERY_GET_RESULT_VERIF_CMDTYPE,
	TABLE_RULE_QUERY_GET_KEY_DESC_VERIF_CMDTYPE,
	TABLE_RULE_QUERY_GET_NEXT_ID_CMDTYPE,
	TABLE_RULE_DELETE_VERIF_CMDTYPE,
	TABLE_LOOKUP_BY_KEY_VERIF_CMDTYPE,
	TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_VERIF_CMDTYPE,
	TABLE_LOOKUP_BY_KEYID_VERIF_CMDTYPE,
	TABLE_RULE_REPLACE_BY_KEY_DESC_VERIF_CMDTYPE,
	TABLE_RULE_QUERY_BY_KEY_DESC_VERIF_CMDTYPE,
	TABLE_RULE_DELETE_BY_KEY_DESC_VERIF_CMDTYPE,
	TABLE_QUERY_DEBUG_VERIF_CMDTYPE
};

/* CTLU Commands Structure identifiers */

/** Table Create Command Structure identifier */
#define TABLE_CREATE_CMD_STR		((TABLE_MODULE << 16) | \
					 TABLE_CREATE_VERIF_CMDTYPE)

/** Table Replace Miss Rule Command Structure identifier */
#define TABLE_REPLACE_MISS_RESULT_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_REPLACE_MISS_RESULT_VERIF_CMDTYPE)

/** Table get params Command Structure identifier */
#define TABLE_GET_PARAMS_CMD_STR	((TABLE_MODULE << 16) | \
					 TABLE_GET_PARAMS_VERIF_CMDTYPE)

/** Table get miss result Command Structure identifier */
#define TABLE_GET_MISS_RESULT_CMD_STR	((TABLE_MODULE << 16) | \
					 TABLE_GET_MISS_RESULT_VERIF_CMDTYPE)

/** Table delete Command Structure identifier */
#define TABLE_DELETE_CMD_STR		((TABLE_MODULE << 16) | \
					 TABLE_DELETE_VERIF_CMDTYPE)

/** Table rule create Command Structure identifier */
#define TABLE_RULE_CREATE_CMD_STR	((TABLE_MODULE << 16) | \
					 TABLE_RULE_CREATE_VERIF_CMDTYPE)

/** Table rule create or replace Command Structure identifier */
#define TABLE_RULE_CREATE_OR_REPLACE_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_RULE_CREATE_OR_REPLACE_VERIF_CMDTYPE)

/** Table rule replace Command Structure identifier */
#define TABLE_RULE_REPLACE_CMD_STR	((TABLE_MODULE << 16) | \
					 TABLE_RULE_REPLACE_VERIF_CMDTYPE)

/** Table rule query get result Command Structure identifier */
#define TABLE_RULE_QUERY_GET_RESULT_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_RULE_QUERY_GET_RESULT_VERIF_CMDTYPE)

/** Table rule query get key descriptor Command Structure identifier */
#define TABLE_RULE_QUERY_GET_KEY_DESC_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_RULE_QUERY_GET_KEY_DESC_VERIF_CMDTYPE)

/** Table rule query get next id Command Structure identifier */
#define TABLE_RULE_QUERY_GET_NEXT_ID_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_RULE_QUERY_GET_NEXT_ID_CMDTYPE)

/** Table rule delete Command Structure identifier */
#define TABLE_RULE_DELETE_CMD_STR	((TABLE_MODULE << 16) | \
					 TABLE_RULE_DELETE_VERIF_CMDTYPE)

/** Table lookup by explicit key Command Structure identifier */
#define TABLE_LOOKUP_BY_KEY_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_LOOKUP_BY_KEY_VERIF_CMDTYPE)

/** Table lookup by keyID and default frame Command Structure identifier */
#define TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_CMD_STR	\
	((TABLE_MODULE << 16) | \
	 TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_VERIF_CMDTYPE)

/** Table lookup by keyID Command Structure identifier */
#define TABLE_LOOKUP_BY_KEYID_CMD_STR	((TABLE_MODULE << 16) | \
					 TABLE_LOOKUP_BY_KEYID_VERIF_CMDTYPE)

/** Table rule replace Command Structure identifier */
#define TABLE_RULE_REPLACE_BY_KEY_DESC_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_RULE_REPLACE_BY_KEY_DESC_VERIF_CMDTYPE)

/** Table rule query Command Structure identifier */
#define TABLE_RULE_QUERY_BY_KEY_DESC_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_RULE_QUERY_BY_KEY_DESC_VERIF_CMDTYPE)

/** Table rule delete Command Structure identifier */
#define TABLE_RULE_DELETE_BY_KEY_DESC_CMD_STR \
	((TABLE_MODULE << 16) | TABLE_RULE_DELETE_BY_KEY_DESC_VERIF_CMDTYPE)

/** Table Query Debug Command Structure identifier */
#define TABLE_QUERY_DEBUG_CMD_STR	((TABLE_MODULE << 16) | \
				TABLE_QUERY_DEBUG_VERIF_CMDTYPE)

/** \addtogroup AIOP_Service_Routines_Verification
 *  @{
 */


/**************************************************************************//**
@Group		AIOP_Table_SRs_Verification_MACROS

@Description	AIOP Table Verification MACROS definitions.

@{
*//***************************************************************************/
/* No Flags */
#define TABLE_VERIF_FLAG_NO_FLAGS		0x00000000

/** when set, the verification will pass NULL as a pointer to the verified
 * function instead of queried/replaced/deleted result*/
#define TABLE_VERIF_FLAG_OLD_RESULT_NULL	0x00000001

/** when set, the verification will pass NULL as a pointer to the verified
 * function instead of queried/replaced/deleted options*/
#define TABLE_VERIF_FLAG_OLD_OPTIONS_NULL	0x00000003

/** when set, the verification will pass NULL as a pointer to the verified
 * function instead of queried/replaced/deleted timestamp*/
#define TABLE_VERIF_FLAG_OLD_TIMESTAMP_NULL	0x00000004

/** When set, the verification wrapper will pass 0xFFFF_FFFF_FFFF_FFFF ruleID
* to the called function. To be used only with suitable functions.*/
#define TABLE_VERIF_FLAG_RULE_ID_ALL_ONE 	0x00000002

/** Number of rule_ids to be supported for the verification process */
#define TABLE_VERIF_RULE_ID_ARRAY_SIZE		32

/** Non-existing rule ID */
#define TABLE_VERIF_RULE_ID_ALL_ONE		0xFFFFFFFFFFFFFFFF

/** @} */ /* end of AIOP_Table_SRs_Verification_MACROS */

/**************************************************************************//**
@Group		AIOP_Table_SRs_Verification

@Description	AIOP Table Verification structures definitions.

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Table Create Command structure.

		Includes information needed for Table Create command
		verification.
*//***************************************************************************/

struct table_create_command {

	/** Table Create Command identifier */
	uint32_t opcode;

	/*! Table create parameters pointer to the workspace */
	uint32_t  table_create_params_ptr;

	/** Command returned status */
	int32_t  status;

	/** Command returned Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	Table Update Miss Result Command structure.

		Includes information needed for Table Update Miss Rule command
		verification.
*//***************************************************************************/
struct table_replace_miss_result_command {
	/** CTLU Update Miss Rule Command identifier */
	uint32_t opcode;

	/* Flags for this operation */
	uint32_t flags;

	/** Miss Rule to update.
	The structure to be passed must be one of the following:
	 - \ref aiop_ctlu_result_chaining
	 - \ref aiop_ctlu_result_reference
	 - \ref aiop_ctlu_result_opaque */
	struct table_result miss_rule;

	/** The old miss result */
	struct table_result old_miss_result;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	Table Get Parameters Command structure.

		Includes information needed for Table Get Parameters
		command verification.
*//***************************************************************************/

struct ctlu_table_get_params_command {
	/** CTLU Table Get Parameters Command identifier */
	uint32_t opcode;

	/** Table get params output*/
	struct table_get_params_output table_get_params_out;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	Table Get Miss Rule Command structure.

		Includes information needed for Table Get Miss Rule command
		verification.
*//***************************************************************************/
struct table_get_miss_result_command {
	/** CTLU Get Miss Rule Command identifier */
	uint32_t opcode;

	/** This union includes:
	- struct aiop_ctlu_result_chaining
	- struct aiop_ctlu_result_reference
	- struct aiop_ctlu_result_opaque */
	struct table_result miss_rule;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	CTLU Table Delete Command structure.

		Includes information needed for CTLU Table Delete command
		verification.
*//***************************************************************************/
struct table_delete_command {
	/** CTLU Table Delete Command identifier */
	uint32_t opcode;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Create struct.

		Includes information needed for CTLU Table Rule Create
		command verification.
*//***************************************************************************/
struct table_rule_create_command{
	/** CTLU Table Rule Create identifier */
	uint32_t  opcode;

	/** A pointer to the rule to be added (workspace pointer)*/
	uint32_t  rule_ptr;

	/** Index in the rule ID array to be used as an address input for the
	 table_rule_create rule ID output. */
	int rule_id_index;

	/** Command returned status */
	int32_t   status;

	/** Table ID */
	t_tbl_id  table_id;

	/** Key size */
	uint8_t   key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Create or Replace/Replace by key descriptor
		Command struct.

		Includes information needed for CTLU Table Rule
		Create or Replace/ Replace by key descriptor command
		verification.
*//***************************************************************************/
struct table_rule_create_replace_command{
	/** CTLU Table Rule Create identifier */
	uint32_t opcode;

	/* Flags for this operation */
	uint32_t flags;

	/** Index in the rule ID array to be used as an address input for the
	 table_rule_create_or_replace rule ID output. */
	int rule_id_index;

	/** A pointer to the rule to be added (workspace pointer)*/
	uint32_t rule_ptr;

	/** Command returned status */
	int32_t  status;

	/** Rule's replaced result - valid only if replace occurred */
	struct table_result replaced_result;

	/** Rule's replaced options - valid only if replace occurred and
	 * correct flag was set */
	uint8_t replaced_options;

	/** Rule's old timstamp */
	uint32_t timestamp;

	/** Table ID */
	uint16_t table_id;

	/** Key size */
	uint8_t key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Replace Command struct.

		Includes information needed for CTLU Table Rule
		Create or Replace/ Replace command verification.
*//***************************************************************************/
struct table_rule_replace_command{
	/** CTLU Table Rule Create identifier */
	uint32_t opcode;

	/* Flags for this operation */
	uint32_t flags;

	/** Index in the rule ID array to be used as an address input for the
	 table_rule_create_or_replace rule ID output. */
	int rule_id_index;

	/** Rule's new result */
	struct table_result new_result;

	/** Rule's new options */
	uint8_t new_options;

	/** Rule's replaced result - valid only if replace occurred */
	struct table_result replaced_result;

	/** Rule's replaced options - valid only if replace occurred and
	 * correct flag was set */
	uint8_t replaced_options;

	/** Rule's old timstamp */
	uint32_t timestamp;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Query Get result Command structure.

		Includes information needed for CTLU Table Rule Query Get
		Result command verification.
*//***************************************************************************/
struct table_rule_query_get_result_command{
	/** CTLU Table Rule Delete identifier */
	uint32_t opcode;

	/** Index in the rule ID array of the rule ID to be used as an input for
	 this command */
	int rule_id_index;

	/* Flags for this operation */
	uint32_t flags;

	/** Rule's queried result - valid only if query successful and
	 * correct flag was set */
	struct table_result queried_result;

	/** Rule's queried options - valid only if query successful and
	 * correct flag was set */
	uint8_t queried_options;

	/** Rule's current timstamp */
	uint32_t timestamp;

	/** Command returned status */
	int status;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Query Get Key Descriptor Command structure.

		Includes information needed for CTLU Table Rule Query Get
		Key Descriptor command verification.
*//***************************************************************************/
struct table_rule_query_get_key_desc_command{
	/** CTLU Table Rule Delete identifier */
	uint32_t opcode;

	/** Index in the rule ID array of the rule ID to be used as an input for
	 this command */
	int rule_id_index;

	/* Flags for this operation */
	uint32_t flags;

	/** Command returned status */
	int status;

	/* MFLU Key descriptor */
	union table_key_desc key_desc;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	CTLU Table Rule Query Get Key Descriptor Command structure.

		Includes information needed for CTLU Table Rule Query Get
		Next ID  command verification.
*//***************************************************************************/
struct table_rule_query_get_next_id_desc_command {
	/** CTLU Table Rule Query Get Next ID identifier */
	uint32_t opcode;

	/** Index in the rule ID array of the rule ID to be used as an input for
	 * this command */
	uint64_t rule_id_index;

	/** Command returned status */
	int status;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;

	/** Index in the rule ID array of the rule ID returned
	 * by this command */
	uint64_t next_rule_id_index;
};

/**************************************************************************//**
@Description	CTLU Table Rule Delete Command structure.

		Includes information needed for CTLU Table Rule Delete
		command verification.
*//***************************************************************************/
struct table_rule_delete_command{
	/** CTLU Table Rule Delete identifier */
	uint32_t opcode;

	/** Index in the rule ID array of the rule ID to be used as an input for
	 this command */
	int rule_id_index;

	/* Flags for this operation */
	uint32_t flags;

	/** Rule's deleted result - valid only if delete occurred and
	 * correct flag was set */
	struct table_result deleted_result;

	/** Rule's deleted options - valid only if delete occurred and
	 * correct flag was set */
	uint8_t deleted_options;

	/** Rule's old timstamp */
	uint32_t timestamp;
	
	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;


	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Lookup by Explicit Key Command structure.

		Includes information needed for CTLU Table Lookup by Explicit
		Key command verification.
*//***************************************************************************/
struct table_lookup_by_key_command{
	/** CTLU Lookup by Key identifier */
	uint32_t opcode;

	/** The structure returned to the caller upon a successful lookup */
	struct table_lookup_result lookup_result;

	/** Input Lookup Key Descriptor pointer to the workspace */
	union table_lookup_key_desc key_desc;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Input key size*/
	uint8_t key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;

};

/**************************************************************************//**
@Description	CTLU Table Lookup by KeyID Default Frame Command structure.

		Includes information needed for CTLU Table Lookup by KeyID
		Default Frame command verification.
*//***************************************************************************/
struct table_lookup_by_keyid_default_frame_command{
	/** CTLU Lookup by KeyID identifier */
	uint32_t opcode;

	/** The structure returned to the caller upon a successful lookup */
	struct table_lookup_result lookup_result;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Key ID */
	uint8_t key_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	CTLU Table Lookup by KeyID Command structure.

		Includes information needed for CTLU Table Lookup by KeyID
		command verification.
*//***************************************************************************/
struct table_lookup_by_keyid_command{
	/** CTLU Lookup by KeyID identifier */
	uint32_t opcode;

	/** The structure returned to the caller upon a successful lookup */
	struct table_lookup_result lookup_result;

	/** Command returned status */
	int32_t  status;

	/** Flags to this command */
	uint32_t flags;

	/** Non Default Parameters for this command */
	struct table_lookup_non_default_params ndf_params;

	/** Table ID */
	uint16_t table_id;

	/** Key ID */
	uint8_t key_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Query by Key Descriptor Command structure.

		Includes information needed for CTLU Table Rule Query
		by Key Descriptor command verification.
*//***************************************************************************/
struct table_rule_query_by_key_desc_command{
	/** CTLU Table Rule Delete identifier */
	uint32_t opcode;

	/** The structure returned to the caller upon a successful Query */
	struct table_result result;

	/** Input Key Descriptor pointer to the workspace */
	uint32_t key_desc_ptr;

	/** Command returned status */
	int32_t  status;

	/** Timestamp of the matched rule*/
	uint32_t timestamp;

	/** Priority */
	uint32_t priority;

	/** Index in the rule ID array to be used as an address input for the
	 table_rule_create rule ID output. */
	int rule_id_index;

	/** Table ID */
	uint16_t table_id;

	/** Input key size*/
	uint8_t key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	CTLU Table Rule Delete Command structure.

		Includes information needed for CTLU Table Rule Delete
		command verification.
*//***************************************************************************/
struct table_rule_delete_by_key_desc_command{
	/** CTLU Table Rule Delete identifier */
	uint32_t opcode;

	/* Flags for this operation */
	uint32_t flags;

	/** Rule's old result */
	struct table_result old_res;

	/** Input Key Descriptor pointer to the workspace */
	uint32_t key_desc_ptr;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Input key size*/
	uint8_t key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	Table Query Debug Command structure.

		Includes information needed for Table Query Debug command 
		verification.

*//***************************************************************************/
struct table_query_debug_command{
	/** CTLU Generate Hash identifier */
	uint32_t opcode;

	/** Table Parameters output pointer in the workspace */
	uint32_t output_ptr;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/** @} */ /* end of AIOP_Table_SRs_Verification */

/** @}*/ /* end of AIOP_Service_Routines_Verification */

uint16_t aiop_verification_table(uint32_t asa_seg_addr);

#endif /* __AIOP_VERIFICATION_TABLE_H_ */

