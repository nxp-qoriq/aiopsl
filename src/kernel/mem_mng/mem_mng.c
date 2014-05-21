/**************************************************************************//**
 @File          mem_mng.c

 @Description   Implementation of the memory allocation management module.
*//***************************************************************************/

#include "common/fsl_string.h"
#include "kernel/fsl_spinlock.h"
#include "common/slob.h"
#ifdef AIOP
#include "common/fsl_malloc.h"
#endif /* AIOP */

#include "mem_mng.h"

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


static void mem_mng_add_early_entry(t_mem_mng    *p_mem_mng,
                                void        *p_memory,
                                uint32_t    size,
                                char        *info,
                                char        *filename,
                                int         line);

static int mem_mng_remove_early_entry(t_mem_mng *p_mem_mng, void *p_memory);

static void mem_mng_add_entry(t_mem_mng             *p_mem_mng,
                           t_mem_mng_partition    *p_partition,
                           void                 *p_memory,
                           uint32_t             size,
                           char                 *info,
                           char                 *filename,
                           int                  line);

static int mem_mng_remove_entry(t_mem_mng          *p_mem_mng,
                              t_mem_mng_partition *p_partition,
                              void              *p_memory);

static int mem_mng_get_partition_id_by_addr_local(t_mem_mng             *p_mem_mng,
                                       uint64_t             addr,
                                       int                  *p_partition_id,
                                       t_mem_mng_partition    **p_partition);

static void mem_mng_free_partition(t_mem_mng *p_mem_mng, list_t *p_partition_iterator);


/*****************************************************************************/
fsl_handle_t mem_mng_init(t_mem_mng_param *p_mem_mng_param)
{
    t_mem_mng    *p_mem_mng;

    if (!(p_mem_mng_param->f_malloc       && p_mem_mng_param->f_free &&
          p_mem_mng_param->f_early_malloc  && p_mem_mng_param->f_early_free))
    {
        REPORT_ERROR(MAJOR, ENODEV, ("malloc/free callback routine"));
        return NULL;
    }

    p_mem_mng = p_mem_mng_param->f_malloc(sizeof(t_mem_mng));
    if (!p_mem_mng)
    {
        REPORT_ERROR(MAJOR, E_NO_MEMORY, ("memory manager structure"));
        return NULL;
    }
    memset(p_mem_mng, 0, sizeof(t_mem_mng));

    p_mem_mng->f_malloc      = p_mem_mng_param->f_malloc;
    p_mem_mng->f_free        = p_mem_mng_param->f_free;
    p_mem_mng->f_early_malloc = p_mem_mng_param->f_early_malloc;
    p_mem_mng->f_early_free   = p_mem_mng_param->f_early_free;
    p_mem_mng->lock    = p_mem_mng_param->lock;

    /* Initialize internal partitions list */
    INIT_LIST(&(p_mem_mng->mem_partitions_list));
    /* Initialize the early allocations list */
    INIT_LIST(&(p_mem_mng->early_mem_debug_list));

    return p_mem_mng;
}


/*****************************************************************************/
void mem_mng_free(fsl_handle_t h_mem_mng)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    list_t              *p_partition_iterator, *p_tmp_iterator;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */
    
    if (p_mem_mng->lock)
    {
#ifdef AIOP
        lock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
        int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif /* AIOP */
        LIST_FOR_EACH_SAFE(p_partition_iterator,
                           p_tmp_iterator,
                           &(p_mem_mng->mem_partitions_list))
        {
            mem_mng_free_partition(p_mem_mng, p_partition_iterator);
        }
#ifdef AIOP
        unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
        spin_unlock_irqrestore(p_mem_mng->lock, int_flags);  
#endif /* AIOP */
    }

    p_mem_mng->f_free(p_mem_mng);
}


/*****************************************************************************/
int mem_mng_get_available_partition_id(fsl_handle_t h_mem_mng)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    list_t              *p_partition_iterator;
    int                 available_id = 1;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif
    /* Return the next ID higher than any registered partition ID */
    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        available_id = p_partition->id + 1;
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif

    return available_id;
}


