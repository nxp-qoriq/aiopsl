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
 @File          fsl_core.h

 @Description   Generic interface to basic core operations.

                The system integrator must ensure that this interface is
                mapped to a specific core implementation, by including the
                appropriate header file.
*//***************************************************************************/
#ifndef __FSL_CORE_H
#define __FSL_CORE_H

#include "fsl_types.h"

#ifdef SOC_PPC_CORE
#include "fsl_core_ppc.h"
#else
#error "core is not defined!"
#endif /* SOC_PPC_CORE */


#if (!defined(CORE_IS_LITTLE_ENDIAN) && !defined(CORE_IS_BIG_ENDIAN))
#error "must define core as little-endian or big-endian!"
#endif /* (!defined(CORE_IS_LITTLE_ENDIAN) && ... */

#ifndef CORE_CACHELINE_SIZE
#error "must define the core cache-line size!"
#endif /* !CORE_CACHELINE_SIZE */


/**************************************************************************//**
 @Function      core_get_id

 @Description   Returns the core ID in the system.

 @Return        Core ID.
*//***************************************************************************/
uint32_t core_get_id(void);

/**************************************************************************//**
 @Function      core_memory_barrier

 @Description   This routine will cause the core to stop executing any commands
                until all previous memory read/write commands are completely out
                of the core's pipeline.

 @Return        None.
*//***************************************************************************/
void core_memory_barrier(void);

/**************************************************************************//**
 @Function      core_test_and_set

 @Description   This routine tries to atomically test-and-set an integer
                in memory to a non-zero value.

                The memory will be set only if it is tested as zero, in which
                case the routine returns the new non-zero value; otherwise the
                routine returns zero.

 @Param[in]     p - pointer to a volatile int in memory, on which test-and-set
                    operation should be made.

 @Retval        Zero        - Operation failed - memory was already set.
 @Retval        Non-zero    - Operation succeeded - memory has been set.
*//***************************************************************************/
int core_test_and_set(volatile int *p);

/**************************************************************************//**
 @Function      core_instruction_sync

 @Description   This routine will cause the core to wait for previous instructions
                (including any interrupts they generate) to complete before the
                synchronization command executes, which purges all instructions
                from the processor's pipeline and refetches the next instruction.

 @Return        None.
*//***************************************************************************/
void core_instruction_sync(void);

/**************************************************************************//**
 @Function      core_dcache_enable

 @Description   Enables the data cache for memory pages that are
                not cache inhibited.

 @Return        None.
*//***************************************************************************/
void core_dcache_enable(void);

/**************************************************************************//**
 @Function      core_dcache_disable

 @Description   Disables the data cache.

 @Return        None.
*//***************************************************************************/
void core_dcache_disable(void);

/**************************************************************************//**
 @Function      core_icache_enable

 @Description   Enables the instruction cache for memory pages that are
                not cache inhibited.

 @Return        None.
*//***************************************************************************/
void core_icache_enable(void);

/**************************************************************************//**
 @Function      core_icache_disable

 @Description   Disables the instruction cache.

 @Return        None.
*//***************************************************************************/
void core_icache_disable(void);

/**************************************************************************//**
 @Function      core_local_irq_save

 @Description   Disable all interrupts by masking them at the CPU.

 @Return        A value that represents the interrupts state before the
                operation, and should be passed to the matching
                XX_RestoreAllIntr() call.
*//***************************************************************************/
uint32_t core_local_irq_save(void);

/**************************************************************************//**
 @Function      core_local_irq_restore

 @Description   Restore previous state of interrupts level at the CPU.

 @Param[in]     flags - A value that represents the interrupts state to restore,
                        as returned by the matching call for XX_DisableAllIntr().

 @Return        None.
*//***************************************************************************/
void core_local_irq_restore(uint32_t flags);


#endif /* __FSL_CORE_H */
