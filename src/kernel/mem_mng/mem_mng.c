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
 @File          mem_mng.c

 @Description   Implementation of the memory allocation management module.
*//***************************************************************************/

//#define ENABLE_DEBUG_ENTRIES

#include "fsl_gen.h"
#include "common/fsl_string.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_slob.h"
#include "fsl_platform.h"
#include "fsl_aiop_common.h"
#include "mem_mng.h"
#include "fsl_sl_dbg.h"
#include "fsl_malloc.h"
#include "fsl_mem_mng.h"
#include "slob.h"
#include "buffer_pool.h"
#include "fsl_cdma.h"



#ifdef UNDER_CONSTRUCTION
//@@@@ todo:

//information:
// per malloc: caller info + string ?
// mallocs in chronological order
// mallocs by size
// memory corruption checks ?

//totals:
// allocated bytes and number of allocations per memory type and alignment
// maximum usage (bytes) per memory type
// overhead (for alignment and management) in bytes and percent
// free space (bytes)

//protection
// check mem management structures for corruption (crc ?)
// memory guards (on head and tail ?)
// setting memory before allocation (identify access to bad addr (mmu) ?)
// setting memory after freeing

#endif /* UNDER_CONSTRUCTION */

/* Array of spinlocks should reside in shared ram memory.
 * They are initialized to 0 (unlocked) */
static uint8_t g_mem_part_spinlock[PLATFORM_MAX_MEM_INFO_ENTRIES] = {0};
static uint8_t g_phys_mem_part_spinlock[PLATFORM_MAX_MEM_INFO_ENTRIES] = {0};
extern struct aiop_init_info g_init_data;
/* Mutex lock to lock calls of slob_put functions. */
//static uint8_t s_free_lock = 0;
/* Mutex lock to lock calls of slob_get functions */
static uint8_t s_malloc_lock = 0;


/* Put all function (execution code) into  dtext_vle section , aka __COLD_CODE */
__START_COLD_CODE



static void mem_mng_add_entry(struct t_mem_mng             *p_mem_mng,
                           t_mem_mng_partition    *p_partition,
                           void                 *p_memory,
                           uint32_t             size,
                           char                 *info,
                           char                 *filename,
                           int                  line);

static int mem_mng_remove_entry(struct t_mem_mng          *p_mem_mng,
                              t_mem_mng_partition *p_partition,
                              void              *p_memory);

static int mem_mng_get_partition_id_by_addr_local(struct t_mem_mng             *p_mem_mng,
                                       uint64_t             addr,
                                       int                  *p_partition_id,
                                       t_mem_mng_partition    **p_partition);


static void mem_mng_free_partition(struct t_mem_mng *p_mem_mng,
                                   t_mem_mng_partition* p_partition);

static void mem_phys_mng_free_partition(struct t_mem_mng *p_mem_mng,
                           t_mem_mng_phys_addr_alloc_partition   *p_partition);




extern const uint8_t AIOP_DDR_START[],AIOP_DDR_END[];
/* Total size of boot memory manager */
const uint32_t  g_boot_mem_mng_size = 1*MEGABYTE;
/* Total size of buffer pool for slob allocations */
const uint32_t g_buffer_pool_size = 512*KILOBYTE;




int boot_mem_mng_init(struct initial_mem_mng* boot_mem_mng,
                      const int mem_partition_id)
{
	uint32_t aiop_lcf_ddr_size =  (uint32_t)(AIOP_DDR_END) -
		(uint32_t)(AIOP_DDR_START);
	switch(mem_partition_id){
	case MEM_PART_DP_DDR:
		boot_mem_mng->base_paddress = g_init_data.sl_info.dp_ddr_paddr
			                     + aiop_lcf_ddr_size;
		boot_mem_mng->base_vaddress = (uint32_t)g_init_data.sl_info.dp_ddr_vaddr
			                    + aiop_lcf_ddr_size;
		boot_mem_mng->size = g_boot_mem_mng_size;
		boot_mem_mng->curr_ptr = boot_mem_mng->base_paddress;

		break;
	case MEM_PART_SYSTEM_DDR:
		boot_mem_mng->base_paddress = g_init_data.sl_info.sys_ddr1_paddr
					                     + aiop_lcf_ddr_size;
		boot_mem_mng->base_vaddress = (uint32_t)g_init_data.sl_info.sys_ddr1_vaddr
					                    + aiop_lcf_ddr_size;
		boot_mem_mng->size = g_boot_mem_mng_size;
		boot_mem_mng->curr_ptr = boot_mem_mng->base_paddress;
		break;
	}

	boot_mem_mng->lock = 0;

	return 0;
}

