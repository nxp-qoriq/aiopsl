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

#include "fsl_tman.h"
#include "fsl_malloc.h"
#include "fsl_aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_core_booke.h"
#include "rcu.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_spinlock.h"
#include "fsl_sl_dbg.h"
#include "fsl_spinlock.h"

struct rcu g_rcu = {0,		/* list_head */
                    0,		/* list_tail */
                    {0},	/* sw_ctstws */
                    0,		/* list_size */
                    NULL,	/* slab */
                    NULL,	/* dcsr regs */
                    MEM_PART_DP_DDR, /* mem_heap */
                    0,		/* committed */
                    0,		/* max */
                    0xFFFF,	/* delay */
                    0};		/* sw_ctstws_lock */

uint8_t g_sl_tmi_id = 0xff;

__TASK uint8_t g_rcu_unlock = 0; 

int rcu_init();
void rcu_free();
void rcu_tman_cb(uint64_t opaque1, uint16_t opaque2);
int rcu_default_early_init();

int rcu_early_init(uint16_t delay, uint32_t committed, uint32_t max)
{
	int err;
	
	/* Count extra buffers */
	if ((max - committed) > g_rcu.max)
		g_rcu.max = (max - committed);


	if (g_rcu.committed >= RCU_DEFAULT_COMMITTED) {
		err = slab_register_context_buffer_requirements(committed,
		                                                committed + g_rcu.max,
		                                                sizeof(struct rcu_job),
		                                                8,
		                                                g_rcu.mem_heap,
		                                                0,
		                                                0);
		ASSERT_COND(!err);

	} else if ((g_rcu.committed + committed) > RCU_DEFAULT_COMMITTED) {
		err = slab_register_context_buffer_requirements((g_rcu.committed + committed) - RCU_DEFAULT_COMMITTED,
		                                                (g_rcu.committed + committed) - RCU_DEFAULT_COMMITTED + g_rcu.max,
		                                                sizeof(struct rcu_job),
		                                                8,
		                                                g_rcu.mem_heap,
		                                                0,
		                                                0);
		ASSERT_COND(!err);
	} else {
		pr_info("RCU already reserved committed %d buffers\n",
		        RCU_DEFAULT_COMMITTED);
	}

	g_rcu.committed += committed;

	/* Smallest delay */
	if (delay < g_rcu.delay)
		g_rcu.delay = delay;

	return 0;
}

int rcu_default_early_init()
{
	int err;

	/* Support no dp ddr */
	if (fsl_mem_exists(MEM_PART_DP_DDR)){
		g_rcu.mem_heap = MEM_PART_DP_DDR;
	}
	else if(fsl_mem_exists(MEM_PART_SYSTEM_DDR)){
		g_rcu.mem_heap = MEM_PART_SYSTEM_DDR;
	}
	else{
		pr_err("DDR memory not found\n");
		return -ENOMEM;
	}

	pr_info("RCU module reserves committed %d \n", RCU_DEFAULT_COMMITTED);
	err = slab_register_context_buffer_requirements(RCU_DEFAULT_COMMITTED,
	                                                RCU_DEFAULT_COMMITTED,
	                                                sizeof(struct rcu_job),
	                                                8,
	                                                g_rcu.mem_heap,
	                                                0,
	                                                0);
	ASSERT_COND(!err);

	return 0;
}

