/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          slab.h

 @Description   External prototypes for the memory manager object
 *//***************************************************************************/

#ifndef __FSL_SLAB_H
#define __FSL_SLAB_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_lib_g   Utility Library Application Programming Interface

 @Description   External routines.

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Group         slab_g   Slab Memory Manager

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

#define MEM_MAX_NAME_LENGTH     8

struct slab;

/**************************************************************************//**
 @Function      slab_create

 @Description   Create a new memory segment.

 @Param[in]     name                Name of memory partition.
 @Param[in]     slab                Handle to new segment is returned through here.
 @Param[in]     num_buffs           Number of blocks in new segment.
 @Param[in]     buff_size           Size of blocks in segment.
 @Param[in]     prefix_size         How many bytes to allocate before the data.
 @Param[in]     postfix_size        How many bytes to allocate after the data.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
 @Param[in]     mem_partition_id    Memory partition ID for allocation.
 @Param[in]     consecutive         Whether to allocate the memory blocks
 continuously or not.

 @Return        E_OK - success, E_NO_MEMORY - out of memory.
 *//***************************************************************************/
int slab_create(char name[],
                struct slab **slab,
                uint32_t num_buffs,
                uint16_t buff_size,
                uint16_t prefix_size,
                uint16_t postfix_size,
                uint16_t alignment,
                uint8_t mem_partition_id,
//                      int         hw_accel,
//                      uint16_t    hw_pool_id,
                int consecutive);

/**************************************************************************//**
 @Function      slab_create_by_address

 @Description   Create a new memory segment with a specified base address.

 @Param[in]     name                Name of memory partition.
 @Param[in]     slab                Handle to new segment is returned through here.
 @Param[in]     num_buffs           Number of blocks in new segment.
 @Param[in]     buff_size           Size of blocks in segment.
 @Param[in]     prefix_size         How many bytes to allocate before the data.
 @Param[in]     postfix_size        How many bytes to allocate after the data.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
 @Param[in]     address             The required base address.

 @Return        E_OK - success, E_NO_MEMORY - out of memory.
 *//***************************************************************************/
int slab_create_by_address(char name[],
                           struct slab **slab,
                           uint32_t num_buffs,
                           uint16_t buff_size,
                           uint16_t prefix_size,
                           uint16_t postfix_size,
                           uint16_t alignment,
                           uint8_t *address);

/**************************************************************************//**
 @Function      slab_free

 @Description   Free a specific memory segment.

 @Param[in]     slab - Handle to memory segment.
 *//***************************************************************************/
void slab_free(struct slab *slab);

/**************************************************************************//**
 @Function      slab_aquire

 @Description   Get a block of memory from a segment.

 @Param[in]     slab - Handle to memory segment.
 @Param[in]     buff - The block to return.

 @Return        E_OK on success, error code otherwise.
 *//***************************************************************************/
int slab_acquire(struct slab *slab, uint64_t *buff);

/**************************************************************************//**
 @Function      slab_release

 @Description   Put a block of memory back to a segment.

 @Param[in]     slab - Handle to memory segment.
 @Param[in]     buff - The block to return.

 @Return        E_OK on success, error code otherwise.
 *//***************************************************************************/
int slab_release(struct slab *slab, uint64_t buff);

/**************************************************************************//**
 @Function      slab_get_size

 @Description   TODO

 @Param[in]     slab   - Handle to memory segment.

 @Return        TODO
 *//***************************************************************************/
uint32_t slab_get_buff_size(struct slab *slab);

/**************************************************************************//**
 @Function      slab_get_num_of_blocks

 @Description   TODO

 @Param[in]     slab   - Handle to memory segment.

 @Return        TODO
 *//***************************************************************************/
uint32_t slab_get_num_buffs(struct slab *slab);

#ifdef DEBUG_MEM_LEAKS
#if !((defined(__MWERKS__) || defined(__GNUC__))&& (__dest_os == __ppc_eabi))
#error  "memory-leaks-debug option is supported only for freescale code_warrior"
#endif /* !(defined(__MWERKS__) && ... */

/**************************************************************************//**
 @Function      slab_check_leaks

 @Description   Report MEM object leaks.

 This routine is automatically called by the MEM_Free() routine,
 but it can also be invoked while the MEM object is alive.

 @Param[in]     slab - Handle to memory segment.
 *//***************************************************************************/
void slab_check_leaks(struct slab *slab);

#else  /* not DEBUG_MEM_LEAKS */
#define slab_check_leaks(slab)
#endif /* not DEBUG_MEM_LEAKS */

/** @} *//* end of slab_g group */
/** @} *//* end of fsl_lib_g group */

#endif /* __FSL_SLAB_H */
