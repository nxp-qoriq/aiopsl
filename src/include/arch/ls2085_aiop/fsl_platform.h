/**

 @File          fsl_platform.h

 @Description   LS2100A Pltform external definitions and structures.
*//***************************************************************************/
#ifndef __FSL_PLATFORM_AIOP_H
#define __FSL_PLATFORM_AIOP_H

/**************************************************************************//**
 @Description   Memory Partition Identifiers

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


#endif /* __FSL_PLATFORM_AIOP_H */

