#include "common/types.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "kernel/fsl_spinlock.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpbp.h"
#include "fsl_dbg.h"
#include "slab.h"
#include "virtual_pools.h"
#include "fdma.h"
#include "fsl_io.h"
#include "cdma.h"
#include "fsl_io_ccsr.h"
#include "aiop_common.h"

#define SLAB_ASSERT_COND_RETURN(COND, ERR)  \
	do { if (!(COND)) return (ERR); } while (0)

#define FOUND_SMALLER_SIZE(A, B) \
	hw_pools[(A)].buff_size > hw_pools[(B)].buff_size

/*  TODO use API from VPs when it will be added */
#define VP_DESC_ARR \
	((struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct)

#define VP_REMAINING_BUFFS(SLAB) \
	(int)((VP_DESC_ARR + SLAB_VP_POOL_GET((SLAB)))->committed_bufs)

#define VP_BPID_GET(SLAB) \
	(uint16_t)virtual_bman_pools[(VP_DESC_ARR + \
		SLAB_VP_POOL_GET((SLAB)))->bman_array_index].bman_pool_id

#define CP_POOL_DATA(MOD, INFO, I) \
	{                                                              \
		if (MOD->hw_pools[I].pool_id == INFO->pool_id) {       \
			INFO->buff_size  = MOD->hw_pools[I].buff_size; \
			INFO->alignment  = MOD->hw_pools[I].alignment; \
			INFO->mem_pid    = MOD->hw_pools[I].mem_pid;   \
			return 0;                                      \
		}                                                      \
	}

/*  TODO use API from VPs, this is temporal  */
extern struct virtual_pools_root_desc virtual_pools_root;
extern struct bman_pool_desc virtual_bman_pools[MAX_VIRTUAL_BMAN_POOLS_NUM];
/*****************************************************************************/
static void free_buffs_from_bman_pool(uint16_t bpid, int num_buffs,
                                      uint16_t icid, uint32_t flags)
{
	int      i;
	uint64_t addr = 0;


	for (i = 0; i < num_buffs; i++) {
		fdma_acquire_buffer(icid, flags, bpid, &addr);
	}
}

/*****************************************************************************/
static inline int find_bpid(uint16_t buff_size,
                            uint16_t alignment,
                            uint8_t  mem_pid,
                            struct   slab_module_info *slab_m,
                            uint16_t *bpid)
{
	int     i = 0, temp = 0, found = 0;
	int     num_bpids = slab_m->num_hw_pools;
	struct  slab_hw_pool_info *hw_pools = slab_m->hw_pools;

	/*
	 * TODO access DDR with CDMA ???
	 * It's init time but maybe it's important for restart
	 */
	for (i = 0; i < num_bpids; i++) {
		if ((hw_pools[i].mem_pid == mem_pid)         &&
			(hw_pools[i].alignment >= alignment) &&
			(SLAB_SIZE_GET(hw_pools[i].buff_size) >= buff_size)) {

			if (!found) {
				/* Keep the first found */
				temp  = i;
				found = 1;
			} else if (FOUND_SMALLER_SIZE(temp, i)) {
				temp = i;
			}
		}
	}

	/* Verify that we really found a legal bpid */
	if (found) {
		*bpid            = hw_pools[temp].pool_id;
		return 0;
	}

	return -ENAVAIL;
}

/**
 * TODO
 * int slab_find_and_free_bpid(int num_buffs,
                               uint16_t *bpid)
 * USE increment "vpool_incr_bman_bufs"
 */

/*****************************************************************************/
int slab_find_and_reserve_bpid(int num_buffs,
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

	struct slab_module_info *slab_m = \
		sys_get_unique_handle(FSL_OS_MOD_SLAB);

	if (slab_m == NULL)
		return -EINVAL;

	error = find_bpid(buff_size,
	                  alignment,
	                  mem_pid,
	                  slab_m,
	                  bpid);
	SLAB_ASSERT_COND_RETURN(error == 0, error);

	error = vpool_decr_total_bman_bufs(*bpid,num_buffs);

	if(error){
		return -ENOMEM;
	}
	*num_filled_buffs = (int)num_buffs;


	return 0;
}

/*****************************************************************************/
static void free_slab_module_memory(struct slab_module_info *slab_m)
{
	/* TODO there still some static allocations in VP init
	 * need to add them to slab_module_init() and then free them here
	 */
	if (slab_m->virtual_pool_struct)
		fsl_os_xfree(slab_m->virtual_pool_struct);
	if (slab_m->callback_func_struct)
		fsl_os_xfree(slab_m->callback_func_struct);
	if (slab_m->hw_pools)
		fsl_os_xfree(slab_m->hw_pools);
	fsl_os_xfree(slab_m);
}

/*****************************************************************************/
static inline int sanity_check_slab_create(uint32_t    num_buffs,
                                           uint16_t    buff_size,
                                           uint16_t    alignment,
                                           uint8_t     mem_pid,
                                           uint32_t    flags)
{
	SLAB_ASSERT_COND_RETURN(num_buffs > 0,   -EINVAL);
	SLAB_ASSERT_COND_RETURN(buff_size > 0,   -EINVAL);
	SLAB_ASSERT_COND_RETURN(alignment > 0,   -EINVAL);
	/* TODO need to support more then 8, align all to 64 bytes */
	SLAB_ASSERT_COND_RETURN(alignment <= 8,  -EINVAL);
	SLAB_ASSERT_COND_RETURN(flags == 0,      -EINVAL);

	SLAB_ASSERT_COND_RETURN(is_power_of_2(alignment), -EINVAL);
	SLAB_ASSERT_COND_RETURN(((mem_pid == MEM_PART_DP_DDR) ||
		(mem_pid == MEM_PART_PEB)), -EINVAL);
	return 0;
}

/*****************************************************************************/
int slab_create(uint32_t    num_buffs,
                uint32_t    max_buffs,
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

	struct slab_module_info *slab_m = \
		sys_get_unique_handle(FSL_OS_MOD_SLAB);

	UNUSED(prefix_size);
	UNUSED(postfix_size);

#ifdef DEBUG
	/* Sanity checks */
	error = sanity_check_slab_create(num_buffs,
	                                 buff_size,
	                                 alignment,
	                                 mem_pid,
	                                 flags);
	if (error)
		return -ENAVAIL;
	/* TODO remove it when max_buffs are supported */
	if (max_buffs > num_buffs)
		return -ENAVAIL;
#endif

	*((uint32_t *)slab) = 0;
	/*
	 * Only HW SLAB is supported
	 */

	if (slab_m == NULL)
		return -EINVAL;

	error = find_bpid(buff_size,
	                  alignment,
	                  mem_pid,
	                  slab_m,
	                  &bpid);
	SLAB_ASSERT_COND_RETURN(error == 0, error);

	data  = 0;
	/* TODO add max_buffs to vpool_create_pool when it will be supported */
	error = vpool_create_pool(bpid,
	                          (int32_t)num_buffs,
	                          (int32_t)num_buffs,
	                          0,
	                          release_cb,
	                          &data);
	if (error)
		return -ENAVAIL;
	if (data > SLAB_VP_POOL_MAX) {
		vpool_release_pool(data);
		return -ENAVAIL;
	}

	*((uint32_t *)slab) = SLAB_HW_POOL_CREATE(data);

	return 0;
}

/*****************************************************************************/
int slab_free(struct slab **slab)
{
	/* TODO Use VP API for BPID and remaining buffers */
	int      remaining_buffs = VP_REMAINING_BUFFS(*slab);
	uint16_t bpid = VP_BPID_GET(*slab);
	if (SLAB_IS_HW_POOL(*slab)) {
		if (vpool_release_pool(SLAB_VP_POOL_GET(*slab)) != 0)
			return -EBUSY;
	} else {
		return -EINVAL;
	}

	*((uint32_t *)slab) = 0; /**< Delete all pool information */
	return 0;
}

/*****************************************************************************/
__HOT_CODE int slab_acquire(struct slab *slab, uint64_t *buff)
{

#ifdef DEBUG
	SLAB_ASSERT_COND_RETURN(SLAB_IS_HW_POOL(slab), -EINVAL);
#endif

	if (vpool_allocate_buf(SLAB_VP_POOL_GET(slab), buff))
		return -ENOMEM;

	return 0;
}

/*****************************************************************************/
/* Must be used only in DEBUG
 * Accessing DDR in runtime also fsl_os_phys_to_virt() is not optimized */
static int slab_check_bpid(struct slab *slab, uint64_t buff)
{
	uint16_t bpid  = VP_BPID_GET(slab);
	uint32_t meta_bpid = 0;
	int      err = -EFAULT;
	struct slab_module_info *slab_m = \
		sys_get_unique_handle(FSL_OS_MOD_SLAB);

	if (buff >= 8) {
		fdma_dma_data(4,
		              slab_m->icid,
		              &meta_bpid,
		              (buff - 4),
		              (slab_m->fdma_dma_flags |
		        	      FDMA_DMA_DA_SYS_TO_WS_BIT));
		if (bpid == (meta_bpid & 0x00003FFF))
			return 0;
	}

	return err;
}

/*****************************************************************************/
__HOT_CODE int slab_release(struct slab *slab, uint64_t buff)
{
	int error = 0;
#ifdef DEBUG
	SLAB_ASSERT_COND_RETURN(SLAB_IS_HW_POOL(slab), -EINVAL);
	SLAB_ASSERT_COND_RETURN(slab_check_bpid(slab, buff) == 0, -EFAULT);
#endif
	error = vpool_refcount_decrement_and_release(SLAB_VP_POOL_GET(slab),
	                                             buff,
	                                             NULL);
	/* It's OK for buffer not to be released as long as
	 * there is no cdma_error */
	if ((error == VIRTUAL_POOLS_BUF_NOT_RELEASED) || (error == 0))
		return 0;
	else
		return -EFAULT;
}
/*****************************************************************************/
static int bpid_init(struct slab_hw_pool_info *hw_pools,
                     struct slab_bpid_info bpid) {

	if (hw_pools == NULL)
		return -EINVAL;

	hw_pools->pool_id          = bpid.bpid;
	hw_pools->alignment        = SLAB_DEFAULT_ALIGN;
	hw_pools->flags            = 0;
	hw_pools->mem_pid          = bpid.mem_pid;
	hw_pools->buff_size        = SLAB_SIZE_SET(bpid.size);
	return vpool_init_total_bman_bufs(bpid.bpid, 0);
}

static int dpbp_add(struct dprc_obj_desc *dev_desc, int ind,
                    struct slab_bpid_info *bpids_arr, uint32_t bpids_arr_size,
                    struct dprc *dprc)
{
	int    dpbp_id   = dev_desc->id;
	int    err       = 0;
	struct dpbp dpbp = { 0 };
	struct dpbp_attr attr;

	if(ind >= bpids_arr_size) {
		pr_err("Too many BPID's in the container num = %d\n", ind + 1);
		return -EINVAL;
	}

	dpbp.regs = dprc->regs;

	if ((err = dpbp_open(&dpbp, dpbp_id)) != 0) {
		pr_err("Failed to open DP-BP%d.\n", dpbp_id);
		return err;
	}

	if ((err = dpbp_enable(&dpbp)) != 0) {
		pr_err("Failed to enable DP-BP%d.\n", dpbp_id);
		return err;
	}

	if ((err = dpbp_get_attributes(&dpbp, &attr)) != 0) {
		pr_err("Failed to get attributes from DP-BP%d.\n", dpbp_id);
		return err;
	}


	pr_info("found DPBP ID: %d, with BPID %d\n",dpbp_id, attr.bpid);
	bpids_arr[ind].bpid = attr.bpid; /*Update found BP-ID*/
	return 0;
}

/*****************************************************************************/
static int dpbp_discovery(struct slab_bpid_info *bpids_arr,
                          uint32_t bpids_arr_size, int *n_bpids)
{
	struct dprc_obj_desc dev_desc;
	int dpbp_id = -1;
	int dev_count;
	int num_bpids = 0;
	int err = 0;
	int i = 0;
	struct dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);


	/*Calling MC to get bpid's*/
	if (dprc == NULL)
	{
		pr_err("Don't find AIOP root container \n");
		return -ENODEV;
	}


	if ((err = dprc_get_obj_count(dprc, &dev_count)) != 0) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->auth);
		return err;
	}


	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(dprc, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			/* TODO: print conditionally based on log level */
			pr_info("Found First DPBP ID: %d, Skipping, will be used for frame buffers\n", dev_desc.id);
			num_bpids = 1;
			break;
		}
	}

	if(num_bpids != 1) { /*Check if first dpbp was found*/
		pr_err("DP-BP not found in the container.\n");
		return -EAGAIN;
	}

	num_bpids = 0; /*for now we save the first dpbp for later use.*/
	/*Continue to search for dpbp's*/
	for (i = i+1; i < dev_count; i++) {
		dprc_get_obj(dprc, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			err = dpbp_add(&dev_desc, num_bpids, bpids_arr,
			               bpids_arr_size, dprc);
			if (err) {
				*n_bpids = num_bpids;
				return err;
			}
			num_bpids++;
		}
	}

	if (num_bpids == 0) {
		pr_err("DP-BP not found in the container.\n");
		return -EAGAIN;
	}

	*n_bpids = num_bpids;
	return 0;
}