/*****************************************************************************/
int boot_mem_mng_free(struct initial_mem_mng* boot_mem_mng)
{
	boot_mem_mng->curr_ptr = boot_mem_mng->base_paddress;
	return 0;
}
/*****************************************************************************/
int boot_get_mem(struct initial_mem_mng* boot_mem_mng,
                 const uint64_t size,uint64_t* paddr)
{

	lock_spinlock(&boot_mem_mng->lock);

	if(boot_mem_mng->curr_ptr + size >=
		boot_mem_mng->base_paddress + boot_mem_mng->size)
		return -ENOMEM;
	*paddr = boot_mem_mng->curr_ptr;
	boot_mem_mng->curr_ptr += size;

	unlock_spinlock(&boot_mem_mng->lock);

	return  0;
}
/*****************************************************************************/
int boot_get_mem_virt(struct initial_mem_mng* boot_mem_mng,
                 const uint64_t size,uint32_t* vaddr)
{
	lock_spinlock(&boot_mem_mng->lock);

	if(boot_mem_mng->curr_ptr + size >=
		boot_mem_mng->base_paddress + boot_mem_mng->size)
		return -ENOMEM;
	*vaddr = (uint32_t)(boot_mem_mng->curr_ptr-boot_mem_mng->base_paddress) +
		boot_mem_mng->base_vaddress ;
	boot_mem_mng->curr_ptr += size;

	unlock_spinlock(&boot_mem_mng->lock);

	return  0;
}
/*****************************************************************************/
int  mem_mng_init(void *  h_boot_mem_mng,
                    struct t_mem_mng    *p_mem_mng)
{
    uint32_t      mem_mng_addr = 0;
    int rc = 0, i, array_size = 0;
    uint32_t slob_blocks_num = 0;
    uint16_t alignment = 0;


    ASSERT_COND_LIGHT(h_boot_mem_mng);
    if(NULL == h_boot_mem_mng)
        return NULL;
    /*rc = boot_get_mem_virt(boot_mem_mng,sizeof(t_mem_mng),&mem_mng_addr);
    if (rc)
    {
        pr_err("Mem. manager memory allocation failed: memory manager "
               "structure\n");
        return -ENOMEM;
    }

    p_mem_mng = UINT_TO_PTR(mem_mng_addr);
    */
    memset(p_mem_mng, 0, sizeof(struct t_mem_mng));

    p_mem_mng->h_boot_mem_mng = h_boot_mem_mng;

    /* Set a next power of 2 of sizeof(t_slob_block) as an alignment */
    NEXT_POWER_OF_2(sizeof(t_slob_block),alignment);
    slob_blocks_num = buff_pool_compute_num_buffers(g_buffer_pool_size,sizeof(t_slob_block),alignment);

    rc = buff_pool_create(&p_mem_mng->slob_bf_pool,E_BFT_SLOB_BLOCK,slob_blocks_num,
                            sizeof(t_slob_block),alignment,h_boot_mem_mng);
    if(rc)
    {
	    pr_err("MAJOR mem.manager memory allocation failed slob free blocks\n");
	    return -ENOMEM;
    }
    /* Initialize internal partitions array */
    array_size = ARRAY_SIZE(p_mem_mng->mem_partitions_array);
    for(i = 0 ; i < array_size;i++ ){
        p_mem_mng->mem_partitions_array[i].was_initialized = 0;
    }

    /* Initialize phys internal partitions array */
    array_size = ARRAY_SIZE(p_mem_mng->phys_allocation_mem_partitions_array);
    // Check that memory partition array size greater than MEM_PART_LAST
    ASSERT_COND_LIGHT(array_size>= MEM_PART_LAST);
    for(i = 0 ; i < array_size;i++ ){
        p_mem_mng->phys_allocation_mem_partitions_array[i].was_initialized = 0;
    }

    p_mem_mng->mem_partitions_initialized = 0;

    return 0 ;
}


