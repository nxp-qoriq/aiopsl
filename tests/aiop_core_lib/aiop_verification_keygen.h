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
@File		aiop_verification_keygen.h

@Description	This file contains the AIOP Keygen SW Verification Structures

*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_KEYGEN_H_
#define __AIOP_VERIFICATION_KEYGEN_H_

#include "dplib/fsl_keygen.h"

/** \enum keygen_verif_cmd_type defines the parser verification CMDTYPE
 * field. */
enum keygen_verif_cmd_type {
	KEYGEN_KCR_BUILDER_INIT_VERIF_CMDTYPE,
	KEYGEN_KCR_BUILDER_ADD_CONSTANT_FEC_VERIF_CMDTYPE,
	KEYGEN_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FEC_VERIF_CMDTYPE,
	KEYGEN_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE,
	KEYGEN_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE,
	KEYGEN_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_VERIF_CMDTYPE,
	KEYGEN_KCR_BUILDER_ADD_VALID_FIELD_FEC_VERIF_CMDTYPE,
	KEYGEN_KCR_CREATE_VERIF_CMDTYPE,
	KEYGEN_KCR_REPLACE_VERIF_CMDTYPE,
	KEYGEN_KCR_DELETE_VERIF_CMDTYPE,
	KEYGEN_KCR_QUERY_VERIF_CMDTYPE,
	KEYGEN_GEN_KEY_VERIF_CMDTYPE,
	KEYGEN_GEN_HASH_VERIF_CMDTYPE,
	KEYGEN_KEY_ID_POOL_CREATE_VERIF_CMDTYPE,
	KEYGEN_KCR_BUILDER_ADD_INPUT_VALUE_FEC_VERIF_CMDTYPE
};

/* Keygen Commands Structure identifiers */
#define KCR_LENGTH 128

/*! Init Key Composition Rule Builder Command Structure identifier*/
#define KEYGEN_KCR_BUILDER_INIT_CMD_STR   ((KEYGEN_MODULE << 16) | \
					KEYGEN_KCR_BUILDER_INIT_VERIF_CMDTYPE)

/*! Add "constant" fec to KCR Command Structure identifier*/
#define KEYGEN_KCR_BUILDER_ADD_CONSTANT_FEC_CMD_STR \
	((KEYGEN_MODULE << 16) | \
	KEYGEN_KCR_BUILDER_ADD_CONSTANT_FEC_VERIF_CMDTYPE)

/*! Add "input value" fec to KCR Command Structure identifier*/
#define KEYGEN_KCR_BUILDER_ADD_INPUT_VALUE_FEC_CMD_STR \
	((KEYGEN_MODULE << 16) | \
	KEYGEN_KCR_BUILDER_ADD_INPUT_VALUE_FEC_VERIF_CMDTYPE)

/*! Add "protocol specific field" fec to KCR Command Structure identifier */
#define KEYGEN_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FIELD_FEC_CMD_STR \
	((KEYGEN_MODULE << 16) | \
	KEYGEN_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FEC_VERIF_CMDTYPE)

/*! Add "protocol based generic extract" fec to KCR Command Structure
 * identifier */
#define KEYGEN_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_CMD_STR \
	((KEYGEN_MODULE << 16) | \
	KEYGEN_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE)

/*! Add "generic extract" fec to KCR Command Structure identifier */
#define KEYGEN_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_CMD_STR \
	((KEYGEN_MODULE << 16) | \
	KEYGEN_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_VERIF_CMDTYPE)

/*! Add "lookup result field" fec to KCR Command Structure identifier */
#define KEYGEN_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_CMD_STR \
	((KEYGEN_MODULE << 16) | \
	KEYGEN_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_VERIF_CMDTYPE)

/*! Add "valid field" fec to KCR Command Structure identifier */
#define KEYGEN_KCR_BUILDER_ADD_VALID_FIELD_FEC_CMD_STR \
	((KEYGEN_MODULE << 16) | \
	KEYGEN_KCR_BUILDER_ADD_VALID_FIELD_FEC_VERIF_CMDTYPE)

