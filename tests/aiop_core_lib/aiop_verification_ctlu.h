/**************************************************************************//**
@File		aiop_verification_ctlu.h

@Description	This file contains the AIOP CTLU SW Verification Structures

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_CTLU_H_
#define __AIOP_VERIFICATION_CTLU_H_

#include "dplib/fsl_table.h"

/** \enum ctlu_verif_cmd_type defines the parser verification CMDTYPE
 * field. */
enum ctlu_verif_cmd_type {
	TABLE_CREATE_VERIF_CMDTYPE = 0,
	TABLE_REPLACE_MISS_RESULT_VERIF_CMDTYPE,
	TABLE_GET_PARAMS_VERIF_CMDTYPE,
	TABLE_GET_MISS_RESULT_VERIF_CMDTYPE,
	TABLE_DELETE_VERIF_CMDTYPE,
	TABLE_RULE_CREATE_VERIF_CMDTYPE,
	TABLE_RULE_CREATE_OR_REPLACE_VERIF_CMDTYPE,
	TABLE_RULE_REPLACE_VERIF_CMDTYPE,
	TABLE_RULE_DELETE_VERIF_CMDTYPE,
	TABLE_LOOKUP_BY_KEYID_VERIF_CMDTYPE,
	TABLE_LOOKUP_BY_KEY_VERIF_CMDTYPE,
	CTLU_KCR_BUILDER_INIT_VERIF_CMDTYPE,
	CTLU_KCR_BUILDER_ADD_CONSTANT_FEC_VERIF_CMDTYPE,
	CTLU_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FEC_VERIF_CMDTYPE,
	CTLU_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE,
	CTLU_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE,
	CTLU_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_VERIF_CMDTYPE,
	CTLU_KCR_BUILDER_ADD_VALID_FIELD_FEC_VERIF_CMDTYPE,
	CTLU_KCR_CREATE_VERIF_CMDTYPE,
	CTLU_KCR_REPLACE_VERIF_CMDTYPE,
	CTLU_KCR_DELETE_VERIF_CMDTYPE,
	CTLU_KCR_QUERY_VERIF_CMDTYPE,
	CTLU_GEN_KEY_VERIF_CMDTYPE,
	CTLU_GEN_HASH_VERIF_CMDTYPE,
	TABLE_QUERY_DEBUG_VERIF_CMDTYPE,
	CTLU_KEY_ID_POOL_CREATE_VERIF_CMDTYPE
};

/* CTLU Commands Structure identifiers */

/** Table Create Command Structure identifier */
#define TABLE_CREATE_CMD_STR	((TABLE_MODULE << 16) | \
				  TABLE_CREATE_VERIF_CMDTYPE)

/** Table Replace Miss Rule Command Structure identifier */
#define TABLE_REPLACE_MISS_RESULT_CMD_STR	((TABLE_MODULE << 16) | \
				TABLE_REPLACE_MISS_RESULT_VERIF_CMDTYPE)

/** Table get params Command Structure identifier */
#define TABLE_GET_PARAMS_CMD_STR	((TABLE_MODULE << 16) | \
					TABLE_GET_PARAMS_VERIF_CMDTYPE)

/** Table get miss result Command Structure identifier */
#define TABLE_GET_MISS_RESULT_CMD_STR	((TABLE_MODULE << 16) | \
					TABLE_GET_MISS_RESULT_VERIF_CMDTYPE)

/** Table delete Command Structure identifier */
#define TABLE_DELETE_CMD_STR	((TABLE_MODULE << 16) | \
				  TABLE_DELETE_VERIF_CMDTYPE)

/** Table rule create Command Structure identifier */
#define TABLE_RULE_CREATE_CMD_STR	((TABLE_MODULE << 16) | \
					  TABLE_RULE_CREATE_VERIF_CMDTYPE)

/** Table rule create or replace Command Structure identifier */
#define TABLE_RULE_CREATE_OR_REPLACE_CMD_STR	((TABLE_MODULE << 16) | \
				TABLE_RULE_CREATE_OR_REPLACE_VERIF_CMDTYPE)

/** Table rule replace Command Structure identifier */
#define TABLE_RULE_REPLACE_CMD_STR	((TABLE_MODULE << 16) | \
					TABLE_RULE_REPLACE_VERIF_CMDTYPE)

/** Table rule delete Command Structure identifier */
#define TABLE_RULE_DELETE_CMD_STR	((TABLE_MODULE << 16) | \
					TABLE_RULE_DELETE_VERIF_CMDTYPE)