int rcu_init()
{
	int err, cluster, core;
	uint64_t tman_addr;
	uint32_t m_timers = 10;

	pr_info("Init RCU\n");
	if (g_rcu.delay == 0xFFFF) {
		pr_err("Call rcu_early_init() to setup rcu \n");
		pr_err("Setting RCU defaults.. \n");
		g_rcu.committed = RCU_DEFAULT_COMMITTED;
		g_rcu.max = RCU_DEFAULT_COMMITTED;
		g_rcu.delay = RCU_DEFAULT_DELAY;
	} else {
		/* Convert extra to committed, default extra max can be reused
		 * by other applications */
		g_rcu.max += g_rcu.committed;
	}

	pr_info("RCU committed = %d, max = %d, delay = %d\n",
	        (int)g_rcu.committed, (int)g_rcu.max, (int)g_rcu.delay);

	g_rcu.regs = (struct aiop_dcsr_regs *)\
		(AIOP_PERIPHERALS_OFF + SOC_PERIPH_OFF_DCSR);

	for (cluster = 0; cluster < AIOP_MAX_NUM_CLUSTERS; cluster++) {
		for (core = 0; core < AIOP_MAX_NUM_CORES_IN_CLUSTER; core++) {
			g_rcu.sw_ctstws[cluster][core] = CTSTWS_TASKS_MASK;
		}
	}

	err = slab_create(g_rcu.committed,
	                  g_rcu.max,
	                  sizeof(struct rcu_job),
	                  8,
	                  g_rcu.mem_heap,
	                  0,
	                  NULL,
	                  &(g_rcu.slab));
	if (err) {
		pr_err("No slab for RCU\n");
		return err;
	}

	/*
	 * TODO Move SL tmi create to a separate module
	 */
	if (g_sl_tmi_id == 0xff) {
		tman_addr = 0;
		err = fsl_os_get_mem((64 * m_timers + 1),
		                     g_rcu.mem_heap,
		                     64,
		                     &tman_addr);
		ASSERT_COND(!err && tman_addr);

		err = tman_create_tmi(tman_addr, m_timers, &g_sl_tmi_id);
		ASSERT_COND(!err);
	}
	
	return 0;
}

static void delete_tmi_cb(uint64_t opaque1, uint16_t opaque2)
{
	UNUSED(opaque1);
	UNUSED(opaque2);
	fdma_terminate_task();
}

void rcu_free()
{
	int err;

	/* destroy slab */
	err = slab_free(&g_rcu.slab);
	ASSERT_COND(!err);

	/* destroy tmi
	 * TODO SL timer should be in a separate module */
	if (g_sl_tmi_id != 0xff) {
		tman_delete_tmi(delete_tmi_cb, 0,
		                      g_sl_tmi_id, 0, 0);
		g_sl_tmi_id = 0xff;
	}
}

static int enqueue(rcu_cb_t *cb, uint64_t param)
{
	uint32_t size;
	int err;
	uint64_t job;
	uint64_t next;
	struct rcu_job s_job;

	job = 0;
	err = slab_acquire(g_rcu.slab, &job);
	if (err || (job == 0)) {
		return err;
	}

	/* Copy the job using CDMA */
	s_job.next	= 0;
	s_job.cb	= cb;
	s_job.param	= param;
	cdma_write(job, &s_job, sizeof(struct rcu_job));


	RCU_MUTEX_W_TAKE;

	if (g_rcu.list_size == 0) {
		g_rcu.list_head = job;
		g_rcu.list_tail = g_rcu.list_head;
	} else {
		ASSERT_COND(g_rcu.list_tail);
		next = job;
		cdma_write(g_rcu.list_tail + offsetof(struct rcu_job, next),
		           &next, sizeof(next));

		g_rcu.list_tail = job;
	}

	atomic_incr32(&g_rcu.list_size, 1);
	size = (uint32_t)g_rcu.list_size;

	RCU_MUTEX_RELEASE;

	return (int)size;
}

static int dequeue(rcu_cb_t **cb, uint64_t *param)
{
	uint64_t job;
	int err;
	struct rcu_job s_job;
	int size;

	ASSERT_COND(g_rcu.list_size > 0);
	ASSERT_COND(g_rcu.list_head);

	RCU_MUTEX_W_TAKE;

	cdma_read(&s_job, g_rcu.list_head, sizeof(struct rcu_job));
	*cb = s_job.cb;
	*param = s_job.param;

	job = g_rcu.list_head;
	g_rcu.list_head = s_job.next;

	if (g_rcu.list_head == 0) {
		g_rcu.list_tail = 0;
		ASSERT_COND(g_rcu.list_size == 1);
	}

	atomic_decr32(&g_rcu.list_size, 1);
	size = g_rcu.list_size;

	RCU_MUTEX_RELEASE;

	err = slab_release(g_rcu.slab, job);
	ASSERT_COND(!err);

	return size;
}

static void prime_CTSTWS()
{
	int cluster, core;
//	uint32_t ctstws;

	for (cluster = 0; cluster < AIOP_MAX_NUM_CLUSTERS; cluster++) {
		for (core = 0; core < AIOP_MAX_NUM_CORES_IN_CLUSTER; core++) {
			iowrite32_ccsr(0,
			               &(g_rcu.regs->\
			               clustr[cluster].core[core].ctstws));
//			ctstws = ioread32_ccsr(&(g_rcu.regs->\
//			               clustr[cluster].core[core].ctstws));
//			pr_debug("core %d ctstws = 0x%x\n", (cluster * 4 + core), ctstws);
		}
	}
}