/*****************************************************************************/
/*
 Caution: * There are no locks on p_mem_mng->mem_partitions_array therefore
 this function should be called only by one core.
 */
void mem_mng_free(void*  h_mem_mng,void*  h_boot_mem_mng)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;

    uint32_t i = 0, array_size = 0;
    UNUSED(h_boot_mem_mng);

    array_size = ARRAY_SIZE(p_mem_mng->mem_partitions_array);
    for ( i = 0; i < array_size ;i++)
    {
        if(p_mem_mng->mem_partitions_array[i].was_initialized){
            mem_mng_free_partition(p_mem_mng, &p_mem_mng->mem_partitions_array[i]);
            p_mem_mng->mem_partitions_array[i].was_initialized = 0;
        }
    }
    array_size = ARRAY_SIZE(p_mem_mng->phys_allocation_mem_partitions_array);
    for ( i = 0; i < array_size ;i++)
    {
        if(p_mem_mng->phys_allocation_mem_partitions_array[i].was_initialized){
	        mem_phys_mng_free_partition(p_mem_mng,&p_mem_mng->phys_allocation_mem_partitions_array[i]);
        }
        p_mem_mng->phys_allocation_mem_partitions_array[i].was_initialized = 0;
    }
}

/*****************************************************************************/
/*
 Caution: * There are no locks on p_mem_mng->mem_partitions_array therefore
 this function should be called only by one core.
 */
int mem_mng_register_partition(void*   h_mem_mng,
                                  int       partition_id,
                                  uintptr_t base_address,
                                  uint64_t  size,
                                  uint32_t  attributes,
                                  char      name[],
                                  int      enable_debug)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition = NULL, *p_new_partition;
    if(p_mem_mng->mem_partitions_array[partition_id].was_initialized)
    {
            pr_err("Mem_manager resource already exists: partition ID %d\n",
                    partition_id);
            return -EEXIST;
    }
    ASSERT_COND(partition_id < ARRAY_SIZE(p_mem_mng->mem_partitions_array));
    p_new_partition = &p_mem_mng->mem_partitions_array[partition_id];

    memset(p_new_partition, 0, sizeof(t_mem_mng_partition));
    /*
    p_new_partition->lock = (uint8_t *)fsl_malloc(sizeof(uint8_t));
    Fix for bug ENGR00337904. Memory address that is used for spinlock
     should reside in shared ram
    */
    p_new_partition->lock = &g_mem_part_spinlock[partition_id];

    if (!p_new_partition->lock)
    {
	p_new_partition->was_initialized = 0;
        pr_err("Mem_manager resource is unavailable: spinlock object for"
                "partition: %s\n", name);
        return -EAGAIN;
    }

    *(p_new_partition->lock) = 0;

    /* Prevent allocation of address 0x00000000 (reserved to NULL) */
    if (base_address == 0)
    {
        base_address += 4;
        size -= 4;
    }

    /* Initialize the memory manager handle for the new partition */
    if (0 != slob_init(&(p_new_partition->h_mem_manager), base_address, size,
                       p_mem_mng->h_boot_mem_mng,&p_mem_mng->slob_bf_pool))
    {
        p_new_partition->was_initialized = 0;
        pr_err("Mem_manager resource is unavailable: slob  object for "
				"partition: %s\n", name);
	return -EAGAIN;
    }


    /* Copy partition name */
    strncpy(p_new_partition->info.name, name, MEM_MNG_MAX_PARTITION_NAME_LEN-1);

#ifdef ENABLE_DEBUG_ENTRIES
    /* Initialize debug entries list */
    INIT_LIST(&(p_new_partition->mem_debug_list));
#endif

    /* Store other parameters */
    p_new_partition->id = partition_id;
    p_new_partition->info.base_address = base_address;
    p_new_partition->info.size = size;
    p_new_partition->info.attributes = attributes;
    p_new_partition->enable_debug = enable_debug;
    p_new_partition->was_initialized = 1;

    return 0;
}
/*****************************************************************************/
/*
 Caution: * There are no locks on p_mem_mng->mem_partitions_array therefore
 this function should be called only by one core.
 */
