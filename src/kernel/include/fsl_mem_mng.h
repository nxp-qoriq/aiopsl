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

#ifndef __FSL_SYS_MEM_MNG_H
#define __FSL_SYS_MEM_MNG_H

#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_list.h"
#include "buffer_pool.h"
#include "fsl_platform.h"
#include "fsl_aiop_common.h"
#include "fsl_malloc.h"

extern struct aiop_init_info g_init_data;

#ifndef INVALID_PHY_ADDR
	#define INVALID_PHY_ADDR (uint64_t)(-1)
#endif /* INVALID_PHY_ADDR */

#ifndef CONST_1G
	#define CONST_1G ((uint64_t)1 * GIGABYTE)
#endif /* CONST_1G */

#ifndef CONST_2G
	#define CONST_2G ((uint64_t)2 * GIGABYTE)
#endif /* CONST_2G */

#ifndef CONST_3G
	#define CONST_3G ((uint64_t)3 * GIGABYTE)
#endif /* CONST_3G */

#ifndef CONST_4G
	#define CONST_4G ((uint64_t)4 * GIGABYTE)
#endif /* CONST_4G */

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

/**< Memory partition for physical address allocation  through fsl_get_mem() */
#define MEMORY_ATTR_PHYS_ALLOCATION	0x00000008

#define MEM_MNG_MAX_PARTITION_NAME_LEN      32


/* @} */
/* Put all function (execution code) into  dtext_vle section,aka __COLD_CODE */
__START_COLD_CODE

/**************************************************************************//**
 @Description   Initial Memory management, used for allocations during boot.
 *//***************************************************************************/
#pragma pack(push, 1)
struct initial_mem_mng
{
	/* The lock must be aligned to a double word boundary. */
	uint64_t    lock;
	uint64_t base_paddress;
	uint32_t base_vaddress;
	uint64_t size;
	uint64_t curr_ptr;
};
#pragma pack(pop)

/**************************************************************************//**
 @Description   Memory partition information for physical address allocation
 *//***************************************************************************/
typedef struct t_mem_mng_phys_addr_alloc_info
{
    char        name[MEM_MNG_MAX_PARTITION_NAME_LEN];
    uint64_t    base_paddress;
    uint64_t    size;
    uint32_t    attributes;
} t_mem_mng_phys_addr_alloc_info;

/**************************************************************************//**
 @Description   Memory partition information
 *//***************************************************************************/
typedef struct t_mem_mng_partition_info
{
    char        name[MEM_MNG_MAX_PARTITION_NAME_LEN];
    uint64_t    base_address;
    uint64_t    size;
    uint32_t    attributes;
#if ENABLE_DEBUG_ENTRIES
    uint32_t    current_usage;
    uint32_t    maximum_usage;
    uint32_t    total_allocations;
    uint32_t    total_deallocations;
#endif

} t_mem_mng_partition_info;

/**************************************************************************//**
 @Description   Memory management partition control structure
 *//***************************************************************************/
typedef struct t_mem_mng_phys_addr_alloc_partition
{
    int                              id;             /**< Partition ID */
    uint64_t                         h_mem_manager;   /**< Memory manager handle */
    t_mem_mng_phys_addr_alloc_info   info;           /**< Partition information */
    uint64_t *                        lock;
    int                              was_initialized;
} t_mem_mng_phys_addr_alloc_partition;

/**************************************************************************//**
 @Description   Memory management partition control structure
 *//***************************************************************************/
typedef struct t_mem_mng_partition
{
    int                     id;             /**< Partition ID */
    uint64_t                h_mem_manager;    /**< Memory manager handle */
    int                     enable_debug;    /**< '1' to track malloc/free operations */
    int                     was_initialized;
    list_t                  mem_debug_list;   /**< List of allocation entries (for debug) */
    list_t                  node;
    t_mem_mng_partition_info   info;           /**< Partition information */
    uint64_t *               lock;
} t_mem_mng_partition;

/**************************************************************************//**
 @Description   Memory management module internal parameters
 *//***************************************************************************/