/** Table lookup by keyID */
#define TABLE_LOOKUP_BY_KEYID_CMD_STR	((TABLE_MODULE << 16) | \
				TABLE_LOOKUP_BY_KEYID_VERIF_CMDTYPE)

/** Table lookup by explicit key */	
#define TABLE_LOOKUP_BY_KEY_CMD_STR		((TABLE_MODULE << 16) | \
					TABLE_LOOKUP_BY_KEY_VERIF_CMDTYPE)

/*! Init Key Composition Rule Builder Command Structure identifier*/
#define CTLU_KCR_BUILDER_INIT_CMD_STR   ((TABLE_ACCEL_ID_CTLU << 16) | \
					CTLU_KCR_BUILDER_INIT_VERIF_CMDTYPE)

/*! Add "constant" fec to KCR Command Structure identifier*/
#define CTLU_KCR_BUILDER_ADD_CONSTANT_FEC_CMD_STR \
	((TABLE_ACCEL_ID_CTLU << 16) | \
	CTLU_KCR_BUILDER_ADD_CONSTANT_FEC_VERIF_CMDTYPE)

/*! Add "protocol specific field" fec to KCR Command Structure identifier */
#define CTLU_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FIELD_FEC_CMD_STR \
	((TABLE_ACCEL_ID_CTLU << 16) | \
	CTLU_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FEC_VERIF_CMDTYPE)

/*! Add "protocol based generic extract" fec to KCR Command Structure
 * identifier */
#define CTLU_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_CMD_STR \
	((TABLE_ACCEL_ID_CTLU << 16) | \
	CTLU_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE)

/*! Add "generic extract" fec to KCR Command Structure identifier */
#define CTLU_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_CMD_STR \
	((TABLE_ACCEL_ID_CTLU << 16) | \
	CTLU_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE)

/*! Add "lookup result field" fec to KCR Command Structure identifier */
#define CTLU_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_CMD_STR \
	((TABLE_ACCEL_ID_CTLU << 16) | \
	CTLU_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_VERIF_CMDTYPE)

/*! Add "valid field" fec to KCR Command Structure identifier */
#define CTLU_KCR_BUILDER_ADD_VALID_FIELD_FEC_CMD_STR \
	((TABLE_ACCEL_ID_CTLU << 16) | \
	CTLU_KCR_BUILDER_ADD_VALID_FIELD_FEC_VERIF_CMDTYPE)

/*! Key composition rule create Command Structure identifier */
#define CTLU_KCR_CREATE_CMD_STR	\
		((TABLE_ACCEL_ID_CTLU << 16) | \
		CTLU_KCR_CREATE_VERIF_CMDTYPE)

/*! Key composition rule replace Command Structure identifier */
#define CTLU_KCR_REPLACE_CMD_STR	\
		((TABLE_ACCEL_ID_CTLU << 16) | \
		CTLU_KCR_REPLACE_VERIF_CMDTYPE)

/*! Key composition rule delete Command Structure identifier */
#define CTLU_KCR_DELETE_CMD_STR	\
		((TABLE_ACCEL_ID_CTLU << 16) | \
		CTLU_KCR_DELETE_VERIF_CMDTYPE)

/** Key composition rule query Command Structure identifier */
#define CTLU_KCR_QUERY_CMD_STR	((TABLE_ACCEL_ID_CTLU << 16) | \
				CTLU_KCR_QUERY_VERIF_CMDTYPE)

/*! Generate key Command Structure identifier*/
#define CTLU_GEN_KEY_CMD_STR	((TABLE_ACCEL_ID_CTLU << 16) | \
				CTLU_GEN_KEY_VERIF_CMDTYPE)

/*! Generate hash Command Structure identifier */
#define CTLU_GEN_HASH_CMD_STR	((TABLE_ACCEL_ID_CTLU << 16) | \
				CTLU_GEN_HASH_VERIF_CMDTYPE)

/** Table Query Debug Command Structure identifier */
#define TABLE_QUERY_DEBUG_CMD_STR	((TABLE_MODULE << 16) | \
				TABLE_QUERY_DEBUG_VERIF_CMDTYPE)

/*!< Create Key ID Pool Command Structure identifier */
#define CTLU_KEY_ID_POOL_CREATE_CMD_STR  ((TABLE_ACCEL_ID_CTLU << 16) | \
				CTLU_KEY_ID_POOL_CREATE_VERIF_CMDTYPE)

/** \addtogroup AIOP_Service_Routines_Verification
 *  @{
 */