static int slab_alocate_memory(int num_bpids, struct slab_module_info *slab_m, struct slab_bpid_info *bpids_arr)
{
	int i = 0, j = 0, num_of_buffs;
	int err = 0;
	dma_addr_t addr = 0;

	/* Set BPIDs */
	for(i = 0; i < num_bpids; i++) {
		err = bpid_init(&(slab_m->hw_pools[i]), bpids_arr[i]);
		if (err) {
			free_slab_module_memory(slab_m);
			return -ENAVAIL;
		}

		switch(slab_m->hw_pools[i].mem_pid){
		case MEM_PART_DP_DDR:
			num_of_buffs = SLAB_NUM_OF_BUFS_DPDDR;
			break;
		case MEM_PART_PEB:
			num_of_buffs = SLAB_NUM_OF_BUFS_PEB;
			break;
		default:
			pr_err("Partition type not supported\n");
			return -EINVAL;
		}

		/*Big malloc allocation for all buffers in bpid*/
		/* TODO number of buffers (SLAB_NUM_OF_BUFFS) should not be hard coded
		 * The buffer sizes are already aligned to 8, remember to shift if not.
		 * */

		addr = (dma_addr_t) fsl_os_xmalloc(
			(size_t)(slab_m->hw_pools[i].buff_size * num_of_buffs),
			slab_m->hw_pools[i].mem_pid,
			slab_m->hw_pools[i].alignment);

		if (addr == NULL) {
			return -ENOMEM;
		}

		addr = fsl_os_virt_to_phys((void *)addr);

		/* Isolation is enabled */
		for(j = 0; j < num_of_buffs; j++){
			fdma_release_buffer(slab_m->icid,
			                    slab_m->fdma_flags,
			                    slab_m->hw_pools[i].pool_id,
			                    addr);

			addr += slab_m->hw_pools[i].buff_size;

		}

		err = vpool_add_total_bman_bufs(slab_m->hw_pools[i].pool_id, num_of_buffs);
		if(err){
			return -EINVAL;
		}

	}
	return err;
}

