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

#ifndef __FSL_SYS_MEM_MNG_H
#define __FSL_SYS_MEM_MNG_H

#include "common/types.h"
#include "fsl_errors.h"
#include "mem_mng_util.h"


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
#define MEMORY_ATTR_NONE           0x00000000
/**< Memory is cacheable */
#define MEMORY_ATTR_CACHEABLE       0x00000001

/**< Memory is non-cacheable */
#define MEMORY_ATTR_NON_CACHEABLE   0x00000002
/**< It is possible to make dynamic memory allocation */
#define MEMORY_ATTR_MALLOCABLE      0x00000004

/**< Memory partition for physical address allocation  through fsl_os_get_mem() */
#define MEMORY_ATTR_PHYS_ALLOCATION	0x00000008


/* @} */
/* Put all function (execution code) into  dtext_vle section,aka __COLD_CODE */
__START_COLD_CODE



/**************************************************************************//**
 @Function      SYS_VirtToPhys

 @Description   Translate virtual address to physical one.

 @Param[in]     addr    - Virtual address

*//***************************************************************************/
uint64_t sys_virt_to_phys(void *addr);

#define SYS_DEFAULT_HEAP_PARTITION  0   /**< Partition ID for default heap */



/**************************************************************************//**
 @Function      sys_shram_alloc

 @Description   Allocate a memory block from shared ram and with
                specific attributes.

 @Param[in]     size        - Requested memory size
 @Param[in]     alignment   - Requested memory alignment
 @Param[in]     info        - Allocation information string (for debug)
 @Param[in]     filename    - Caller file name (for debug)
 @Param[in]     line        - Caller line number (for debug)

 @Return        Pointer to allocated memory; NULL on failure.
*//***************************************************************************/
void * sys_shram_alloc(uint32_t    size,
                    uint32_t    alignment,
                    char        *info,
                    char        *filename,
                    int         line);

/**************************************************************************//**
 @Function      sys_shram_free

 @Description   Free a memory block that was previously allocated using the
                sys_shram_alloc() routine.

 @Param[in]     mem - Pointer to the memory block

 @Return        None.
*//***************************************************************************/
void sys_shram_free(void *mem);

/**************************************************************************//**
 @Function      SYS_GetAvailableMemPartition

 @Description   Returns an available memory partition ID.

                The returned ID may be used for registration of a new memory
                partition.

 @Return        An available memory partition ID.
*//***************************************************************************/
int sys_get_available_mem_partition(void);

/**************************************************************************//**
 @Function      sys_register_phys_addr_alloc_partition

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
int sys_register_phys_addr_alloc_partition(int        partition_id,
		                                   uint64_t  base_paddress,
                                           uint64_t   size,
                                            uint32_t   attributes,
                                            char       name[]);

/**************************************************************************//**
 @Function      sys_register_mem_partition

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
 @Function      sys_get_phys_addr_alloc_partition_info

 @Description   Retrieves memory partition details of a given memory partition.

 @Param[in]     partitionId - Memory partition ID

 @Param[out]    partition_info - Info about given partition.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int sys_get_phys_addr_alloc_partition_info(int partition_id,
                                           t_mem_mng_phys_addr_alloc_info* partition_info);


/**************************************************************************//**
 @Function      sys_get_mem_partition_info

 @Description   Retrieves memory partition details of a given memory partition.

 @Param[in]     partitionId - Memory partition ID

 @Param[out]    partition_info - Info about given partition.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/

int sys_get_mem_partition_info(int partition_id,
                               t_mem_mng_partition_info* partition_info);
#if 0
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
#endif

/**************************************************************************//**
@Function      sys_get_phys_mem

@Description   Allocates contiguous block of memory with the specified
               alignment and from the specified  memory partition.
@Param[in]     mem_partition_id    Memory partition ID; The value zero must
                                    be mapped to the default heap partition.
@Param[in]     alignment           Required memory alignment (in bytes).
@Param[out]    paddr               A valid allocated physical address if success,
                                   NULL if failure.
@Return        0                   on success,
               -ENOMEM (not enough memory to allocate)or
               -EINVAL ( invalid memory partition ) on failure.
*//***************************************************************************/
int sys_get_phys_mem(uint64_t size, int mem_partition_id,  uint64_t alignment,
                uint64_t* paddr);

/**************************************************************************//**
@Function     sys_put_phys_mem

@Description   Frees the memory block pointed to by "paddr".
               paddr should be allocated by fsl_os_get_mem()

@Param[in]    paddr  An address to be freed.

*//***************************************************************************/
void  sys_put_phys_mem(uint64_t paddr);

/**************************************************************************//**
@Function      sys_mem_exists

@Description   Returns  if the provided  memory partition exists

@Param[in]    mem_partition_id   A memory partition id
@Return        1                 if exists,
               0                 if does not exist
*//***************************************************************************/
int sys_mem_exists(int mem_partition_id);

void  sys_mem_partitions_init_complete();


extern const  uint32_t g_boot_mem_mng_size;
#define MEMORY_PARTITIONS\
{   /* Memory partition ID                  Phys. Addr.  Virt. Addr.  Size , Attributes */\
	{MEM_PART_SYSTEM_DDR1_BOOT_MEM_MNG,  0xFFFFFFFF,  0xFFFFFFFF, g_boot_mem_mng_size,\
	        MEMORY_ATTR_NONE, "BOOT MEMORY MANAGER"},\
	{MEM_PART_DP_DDR,                    0xFFFFFFFF,  0xFFFFFFFF,  0xFFFFFFFF,\
		MEMORY_ATTR_PHYS_ALLOCATION,"DP_DDR"},\
	{MEM_PART_MC_PORTALS,                0xFFFFFFFF,  0xFFFFFFFF, 0xFFFFFFFF,\
		MEMORY_ATTR_NONE,"MC Portals"},\
	{MEM_PART_CCSR,                      0xFFFFFFFF,  0xFFFFFFFF, 0xFFFFFFFF,\
		MEMORY_ATTR_NONE,"SoC CCSR"  },\
	{MEM_PART_SH_RAM,                    0xFFFFFFFF,   0xFFFFFFFF,0xFFFFFFFF,\
		MEMORY_ATTR_MALLOCABLE,"Shared-SRAM"},\
	{MEM_PART_PEB,                        0xFFFFFFFF,  0xFFFFFFFF,0xFFFFFFFF,\
		MEMORY_ATTR_PHYS_ALLOCATION,"PEB"},\
	{MEM_PART_SYSTEM_DDR,                 0xFFFFFFFF,  0xFFFFFFFF,0xFFFFFFFF,\
		MEMORY_ATTR_PHYS_ALLOCATION,"SYSTEM_DDR"},\
}
/** @} */ /* end of sys_mem_grp */
/** @} */ /* end of sys_grp */
__END_COLD_CODE

#endif /* __FSL_SYS_MEM_MNG_H */
