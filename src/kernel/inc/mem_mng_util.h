/**************************************************************************//**
 @File          mem_mng_ext.h

 @Description   External definitions for memory allocation management module.
*//***************************************************************************/
#ifndef __MEM_MNG_UTIL_H
#define __MEM_MNG_UTIL_H

#include "common/types.h"
#include "fsl_errors.h"


/**************************************************************************//**
 @Group         mem_mng_grp     Memory Allocation Management module

 @Description   API for memory allocation management.

 @{
*//***************************************************************************/

#define MEM_MNG_MAX_PARTITION_NAME_LEN      32
        /**< Maximum length of memory partition name */


#define MEM_MNG_EARLY_PARTITION_ID          ((int)(-1))
        /**< Dummy partition ID for early memory allocations (before partitions
             are registered); allows memory leaks checking even for early
             memory allocations. */


/**************************************************************************//**
 @Description   Memory management module initialization parameters
 *//***************************************************************************/
typedef struct t_mem_mng_param
{
    void *      (*f_malloc)(uint32_t size);
                /**< Memory allocation routine for internal structures */
    void        (*f_free)(void *p_mem);
                /**< Memory deallocation routine for internal structures */

    void *      (*f_early_malloc)(uint32_t size, uint32_t alignment);
                /**< Early allocation routine (before partitions are registered) */
    void        (*f_early_free)(void *p_addr);
                /**< Early deallocation routine (before partitions are registered) */

#ifdef AIOP
    uint8_t *   lock;
#else /* not AIOP */
    fsl_handle_t    lock;
#endif /* AIOP */
                /**< We might not have malloc at the beginning, so we
                     need the spinlock object to be allocated outside of the manager */
} t_mem_mng_param;


/**************************************************************************//**
 @Function      MEM_MNG_Init

 @Description   Initialize the memory allocation management module.

 @Param[in]     p_MemMngParam - MEM_MNG initialization parameters.

 @Return        Handle to initialized MEM_MNG object, or NULL on error.
*//***************************************************************************/
fsl_handle_t mem_mng_init(t_mem_mng_param *p_mem_mng_param);

/**************************************************************************//**
 @Function      MEM_MNG_Free

 @Description   Free the memory allocation management module.

 @Param[in]     h_MemMng - Handle to MEM_MNG object.

 @Return        None.
*//***************************************************************************/
void mem_mng_free(fsl_handle_t h_mem_mng);


void * mem_mng_alloc_mem(fsl_handle_t    h_mem_mng,
                        int         partition_id,
                        uint32_t    size,
                        uint32_t    alignment,
                        char        *info,
                        char        *filename,
                        int         line);

void mem_mng_free_mem(fsl_handle_t h_mem_mng, void *p_memory);


/**************************************************************************//**
 @Description   Memory partition information
 *//***************************************************************************/
typedef struct t_mem_mng_partition_info
{
    char        name[MEM_MNG_MAX_PARTITION_NAME_LEN];
    uint64_t    base_address;
    uint64_t    size;
    uint32_t    attributes;
    uint32_t    current_usage;
    uint32_t    maximum_usage;
    uint32_t    total_allocations;
    uint32_t    total_deallocations;

} t_mem_mng_partition_info;


int mem_mng_get_partition_id_by_addr(fsl_handle_t   h_mem_mng,
                                     uint64_t   addr,
                                     int        *p_partition_id);


int mem_mng_get_available_partition_id(fsl_handle_t h_mem_mng);

int mem_mng_register_partition(fsl_handle_t  h_mem_mng,
                                  int       partition_id,
                                  uintptr_t base_address,
                                  uint64_t  size,
                                  uint32_t  attributes,
                                  char      name[],
                                  void *    (*f_user_malloc)(uint32_t size, uint32_t alignment),
                                  void      (*f_user_free)(void *p_addr),
                                  int       enable_debug);

int mem_mng_unregister_partition(fsl_handle_t h_mem_mng, int partition_id);


/**************************************************************************//**
 @Function      MEM_MNG_GetPartitionInfo

 @Description   Get information and usage statistics of a selected partition.
*//***************************************************************************/
int mem_mng_get_partition_info(fsl_handle_t               h_mem_mng,
                                 int                    partition_id,
                                 t_mem_mng_partition_info  *p_partition_info);


typedef void (t_mem_mng_leak_report_func)(void      *p_memory,
                                      uint32_t  size,
                                      char      *info,
                                      char      *filename,
                                      int       line);

uint32_t mem_mng_check_leaks(fsl_handle_t                h_mem_mng,
                            int                     partition_id,
                            t_mem_mng_leak_report_func  *f_report_leak);


/** @} */ /* end of mem_mng_grp */


#endif /* __MEM_MNG_UTIL_H */