/**************************************************************************//**
@Group		AIOP_CTLU_SRs_Verification

@Description	AIOP CTLU Verification structures definitions.

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Table Parameters Query Output Message Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_table_params_query_output_message {
	/** Table Type, Includes IEX, MRES & AGT bits */
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

	/** Current Number of rules */
	uint32_t current_rules;		

	/** Current Number of entries */
	uint32_t current_entries;	

	uint64_t  reserved0;

	uint64_t  reserved1;

	/** Miss Result */
	struct ctlu_table_rule_result miss_lookup_fcv;
					
};
#pragma pack(pop)

/**************************************************************************//**
@Description	CTLU Create Table Command structure.

		Includes information needed for CTLU Create Table command
		verification.
*//***************************************************************************/

struct ctlu_table_create_command {

	/** CTLU Create Table Command identifier */
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
@Description	CTLU Update Miss Rule Command structure.

		Includes information needed for CTLU Update Miss Rule command
		verification.
*//***************************************************************************/
struct ctlu_update_miss_rule_command {
	/** Miss Rule to update.
	The structure to be passed must be one of the following:
	 - \ref aiop_ctlu_result_chaining
	 - \ref aiop_ctlu_result_reference
	 - \ref aiop_ctlu_result_opaque */
	struct ctlu_table_rule_result miss_rule;

	/** The old miss result */
	struct ctlu_table_rule_result old_miss_result;

	/** CTLU Update Miss Rule Command identifier */
	uint32_t opcode;

	/** Command returned status. */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	CTLU Table Get Parameters Command structure.

		Includes information needed for CTLU Table Get Parameters
		command verification.
*//***************************************************************************/

struct ctlu_table_get_params_command {
	/** CTLU Table Get Parameters Command identifier */
	uint32_t opcode;

	/*! Table get params output*/
	struct ctlu_table_get_params_output table_get_params_out;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Get Miss Rule Command structure.

		Includes information needed for CTLU Get Miss Rule command
		verification.
*//***************************************************************************/
struct ctlu_get_miss_rule_command {
	/** CTLU Get Miss Rule Command identifier */
	uint32_t opcode;

	/** This union includes:
	- struct aiop_ctlu_result_chaining
	- struct aiop_ctlu_result_reference
	- struct aiop_ctlu_result_opaque */
	struct ctlu_table_rule_result miss_rule;

	/** Command returned status */
	int32_t  status;

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
struct ctlu_table_delete_command {
	/** CTLU Table Delete Command identifier */
	uint32_t opcode;

	/** Command returned status */
	int32_t  status;

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
struct ctlu_table_rule_create_command{
	/** CTLU Table Rule Create identifier */
	uint32_t opcode;

	/** A pointer to the rule to be added (workspace pointer)*/
	uint32_t rule_ptr;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Key size */
	uint8_t key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Create or Replace/Replace Command struct.

		Includes information needed for CTLU Table Rule
		Create or Replace/ Replace command verification.
*//***************************************************************************/
struct ctlu_table_rule_create_replace_command{
	/** Rule's old result - valid only if replace occurred */
	struct ctlu_table_rule_result old_res;

	/** CTLU Table Rule Create identifier */
	uint32_t opcode;

	/** A pointer to the rule to be added (workspace pointer)*/
	uint32_t rule_ptr;

	/** Command returned status */
	int32_t  status;

	/** Table ID */
	uint16_t table_id;

	/** Key size */
	uint8_t key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	CTLU Table Rule Delete Command structure.

		Includes information needed for CTLU Table Rule Delete
		command verification.
*//***************************************************************************/
struct ctlu_table_rule_delete_command{
	/** Rule's old result */
	struct ctlu_table_rule_result old_res;

	/** CTLU Table Rule Delete identifier */
	uint32_t opcode;

	/** Input Key pointer to the workspace */
	uint32_t key_ptr;

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
@Description	CTLU Table Lookup by KeyID Command structure.

		Includes information needed for CTLU Table Lookup by KeyID
		command verification.
*//***************************************************************************/
struct ctlu_table_lookup_by_keyid_command{
	/** CTLU Lookup by KeyID identifier */
	uint32_t opcode;

	/** The structure returned to the caller upon a successful lookup */
	struct ctlu_lookup_result lookup_result;

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
@Description	CTLU Table Lookup by Explicit Key Command structure.

		Includes information needed for CTLU Table Lookup by Explicit
		Key command verification.
*//***************************************************************************/
struct ctlu_table_lookup_by_key_command{
	/** CTLU Lookup by Key identifier */
	uint32_t opcode;

