/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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

#include "common/types.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "kernel/fsl_spinlock.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpbp.h"
#include "fsl_dbg.h"
#include "slab.h"
#include "fdma.h"
#include "fsl_io.h"
#include "fsl_cdma.h"
#include "fsl_io_ccsr.h"
#include "aiop_common.h"
#include "fsl_mc_init.h"

__SHRAM struct slab_bman_pool_desc g_slab_bman_pools[SLAB_MAX_BMAN_POOLS_NUM];
__SHRAM struct slab_virtual_pools_main_desc g_slab_virtual_pools;

#define SLAB_ASSERT_COND_RETURN(COND, ERR)  \
	do { if (!(COND)) return (ERR); } while (0)

#define FOUND_SMALLER_SIZE(A, B) \
	hw_pools[(A)].buff_size > hw_pools[(B)].buff_size

/*  TODO use API from VPs when it will be added */
#define VP_DESC_ARR(SLAB) \
	((struct slab_v_pool *)g_slab_virtual_pools.virtual_pool_struct[SLAB_CLUSTER_ID_GET(SLAB)])

#define VP_REMAINING_BUFFS(SLAB) \
	(uint32_t)((VP_DESC_ARR + SLAB_VP_POOL_GET((SLAB)))->committed_bufs)

#define VP_BPID_GET(SLAB) \
	(uint16_t)g_slab_bman_pools[(VP_DESC_ARR(SLAB_VP_POOL_GET(SLAB)) + \
		SLAB_POOL_ID_GET(SLAB_VP_POOL_GET((SLAB))))->bman_array_index].bman_pool_id

#define CP_POOL_DATA(MOD, INFO, I) \
	{                                                              \
		if (MOD->hw_pools[I].pool_id == INFO->pool_id) {       \
			INFO->buff_size  = MOD->hw_pools[I].buff_size; \
			INFO->alignment  = MOD->hw_pools[I].alignment; \
			INFO->mem_pid    = MOD->hw_pools[I].mem_pid;   \
			return 0;                                      \
		}                                                      \
	}

static void slab_pool_init(
	struct slab_v_pool *virtual_pool_struct,
	slab_release_cb_t **callback_func,
	uint16_t num_of_virtual_pools,
	uint8_t flags,
	uint16_t cluster);
/***************************************************************************
 * slab_create_pool used by: slab_create
 ***************************************************************************/
