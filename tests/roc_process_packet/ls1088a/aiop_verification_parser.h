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
@File		aiop_verification_parser.h

@Description	This file contains the AIOP PARSER SW Verification Structures

*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_PARSER_H_
#define __AIOP_VERIFICATION_PARSER_H_

#include "fsl_ldpaa.h"
#include "fsl_parser.h"


/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_PARSER_SRs_Verification

 @Description	AIOP PARSER Verification structures definitions.

 @{
*//***************************************************************************/

/** \enum parser_verif_cmd_type defines the parser verification CMDTYPE
 * field. */
enum parser_verif_cmd_type {
	PARSER_PRP_CREATE_VERIF_CMDTYPE = 0,
	PARSER_PRP_DELETE_VERIF_CMDTYPE,
	PARSER_PRP_QUERY_VERIF_CMDTYPE,
	PARSER_PRP_REPLACE_VERIF_CMDTYPE,	
	PARSER_GEN_PARSE_RES_VERIF_CMDTYPE,
	PARSER_GEN_PARSE_RES_EXP_VERIF_CMDTYPE,
	PARSER_PRP_ID_POOL_CREATE_VERIF_CMDTYPE,
	PARSER_INIT_FOR_VERIF_CMDTYPE,
	PARSER_MACROS_VERIF_CMDTYPE,
	PARSER_GEN_PARSE_RES_VERIF_CHECKSUM_CMDTYPE,
	PARSER_INIT_GROSS_VERIF_CMDTYPE,
	PARSER_SET_PRPID_HXS_VERIF_CMDTYPE,
	PARSER_SET_FRAME_LENGTH_VERIF_CMDTYPE,
	PARSER_GEN_PARSE_RES_BASIC_VERIF_CMDTYPE
};

#define PARSER_PRP_CREATE_STR  ((PARSE_MODULE << 16) | \
				PARSER_PRP_CREATE_VERIF_CMDTYPE)

#define PARSER_PRP_DELETE_STR  ((PARSE_MODULE << 16) | \
				PARSER_PRP_DELETE_VERIF_CMDTYPE)

#define PARSER_PRP_QUERY_STR  ((PARSE_MODULE << 16) | \
				PARSER_PRP_QUERY_VERIF_CMDTYPE)

#define PARSER_PRP_REPLACE_STR  ((PARSE_MODULE << 16) | \
				PARSER_PRP_REPLACE_VERIF_CMDTYPE)

#define PARSER_GEN_PARSE_RES_STR  ((PARSE_MODULE << 16) | \
				PARSER_GEN_PARSE_RES_VERIF_CMDTYPE)

#define PARSER_GEN_PARSE_RES_EXP_STR  ((PARSE_MODULE << 16) | \
					PARSER_GEN_PARSE_RES_EXP_VERIF_CMDTYPE)

#define PARSER_PRP_ID_POOL_CREATE_STR ((PARSE_MODULE << 16) | \
					PARSER_PRP_ID_POOL_CREATE_VERIF_CMDTYPE)

#define PARSER_INIT_FOR_VERIF_STR ((PARSE_MODULE << 16) | \
					PARSER_INIT_FOR_VERIF_CMDTYPE)

#define PARSER_MACROS_STR ((PARSE_MODULE << 16) | \
					PARSER_MACROS_VERIF_CMDTYPE)

#define PARSER_GEN_PARSE_RES_CHECKSUM_STR ((PARSE_MODULE << 16) | \
					PARSER_GEN_PARSE_RES_VERIF_CHECKSUM_CMDTYPE)

#define PARSER_GEN_INIT_GROSS_STR ((PARSE_MODULE << 16) | \
				PARSER_INIT_GROSS_VERIF_CMDTYPE)

#define PARSER_SET_PRPID_HXS_STR ((PARSE_MODULE << 16) | \
		PARSER_SET_PRPID_HXS_VERIF_CMDTYPE)

#define PARSER_SET_FRAME_LENGTH_STR ((PARSE_MODULE << 16) | \
		PARSER_SET_FRAME_LENGTH_VERIF_CMDTYPE)

#define  PARSER_GEN_PARSE_RES_BASIC_STR ((PARSE_MODULE << 16) | \
		PARSER_GEN_PARSE_RES_BASIC_VERIF_CMDTYPE)