static int done_CTSTWS()
{
	uint32_t cluster;
	uint32_t core;
	uint32_t mask;
	uint32_t ctstws;

	for (cluster = 0; cluster < AIOP_MAX_NUM_CLUSTERS; cluster++) {
		for (core = 0; core < AIOP_MAX_NUM_CORES_IN_CLUSTER; core++) {

			ctstws = ioread32_ccsr(&(g_rcu.regs->\
				clustr[cluster].core[core].ctstws));
			mask = g_rcu.sw_ctstws[cluster][core];

			if (ctstws & mask) {
				sl_pr_debug("core 0x%x ctstws = 0x%x\n", (cluster * 4 + core), ctstws);
				return 0;
			}
		}
	}

	return 1;
}

static int init_one_shot_timer(int batch_size)
{
	int err;
	uint32_t timer_handle;
	uint16_t delay;

	/*
	 * TODO consider using different delays for polling and the first
	 * rcu task, should there be a correlation between them ?
	 */
	delay = g_rcu.delay * 1000; /* User defined delay */
	if (batch_size > 0)
		delay = 1000; /* 1 msec delay for polling on CTSTWS */

	/* Tman requirement */
	ASSERT_COND(delay > 10);
	ASSERT_COND(delay < ((0x1 << 16) - 10));

	err = tman_create_timer(g_sl_tmi_id/* tmi_id */,
	                        TMAN_CREATE_TIMER_MODE_USEC_GRANULARITY |
	                        TMAN_CREATE_TIMER_ONE_SHOT /* flags */,
	                        delay /* duration */,
	                        (uint64_t)batch_size /* opaque_data1 */,
	                        0 /* opaque_data2 */,
	                        rcu_tman_cb /* tman_timer_cb */,
	                        &timer_handle /* *timer_handle */);
	return err;
}

void rcu_tman_cb(uint64_t ubatch_size, uint16_t opaque2)
						__attribute__ ((noreturn))
{
	int batch_size = (int)ubatch_size;
	int i;
	rcu_cb_t *cb;
	uint64_t param;
	int size;

	UNUSED(opaque2);

	rcu_read_unlock();

	/* Confirmation for running the timer again */
	tman_timer_completion_confirmation(\
		TMAN_GET_TIMER_HANDLE(HWC_FD_ADDRESS));

	if (batch_size == -1) {
		batch_size = (int)g_rcu.list_size;
		/* next prime will be only after this one has finished */
		prime_CTSTWS();
	}

	if (done_CTSTWS()) {
		sl_pr_debug("############ DONE batch_size = %d\n", batch_size);
		ASSERT_COND(batch_size > 0);
		for (i = 0; i < batch_size; i++) {
			size = dequeue(&cb, &param);
			ASSERT_COND(size >= 0);
			cb(param);
		}

		if (size != 0) {
			sl_pr_debug("Do prime -1 \n");
			init_one_shot_timer(-1);
		}

	} else {
		sl_pr_debug("############ Keep pooling batch_size = %d\n", batch_size);
		init_one_shot_timer(batch_size);
	}

	/* rcu_read_lock happens automatically inside terminate */
	fdma_terminate_task();
}

int rcu_synchronize(rcu_cb_t *cb, uint64_t param)
{
	int size;

	ASSERT_COND(cb);
	ASSERT_COND(g_rcu.slab);

	size = enqueue(cb, param);
	if (size < 0) {
		sl_pr_err("Failed enqueue err = %d list size = %d \n",
		       size, g_rcu.list_size);
		return -ENOMEM;
	}

	if (size == 1) {
		sl_pr_debug("Do prime -1 \n");
		size = init_one_shot_timer(-1);
		if (size) {
			sl_pr_err("Failed timer err = %d\n", size);
			return size;
		}
	}
	return 0;
}

