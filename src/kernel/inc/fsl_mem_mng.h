#ifndef __SYS_MEM_MNG_H
#define __SYS_MEM_MNG_H

#include "common/types.h"
#include "common/errors.h"


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
#define MEMORY_ATTR_CACHEABLE           0x00000001
                                        /**< Memory is cacheable */
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

 @Return        E_OK on success; Error code otherwise.
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


#endif /* __SYS_MEM_MNG_H */
