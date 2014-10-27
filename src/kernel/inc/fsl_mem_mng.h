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

#ifndef __FSL_SYS_MEM_MNG_H
#define __FSL_SYS_MEM_MNG_H

#include "common/types.h"
#include "fsl_errors.h"


/**************************************************************************//**
 @Group         sys_grp     System Interfaces

 @Description   Bare-board system programming interfaces.

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group         sys_mem_grp     System Memory Management Service

 @Description   Bare-board system interface for memory management.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Anchor        mem_attr

 @Collection    Memory Attributes

                Various attributes of memory partitions. These values may be
                or'ed together to create a mask of all memory attributes.
 @{
*//***************************************************************************/

/**< No memory attribute */
#define MEMORY_ATTR_NONE          0x00000000
/**< Memory is cacheable */
#define MEMORY_ATTR_CACHEABLE           0x00000001 
/**< It is possible to make dynamic memory allocation */
#define MEMORY_ATTR_MALLOCABLE          0x00000002
/* @} */


int sys_register_virt_mem_mapping(uint64_t virt_addr, uint64_t phys_addr, uint64_t size);

int sys_unregister_virt_mem_mapping(uint64_t virt_addr);


/**************************************************************************//**
 @Function      SYS_VirtToPhys

 @Description   Translate virtual address to physical one.

 @Param[in]     addr    - Virtual address

*//***************************************************************************/
dma_addr_t sys_virt_to_phys(void *addr);

/**************************************************************************//**
 @Function      SYS_PhysToVirt

 @Description   Translate physical address to virtual one.

 @Param[in]     addr    - Physical address

*//***************************************************************************/
void * sys_phys_to_virt(dma_addr_t addr);



#define SYS_DEFAULT_HEAP_PARTITION  0   /**< Partition ID for default heap */
#define MEM_PART_DEFAULT_HEAP_PARTITION  1  /**< Partition ID for default heap */


/**************************************************************************//**
 @Function      SYS_MemAlloc

 @Description   Allocate a memory block from a specific partition and with
                specific attributes.

 @Param[in]     partitionId - Requested memory partition ID
 @Param[in]     size        - Requested memory size
 @Param[in]     alignment   - Requested memory alignment
 @Param[in]     info        - Allocation information string (for debug)
 @Param[in]     filename    - Caller file name (for debug)
 @Param[in]     line        - Caller line number (for debug)

 @Return        Pointer to allocated memory; NULL on failure.
*//***************************************************************************/
void * sys_mem_alloc(int         partition_id,
                    uint32_t    size,
                    uint32_t    alignment,
                    char        *info,
                    char        *filename,
                    int         line);

/**************************************************************************//**
 @Function      SYS_MemFree

 @Description   Free a memory block that was previously allocated using the
                SYS_MemAlloc() routine.

 @Param[in]     p_Memory - Pointer to the memory block

 @Return        None.
*//***************************************************************************/
void sys_mem_free(void *p_memory);

/**************************************************************************//**
 @Function      SYS_GetAvailableMemPartition

 @Description   Returns an available memory partition ID.

                The returned ID may be used for registration of a new memory
                partition.

 @Return        An available memory partition ID.
*//***************************************************************************/
int sys_get_available_mem_partition(void);

/**************************************************************************//**
 @Function      SYS_RegisterMemPartition

 @Description   Register a new memory partition to the system's memory manager.

                Note that if \c f_UserMalloc and \c f_UserFree are not NULL,
                the system will not manage the partition, but only record
                allocations and de-allocations for debug purposes (providing
                that \c enableDebug is set to '1').

 @Param[in]     partitionId     - Memory partition ID
 @Param[in]     baseAddress     - Base address of memory partition
 @Param[in]     size            - Size (in bytes) of memory partition
 @Param[in]     attributes      - Memory attributes mask (a combination of MEMORY_ATTR_x flags)
 @Param[in]     name            - Memory partition name (up to 32 bytes).
 @Param[in]     f_UserMalloc    - User's memory allocation routine, for bypassing the
                                  default memory manager; Set to NULL for default operation.
 @Param[in]     f_UserFree      - User's memory freeing routine, for bypassing the
                                  default memory manager; Set to NULL for default operation.
 @Param[in]     enableDebug     - '1' to enable memory leaks debug; '0' to disable.

 @Return        Pointer to allocated memory; NULL on failure.
*//***************************************************************************/
int sys_register_mem_partition(int        partition_id,
                                 uintptr_t  base_address,
                                 uint64_t   size,
                                 uint32_t   attributes,
                                 char       name[],
                                 void *     (*f_user_malloc)(uint32_t size, uint32_t alignment),
                                 void       (*f_user_free)(void *p_addr),
                                 int        enable_debug);

/**************************************************************************//**
 @Function      SYS_UnregisterMemPartition

 @Description   Unregister a specific memory partition.

                Note that if \c f_UserMalloc and \c f_UserMalloc are not NULL,
                the system will not manage the partition, but only record
                allocations and de-allocations for debug purposes (providing
                that \c enableDebug is set to '1').

 @Param[in]     partitionId     - Memory partition ID

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int sys_unregister_mem_partition(int partition_id);

/**************************************************************************//**
 @Function      SYS_GetMemPartitionBase

 @Description   Retrieves the base address of a given memory partition.

 @Param[in]     partitionId - Memory partition ID

 @Return        Base address of the memory partition.
*//***************************************************************************/
uint64_t sys_get_mem_partition_base(int partition_id);

/**************************************************************************//**
 @Function      SYS_GetMemPartitionAttributes

 @Description   Retrieves the memory attributes mask of a given memory partition.

 @Param[in]     partitionId - Memory partition ID

 @Return        Memory attributes of the memory partition.
*//***************************************************************************/
uint32_t sys_get_mem_partition_attributes(int partition_id);

/**************************************************************************//**
 @Function      SYS_PrintMemPartitionDebugInfo

 @Description   Prints information and statistics of the given memory partition,
                and possibly reports known memory leaks.

 @Param[in]     partitionId - Memory partition ID
 @Param[in]     reportLeaks - '1' to report memory leaks; 0 otherwise.

 @Return        None.
*//***************************************************************************/
void sys_print_mem_partition_debug_info(int partition_id, int report_leaks);

/** @} */ /* end of sys_mem_grp */
/** @} */ /* end of sys_grp */


#endif /* __FSL_SYS_MEM_MNG_H */
