/**************************************************************************//*
@File          slab.h

@Description   This is slab internal header file which includes all the
		architecture specific implementation defines.

@Cautions      This file is private for AIOP.
*//***************************************************************************/
#ifndef __SLAB_H
#define __SLAB_H

#include "common/fsl_slab.h"
#include "kernel/platform.h"

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
 * 31----------23--------------1--------0
 * | HW accel   |VP ID         |HW flg  |
 * -------------------------------------
 */
#define SLAB_VP_POOL_MASK      0x00FFFFFE
#define SLAB_VP_POOL_MAX       (SLAB_VP_POOL_MASK >> 1)
/**< Maximal number to be used as VP id */
#define SLAB_VP_POOL_SHIFT     1
#define SLAB_HW_ACCEL_MASK     0xFF000000
#define SLAB_VP_POOL_GET(SLAB) \
	((uint32_t)((SLAB_HW_HANDLE(SLAB) & SLAB_VP_POOL_MASK) >> 1))
/**< Returns VP id to be used with virtual pools API */

#define SLAB_HW_META_OFFSET     8 /**< metadata offset in bytes */
#define SLAB_SIZE_GET(SIZE)     ((SIZE) - SLAB_HW_META_OFFSET)
/**< Real buffer size used by user */
#define SLAB_SIZE_SET(SIZE)     ((SIZE) + SLAB_HW_META_OFFSET)
/**< Buffer size that needs to be set for CDMA, including metadata */

#define SLAB_HW_POOL_CREATE(VP) \
((((VP) & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) \
	| SLAB_HW_POOL_SET)

/**************************************************************************//**
@Description   SLAB module defaults macros
*//***************************************************************************/
/** bpid, user required size, partition */

#define SLAB_BPIDS_ARR	\
	{ \
	{3,	256,    MEM_PART_DP_DDR}, \
	{5,	512,    MEM_PART_DP_DDR}, \
	{2,	1024,   MEM_PART_DP_DDR}, \
	{4,	2048,   MEM_PART_DP_DDR}, \
	{1,	3072,   MEM_PART_DP_DDR}, \
	{6,	4096,   MEM_PART_DP_DDR}, \
	{7,	256,    MEM_PART_PEB},                   \
	{8,	512,    MEM_PART_PEB},                   \
	{9,	1024,   MEM_PART_PEB},                   \
	{10,	2048,   MEM_PART_PEB},                   \
	{11,	4096,   MEM_PART_PEB}                    \
	}


#define SLAB_FAST_MEMORY        MEM_PART_SH_RAM
#define SLAB_DDR_MEMORY         MEM_PART_DP_DDR
#define SLAB_DEFAULT_ALIGN      8
#define SLAB_MAX_NUM_VP         1000

/**************************************************************************//**
@Description   Information for every bpid
*//***************************************************************************/
struct slab_bpid_info {
	uint16_t bpid;
	uint16_t size;
	e_memory_partition_id mem_pid;
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
};

/**************************************************************************//**
@Description   Information to be kept about SLAB module
*//***************************************************************************/
struct slab_module_info {
	struct  slab_hw_pool_info *hw_pools;
	/**< List of BMAN pools */
	void    *virtual_pool_struct;
	/**< VP internal structure size of
	 * (struct virtual_pool_desc) * MAX_VIRTUAL_POOLS_NUM */
	void    *callback_func_struct;
	/**< VP internal structure size of
	 * (struct callback_s) * MAX_VIRTUAL_POOLS_NUM */
	/* TODO uint8_t spinlock; */
	/**< Spinlock placed at SHRAM */
	uint8_t num_hw_pools;
	/**< Number of BMAN pools */
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
@Function      slab_find_and_fill_bpid

@Description   Finds and fills buffer pool with new buffers

		This function is part of SLAB module therefore it should be
		called only after it has been initialized by slab_module_init()

@Param[in]     num_buffs         Number of buffers in new pool.
@Param[in]     buff_size         Size of buffers in pool.
@Param[in]     alignment         Requested alignment for data field (in bytes).
				 AIOP: HW pool supports up to 8 bytes alignment.
@Param[in]     mem_partition_id  Memory partition ID for allocation.
				 AIOP: HW pool supports only PEB and DPAA DDR.
@Param[out]    num_filled_buffs  Number of buffers that we succeeded to fill.
@Param[out]    bpid              Id of pool that was filled with new buffers.

@Return        0       - on success,
	       -ENAVAIL - could not release into bpid
	       -ENOMEM  - not enough memory for mem_partition_id
 *//***************************************************************************/
int slab_find_and_fill_bpid(uint32_t num_buffs,
			uint16_t buff_size,
			uint16_t alignment,
			uint8_t  mem_partition_id,
			int      *num_filled_buffs,
			uint16_t *bpid);

#endif /* __SLAB_H */
