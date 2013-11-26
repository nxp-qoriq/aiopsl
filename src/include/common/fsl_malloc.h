/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_malloc.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_MALLOC_H
#define __FSL_MALLOC_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_os_g  FSL OS Interface (System call hooks)

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/

#ifndef DEBUG_FSL_OS_MALLOC
/**************************************************************************//**
 @Function      fsl_os_malloc

 @Description   allocates contiguous block of memory.

 @Param[in]     size    Number of bytes to allocate.

 @Return        The address of the newly allocated block on success, NULL on failure.
*//***************************************************************************/
void * fsl_os_malloc(size_t size);

/**************************************************************************//**
 @Function      fsl_os_malloc_smart

 @Description   Allocates contiguous block of memory in a specified
                alignment and from the specified segment.

 @Param[in]     size                Number of bytes to allocate.
 @Param[in]     mem_partition_id    Memory partition ID; The value zero must
                                    be mapped to the default heap partition.
 @Param[in]     alignment           Required memory alignment (in bytes).

 @Return        The address of the newly allocated block on success, NULL on failure.
*//***************************************************************************/
void * fsl_os_malloc_smart(size_t size, int mem_partition_id, uint32_t alignment);

#else /* DEBUG_FSL_OS_MALLOC */
void * fsl_os_malloc_debug(size_t size, char *fname, int line);

void * fsl_os_malloc_smart_debug(size_t size,
                           int      mem_partition_id,
                           uint32_t alignment,
                           char     *fname,
                           int      line);

#define fsl_os_malloc(sz) \
    fsl_os_malloc_debug((sz), __FILE__, __LINE__)

#define fsl_os_malloc_smart(sz, memt, al) \
    fsl_os_malloc_smart_debug((sz), (memt), (al), __FILE__, __LINE__)
#endif /* !DEBUG_FSL_OS_MALLOC */

/**************************************************************************//**
 @Function      fsl_os_free_smart

 @Description   Frees the memory block pointed to by "p".
                Only for memory allocated by fsl_os_malloc_smart().

 @Param[in]     mem     A pointer to the memory block.
*//***************************************************************************/
void fsl_os_free_smart(void *mem);

/**************************************************************************//**
 @Function      fsl_os_free

 @Description   frees the memory block pointed to by "p".

 @Param[in]     mem     A pointer to the memory block.
*//***************************************************************************/
void fsl_os_free(void *mem);


/**************************************************************************//**
 @Function      fsl_os_buff_acquire

 @Description   Get a block of memory from a segment.

 @Param[in]     pool_id     An ID of the pool (or memory segment).
 @Param[out]    buff        The buffer to be returned.

 @Return        E_OK on success, error code otherwise.
 *//***************************************************************************/
int fsl_os_buff_acquire(int pool_id, uint64_t *buff);

/**************************************************************************//**
 @Function      fsl_os_buff_release

 @Description   Put a block of memory back to a segment.

 @Param[in]     pool_id     An ID of the pool (or memory segment).
 @Param[in]     buff        A pointer to a buffer to release.

 @Return        E_OK on success, error code otherwise.
 *//***************************************************************************/
int fsl_os_buff_release(int pool_id, uint64_t buff);

/**************************************************************************//**
 @Function      fsl_os_buff_dec_refcnt_n_release

 @Description   TODO

 @Param[in]     pool_id     An ID of the pool (or memory segment).
 @Param[in]     buff        A pointer to a buffer to release.

 @Return        E_OK on success, error code otherwise.
 *//***************************************************************************/
int fsl_os_buff_dec_refcnt_n_release(int pool_id, uint64_t buff);

/**************************************************************************//**
 @Function      fsl_os_buff_inc_refcnt

 @Description   TODO

 @Param[in]     pool_id     An ID of the pool (or memory segment).
 @Param[in]     buff        A pointer to a buffer to increment its reference-counter.

 @Return        E_OK on success, error code otherwise.
 *//***************************************************************************/
int fsl_os_buff_inc_refcnt(int pool_id, uint64_t buff);

/**************************************************************************//**
 @Function      fsl_os_buff_get_size

 @Description   TODO

 @Param[in]     pool_id     An ID of the pool (or memory segment).

 @Return        TODO
 *//***************************************************************************/
uint32_t fsl_os_buff_get_size(int pool_id);

/**************************************************************************//**
 @Function      fsl_os_buff_get_num_of_blocks

 @Description   TODO

 @Param[in]     pool_id     An ID of the pool (or memory segment).

 @Return        TODO
 *//***************************************************************************/
uint32_t fsl_os_buff_get_num_buffs(int pool_id);

/** @} */ /* end of fsl_os_g group */


#endif /* __FSL_MALLOC_H */