/*****************************************************************************/
int mem_mng_register_partition(fsl_handle_t  h_mem_mng,
                                  int       partition_id,
                                  uintptr_t base_address,
                                  uint64_t  size,
                                  uint32_t  attributes,
                                  char      name[],
                                  void *    (*f_user_malloc)(uint32_t size, uint32_t alignment),
                                  void      (*f_user_free)(void *p_addr),
                                  int      enable_debug)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition = NULL, *p_new_partition;
    list_t              *p_partition_iterator;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

    if ((f_user_malloc || f_user_free) && !(f_user_malloc && f_user_free))
    {
        RETURN_ERROR(MAJOR, E_INVALID_VALUE,
                     ("f_user_malloc and f_user_free must be both NULL or not NULL"));
    }

#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif
    
    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
#ifdef AIOP
            unlock_spinlock(p_mem_mng->lock);
#else
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif
            RETURN_ERROR(MAJOR, E_ALREADY_EXISTS, ("partition ID %d", partition_id));
        }
        else if (p_partition->id > partition_id)
        {
            break;
        }
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif

    p_new_partition = (t_mem_mng_partition *)p_mem_mng->f_malloc(sizeof(t_mem_mng_partition));
    if (!p_new_partition)
    {
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory manager partition"));
    }
    memset(p_new_partition, 0, sizeof(t_mem_mng_partition));
#ifdef AIOP
    p_new_partition->lock = (uint8_t *)fsl_os_malloc(sizeof(uint8_t));
#else
    p_new_partition->lock = spin_lock_create();
#endif
    if (!p_new_partition->lock)
    {
        p_mem_mng->f_free(p_new_partition);
        RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("spinlock object for partition: %s", name));
    }
    
#ifdef AIOP
    p_new_partition->lock = 0;
#endif /* AIOP */

    if (!f_user_malloc)
    {
        /* Prevent allocation of address 0x00000000 (reserved to NULL) */
        if (base_address == 0)
        {
            base_address += 4;
            size -= 4;
        }

        /* Initialize the memory manager handle for the new partition */
        if (E_OK != slob_init(&(p_new_partition->h_mem_manager), base_address, size))
        {
            p_mem_mng->f_free(p_new_partition);
            RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("MM object for partition: %s", name));
        }
    }

    /* Copy partition name */
    strncpy(p_new_partition->info.name, name, MEM_MNG_MAX_PARTITION_NAME_LEN-1);

    /* Initialize debug entries list */
    INIT_LIST(&(p_new_partition->mem_debug_list));

    /* Store other parameters */
    p_new_partition->id = partition_id;
    p_new_partition->info.base_address = base_address;
    p_new_partition->info.size = size;
    p_new_partition->info.attributes = attributes;
    p_new_partition->f_user_malloc = f_user_malloc;
    p_new_partition->f_user_free = f_user_free;
    p_new_partition->enable_debug = enable_debug;

#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif
    /* Add the new partition to the sorted position in the partitions list */
    if (list_is_empty(&(p_mem_mng->mem_partitions_list)))
    {
        list_add(&(p_new_partition->node), &(p_mem_mng->mem_partitions_list));
    }
    else
    {
	ASSERT_COND(p_partition);
        if (p_partition->id < partition_id)
            list_add(&(p_new_partition->node), &(p_partition->node));
        else
            list_add(&(p_new_partition->node), p_partition->node.prev);
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif /* AIOP */

    return E_OK;
}


/*****************************************************************************/
int mem_mng_unregister_partition(fsl_handle_t h_mem_mng, int partition_id)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    list_t              *p_partition_iterator, *p_tmp_iterator;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif /* AIOP */
    /* Find the requested partition and release it */
    LIST_FOR_EACH_SAFE(p_partition_iterator,
                       p_tmp_iterator,
                       &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
#ifdef AIOP
            unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif /* AIOP */
            mem_mng_free_partition(p_mem_mng, p_partition_iterator);
            return E_OK;
        }
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif

    RETURN_ERROR(MAJOR, E_NOT_SUPPORTED, ("partition ID %d", partition_id));
}


