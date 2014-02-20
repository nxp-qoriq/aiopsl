/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_slab.h

 @Description   External prototypes for the buffer pool manager
 *//***************************************************************************/

#ifndef __FSL_SLAB_H
#define __FSL_SLAB_H

#include "common/types.h"


/**************************************************************************//**
 @ingroup         fsl_lib_g   Utility Library 
 @{
 *//***************************************************************************/
/**************************************************************************//**
 @ingroup         fsl_mm_g  
 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Group         slab_g   SLAB

 @Description   Slab Memory Manager module functions, definitions and enums.

 @{
 *//***************************************************************************/

/* Each buffer is of the following structure:
 *
 *
 *  +-----------+----------+---------------------------+-----------+-----------+
 *  | Alignment |  Prefix  | Data                      | Postfix   | Alignment |
 *  |  field    |   field  |  field                    |   field   | Padding   |
 *  +-----------+----------+---------------------------+-----------+-----------+
 *  and at the beginning of all bytes, an additional optional padding might reside
 *  to ensure that the first blocks data field is aligned as requested.
 */

/**************************************************************************//**
 @Description   Available debug information about every slab 
*//***************************************************************************/
struct slab_debug_info {
    uint32_t buff_size; /**< Maximal buffer size */
    uint16_t pool_id;   /**< HW pool ID */
    uint16_t alignment; /**< Maximal alignment */
    uint16_t num_buffs; /**< The number of available buffers */
    uint16_t max_buffs; /**< The maximal number of buffers inside this pool */
    uint16_t mem_partition_id; /**< Memory partition */
};

/**************************************************************************//**
 @Description   Type of the function callback to be called on release of buffer into pool 
*//***************************************************************************/
typedef int (*slab_release_cb_t)(uint64_t); 

/**************************************************************************//**
 @Function      slab_create

 @Description   Create a new buffers pool.

 @Param[in]     num_buffs           Number of buffers in new pool.
 @Param[in]     extra_buffs         Set it to 0; 
                                    Number of extra buffers that can be allocated by this new pool.
 @Param[in]     buff_size           Size of buffers in pool.
 @Param[in]     prefix_size         How many bytes to allocate before the data.
                                    AIOP: Not supported by AIOP HW pools.
 @Param[in]     postfix_size        How many bytes to allocate after the data.
                                    AIOP: Not supported by AIOP HW pools.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
                                    AIOP: HW pool supports up to 8 bytes alignment.
 @Param[in]     mem_partition_id    Memory partition ID for allocation.
                                    AIOP: HW pool supports only PEB and DPAA DDR.
 @Param[in]     flags               Set it to 0 for default slab creation.                           
 @Param[in]     release_cb          Function to be called on release of buffer into pool
 @Param[out]    slab                Handle to new pool is returned through here.

 @Return        0       - on success, 
               -ENAVAIL - resource not available or not found,
               -ENOMEM  - not enough memory for mem_partition_id
 *//***************************************************************************/
int slab_create(uint16_t    num_buffs,
                uint16_t    extra_buffs,
                uint16_t    buff_size,
                uint16_t    prefix_size,
                uint16_t    postfix_size,
                uint16_t    alignment,
                uint8_t     mem_partition_id,
                uint32_t    flags,
                slab_release_cb_t release_cb,
                uint32_t    *slab);

/**************************************************************************//**
 @Function      slab_create_by_address

 @Description   Create a new buffers pool starting from address base.
                AIOP: Not supported by AIOP HW pools.
                
 @Param[in]     num_buffs           Number of buffers in new pool.
 @Param[in]     extra_buffs         Number of extra buffers that can be allocated by this new pool.
 @Param[in]     buff_size           Size of buffers in pool.
 @Param[in]     prefix_size         How many bytes to allocate before the data.
 @Param[in]     postfix_size        How many bytes to allocate after the data.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
 @Param[in]     flags               Set it 0 for default slab creation. 
 @Param[in]     release_cb          Function to be called on release of buffer into pool
 @Param[out]    slab                Handle to new pool is returned through here.

 @Return        0       - on success, 
               -ENAVAIL - resource not available or not found.
 *//***************************************************************************/
int slab_create_by_address(uint16_t num_buffs,
                           uint16_t extra_buffs,                           
                           uint16_t buff_size,
                           uint16_t prefix_size,
                           uint16_t postfix_size,
                           uint16_t alignment,
                           uint8_t  *address,
                           uint32_t flags,
                           slab_release_cb_t release_cb,
                           uint32_t *slab);

/**************************************************************************//**
 @Function      slab_free

 @Description   Free a specific pool and all it's buffers.

 @Param[in]     slab - Handle to memory pool.
 
 @Return        0      - on success, 
               -EBUSY  - this slab can't be freed 
               -EINVAL - not a valid slab handle                
 *//***************************************************************************/
int slab_free(uint32_t slab);

/**************************************************************************//**
 @Function      slab_acquire

 @Description   Get a buffer of memory from a pool;
                AIOP HW pool buffer reference counter will be set to 1.

 @Param[in]     slab - Handle to memory pool.
 @Param[out]    buff - The buffer to return.

 @Return        0      - on success, 
               -ENOMEM - no buffer available, 
               -EINVAL - not a valid slab handle
 *//***************************************************************************/
int slab_acquire(uint32_t slab, uint64_t *buff);

/**************************************************************************//**
@Function      slab_release

@Description   Return the buffer back to a pool;
               AIOP HW pool buffer reference counter will be decremented. 

@Param[in]     slab - Handle to memory pool.
@Param[in]     buff - The buffer to return.

@Return        0      - on success, 
              -EFAULT - failed to release buffer, 
              -EINVAL - not a valid slab handle 
*//***************************************************************************/
int slab_release(uint32_t slab, uint64_t buff);

/**************************************************************************//**
@Function      slab_debug_info_get

@Description   Decrement buffer reference counter if such exists 
               and return the buffer back to a pool.

@Param[in]     slab - Handle to memory pool.
@Param[out]    slab_info - The pointer to place the debug information.

@Return        0      - on success, 
              -EINVAL - invalid parameter.
*//***************************************************************************/
int slab_debug_info_get(uint32_t slab, struct slab_debug_info *slab_info);

/** @} *//* end of slab_g group */
/** @} *//* end of fsl_mm_g group */
/** @} *//* end of fsl_lib_g group */

#endif /* __FSL_SLAB_H */
