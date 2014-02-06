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

#define __ERR_MODULE__  MODULE_SLAB

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

/**************************************************************************//**
 @Description   Information to be kept about every HW pool inside DDR
*//***************************************************************************/
struct slab_hw_pool_info {
    uint32_t flags;
    uint32_t buff_size; /**< Maximal buffer size */
    uint16_t pool_id;
    uint16_t mem_partition_id;
    uint16_t alignment;
};


struct slab_module_info {    
    struct  slab_hw_pool_info *hw_pools;
    void    *virtual_pool_struct;           /**< VP internal structure size of (struct virtual_pool_desc) * MAX_VIRTUAL_POOLS_NUM */ 
    void    *callback_func_struct;          /**< VP internal structure size of (struct callback_s) * MAX_VIRTUAL_POOLS_NUM */
    uint8_t spinlock;
    uint8_t num_hw_pools;
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
