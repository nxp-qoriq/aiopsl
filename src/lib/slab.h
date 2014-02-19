/**************************************************************************//*
 @File          slab.h

 @Description   This is slab internal header file which includes all the 
                architecture specific implementation defines.

 @Cautions      This file is private for AIOP.
*//***************************************************************************/
#ifndef __SLAB_H
#define __SLAB_H

#include "common/fsl_slab.h"
#include "common/list.h"


/**************************************************************************//**
 @Description   SLAB common internal macros 
*//***************************************************************************/
#define SLAB_HW_POOL_SET      0x00000001
/**< Flag which indicates that this SLAB handle is HW pool */
#define SLAB_IS_HW_POOL(SLAB) ((SLAB) & SLAB_HW_POOL_SET)
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
#define SLAB_VP_POOL_MAX       (SLAB_VP_POOL_MASK >> 1) /**< Maximal number to be used as VP id */
#define SLAB_VP_POOL_SHIFT     1
#define SLAB_HW_ACCEL_MASK     0xFF000000
#define SLAB_VP_POOL_GET(SLAB) ((uint32_t)(((SLAB) & SLAB_VP_POOL_MASK) >> 1)) 
/**< Returns VP id to be used with virtual pools API */ 

#define SLAB_HW_METADATA_OFFSET     8 /**< bytes */
#define SLAB_HW_BUFF_SIZE(SIZE)     ((SIZE) - SLAB_HW_METADATA_OFFSET) 
/**< Real buffer size used by user */

/**************************************************************************//**
 @Description   SLAB module defaults macros 
*//***************************************************************************/
#define SLAB_BPIDS_PARTITION0       {1, 2, 3, 4, 5}
#define SLAB_MODULE_FAST_MEMORY     MEM_PART_SH_RAM
#define SLAB_MODULE_DDR_MEMORY      MEM_PART_1ST_DDR_NON_CACHEABLE
#define SLAB_DEFAULT_BUFF_SIZE      256
#define SLAB_DEFAULT_BUFF_ALIGN     8
#define SLAB_MAX_NUM_VP             1000

/**************************************************************************//**
 @Description   Information to be kept about every HW pool inside DDR
*//***************************************************************************/
struct slab_hw_pool_info {
    uint32_t flags;     /**< Control flags */
    uint16_t buff_size; /**< Maximal buffer size including 8 bytes of CDMA metadata */
    uint16_t pool_id;   /**< BMAN pool ID */
    uint16_t alignment; /**< Buffer alignment */
    uint16_t mem_partition_id; /**< Memory partition for buffers allocation */
};

/**************************************************************************//**
 @Description   Information to be kept about SLAB module
*//***************************************************************************/
struct slab_module_info {    
    struct  slab_hw_pool_info *hw_pools;    /**< List of BMAN pools */
    void    *virtual_pool_struct;           /**< VP internal structure size of (struct virtual_pool_desc) * MAX_VIRTUAL_POOLS_NUM */ 
    void    *callback_func_struct;          /**< VP internal structure size of (struct callback_s) * MAX_VIRTUAL_POOLS_NUM */
    uint8_t spinlock;                       /**< Spinlock placed at SHRAM */
    uint8_t num_hw_pools;                   /**< Number of BMAN pools */
};

/**************************************************************************//**
 @Function      slab_module_init

 @Description   Initialize SLAB module
                
                In AIOP during slab_module_init() we’ll call MC API in order to get all BPIDs
                
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

#endif /* __SLAB_H */