/*****************************************************************************/
int mem_mng_get_partition_info(fsl_handle_t               h_mem_mng,
                                  int                   partition_id,
                                  t_mem_mng_partition_info *p_partition_info)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    list_t              *p_partition_iterator;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif
    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
            *p_partition_info = p_partition->info;
#ifdef AIOP
            unlock_spinlock(p_mem_mng->lock);
#else
    	    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif
            return E_OK;
        }
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif

    RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("partition ID %d", partition_id));
}


/*****************************************************************************/
int mem_mng_get_partition_id_by_addr(fsl_handle_t   h_mem_mng,
                                     uint64_t   addr,
                                     int        *p_partition_id)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    int                found;

    found = mem_mng_get_partition_id_by_addr_local(p_mem_mng, addr, p_partition_id, &p_partition);

    if (!found)
    {
        RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("partition ID"));
    }

    return E_OK;
}


/*****************************************************************************/
uint32_t mem_mng_check_leaks(fsl_handle_t                h_mem_mng,
                            int                     partition_id,
                            t_mem_mng_leak_report_func  *f_report_leak)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_partition_iterator;
    list_t              *p_debug_iterator;
    uint32_t            count = 0;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif
    if (partition_id == MEM_MNG_EARLY_PARTITION_ID)
    {
        if (f_report_leak)
        {
            LIST_FOR_EACH(p_debug_iterator, &(p_mem_mng->early_mem_debug_list))
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
        {
            LIST_FOR_EACH(p_debug_iterator, &(p_mem_mng->early_mem_debug_list))
            {
                count++;
            }
        }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif
        return count;
    }
    else
    {
        /* Search in registered partitions */
        LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
        {
            p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

            if (p_partition->id == partition_id)
            {
#ifdef AIOP
                lock_spinlock(p_partition->lock);
#else
                spin_lock(p_partition->lock);
#endif
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

#ifdef AIOP
                unlock_spinlock(p_partition->lock);
                unlock_spinlock(p_mem_mng->lock);
#else
                spin_unlock(p_partition->lock);
                spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif
                return count;
            }
        }
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif

    REPORT_ERROR(MAJOR, E_NOT_FOUND, ("partition ID %d", partition_id));
    return 0;
}


/*****************************************************************************/
void * mem_mng_alloc_mem(fsl_handle_t    h_mem_mng,
                        int         partition_id,
                        uint32_t    size,
                        uint32_t    alignment,
                        char        *info,
                        char        *filename,
                        int         line)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    list_t              *p_partition_iterator, *p_temp;
    void                *p_memory;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

    if (size == 0)
    {
        REPORT_ERROR(MAJOR, E_INVALID_VALUE, ("allocation size must be positive"));
    }

    /* Check if this is an early allocation */
    if (partition_id == MEM_MNG_EARLY_PARTITION_ID)
    {
        /* Use early allocation routine */
        p_memory = p_mem_mng->f_early_malloc(size, alignment);
        if (!p_memory)
        {
            REPORT_ERROR(MINOR, E_NO_MEMORY, ("early allocation"));
            return NULL;
        }

        mem_mng_add_early_entry(p_mem_mng,
                            p_memory,
                            size,
                            info,
                            filename,
                            line);

        return p_memory;
    }

#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif
    /* Not early allocation - allocate from registered partitions */
    LIST_FOR_EACH_SAFE(p_partition_iterator, p_temp, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
#ifdef AIOP
            unlock_spinlock(p_mem_mng->lock);
#else
    	    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif
            if (p_partition->f_user_malloc)
            {
                /* User-defined malloc */
                p_memory = p_partition->f_user_malloc(size, alignment);
                if (!p_memory)
                    /* Do not report error - let the allocating entity report it */
                    return NULL;
            }
            else
            {
                /* Internal MM malloc */
                p_memory = UINT_TO_PTR(
                    slob_get(p_partition->h_mem_manager, size, alignment, ""));
                if ((uintptr_t)p_memory == ILLEGAL_BASE)
                    /* Do not report error - let the allocating entity report it */
                    return NULL;
            }
#ifdef AIOP
            lock_spinlock(p_mem_mng->lock);
#else
            int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif

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

#ifdef AIOP
            unlock_spinlock(p_mem_mng->lock);
#else
    	    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif
            return p_memory;
        }
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif

    REPORT_ERROR(MAJOR, E_NOT_FOUND, ("partition ID %d", partition_id));
    return NULL;
}


