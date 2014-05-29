/**************************************************************************//**
@File		aiop_verification_parser.h

@Description	This file contains the AIOP PARSER SW Verification Structures

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_PARSER_H_
#define __AIOP_VERIFICATION_PARSER_H_

#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_parser.h"


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
	PARSER_INIT_GROSS_VERIF_CMDTYPE
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

/**************************************************************************//**
@Description	Parser verification init gross running sum Command structure.

		This command inits the gross running sum field in parse profile result
		
*//***************************************************************************/
struct parser_init_gross_verif_command {
	uint32_t	opcode;
	uint32_t 	status;
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
	uint32_t parse_profile;
	uint8_t  prpid;
	uint8_t  pad[3];
};

/**************************************************************************//**
@Description	Parser Profile Delete Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_delete_verif_command {
	uint32_t opcode;
	int32_t  status;
	uint8_t  prpid;
	uint8_t  pad[7];
};


/**************************************************************************//**
@Description	Parser Profile Query Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_query_verif_command {
	uint32_t opcode;
/*	int32_t  status;*/
	uint32_t parse_profile;
	uint8_t  prpid;
	/*	uint8_t  pad[3];*/
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
	uint32_t          opcode;
	int32_t           status;
	uint16_t		  hxs;
	uint16_t		  l3_checksum;
	/*uint16_t		  l4_checksum;*/
	uint8_t           offset;
	uint8_t           pad[3];
};

/**************************************************************************//**
@Description	Parser Generate Parser Results Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_verif_command {
	uint32_t opcode;
	int32_t  status;
	uint8_t prpid;
	uint8_t  flags;
	uint8_t  pad[6];
};

/**************************************************************************//**
@Description	Parser Generate Parser Results (EXP) Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_gen_parser_res_exp_verif_command {
	uint32_t                      opcode;
	int32_t                       status;
	uint16_t		      		  hxs;
	uint8_t                       flags;
	uint8_t                       offset;
	uint8_t                       pad[4];
};

/**************************************************************************//**
@Description	Parser Profile ID Pool Create Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_prp_id_pool_create_verif_command {
	uint32_t opcode;
	int32_t  status;
};

/**************************************************************************//**
@Description	Parser MACROs Command structure.

		Includes information needed for Parser Commands verification.
*//***************************************************************************/
struct parser_macros_command {
	uint32_t             opcode;
	uint32_t             macros_struct;
};

void aiop_init_parser(uint8_t *prpid);

uint16_t aiop_verification_parser(uint32_t asa_seg_addr);

/** @} */ /* end of AIOP_PARSER_SRs_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_PARSER_H_ */
