#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "common/spinlock.h"
#include "common/dbg.h"
#include "kernel/platform.h"
#include "slab.h"
#include "virtual_pools.h"
#include "../drivers/dplib/arch/accel/fdma.h"  /* TODO: need to place fdma_release_buffer() in separate .h file */
#include "io.h"
#include "fsl_cdma.h"

/* TODO need to read the ICID from somewhere */
#define SLAB_FDMA_ICID              0 /**< ICID to be used for FDMA release & acquire*/
#define SLAB_ASSERT_COND_RETURN(COND, ERR)  do { if (!(COND)) return (ERR); } while(0)

/*  TODO use API from VPs  */
extern struct virtual_pools_root_desc virtual_pools_root;
extern struct bman_pool_desc virtual_bman_pools[MAX_VIRTUAL_BMAN_POOLS_NUM];
/*****************************************************************************/
static void free_buffs_from_bman_pool(uint16_t bpid, int num_buffs)
{
        int      i;
        uint64_t addr = 0;

        for (i = 0; i < num_buffs; i++) {
                fdma_acquire_buffer(SLAB_FDMA_ICID, FDMA_ACQUIRE_NO_FLAGS, bpid, &addr);
                addr = (uint64_t)fsl_os_phys_to_virt(addr);
                fsl_os_xfree((void *)addr);
        }

}

/*****************************************************************************/
static inline int find_bpid(uint16_t buff_size,
                            uint16_t alignment,
                            uint8_t  mem_pid,
                            struct   slab_module_info *slab_module,
                            uint16_t *bpid,
                            uint16_t *alloc_buff_size,
                            uint16_t *alloc_alignment)
{
        int     i = 0, temp = 0, found = 0;
        int     num_bpids = slab_module->num_hw_pools;
        struct  slab_hw_pool_info *hw_pools = slab_module->hw_pools;

        /*
         * TODO access DDR with CDMA !!!!!
         */
        for(i = 0; i < num_bpids; i++) {
                if ((hw_pools[i].mem_pid             == mem_pid) &&
                        (hw_pools[i].alignment >= alignment) &&
                        (SLAB_SIZE_GET(hw_pools[i].buff_size) >= buff_size)) {

                        if (!found) {
                                /* Keep the first found */
                                temp  = i;
                                found = 1;
                        } else if (hw_pools[temp].buff_size > hw_pools[i].buff_size) {
                                /* Choose smallest possible size */
                                temp = i;
                        }
                }
        }

        /* Verify that we really found a legal bpid */
        if (found) {
                *bpid            = hw_pools[temp].pool_id;
                *alloc_buff_size = hw_pools[temp].buff_size; /* size for malloc */
                *alloc_alignment = hw_pools[temp].alignment; /* alignment for malloc */
                return 0;
        }

        return -ENAVAIL;
}

/*****************************************************************************/
int slab_find_and_fill_bpid(uint16_t num_buffs,
                            uint16_t buff_size,
                            uint16_t alignment,
                            uint8_t  mem_pid,
                            int      *num_filled_buffs,
                            uint16_t *bpid)
{
        int        error = 0, i = 0;
        dma_addr_t addr  = 0;
        uint16_t   new_buff_size = 0;
        uint16_t   new_alignment = 0;

        struct slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 0);

        error = find_bpid(buff_size, alignment, mem_pid, slab_module, bpid, &new_buff_size, &new_alignment);
        SLAB_ASSERT_COND_RETURN(error == 0, error);

        /*
         * It's an easy implementation
         * TODO icid != 0 for fdma_release_buffer  ??
         */
        *num_filled_buffs = 0;
        for (i = 0; i < num_buffs; i++) {

                addr = (dma_addr_t)fsl_os_xmalloc(new_buff_size, mem_pid, new_alignment);
                if (addr == NULL) {
                        /* Free buffs that we already filled */
                        free_buffs_from_bman_pool(*bpid, i);
                        return -ENOMEM;
                }
                addr = fsl_os_virt_to_phys((void *)addr);

                /* Isolation is enabled */
                if (fdma_release_buffer(SLAB_FDMA_ICID, FDMA_RELEASE_NO_FLAGS, *bpid, addr)) {
                        fsl_os_xfree(fsl_os_phys_to_virt(addr));
                        /* Free buffs that we already filled */
                        free_buffs_from_bman_pool(*bpid, i);
                        return -ENAVAIL;
                }
        }

        *num_filled_buffs = num_buffs;
        vpool_add_total_bman_bufs(*bpid, num_buffs);

        return 0;
}

