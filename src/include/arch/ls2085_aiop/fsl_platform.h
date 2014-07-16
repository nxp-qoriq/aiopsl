/**
 @File          fsl_platform.h
 
 @Description   This file contains typedefs for dynamic memory allocation.
 
		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#ifndef __FSL_PLATFORM_AIOP_H
#define __FSL_PLATFORM_AIOP_H

/**************************************************************************//**
 @Group         mem_mng_g_id Memory Management
 @Description   Prototypes, externals and typedefs for system  memory management.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Memory Partition Identifiers.
                Used as a parameter for fsl_os_xmalloc() and fsl_os_malloc(). 
                Note that not all memory partitions are supported by all
                platforms. Every platform may select which memory partitions
                to support.
*//***************************************************************************/
typedef enum memory_partition_id {
    MEM_PART_DP_DDR = 1,/**< Primary DDR non-cacheable memory partition */
    MEM_PART_SYSTEM_DDR,    /**< Secondary DDR non-cacheable memory partition */
    MEM_PART_CPC_SRAM,                 /**< CPC SRAM cacheable memory partition */
    MEM_PART_SH_RAM,                   /**< Shared-SRAM memory */
    MEM_PART_PEB,                      /**< Packet-Express-Buffer memory partition */
    MEM_PART_INVALID                   /**< Invalid memory partition */
} e_memory_partition_id;

/** @} */ /* end of sytem_mem_management_id  group */
#endif /* __FSL_PLATFORM_AIOP_H */