/**************************************************************************//**
@Description	Software Parse Result  structure.
		
*//***************************************************************************/
struct sw_parse_result{
	uint8_t vxlan;
	uint8_t esp;
	uint8_t ike;
};

/**************************************************************************//**
@Description	Parser verification init gross running sum Command structure.

		This command inits the gross running sum field in parse profile result
		
*//***************************************************************************/
struct parser_init_gross_verif_command {
	uint32_t	opcode;
	uint8_t 	pad[4];
};

/**************************************************************************//**
@Description	Parser verification set frame length Command structure.

		This command set frame length in FD field. This command needs to be
		called once before creating frame truncation.
*//***************************************************************************/
struct parser_set_frame_length_command {
	uint32_t	opcode;
	uint16_t	frame_length;
	uint8_t 	pad[2];
};

/**************************************************************************//**
@Description	Parser verification init Command structure.

		This command inits the parse profile, receives prpid and updates
		the prpid and hxs in the task defaults. This command needs to be
		called once in the beginning of the test which requires running
		parser.
*//***************************************************************************/
struct parser_init_verif_command {
	uint32_t	opcode;
	uint16_t	parser_starting_hxs;
	uint8_t		prpid;
	uint8_t 	pad;
};
/**************************************************************************//**
@Description	Parser Profile Create Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_create_verif_command {
	uint32_t opcode;
	int32_t  status;
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t parser_status_addr;
	uint32_t parse_profile;
	uint8_t  prpid;
	uint8_t  pad[7];
};

/**************************************************************************//**
@Description	Parser Profile Delete Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_delete_verif_command {
	uint32_t opcode;
	int32_t  status;
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t parser_status_addr;
	uint8_t  prpid;
	uint8_t  pad[3];
};


/**************************************************************************//**
@Description	Parser Profile Query Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_query_verif_command {
	uint32_t opcode;
	struct parse_profile_input parse_profile;
	uint8_t  prpid;
	uint8_t  pad[7];
};

/**************************************************************************//**
@Description	Parser Profile Replace Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_replace_verif_command {
	uint32_t opcode;
	uint32_t parse_profile;
	uint8_t  prpid;
	uint8_t  pad[7];
};

/**************************************************************************//**
@Description	Parser Generate Parser Results Checksum Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_checksum_verif_command {
	uint32_t	opcode;
	int32_t		status;
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t 	parser_status_addr;
	uint16_t	hxs;
	uint16_t	l3_checksum;
	uint16_t	l4_checksum;
	uint8_t		offset;
	uint8_t		pad[5];
};

/**************************************************************************//**
@Description	Parser Generate Parser Results Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_verif_command {
	uint32_t opcode;
	int32_t  status;
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t parser_status_addr;
	uint8_t prpid;
	uint8_t  flags;
	uint8_t  pad[2];
};

/**************************************************************************//**
@Description	Parser Generate Parser Results (EXP) Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_exp_verif_command {
	uint32_t	opcode;
	int32_t		status;
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t 	parser_status_addr;
	uint16_t	hxs;
	uint8_t		flags;
	uint8_t		offset;
};

/**************************************************************************//**
@Description	Parser Profile ID Pool Create Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_id_pool_create_verif_command {
	uint32_t opcode;
	uint8_t  pad[4];
};

/**************************************************************************//**
@Description	Parser MACROs Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_macros_command {
	uint32_t             opcode;
	struct parse_result  macros_struct;
};

/**************************************************************************//**
@Description	Parser Set prpid & hxs Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_set_prpid_hxs_command {
	uint32_t	opcode;
	uint16_t	starting_hxs;
	uint8_t		prpid;
	uint8_t 	pad;
};

/**************************************************************************//**
@Description	Parser Generate Parser Results Basic Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_basic_verif_command {
	uint32_t	opcode;
	int32_t		status;
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t 	parser_status_addr;
	uint8_t		pad[4];
};


void aiop_init_parser(uint8_t *prpid);

uint16_t aiop_verification_parser(uint32_t asa_seg_addr);

/** @} */ /* end of AIOP_PARSER_SRs_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_PARSER_H_ */
