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
@File		osm.c

@Description	This file contains the AIOP OSM Operations API implementation.
*//***************************************************************************/

#include "fsl_osm.h"
#include "general.h"

extern __TASK struct aiop_default_task_params default_task_params;

void osm_scope_transition_to_exclusive_with_new_scope_id(
		uint32_t scope_id) {

	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
	switch (default_task_params.current_scope_level) {
	case (LEVEL1):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL2):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL3):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL4):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	}

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_EXCL_WITH_NEW_SCOPEID_OP,
			scope_id)) {
		/*OSM_TRANSITION_FROM_NO_SCOPE_ERR*/
		osm_exception_handler(
		OSM_SCOPE_TRANSITION_TO_EXCLUSIVE_WITH_NEW_SCOPE_ID,
		__LINE__);
	} else {
		/** 1 = Exclusive mode. */
		REGISTER_OSM_EXCLUSIVE;
		}
}

void osm_scope_transition_to_concurrent_with_increment_scope_id(void)
{

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_CONCUR_OP,
		OSM_SCOPE_ID_STAGE_INCREMENT_MASK)) {
		/*OSM_TRANSITION_FROM_NO_SCOPE_ERR*/
		osm_exception_handler(
		OSM_SCOPE_TRANSITION_TO_CONCURRENT_WITH_INCREMENT_SCOPE_ID,
		__LINE__);
	} else {
		/** 0 = Concurrent mode. */
		REGISTER_OSM_CONCURRENT;
		}
}

void osm_scope_transition_to_concurrent_with_new_scope_id(
		uint32_t scope_id) {

	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
	switch (default_task_params.current_scope_level) {
	case (LEVEL1):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL2):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL3):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL4):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	}

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_CONCUR_WITH_NEW_SCOPEID_OP,
			scope_id)) {
		/*OSM_TRANSITION_FROM_NO_SCOPE_ERR*/
		osm_exception_handler(
		OSM_SCOPE_TRANSITION_TO_CONCURRENT_WITH_NEW_SCOPE_ID,
		__LINE__);
	} else {
		/** 0 = Concurrent mode. */
		REGISTER_OSM_CONCURRENT;
		}
}

void osm_scope_relinquish_exclusivity(void)
{

	/* call OSM */
	__e_osmcmd(OSM_SCOPE_RELINQUISH_EXCL_OP, 0);
	/** 0 = Concurrent mode. */
	REGISTER_OSM_CONCURRENT;
}

void osm_scope_enter_to_exclusive_with_increment_scope_id(void)
{

	if (default_task_params.current_scope_level == LEVEL4)
		return;
	
	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_SCOPE_INC_REL_PARENT_OP
			, OSM_SCOPE_ID_LEVEL_INCREMENT_MASK)) {
		osm_exception_handler(
		OSM_SCOPE_ENTER_TO_EXCLUSIVE_WITH_INCREMENT_SCOPE_ID,
		__LINE__);
	} else {
		default_task_params.current_scope_level++;
		if (default_task_params.current_scope_level > 1)
			/** 0 = Parent: Concurrent mode. */
			default_task_params.scope_mode_level_arr
			[default_task_params.current_scope_level-2] =
					CONCURRENT;
		/** 1 = Child: Exclusive mode. */
		REGISTER_OSM_EXCLUSIVE;
		return;
	}
}

void osm_scope_enter_to_exclusive_with_new_scope_id(
		uint32_t child_scope_id) {

	if (default_task_params.current_scope_level == LEVEL4)
		return;
	
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
	switch (default_task_params.current_scope_level) {
	case (LEVEL0):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL1):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 &
		OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL2):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 &
		OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL3):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 &
		OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	}

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_REL_PARENT_OP, child_scope_id)) {
		osm_exception_handler(
		OSM_SCOPE_ENTER_TO_EXCLUSIVE_WITH_NEW_SCOPE_ID,
		__LINE__);
	} else {
		default_task_params.current_scope_level++;
		if (default_task_params.current_scope_level > 1)
			/** 0 = Parent: Concurrent mode. */
			default_task_params.scope_mode_level_arr
			[default_task_params.current_scope_level-2] =
					CONCURRENT;
		/** 1 = Child: Exclusive mode. */
		REGISTER_OSM_EXCLUSIVE;
		return;
	}
}

