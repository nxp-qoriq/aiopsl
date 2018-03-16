/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __RCU_H
#define __RCU_H

#include "fsl_rcu.h"
#include "fsl_slab.h"
#include "fsl_aiop_common.h"
#include "fsl_malloc.h"

#define CTSTWS_TASK0_BIT	0x80000000
#define CTSTWS_TASKS_MASK	0xFFFF0000

#define RCU_DEFAULT_COMMITTED	128
#define RCU_DEFAULT_DELAY	10

#define RCU_MUTEX_R_TAKE \
	do { \
		cdma_mutex_lock_take((uint64_t)(&g_rcu), CDMA_MUTEX_READ_LOCK); \
	} while(0)

#define RCU_MUTEX_W_TAKE \
	do { \
		cdma_mutex_lock_take((uint64_t)(&g_rcu), CDMA_MUTEX_WRITE_LOCK); \
	} while(0)

#define RCU_MUTEX_RELEASE \
	do { \
		cdma_mutex_lock_release((uint64_t)(&g_rcu)); \
	} while(0)


struct rcu {
	uint64_t list_head;		/**< RCU list is palced at DP DDR */
	uint64_t list_tail;		/**< RCU list is palced at DP DDR */
	uint32_t sw_ctstws[AIOP_MAX_NUM_CLUSTERS][AIOP_MAX_NUM_CORES_IN_CLUSTER];
	int32_t list_size;
	struct slab *slab;
	struct aiop_dcsr_regs *regs;
	e_memory_partition_id mem_heap;
	uint32_t committed;	/**< Committed number of accumulated rcu jobs */
	uint32_t max;		/**< Maximal number of accumulated rcu jobs */
	uint16_t delay;
	/**< TODO : Do we need 2 different delays ? see init_one_shot_timer() */
	uint8_t sw_ctstws_lock;
};

struct rcu_job {
	uint64_t next;
	uint64_t param;
	rcu_cb_t *cb;
};

#endif /* __RCU_H */