/*****************************************************************************/
static void free_slab_module_memory()
{
        struct slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 0);

        /* TODO there still some static allocations in VP init
         * need to add them to slab_module_init() and then free them here
         */
        fsl_os_xfree(slab_module->virtual_pool_struct);
        fsl_os_xfree(slab_module->callback_func_struct);
        fsl_os_xfree(slab_module->hw_pools);
        fsl_os_xfree(slab_module);
}

/*****************************************************************************/
static inline int sanity_check_slab_create(uint16_t    num_buffs,
                                           uint16_t    buff_size,
                                           uint16_t    alignment,
                                           uint8_t     mem_pid,
                                           uint32_t    flags)
{
        SLAB_ASSERT_COND_RETURN(num_buffs > 0,   -EINVAL);
        SLAB_ASSERT_COND_RETURN(buff_size > 0,   -EINVAL);
        SLAB_ASSERT_COND_RETURN(alignment > 0,   -EINVAL);
        SLAB_ASSERT_COND_RETURN(alignment <= 8,  -EINVAL); /* TODO need to support more then 8 ?*/
        SLAB_ASSERT_COND_RETURN(flags == 0,      -EINVAL);

        SLAB_ASSERT_COND_RETURN(is_power_of_2(alignment), -EINVAL);
        SLAB_ASSERT_COND_RETURN(((mem_pid == MEM_PART_1ST_DDR_NON_CACHEABLE) ||
                (mem_pid == MEM_PART_PEB)), -EINVAL);
        return 0;
}

/*****************************************************************************/
int slab_create(uint16_t    num_buffs,
                uint16_t    extra_buffs,
                uint16_t    buff_size,
                uint16_t    prefix_size,
                uint16_t    postfix_size,
                uint16_t    alignment,
                uint8_t     mem_pid,
                uint32_t    flags,
                slab_release_cb_t release_cb,
                struct slab **slab)
{
        int        error = 0;
        dma_addr_t addr  = 0;
        uint32_t   data  = 0;
        uint16_t   bpid  = 0;

        UNUSED(prefix_size);
        UNUSED(postfix_size);

#ifdef DEBUG
        /* Sanity checks */
        error = sanity_check_slab_create(num_buffs, buff_size, alignment, mem_pid, flags);
        if (error)           return -ENAVAIL;
        if (extra_buffs > 0) return -ENAVAIL; /* TODO remove it when extra_buffs are supported */
#endif

        *((uint32_t *)slab) = 0;
        /*
         * Only HW SLAB is supported
         */
        error = slab_find_and_fill_bpid(num_buffs, buff_size, alignment, mem_pid, (int *)(&data), &bpid);
        if (error) return error; /* -EINVAL or -ENOMEM */

        data  = 0;
        error = vpool_create_pool(bpid, num_buffs + extra_buffs, num_buffs, 0, release_cb , &data);
        if (error)
                return -ENAVAIL;
        if (data > SLAB_VP_POOL_MAX) {
                vpool_release_pool(data);
                return -ENAVAIL;
        }

        *((uint32_t *)slab) = ((data & (SLAB_VP_POOL_MASK >> SLAB_VP_POOL_SHIFT)) << SLAB_VP_POOL_SHIFT) | SLAB_HW_POOL_SET;

        return 0;
}