/*****************************************************************************/
int slab_module_init(void)
{
	/* TODO Call MC to get all BPID per partition */
	struct   slab_bpid_info bpids_arr[] = SLAB_BPIDS_ARR;
	int      num_bpids = ARRAY_SIZE(bpids_arr);
	struct   slab_module_info *slab_m = NULL;
	int      err = 0;
	uint32_t cdma_cfg = 0;
	struct aiop_tile_regs *ccsr = (struct aiop_tile_regs *)\
		sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW, 0,
		                                  E_MAPPED_MEM_TYPE_GEN_REGS);

#ifndef AIOP_STANDALONE
	err = dpbp_discovery(&bpids_arr[0], ARRAY_SIZE(bpids_arr), &num_bpids);
	if (err) {
		pr_err("Failed DPBP discovery\n");
		return -ENODEV;
	}
#endif

	slab_m = fsl_os_xmalloc(sizeof(struct slab_module_info),
	                        SLAB_FAST_MEMORY,
	                        1);
	if (slab_m == NULL)
		return -ENOMEM;


	slab_m->num_hw_pools = (uint8_t)(num_bpids & 0xFF);
	slab_m->hw_pools     =
		fsl_os_xmalloc(sizeof(struct slab_hw_pool_info) * num_bpids,
		               SLAB_DDR_MEMORY,
		               1);

	slab_m->virtual_pool_struct  =
		fsl_os_xmalloc((sizeof(struct virtual_pool_desc) *
			SLAB_MAX_NUM_VP),
			SLAB_FAST_MEMORY,
			1);
	slab_m->callback_func_struct =
		fsl_os_xmalloc((sizeof(struct callback_s) * SLAB_MAX_NUM_VP),
		               SLAB_FAST_MEMORY,
		               1);

	if ((slab_m->hw_pools == NULL) ||
		(slab_m->virtual_pool_struct == NULL) ||
		(slab_m->callback_func_struct == NULL)) {

		free_slab_module_memory(slab_m);
		return -ENOMEM;
	}

	/* TODO vpool_init() API will change to get more allocated
	 * by malloc() memories */
	err = vpool_init((uint64_t)(slab_m->virtual_pool_struct),
	                 (uint64_t)(slab_m->callback_func_struct),
	                 SLAB_MAX_NUM_VP,
	                 0);
	if (err) {
		free_slab_module_memory(slab_m);
		return -ENAVAIL;
	}



	err = slab_alocate_memory(num_bpids, slab_m, bpids_arr);
	if(err){
		return err;
	}

	/* CDMA CFG register bits are needed for filling BPID */
	cdma_cfg           = ioread32_ccsr(&ccsr->cdma_regs.cfg);
	slab_m->icid       = (uint16_t)(cdma_cfg & CDMA_ICID_MASK);
	slab_m->fdma_flags = FDMA_ACQUIRE_NO_FLAGS;
	if (cdma_cfg & CDMA_BDI_BIT)
		slab_m->fdma_flags |= FDMA_ACQUIRE_BDI_BIT;

	slab_m->fdma_dma_flags = 0;
	if (cdma_cfg & CDMA_BMT_BIT)
		slab_m->fdma_dma_flags |= FDMA_DMA_BMT_BIT;
	if (cdma_cfg & CDMA_PL_BIT)
		slab_m->fdma_dma_flags |= FDMA_DMA_PL_BIT;
	if (cdma_cfg & CDMA_VA_BIT)
		slab_m->fdma_dma_flags |= FDMA_DMA_VA_BIT;

	pr_debug("CDMA CFG register = 0x%x addr = 0x%x\n", cdma_cfg, \
	         (uint32_t)&ccsr->cdma_regs.cfg);
	pr_debug("ICID = 0x%x flags = 0x%x\n", slab_m->icid, \
	         slab_m->fdma_flags);
	pr_debug("ICID = 0x%x dma flags = 0x%x\n", slab_m->icid, \
	         slab_m->fdma_dma_flags);

	/* Add to all system handles */
	err = sys_add_handle(slab_m, FSL_OS_MOD_SLAB, 1, 0);
	return err;
}