static int slab_create_pool(
	int32_t max_bufs,
	int32_t committed_bufs,
	uint8_t flags,
	slab_release_cb_t *callback_func,
	uint32_t *slab_virtual_pool_id)
{

	uint32_t slab_vpool_id, cluster;
	uint32_t num_of_virtual_pools = g_slab_virtual_pools.num_of_virtual_pools;
	uint16_t bman_array_index = SLAB_MAX_BMAN_POOLS_NUM;
	int free_pool_found = FALSE;

	struct slab_v_pool *slab_virtual_pool;
	slab_release_cb_t **callback;

#ifdef DEBUG

	/* max_bufs must be equal or greater than committed_bufs */
	if (committed_bufs > max_bufs)
		return -EINVAL;

	/* committed_bufs and max_bufs must not be 0 */
	if ((!committed_bufs) || (!max_bufs))
		return -EINVAL;
#endif

	/* Spinlock this BMAN pool counter */
	lock_spinlock(
		(uint8_t *)&g_slab_bman_pools[bman_array_index].spinlock);

	/* Check if there are enough buffers to commit */
	if (g_slab_bman_pools[bman_array_index].remaining >= committed_bufs) {
		/* decrement the total available BMAN pool buffers */
		g_slab_bman_pools[bman_array_index].remaining -=
			committed_bufs;

		unlock_spinlock((uint8_t *)
		                &g_slab_bman_pools[bman_array_index].spinlock);

	} else {
		unlock_spinlock((uint8_t *)
		                &g_slab_bman_pools[bman_array_index].spinlock);
		return -ENOSPC;
	}

	lock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);

	/* Allocate a virtual pool ID */
	/* Return with error if it was not possible to
	 * allocate a virtual pool */
	for(cluster = 0; cluster < SLAB_MAX_NUM_OF_CLUSTERS_FOR_VPS; cluster++)
	{
		slab_virtual_pool =(struct slab_v_pool *)
				g_slab_virtual_pools.virtual_pool_struct[cluster];

		for(slab_vpool_id = 0; slab_vpool_id < num_of_virtual_pools; slab_vpool_id++) {
			if(slab_virtual_pool->max_bufs == 0) {
				/* use max_bufs as indicator */
				slab_virtual_pool->max_bufs = max_bufs;
				callback = (slab_release_cb_t **)g_slab_virtual_pools.callback_func[cluster];
				g_slab_virtual_pools.clusters_count[cluster] ++;
				free_pool_found = TRUE;
				break;
			}
			slab_virtual_pool++; /* increment the pointer for slab virtual pull */
		}
		if(free_pool_found)/*if we found empty virtual pool to use, break*/
			break;
		else
		{
			if(cluster + 1 < SLAB_MAX_NUM_OF_CLUSTERS_FOR_VPS)
			{
				/*try to allocate in ddr*/
				if(g_slab_virtual_pools.virtual_pool_struct[cluster + 1] == NULL){
					/*Try to allocate space for new cluster in ddr*/
					slab_virtual_pool =
						(struct slab_v_pool *)
						fsl_os_xmalloc((sizeof(struct slab_v_pool)* SLAB_MAX_NUM_VP_DDR),
						               SLAB_DDR_MEMORY,
						               1);
					callback = (slab_release_cb_t **) fsl_os_xmalloc((
							sizeof(slab_release_cb_t *) * SLAB_MAX_NUM_VP_DDR),
							SLAB_DDR_MEMORY,
							1);
					if(slab_virtual_pool == NULL ||
						callback == NULL )
					{
						if(slab_virtual_pool)
							fsl_os_xfree(slab_virtual_pool);
						if(callback)
							fsl_os_xfree(callback);

						atomic_incr32(&g_slab_bman_pools[bman_array_index].remaining,
						              committed_bufs);
						unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);
						return -ENOMEM;

					}
					else
					{
						slab_pool_init(slab_virtual_pool,
						               callback,
						               SLAB_MAX_NUM_VP_DDR,
						               0,
						               (uint16_t)cluster + 1);
						slab_vpool_id = 0; /*pool ID, NOT CLUSTER!!!*/
						/*Use the first pool in the cluster*/
						slab_virtual_pool->max_bufs = max_bufs;
						g_slab_virtual_pools.clusters_count[cluster + 1] ++;
						/*initialize cluster pools*/
						cluster ++;
						break;
					}


				}

			}
			else
			{       /*all the pools in all the clusters are full */
				atomic_incr32(&g_slab_bman_pools[bman_array_index].remaining,
				              committed_bufs);
				unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);
				return -ENOMEM;
			}

		}

	}
	unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);

	*slab_virtual_pool_id = SLAB_CLUSTER_ID_SET(cluster) | slab_vpool_id; /* Return the ID (cluster and pool_id) */


	slab_virtual_pool->committed_bufs = committed_bufs;
	slab_virtual_pool->allocated_bufs = 0;
	slab_virtual_pool->bman_array_index = bman_array_index;
	slab_virtual_pool->flags = flags;

	/* Check if a callback_func exists*/
	if(callback_func != NULL){
		/* Check if a callback structure exists and initialize the entry */
		if (g_slab_virtual_pools.callback_func[cluster] != NULL) {
			callback += slab_vpool_id;
			*callback = callback_func;
		}
	}

	return 0;
} /* End of vpool_create_pool */

/***************************************************************************
 * slab_release_pool used by: slab_free / slab_create - on error.
 ***************************************************************************/
static int slab_release_pool(uint32_t slab_virtual_pool_id)
{

	uint32_t cluster = SLAB_CLUSTER_ID_GET(slab_virtual_pool_id);
	struct slab_v_pool *slab_virtual_pool =
		(struct slab_v_pool *)
		g_slab_virtual_pools.virtual_pool_struct[cluster];

	slab_release_cb_t **callback = (slab_release_cb_t **)
							g_slab_virtual_pools.callback_func[cluster];

	slab_virtual_pool_id = SLAB_POOL_ID_GET(slab_virtual_pool_id);
	callback += slab_virtual_pool_id;
	slab_virtual_pool += slab_virtual_pool_id;

	lock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);



	if (slab_virtual_pool->allocated_bufs != 0) {
		unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);
		return -EACCES;
	}

	slab_virtual_pool->max_bufs = 0;

	g_slab_virtual_pools.clusters_count[cluster] --;
	fsl_os_print("pool #%d free\n",slab_virtual_pool_id);

	if(g_slab_virtual_pools.clusters_count[cluster] == 0 && cluster > 0) /*release pool cluster (counter of used pools is 0*/
	{
		if (g_slab_virtual_pools.virtual_pool_struct[cluster]){
			fsl_os_print("cluster #%d free\n",cluster);
			fsl_os_xfree(g_slab_virtual_pools.virtual_pool_struct[cluster]);
		}
		if (g_slab_virtual_pools.callback_func[cluster])
			fsl_os_xfree(g_slab_virtual_pools.callback_func[cluster]);

	}
	else if (*callback != NULL){ /*just erase the callback, No cluster release needed*/
		*callback = NULL;
	}

	/* max_bufs = 0 indicates a free pool */


	unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);

	/* Increment the total available BMAN pool buffers */
	atomic_incr32(
		&g_slab_bman_pools[slab_virtual_pool->bman_array_index].remaining,
		slab_virtual_pool->committed_bufs);

	return 0;
} /* End of vpool_release_pool */

