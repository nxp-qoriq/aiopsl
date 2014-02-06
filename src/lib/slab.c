#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "common/spinlock.h"
#include "common/dbg.h"
#include "kernel/platform.h"
#include "slab.h"
#include "virtual_pools.h"
#include "fsl_fdma.h"
#include "io.h"
#include "inc/mem_mng_util.h"
#include "sys.h"

#define SLAB_BPIDS_PARTITION0       {1, 2, 3, 4, 5}
#define SLAB_MODULE_FAST_MEMORY     MEM_PART_SH_RAM
#define SLAB_MODULE_DDR_MEMORY      MEM_PART_1ST_DDR_NON_CACHEABLE
#define SLAB_DEFAULT_BUFF_SIZE      256
#define SLAB_DEFAULT_BUFF_ALIGN     8
#define SLAB_MAX_NUM_VP             1000
#define SLAB_ASSERT_COND_RETURN(COND, ERR)  do { if (!(COND)) return (ERR); } while(0)

/* Global System Object */
extern t_system sys;

/*****************************************************************************/
int static find_bpid(uint16_t *bpid, 
                     uint16_t buff_size, 
                     uint16_t alignment, 
                     uint8_t  mem_partition_id,
                     struct   slab_module_info *slab_module)
{
    int     i = 0, temp = 0;
    int     num_bpids = slab_module->num_hw_pools;
    struct  slab_hw_pool_info *hw_pools = slab_module->hw_pools;
    
    for(i = 0; i < num_bpids; i++) {
        if ((hw_pools[i].mem_partition_id == mem_partition_id) &&
            (hw_pools[i].alignment        >= alignment)        &&
            (hw_pools[i].buff_size        >= buff_size)) {
            
            if (hw_pools[temp].buff_size > hw_pools[i].buff_size) {
                /* Choose smallest possible size */
                temp = i;
            }
        }
    }
    
    /* Verify that we really found a legal bpid */
    if ((hw_pools[temp].mem_partition_id == mem_partition_id) &&
        (hw_pools[temp].alignment        >= alignment)        &&
        (hw_pools[temp].buff_size        >= buff_size)) {
        
        *bpid = temp;
        return 0;
    }

    return -ENAVAIL;
}

/*****************************************************************************/
int static find_and_fill_bpid(uint16_t *bpid, 
                              uint16_t num_buffs, 
                              uint16_t buff_size, 
                              uint16_t alignment, 
                              uint8_t  mem_partition_id,
                              struct   slab_module_info *slab_module,
                              uint32_t *num_filled_buffs)
{    
    int        error = 0, i = 0;
    dma_addr_t addr  = 0;
    uint16_t   icid  = 0;
    
    error = find_bpid(bpid, buff_size, alignment, mem_partition_id, slab_module);
    SLAB_ASSERT_COND_RETURN(error == 0, error);
    
    /*
     * It's an easy implementation
     * TODO improve it to support consecutive 
     * TODO spinlocks for fdma release ??
     * TODO icid
     */
    for (i = 0; i < num_buffs; i++) {
        
        addr = fsl_os_virt_to_phys(fsl_os_xmalloc(buff_size, mem_partition_id, alignment));  
        
        /* Isolation is enabled */
        if (fdma_release_buffer(icid, FDMA_RELEASE_NO_FLAGS, *bpid, addr)) {
            fsl_os_xfree(fsl_os_phys_to_virt(addr));
            *num_filled_buffs = i + 1;
            /* Do something with the buffers that were released
             * Notify VP about it */
            vpool_add_total_bman_bufs(*bpid, *num_filled_buffs);
            return -ENAVAIL;
        }
    }
    
    *num_filled_buffs = num_buffs;
    vpool_add_total_bman_bufs(*bpid, num_buffs);

    return 0;
}

