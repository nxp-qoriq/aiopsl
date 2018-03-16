/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		osm.c

@Description	This file contains the AIOP OSM Operations API implementation.
*//***************************************************************************/

#include "fsl_osm.h"
#include "general.h"

#ifdef SL_DEBUG
#include "fsl_sys.h"
#include "fsl_aiop_common.h"
#include "fsl_io.h"
#endif

extern __TASK struct aiop_default_task_params default_task_params;

void osm_scope_transition_to_exclusive_with_new_scope_id(
		uint32_t scope_id) {

	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
	switch (default_task_params.current_scope_level) {
	case (LEVEL1):
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
	case (LEVEL2):
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
	case (LEVEL3):
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
	case (LEVEL4):
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
#ifdef SL_DEBUG
	default:
		DEBUG_SCOPE_ID(scope_id);
		break;
#endif	/* SL_DEBUG */
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
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
	case (LEVEL2):
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
	case (LEVEL3):
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
	case (LEVEL4):
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
#ifdef SL_DEBUG
	default:
		DEBUG_SCOPE_ID(scope_id);
		break;
#endif	/* SL_DEBUG */
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

void osm_scope_enter_to_exclusive_with_increment_scope_id(void)
{
#ifndef SL_DEBUG
	if (default_task_params.current_scope_level == LEVEL4)
		return;
#endif
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

/******************************************************************************/
void osm_scope_enter(uint32_t scope_enter_flags, uint32_t child_scope_id)
{
#ifndef SL_DEBUG
	if (default_task_params.current_scope_level == LEVEL4)
		return;
#endif
	switch (scope_enter_flags) {
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT):
		/* Update the SCOPE_ID_LEVEL_INCREMENT field in the
		 * new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL1):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL2):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL3):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
#ifdef SL_DEBUG
		default:
			DEBUG_SCOPE_ID(child_scope_id);
			break;
#endif
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
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT):
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
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL1):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL2):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL3):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
#ifdef SL_DEBUG
		default:
			DEBUG_SCOPE_ID(child_scope_id);
			break;
#endif	/* SL_DEBUG */
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
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL1):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL2):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL3):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
#ifdef SL_DEBUG
		default:
			DEBUG_SCOPE_ID(child_scope_id);
			break;
#endif	/* SL_DEBUG */
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
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT):
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
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (default_task_params.current_scope_level) {
		case (LEVEL0):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL1):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL2):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
		case (LEVEL3):
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
#ifdef SL_DEBUG
		default:
			DEBUG_SCOPE_ID(child_scope_id);
			break;
#endif	/* SL_DEBUG */
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
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
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
	default:
		return;
		break;
	}
}

void osm_scope_transition_to_exclusive_with_increment_scope_id_wrp(void)
{
	osm_scope_transition_to_exclusive_with_increment_scope_id();
}

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

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
#ifndef SL_DEBUG
	exception_handler(__FILE__, func_name, line, err_msg);
#else
	UNUSED(line);
#endif
}

#pragma pop

#ifdef SL_DEBUG
void dump_osm_regs(void)
{
	struct aiop_tile_regs	*aiop_regs;
	struct aiop_osm_regs	*osm_regs;
	int			i;

	register uint32_t	task, tmp;

	aiop_regs = (struct aiop_tile_regs *)
			sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	osm_regs = (struct aiop_osm_regs *)&aiop_regs->osm_regs;
	/*
	 * AIOP-z490 CPU
	 * 11.6.5 Task Control and Status Register 0, 1 (TASKCSR0, TASKCSR1)
	 */
	asm
	{
		mfdcr		tmp, dcr476	/* TASKCSR0 */
		e_clrlwi	task, tmp, 24	/* Clear top 24 bits */
	}
	/*
	 * Initialize TASK_ID in ORTAR to enable OSM registers.
	 *
	 * The order scope manager read task address register (ORTAR). Writing
	 * this register will populate the read task data registers with state
	 * information for the specified task number and will guarantee a
	 * consistent reading across all scopes.
	 * Use of this feature is not recommended during operation since it may
	 * have adverse effects on functional performance.*/
	iowrite32be(task, &osm_regs->ortar);

	fsl_print("Dump of OSM register\n");
	fsl_print("\t [at %08x] OMR    = 0x%08x\n", (uint32_t)&osm_regs->omr,
		  ioread32be(&osm_regs->omr));
	fsl_print("\t [at %08x] OSR    = 0x%08x\n", (uint32_t)&osm_regs->osr,
		  ioread32be(&osm_regs->osr));
	fsl_print("\t [at %08x] ORTAR  = 0x%08x\n", (uint32_t)&osm_regs->ortar,
		  ioread32be(&osm_regs->ortar));
	for (i = 0; i < 8; i++)
		fsl_print("\t [at %08x] ORTDR%d = 0x%08x\n",
			  (uint32_t)&osm_regs->ortdr[i], i,
			  ioread32be(&osm_regs->ortdr[i]));
	fsl_print("\t [at %08x] OEMVR  = 0x%08x\n", (uint32_t)&osm_regs->oemvr,
		  ioread32be(&osm_regs->oemvr));
	fsl_print("\t [at %08x] OEMMR  = 0x%08x\n",
		  (uint32_t)&osm_regs->oemmr, ioread32be(&osm_regs->oemmr));
	fsl_print("\t [at %08x] OCR    = 0x%08x\n",
		  (uint32_t)&osm_regs->ocr, ioread32be(&osm_regs->ocr));
	fsl_print("\t [at %08x] OERR   = 0x%08x\n",
		  (uint32_t)&osm_regs->oerr, ioread32be(&osm_regs->oerr));
	fsl_print("\t [at %08x] OEDR   = 0x%08x\n",
		  (uint32_t)&osm_regs->oedr, ioread32be(&osm_regs->oedr));
	fsl_print("\t [at %08x] OEDDR  = 0x%08x\n",
		  (uint32_t)&osm_regs->oeddr, ioread32be(&osm_regs->oeddr));
	for (i = 0; i < 8; i++)
		fsl_print("\t [at %08x] OECR%d  = 0x%08x\n",
			  (uint32_t)&osm_regs->oecr[i], i,
			  ioread32be(&osm_regs->oecr[i]));
	fsl_print("\t [at %08x] OEUOMR = 0x%08x\n",
		  (uint32_t)&osm_regs->oeuomr, ioread32be(&osm_regs->oeuomr));
}
#endif	/* SL_DEBUG */
