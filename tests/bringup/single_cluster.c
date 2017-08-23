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

#define BRINGUP_TEST

#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_spinlock.h"
#include "fsl_smp.h"
#ifndef BRINGUP_TEST
#include "fsl_dbg.h"
#endif


/* For bringup tests compile with BRINGUP_TEST */

#define WAITING_TIMEOUT		0x10000000
#define CORE_ID_GET		(get_cpu_id() >> 4)

static uint32_t core_arr[INTG_MAX_NUM_OF_CORES] = {0};
static int64_t  counter __attribute__((aligned(8))) = 0;
static int     err[INTG_MAX_NUM_OF_CORES] = {0};

static void cleanup()
{

#ifdef BRINGUP_TEST
	sys_barrier();
#endif

	if (sys_is_master_core()) {
		counter = 0;
	}

#ifdef BRINGUP_TEST
	sys_barrier();
#endif

}

static int single_core_test(uint32_t test_core_id)
{
	uint32_t core_id = CORE_ID_GET;

	if (core_id == test_core_id) {
		if (!sys_is_core_active(core_id))
			return 1;

		core_arr[core_id] = core_id;
		return 0;
	}

	return 2;
}

static int multi_core_test(int8_t num_cores)
{
	uint32_t core_id =  CORE_ID_GET;
	int      i;
	int      t = 0;
	volatile int64_t* counter_ptr;

	if (core_id < num_cores) {
		core_arr[core_id] = 0xff;
		err[core_id] = 0x0;

		/* Check smp.c API */
		if (sys_get_num_of_cores() < num_cores) {
			err[core_id] |= 4;
		}

		/* Check smp.c API */
		err[core_id] |= single_core_test(core_id);

		/* Check fsl_spinlock.h API */
		atomic_incr64(&counter, 1);

		counter_ptr = &counter;

		while (*counter_ptr < num_cores) {
			t++;
			if (t >= WAITING_TIMEOUT) {
				break;
			}
		}

		if (*counter_ptr < num_cores)
			err[core_id] |= 16;

		/* Check that all cores get here use CORE_ID */
		for (i = 0; i < num_cores; i++) {
			if (core_arr[i] != i) {
				err[core_id] |= 8;
				break;
			}
		}

		t = 0;
		for (i = 0; i < num_cores; i++) {
			t |= err[i];
		}
	}

	return t;
}

int single_cluster_test();
int single_cluster_test()
{
#ifndef BRINGUP_TEST
	return multi_core_test(1);
#else
	return multi_core_test(TEST__CLUSTER_CORE_NO);
#endif
}

int multi_cluster_test();
int multi_cluster_test()
{
	/* Blocking & testing barrier & locks */
	cleanup();

#ifndef BRINGUP_TEST
	return multi_core_test(1);
#else
	return multi_core_test(TEST__TOTAL_CORE_NO);
#endif
}