/***************************************************************************
 * slab_pool_allocate_buff used by: slab_acquire
 ***************************************************************************/
__HOT_CODE static int slab_pool_allocate_buff(uint32_t slab_virtual_pool_id,
                                              uint64_t *context_address)
{
	int return_val;
	int allocate = 0;
	uint32_t cluster = SLAB_CLUSTER_ID_GET(slab_virtual_pool_id); /* fetch cluster ID*/


	// TODO: remove this if moving to handle
	struct slab_v_pool *slab_virtual_pool = (struct slab_v_pool *)
						g_slab_virtual_pools.virtual_pool_struct[cluster];

	slab_virtual_pool_id = SLAB_POOL_ID_GET(slab_virtual_pool_id); /*fetch pool id*/
	slab_virtual_pool += slab_virtual_pool_id;

	lock_spinlock((uint8_t *)&slab_virtual_pool->spinlock);

	/* First check if there are still available buffers
	 * in the VP committed area */
	if(slab_virtual_pool->allocated_bufs <
		slab_virtual_pool->committed_bufs) {
		allocate = 1; /* allocated from committed area */
		/* Else, check if there are still available buffers
		 * in the VP max-committed area */
	} else if (slab_virtual_pool->allocated_bufs < slab_virtual_pool->max_bufs) {
		/* There is still an extra space in the virtual pool,
		 * check BMAN pool */

		/* spinlock this BMAN pool counter */
		lock_spinlock((uint8_t *)&g_slab_bman_pools[slab_virtual_pool->
		                                            bman_array_index].spinlock);

		if ((g_slab_bman_pools[
		                       slab_virtual_pool->bman_array_index].remaining) > 0)
		{
			allocate = 2; /* allocated from remaining area */
			g_slab_bman_pools
			[slab_virtual_pool->bman_array_index].remaining--;
		}

		unlock_spinlock((uint8_t *)&g_slab_bman_pools[slab_virtual_pool->
		                                              bman_array_index].spinlock);
	}

	/* Request CDMA to allocate a buffer*/
	if (allocate) {

		slab_virtual_pool->allocated_bufs++;

		unlock_spinlock((uint8_t *)&slab_virtual_pool->spinlock);

		/* allocate a buffer with the CDMA */
		return_val = cdma_acquire_context_memory(
			(uint16_t)g_slab_bman_pools
			[slab_virtual_pool->bman_array_index].bman_pool_id,
			(uint64_t *)context_address); /* context_memory */

		/* If allocation failed,
		 * undo the counters increment/decrement */
		if (return_val) {
			atomic_decr32(&slab_virtual_pool->allocated_bufs, 1);
			if (allocate == 2) /* only if it was allocated from
					the remaining area */
				atomic_incr32(&g_slab_bman_pools[slab_virtual_pool->
				                                 bman_array_index].remaining, 1);
			return (return_val);
		}

		return 0;

	} else {
		unlock_spinlock((uint8_t *)&slab_virtual_pool->spinlock);
		return -ENAVAIL;
	}

} /* End of slab_pool_allocate_buff */


/***************************************************************************
 * slab_read_pool used by: slab_debug_info_get
 ***************************************************************************/
static int slab_read_pool(uint32_t slab_virtual_pool_id,
                                  uint16_t *bman_pool_id,
                                  int32_t *max_bufs,
                                  int32_t *committed_bufs,
                                  int32_t *allocated_bufs,
                                  uint32_t *flags,
                                  slab_release_cb_t **callback_func)
{

	slab_release_cb_t **callback;
	uint32_t cluster =SLAB_CLUSTER_ID_GET(slab_virtual_pool_id);;
	// TODO: remove this if moving to handle
	struct slab_v_pool *slab_virtual_pool =
		(struct slab_v_pool *)
		g_slab_virtual_pools.virtual_pool_struct[cluster];
	slab_virtual_pool_id = SLAB_POOL_ID_GET(slab_virtual_pool_id);

	slab_virtual_pool += slab_virtual_pool_id;

	*max_bufs = slab_virtual_pool->max_bufs;
	*committed_bufs = slab_virtual_pool->committed_bufs;
	*allocated_bufs = slab_virtual_pool->allocated_bufs;
	*bman_pool_id =
		g_slab_bman_pools[slab_virtual_pool->bman_array_index].bman_pool_id;

	*flags = (uint8_t)slab_virtual_pool->flags;

	/* Check if callback exists and return its address (can be null) */
	if (g_slab_virtual_pools.callback_func[cluster] != NULL) {
		callback = (slab_release_cb_t **)
			g_slab_virtual_pools.callback_func[cluster];
		callback += slab_virtual_pool_id;
		*callback_func = *callback;
	} else {
		*callback_func = 0;
	}

	return 0;
} /* slab_read_virtual_pool */

