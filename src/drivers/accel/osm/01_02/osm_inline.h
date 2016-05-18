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
@File		osm_inline.h

@Description	This file contains the AIOP OSM Operations Inline API 
		implementation.
*//***************************************************************************/

#ifndef __OSM_INLINE_H
#define __OSM_INLINE_H

#include "osm.h"
#include "fsl_osm.h"
#include "general.h"


inline void osm_scope_exit(void)
{
	/* call OSM */
	__e_osmcmd(OSM_SCOPE_EXIT_OP, 0);
		if (default_task_params.current_scope_level != 0)
			default_task_params.current_scope_level--;
}

inline void osm_task_init(void)
{
	uint8_t osm_val = ((struct presentation_context *)HWC_PRC_ADDRESS)
			->osrc_oep_osel_osrm;

	/**<	0 = No order scope specified.\n
		1 = Scope was specified for level 1 of hierarchy */
	/*default_task_params.current_scope_level =
			((osm_val & PRC_OSRC_MASK) ? 1 : 0);*/
	default_task_params.current_scope_level = osm_val >>
			PRC_OSRC_BIT_OFFSET;

	/**(uint32_t *)default_task_params.scope_mode_level_arr = 0 ;*/
	/**<	0 = Concurrent mode.\n
		1 = Exclusive mode. */
	/*default_task_params.scope_mode_level_arr[0] =
			((osm_val & PRC_OEP_MASK) ? 1 : 0);*/
	default_task_params.scope_mode_level_arr[0] = (osm_val & PRC_OEP_MASK)
			>> PRC_OEP_BIT_OFFSET;
}

inline void osm_scope_transition_to_exclusive_with_increment_scope_id(void)
{
	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_EXCL_OP,
			OSM_SCOPE_ID_STAGE_INCREMENT_MASK)) {
		/*OSM_TRANSITION_FROM_NO_SCOPE_ERR*/
		osm_exception_handler(
		OSM_SCOPE_TRANSITION_TO_EXCLUSIVE_WITH_INCREMENT_SCOPE_ID,
		__LINE__);
	} else {
		/** 1 = Exclusive mode. */
		REGISTER_OSM_EXCLUSIVE;
	}
}

inline void osm_get_scope(struct scope_status_params *scope_status)
{
	scope_status->scope_level = default_task_params.current_scope_level;
	scope_status->scope_mode = default_task_params.scope_mode_level_arr
			[default_task_params.current_scope_level-1];
}

inline void osm_scope_relinquish_exclusivity(void)
{

	/* call OSM */
	__e_osmcmd(OSM_SCOPE_RELINQUISH_EXCL_OP, 0);
	/** 0 = Concurrent mode. */
	REGISTER_OSM_CONCURRENT;
}

inline void osm_scope_enter_to_exclusive_with_new_scope_id(
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

#endif /*__OSM_INLINE_H*/
