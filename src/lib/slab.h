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

/**************************************************************************//*
@File          slab.h

@Description   This is slab internal header file which includes all the
		architecture specific implementation defines.

@Cautions      This file is private for AIOP.
*//***************************************************************************/
#ifndef __SLAB_H
#define __SLAB_H

#include "fsl_slab.h"
#include "platform.h"


#define SLAB_HW_HANDLE(SLAB) ((uint32_t)(SLAB)) /**< Casted HW handle */

/**************************************************************************//**
@Description   SLAB common internal macros
*//***************************************************************************/
#define SLAB_HW_POOL_SET      0x00000001
/**< Flag which indicates that this SLAB handle is HW pool */
#define SLAB_IS_HW_POOL(SLAB) (SLAB_HW_HANDLE(SLAB) & SLAB_HW_POOL_SET)
/**< Slab handle is HW pool */

/**************************************************************************//**
@Description   SLAB AIOP HW pool internal macros
*//***************************************************************************/
/*
 *  HW SLAB structure
 *
 * 31----------23--------15------1--------0
 * | HW accel   |cluster | VP ID |HW flg  |
 * ----------------------------------------
 */
#define SLAB_VP_POOL_MASK      0x01FFFFFE       /*Cluster and VP ID - 23 bits */
#define SLAB_POOL_ID_MASK      0x00007FFF       /*VP ID - 15 bits */
#define SLAB_CLUSTER_ID_MASK   0x000000FF       /*Cluster ID is length of 8 bits*/
#define SLAB_VP_POOL_MAX       (SLAB_VP_POOL_MASK >> 1)
/**< Maximal number to be used as VP id */
#define SLAB_VP_POOL_SHIFT     1
#define SLAB_HW_ACCEL_MASK     0xFF000000
#define SLAB_VP_POOL_GET(SLAB) \
	((uint32_t)((SLAB_HW_HANDLE(SLAB) & SLAB_VP_POOL_MASK) >> 1))

/*Those macros should be use over SLAB_VP_POOL_GET/SET*/
#define SLAB_POOL_ID_GET(POOL_ID) ((uint32_t)(POOL_ID & SLAB_POOL_ID_MASK))
#define SLAB_CLUSTER_ID_GET(CLUSTER) ((uint16_t)((CLUSTER >> 15) & SLAB_CLUSTER_ID_MASK))
#define SLAB_CLUSTER_ID_SET(CLUSTER) ((uint32_t)((uint32_t)CLUSTER << 15))

/**< Returns slab's virtual pool id*/

#define SLAB_HW_META_OFFSET     8 /**< metadata offset in bytes */
#define SLAB_SIZE_GET(SIZE)     (uint16_t)((SIZE) - SLAB_HW_META_OFFSET)
/**< Real buffer size used by user */
#define SLAB_SIZE_SET(SIZE)     (uint16_t)((SIZE) + SLAB_HW_META_OFFSET)
/**< Buffer size that needs to be set for CDMA, including metadata */