/***************************************************************************
 * slab_pool_init used by: slab_module_init
 ***************************************************************************/
static void slab_pool_init(
	struct slab_v_pool *virtual_pool_struct,
	slab_release_cb_t **callback_func,
	uint16_t num_of_virtual_pools,
	uint8_t flags,
	uint16_t cluster)
{
	int32_t i;

	struct slab_v_pool *slab_virtual_pool;

	/* Mask when down-casting.
	 *  Currently the address is only in Shared RAM (32 bit)
	 */
	slab_virtual_pool =
		(struct slab_v_pool *)virtual_pool_struct ;

	g_slab_virtual_pools.virtual_pool_struct[cluster] = virtual_pool_struct;
	g_slab_virtual_pools.callback_func[cluster] = callback_func;
	g_slab_virtual_pools.num_of_virtual_pools = num_of_virtual_pools;
	g_slab_virtual_pools.clusters_count[cluster] = 0;
	g_slab_virtual_pools.flags = flags;

	/* Init 'max' to zero, since it's an indicator to
	 * pool ID availability */
	for(i = 0; i < num_of_virtual_pools; i++) {
		slab_virtual_pool->max_bufs = 0;
		slab_virtual_pool->spinlock = 0; /* clear spinlock indicator */
		slab_virtual_pool++; /* increment the pointer */
	}

	/* Init 'remaining' to -1, since it's an indicator an empty index */
	if(cluster == 0) /*Only for main cluster which is in SHRAM*/
	{
		for (i=0; i< SLAB_MAX_BMAN_POOLS_NUM; i++) {
			g_slab_bman_pools[i].remaining = -1;
			/* clear spinlock indicator */
			g_slab_bman_pools[i].spinlock = 0;
		}
	}
} /* End of vpool_init */

/*****************************************************************************/

static int slab_add_bman_buffs_to_pool(
	uint16_t bman_pool_id,
	int32_t additional_bufs)
{
	int i;
	int16_t bman_array_index = -1;

#ifdef DEBUG
	/* Check the arguments correctness */
	if (bman_pool_id >= SLAB_MAX_BMAN_POOLS_NUM)
		return -EINVAL;
#endif

	/* Check which BMAN pool ID array element matches the ID */
	for (i=0; i< SLAB_MAX_BMAN_POOLS_NUM; i++) {
		if (g_slab_bman_pools[i].bman_pool_id == bman_pool_id) {
			bman_array_index = (int16_t)i;
			break;
		}
	}

#ifdef DEBUG
	/* Check the arguments correctness */
	if (bman_array_index < 0)
		return -EINVAL;
#endif
	/* Increment the total available BMAN pool buffers */
	atomic_incr32(&g_slab_bman_pools[bman_array_index].remaining,
	              additional_bufs);

	return 0;
} /* End of vpool_add_total_bman_bufs */

/*****************************************************************************/

static int slab_decr_bman_buffs_from_pool(
	uint16_t bman_pool_id,
	int32_t less_bufs)
{

	int i;
	uint16_t bman_array_index = 0;

#ifdef DEBUG
	/* Check the arguments correctness */
	if (bman_pool_id >= SLAB_MAX_BMAN_POOLS_NUM)
		return -EINVAL;
#endif

	/* Check which BMAN pool ID array element matches the ID */
	for (i=0; i< SLAB_MAX_BMAN_POOLS_NUM; i++) {
		if (g_slab_bman_pools[i].bman_pool_id == bman_pool_id) {
			bman_array_index = (uint16_t)i;
			break;
		}
	}

	lock_spinlock(
		(uint8_t *)&g_slab_bman_pools[bman_array_index].spinlock);

	/* Check if there are enough buffers to reserve */
	if (g_slab_bman_pools[bman_array_index].remaining >= less_bufs) {
		/* decrement the BMAN pool buffers */
		g_slab_bman_pools[bman_array_index].remaining -=
			less_bufs;

		unlock_spinlock((uint8_t *)
		                &g_slab_bman_pools[bman_array_index].spinlock);

	} else {
		unlock_spinlock((uint8_t *)
		                &g_slab_bman_pools[bman_array_index].spinlock);
		return -ENOMEM;
	}

	return 0;
} /* End of vpool_decr_bman_bufs */




