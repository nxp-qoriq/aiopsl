/**************************************************************************//**
@File          parser.h

@Description   This file contains the AIOP SW Parser API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __PARSER_H
#define __PARSER_H

#include "common/types.h"
#include "dplib/fsl_parser.h"


/**************************************************************************//**
@Group		PARSER AIOP_Parser Internal

@Description	Freescale AIOP Parser API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	PARSER_MACROS Parser Macros

@Description	Freescale AIOP Parser Macros

@{
*//***************************************************************************/


/**************************************************************************//**
@Group	PARSER_MTYPE Parser Message Types
@{
*//***************************************************************************/
	/** Parse profile create message type */
#define PARSER_PRP_CREATE_MTYPE			0x00AD
	/** Parse profile create message type */
#define PARSER_PRP_DELETE_MTYPE			0x00A2
	/** Parse profile query message type */
#define PARSER_PRP_QUERY_MTYPE			0x00A7
	/** Generate Parse Result message type */
#define PARSER_GEN_PARSE_RES_MTYPE		0x0015

/** @} */ /* end of PARSER_MTYPE */

/** @} */ /* end of PARSER_MACROS */

/**************************************************************************//**
@Group		PARSER_STRUCTS Parser Structures

@Description	Freescale AIOP Parser Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Parser Profile Create or Update Params
*//***************************************************************************/
#pragma pack(push, 1)
struct parse_profile_create_params{
	/** Reserved */
	uint8_t reserved1[4];

	/** Parse Profile ID */
	uint8_t prpid;

	/** Reserved */
	uint8_t reserved2[3];

	/** Parse Profile */
	struct	parse_profile_record parse_profile;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	Parser Profile Delete/Query Params
*//***************************************************************************/
#pragma pack(push, 1)
struct parse_profile_delete_query_params{
	/** Reserved */
	uint8_t reserved1;

	/** Command MTYPE */
	uint8_t	mtype;

	/** Reserved */
	uint16_t reserved2;

	/** Parse Profile ID */
	uint8_t prpid;

	/** Reserved */
	uint8_t reserved3[3];
};
#pragma pack(pop)


/** @} */ /* end of PARSER_STRUCTS */


/** @} */ /* end of PARSER */


#endif /* __PARSER_H */