/*! Key composition rule create Command Structure identifier */
#define KEYGEN_KCR_CREATE_CMD_STR	\
		((KEYGEN_MODULE << 16) | \
		KEYGEN_KCR_CREATE_VERIF_CMDTYPE)

/*! Key composition rule replace Command Structure identifier */
#define KEYGEN_KCR_REPLACE_CMD_STR	\
		((KEYGEN_MODULE << 16) | \
		KEYGEN_KCR_REPLACE_VERIF_CMDTYPE)

/*! Key composition rule delete Command Structure identifier */
#define KEYGEN_KCR_DELETE_CMD_STR	\
		((KEYGEN_MODULE << 16) | \
		KEYGEN_KCR_DELETE_VERIF_CMDTYPE)

/** Key composition rule query Command Structure identifier */
#define KEYGEN_KCR_QUERY_CMD_STR	((KEYGEN_MODULE << 16) | \
				KEYGEN_KCR_QUERY_VERIF_CMDTYPE)

/*! Generate key Command Structure identifier*/
#define KEYGEN_GEN_KEY_CMD_STR	((KEYGEN_MODULE << 16) | \
			KEYGEN_GEN_KEY_VERIF_CMDTYPE)

/*! Generate hash Command Structure identifier */
#define KEYGEN_GEN_HASH_CMD_STR	((KEYGEN_MODULE << 16) | \
		KEYGEN_GEN_HASH_VERIF_CMDTYPE)

/*!< Create Key ID Pool Command Structure identifier */
#define KEYGEN_KEY_ID_POOL_CREATE_CMD_STR  ((KEYGEN_MODULE << 16) | \
			KEYGEN_KEY_ID_POOL_CREATE_VERIF_CMDTYPE)

/** \addtogroup AIOP_Service_Routines_Verification
 *  @{
 */


/**************************************************************************//**
@Group		AIOP_Kegen_SRs_Verification

@Description	AIOP Keygen Verification structures definitions.

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Key Composition Rule Builder Initialization Command structure.

		Includes information needed for Key Composition Rule Builder
		Initialization command verification.
*//***************************************************************************/
struct keygen_kcr_builder_init_command{
	/** KCR Builder Initialization identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;
};


/**************************************************************************//**
@Description	Add Constant FEC to KCR Command structure.

		Includes information needed for Add Constant FEC to
		Key Composition Rule command verification.
*//***************************************************************************/
struct keygen_kcr_builder_add_constant_fec_command{
	/** Add Constant FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** Command returned status */
	int32_t status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;

	/** 1 bytes of user defined constant */
	uint8_t constant;

	/** Number of replications (1-16) of the constant in the key */
	uint8_t num;

	/** 64-bit alignment */
	uint8_t	pad[6];
};

/**************************************************************************//**
@Description	Add Input value FEC to KCR Command structure.

		Includes information needed for Add Input value FEC to
		Key Composition Rule command verification.
*//***************************************************************************/
struct keygen_kcr_builder_add_input_value_fec_command{
	/** Add Input Value FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** a structure of up to 4 bitwise masks from defined offsets */
	struct kcr_builder_fec_mask mask;

	/*! size of extraction */
	uint8_t extract_size;

	/*! Offset of extraction in Input value */
	uint8_t offset;

	/** 64-bit alignment */
	uint8_t	pad1[1];

	/** Command returned status */
	int32_t  status;
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;

	/** 64-bit alignment */
	uint8_t	pad2[4];
};


/**************************************************************************//**
@Description	Add Protocol Specific FEC to KCR Command structure.

		Includes information needed for Add Protocol Specific FEC to
		Key Composition Rule command verification.
*//***************************************************************************/
struct keygen_kcr_builder_add_protocol_specific_field_fec_command{
	/** Add Protocol Specific FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** FEC Mask array.
	If mask is not required for this FEC, num_of_masks should be set to 0.
	*/
	struct kcr_builder_fec_mask mask;