/*****************************************************************************/
int static find_and_fill_bpid_by_address(uint16_t *bpid, 
                                         uint16_t num_buffs, 
                                         uint16_t buff_size, 
                                         uint16_t alignment, 
                                         uint8_t  *address,
                                         struct   slab_module_info *slab_module,
                                         int      *num_filled_buffs)
{    
    int        error = 0, i = 0;
    dma_addr_t addr  = 0;
    uint16_t   icid  = 0;
    uint8_t    *address_base = address;
    
    error = mem_mng_get_partition_id_by_addr(sys.mem_mng, (uint64_t)address, &i);
    error = find_bpid(bpid, buff_size, alignment, (uint8_t)i, slab_module);
    if (error) return error;
    
    /*
     * It's an easy implementation
     * TODO spinlocks for fdma release ??
     * TODO icid
     * TODO think about a flow where VP might already have enough buffers inside BPID 
     *      therefore no need to fill it ? Why do you notify VP vpool_add_total_bman_bufs ?
     */
    for (i = 0; i < num_buffs; i++) {
        
        address_base = (uint8_t *)ALIGN_UP(address_base, alignment);
        addr = fsl_os_virt_to_phys(address_base);  
        
        /* Isolation is enabled */
        if (fdma_release_buffer(icid, FDMA_RELEASE_NO_FLAGS, *bpid, addr)) {            
            *num_filled_buffs = i + 1;
            /* Don't free buffer because it's user allocation 
             * Do something with the buffers that were released
             * Notify VP about it */
            vpool_add_total_bman_bufs(*bpid, *num_filled_buffs);
            return -ENAVAIL;
        }
        address_base += buff_size;
    }
    
    *num_filled_buffs = num_buffs;
    vpool_add_total_bman_bufs(*bpid, num_buffs);

    return 0;
}

/*****************************************************************************/
static void free_slab_module_memory() 
{
    struct slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 1, 0);

    fsl_os_xfree(slab_module->virtual_pool_struct);
    fsl_os_xfree(slab_module->callback_func_struct);
    fsl_os_xfree(slab_module->hw_pools);
    fsl_os_xfree(slab_module);    
}

/*****************************************************************************/
static int sanity_check_slab_create(uint16_t    num_buffs,
                                    uint16_t    buff_size,
                                    uint16_t    alignment,
                                    uint8_t     mem_partition_id,
                                    uint8_t     *address,
                                    uint32_t    flags)
{
    SLAB_ASSERT_COND_RETURN(num_buffs > 0,   -EINVAL);
    SLAB_ASSERT_COND_RETURN(buff_size > 0,   -EINVAL);
    SLAB_ASSERT_COND_RETURN(alignment > 0,   -EINVAL);
    SLAB_ASSERT_COND_RETURN(flags == 0,      -EINVAL);
    SLAB_ASSERT_COND_RETURN(address != NULL, -EINVAL);
    
    SLAB_ASSERT_COND_RETURN(is_power_of_2(alignment), -EINVAL);
    SLAB_ASSERT_COND_RETURN(((mem_partition_id == MEM_PART_1ST_DDR_NON_CACHEABLE) || 
                       (mem_partition_id == MEM_PART_PEB)), -EINVAL);    
    return 0;
}

/*****************************************************************************/
int slab_create(uint16_t    num_buffs,
                uint16_t    buff_size,
                uint16_t    prefix_size,
                uint16_t    postfix_size,
                uint16_t    alignment,
                uint8_t     mem_partition_id,
                uint32_t    flags,
                slab_release_cb_t release_cb,
                uint32_t    *slab)
{
    struct slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 1, 0);

    int        error = 0;
    dma_addr_t addr  = 0;
    uint32_t   data  = 0;
    uint16_t   bpid  = 0;

    UNUSED(prefix_size);
    UNUSED(postfix_size);
    
    /* Sanity checks 
     * TODO place sanity check under the right debug macro */
    error = sanity_check_slab_create(num_buffs, buff_size, alignment, mem_partition_id, (uint8_t *)0x100/* dummy*/, flags);
    if (error) return -ENAVAIL;

    *slab = 0;
    /*
     * Only HW SLAB is supported
     */
    error = find_and_fill_bpid(&bpid, num_buffs, buff_size, alignment, mem_partition_id, slab_module, &data);
    if (error) return -ENAVAIL;
    
    data  = 0;
    error = vpool_create_pool(bpid, num_buffs, num_buffs, 0, release_cb , &data);
    if (error) 
        return -ENAVAIL;
    if (data > SLAB_VP_POOL_MAX) { 
        vpool_release_pool(data);
        return -ENAVAIL;
    }
      
    *slab = ((data & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) | SLAB_HW_POOL_SET;
    
    return 0;
}