	/** The structure returned to the caller upon a successful lookup */
	struct ctlu_lookup_result lookup_result;

	/** A pointer to the key in the workspace */
	uint32_t key_ptr;

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
@Description	Key Composition Rule Builder Initialization Command structure.

		Includes information needed for Key Composition Rule Builder
		Initialization command verification.
*//***************************************************************************/
struct ctlu_kcr_builder_init_command{
	/** KCR Builder Initialization identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** Command returned status */
	int32_t status;

	/** 64-bit alignment */
	uint8_t	pad[4];
};


/**************************************************************************//**
@Description	Add Constant FEC to KCR Command structure.

		Includes information needed for Add Constant FEC to
		Key Composition Rule command verification.
*//***************************************************************************/
struct ctlu_kcr_builder_add_constant_fec_command{
	/** Add Constant FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** Command returned status */
	int32_t status;

	/** 1 bytes of user defined constant */
	uint8_t constant;

	/** Number of replications (1-16) of the constant in the key */
	uint8_t num;

	/** 64-bit alignment */
	uint8_t	pad[2];
};

/**************************************************************************//**
@Description	Add Protocol Specific FEC to KCR Command structure.

		Includes information needed for Add Protocol Specific FEC to
		Key Composition Rule command verification.
*//***************************************************************************/
struct ctlu_kcr_builder_add_protocol_specific_field_fec_command{
	/** Add Protocol Specific FEC to KCR identifier */
	uint32_t opcode;

	/** FEC Mask array.
	If mask is not required for this FEC, num_of_masks should be set to 0.
	*/
	struct ctlu_kcr_builder_fec_mask mask;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** Command returned status */
	int32_t status;

	/** FECID out of \ref kcr_builder_protocol_fecid */
	uint8_t fecid;

	/** 64-bit alignment */
	uint8_t pad[7];
};

/**************************************************************************//**
@Description	Add Protocol Based Generic Extract FEC to KCR Command structure.

		Includes information needed for Add Protocol Based Generic 
		Extract FEC to Key Composition Rule command verification.
*//***************************************************************************/
struct ctlu_kcr_builder_add_protocol_based_generic_extract_fec_command{
	/** Add Generic Extract FEC to KCR identifier */
	uint32_t opcode;

	/** FEC Mask array.
	If mask is not required for this FEC, num_of_masks should be set to 0.
	*/
	struct ctlu_kcr_builder_fec_mask mask;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	int32_t kb_ptr;

	/** Command returned status */
	int32_t status;

	/** Parse Result Offset */
	uint8_t parse_result_offset;

	/*! size of extraction */
	uint8_t extract_size;

	/*! offset in frame or parse result */
	uint8_t extract_offset;

	/** 64-bit alignment */
	uint8_t	pad1[5];
};


/**************************************************************************//**
@Description	Add Generic Extract FEC to KCR Command structure.

		Includes information needed for Add Generic Extract FEC to
	 	Key Composition Rule command verification.

*//***************************************************************************/
struct ctlu_kcr_builder_add_generic_extract_fec_command{
	/** Add Generic Extract FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** a structure of up to 4 bitwise masks from defined offsets */
	struct ctlu_kcr_builder_fec_mask mask;

	/** Should be one of \ref FSL_CTLU_KCR_BUILDER_GEC_FLAGS:
	- CTLU_KCR_GEC_FRAME (For Generic Extraction from start	of frame)
	- CTLU_KCR_GEC_PARSE_RES (For Generic Extraction from Parser Result) */
	uint32_t flags;

	/** Command returned status */
	int32_t  status;

	/*! size of extraction */
	uint8_t extract_size;

	/*! offset in frame or parse result */
	uint8_t offset;

	/** 64-bit alignment */
	uint8_t	pad1[2];
};

/**************************************************************************//**
@Description	Add Lookup Result Field FEC to KCR Command structure.

		Includes information needed for Add Lookup Result Field 
		FEC to Key Composition Rule command verification.
*//***************************************************************************/
struct ctlu_kcr_builder_add_lookup_result_field_fec_command{
	/** Add Lookup Result Field FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** a structure of up to 4 bitwise masks from defined offsets */
	struct ctlu_kcr_builder_fec_mask mask;

	/** Command returned status */
	int32_t  status;

	/**  Please refer to \ref FSL_CTLU_KCR_BUILDER_EXT_LOOKUP_RES_FIELD */
	uint8_t extract_field; 