void rcu_read_unlock()
{
	uint32_t temp;
	uint32_t my_cluster, my_core;
	uint32_t my_task_id;

	my_core		= core_get_id();
	my_cluster	= my_core / AIOP_MAX_NUM_CLUSTERS;
	my_core		= my_core & (AIOP_MAX_NUM_CORES_IN_CLUSTER - 1);
	my_task_id	= (booke_get_TASKSCR0() & 0xF);

	/* Need automatic lock upon task termination 
	 * see RCU_CHECK_UNLOCK_CANCEL */
	g_rcu_unlock = 1;

	lock_spinlock(&g_rcu.sw_ctstws_lock);

	/* 1 - need to wait for task
	 * 0 - no need to wait for task
	 * must be atomic write */
	temp = g_rcu.sw_ctstws[my_cluster][my_core] & \
		~(CTSTWS_TASK0_BIT >> my_task_id);
	g_rcu.sw_ctstws[my_cluster][my_core] = temp;

	unlock_spinlock(&g_rcu.sw_ctstws_lock);
}

void rcu_read_lock()
{
	uint32_t temp;
	uint32_t my_cluster, my_core;
	uint32_t my_task_id;

	my_core		= core_get_id();
	my_cluster	= my_core / AIOP_MAX_NUM_CLUSTERS;
	my_core		= my_core & (AIOP_MAX_NUM_CORES_IN_CLUSTER - 1);
	my_task_id	= (booke_get_TASKSCR0() & 0xF);

	lock_spinlock(&g_rcu.sw_ctstws_lock);

	/* 1 - need to wait for task
	 * 0 - no need to wait for task
	 * must be atomic write */
	temp = g_rcu.sw_ctstws[my_cluster][my_core] | \
		(CTSTWS_TASK0_BIT >> my_task_id);
	g_rcu.sw_ctstws[my_cluster][my_core] = temp;

	unlock_spinlock(&g_rcu.sw_ctstws_lock);
}

#if 0 /* PSEUDOCODE_AFTER_REVIEW */

/*
 *
 * This is pseudo code that was used for design review - ignore it
 *
 */

static void rcu_read_unlock()
{
	SPIN_LOCK(SW_CTSTWS);
	/* 1 - need to wait for task
	 * 0 - no need to wait for task
	 * must be atomic write */
	temp = SW_CTSTWS & ~(1 << task_id);
	SW_CTSTWS = temp;
	SPIN_UNLOCK(SW_CTSTWS);
}

static void rcu_read_unlock_cancel()
{
	SPIN_LOCK(SW_CTSTWS);
	/* 1 - need to wait for task
	 * 0 - no need to wait for task
	 * must be atomic write */
	temp = SW_CTSTWS | (1 << task_id);
	SW_CTSTWS = temp;
	SPIN_UNLOCK(SW_CTSTWS);
}

/************** Implementation based on the TMAN **************
 * Note init_one_shot_timer() can be easily replaced by cmdif_send() */

void rcu_init()
{
	/* rcu_slab is used for rcu_list, it is a static global variable
	 * max and commited are going to be user parameters at compile time
	 * TODO must have slab early init */
	rcu_slab = slab_create(DDR, committed, max);

	/* rcu_list is limited by max of rcu_slab,
	 * it is a static global variable */
	rcu_list = init_list();

	/* one shot timer duration is going to be user parameter
	 * at compile time
	 * TODO No need to reserve TMI for RCU sync, it maybe user parameter */
	tman_create_tmi();

	/* One SW register per one HW register */
	SW_CTSTWS = 0xffffffff;
}

int rcu_synchronize(void (*cb)(uint64_t), uint64_t param)
{
	/* rcu_list is spinlock protected */
	size = enqueue(cb, param);

	if (size == 1)
		err = init_one_shot_timer(tman_cb, -1);

	return err;
}

static void tman_cb(uint64_t ignore, uint16_t batch_size)
{
	rcu_read_unlock();

	if (batch_size == -1) {
		/* rcu_list is spinlock protected */
		batch_size = rcu_list_size();

		/* next prime will be only after this one has finished */
		CTSTWS = 0;
	}

	if ((CTSTWS & SW_CTSTWS) == 0) {

		for (i = 0; i < batch_size; i++) {

			/* rcu_list is spinlock protected */
			(cb, param) = dequeue();
			cb(param);
		}

		if (!empty(rcu_list))
			init_one_shot_timer(tman_cb, -1);
	} else {
		init_one_shot_timer(tman_cb, batch_size);
	}

	rcu_read_unlock_cancel();
	fdma_terminate_task();
}
#endif