static void free_buffs_from_bman_pool(uint16_t bpid, int32_t num_buffs,
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
		*bpid = hw_pools[temp].pool_id;
		return 0;
	}

	return -ENAVAIL;
}

/*****************************************************************************/
int slab_find_and_free_bpid(uint32_t num_buffs,
                            uint16_t *bpid)
{
	int error = 0;
	error = slab_add_bman_buffs_to_pool(*bpid,(int32_t)num_buffs);

	if(error)
		return error;


	return 0;
}

/*****************************************************************************/
int slab_find_and_reserve_bpid(uint32_t num_buffs,
                               uint16_t buff_size,
                               uint16_t alignment,
                               uint8_t  mem_pid,
                               int *num_reserved_buffs,
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

	error = slab_decr_bman_buffs_from_pool(*bpid,(int)num_buffs);

	SLAB_ASSERT_COND_RETURN(error == 0, error);

	*num_reserved_buffs = (int)num_buffs;


	return 0;
}

/*****************************************************************************/
static void free_slab_module_memory(struct slab_module_info *slab_m)
{
	/* TODO there still some static allocations in VP init
	 * need to add them to slab_module_init() and then free them here
	 */
	int i;
	/* TODO - cluster free support needed*/
	for(i = SLAB_MAX_NUM_OF_CLUSTERS_FOR_VPS -1; i >= 0; i-- ){
	if (g_slab_virtual_pools.virtual_pool_struct[i])
		fsl_os_xfree(g_slab_virtual_pools.virtual_pool_struct[i]);
	if (g_slab_virtual_pools.callback_func[i])
		fsl_os_xfree(g_slab_virtual_pools.callback_func[i]);
	}
	if (slab_m->hw_pools)
		fsl_os_xfree(slab_m->hw_pools);
	fsl_os_xfree(slab_m);
}

