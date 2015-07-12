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

/**

 @File          fsl_smp.h

 @Description   Definitions and functions for initializing and operating
                multi-processing services.

*//***************************************************************************/
#ifndef __FSL_SYS_MP_H
#define __FSL_SYS_MP_H

#include "common/types.h"
#include "fsl_core.h"


/**************************************************************************//**
 @Group         sys_g     System Interfaces

 @Description   Bare-board system programming interfaces.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         sys_mp_grp      System Multi-Processing Interfaces

 @Description   System interfaces for Multi-Processing.

                The system Multi-Processing interfaces include:
                - General functions
                - Spinlocks
                - Barrier

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Collection    General Multi-Processing Application Programming Interface

 @Description   Spinlock definitions and functions.

 @{
*//***************************************************************************/

#define SYS_ANY_CORE    ((uint32_t)(~0)) /**< Assignment to any valid core (for use
                                              in system layout object descriptors) */

/**************************************************************************//**
 @Function      sys_is_core_active

 @Description   Checks if a given core is active in current configuration
                (according to the partition cores mask).

                This macro evaluates to 1 if the respective bit of current
                core is set.
*//***************************************************************************/
int sys_is_core_active(uint32_t core_id);

/**************************************************************************//**
 @Function      sys_is_master_core

 @Description   Checks if the current core is the master core.

                In SMP configuration, the master core is marked by the user in
                the masterCoresMask variable in the system initialization parameters.

 @Cautions      This macro may be interpreted to a function call. When using
                it more than once in a code section, consider saving it into a
                local variable.
*//***************************************************************************/
int sys_is_master_core(void);

/**************************************************************************//**
 @Function      sys_is_cluster_master

 @Description   Checks if the current core is the cluster master core.

                In SMP configuration, the cluster master core is marked by the 
                user in the system initialization parameters.

 @Cautions      This macro may be interpreted to a function call. When using
                it more than once in a code section, consider saving it into a
                local variable.
*//***************************************************************************/
int sys_is_cluster_master(void);

/**************************************************************************//**
 @Function      sys_get_cores_mask

 @Description   Returns the system's active cores in a mask format.

                Core 0 is marked by the least significant bit, core 1 is marked
                by the next bit to the left, and so on.
*//***************************************************************************/
uint64_t sys_get_cores_mask(void);

/**************************************************************************//**
 @Function      sys_get_num_of_cores

 @Description   Returns the number of active cores in the system.
*//***************************************************************************/
uint32_t sys_get_num_of_cores(void);

/**************************************************************************//**
 @Function      sys_get_max_num_of_cores

 @Description   Returns the maximum number of cores in the system.
*//***************************************************************************/
uint32_t sys_get_max_num_of_cores(void);

/* @} */ /* end of general collection */

/**************************************************************************//**
 @Collection    Barrier Application Programming Interface

 @Description   Definitions and functions for central program barrier.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      sys_barrier

 @Description   Forces the program to perform busy-wait cycles until all cores
                reach this point in the flow.

 @Param         None.
*//***************************************************************************/
void sys_barrier(void);

/** @} */ /* end of sys_mp_grp */
/** @} */ /* end of sys_g */


#endif /* __FSL_SYS_MP_H */