int mem_mng_register_phys_addr_alloc_partition(void*   h_mem_mng,
                                  int       partition_id,
                                  uint64_t base_paddress,
                                  uint64_t  size,
                                  uint32_t  attributes,
                                  char      name[])
{
	struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
	t_mem_mng_phys_addr_alloc_partition   *p_partition = NULL, *p_new_partition;
	uint32_t i = 0, array_size = 0;
	ASSERT_COND(partition_id < ARRAY_SIZE(p_mem_mng->phys_allocation_mem_partitions_array));
    if(p_mem_mng->phys_allocation_mem_partitions_array[partition_id].was_initialized){

            pr_err("Mem. manager resource already exists: partition ID %d\n",
                   partition_id);
            return -EEXIST;
     }

   p_new_partition = &p_mem_mng->phys_allocation_mem_partitions_array[partition_id];
   memset(p_new_partition, 0, sizeof(t_mem_mng_phys_addr_alloc_partition));

    /*
     * Fix for bug ENGR00337904. Memory address that is used for spinlock
     * should reside in shared ram
    p_new_partition->lock = (uint8_t *)fsl_malloc(sizeof(uint8_t));
    */
    p_new_partition->lock = &g_phys_mem_part_spinlock[partition_id];
    *(p_new_partition->lock) = 0;

    if (!p_new_partition->lock)
    {
         /*p_mem_mng->f_free(p_new_partition); */
        p_new_partition->was_initialized = 0;
        pr_err("Mem. manager resource is unavailable: spinlock object for "
               "partition: %s\n", name);
        return -EAGAIN;
    }


    /* Initialize the memory manager handle for the new partition */
   if (0 != slob_init(&(p_new_partition->h_mem_manager), base_paddress, size,
		   p_mem_mng->h_boot_mem_mng,&p_mem_mng->slob_bf_pool))
   {
       p_new_partition->was_initialized = 0;
       pr_err("Mem. manager resource is unavailable: slob object for "
               "partition: %s\n",name);
       return -EAGAIN;
   }

   /* Copy partition name */
   strncpy(p_new_partition->info.name, name, MEM_MNG_MAX_PARTITION_NAME_LEN-1);

   /* Store other parameters */
   p_new_partition->id = partition_id;
   p_new_partition->info.base_paddress = base_paddress;
   p_new_partition->info.size = size;
   p_new_partition->info.attributes = attributes;
   p_new_partition->was_initialized = 1;



   return 0;
}


/*****************************************************************************/
/*
 Caution: * There are no locks on p_mem_mng->mem_partitions_array therefore
 this function should be called only by one core.
 */
int mem_mng_unregister_partition(void*  h_mem_mng, int partition_id)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    t_mem_mng_phys_addr_alloc_partition *p_phys_partition;
    uint32_t i = 0, array_size = 0;

    /* Find the requested partition and release it */
    p_partition = &p_mem_mng->mem_partitions_array[partition_id];

    if (p_partition->was_initialized)
    {
            mem_mng_free_partition(p_mem_mng, p_partition);
            p_partition->was_initialized = 0;
            return 0;
    }
    else
    {
	p_phys_partition = &p_mem_mng->phys_allocation_mem_partitions_array[partition_id];
        if (p_phys_partition->was_initialized)
        {

             p_phys_partition->was_initialized = 0;
             return 0;
        }
    }

    pr_err("Mem. manager unsupported operation: partition ID %d\n", partition_id);
    return -ENOTSUP;
}


/*****************************************************************************/
/*
 * Caution: No locks on p_mem_mng->mem_partitions_array.
 */
int mem_mng_get_partition_info(void*                h_mem_mng,
                                  int                   partition_id,
                                  t_mem_mng_partition_info *p_partition_info)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    p_partition =  &(p_mem_mng->mem_partitions_array[partition_id]);
    if (p_partition->was_initialized)
    {
	    *p_partition_info = p_partition->info;
            return 0;
    }
    sl_pr_err("Mem. manager resource is unavailable: partition ID %d\n",
              partition_id);
    return -EAGAIN;
}
/*****************************************************************************/
/*
 * Caution: No locks on p_mem_mng->mem_partitions_array.
 */
int mem_mng_get_phys_addr_alloc_info(void*                h_mem_mng,
                                  int                   partition_id,
                                  t_mem_mng_phys_addr_alloc_info *p_partition_info)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_phys_addr_alloc_partition   *p_partition;
    p_partition =  &(p_mem_mng->phys_allocation_mem_partitions_array[partition_id]);
    if (p_partition->was_initialized)
    {
            *p_partition_info = p_partition->info;
            return 0;
    }

    sl_pr_err("Mem. manager resource is unavailable: partition ID %d\n",
              partition_id);
    return -EAGAIN;
}