/*****************************************************************************/
static inline int sanity_check_slab_create(uint32_t    committed_buffs,
                                           uint16_t    buff_size,
                                           uint16_t    alignment,
                                           uint8_t     mem_pid,
                                           uint32_t    flags)
{
	SLAB_ASSERT_COND_RETURN(committed_buffs > 0,   -EINVAL);
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
int slab_create(uint32_t    committed_buffs,
                uint32_t    max_buffs,
                uint16_t    buff_size,
                uint16_t    prefix_size,
                uint16_t    postfix_size,
                uint16_t    alignment,
                uint8_t     mem_pid,
                uint32_t    flags,
                slab_release_cb_t *release_cb,
                struct slab **slab)
{
	int        error = 0;
	uint32_t   slab_virtual_pool_id = 0, cluster, slab_vpool_id;
	uint32_t num_of_virtual_pools = g_slab_virtual_pools.num_of_virtual_pools;
	uint16_t bman_array_index = SLAB_MAX_BMAN_POOLS_NUM;
	int i, found = FALSE;
	struct slab_v_pool *slab_virtual_pool;
	slab_release_cb_t **callback;
	struct slab_module_info *slab_m = sys_get_unique_handle(FSL_OS_MOD_SLAB);
	int     num_bpids = slab_m->num_hw_pools;
	struct  slab_hw_pool_info *hw_pools = slab_m->hw_pools;

	UNUSED(prefix_size);
	UNUSED(postfix_size);
	max_buffs = committed_buffs;
#ifdef DEBUG
	/* Sanity checks */
	error = sanity_check_slab_create(committed_buffs,
	                                 buff_size,
	                                 alignment,
	                                 mem_pid,
	                                 flags);
	if (error)
		return -ENAVAIL;
	/* TODO remove it when max_buffs are supported */
	if (max_buffs > committed_buffs)
		return -ENAVAIL;

	/* committed_bufs and max_bufs must not be 0  when max buffs will be supported*/
	/*if ((!committed_buffs) || (!max_buffs))
		return -EINVAL;*/
#endif

	*((uint32_t *)slab) = 0;
	/* Only HW SLAB is supported */

	/* TODO add max_buffs to slab_create_pool when it will be supported */

	/*Find hardware pool with enough space*/
	for (i = 0; i < num_bpids; i++)
	{
		if ((hw_pools[i].mem_pid == mem_pid)         &&
			(hw_pools[i].alignment >= alignment) &&
			(SLAB_SIZE_GET(hw_pools[i].buff_size) >= buff_size))
		{
			/*First find the first relevant
			 * Lock
			 * check if there is enough buffers
			 * acquire
			 * unlock
			 * if lower found, lock acquire return*/


			lock_spinlock((uint8_t *)&g_slab_bman_pools[i].spinlock);
			if (g_slab_bman_pools[i].remaining >= committed_buffs)
			{
				if(!found) { /*found  = FALSE in the first time */
					/* decrement the total available BMAN pool buffers */
					g_slab_bman_pools[i].remaining -= committed_buffs;
					found = TRUE;
					bman_array_index = (uint16_t)i;
					unlock_spinlock((uint8_t *) &g_slab_bman_pools[i].spinlock);
				}
				else if(FOUND_SMALLER_SIZE(bman_array_index, i))
				{
					g_slab_bman_pools[i].remaining -= committed_buffs;
					g_slab_bman_pools[bman_array_index].remaining += committed_buffs;
					unlock_spinlock((uint8_t *) &g_slab_bman_pools[i].spinlock);
					bman_array_index = (uint16_t)i;

				}

			}
			else
				unlock_spinlock((uint8_t *)
				                &g_slab_bman_pools[i].spinlock);

		}
	} /*for (i = 0; i < num_bpids; i++)*/

	if(!found)
		return -ENOSPC;

	found = FALSE;

/***************************************************************************************/
	lock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);
	/* Allocate a virtual pool ID */
	/* Return with error if it was not possible to
	 * allocate a virtual pool */
	for(cluster = 0; cluster < SLAB_MAX_NUM_OF_CLUSTERS_FOR_VPS; cluster++)
	{
		slab_virtual_pool =(struct slab_v_pool *)
						g_slab_virtual_pools.virtual_pool_struct[cluster];

		for(slab_vpool_id = 0; slab_vpool_id < num_of_virtual_pools; slab_vpool_id++) {
			if(slab_virtual_pool->max_bufs == 0) {
				/* use max_bufs as indicator */
				slab_virtual_pool->max_bufs = (int32_t)max_buffs;
				callback = (slab_release_cb_t **)g_slab_virtual_pools.callback_func[cluster];
				g_slab_virtual_pools.clusters_count[cluster] ++;
				found = TRUE;
				break;
			}
			slab_virtual_pool++; /* increment the pointer for slab virtual pull */
		}
		if(found)/*if we found empty virtual pool to use, break*/
			break;
		else
		{
			if(cluster + 1 < SLAB_MAX_NUM_OF_CLUSTERS_FOR_VPS)
			{
				/*try to allocate in ddr*/
				if(g_slab_virtual_pools.virtual_pool_struct[cluster + 1] == NULL){
					/*Try to allocate space for new cluster in ddr*/
					slab_virtual_pool =
						(struct slab_v_pool *)
						fsl_os_xmalloc((sizeof(struct slab_v_pool)* SLAB_MAX_NUM_VP_DDR),
						               SLAB_DDR_MEMORY,
						               1);
					callback = (slab_release_cb_t **) fsl_os_xmalloc((
						sizeof(slab_release_cb_t *) * SLAB_MAX_NUM_VP_DDR),
						SLAB_DDR_MEMORY,
						1);
					if(slab_virtual_pool == NULL ||
						callback == NULL )
					{
						if(slab_virtual_pool)
							fsl_os_xfree(slab_virtual_pool);
						if(callback)
							fsl_os_xfree(callback);

						atomic_incr32(&g_slab_bman_pools[bman_array_index].remaining,
						              (int32_t)committed_buffs);
						unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);
						return -ENOMEM;

					}
					else
					{
						slab_pool_init(slab_virtual_pool,
						               callback,
						               SLAB_MAX_NUM_VP_DDR,
						               0,
						               (uint16_t)cluster + 1);
						slab_vpool_id = 0; /*pool ID, NOT CLUSTER!!!*/
						/*Use the first pool in the cluster*/
						slab_virtual_pool->max_bufs = (int32_t)max_buffs;
						g_slab_virtual_pools.clusters_count[cluster + 1] ++;
						/*initialize cluster pools*/
						cluster ++;
						break;
					}


				}

			}
			else
			{       /*all the pools in all the clusters are full */
				atomic_incr32(&g_slab_bman_pools[bman_array_index].remaining,
				              (int32_t)committed_buffs);
				unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);
				return -ENOMEM;
			}

		}

	}
	unlock_spinlock((uint8_t *)&g_slab_virtual_pools.global_spinlock);

	slab_virtual_pool_id = SLAB_CLUSTER_ID_SET(cluster) | slab_vpool_id; /* Return the ID (cluster and pool_id) */


	slab_virtual_pool->committed_bufs = (int32_t)committed_buffs;
	slab_virtual_pool->allocated_bufs = 0;
	slab_virtual_pool->bman_array_index = bman_array_index;
	slab_virtual_pool->flags = (uint8_t)flags;

	/* Check if a callback_func exists*/
	if(release_cb != NULL){
		/* Check if a callback structure exists and initialize the entry */
		if (g_slab_virtual_pools.callback_func[cluster] != NULL) {
			callback += slab_vpool_id;
			*callback = release_cb;
		}
	}



	SLAB_ASSERT_COND_RETURN(error == 0, error);

	*((uint32_t *)slab) = SLAB_HW_POOL_CREATE(slab_virtual_pool_id);

	return 0;
}

