/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
 @File          mem_mng.h

 @Description   Internal definitions for memory allocation management module.
*//***************************************************************************/
#ifndef __MEM_MNG_H
#define __MEM_MNG_H

#include "fsl_mem_mng.h"
#include "fsl_list.h"
#include "buffer_pool.h"
#include "fsl_platform.h"

typedef struct t_mem_mng_debug_entry
{
    void        *p_memory;
    uint32_t    size;
    char        *info;
    char        *filename;
    int         line;
    list_t      node;

} t_mem_mng_debug_entry;

#define MEM_MNG_DBG_OBJECT(p_list)  \
    LIST_OBJECT(p_list, t_mem_mng_debug_entry, node)


#define MEM_MNG_PARTITION_OBJECT(p_list)  \
    LIST_OBJECT(p_list, t_mem_mng_partition, node)

#define MEM_MNG_PHYS_ADDR_ALLOC_PARTITION_OBJECT(p_list)  \
    LIST_OBJECT(p_list, t_mem_mng_phys_addr_alloc_partition, node)

typedef enum buffer_pool_type{
	E_BFT_SLOB_BLOCK = 0,
	E_BFT_DEBUG_BLOCK
} e_buffer_pool_type;

/**************************************************************************//**
 @Function      mem_mng_init

 @Description   Initialize the memory allocation management module.

 @Param[in]     p_mem_mng_param - MEM_MNG initialization parameters.

 @Param[in]     h_boot_mem_mng - Handle to boot memory manage.

 @Return        Handle to initialized MEM_MNG object, or NULL on error.
*//***************************************************************************/
int mem_mng_init(void * h_boot_mem_mng,
                 struct t_mem_mng    *p_mem_mng);

/**************************************************************************//**
 @Function      mem_mng_get_partition_info

 @Description   Get information and usage statistics of a selected partition.
*//***************************************************************************/
int mem_mng_get_partition_info(void *               h_mem_mng,
                                 int                    partition_id,
                                 t_mem_mng_partition_info  *p_partition_info);

/**************************************************************************//**
 @Function      mem_mng_get_phys_addr_alloc_info

 @Description   Get information and usage statistics of a selected partition.
*//***************************************************************************/
int mem_mng_get_phys_addr_alloc_info(void *               h_mem_mng,
                                 int                    partition_id,
                                 t_mem_mng_phys_addr_alloc_info  *p_partition_info);

/**************************************************************************//**
 @Function      mem_mng_free

 @Description   Free the memory allocation management module.

 @Param[in]     h_mem_mng - Handle to MEM_MNG object.

@Param[in]     h_boot_mem_mng - Handle to boot memory manage.

 @Return        None.
*//***************************************************************************/
void mem_mng_free(void * h_mem_mng, void * h_boot_mem_mng);


/**************************************************************************//**/
int mem_mng_get_phys_mem(void *    h_mem_mng,
                        int         partition_id,
                        uint64_t    size,
                        uint64_t    alignment,
                        uint64_t*  paddr);
/**************************************************************************//**/
void mem_mng_put_phys_mem(void * h_mem_mng, uint64_t p_memory);

/**************************************************************************//**/
void * mem_mng_alloc_mem(void *    h_mem_mng,
                        int         partition_id,
                        uint32_t    size,
                        uint32_t    alignment,
                        char        *info,
                        char        *filename,
                        int         line);
/**************************************************************************//**/
void mem_mng_free_mem(void * h_mem_mng, void *p_memory);

/**************************************************************************//**/
int mem_mng_register_partition(void *  h_mem_mng,
                                  int       partition_id,
                                  uintptr_t base_address,
                                  uint64_t  size,
                                  uint32_t  attributes,
                                  char      name[],
                                  int       enable_debug);

int mem_mng_unregister_partition(void * h_mem_mng, int partition_id);
/**************************************************************************//**/
int mem_mng_register_phys_addr_alloc_partition(void *  h_mem_mng,
                                  int       partition_id,
                                  uint64_t base_paddress,
                                  uint64_t  size,
                                  uint32_t  attributes,
                                  char      name[]);
/**************************************************************************//**/
typedef void (t_mem_mng_leak_report_func)(void      *p_memory,
                                      uint32_t  size,
                                      char      *info,
                                      char      *filename,
                                      int       line);

uint32_t mem_mng_check_leaks(void *                h_mem_mng,
                            int                     partition_id,
                            t_mem_mng_leak_report_func  *f_report_leak);
/**************************************************************************//**/
int mem_mng_mem_partitions_init_completed(void * h_mem_mng);
/**************************************************************************//**
 *
 @Function      boot_mem_mng_init

 @Description   Initialize the memory allocation management module.

 @Param[in]     boot_mem_mng - MEM_MNG initialization parameters.

 @Return        Handle to initialized MEM_MNG object, or NULL on error.
*//***************************************************************************/
int boot_mem_mng_init(struct initial_mem_mng* boot_mem_mng,const int mem_partition_id);

/**************************************************************************//**
 @Function      boot_mem_mng_free

 @Description   Free the memory allocation management module.

 @Param[in]     boot_mem_mng - initial_mem_mng

 @Return        None.
*//***************************************************************************/
int boot_mem_mng_free(struct initial_mem_mng* boot_mem_mng);


#endif /* __MEM_MNG_H */

