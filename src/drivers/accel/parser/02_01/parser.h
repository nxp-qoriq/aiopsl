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
@File		parser.h

@Description	This file contains the AIOP SW Parser API

@note		This file is intended for internal use only (it may not be 
		included elsewhere).
*//***************************************************************************/

#ifndef __PARSER_H
#define __PARSER_H

#include "fsl_types.h"
#include "fsl_parser.h"

/**************************************************************************//**
@Group		PARSER AIOP PARSER Internal

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

/**************************************************************************//**
@Group	PARSER_DEFINES Parser Defines
@{
*//***************************************************************************/
	/** Gross Running Sum Valid */
#define PARSER_GRSV_MASK	0x04000000
	/** Failure indication */
#define PARSER_STATUS_FAIL	0x80000000
	/** Parser status mask */
#define PARSER_STATUS_MASK	0xFFFF0000


/**************************************************************************//**
@Group AIOP_PARSE_RESULT_GEN_HW_STATUS Parse Result Generation HW Status
@{
*//***************************************************************************/
	/** Command status success */
#define PARSER_HW_STATUS_SUCCESS				0x00000000
	/** Parser SR failed due to Cycle limit exceeded */
#define PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED			0x00800000
	/** Parser SR failed due to invalid soft parse instruction */
#define PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION		0x00400000
	/** Parser SR failed due to parsing error */
#define PARSER_HW_STATUS_PARSING_ERROR				0x00200000
	/** Parser SR failed due to block limit exceeded */
#define PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED			0x00100000
	/** L3 checksum validation success */
#define PARSER_HW_STATUS_L3_CHECKSUM_VALIDATION_SUCCEEDED	0x00080000
	/** L3 checksum validation failure */
#define PARSER_HW_STATUS_FAIL_L3_CHECKSUM_VALIDATION_ERROR	0x000C0000
	/** L4 checksum validation success */
#define PARSER_HW_STATUS_L4_CHECKSUM_VALIDATION_SUCCEEDED	0x00020000
	/** L4 checksum validation failure */
#define PARSER_HW_STATUS_FAIL_L4_CHECKSUM_VALIDATION_ERROR	0x00030000
	/** L3 & L4 checksum validation success */
#define PARSER_HW_STATUS_L3_L4_CHECKSUM_VALIDATION_SUCCEEDED	0x000A0000
	/** L3 & L4 checksum generation failed */
#define PARSER_HW_STATUS_CHECKSUM_GENERATION_FAILED		0x00000000
	/** L3 checksum generation success */
#define PARSER_HW_STATUS_L3_CHECKSUM_GENERATION_SUCCEEDED	0x00080000
	/** L4 checksum generation success */
#define PARSER_HW_STATUS_L4_CHECKSUM_GENERATION_SUCCEEDED	0x00020000
	/** L3 & L4 checksum generation success */
#define PARSER_HW_STATUS_L3_L4_CHECKSUM_GENERATION_SUCCEEDED	0x000A0000


/** @} */ /* end of AIOP_PARSE_RESULT_GEN_HW_STATUS */

/**************************************************************************//**
@Group AIOP_PARSE_OTHER Parse Result Other Defines
@{
*//***************************************************************************/
	/** Uninitialized parser field of byte */
#define PARSER_UNINITILIZED_FIELD_OF_BYTE	0xFF
	/** Uninitialized parser field of two bytes */
#define PARSER_UNINITILIZED_FIELD_OF_HALF_WORD	0xFFFF
	/** Returned Status - L3 checksum generation succeeded */
#define PARSER_STATUS_L3_CHECKSUM_GENERATION_SUCCEEDED		0x00002000
	/** Returned Status - L4 checksum generation succeeded */
#define PARSER_STATUS_L4_CHECKSUM_GENERATION_SUCCEEDED		0x00004000
	/** Returned Status - L3 & L4 checksum generation succeeded */
#define PARSER_STATUS_L3_L4_CHECKSUM_GENERATION_SUCCEEDED	0x00006000


/** @} */ /* end of AIOP_PARSE_OTHER */
/** @} */ /* end of PARSER_DEFINES */

/** @} */ /* end of PARSER_MACROS */

/** \addtogroup PARSER_Enumerations
 *  @{
 */

/**************************************************************************//**
 @enum parser_functions

 @Description	AIOP Parser Functions enumeration.

 @{
*//***************************************************************************/
enum parser_function_identifier {
	PARSER_PROFILE_CREATE = 0,
	PARSER_PROFILE_REPLACE,
	PARSER_PROFILE_DELETE,
	PARSER_PROFILE_QUERY,
	PARSE_RESULT_GENERATE_DEFAULT,
	PARSE_RESULT_GENERATE,
	PARSE_RESULT_GENERATE_CHECKSUM,
	PARSE_RESULT_GENERATE_BASIC
};

/** @}*/ /* end of group PARSER_Enumerations */

/**************************************************************************//**
@Group		PARSER_STRUCTS Parser Structures

@Description	Freescale AIOP Parser Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Parser Profile Create or Update Params
*//***************************************************************************/
#pragma pack(push, 1)
struct parse_profile_create_params {
	/** Command Mtype Field */
	uint32_t mtype;

	/** Parse Profile ID Field */
	uint32_t prpid;

	/** Parse Profile */
	struct	parse_profile_record parse_profile;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	Parser Profile Delete/Query Params
*//***************************************************************************/
#pragma pack(push, 1)
struct parse_profile_delete_query_params {
	/** Command MTYPE Field */
	uint32_t mtype;

	/** Parse Profile ID  */
	uint32_t prpid;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	Input message Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct parser_input_message_params {
	uint16_t fha;
	uint16_t frs;
	uint16_t pra;
	uint16_t reserved1;
	uint16_t fda;
	uint16_t reserved2;
	uint16_t falugrp;
	uint16_t gross_running_sum;
	uint64_t opaquein;
};
#pragma pack(pop)


/** @} */ /* end of PARSER_STRUCTS */


/**************************************************************************//**
@Function	parser_exception_handler

@Description	Handler for the error status returned from the PARSER API
		functions.

@Param[in]	file_path - The path of the file in which the error occurred.
@Param[in]	func_id - The function in which the error occurred.
@Param[in]	line - The line in which the error occurred.
@Param[in]	status - Status to be handled be this function.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void parser_exception_handler(enum parser_function_identifier func_id,
			     uint32_t line,
			     int32_t status);


/** @} */ /* end of PARSER */


#endif /* __PARSER_H */