/*****************************************************************************/
int slab_free(struct slab **slab)
{
	int err;

	if (SLAB_IS_HW_POOL(*slab)) {

		err = slab_release_pool(SLAB_VP_POOL_GET(*slab));

		if(err)
			return err;

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

	if (slab_pool_allocate_buff(SLAB_VP_POOL_GET(slab), buff))
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
	uint32_t slab_virtual_pool_id = SLAB_VP_POOL_GET(slab);
	uint32_t cluster = SLAB_CLUSTER_ID_GET(slab_virtual_pool_id);
	slab_release_cb_t **callback = (slab_release_cb_t **)
						g_slab_virtual_pools.callback_func[cluster];
	struct slab_v_pool *slab_virtual_pool = (struct slab_v_pool *)
						g_slab_virtual_pools.virtual_pool_struct[cluster];

#ifdef DEBUG
	SLAB_ASSERT_COND_RETURN(SLAB_IS_HW_POOL(slab), -EINVAL);
	SLAB_ASSERT_COND_RETURN(slab_check_bpid(slab, buff) == 0, -EFAULT);
#endif

	slab_virtual_pool_id = SLAB_POOL_ID_GET(slab_virtual_pool_id); /*Fetch pool ID*/

	callback += slab_virtual_pool_id;
	if (*callback != NULL)
		(*callback)(buff);

	cdma_release_context_memory(buff);

	slab_virtual_pool += slab_virtual_pool_id;

	lock_spinlock((uint8_t *)&slab_virtual_pool->spinlock);

	/* First check if buffers were allocated from the common pool */
	if(slab_virtual_pool->allocated_bufs >
		slab_virtual_pool->committed_bufs) 	{
		/* One buffer returns to the common pool */
		atomic_incr32(&g_slab_bman_pools
		              [slab_virtual_pool->bman_array_index].remaining, 1);
	}
	slab_virtual_pool->allocated_bufs--;
	unlock_spinlock((uint8_t *)&slab_virtual_pool->spinlock);
	return 0;
}
/*****************************************************************************/
static int bpid_init(struct slab_hw_pool_info *hw_pools,
                     struct slab_bpid_info bpid) {
	int i;
	if (hw_pools == NULL)
		return -EINVAL;

	hw_pools->pool_id          = bpid.bpid;
	hw_pools->alignment        = SLAB_DEFAULT_ALIGN;
	hw_pools->flags            = 0;
	hw_pools->mem_pid          = bpid.mem_pid;
	hw_pools->buff_size        = SLAB_SIZE_SET(bpid.size);

	for (i=0; i< SLAB_MAX_BMAN_POOLS_NUM; i++) {
		/* check if virtual_bman_pools[i] is empty */
		if (g_slab_bman_pools[i].remaining == -1) {
			g_slab_bman_pools[i].bman_pool_id = bpid.bpid;
			g_slab_bman_pools[i].remaining = 0;
			break;
		}
	}
	return 0;
}

static int dpbp_add(struct dprc_obj_desc *dev_desc, int ind,
                    struct slab_bpid_info *bpids_arr, uint32_t bpids_arr_size,
                    struct mc_dprc *dprc)
{
	int      dpbp_id  = dev_desc->id;
	int      err      = 0;
	uint16_t dpbp     = 0;
	struct dpbp_attr attr;

	if(ind >= bpids_arr_size) {
		pr_err("Too many BPID's in the container num = %d\n", ind + 1);
		return -EINVAL;
	}

	if ((err = dpbp_open(&dprc->io, dpbp_id, &dpbp)) != 0) {
		pr_err("Failed to open DP-BP%d.\n", dpbp_id);
		return err;
	}

	if ((err = dpbp_enable(&dprc->io, dpbp)) != 0) {
		pr_err("Failed to enable DP-BP%d.\n", dpbp_id);
		return err;
	}

	if ((err = dpbp_get_attributes(&dprc->io, dpbp, &attr)) != 0) {
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);


	/*Calling MC to get bpid's*/
	if (dprc == NULL)
	{
		pr_err("Don't find AIOP root container \n");
		return -ENODEV;
	}


	if ((err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count)) != 0) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}


	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			/* TODO: print conditionally based on log level */
			pr_info("Found First DPBP ID: %d, Skipping, will be used for frame buffers\n", dev_desc.id);
			num_bpids = 1;
			break;
		}
	}

	if(num_bpids != 1) { /*Check if first dpbp was found*/
		pr_err("DP-BP not found in the container.\n");
		return -ENODEV;
	}

	num_bpids = 0; /*for now we save the first dpbp for later use.*/
	/*Continue to search for dpbp's*/
	for (i = i+1; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
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
		return -ENODEV;
	}

	*n_bpids = num_bpids;
	return 0;
}