/*****************************************************************************/
void slab_module_free(void)
{
	/*TODO free reserved bpids before freeing the module using free_buffs_from_bman_pool*/
	struct slab_module_info *slab_m = \
		sys_get_unique_handle(FSL_OS_MOD_SLAB);

	sys_remove_handle(FSL_OS_MOD_SLAB, 0);

	if (slab_m != NULL)
		free_slab_module_memory(slab_m);
}

/*****************************************************************************/
int slab_debug_info_get(struct slab *slab, struct slab_debug_info *slab_info)
{
	int32_t temp = 0, m_buffs = 0, num_buffs = 0;
	int     i;
	struct slab_module_info *slab_m = \
		sys_get_unique_handle(FSL_OS_MOD_SLAB);

	if ((slab_info != NULL) && (slab_m != NULL) && SLAB_IS_HW_POOL(slab)) {
		if (vpool_read_pool(SLAB_VP_POOL_GET(slab),
		                    &slab_info->pool_id,
		                    &temp,
		                    &m_buffs,
		                    &num_buffs,
		                    (uint32_t *)&temp,
		                    &temp) == 0) {
			/* Modify num_buffs to have the number of available
			 * buffers not allocated */
			slab_info->num_buffs = (uint32_t)(m_buffs - num_buffs);
			slab_info->max_buffs = (uint32_t)m_buffs;

			temp = slab_m->num_hw_pools;
			for (i = 0; i < temp; i++)
				CP_POOL_DATA(slab_m, slab_info, i);
		}
	}

	return -EINVAL;
}

/*****************************************************************************/
__HOT_CODE int slab_refcount_incr(struct slab *slab, uint64_t buff)
{
#ifdef DEBUG
	SLAB_ASSERT_COND_RETURN(SLAB_IS_HW_POOL(slab), -EINVAL);
#endif
	cdma_refcount_increment(buff);

	return 0;
}

/*****************************************************************************/
__HOT_CODE int slab_refcount_decr(struct slab *slab, uint64_t buff)
{
	return slab_release(slab, buff);
}