/*****************************************************************************/
void mem_mng_free_mem(fsl_handle_t h_mem_mng, void *p_memory)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    uintptr_t           addr = (uintptr_t)p_memory;
    int                 partition_id;
    int                address_found = 1;

    /* Try to find the entry in the early allocations list */
    if (mem_mng_remove_early_entry(p_mem_mng, p_memory))
    {
        p_mem_mng->f_early_free(p_memory);
    }
    else
    {
        if (mem_mng_get_partition_id_by_addr_local(p_mem_mng, addr, &partition_id, &p_partition))
        {
            if (p_partition->enable_debug &&
                !mem_mng_remove_entry(p_mem_mng, p_partition, p_memory))
            {
                address_found = 0;
            }

            if (address_found)
            {
                if (p_partition->f_user_free)
                {
                    /* User-defined malloc */
                    p_partition->f_user_free(p_memory);
                }
                else
                {
                    slob_put(p_partition->h_mem_manager, PTR_TO_UINT(p_memory));
                }
            }
        }
        else
        {
            address_found = 0;
        }
    }

    if (!address_found)
    {
        REPORT_ERROR(MAJOR, E_NOT_FOUND,
                     ("attempt to free unallocated address (0x%08x)\n", p_memory));
    }
}


/*****************************************************************************/
static int mem_mng_get_partition_id_by_addr_local(t_mem_mng          *p_mem_mng,
                                       uint64_t          addr,
                                       int               *p_partition_id,
                                       t_mem_mng_partition **p_partition)
{
    t_mem_mng_partition   *p_tmp_partition;
    list_t              *p_partition_iterator;
#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    uint32_t            int_flags;
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif /* AIOP */

    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_tmp_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if ((addr >= p_tmp_partition->info.base_address) &&
            (addr < (p_tmp_partition->info.base_address + p_tmp_partition->info.size)))
        {
            *p_partition_id = p_tmp_partition->id;
            *p_partition = p_tmp_partition;
#ifdef AIOP
            unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
    	    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif /* AIOP */
            return 1;
        }
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif /* AIOP */

    return 0;
}


/*****************************************************************************/
static void mem_mng_free_partition(t_mem_mng *p_mem_mng, list_t *p_partition_iterator)
{
    t_mem_mng_partition   *p_partition;
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_debug_iterator, *p_tmp_iterator;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

    p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

#ifdef AIOP
    lock_spinlock(p_partition->lock);
#else
    int_flags = spin_lock_irqsave(p_partition->lock);
#endif
    
    /* Release the debug entries list */
    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_partition->mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        list_del(p_debug_iterator);
        p_mem_mng->f_free(p_mem_mng_debug_entry);
    }
#ifdef AIOP
    unlock_spinlock(p_partition->lock);
#else /* not AIOP */
    spin_unlock_irqrestore(p_partition->lock, int_flags);
#endif /* AIOP */

    if (!p_partition->f_user_malloc)
    {
        /* Release the memory manager object */
        slob_free(p_partition->h_mem_manager);
    }

    /* Remove from partitions list and free the allocated memory */
    list_del(p_partition_iterator);
    if (p_partition->lock) {
#ifdef AIOP
	fsl_os_free((void *) p_partition->lock);
#else
        spin_lock_free(p_partition->lock);
#endif
    }
    p_mem_mng->f_free(p_partition);
}


/*****************************************************************************/
static void mem_mng_add_early_entry(t_mem_mng    *p_mem_mng,
                                void        *p_memory,
                                uint32_t    size,
                                char        *info,
                                char        *filename,
                                int         line)
{
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

    p_mem_mng_debug_entry =
        (t_mem_mng_debug_entry *)p_mem_mng->f_malloc(sizeof(t_mem_mng_debug_entry));

    if (p_mem_mng_debug_entry != NULL)
    {
        INIT_LIST(&p_mem_mng_debug_entry->node);
        p_mem_mng_debug_entry->p_memory = p_memory;
        p_mem_mng_debug_entry->filename = filename;
        p_mem_mng_debug_entry->info = info;
        p_mem_mng_debug_entry->line = line;
        p_mem_mng_debug_entry->size = size;
#ifdef AIOP
        lock_spinlock(p_mem_mng->lock);
#else
        int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif
        list_add_to_tail(&p_mem_mng_debug_entry->node, &(p_mem_mng->early_mem_debug_list));
#ifdef AIOP
        unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
    	spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif /* AIOP */
    }
    else
    {
        REPORT_ERROR(MAJOR, E_NO_MEMORY, ("memory manager debug entry"));
    }
}