struct t_mem_mng
{
    t_mem_mng_partition mem_partitions_array[PLATFORM_MAX_MEM_INFO_ENTRIES];
                /**< List of partition control structures */
    t_mem_mng_phys_addr_alloc_partition
           phys_allocation_mem_partitions_array[PLATFORM_MAX_MEM_INFO_ENTRIES];
                /**< List of partition for fsl_get_mem function() control structures */
    uint32_t    mem_partitions_initialized;
    void * h_boot_mem_mng;
    struct buffer_pool slob_bf_pool;

};

#define SYS_DEFAULT_HEAP_PARTITION  0   /**< Partition ID for default heap */

/**************************************************************************//**
 @Function      sys_virt_to_phys

 @Description   Translate virtual address to physical one.

 @Param[in]     virt_addr    - Virtual address

 @Return        Physical address; INVALID_PHY_ADDR on failure.
*//***************************************************************************/
uint64_t sys_virt_to_phys(void *virt_addr);


/**************************************************************************//**
 @Function      sys_phys_to_virt

 @Description   Translate physical address to virtual one.

 @Param[in]     phy_addr    - Physical address

 @Return        Virtual address; NULL on failure.
*//***************************************************************************/
void* sys_phys_to_virt(uint64_t phy_addr);


/**************************************************************************//**
 @Function      sys_fast_virt_to_phys

 @Description   Fast translation of virtual address to physical one.
                There is no error checking.
                The assumption is that the virtual address is valid
                for the memory partition.

 @Param[in]     vaddr    - Virtual address
 @Param[in]     id       - Memory Partition Identifier

 @Return        Physical address; INVALID_PHY_ADDR on failure.
*//***************************************************************************/
inline uint64_t sys_fast_virt_to_phys(void *vaddr, e_memory_partition_id id)
{
	switch (id)
	{
	case MEM_PART_DP_DDR:
		return ((uint64_t)vaddr - g_init_data.sl_info.dp_ddr_vaddr +
			g_init_data.sl_info.dp_ddr_paddr);
	case MEM_PART_SYSTEM_DDR:
		return ((uint64_t)vaddr - g_init_data.sl_info.sys_ddr1_vaddr +
			g_init_data.sl_info.sys_ddr1_paddr);
	case MEM_PART_PEB:
		return ((uint64_t)vaddr - g_init_data.sl_info.peb_vaddr +
			g_init_data.sl_info.peb_paddr);
	}
	return INVALID_PHY_ADDR;
}


/**************************************************************************//**
 @Function      sys_fast_phys_to_virt

 @Description   Fast translation of physical address to virtual one.
                There is no error checking.
                The assumption is that the physical address is valid
                for the memory partition.

 @Param[in]     phy_addr    - Physical address
 @Param[in]     id          - Memory Partition Identifier

 @Return        Virtual address; NULL on failure.
*//***************************************************************************/
inline void* sys_fast_phys_to_virt(uint64_t phy_addr, e_memory_partition_id id)
{
	switch (id)
	{
	case MEM_PART_DP_DDR:
		return (void*)(phy_addr - g_init_data.sl_info.dp_ddr_paddr +
			       g_init_data.sl_info.dp_ddr_vaddr);
	case MEM_PART_SYSTEM_DDR:
		return (void*)(phy_addr - g_init_data.sl_info.sys_ddr1_paddr +
			       g_init_data.sl_info.sys_ddr1_vaddr);
	case MEM_PART_PEB:
		return (void*)(phy_addr - g_init_data.sl_info.peb_paddr +
			       g_init_data.sl_info.peb_vaddr);
	}
	return NULL;
}



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
               paddr should be allocated by fsl_get_mem()

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
#define MEM_PART_SYSTEM_DDR1_BOOT_MEM_MNG MEM_PART_LAST+1
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

/*****************************************************************************/
int boot_get_mem(struct initial_mem_mng* boot_mem_mng,
                 const uint64_t size,uint64_t* paddr);
/*****************************************************************************/
int boot_get_mem_virt(struct initial_mem_mng* boot_mem_mng,
                      const uint64_t size,uint32_t* vaddr);

/** @} */ /* end of sys_mem_grp */
/** @} */ /* end of sys_grp */
__END_COLD_CODE

#endif /* __FSL_SYS_MEM_MNG_H */