#define SLAB_HW_POOL_CREATE(VP) \
((((VP) & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) \
	| SLAB_HW_POOL_SET)
/**< set slab's virtual pool id shifted to have space for slab hardware pool bit 0*/
/**************************************************************************//**
@Description   SLAB module defaults macros
*//***************************************************************************/
/** bpid, user required size, partition */

#define SLAB_FAST_MEMORY        MEM_PART_SH_RAM
#define SLAB_DDR_MEMORY         MEM_PART_DP_DDR
#define SLAB_NUM_MEM_PARTITIONS MEM_PART_LAST
#define SLAB_NUM_BPIDS_USED_FOR_DPNI BPIDS_USED_FOR_POOLS_IN_DPNI
#define SLAB_DEFAULT_ALIGN      8
#define SLAB_MAX_NUM_VP_SHRAM   1000
#define SLAB_MAX_NUM_VP_DDR     64
#define SLAB_BUFFER_TO_MANAGE_IN_DDR  1
#define IS_POWER_VALID_ALLIGN(_val, _max_size) \
    (((((uint32_t)_val) <= (_max_size)) && ((((uint32_t)_val) & (~((uint32_t)_val) + 1)) == ((uint32_t)_val))))



/* Maximum number of BMAN pools used by the slab virtual pools */
#define SLAB_MAX_BMAN_POOLS_NUM 16

/**************************************************************************//**
@Description   Information for every bpid
*//***************************************************************************/
struct slab_bpid_info {
	uint16_t bpid;
	/**< Bpid - slabs bman id */
	uint16_t size;
	/**< Size of memory the bman pool is taking  */
	e_memory_partition_id mem_pid;
	/**< Memory Partition Identifier */
	uint16_t alignment;
	/**< Buffer alignment */
	uint32_t num_buffers;
	/**< Number of MAX requested buffers per pool */
};

/**************************************************************************//**
@Description   Information to be kept about every HW pool inside DDR
*//***************************************************************************/
struct slab_hw_pool_info {
	uint32_t flags;
	/**< Control flags */
	uint16_t buff_size;
	/**< Maximal buffer size including 8 bytes of CDMA metadata */
	uint16_t pool_id;
	/**< BMAN pool ID */
	uint16_t alignment;
	/**< Buffer alignment */
	uint16_t mem_pid;
	/**< Memory partition for buffers allocation */
	uint32_t total_num_buffs;
	/**< Number of allocated buffers per pool */
};

/**************************************************************************//**
@Description   Information to be kept about SLAB module
*//***************************************************************************/
struct slab_module_info {
	uint32_t fdma_flags;
	/**< Flags to be used for FDMA release/acquire */
	uint32_t fdma_dma_flags;
	/**< Flags to be used for FDMA dma data,
	 * not including fdma_dma_data_access_options */
	struct  slab_hw_pool_info *hw_pools;
	/**< List of BMAN pools */
	uint16_t icid;
	/**< CDMA ICID to be used for FDMA release/acquire*/
	/* TODO uint8_t spinlock; */
	/**< Spinlock placed at SHRAM */
	uint8_t num_hw_pools;
	/**< Number of BMAN pools */
};

/* Virtual Pool structure */
struct slab_v_pool {
	int32_t max_bufs;
	/**< Number of MAX requested buffers per pool */
	int32_t committed_bufs;
	/**< Number of requested committed buffers per pool */
	int32_t allocated_bufs;
	/**< Number of allocated buffers per pool */
	uint8_t spinlock;
	/**< spinlock for locking the pool */
	uint8_t flags;
	/**< Flags to use when using the pool - unused  */
	uint16_t bman_array_index;
	/**< Index of bman pool that the buffers were taken from*/
	slab_release_cb_t *callback_func;
	/**< Callback function to release virtual pool  */
};

/* BMAN Pool structure */
struct slab_bman_pool_desc {
	int32_t remaining;
	/**< Number of remaining buffers in the bman pool */
	uint8_t spinlock;
	/**< Spinlock for locking bman pool */
	uint8_t flags;
	/**< Flags to use when using the pool - unused  */
	uint16_t bman_pool_id;
	/**< Bman pool id - bpid  */
};

/* virtual root pool struct - holds all virtual pools data */
struct slab_virtual_pools_main_desc {
	struct slab_v_pool *virtual_pool_struct; /*cluster 0*/
	/**< Pointer to virtual pools array*/
	uint64_t *slab_context_address; /*0 is not used*/
	/**< memory to buffer for virtual pools array*/
	uint16_t shram_count;
	/**< Counter for pools in shram*/
	uint32_t num_clusters;
	/**< number of cluster for pools in DDR*/
	uint8_t flags;
	/**< Flags to use when using the pools - unused  */
	uint8_t global_spinlock;
	/**< Spinlock for locking the global virtual root pool */
};

/* defined array of available buffer sizes that can be requested*/
#define SLAB_BUFF_SIZES_ARR	\
	{ \
	{24,   0, 0, 0},  \
	{56,   0, 0, 0},  \
	{120,   0, 0, 0}, \
	{248,   0, 0, 0}, \
	{504,   0, 0, 0}, \
	{1016,  0, 0, 0}, \
	{2040,  0, 0, 0}, \
	{4088,  0, 0, 0}, \
	{8184,  0, 0, 0}, \
	{16376, 0, 0, 0}, \
	{32760, 0, 0, 0}  \
	}


struct request_table_info{
	uint16_t buff_size;
	/**< Available buffer sizes not including 8 bytes of CDMA metadata */
	uint32_t extra;
	/**< Number of extra requested buffers */
	uint32_t max;
	/**< Number of max requested buffers */
	uint32_t committed_bufs;
	/**< Number of requested committed buffers */
	uint16_t alignment;
	/**< Buffer alignment */
};

struct early_init_request_table{
	struct request_table_info *table_info;
	/**< Tables to store early initialization request for buffers. */
};
struct memory_types_table{
	struct early_init_request_table *mem_pid_buffer_request[SLAB_NUM_MEM_PARTITIONS];
	/**< Tables to store early initialization request depends on memory
	 * partition. */
	uint32_t num_ddr_pools;
	/*counter for requested ddr management pools*/
};

/**************************************************************************//**
@Function      slab_module_init

@Description   Initialize SLAB module

		In AIOP during slab_module_init() we will call MC API in order
		to get all BPIDs

@Return        0 on success, error code otherwise.
 *//***************************************************************************/
int slab_module_init(void);

/**************************************************************************//**
@Function      slab_module_free

@Description   Frees SLAB module

		In addition to memory de-allocation it will return BPIDs to MC

@Return        None
 *//***************************************************************************/
void slab_module_free(void);

/**************************************************************************//**
@Function      slab_find_and_reserve_bpid

@Description   Finds and reserve buffers from buffer pool.

		This function is part of SLAB module therefore it should be
		called only after it has been initialized by slab_module_init()

@Param[in]     num_buffs         Number of buffers in new pool.
@Param[in]     buff_size         Size of buffers in pool.
@Param[in]     alignment         Requested alignment for data field (in bytes).
@Param[in]     mem_partition_id  Memory partition ID for buffer type.
				 AIOP: HW pool supports only PEB and DDR.
@Param[out]    bpid_array_index  Index for bman pool array which reserved the
				 buffers.
@Param[out]    bpid              Id of pool that supply the requested buffers.

@Return        0       - on success,
	       -EINVAL - slab module handle is null
	       -ENOMEM - not enough memory for mem_partition_id
 *//***************************************************************************/
int slab_find_and_reserve_bpid(uint32_t num_buffs,
			uint16_t buff_size,
			uint16_t alignment,
			enum memory_partition_id  mem_partition_id,
			uint16_t *bpid_array_index,
			uint16_t *bpid);

/**************************************************************************//**
@Function      slab_find_and_unreserve_bpid

@Description   Finds and unreserved buffers in buffer pool.

		This function is part of SLAB module therefore it should be
		called only after it has been initialized by slab_module_init()
		the function is for service layer to return buffers to bman pool.

@Param[in]    num_buffs        Number of buffers in new pool.
@Param[in]    bpid              Id of pool that was filled with new buffers.

@Return        0       - on success,
	       -ENAVAIL - bman pool not found
 *//***************************************************************************/
int slab_find_and_unreserve_bpid(int32_t num_buffs,
                            uint16_t bpid);

#endif /* __SLAB_H */
