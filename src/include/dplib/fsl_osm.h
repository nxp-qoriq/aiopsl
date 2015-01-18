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
@File		fsl_osm.h

@Description	This file contains the AIOP SW OSM Operations API
*//***************************************************************************/

#ifndef __FSL_LIB_OSM_H
#define __FSL_LIB_OSM_H

#include "common/types.h"


/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		FSL_AIOP_OSM OSM

 @Description	FSL AIOP OSM macros and functions

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		OSM_Structures OSM Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	OSM ScopeID Stage and Level Increment Definitions.

*//***************************************************************************/

	/** Scope_id stage increment mask.
		The OSM_SCOPE_ID_STAGE_INCREMENT_MASK defines the OSM_STAGE_ID
		field (size and location) in the OSM_SCOPE_ID.
		The OSM_STAGE_ID is incremented when a task calls to
		osm_scope_transition_to_exclusive_with_increment_scope_id()
		API and, as a result, a new scope_id is automatically created.
		If more bits are allocated for this field, the user can call
		osm_scope_transition_to_concurrent_with_new_scope_id() API more
		times without actually modifying a new scope_id (i.e. calling
		osm_scope_transition_to_exclusive_with_new_scope_id() API).*/
#define OSM_SCOPE_ID_STAGE_INCREMENT_MASK	0x0000000F

	/** Scope_id hierarchical level increment mask.
		The OSM_SCOPE_ID_LEVEL_INCREMENT_MASK defines the OSM_LEVEL_ID
		field (size and location) in any OSM_SCOPE_ID.
		Since 4 hierarchy scope levels are supported, 2 bits must be
		allocated for it.
		OSM_LEVEL_ID indicates the current hierarchy scope level and
		it is incremented when a task calls osm_scope_enter() API. */
#define OSM_SCOPE_ID_LEVEL_INCREMENT_MASK	0x00000030

/**************************************************************************//**
@Description	Scope Status structure.

		Includes information on the scope status structure.

*//***************************************************************************/
struct scope_status_params {
	uint8_t scope_level;/**< The current hierarchy scope level (0 to 4).
		0 = The task isn't found in any ordering scope (null scope_id).
		\n
		1 = The task is in the top level of hierarchy (level 1).\n
		2 = The task is in level 2 of hierarchy.\n
		3 = The task is in level 3 of hierarchy.\n
		4 = The task is in level 4 of hierarchy.*/
	uint8_t	scope_mode;/**< The current scope mode. Valid only if
		scope_level is not equal to zero (null scope_id).\n
		0 = Concurrent mode.\n
		1 = Exclusive mode.*/
};

/** @} end of group OSM_Structures */


/**************************************************************************//**
 @Group		OSM_Commands_Flags OSM Commands Flags

 @Description	OSM Commands Flags

 @{

*//***************************************************************************/

/**************************************************************************//**
@Group		OSMScopeEnterModeBits Scope enter mode bits

@{
*//***************************************************************************/

#define OSM_SCOPE_ENTER_CHILD_TO_CONCURENT  0x00 /**< "child" to XC scope mode.
		Hierarchically enter to a "child" ordering scope to
		concurrent (XC) stage (scope_concurrent_child). */
#define OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE  0x01 /**< "child" to XX scope mode.
		Hierarchically enter to a "child" ordering scope to
		exclusive (XX) stage (scope_exclusive_child).*/
#define OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT  0x02 /**< "child" scope_id.
		0 = The task is now associated with the specified "child"
		ordering queue (child_scope_id). The OSM_LEVEL_ID is updated
		automatically.\n
		1 = The task is now associated with a "child" ordering queue
		that is derived automatically from its "parent" ordering queue
		after the OSM_LEVEL_ID has been incremented by 1. */
#define OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY 0x04 /**< "parent" scope
		mode.
		0 = Proceed in the current ("parent") ordering scope XC or XX
		stage.\n
		1 = Proceed from the current ("parent") ordering scope
		stage to XC stage (scope_concurrent_parent).
		Another task may then enter the current ordering scope XX
		stage.*/

/** @} end of group OSMScopeEnterModeBits */

/** @} end of group OSM_Commands_Flags */