/*****************************************************************************/
#if 0
int mem_mng_get_partition_id_by_addr(void*    h_mem_mng,
                                     uint64_t   addr,
                                     int        *p_partition_id)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    int                found;

    found = mem_mng_get_partition_id_by_addr_local(p_mem_mng, addr, p_partition_id, &p_partition);

    if (!found)
    {
        pr_err("Mem.manager resource is unavailable: partition ID\n",
               p_partition_id);
        return -EAGAIN;
    }

    return 0;
}

#endif
/*****************************************************************************/
/*
 * Caution: No locks on p_mem_mng->mem_partitions_array.
 */
uint32_t mem_mng_check_leaks(void*                 h_mem_mng,
                            int                     partition_id,
                            t_mem_mng_leak_report_func  *f_report_leak)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
#ifdef ENABLE_DEBUG_ENTRIES
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_debug_iterator;
#else
    UNUSED(f_report_leak);
#endif

    uint32_t            count = 0;
    uint32_t  i = 0;

/* Search in registered partitions */
    p_partition  = &p_mem_mng->mem_partitions_array[partition_id];
    if(p_partition->id != partition_id)
    {
        pr_err("Mem. manager resource not found: partition ID %d\n",
            partition_id);
        return 0;
    }
#ifdef ENABLE_DEBUG_ENTRIES
    if (f_report_leak)
    {
        LIST_FOR_EACH(p_debug_iterator, &(p_partition->mem_debug_list))
	{
	    count++;
            p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

	    f_report_leak(p_mem_mng_debug_entry->p_memory,
			  p_mem_mng_debug_entry->size,
			  p_mem_mng_debug_entry->info,
			  p_mem_mng_debug_entry->filename,
			  p_mem_mng_debug_entry->line);

	}
    }
    else
	count = (uint32_t)list_num_of_objs(&(p_partition->mem_debug_list));
#endif

	unlock_spinlock(p_partition->lock);

	return count;
}

/*****************************************************************************/
/*
 * Caution: No locks on p_mem_mng->mem_partitions_array.
 */

MEM_MNG_CODE_PLACEMENT void * mem_mng_alloc_mem(void*     h_mem_mng,
                        int         partition_id,
                        uint32_t    size,
                        uint32_t    alignment,
                        char        *info,
                        char        *filename,
                        int         line)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    struct initial_mem_mng* boot_mem_mng = NULL;
    t_mem_mng_partition   *p_partition;
    uint32_t i = 0, array_size = 0,virt_address = 0;
    void                *p_memory;


#ifndef ENABLE_DEBUG_ENTRIES
    UNUSED(info);
    UNUSED(filename);
    UNUSED(line);
#endif

    if (size == 0)
    {
        sl_pr_err("Mem.manager invalid value: allocation size must be positive\n");
    }

    /* Not early allocation - allocate from registered partitions */
    p_partition = &p_mem_mng->mem_partitions_array[partition_id];
    if(!p_partition->was_initialized || p_partition->id != partition_id)
    {
        sl_pr_err("Mem. manager resource not found: partition ID %d\n",
                  partition_id);
        return NULL;
    }
    cdma_mutex_lock_take((uint64_t)&s_malloc_lock,CDMA_MUTEX_WRITE_LOCK);
    p_memory = UINT_TO_PTR(
    slob_get(&p_partition->h_mem_manager, size, alignment));
    cdma_mutex_lock_release((uint64_t)&s_malloc_lock);
    if ((uintptr_t)p_memory == 0LL)
        /* Do not report error - let the allocating entity report it */
    return NULL;
#ifdef ENABLE_DEBUG_ENTRIES

        lock_spinlock(p_mem_mng->lock);

        if (p_partition->enable_debug)
        {
            mem_mng_add_entry(p_mem_mng,
                           p_partition,
                           p_memory,
                           size,
                           info,
                           filename,
                           line);
        }


        unlock_spinlock(p_mem_mng->lock);
#endif
        return p_memory;
}

/*****************************************************************************/
/*
 * Caution: No locks on p_mem_mng->mem_partitions_array.
 */
