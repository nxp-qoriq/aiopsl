/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

/**************************************************************************//**
 @File          mem_mng_ext.h

 @Description   External definitions for memory allocation management module.
*//***************************************************************************/
#ifndef __MEM_MNG_UTIL_H
#define __MEM_MNG_UTIL_H

#include "common/types.h"
#include "fsl_errors.h"

/* Put all function (execution code) into  dtext_vle section,aka __COLD_CODE */
__START_COLD_CODE
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
 @Description   Initial Memory management, used for allocations during boot. 
 *//***************************************************************************/
struct initial_mem_mng
{
    uint64_t base_paddress;
    uint32_t base_vaddress;
    uint64_t size;
    uint64_t curr_ptr;
#ifdef AIOP
    uint8_t    lock;
#else /* not AIOP */
    fsl_handle_t    lock;
#endif

};

extern const  uint32_t g_boot_mem_mng_size;
#define MEM_PART_SYSTEM_DDR1_BOOT_MEM_MNG MEM_PART_LAST+1

/*****************************************************************************/
int boot_get_mem(struct initial_mem_mng* boot_mem_mng,
                 uint64_t size,uint64_t* paddr);
/*****************************************************************************/
int boot_get_mem_virt(struct initial_mem_mng* boot_mem_mng,
                      uint64_t size,uint32_t* vaddr);
/**************************************************************************//**
 @Function      boot_mem_mng_init

 @Description   Initialize the memory allocation management module.

 @Param[in]     boot_mem_mng - MEM_MNG initialization parameters.

 @Return        Handle to initialized MEM_MNG object, or NULL on error.
*//***************************************************************************/
int boot_mem_mng_init(struct initial_mem_mng* boot_mem_mng,int mem_partition_id);

/**************************************************************************//**
 @Function      boot_mem_mng_free

 @Description   Free the memory allocation management module.

 @Param[in]     boot_mem_mng - initial_mem_mng

 @Return        None.
*//***************************************************************************/
int boot_mem_mng_free(struct initial_mem_mng* boot_mem_mng);


/**************************************************************************//**
 @Function      mem_mng_init

 @Description   Initialize the memory allocation management module.

 @Param[in]     p_mem_mng_param - MEM_MNG initialization parameters.

 @Param[in]     h_boot_mem_mng - Handle to boot memory manage.

 @Return        Handle to initialized MEM_MNG object, or NULL on error.
*//***************************************************************************/
fsl_handle_t mem_mng_init(t_mem_mng_param *p_mem_mng_param,
                          fsl_handle_t h_boot_mem_mng);

/**************************************************************************//**
 @Function      mem_mng_free

 @Description   Free the memory allocation management module.

 @Param[in]     h_mem_mng - Handle to MEM_MNG object.

@Param[in]     h_boot_mem_mng - Handle to boot memory manage.

 @Return        None.
*//***************************************************************************/
void mem_mng_free(fsl_handle_t h_mem_mng, fsl_handle_t h_boot_mem_mng);


/**************************************************************************//**/
int mem_mng_get_phys_mem(fsl_handle_t    h_mem_mng,
                        int         partition_id,
                        uint64_t    size,
                        uint64_t    alignment,
                        uint64_t*  paddr);

void mem_mng_put_phys_mem(fsl_handle_t h_mem_mng, uint64_t p_memory);

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


/**************************************************************************//**
 @Description   Memory partition information for physical address allocation
 *//***************************************************************************/
typedef struct t_mem_mng_phys_addr_alloc_info
{
    char        name[MEM_MNG_MAX_PARTITION_NAME_LEN];
    uint64_t    base_paddress;
    uint64_t    size;
    uint32_t    attributes;
} t_mem_mng_phys_addr_alloc_info;


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


int mem_mng_register_phys_addr_alloc_partition(fsl_handle_t  h_mem_mng,
                                  int       partition_id,
                                  uint64_t base_paddress,
                                  uint64_t  size,
                                  uint32_t  attributes,
                                  char      name[]);


/**************************************************************************//**
 @Function      mem_mng_get_partition_info

 @Description   Get information and usage statistics of a selected partition.
*//***************************************************************************/
int mem_mng_get_partition_info(fsl_handle_t               h_mem_mng,
                                 int                    partition_id,
                                 t_mem_mng_partition_info  *p_partition_info);

/**************************************************************************//**
 @Function      mem_mng_get_phys_addr_alloc_info

 @Description   Get information and usage statistics of a selected partition.
*//***************************************************************************/
int mem_mng_get_phys_addr_alloc_info(fsl_handle_t               h_mem_mng,
                                 int                    partition_id,
                                 t_mem_mng_phys_addr_alloc_info  *p_partition_info);

typedef void (t_mem_mng_leak_report_func)(void      *p_memory,
                                      uint32_t  size,
                                      char      *info,
                                      char      *filename,
                                      int       line);

uint32_t mem_mng_check_leaks(fsl_handle_t                h_mem_mng,
                            int                     partition_id,
                            t_mem_mng_leak_report_func  *f_report_leak);


/** @} */ /* end of mem_mng_grp */

__END_COLD_CODE

#endif /* __MEM_MNG_UTIL_H */


