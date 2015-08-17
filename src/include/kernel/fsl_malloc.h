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
 @File          fsl_malloc.h

 @Description   Prototypes, externals and typedefs for dynamic  memory allocation.

*//***************************************************************************/

#ifndef __FSL_MALLOC_H
#define __FSL_MALLOC_H

#include "fsl_types.h"


/**************************************************************************//**
 @Group         malloc_g_id   Dynamic Memory Allocation
 @Description   Prototypes, externals and typedefs for system  memory management.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Memory Partition Identifiers.
                Used as a parameter for fsl_os_get_mem().
                Note that not all memory partitions are supported by all
                platforms. Every platform may select which memory partitions
                to support.
*//***************************************************************************/
typedef enum memory_partition_id {
    MEM_PART_DP_DDR = 2,/**< Primary DDR  memory partition */
    MEM_PART_SYSTEM_DDR,    /**< Secondary DDR  memory partition */
    MEM_PART_SH_RAM,                   /**< Shared-SRAM memory */
    MEM_PART_PEB,                      /**< Packet-Express-Buffer memory partition */
    MEM_PART_MC_PORTALS,               /**< MC portals for cmdif memory partition */
    MEM_PART_CCSR,                     /**< SoC CCSR memory partition */
    MEM_PART_LAST                      /**< Invalid memory partition */
} e_memory_partition_id;

/**************************************************************************//**
 @Function      fsl_malloc

 @Description   Allocates contiguous block of memory from shared ram.

 @Param[in]     size        Number of bytes to allocate.
 @Param[in]     alignment   Required memory alignment (in bytes). Should be a
                            power of 2 ( 4, 8, 16 etc.)

 @Return        A 32 bit  address of the newly allocated block on success, NULL on failure.
*//***************************************************************************/
void * fsl_malloc(size_t size, uint32_t alignment);

/**************************************************************************//**
 @Function      fsl_free

 @Description   frees the memory block pointed to by "mem" in shared ram.
                Applicable only for memory allocated by fsl_malloc().

 @Param[in]     mem     A pointer to the memory block.
*//***************************************************************************/
void fsl_free(void *mem);

/**************************************************************************//**
@Function      fsl_os_get_mem

@Description   Allocates contiguous block of memory with the specified
                alignment and from the specified  memory partition.
@Param[in]     size                Number of bytes to allocate.
@Param[in]     mem_partition_id    Memory partition ID,
               Valid values: MEM_PART_DP_DDR,MEM_PART_PEB,MEM_PART_SYSTEM_DDR
@Param[in]     alignment           Required memory alignment (in bytes).
                                   Should be a power of 2 ( 4, 8, 16 etc.)
@Param[out]    paddr               A valid allocated physical address(64 bit) if success,
                                   in case of failure (non_zero return value), this
                                   value is undefined and should not be used.
@Return        0                   on success,
               -ENOMEM (not enough memory to allocate)or
               -EINVAL ( invalid memory partition ) on failure.
*//***************************************************************************/
int fsl_os_get_mem(uint64_t size, int mem_partition_id, uint64_t alignment,
                   uint64_t* paddr);

/**************************************************************************//**
@Function      fsl_os_put_mem

@Description   Frees the memory block pointed to by "paddr".
               paddr should be allocated by fsl_os_get_mem()

@Param[in]    paddr  An address to be freed.

*//***************************************************************************/
void  fsl_os_put_mem(uint64_t paddr);


/**************************************************************************//**
@Function      fsl_mem_exists

@Description   Returns  if the provided  memory partition exists

@Param[in]    mem_partition_id   A memory partition id
@Return        1                 if exists,
               0                 if does not exist
*//***************************************************************************/
int fsl_mem_exists(int mem_partition_id);

/** @} */ /* end of sytem_mem_management_id  group */


#endif /* __FSL_MALLOC_H */
