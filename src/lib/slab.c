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

#define SLAB_BPIDS_PARTITION0       {1, 2, 3, 4, 5}
#define SLAB_MODULE_FAST_MEMORY     MEM_PART_SH_RAM
#define SLAB_MODULE_DDR_MEMORY      MEM_PART_1ST_DDR_NON_CACHEABLE
#define SLAB_DEFAULT_BUFF_SIZE      256
#define SLAB_MAX_NUM_VP             1000

/*****************************************************************************/
int static find_bpid(uint16_t *bpid, 
                     uint16_t buff_size, 
                     uint16_t alignment, 
                     uint8_t  mem_partition_id)
{
    UNUSED(bpid);
    UNUSED(alignment);
    UNUSED(mem_partition_id);
    UNUSED(buff_size);

    return -ENOTSUP;
}

/*****************************************************************************/
int static find_and_fill_bpid(uint16_t *bpid, 
                              uint16_t num_buffs, 
                              uint16_t buff_size, 
                              uint16_t alignment, 
                              uint8_t  mem_partition_id)
{
    UNUSED(bpid);
    UNUSED(num_buffs);
    UNUSED(alignment);
    UNUSED(mem_partition_id);
    UNUSED(buff_size);

    return -ENOTSUP;
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
    int        error = 0;
    dma_addr_t addr  = 0;
    uint32_t   data  = 0;

    UNUSED(prefix_size);
    UNUSED(postfix_size);
    UNUSED(alignment);
    UNUSED(mem_partition_id);
    UNUSED(flags);
    UNUSED(release_cb);
    UNUSED(buff_size);

    /*
     * Only HW SLAB is supported
     * TODO choose BMAN pool 
     * TODO Fill BMAN pool
     * TODO read icid
     */
    addr = fsl_os_virt_to_phys(fsl_os_xmalloc(buff_size, mem_partition_id, alignment));    
    error = fdma_release_buffer(0, 0, 1, NULL);
    error = vpool_add_total_bman_bufs(1, num_buffs);
    error = vpool_create_pool(1, num_buffs, num_buffs, 0, NULL , &data);
    if (error) 
        return -ENAVAIL;
    if (data > SLAB_VP_POOL_MAX) { 
        vpool_release_pool(data);
        return -ENAVAIL;
    }
    *slab = (((data) & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) | SLAB_HW_POOL_SET;

    return -ENOTSUP;
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
    int      error = 0;
    uint32_t data  = 0;
    
    UNUSED(prefix_size);
    UNUSED(postfix_size);
    UNUSED(alignment);
    UNUSED(address);
    UNUSED(flags);
    UNUSED(release_cb);
    UNUSED(buff_size);

    *slab = 0;
    /*
     * Only HW SLAB is supported
     * TODO choose BMAN pool 
     * TODO Fill BMAN pool
     */
    
    error = vpool_create_pool(1, num_buffs, num_buffs, 0, NULL , &data);
    if (error) 
        return -ENAVAIL;
    if (data > SLAB_VP_POOL_MAX) { 
        vpool_release_pool(data);
        return -ENAVAIL;
    }
      

    *slab = (((data) & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) | SLAB_HW_POOL_SET;
    
    return -ENOTSUP;    
}

/*****************************************************************************/
void slab_free(uint32_t slab)
{
    /* TODO free all buffers */
    
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
        slab_module->hw_pools[i].alignment        = 8;
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
