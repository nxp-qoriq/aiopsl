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
#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_core_booke.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_smp.h"
#ifndef BRINGUP_TEST
#include "fsl_dbg.h"
#endif
#include "sys.h"

/* For bringup tests compile with BRINGUP_TEST */

#define WAITING_TIMEOUT		0x10000000
#define CORE_ID_GET		(get_cpu_id() >> 4)

uint32_t core_arr[INTG_MAX_NUM_OF_CORES] = {0};
int8_t  counter = 0;
int     err[INTG_MAX_NUM_OF_CORES] = {0};
extern t_system sys; /* Global System Object */

static void _sys_barrier(void)
{
    uint64_t core_mask = (uint64_t)(1 << CORE_ID_GET);

    lock_spinlock(&(sys.barrier_lock));
    /* Mark this core's presence */
    sys.barrier_mask &= ~(core_mask);

    if (sys.barrier_mask)
    {
        unlock_spinlock(&(sys.barrier_lock));
        /* Wait until barrier is reset */
        while (!(sys.barrier_mask & core_mask)) {}
    }
    else
    {
        /* Last core to arrive - reset the barrier */
        sys.barrier_mask = sys.active_cores_mask;
        unlock_spinlock(&(sys.barrier_lock));
    }
}

static void cleanup()
{

#ifdef BRINGUP_TEST
	_sys_barrier();
#endif

	if (sys.is_tile_master[CORE_ID_GET]) {
		counter = 0;
	}

#ifdef BRINGUP_TEST
	_sys_barrier();
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

static int multi_core_test(uint8_t num_cores)
{
	uint32_t core_id =  CORE_ID_GET;
	int      i;
	int      t = 0;

	if (core_id < num_cores) {

		core_arr[core_id] = 0xff;
		err[core_id] = 0x0;

		/* Check smp.c API */
		if (sys_get_num_of_cores() < num_cores) {
			err[core_id] |= 4;
		}

		/* Check fsl_spinlock.h API */
		atomic_incr8(&counter, 1);

		/* Check smp.c API */
		err[core_id] |= single_core_test(core_id);

		do {
			t++;
		} while((counter < num_cores) && (t < WAITING_TIMEOUT));

		if (counter < num_cores)
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
	return multi_core_test(4);
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
	return multi_core_test(16);
#endif
}