/**************************************************************************//**
@Group		OSM_Functions	OSM Functions

@Description	AIOP OSM operations functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	osm_scope_transition_to_exclusive_with_increment_scope_id

@Description	Transition from the current ordering scope to exclusive (XX)
		stage.

		The task is now associated with an incremental stage of the
		original scope_id (according to the
		\ref OSM_SCOPE_ID_STAGE_INCREMENT_MASK definition).

		Code following this command is executed in exclusive mode
		according to the new incremental ordering scope.

@Return		None.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error. Fatal error occurs if
		no order scope_id was specified for this task (null scope_id) before
		calling this function.
*//***************************************************************************/
void osm_scope_transition_to_exclusive_with_increment_scope_id(void);


/*************************************************************************//**
@Function	osm_scope_transition_to_exclusive_with_new_scope_id

@Description	Transition from the current ordering scope to exclusive (XX)
		stage.

		The task is now associated with the new specified scope_id.

		Code following this command is executed in exclusive mode
		according to the new ordering scope.

@Param[in]	scope_id

@Return		None.

@remark		The OSM_STAGE_ID is automatically updated in the new scope_id
		according to the
		\ref OSM_SCOPE_ID_LEVEL_INCREMENT_MASK definitions.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error. Fatal error occurs if
		no order scope_id was specified for this task (null scope_id) before
		calling this function.
*//***************************************************************************/
void osm_scope_transition_to_exclusive_with_new_scope_id(
		uint32_t scope_id);


/*************************************************************************//**
@Function	osm_scope_transition_to_concurrent_with_increment_scope_id

@Description	Transition from the current ordering scope to concurrent (XC)
		stage.

		The task is now associated with an incremental stage of the
		original scope_id (according to the
		\ref OSM_SCOPE_ID_STAGE_INCREMENT_MASK definition).

		No ordering restrictions are applied to the code following
		this command. Tasks in the same ordering scope to run in
		parallel.

@Return		None.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error. Fatal error occurs if
		no order scope_id was specified for this task (null scope_id) before
		calling this function.
*//***************************************************************************/
void osm_scope_transition_to_concurrent_with_increment_scope_id(void);


/*************************************************************************//**
@Function	osm_scope_transition_to_concurrent_with_new_scope_id

@Description	Transition from the current ordering scope to concurrent (XC)
		stage.

		The task is now associated with the new specified scope_id.

		No ordering restrictions are applied to the code following
		this command. Tasks in the same ordering scope to run in
		parallel.

@Param[in]	scope_id

@Return		None.

@remark		The OSM_LEVEL_ID field is automatically updated in the new
		scope_id according to the
		\ref OSM_SCOPE_ID_LEVEL_INCREMENT_MASK definitions.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error. Fatal error occurs if
		no order scope_id was specified for this task (null scope_id) before
		calling this function.
*//***************************************************************************/
void osm_scope_transition_to_concurrent_with_new_scope_id(
		uint32_t scope_id);


