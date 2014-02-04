#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "common/spinlock.h"
#include "slab.h"
#include "virtual_pools.h"

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
    int error = 0;
    
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
     */
    
    error = vpool_create_pool(1, num_buffs, num_buffs, 0, NULL, slab);
    *slab = (((*slab) & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) | SLAB_HW_POOL_SET;

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
    int error = 0;
    
    UNUSED(prefix_size);
    UNUSED(postfix_size);
    UNUSED(alignment);
    UNUSED(address);
    UNUSED(flags);
    UNUSED(release_cb);
    UNUSED(buff_size);

    /*
     * Only HW SLAB is supported
     * TODO choose BMAN pool 
     * TODO Fill BMAN pool
     */
    
    error = vpool_create_pool(1, num_buffs, num_buffs, 0, NULL , slab);
    *slab = (((*slab) & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) | SLAB_HW_POOL_SET;
    
    return -ENOTSUP;    
}

/*****************************************************************************/
void slab_free(uint32_t slab)
{
    /* TODO free all buffers */
    
    if (SLAB_IS_HW_POOL(slab)) {
        if (vpool_release_pool(SLAB_VP_POOL_GET(slab)) != VIRTUAL_POOLS_SUCCESS) {
            
        }
    };              
}

/*****************************************************************************/
int slab_acquire(uint32_t slab, uint64_t *buff)
{
    if (vpool_allocate_buf(SLAB_VP_POOL_GET(slab), buff) != 0)
    {
        return -ENOMEM;            
    }
    return 0;
}

/*****************************************************************************/
int slab_release(uint32_t slab, uint64_t buff)
{
    if (vpool_release_buf(SLAB_VP_POOL_GET(slab), buff) != 0)
    {
        return -EFAULT;
    }
    return 0;    
}


