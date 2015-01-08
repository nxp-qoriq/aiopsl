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
 @File          mem_mng.h

 @Description   Internal definitions for memory allocation management module.
*//***************************************************************************/
#ifndef __MEM_MNG_H
#define __MEM_MNG_H

#include "inc/mem_mng_util.h"
#include "fsl_list.h"


#define __ERR_MODULE__  MODULE_UNKNOWN


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


/**************************************************************************//**
 @Description   Memory management partition control structure
 *//***************************************************************************/
typedef struct t_mem_mng_partition
{
    int                     id;             /**< Partition ID */
    fsl_handle_t                h_mem_manager;   /**< Memory manager handle */
    void *                  (*f_user_malloc)(uint32_t size, uint32_t alignment);
                                            /**< Memory allocation routine */
    void                    (*f_user_free)(void *p_addr);
                                            /**< Memory deallocation routine */
    int                     enable_debug;    /**< '1' to track malloc/free operations */
    list_t                  mem_debug_list;   /**< List of allocation entries (for debug) */
    list_t                  node;
    t_mem_mng_partition_info   info;           /**< Partition information */
#ifdef AIOP
    uint8_t *               lock;
#else
    fsl_handle_t                lock;
#endif
} t_mem_mng_partition;


/**************************************************************************//**
 @Description   Memory management partition control structure
 *//***************************************************************************/
typedef struct t_mem_mng_phys_addr_alloc_partition
{
    int                              id;             /**< Partition ID */
    fsl_handle_t                     h_mem_manager;   /**< Memory manager handle */
    list_t                           node;
    t_mem_mng_phys_addr_alloc_info   info;           /**< Partition information */
#ifdef AIOP
    uint8_t *                        lock;
#else
    fsl_handle_t                     lock;
#endif
    uint64_t 	                     curr_paddress;
} t_mem_mng_phys_addr_alloc_partition;


#define MEM_MNG_PARTITION_OBJECT(p_list)  \
    LIST_OBJECT(p_list, t_mem_mng_partition, node)

#define MEM_MNG_PHYS_ADDR_ALLOC_PARTITION_OBJECT(p_list)  \
    LIST_OBJECT(p_list, t_mem_mng_phys_addr_alloc_partition, node)
/**************************************************************************//**
 @Description   Memory management module internal parameters
 *//***************************************************************************/
typedef struct t_mem_mng
{
    void *      (*f_malloc)(uint32_t size);
                /**< Memory allocation routine (for internal structures) */
    void        (*f_free)(void *p_addr);
                /**< Memory deallocation routine (for internal structures) */
    list_t      mem_partitions_list;
                /**< List of partition control structures */
    list_t      phys_allocation_mem_partitions_list;
                /**< List of partition for fsl_os_get_mem function() control structures */
    /* Variables related to early memory partition */
    void *      (*f_early_malloc)(uint32_t size, uint32_t alignment);
                /**< Early allocation routine (before partitions are registered) */
    void        (*f_early_free)(void *p_addr);
                /**< Early deallocation routine (before partitions are registered) */
    list_t      early_mem_debug_list;
                /**< List of early memory allocation entries (for debug) */

#ifdef AIOP
    uint8_t *   lock;
#else /* not AIOP */
    fsl_handle_t    lock;
#endif
    
} t_mem_mng;


#endif /* __MEM_MNG_H */