static int slab_alocate_memory(int num_bpids, struct slab_module_info *slab_m, struct slab_bpid_info *bpids_arr)
{
	int i = 0, j = 0;
	int err = 0;
	dma_addr_t addr = 0;
	int32_t num_of_buffs;
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

		err = slab_add_bman_buffs_to_pool(slab_m->hw_pools[i].pool_id, num_of_buffs);
		if(err){
			return -EINVAL;
		}

		slab_m->hw_pools[i].total_num_buffs = num_of_buffs;

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
	slab_release_cb_t **callback_func = NULL;
	struct slab_v_pool *virtual_pool_struct = NULL;
	struct aiop_tile_regs *ccsr = (struct aiop_tile_regs *)\
		sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW, 0,
		                                  E_MAPPED_MEM_TYPE_GEN_REGS);


	err = dpbp_discovery(&bpids_arr[0], ARRAY_SIZE(bpids_arr), &num_bpids);
	if (err) {
		pr_err("Failed DPBP discovery\n");
		return -ENODEV;
	}


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

	virtual_pool_struct  = (struct slab_v_pool *)
				fsl_os_xmalloc((sizeof(struct slab_v_pool) *
					SLAB_MAX_NUM_VP_SHRAM),
					SLAB_FAST_MEMORY,
					1);
	callback_func =	(slab_release_cb_t **) fsl_os_xmalloc((
		sizeof(slab_release_cb_t *) * SLAB_MAX_NUM_VP_SHRAM),
		SLAB_FAST_MEMORY,
		1);

	if ((slab_m->hw_pools == NULL) ||
		(virtual_pool_struct == NULL) ||
		(callback_func == NULL)) {

		free_slab_module_memory(slab_m);
		return -ENOMEM;
	}

	/* TODO vpool_init() API will change to get more allocated
	 * by malloc() memories */
	slab_pool_init(virtual_pool_struct,
	               callback_func,
	               SLAB_MAX_NUM_VP_SHRAM,
	               0,
	               0);


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

	err = slab_alocate_memory(num_bpids, slab_m, bpids_arr);
	if(err){
		return err;
	}

	/* Add to all system handles */
	err = sys_add_handle(slab_m, FSL_OS_MOD_SLAB, 1, 0);
	return err;
}

/*****************************************************************************/
void slab_module_free(void)
{
	int i;
	struct slab_module_info *slab_m = \
		sys_get_unique_handle(FSL_OS_MOD_SLAB);

	sys_remove_handle(FSL_OS_MOD_SLAB, 0);

	if (slab_m != NULL){
		for(i = 0; i < slab_m->num_hw_pools; i++)
			free_buffs_from_bman_pool(
				slab_m->hw_pools[i].pool_id,
				slab_m->hw_pools[i].total_num_buffs,
				slab_m->icid,
				slab_m->fdma_flags);
		free_slab_module_memory(slab_m);

	}
}

/*****************************************************************************/
int slab_debug_info_get(struct slab *slab, struct slab_debug_info *slab_info)
{
	int32_t temp = 0, m_buffs = 0, num_buffs = 0;
	uint32_t flags =0;
	int     i;
	slab_release_cb_t *release_cb = NULL;
	struct slab_module_info *slab_m = \
		sys_get_unique_handle(FSL_OS_MOD_SLAB);

	if ((slab_info != NULL) && (slab_m != NULL) && SLAB_IS_HW_POOL(slab)) {
		if (slab_read_pool(SLAB_VP_POOL_GET(slab),
		                           &slab_info->pool_id,
		                           &temp,
		                           &m_buffs,
		                           &num_buffs,
		                           &flags,
		                           &release_cb) == 0) {
			/* Modify num_buffs to have the number of available
			 * buffers not allocated */
			slab_info->committed_buffs = (uint32_t)(m_buffs - num_buffs);
			slab_info->max_buffs = (uint32_t)m_buffs;

			temp = slab_m->num_hw_pools;
			for (i = 0; i < temp; i++)
				CP_POOL_DATA(slab_m, slab_info, i);
		}
	}

	return -EINVAL;
}