	/** FECID out of \ref kcr_builder_protocol_fecid */
	uint8_t fecid;

	/** 64-bit alignment */
	uint8_t pad[2];

	/** Command returned status */
	int32_t status;
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
	
	/** 64-bit alignment */
	uint8_t	pad2[4];
};

/**************************************************************************//**
@Description	Add Protocol Based Generic Extract FEC to KCR Command structure.

		Includes information needed for Add Protocol Based Generic 
		Extract FEC to Key Composition Rule command verification.
*//***************************************************************************/
struct keygen_kcr_builder_add_protocol_based_generic_extract_fec_command{
	/** Add Generic Extract FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	int32_t kb_ptr;

	/** FEC Mask array.
	If mask is not required for this FEC, num_of_masks should be set to 0.
	*/
	struct kcr_builder_fec_mask mask;

	/** Parse Result Offset */
	uint8_t parse_result_offset;

	/*! size of extraction */
	uint8_t extract_size;

	/*! offset in frame or parse result */
	uint8_t extract_offset;

	/** Command returned status */
	int32_t status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;

	/** 64-bit alignment */
	uint8_t	pad2[4];
	
};


/**************************************************************************//**
@Description	Add Generic Extract FEC to KCR Command structure.

		Includes information needed for Add Generic Extract FEC to
	 	Key Composition Rule command verification.

*//***************************************************************************/
struct keygen_kcr_builder_add_generic_extract_fec_command{
	/** Add Generic Extract FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** a structure of up to 4 bitwise masks from defined offsets */
	struct kcr_builder_fec_mask mask;

	/*! size of extraction */
	uint8_t extract_size;

	/*! offset in frame or parse result */
	uint8_t offset;

	/** 64-bit alignment */
	uint8_t	pad1[1];

	/** Should be one of \ref kcr_builder_gec_source:
	- KEYGEN_KCR_GEC_FRAME (For Generic Extraction from start of frame)
	- KEYGEN_KCR_GEC_PARSE_RES (For Generic Extraction from Parser Result) */
	enum kcr_builder_gec_source gec_source;

	/** Command returned status */
	int32_t  status;
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
};

#ifdef REV2
/**************************************************************************//**
@Description	Add Lookup Result Field FEC to KCR Command structure.

		Includes information needed for Add Lookup Result Field 
		FEC to Key Composition Rule command verification.
*//***************************************************************************/
struct keygen_kcr_builder_add_lookup_result_field_fec_command{
	/** Add Lookup Result Field FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** a structure of up to 4 bitwise masks from defined offsets */
	struct kcr_builder_fec_mask mask;

	/**  Please refer to \ref FSL_KEYGEN_KCR_BUILDER_EXT_LOOKUP_RES_FIELD */
	enum kcr_builder_ext_lookup_res_field extract_field; 

	/*! size of extraction in case of Opaque0 or Opaque1. */
	uint8_t extract_size_in_opaque;

	/*! Offset in Opaque0 or Opaque1 in lookup result */
	uint8_t offset_in_opaque;

	/** Command returned status */
	int32_t  status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
	
	/** 64-bit alignment */
	uint8_t	pad2[4];
};
#endif /*REV2*/

/**************************************************************************//**
@Description	Add Lookup Valid Field FEC to KCR Command structure.

		Includes information needed for Add Valid Field 
		FEC to Key Composition Rule command verification.
*//***************************************************************************/
struct keygen_kcr_builder_add_valid_field_fec_command{
	/** Add Lookup Valid Field FEC to KCR identifier */
	uint32_t opcode;

	/** A pointer to a struct in the workspace, that will contain the Key
	 * Composition Rule vector and its length */
	uint32_t kb_ptr;

	/** Command returned status */
	int32_t  status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
	