void osm_scope_enter(
		uint32_t scope_enter_flags,
		uint32_t child_scope_id) {

	if (default_task_params.current_scope_level == LEVEL4)
		return;
	
	switch (scope_enter_flags) {
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_CONC_OP,
			child_scope_id)) {
			osm_exception_handler(OSM_SCOPE_ENTER,__LINE__);
		} else {
			default_task_params.current_scope_level++;
			/** 0 = Child: Concurrent mode. */
			REGISTER_OSM_CONCURRENT;
			return;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT):
	{
		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_CONC_SCOPE_INC_OP,
				OSM_SCOPE_ID_LEVEL_INCREMENT_MASK)) {
			osm_exception_handler(OSM_SCOPE_ENTER, __LINE__);
		} else {
			default_task_params.current_scope_level++;
			/** 0 = Child: Concurrent mode. */
			REGISTER_OSM_CONCURRENT;
			return;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_CONC_REL_PARENT_OP,
			child_scope_id)) {
			osm_exception_handler(OSM_SCOPE_ENTER, __LINE__);
		} else {
			default_task_params.current_scope_level++;
			if (default_task_params.current_scope_level > 1)
				/** 0 = Parent: Concurrent mode. */
				default_task_params.scope_mode_level_arr
				[default_task_params.current_scope_level-2] =
						CONCURRENT;
			/** 0 = Child: Concurrent mode. */
			REGISTER_OSM_CONCURRENT;
			return;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
		if (__e_osmcmd_(
			OSM_SCOPE_ENTER_CONC_SCOPE_INC_REL_PARENT_OP
				, OSM_SCOPE_ID_LEVEL_INCREMENT_MASK)) {
			osm_exception_handler(OSM_SCOPE_ENTER, __LINE__);
		} else {
			default_task_params.current_scope_level++;
			if (default_task_params.current_scope_level > 1)
				/** 0 = Parent: Concurrent mode. */
				default_task_params.scope_mode_level_arr
				[default_task_params.current_scope_level-2] =
						CONCURRENT;
			/** 0 = Child: Concurrent mode. */
			REGISTER_OSM_CONCURRENT;
			return;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(
			OSM_SCOPE_ENTER_EXCL_OP, child_scope_id)) {
			osm_exception_handler(OSM_SCOPE_ENTER, __LINE__);
		} else {
			default_task_params.current_scope_level++;
			/** 1 = Child: Exclusive mode. */
			REGISTER_OSM_EXCLUSIVE;
			return;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT):
	{
		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_SCOPE_INC_OP,
				OSM_SCOPE_ID_LEVEL_INCREMENT_MASK)) {
			osm_exception_handler(OSM_SCOPE_ENTER, __LINE__);
		} else {
			default_task_params.current_scope_level++;
			/** 1 = Child: Exclusive mode. */
			REGISTER_OSM_EXCLUSIVE;
			return;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_REL_PARENT_OP,
				child_scope_id)) {
			osm_exception_handler(OSM_SCOPE_ENTER, __LINE__);
		} else {
			default_task_params.current_scope_level++;
			if (default_task_params.current_scope_level > 1)
				/** 0 = Parent: Concurrent mode. */
				default_task_params.scope_mode_level_arr
				[default_task_params.current_scope_level-2] =
						CONCURRENT;
			/** 1 = Child: Exclusive mode. */
			REGISTER_OSM_EXCLUSIVE;
			return;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
		/* call OSM */
		if (__e_osmcmd_(
			OSM_SCOPE_ENTER_EXCL_SCOPE_INC_REL_PARENT_OP
				, OSM_SCOPE_ID_LEVEL_INCREMENT_MASK)) {
			osm_exception_handler(OSM_SCOPE_ENTER, __LINE__);
		} else {
			default_task_params.current_scope_level++;
			if (default_task_params.current_scope_level > 1)
				/** 0 = Parent: Concurrent mode. */
				default_task_params.scope_mode_level_arr
				[default_task_params.current_scope_level-2] =
						CONCURRENT;
			/** 1 = Child: Exclusive mode. */
			REGISTER_OSM_EXCLUSIVE;
			return;
		}
		break;
	}
	default:
		return;
		break;
	}
}

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

void osm_exception_handler(enum osm_function_identifier func_id,
		     uint32_t line)
{
	char *func_name;
	char *err_msg;
	
	/* Translate function ID to function name string */
	switch(func_id) {
	case OSM_SCOPE_TRANSITION_TO_EXCLUSIVE_WITH_INCREMENT_SCOPE_ID:
		func_name = "osm_scope_transition_to_exclusive_with_increment_"
				"scope_id\n";
		break;
	case OSM_SCOPE_TRANSITION_TO_EXCLUSIVE_WITH_NEW_SCOPE_ID:
		func_name = "osm_scope_transition_to_exclusive_with_new_"
				"scope_id\n";
		break;
	case OSM_SCOPE_TRANSITION_TO_CONCURRENT_WITH_INCREMENT_SCOPE_ID:
		func_name = "osm_scope_transition_to_concurrent_with_increment"
				"_scope_id\n";
		break;
	case OSM_SCOPE_TRANSITION_TO_CONCURRENT_WITH_NEW_SCOPE_ID:
		func_name = "osm_scope_transition_to_concurrent_with_new_"
				"scope_id\n";
		break;
	case OSM_SCOPE_ENTER_TO_EXCLUSIVE_WITH_INCREMENT_SCOPE_ID:
		func_name = "osm_scope_enter_to_exclusive_with_increment_"
				"scope_id\n";
		break;
	case OSM_SCOPE_ENTER_TO_EXCLUSIVE_WITH_NEW_SCOPE_ID:
		func_name = "osm_scope_enter_to_exclusive_with_new_"
				"scope_id\n";
		break;
	case OSM_SCOPE_ENTER:
		func_name = "osm_scope_enter";
		break;
	default:
		/* create own exception */
		func_name = "Unknown Function";
	}

	err_msg = "OSM error.\n";

	exception_handler(__FILE__, func_name, line, err_msg);
}

#pragma pop
