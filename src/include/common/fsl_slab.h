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

/* Each block is of the following structure:
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
 @Description   Type of the function callback to be called on release of buffer into pool 
*//***************************************************************************/
typedef int32_t (*slab_release_cb_t)(uint64_t); 

/**************************************************************************//**
 @Function      slab_create

 @Description   Create a new buffers pool.

 @Param[in]     num_buffs           Number of blocks in new pool.
 @Param[in]     buff_size           Size of blocks in pool.
 @Param[in]     prefix_size         How many bytes to allocate before the data.
                                    AIOP: Not supported by AIOP HW pools.
 @Param[in]     postfix_size        How many bytes to allocate after the data.
                                    AIOP: Not supported by AIOP HW pools.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
 @Param[in]     mem_partition_id    Memory partition ID for allocation.
                                    AIOP: HW pool supports only PEB and DPAA DDR.
 @Param[in]     flags               Set it 0 for default slab creation. 
 @Param[in]     release_cb          Function to be called on release of buffer into pool
 @Param[out]    slab                Handle to new pool is returned through here.

 @Return        0 - on success, -ENOMEM - out of memory.
 *//***************************************************************************/
int slab_create(uint16_t    num_buffs,
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

 @Param[in]     num_buffs           Number of blocks in new pool.
 @Param[in]     buff_size           Size of blocks in pool.
 @Param[in]     prefix_size         How many bytes to allocate before the data.
                                    AIOP: Not supported by AIOP HW pools.
 @Param[in]     postfix_size        How many bytes to allocate after the data.
                                    AIOP: Not supported by AIOP HW pools.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
 @Param[in]     flags               Set it 0 for default slab creation. 
 @Param[in]     release_cb          Function to be called on release of buffer into pool
 @Param[out]    slab                Handle to new pool is returned through here.

 @Return        0 - on success, -ENOMEM - out of memory.
 *//***************************************************************************/
int slab_create_by_address(uint16_t num_buffs,
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

 @Description   Free a specific pool.

 @Param[in]     slab - Handle to memory pool.
 *//***************************************************************************/
void slab_free(uint32_t slab);

/**************************************************************************//**
 @Function      slab_acquire

 @Description   Get a buffer of memory from a pool;
                Buffer reference counter if such exists will be set to 1.

 @Param[in]     slab - Handle to memory pool.
 @Param[out]    buff - The buffer to return.

 @Return        E_OK on success, error code otherwise.
 *//***************************************************************************/
int slab_acquire(uint32_t slab, uint64_t *buff);

/**************************************************************************//**
@Function      slab_release

@Description   Decrement buffer reference counter if such exists 
               and return the buffer back to a pool.

@Param[in]     slab - Handle to memory pool.
@Param[in]     buff - The buffer to return.

@Return        E_OK on success, error code otherwise.
*//***************************************************************************/
int slab_release(uint32_t slab, uint64_t buff);

/** @} *//* end of slab_g group */
/** @} *//* end of fsl_mm_g group */
/** @} *//* end of fsl_lib_g group */

#endif /* __FSL_SLAB_H */
