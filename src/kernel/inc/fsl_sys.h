/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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

/*!
 *  @file    sys.h
 *  @brief   general system configuratoins and declarations
 *
 *  @details
 *
 *  @internal
 *  @requirements TODO: Put the CQ PRD record
 *  @implements   TODO: Add link to the design chapter/description
 */

#ifndef __FSL_SYS_H
#define __FSL_SYS_H

#include "common/types.h"
#include "fsl_errors.h"
#include "fsl_soc_aiop_spec.h"


/**************************************************************************//**
@Description   Platform Configuration Parameters Structure.

		This structure must be specifically defined by each platform.
*//***************************************************************************/


/**************************************************************************//**
@Collection    Object Identifier Macros
@{
*//***************************************************************************/
#define SYS_NULL_OBJECT_ID          0xffffffff
				    /**< Object ID representing no object */
#define SYS_MAX_NUM_OF_MODULE_IDS   3

#define CAST_ID_TO_HANDLE(_id)      (UINT_TO_PTR(_id))
			/**< Macro for casting an object ID to a handle */
#define CAST_HANDLE_TO_ID(_h)       (PTR_TO_UINT(_h))
			/**< Macro for casting a handle to an object ID */
/* @} */


struct platform_param;

/*!
 * @ingroup aiopapp_init
 * Struct defining the AIOP Applicatoins initialization and de-initialliation
 * functions.
 * Each AIOP Applicaton requiring initializatoin shall provide a
 * sys_module_desc entry.
 */
struct sys_module_desc {
int     (*early_init) (void);
int     (*init) (void);
void    (*free) (void);
};

/**************************************************************************//**
@Function      sys_init

@Description   System initialization routine.

		This routine initializes the internal system structures and
		services, such as memory management, objects repository and
		more, as well as the platform object.

@Return        Zero for success; Non-zero value on error.
*//***************************************************************************/
int sys_init(void);

/**************************************************************************//**
@Function      sys_free

@Description   System termination routine.

		This routine releases all internal structures that were
		initialized by the sys_init() routine.

@Return        None.
*//***************************************************************************/
void sys_free(void);

/**************************************************************************//**
@Function      sys_add_handle

@Description   Forces a handle for a specific object in the system.

		This routine allows forcing an object handle into the system
		and thus bypassing the normal initialization flow.

		The forced handle must be removed as soon as it is not valid
		anymore, using the sys_remove_handle() routine.

@Param[in]     h_module    - The object handle;
@Param[in]     module      - The object (module/sub-module) type.
@Param[in]     num_of_ids    - Number of IDs that are passed in the variadic-
				argument;
@Param[in]     ...         - Unique object IDs sequence;

@Return        0 on success; Error code otherwise.

@Cautions      This routine must not be used in normal flow - it serves only
		rare and special cases in platform initialization.
*//***************************************************************************/
int sys_add_handle(fsl_handle_t h_module, enum fsl_os_module module,
			int num_of_ids, ...);

/**************************************************************************//**
@Function      sys_remove_handle

@Description   Removes a previously forced handle of a specific object.

		This routine must be called to remove object handles that
		were previously forced using the SYS_ForceHandle() routine.

@Param[in]     module      - The object (module/sub-module) type.
@Param[in]     num_of_ids  - Number of IDs that are passed in
							the variadic-argument.
@Param[in]     ...         - Unique object IDs sequence;

@Return        0 on success; Error code otherwise.

@Cautions      This routine must not be used in normal flow - it serves only
		rare and special cases in platform initialization.
*//***************************************************************************/
int sys_remove_handle(enum fsl_os_module module, int num_of_ids, ...);

/**************************************************************************//**
@Function      sys_get_handle

@Description   Returns a specific object handle.

		This routine may be used to get the handle of any module or
		sub-module in the system.

		For singleton objects, it is recommended to use the
		sys_get_unique_handle() routine.

@Param[in]     module  - Module/sub-module type.
@Param[in]     num_of_ids  - Number of IDs that are passed in the variadic-
				argument.
@Param[in]     ...     - Unique object IDs sequence. For sub-modules,
		this is the unique object ID; For modules,
		this value must always be zero.

@Return        The handle of the specified object if exists;
		NULL if the object is not known or is not initialized.
*//***************************************************************************/
fsl_handle_t sys_get_handle(enum fsl_os_module module, int num_of_ids, ...);

/**************************************************************************//**
@Function      sys_get_unique_handle

@Description   Returns a specific object handle (for singleton objects).

		This routine may be used to get the handle of any singleton
		module or sub-module in the system.

		This routine simply calls the sys_get_handle() routine with
		the \c id parameter set to zero.

@Param[in]     module - Module/sub-module type.

@Return        The handle of the specified object if exists;
		NULL if the object is not known or is not initialized.
*//***************************************************************************/
static __inline__ fsl_handle_t sys_get_unique_handle(enum fsl_os_module module)
{
return sys_get_handle(module, 1, 0);
}


#endif /* __FSL_SYS_H */