	/** bitwise mask */
	uint8_t mask;

	/** 64-bit alignment */
	uint8_t	pad[7];
};

/**************************************************************************//**
@Description	Key Composition Create or Replace Command structure.

		Includes information needed for Key Composition Create 
		or Replace command verification.
*//***************************************************************************/
struct keygen_kcr_create_or_replace_command{
	/** Key Composition Create or Replace identifier */
	uint32_t opcode;

	/**  A pointer to the Key Composition Rule vector in the workspace */
	uint32_t kcr_ptr;

	/** Table Accelerator ID */
	enum keygen_hw_accel_id acc_id;

	/** Command returned status */
	int32_t  status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
	
	/** Key ID */
	uint8_t  key_id;

	/** 64-bit alignment */
	uint8_t	pad[3];
};


/**************************************************************************//**
@Description	Key Composition Delete Command structure.

		Includes information needed for Key Composition Delete
		 command verification.
*//***************************************************************************/
struct keygen_kcr_delete_command{
	/** Key Composition Delete identifier */
	uint32_t opcode;

	/** Command returned status */
	int32_t  status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
	
	/** Table Accelerator ID */
	enum keygen_hw_accel_id acc_id;

	/** Key ID */
	uint8_t key_id;
	
	/** 64-bit alignment */
	uint8_t	pad[7];
};

/**************************************************************************//**
@Description	Key Composition Rule Query Command structure.

		Includes information needed for Key Composition Rule Query
		 command verification.
*//***************************************************************************/
struct keygen_kcr_query_command{
	/** Key Composition Create or Update identifier */
	uint32_t opcode;

	/** Pointer to the Key Composition Rule vector in the Workspace*/
	uint32_t kcr_ptr;

	/** Table Accelerator ID */
	enum keygen_hw_accel_id acc_id;

	/** Key ID */
	uint8_t key_id;

	/** 64-bit alignment */
	uint8_t	pad[3];
};

/**************************************************************************//**
@Description	Generate Key Command structure.

		Includes information needed for Generate Key command
		verification.
*//***************************************************************************/
struct keygen_gen_key_command{
	/** CTLU Generate Key identifier */
	uint32_t opcode;

	uint8_t pad[4];
	
	/** OpaqueIn field for key composition */
	uint64_t opaquein;
	
	/**  A pointer to the Key to be generated in the workspace */
	uint32_t key_ptr;

	/** Command returned status */
	int32_t  status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
	
	/** Table Accelerator ID */
	enum keygen_hw_accel_id acc_id;

	/** Key ID */
	uint8_t key_id;

	/** Output key size*/
	uint8_t key_size;

	/** 64-bit alignment */
	uint8_t	pad2[6];
};

/**************************************************************************//**
@Description	Generate Hash Command structure.

		Includes information needed for Generate Hash command 
		verification.
*//***************************************************************************/
struct keygen_gen_hash_command{
	/** CTLU Generate Hash identifier */
	uint32_t opcode;

	/** Pointer to the input key in the workspace*/
	uint32_t key_ptr;

	/** Hash */
	uint32_t hash;			/*!< Hash Result */

	/** Command returned status */
	int32_t  status;

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t keygen_status_addr;
	
	/** Input key size*/
	uint8_t key_size;

	/** Padding */
	uint8_t	pad[3];
};

/**************************************************************************//**
@Description	Key ID Pool Create Command structure.

		Includes information needed for Create Key ID pool Command
		verification.
*//***************************************************************************/
struct keygen_key_id_pool_create_verif_command {
	uint32_t opcode;
	uint8_t  pad[4];
};

/** @} */ /* end of AIOP_Keygen_SRs_Verification */

/** @}*/ /* end of AIOP_Service_Routines_Verification */


uint16_t aiop_verification_keygen(uint32_t asa_seg_addr);

#endif /* __AIOP_VERIFICATION_KEYGEN_H_ */