/*************************************************************************//**
@Function	osm_scope_relinquish_exclusivity

@Description	Proceed from exclusive (XX) to concurrent (XC) stage of the
		same ordering scope (relinquish exclusivity).

		Another task may then enter the current ordering scope's XX
		stage.

		No ordering restrictions are applied to the code following
		this command. Tasks in the same ordering scope can run in
		parallel.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void osm_scope_relinquish_exclusivity(void);


/*************************************************************************//**
@Function	osm_scope_enter_to_exclusive_with_increment_scope_id

@Description	Enter the next level of ordering scope in the hierarchy
		("child") to either exclusive (XX) stage.

		Proceed from the current ("parent") ordering scope stage to XC
		stage.

		The task is now associated with an incremental level of the
		parent scope_id (the OSM_LEVEL_ID is automatically updated
		according to the \ref OSM_SCOPE_ID_LEVEL_INCREMENT_MASK
		definition).

@Return		None.

@remark		This function is a subset of the osm_scope_enter() function.
@remark		This function will be ignored if it is an attempt to enter a new
		scope when the maximum depth of hierarchy level was reached.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
void osm_scope_enter_to_exclusive_with_increment_scope_id(void);


/*************************************************************************//**
@Function	osm_scope_enter_to_exclusive_with_new_scope_id

@Description	Enter the next level of ordering scope in the hierarchy
		("child") to either exclusive (XX) stage.

		Proceed from the current ("parent") ordering scope stage to XC
		stage.

		The task is now associated with a new specified "child"
		ordering queue (child_scope_id).

@Param[in]	child_scope_id - "child" ordering scope_id.

@Return		None.

@remark		This function is a subset of the osm_scope_enter() function.
@remark		This function will be ignored if it is an attempt to enter a new
		scope when the maximum depth of hierarchy level was reached.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
void osm_scope_enter_to_exclusive_with_new_scope_id(
		uint32_t child_scope_id);


/*************************************************************************//**
@Function	osm_scope_enter

@Description	Enter the next level of ordering scope in the hierarchy
		("child") to either exclusive (XX) or concurrent (XC) stage,
		according to the flag \ref OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE
		parameter setting.

		Proceed from the current ("parent") ordering scope stage to XC
		stage according to the flag
		\ref OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY parameter
		setting.

		According to the \ref OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT
		parameter setting, The task is now associated with one of the
		two following options:
		1. The specified "child" ordering queue (child_scope_id)
		2. An incremental level of the parent scope_id

		In both options, the OSM_LEVEL_ID is automatically updated
		according to the \ref OSM_SCOPE_ID_LEVEL_INCREMENT_MASK
		definition.

		Software may require that the order of the captured tasks
		associated with the parent ordering scope will be maintained
		also for the child cascade of ordering scopes. To achieve this,
		tasks must call osm_scope_enter() from the parent XX stage.
		If osm_scope_enter() is called from the XC stage, there is no
		guarantee that the orders will match as the tasks are running
		in parallel when osm_scope_enter() is called.

		However, when a task calls osm_scope_enter() from the parent
		XX stage, no other task may enter the parent XX stage until the
		first task has completed executing the child cascade and called
		osm_scope_exit() to return to the parent ordering scope.

		In order to remove the above restriction so that tasks can
		enter the parent XX stage and still save the order between the
		parent and child, the task has to enter the child from the
		parent XX stage but also automatically proceed from the current
		(parent) ordering scope stage to XC. This feature can be
		achieved by the flag
		\ref OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY parameter.

		Note that the osm_scope_relinquish_exclusivity() API cannot be
		used instead because it is defined to operate only on the
		current (child) ordering scope, not the parent.

@Param[in]	scope_enter_flags - \link OSMScopeEnterModeBits Scope enter
		mode bits \endlink
@Param[in]	child_scope_id - "child" ordering scope_id. This parameter is
		relevant only if \ref OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT is
		not set.

@Return		None.

@remark		This function will be ignored if it is an attempt to enter a new
		scope when the maximum depth of hierarchy level was reached.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
void osm_scope_enter(
		uint32_t scope_enter_flags,
		uint32_t child_scope_id);


/*************************************************************************//**
@Function	osm_scope_exit

@Description	Exit the current ordering scope and return to its "parent"
		ordering scope. (it decreases ordering level by one).

		It can be made from either the XX or the XC stage of the
		current ordering scope.

		If task is exited from the top level of hierarchy (level 1),
		then the task enters a special state in which it is not in any
		ordering scope (null scope_id). In this case:
		- It is illegal to call to osm_scope_exit() function.
		- It is illegal to call to any of osm_scope_transition_to
		functions.
		- It is illegal to call to osm_scope_relinquish_exclusivity()
		function.
		- Tasks may call osm_scope_enter() function
		(\ref OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT flag must be reset)
		to enter a level 1 ordering scope but the order will be
		arbitrary.

@Return		None.

@remark		A task is removed from its ordering queue automatically when
		it terminates.

@Cautions	None.
*//***************************************************************************/
void osm_scope_exit(void);


/*************************************************************************//**
@Function	osm_get_scope

@Description	Returns the current scope status:
		Scope mode (i.e. concurrent or exclusive) and hierarchy level
		of the task calling this function.

		The scope status are automatically updated after any OSM
		function that returns without any failure.

@Param[out]	scope_status - \link scope_status_params Get scope status
		\endlink

@Return		None.

@Cautions	None.
*//***************************************************************************/
void osm_get_scope(struct scope_status_params *scope_status);

/** @} end of group OSM_Functions */
/** @} */ /* end of FSL_AIOP_OSM */
/** @} */ /* end of ACCEL */


#endif /* __FSL_LIB_OSM_H */