/*  TODO use API from VPs  */
/*****************************************************************************/
int slab_free(struct slab *slab)
{
        struct   slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 0);
        int      remaining_buffs = (int)((struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct + SLAB_VP_POOL_GET(slab))->committed_bufs;
        uint16_t bpid = (uint16_t)virtual_bman_pools[((struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct + SLAB_VP_POOL_GET(slab))->bman_array_index].bman_pool_id;

        /* TODO Use VP API for BPID and remaining buffers */

        if (SLAB_IS_HW_POOL(slab)) {
                if (vpool_release_pool(SLAB_VP_POOL_GET(slab)) != VIRTUAL_POOLS_SUCCESS) {
                        return -EBUSY;
                } else {
                        /* TODO use VP API to update VP BPID !! */
                        vpool_decr_total_bman_bufs(bpid, remaining_buffs);
                        /* Free all the remaining buffers for VP */
                        free_buffs_from_bman_pool(bpid, remaining_buffs);
                }
        } else {
                return -EINVAL;
        }
        return 0;
}

/*****************************************************************************/
int slab_acquire(struct slab *slab, uint64_t *buff)
{

#ifdef DEBUG
        SLAB_ASSERT_COND_RETURN(SLAB_IS_HW_POOL(slab), -EINVAL);
#endif

        if (vpool_allocate_buf(SLAB_VP_POOL_GET(slab), buff))
        {
                return -ENOMEM;
        }
        return 0;
}

/*****************************************************************************/
int slab_release(struct slab *slab, uint64_t buff)
{

#ifdef DEBUG
        SLAB_ASSERT_COND_RETURN(SLAB_IS_HW_POOL(slab), -EINVAL);
#endif
        if (vpool_refcount_decrement_and_release(SLAB_VP_POOL_GET(slab), buff, NULL))
        {
                return -EFAULT;
        }
        return 0;
}
/*****************************************************************************/
static int bpid_init(struct slab_hw_pool_info *hw_pools,
                      struct slab_bpid_info bpid) {

        hw_pools->pool_id          = bpid.bpid;
        hw_pools->alignment        = SLAB_DEFAULT_ALIGN;
        hw_pools->flags            = 0;
        hw_pools->mem_pid          = bpid.mem_pid;
        hw_pools->buff_size        = SLAB_SIZE_SET(bpid.size);
        /* TODO check which buff_size I should pass to VP with or without metadata
         * TODO buff_size should be removed one day*/
        return vpool_init_total_bman_bufs(bpid.bpid,
                                          0,
                                          (uint32_t)bpid.size);
}

/*****************************************************************************/
int slab_module_init(void)
{
        /* TODO Call MC to get all BPID per partition */
        struct   slab_bpid_info bpids_arr[] = SLAB_BPIDS_ARR;

        	//SLAB_BPIDS_ARR;
        int      num_bpids = ARRAY_SIZE(bpids_arr);
        struct   slab_module_info *slab_module = NULL;
        int      i = 0;
        int      error = 0;

        slab_module = fsl_os_xmalloc(sizeof(struct slab_module_info),
                                     SLAB_FAST_MEMORY, 1);

        slab_module->num_hw_pools = (uint8_t)(num_bpids & 0xFF);
        slab_module->hw_pools     = fsl_os_xmalloc(sizeof(struct slab_hw_pool_info) * num_bpids, SLAB_DDR_MEMORY, 1);

        slab_module->virtual_pool_struct  = fsl_os_xmalloc((sizeof(struct virtual_pool_desc) * SLAB_MAX_NUM_VP), SLAB_FAST_MEMORY, 1);
        slab_module->callback_func_struct = fsl_os_xmalloc((sizeof(struct callback_s) * SLAB_MAX_NUM_VP), SLAB_FAST_MEMORY, 1);

        /* TODO vpool_init() API will change to get more allocated by malloc() memories */
        error = vpool_init((uint64_t)(slab_module->virtual_pool_struct), (uint64_t)(slab_module->callback_func_struct), SLAB_MAX_NUM_VP, 0);
        if (error) {
                free_slab_module_memory();
                return -ENAVAIL;
        }

        /* Set BPIDs */
        while (i < num_bpids) {
                error = bpid_init(&(slab_module->hw_pools[i]), bpids_arr[i]);
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

/*****************************************************************************/
int slab_debug_info_get(struct slab *slab, struct slab_debug_info *slab_info)
{
        int32_t temp = 0, max_buffs = 0, num_buffs = 0;
        int     i;
        struct slab_module_info *slab_module = sys_get_handle(FSL_OS_MOD_SLAB, 0);

        if (slab_info != NULL) {
                if (vpool_read_pool(SLAB_VP_POOL_GET(slab),
                                    &slab_info->pool_id,
                                    &temp,
                                    &max_buffs,
                                    &num_buffs,
                                    (uint32_t *)&temp,
                                    &temp) == 0) {
                        /* Modify num_buffs to have the number of available buffers not allocated */
                        slab_info->num_buffs = (uint16_t)(max_buffs - num_buffs);
                        slab_info->max_buffs = (uint16_t)max_buffs;

                        temp = slab_module->num_hw_pools;
                        for (i = 0; i < temp; i++) {
                                if (slab_module->hw_pools[i].pool_id == slab_info->pool_id) {
                                        slab_info->buff_size  = slab_module->hw_pools[i].buff_size;
                                        slab_info->alignment  = slab_module->hw_pools[i].alignment;
                                        slab_info->mem_pid    = slab_module->hw_pools[i].mem_pid;
                                        return 0;
                                } /* if */
                        } /* for */
                }
        }

        return -EINVAL;
}

/*****************************************************************************/
int slab_refcount_incr(struct slab *slab, uint64_t buff)
{
#ifdef DEBUG
	SLAB_ASSERT_COND_RETURN(SLAB_IS_HW_POOL(slab), -EINVAL);
#endif
	if (cdma_refcount_increment(buff)) {
		return -EFAULT;
	}
	return 0;
}

/*****************************************************************************/
int slab_refcount_decr(struct slab *slab, uint64_t buff)
{
	return slab_release(slab, buff);
}
