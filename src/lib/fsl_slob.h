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
 @File          slob.h

 @Description   Memory Manager Application Programming Interface
*//***************************************************************************/
#ifndef __FSL_SLOB_H
#define __FSL_SLOB_H

#include "common/types.h"


#define MM_MAX_ALIGNMENT    20  /**< Alignments from 2 to 128 are available
                                     where maximum alignment defined as
                                     MM_MAX_ALIGNMENT power of 2 */
#define MM_MAX_NAME_LEN     32  /**< TODO */

/* Put all function (execution code) into  dtext_vle section, aka __COLD_CODE */
__START_COLD_CODE
/**************************************************************************//**
 @Group         fsl_lib_g   Utility Library Application Programming Interface

 @Description   External routines.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         slob_g Flexible Memory Manager

 @Description   Flexible Memory Manager module functions,definitions and enums.
                (All of the following functions,definitions and enums can be found in slob_ext.h)

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      slob_init

 @Description   Initializes a new MM object.

                It initializes a new memory block consisting of base address
                and size of the available memory by calling to MemBlock_Init
                routine. It is also initializes a new free block for each
                by calling FreeBlock_Init routine, which is pointed to
                the almost all memory started from the required alignment
                from the base address and to the end of the memory.
                The handle to the new MM object is returned via "MM"
                argument (passed by reference).

 @Param[in]     slob    - Handle to the MM object.
 @Param[in]     base    - Base address of the MM.
 @Param[in]     size    - Size of the MM.

 @Return        0 is returned on success. E_NOMEMORY is returned if the new MM object or a new free block can not be initialized.
*//***************************************************************************/
int slob_init(fsl_handle_t *slob, uint64_t base, uint64_t size);

/**************************************************************************//**
 @Function      slob_free

 @Description   Releases memory allocated for MM object.

 @Param[in]     slob - Handle of the MM object.
*//***************************************************************************/
void slob_free(fsl_handle_t slob);

/**************************************************************************//**
 @Function      slob_get_base

 @Description   Gets the base address of the required MM objects.

 @Param[in]     slob - Handle to the MM object.

 @Return        base address of the block.
*//***************************************************************************/
uint64_t slob_get_base(fsl_handle_t slob);

/**************************************************************************//**
 @Function      slob_add

 @Description   Adds a new memory block for memory allocation.

                When a new memory block is initialized and added to the
                memory list, it calls to MM_AddFree routine to add the
                new free block to the free lists.

 @Param[in]     slob    - Handle to the MM object.
 @Param[in]     base    - Base address of the memory block.
 @Param[in]     size    - Size of the memory block.

 @Return        0 on success, otherwise returns an error code.
*//***************************************************************************/
int slob_add(fsl_handle_t slob, uint64_t base, uint64_t size);

/**************************************************************************//**
 @Function      slob_get

 @Description   Allocates a block of memory according to the given size and the alignment.

                The Alignment argument tells from which
                free list allocate a block of memory. 2^alignment indicates
                the alignment that the base address of the allocated block
                should have. So, the only values 1, 2, 4, 8, 16, 32 and 64
                are available for the alignment argument.
                The routine passes through the specific free list of free
                blocks and seeks for a first block that have anough memory
                that  is required (best fit).
                After the block is found and data is allocated, it calls
                the internal MM_CutFree routine to update all free lists
                do not include a just allocated block. Of course, each
                free list contains a free blocks with the same alignment.
                It is also creates a busy block that holds
                information about an allocated block.

 @Param[in]     slob        - Handle to the MM object.
 @Param[in]     size        - Size of the MM.
 @Param[in]     alignment   - Index as a power of two defines a required
                              alignment (in bytes); Should be 1, 2, 4, 8, 16, 32 or 64
 @Param[in]     name        - The name that specifies an allocated block.

 @Return        base address of an allocated block ILLEGAL_BASE if can't allocate a block
*//***************************************************************************/
uint64_t slob_get(fsl_handle_t slob, uint64_t size, uint64_t alignment, char *name);