/*****************************************************************************/
static int mem_mng_remove_early_entry(t_mem_mng *p_mem_mng, void *p_memory)
{
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_debug_iterator, *p_tmp_iterator;
#ifdef AIOP
    lock_spinlock(p_mem_mng->lock);
#else
    uint32_t            int_flags;
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
#endif /* AIOP */
    
    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_mem_mng->early_mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        if (p_mem_mng_debug_entry->p_memory == p_memory)
        {
            list_del(p_debug_iterator);
#ifdef AIOP
            unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
    	    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif /* AIOP */
            p_mem_mng->f_free(p_mem_mng_debug_entry);
            return 1;
        }
    }
#ifdef AIOP
    unlock_spinlock(p_mem_mng->lock);
#else /* not AIOP */
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
#endif /* AIOP */

    return 0;
}


/*****************************************************************************/
static void mem_mng_add_entry(t_mem_mng             *p_mem_mng,
                           t_mem_mng_partition    *p_partition,
                           void                 *p_memory,
                           uint32_t             size,
                           char                 *info,
                           char                 *filename,
                           int                  line)
{
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
#ifndef AIOP
    uint32_t            int_flags;
#endif /* AIOP */

    p_mem_mng_debug_entry =
        (t_mem_mng_debug_entry *)p_mem_mng->f_malloc(sizeof(t_mem_mng_debug_entry));

    if (p_mem_mng_debug_entry != NULL)
    {
        INIT_LIST(&p_mem_mng_debug_entry->node);
        p_mem_mng_debug_entry->p_memory = p_memory;
        p_mem_mng_debug_entry->filename = filename;
        p_mem_mng_debug_entry->info = info;
        p_mem_mng_debug_entry->line = line;
        p_mem_mng_debug_entry->size = size;
#ifdef AIOP
        lock_spinlock(p_partition->lock);
#else
        int_flags = spin_lock_irqsave(p_partition->lock);
#endif
        list_add_to_tail(&p_mem_mng_debug_entry->node, &(p_partition->mem_debug_list));
#ifdef AIOP
        unlock_spinlock(p_partition->lock);
#else /* not AIOP */
        spin_unlock_irqrestore(p_partition->lock, int_flags);
#endif /* AIOP */

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
        REPORT_ERROR(MAJOR, E_NO_MEMORY, ("memory manager debug entry"));
    }
}


/*****************************************************************************/
static int mem_mng_remove_entry(t_mem_mng          *p_mem_mng,
                              t_mem_mng_partition *p_partition,
                              void              *p_memory)
{
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_debug_iterator, *p_tmp_iterator;
#ifdef AIOP
    lock_spinlock(p_partition->lock);
#else
    uint32_t            int_flags;
    int_flags = spin_lock_irqsave(p_partition->lock);
#endif /* AIOP */

    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_partition->mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        if (p_mem_mng_debug_entry->p_memory == p_memory)
        {
            p_partition->info.current_usage -= p_mem_mng_debug_entry->size;
            p_partition->info.total_deallocations += 1;

            list_del(p_debug_iterator);
#ifdef AIOP
            unlock_spinlock(p_partition->lock);
#else /* not AIOP */
    	    spin_unlock_irqrestore(p_partition->lock, int_flags);
#endif /* AIOP */
            p_mem_mng->f_free(p_mem_mng_debug_entry);
            return 1;
        }
    }

#ifdef AIOP
    unlock_spinlock(p_partition->lock);
#else /* not AIOP */
    spin_unlock_irqrestore(p_partition->lock, int_flags);
#endif /* AIOP */

    return 0;
}


