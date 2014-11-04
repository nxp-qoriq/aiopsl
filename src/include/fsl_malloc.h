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

/**************************************************************************//**        
 @File          fsl_malloc.h

 @Description   Prototypes, externals and typedefs for dynamic  memory allocation.
 
*//***************************************************************************/

#ifndef __FSL_MALLOC_H
#define __FSL_MALLOC_H

#include "common/types.h"


/**************************************************************************//**
 @Group         malloc_g_id   Dynamic Memory Allocation
 @Description   Prototypes, externals and typedefs for system  memory management.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      fsl_os_malloc

 @Description   Allocates contiguous block of memory from default heap.
                aiop_link.lcf linker file contains information where default 
                heap is mapped to. Currently the heap is located within DP_DDR
                memory partition.

 @Param[in]     size    Number of bytes to allocate.

 @Return        The address of the newly allocated block on success, NULL on failure.
*//***************************************************************************/
void * fsl_os_malloc(size_t size);

/**************************************************************************//**
 @Function     fsl_os_xmalloc

 @Description   Allocates contiguous block of memory in a specified
                alignment and from the specified segment.

 @Param[in]     size                Number of bytes to allocate.
 @Param[in]     mem_partition_id    Memory partition ID; The value zero must
                                    be mapped to the default heap partition.
 @Param[in]     alignment           Required memory alignment (in bytes).

 @Return        The address of the newly allocated block on success, NULL on failure.
*//***************************************************************************/
void *fsl_os_xmalloc(size_t size, int mem_partition_id, uint32_t alignment);

/**************************************************************************//**
 @Function      fsl_os_xfree

 @Description   Frees the memory block pointed to by "mem".
                Only for memory allocated by fsl_os_xmalloc().

 @Param[in]     mem     A pointer to the memory block.
*//***************************************************************************/
void fsl_os_xfree(void *mem);

/**************************************************************************//**
 @Function      fsl_os_free

 @Description   frees the memory block pointed to by "mem".

 @Param[in]     mem     A pointer to the memory block.
*//***************************************************************************/
void fsl_os_free(void *mem);

/** @} */ /* end of sytem_mem_management_id  group */


#endif /* __FSL_MALLOC_H */