	/*! size of extraction in case of Opaque0 or Opaque1. */
	uint8_t extract_size_in_opaque;

	/*! Offset in Opaque0 or Opaque1 in lookup result */
	uint8_t offset_in_opaque;

	/** 64-bit alignment */
	uint8_t	pad1[5];
};

/**************************************************************************//**
@Description	Add Lookup Valid Field FEC to KCR Command structure.

		Includes information needed for Add Valid Field 
		FEC to Key Composition Rule command verification.
*//***************************************************************************/
struct ctlu_kcr_builder_add_valid_field_fec_command{
	/** Add Lookup Valid Field FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** Command returned status */
	int32_t  status;

	/** bitwise mask */
	uint8_t mask;

	/** 64-bit alignment */
	uint8_t	pad[3];
};

/**************************************************************************//**
@Description	Key Composition Create or Replace Command structure.

		Includes information needed for Key Composition Create 
		or Replace command verification.
*//***************************************************************************/
struct ctlu_kcr_create_or_replace_command{
	/** Key Composition Create or Replace identifier */
	uint32_t opcode;

	/** Command returned status */
	int32_t  status;

	/**  A pointer to the Key Composition Rule vector in the workspace */
	uint32_t kcr_ptr;

	/** Key ID */
	uint8_t  key_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};


/**************************************************************************//**
@Description	Key Composition Delete Command structure.

		Includes information needed for Key Composition Delete
		 command verification.
*//***************************************************************************/
struct ctlu_kcr_delete_command{
	/** Key Composition Delete identifier */
	uint32_t opcode;

	/** Command returned status */
	int32_t  status;

	/** Key ID */
	uint8_t key_id;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	Key Composition Rule Query Command structure.

		Includes information needed for Key Composition Rule Query
		 command verification.
*//***************************************************************************/
struct ctlu_kcr_query_command{
	/** Key Composition Create or Update identifier */
	uint32_t opcode;

	/** Command returned status */
	int32_t status;

	/** Pointer to the Key Composition Rule vector in the Workspace*/
	uint32_t kcr_ptr;

	/** Key ID */
	uint8_t key_id;

	/** Key Composition Rule size */
	uint8_t size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	Generate Key Command structure.

		Includes information needed for Generate Key command
		verification.
*//***************************************************************************/
struct ctlu_gen_key_command{
	/** CTLU Generate Key identifier */
	uint32_t opcode;

	/**  A pointer to the Key to be generated in the workspace */
	uint32_t key_ptr;

	/** Command returned status */
	int32_t  status;

	/** Key ID */
	uint8_t key_id;

	/** Output key size*/
	uint8_t key_size;

	/** Table Accelerator ID */
	enum table_hw_accel_id acc_id;
};

/**************************************************************************//**
@Description	Generate Hash Command structure.

		Includes information needed for Generate Hash command 
		verification.
*//***************************************************************************/
struct ctlu_gen_hash_command{
	/** CTLU Generate Hash identifier */
	uint32_t opcode;

	/** Pointer to the input key in the workspace*/
	uint32_t key_ptr;

	/** Hash */
	uint32_t hash;			/*!< Hash Result */

	/** Command returned status */
	int32_t  status;

	/** Input key size*/
	uint8_t key_size;

	/** Padding */
	uint8_t	pad[7];
};

/**************************************************************************//**
@Description	Table Query Debug Command structure.

		Includes information needed for Table Query Debug command 
		verification.

*//***************************************************************************/
struct ctlu_table_query_debug_command{
	uint32_t opcode; 		/*!< CTLU Generate Hash identifier */
	uint32_t output_ptr; /*!< Table Parameters output pointer in the workspace */
	int32_t  status;		/*!< Command returned status */
	uint16_t table_id;		/*!< Table ID */
	uint8_t	pad[2];			/*!< 64-bit alignment */
};

/**************************************************************************//**
@Description	Key ID Pool Create Command structure.

		Includes information needed for Create Key ID pool Command
		verification.
*//***************************************************************************/
struct ctlu_key_id_pool_create_verif_command {
	uint32_t opcode;
	int32_t  status;
};

/** @} */ /* end of AIOP_CTLU_SRs_Verification */

/** @}*/ /* end of AIOP_Service_Routines_Verification */

int32_t table_query_debug(uint16_t table_id,
			       struct ctlu_table_params_query_output_message 
			       *output);

uint16_t aiop_verification_ctlu(uint32_t asa_seg_addr);

#endif /* __AIOP_VERIFICATION_CTLU_H_ */