MEM_MNG_CODE_PLACEMENT int mem_mng_get_phys_mem(void*  h_mem_mng, int  partition_id,
                        uint64_t size, uint64_t alignment,
                        uint64_t *paddr)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_phys_addr_alloc_partition   *p_partition;
    uint32_t array_size = 0, i = 0;


    if (size == 0)
    {
        sl_pr_err("Mem. manager invalid value: allocation size must be positive\n");
    }

    /* Not early allocation - allocate from registered partitions */
    p_partition = &p_mem_mng->phys_allocation_mem_partitions_array[partition_id];
    if (p_partition->was_initialized)
    {
	    cdma_mutex_lock_take((uint64_t)&s_malloc_lock,CDMA_MUTEX_WRITE_LOCK);
            if((*paddr = slob_get(&p_partition->h_mem_manager,size,
                                  (uint32_t)alignment)) == 0LL)
            {
                sl_pr_err("Mem. manager memory allocation failed: Required size 0x%x%08x exceeds "
                   "available memory for partition ID %d\n",
                   (uint32_t)(size >> 32),(uint32_t)size,partition_id);
                cdma_mutex_lock_release((uint64_t)&s_malloc_lock);
                return -ENOMEM;
            }
            cdma_mutex_lock_release((uint64_t)&s_malloc_lock);
            return 0; // Success
    }
    sl_pr_err("Partition ID %d is not found\n", partition_id);
    return -EINVAL;
}

/*****************************************************************************/
/*
 * Caution: No locks on p_mem_mng->mem_partitions_array.
 */
MEM_MNG_CODE_PLACEMENT void mem_mng_put_phys_mem(void*  h_mem_mng, uint64_t paddress)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_phys_addr_alloc_partition   *p_partition;
    uint32_t             array_size = 0, i = 0;

    array_size = ARRAY_SIZE(p_mem_mng->phys_allocation_mem_partitions_array);
    /* Not early allocation - allocate from registered partitions */
    for(i = 0; i < array_size ; i++)
    {
        p_partition  = &(p_mem_mng->phys_allocation_mem_partitions_array[i]);
        if (p_partition->was_initialized && paddress >= p_partition->info.base_paddress &&
           paddress < (p_partition->info.base_paddress + p_partition->info.size))
	{
            cdma_mutex_lock_take((uint64_t)&s_malloc_lock,CDMA_MUTEX_WRITE_LOCK);
            slob_put(&p_partition->h_mem_manager,paddress);
            cdma_mutex_lock_release((uint64_t)&s_malloc_lock);
            return;
	}
    }// for
    return;
}
/*****************************************************************************/

MEM_MNG_CODE_PLACEMENT void mem_mng_free_mem(void*  h_mem_mng, void *p_memory)
{
    struct t_mem_mng            *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    uintptr_t           addr = (uintptr_t)p_memory;
    int                 partition_id;
    int                address_found = 1;

    if (mem_mng_get_partition_id_by_addr_local(p_mem_mng, addr, &partition_id, &p_partition))
    {
#ifdef ENABLE_DENUG_ENTRIES
        if (p_partition->enable_debug &&
	    !mem_mng_remove_entry(p_mem_mng, p_partition, p_memory))
	{
		address_found = 0;
	}
#endif
	if (address_found)
	{
		cdma_mutex_lock_take((uint64_t)&s_malloc_lock,CDMA_MUTEX_WRITE_LOCK);
		slob_put(&p_partition->h_mem_manager, PTR_TO_UINT(p_memory));
		cdma_mutex_lock_release((uint64_t)&s_malloc_lock);
	}
    }
    else
    {
	address_found = 0;
    }

    if (!address_found)
    {
        pr_err("Mem. manager resource not found: attempt to free "
                "unallocated address (0x%08x)\n",p_memory);
    }
}


/*****************************************************************************/
/*
 * Caution: No locks on p_mem_mng->mem_partitions_array.
 */
