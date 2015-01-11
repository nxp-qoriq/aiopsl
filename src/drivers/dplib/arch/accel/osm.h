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
@File		osm.h

@Description	This file contains the AIOP SW OSM internal API
*//***************************************************************************/


#ifndef __OSM_H_
#define __OSM_H_

#include "general.h"
#include "fsl_errors.h"

extern __TASK struct aiop_default_task_params default_task_params;


#define CONCURRENT	0	/** Concurrent Mode */
#define EXCLUSIVE	1	/** Exclusive Mode */

#define LEVEL0		0x00000000  /** Null ScopeID */
#define LEVEL1		0x00000001  /** Level 1 hierarchy scope level */
#define LEVEL2		0x00000002  /** Level 2 hierarchy scope level */
#define LEVEL3		0x00000003  /** Level 3 hierarchy scope level */
#define LEVEL4		0x00000004  /** Level 4 hierarchy scope level */

/** Level field in scope_id independently of the
	OSM_SCOPE_ID_LEVEL_INCREMENT_MASK definition */
	/** Level 1 in scope_id */
#define OSM_SCOPE_ID_LEVEL_1		0x00000000
	/** Level 2 in scope_id */
#define OSM_SCOPE_ID_LEVEL_2		0x55555555
	/** Level 3 in scope_id */
#define OSM_SCOPE_ID_LEVEL_3		0xaaaaaaaa
	/** Level 4 in scope_id */
#define OSM_SCOPE_ID_LEVEL_4		0xffffffff

/* Macros */
#define REGISTER_OSM_CONCURRENT \
              default_task_params.scope_mode_level_arr \
                 [default_task_params.current_scope_level-1] = CONCURRENT
#define REGISTER_OSM_EXCLUSIVE \
              default_task_params.scope_mode_level_arr \
                 [default_task_params.current_scope_level-1] = EXCLUSIVE

/* OSM Commands OP */

	/** OSM Scope Transition to Exclusive Opcode */
#define OSM_SCOPE_TRANSITION_TO_EXCL_OP				0x0d
	/** OSM Scope Transition to Exclusive and Modify Scope_id Opcode */
#define OSM_SCOPE_TRANSITION_TO_EXCL_WITH_NEW_SCOPEID_OP	0x09
	/** OSM Scope Transition to Concurrent Opcode */
#define OSM_SCOPE_TRANSITION_TO_CONCUR_OP			0x0c
	/** OSM Scope Transition to Concurrent and Modify Scope_id Opcode */
#define OSM_SCOPE_TRANSITION_TO_CONCUR_WITH_NEW_SCOPEID_OP	0x08
	/** OSM Scope Relinquish Exclusivity Opcode */
#define OSM_SCOPE_RELINQUISH_EXCL_OP				0x05
	/** OSM Scope Exit Opcode */
#define OSM_SCOPE_EXIT_OP					0x02

/* OSM SCOPE_ENTER Commands OP */

	/** OSM Scope Enter: relinquish_exclusivity_parent=false, scope_id=N,
	 * request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_OP				0x10
	/** OSM Scope Enter: relinquish_exclusivity_parent=false,
	 * scope_id=INCR, request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_SCOPE_INC_OP		0x14
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=N, request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_REL_PARENT_OP		0x12
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=INCR, request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_SCOPE_INC_REL_PARENT_OP	0x16
	/** OSM Scope Enter: relinquish_exclusivity_parent=false, scope_id=N,
	 * request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_OP				0x11
	/** OSM Scope Enter: relinquish_exclusivity_parent=false,
	 * scope_id=INCR, request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_SCOPE_INC_OP		0x15
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=N, request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_REL_PARENT_OP		0x13
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=INCR, request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_SCOPE_INC_REL_PARENT_OP	0x17


/**************************************************************************//**
 @enum osm_functions

 @Description	AIOP OSM Functions enumertion.

 @{
*//***************************************************************************/
enum osm_function_identifier {
	OSM_SCOPE_TRANSITION_TO_EXCLUSIVE_WITH_INCREMENT_SCOPE_ID = 0,
	OSM_SCOPE_TRANSITION_TO_EXCLUSIVE_WITH_NEW_SCOPE_ID,
	OSM_SCOPE_TRANSITION_TO_CONCURRENT_WITH_INCREMENT_SCOPE_ID,
	OSM_SCOPE_TRANSITION_TO_CONCURRENT_WITH_NEW_SCOPE_ID,
	OSM_SCOPE_RELINQUISH_EXCLUSIVITY,
	OSM_SCOPE_ENTER_TO_EXCLUSIVE_WITH_INCREMENT_SCOPE_ID,
	OSM_SCOPE_ENTER_TO_EXCLUSIVE_WITH_NEW_SCOPE_ID,
	OSM_SCOPE_ENTER,
	OSM_SCOPE_EXIT,
	OSM_GET_SCOPE
};

/** @}*/ /* end of group OSM_Enumerations */

/**************************************************************************//**
 @enum osm_errors

 @Description	AIOP OSM Error codes.

 @{
*//***************************************************************************/
enum osm_errors {
		/** Success. */
	OSM_SUCCESS = 0,
		/** Transition from no scope (null scope_id). */
	OSM_TRANSITION_FROM_NO_SCOPE_ERR = 0x1,
		/** Duplicate scope identifier detected. (DID error) */
	OSM_DUPLICATE_SCOPE_ERR = 0x2,
};

/* @} end of enum osm_errors */

/**************************************************************************//**
@Group		OSM_Internal_Functions	OSM Internal Functions

@Description	AIOP OSM operations internal functions.

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	osm_exception_handler

@Description	Handler for the error status returned from the OSM API
		functions.

@Param[in]	file_path - The path of the file in which the error occurred.
@Param[in]	func_id - The function in which the error occurred.
@Param[in]	line - The line in which the error occurred.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void osm_exception_handler(enum osm_function_identifier func_id,
			     uint32_t line);

/** @} end of group OSM_Internal_Functions */

#endif /* __OSM_H_ */