/**************************************************************************//**
 @Function      slob_get_force

 @Description   Force memory allocation.

                It means to allocate a block of memory of the given
                size from the given base address.
                The routine checks if the required block can be allocated
                (that is it is free) and then, calls the internal MM_CutFree
                routine to update all free lists do not include that block.

 @Param[in]     slob    - Handle to the MM object.
 @Param[in]     base    - Base address of the MM.
 @Param[in]     size    - Size of the MM.
 @Param[in]     name    - Name that specifies an allocated block.

 @Return        base address of an allocated block, ILLEGAL_BASE if can't allocate a block.
*//***************************************************************************/
uint64_t slob_get_force(fsl_handle_t slob, uint64_t base, uint64_t size, char *name);

/**************************************************************************//**
 @Function      slob_get_force_min

 @Description   Allocates a block of memory according to the given size, the alignment and minimum base address.

                The Alignment argument tells from which
                free list allocate a block of memory. 2^alignment indicates
                the alignment that the base address of the allocated block
                should have. So, the only values 1, 2, 4, 8, 16, 32 and 64
                are available for the alignment argument.
                The minimum baser address forces the location of the block
                to be from a given address onward.
                The routine passes through the specific free list of free
                blocks and seeks for the first base address equal or smaller
                than the required minimum address and end address larger than
                than the required base + its size - i.e. that may contain
                the required block.
                After the block is found and data is allocated, it calls
                the internal MM_CutFree routine to update all free lists
                do not include a just allocated block. Of course, each
                free list contains a free blocks with the same alignment.
                It is also creates a busy block that holds
                information about an allocated block.

 @Param[in]     slob        - Handle to the MM object.
 @Param[in]     size        - Size of the MM.
 @Param[in]     alignment   - Index as a power of two defines a required
                              alignment (in bytes); Should be 1, 2, 4, 8, 16, 32 or 64
 @Param[in]     min         - The minimum base address of the block.
 @Param[in]     name        - Name that specifies an allocated block.

 @Return        base address of an allocated block,ILLEGAL_BASE if can't allocate a block.
*//***************************************************************************/
uint64_t slob_get_force_min(fsl_handle_t slob,
                            uint64_t size,
                            uint64_t alignment,
                            uint64_t min,
                            char     *name);

/**************************************************************************//**
 @Function      slob_put

 @Description   Puts a block of memory of the given base address back to the memory.

                It checks if there is a busy block with the
                given base address. If not, it returns 0, that
                means can't free a block. Otherwise, it gets parameters of
                the busy block and after it updates lists of free blocks,
                removes that busy block from the list by calling to MM_CutBusy
                routine.
                After that it calls to MM_AddFree routine to add a new free
                block to the free lists.

 @Param[in]     slob    - Handle to the MM object.
 @Param[in]     base    - Base address of the MM.

 @Return         The size of bytes released, 0 if failed.
*//***************************************************************************/
uint64_t slob_put(fsl_handle_t slob, uint64_t base);

/**************************************************************************//**
 @Function      slob_put_force

 @Description   Releases a block of memory of the required size from the required base address.

                First, it calls to MM_CutBusy routine
                to cut a free block from the busy list. And then, calls to
                MM_AddFree routine to add the free block to the free lists.

 @Param[in]     slob    - Handle to the MM object.
 @Param[in]     base    - Base address of of a block to free.
 @Param[in]     size    - Size of a block to free.

 @Return        The number of bytes released, 0 on failure.
*//***************************************************************************/
uint64_t slob_put_force(fsl_handle_t slob, uint64_t base, uint64_t size);

/**************************************************************************//**
 @Function      slob_dump

 @Description   Prints results of free and busy lists.

 @Param[in]     slob        - Handle to the MM object.
*//***************************************************************************/
void slob_dump(fsl_handle_t slob);

/**************************************************************************//**
 @Function      slob_in_range

 @Description   Checks if a specific address is in the memory range of the passed MM object.

 @Param[in]     slob    - Handle to the MM object.
 @Param[in]     addr    - The address to be checked.

 @Return        Non-zero value if the address is in the address range of the block, zero otherwise.
*//***************************************************************************/
int slob_in_range(fsl_handle_t slob, uint64_t addr);

/**************************************************************************//**
 @Function      slob_get_free_mem_size

 @Description   Returns the size (in bytes) of free memory.

 @Param[in]     slob    - Handle to the MM object.

 @Return        Free memory size in bytes.
*//***************************************************************************/
uint64_t slob_get_free_mem_size(fsl_handle_t slob);

/** @} */ /* end of slob_g group */
/** @} */ /* end of fsl_lib_g group */

__START_COLD_CODE
#endif /* __FSL_SLOB_H */