static int mem_mng_get_partition_id_by_addr_local(struct t_mem_mng          *p_mem_mng,
                                       uint64_t          addr,
                                       int               *p_partition_id,
                                       t_mem_mng_partition **p_partition)
{
    t_mem_mng_partition   *p_tmp_partition;
    uint32_t i = 0, array_size = 0;

    array_size = ARRAY_SIZE(p_mem_mng->mem_partitions_array);
    for (i = 0; i < array_size; i++)
    {
	p_tmp_partition =  &p_mem_mng->mem_partitions_array[i];
        if (p_tmp_partition->was_initialized &&
            (addr >= p_tmp_partition->info.base_address) &&
            (addr < (p_tmp_partition->info.base_address + p_tmp_partition->info.size)))
        {
            *p_partition_id = p_tmp_partition->id;
            *p_partition = p_tmp_partition;
            return 1;
        }
    }

    return 0;
}
/*****************************************************************************/
static void mem_mng_free_partition(struct t_mem_mng *p_mem_mng,
                                   t_mem_mng_partition* p_partition)
{
#ifdef ENABLE_DEBUG_ENTRIES
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_debug_iterator, *p_tmp_iterator;
#endif

    UNUSED(p_mem_mng);

    lock_spinlock(p_partition->lock);

#ifdef ENABLE_DEBUG_ENTRIES
    /* Release the debug entries list */
    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_partition->mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        list_del(p_debug_iterator);
        sys_default_free(p_mem_mng_debug_entry);
    }
#endif

    unlock_spinlock(p_partition->lock);

    /* Release the memory manager object */
    slob_free(&p_partition->h_mem_manager);
}

/*****************************************************************************/
static void mem_phys_mng_free_partition(struct t_mem_mng *p_mem_mng,
                                        t_mem_mng_phys_addr_alloc_partition   *p_partition )
{
    UNUSED(p_mem_mng);
    /* Release the memory manager object */
    slob_free(&p_partition->h_mem_manager);
}
/*****************************************************************************/
#ifdef ENABLE_DEBUG_ENTRIES
static void mem_mng_add_entry(struct t_mem_mng             *p_mem_mng,
                           t_mem_mng_partition    *p_partition,
                           void                 *p_memory,
                           uint32_t             size,
                           char                 *info,
                           char                 *filename,
                           int                  line)
{
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    UNUSED(p_mem_mng);

    p_mem_mng_debug_entry = sys_default_malloc(sizeof(t_mem_mng_debug_entry));
        //(t_mem_mng_debug_entry *)p_mem_mng->f_malloc(sizeof(t_mem_mng_debug_entry));


    if (p_mem_mng_debug_entry != NULL)
    {
        INIT_LIST(&p_mem_mng_debug_entry->node);
        p_mem_mng_debug_entry->p_memory = p_memory;
        p_mem_mng_debug_entry->filename = filename;
        p_mem_mng_debug_entry->info = info;
        p_mem_mng_debug_entry->line = line;
        p_mem_mng_debug_entry->size = size;

        lock_spinlock(p_partition->lock);

        list_add_to_tail(&p_mem_mng_debug_entry->node, &(p_partition->mem_debug_list));

        unlock_spinlock(p_partition->lock);

        p_partition->info.current_usage += size;
        p_partition->info.total_allocations += 1;

#if 0
        //@@@@ add check for size overflow
#endif
        if (p_partition->info.current_usage > p_partition->info.maximum_usage)
        {
            p_partition->info.maximum_usage = p_partition->info.current_usage;
        }
    }
    else
    {
        pr_err("Mem. manager memory allocation failed: memory manager "
                "debug entry \n");
    }
}


/*****************************************************************************/
static int mem_mng_remove_entry(struct t_mem_mng          *p_mem_mng,
                              t_mem_mng_partition *p_partition,
                              void              *p_memory)
{
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_debug_iterator, *p_tmp_iterator;

    lock_spinlock(p_partition->lock);

    UNUSED(p_mem_mng);
    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_partition->mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        if (p_mem_mng_debug_entry->p_memory == p_memory)
        {
            p_partition->info.current_usage -= p_mem_mng_debug_entry->size;
            p_partition->info.total_deallocations += 1;

            list_del(p_debug_iterator);

            unlock_spinlock(p_partition->lock);

            sys_default_free(p_mem_mng_debug_entry);
            return 1;
        }
    }


    unlock_spinlock(p_partition->lock);

    return 0;
}
#endif

int mem_mng_mem_partitions_init_completed(void*  h_mem_mng)
{
	struct t_mem_mng  *p_mem_mng = (struct t_mem_mng *)h_mem_mng;
	p_mem_mng->mem_partitions_initialized = 1;
	return 0;
}
__END_COLD_CODE
