/**************************************************************************//**
 @File          mem_mng.c

 @Description   Implementation of the memory allocation management module.
*//***************************************************************************/

#include "common/fsl_string.h"
#include "common/spinlock.h"
#include "common/slob.h"

#include "mem_mng.h"

__SHRAM uint8_t mem_lock;

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
    uint32_t            int_flags;

    if (p_mem_mng->lock)
    {
        int_flags = spin_lock_irqsave(p_mem_mng->lock);
        LIST_FOR_EACH_SAFE(p_partition_iterator,
                           p_tmp_iterator,
                           &(p_mem_mng->mem_partitions_list))
        {
            mem_mng_free_partition(p_mem_mng, p_partition_iterator);
        }
        spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
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
    uint32_t            int_flags;

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
    /* Return the next ID higher than any registered partition ID */
    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        available_id = p_partition->id + 1;
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

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
    uint32_t            int_flags;

    if ((f_user_malloc || f_user_free) && !(f_user_malloc && f_user_free))
    {
        RETURN_ERROR(MAJOR, E_INVALID_VALUE,
                     ("f_user_malloc and f_user_free must be both NULL or not NULL"));
    }

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
            RETURN_ERROR(MAJOR, E_ALREADY_EXISTS, ("partition ID %d", partition_id));
        }
        else if (p_partition->id > partition_id)
        {
            break;
        }
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

    p_new_partition = (t_mem_mng_partition *)p_mem_mng->f_malloc(sizeof(t_mem_mng_partition));
    if (!p_new_partition)
    {
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory manager partition"));
    }
    memset(p_new_partition, 0, sizeof(t_mem_mng_partition));
    p_new_partition->lock = spin_lock_create();
    if (!p_new_partition->lock)
    {
        p_mem_mng->f_free(p_new_partition);
        RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("spinlock object for partition: %s", name));
    }

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

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
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
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

    return E_OK;
}


/*****************************************************************************/
int mem_mng_unregister_partition(fsl_handle_t h_mem_mng, int partition_id)
{
    t_mem_mng            *p_mem_mng = (t_mem_mng *)h_mem_mng;
    t_mem_mng_partition   *p_partition;
    list_t              *p_partition_iterator, *p_tmp_iterator;
    uint32_t            int_flags;

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
    /* Find the requested partition and release it */
    LIST_FOR_EACH_SAFE(p_partition_iterator,
                       p_tmp_iterator,
                       &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
            mem_mng_free_partition(p_mem_mng, p_partition_iterator);
            return E_OK;
        }
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

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
    uint32_t            int_flags;

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
            *p_partition_info = p_partition->info;
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
            return E_OK;
        }
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

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
    uint32_t            int_flags;

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
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
        spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
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
                spin_lock(p_partition->lock);

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

                spin_unlock(p_partition->lock);
                spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
                return count;
            }
        }
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

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
    uint32_t            int_flags;

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

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
    /* Not early allocation - allocate from registered partitions */
    LIST_FOR_EACH_SAFE(p_partition_iterator, p_temp, &(p_mem_mng->mem_partitions_list))
    {
        p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if (p_partition->id == partition_id)
        {
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
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
            int_flags = spin_lock_irqsave(p_mem_mng->lock);

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

            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
            return p_memory;
        }
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

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
    uint32_t            int_flags;

    int_flags = spin_lock_irqsave(p_mem_mng->lock);
    LIST_FOR_EACH(p_partition_iterator, &(p_mem_mng->mem_partitions_list))
    {
        p_tmp_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

        if ((addr >= p_tmp_partition->info.base_address) &&
            (addr < (p_tmp_partition->info.base_address + p_tmp_partition->info.size)))
        {
            *p_partition_id = p_tmp_partition->id;
            *p_partition = p_tmp_partition;
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
            return 1;
        }
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);

    return 0;
}


/*****************************************************************************/
static void mem_mng_free_partition(t_mem_mng *p_mem_mng, list_t *p_partition_iterator)
{
    t_mem_mng_partition   *p_partition;
    t_mem_mng_debug_entry  *p_mem_mng_debug_entry;
    list_t              *p_debug_iterator, *p_tmp_iterator;
    uint32_t            int_flags;

    p_partition = MEM_MNG_PARTITION_OBJECT(p_partition_iterator);

    int_flags = spin_lock_irqsave(p_partition->lock);
    /* Release the debug entries list */
    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_partition->mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        list_del(p_debug_iterator);
        p_mem_mng->f_free(p_mem_mng_debug_entry);
    }
    spin_unlock_irqrestore(p_partition->lock, int_flags);

    if (!p_partition->f_user_malloc)
    {
        /* Release the memory manager object */
        slob_free(p_partition->h_mem_manager);
    }

    /* Remove from partitions list and free the allocated memory */
    list_del(p_partition_iterator);
    if (p_partition->lock)
        spin_lock_free(p_partition->lock);
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
    uint32_t            int_flags;

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
        lock_spinlock(&mem_lock);
        int_flags = spin_lock_irqsave(p_mem_mng->lock);
        list_add_to_tail(&p_mem_mng_debug_entry->node, &(p_mem_mng->early_mem_debug_list));
        spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
        unlock_spinlock(&mem_lock);
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
    uint32_t            int_flags;

    lock_spinlock(&mem_lock);
    int_flags = spin_lock_irqsave(p_mem_mng->lock);
    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_mem_mng->early_mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        if (p_mem_mng_debug_entry->p_memory == p_memory)
        {
            list_del(p_debug_iterator);
            spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
            unlock_spinlock(&mem_lock);
            p_mem_mng->f_free(p_mem_mng_debug_entry);
            return 1;
        }
    }
    spin_unlock_irqrestore(p_mem_mng->lock, int_flags);
    unlock_spinlock(&mem_lock);

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
    uint32_t            int_flags;

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
        lock_spinlock(&mem_lock);
        int_flags = spin_lock_irqsave(p_partition->lock);
        list_add_to_tail(&p_mem_mng_debug_entry->node, &(p_partition->mem_debug_list));
        spin_unlock_irqrestore(p_partition->lock, int_flags);
        unlock_spinlock(&mem_lock);

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
    uint32_t            int_flags;

    lock_spinlock(&mem_lock);
    int_flags = spin_lock_irqsave(p_partition->lock);

    LIST_FOR_EACH_SAFE(p_debug_iterator, p_tmp_iterator, &(p_partition->mem_debug_list))
    {
        p_mem_mng_debug_entry = MEM_MNG_DBG_OBJECT(p_debug_iterator);

        if (p_mem_mng_debug_entry->p_memory == p_memory)
        {
            p_partition->info.current_usage -= p_mem_mng_debug_entry->size;
            p_partition->info.total_deallocations += 1;

            list_del(p_debug_iterator);
            spin_unlock_irqrestore(p_partition->lock, int_flags);
            unlock_spinlock(&mem_lock);
            p_mem_mng->f_free(p_mem_mng_debug_entry);
            return 1;
        }
    }

    spin_unlock_irqrestore(p_partition->lock, int_flags);
    unlock_spinlock(&mem_lock);

    return 0;
}


