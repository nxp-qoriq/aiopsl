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

#include "fsl_rcu.h"
#include "fsl_slab.h"
#include "fsl_tman.h"
#include "fsl_malloc.h"
#include "aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_core_booke.h"

struct slab *g_rcu_slab = NULL;
uint8_t g_sl_tmi_id = 0xff;

int rcu_early_init(void);
int rcu_early_init(void)
{
	int err;
	
	/* TODO max, committed to be taken from app.h */
	err = slab_register_context_buffer_requirements(10,
	                                                30,
	                                                64,
	                                                64,
	                                                MEM_PART_DP_DDR,
	                                                0,
	                                                0);
	ASSERT_COND(!err);
}

int rcu_init();
int rcu_init()
{
	int err;
	uint64_t tman_addr;

	err = slab_create(10,
	                  30,
	                  64,
	                  64,
	                  MEM_PART_DP_DDR,
	                  0,
	                  NULL,
	                  &g_rcu_slab);
	ASSERT_COND(!err);
	
	if (g_sl_tmi_id == 0xff) {
		tman_addr = 0;
		err = fsl_os_get_mem((64 * 16 + 1), 
		                     MEM_PART_DP_DDR, 
		                     64, 
		                     &tman_addr);
		ASSERT_COND(!err && tman_addr);

		err = tman_create_tmi(tman_addr, 10, &g_sl_tmi_id);
		ASSERT_COND(!err);
	}
}

int rcu_synchronize(void (*cb)(uint64_t), uint64_t param)
{
	struct aiop_dcsr_regs *regs = (struct aiop_dcsr_regs *)\
		(AIOP_PERIPHERALS_OFF + SOC_PERIPH_OFF_DCSR);
	int cluster;
	int core;
	uint32_t ctstws;
	uint32_t task_id;
	
	cluster = 1;
	core = 2;
	
	pr_debug("Cluster %d Core %d CTWS 0x%x should be 0x%x\n",
	         cluster + 1,
	         core + 1,
	         (uint32_t)(&regs->clustr[cluster].core[core].ctstws),
	         (0x02000000 + 0x0100000 + 0x90000 + 0x0800 + 0x30C));
	
	ctstws = ioread32_ccsr(&regs->clustr[cluster].core[core].ctstws);
	iowrite32_ccsr(0, &regs->clustr[cluster].core[core].ctstws);
	ctstws = ioread32_ccsr(&regs->clustr[cluster].core[core].ctstws);

	pr_debug("Cluster %d Core %d CTWS val 0x%x\n",
	         cluster + 1,
	         core + 1,
	         ctstws);

	ctstws = booke_get_CTSTWS();
	core = core_get_id();
	cluster = core / 4; 
	core = core & 0x3;
	task_id = (booke_get_TASKSCR0() & 0xF);
	
	iowrite32_ccsr(0, &regs->clustr[cluster].core[core].ctstws);
	ctstws = ioread32_ccsr(&regs->clustr[cluster].core[core].ctstws);

	pr_debug("Cluster %d Core %d CTWS val 0x%x task_id 0x%x\n",
	         cluster + 1,
	         core + 1,
	         ctstws,
	         task_id);

	ASSERT_COND(ctstws == booke_get_CTSTWS());

	return 0;
}

#if 0
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