/*****************************************************************************/
int slab_create_by_address(uint16_t num_buffs,
                           uint16_t buff_size,
                           uint16_t prefix_size,
                           uint16_t postfix_size,
                           uint16_t alignment,
                           uint8_t  *address,
                           uint32_t flags,
                           slab_release_cb_t release_cb,
                           uint32_t *slab)
{
    struct slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 1, 0);
    
    int      error = 0;
    uint32_t data  = 0;    
    uint16_t bpid  = 0;
    
    UNUSED(prefix_size);
    UNUSED(postfix_size);

    /* Sanity checks 
     * TODO place sanity check under the right debug macro */
    error = sanity_check_slab_create(num_buffs, buff_size, alignment, MEM_PART_PEB/* dummy*/, address, flags);
    if (error) return -ENAVAIL;

    *slab = 0;
    /*
     * Only HW SLAB is supported
     */
    error = find_and_fill_bpid_by_address(&bpid, num_buffs, buff_size, alignment, address, slab_module, (int *)(&data));
    if (error) return -ENAVAIL;
    
    data  = 0;
    error = vpool_create_pool(bpid, num_buffs, num_buffs, 0, release_cb , &data);
    if (error) 
        return -ENAVAIL;
    if (data > SLAB_VP_POOL_MAX) { 
        vpool_release_pool(data);
        return -ENAVAIL;
    }
      
    *slab = ((data & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) | SLAB_HW_POOL_SET;
    
    return 0;    
}

/*****************************************************************************/
void slab_free(uint32_t slab)
{
    struct slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 1, 0);
    
    /* TODO free all buffers ?? */
    
    if (SLAB_IS_HW_POOL(slab)) {
        if (vpool_release_pool(SLAB_VP_POOL_GET(slab)) != VIRTUAL_POOLS_SUCCESS) {
            pr_err("Failed to release HW pool %d", SLAB_VP_POOL_GET(slab));
        }
        
    };              
}

/*****************************************************************************/
int slab_acquire(uint32_t slab, uint64_t *buff)
{
    if (vpool_allocate_buf(SLAB_VP_POOL_GET(slab), buff))
    {
        return -ENOMEM;            
    }
    return 0;
}

/*****************************************************************************/
int slab_release(uint32_t slab, uint64_t buff)
{
    if (vpool_release_buf(SLAB_VP_POOL_GET(slab), buff))
    {
        return -EFAULT;
    }
    return 0;    
}

/*****************************************************************************/
int slab_module_init(void)
{    
    uint16_t bpids_arr[] = SLAB_BPIDS_PARTITION0;    /* TODO Call MC to get all BPID per partition */
    int      num_bpids = (sizeof(bpids_arr) / sizeof(uint16_t));
    struct   slab_module_info *slab_module = NULL;
    int      i = 0;
    int      error = 0;

    slab_module = fsl_os_xmalloc(sizeof(struct slab_module_info), SLAB_MODULE_FAST_MEMORY, 1);
    
    slab_module->num_hw_pools = (uint8_t)(num_bpids & 0xFF);
    slab_module->hw_pools     = fsl_os_xmalloc(sizeof(struct slab_hw_pool_info) * num_bpids, SLAB_MODULE_DDR_MEMORY, 1);
    
    slab_module->virtual_pool_struct  = fsl_os_xmalloc((sizeof(struct virtual_pool_desc) * SLAB_MAX_NUM_VP), SLAB_MODULE_FAST_MEMORY, 1);
    slab_module->callback_func_struct = fsl_os_xmalloc((sizeof(struct callback_s) * SLAB_MAX_NUM_VP), SLAB_MODULE_FAST_MEMORY, 1);
    
    error = vpool_init((uint64_t)(slab_module->virtual_pool_struct), (uint64_t)(slab_module->callback_func_struct), SLAB_MAX_NUM_VP, 0);
    if (error) { 
        free_slab_module_memory();
        return -ENAVAIL;
    }

    while (i < num_bpids) {
        
        slab_module->hw_pools[i].pool_id          = bpids_arr[i];
        slab_module->hw_pools[i].alignment        = SLAB_DEFAULT_BUFF_ALIGN;
        slab_module->hw_pools[i].buff_size        = SLAB_DEFAULT_BUFF_SIZE;
        slab_module->hw_pools[i].flags            = 0;
        slab_module->hw_pools[i].mem_partition_id = SLAB_MODULE_DDR_MEMORY;
        
        error = vpool_init_total_bman_bufs(bpids_arr[i], 0, SLAB_DEFAULT_BUFF_SIZE); 
        if (error) {
            free_slab_module_memory();
            return -ENAVAIL;
        }
        
        i++;
    }
    
    /* Add to all system handles */
    error = sys_add_handle(slab_module, FSL_OS_MOD_SLAB, 1, 0);
    return error;        
}

/*****************************************************************************/
void slab_module_free(void)
{
    free_slab_module_memory();
    sys_remove_handle(FSL_OS_MOD_SLAB, 0);
}
