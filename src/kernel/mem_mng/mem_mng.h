/**************************************************************************//**
 @File          mem_mng.h

 @Description   Internal definitions for memory allocation management module.
*//***************************************************************************/
#ifndef __MEM_MNG_H
#define __MEM_MNG_H

#include "inc/mem_mng_util.h"
#include "common/list.h"


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
    fsl_handle_t                lock;
} t_mem_mng_partition;

#define MEM_MNG_PARTITION_OBJECT(p_list)  \
    LIST_OBJECT(p_list, t_mem_mng_partition, node)


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

    /* Variables related to early memory partition */
    void *      (*f_early_malloc)(uint32_t size, uint32_t alignment);
                /**< Early allocation routine (before partitions are registered) */
    void        (*f_early_free)(void *p_addr);
                /**< Early deallocation routine (before partitions are registered) */
    list_t      early_mem_debug_list;
                /**< List of early memory allocation entries (for debug) */

    fsl_handle_t    lock;
} t_mem_mng;



#endif /* __MEM_MNG_H */

