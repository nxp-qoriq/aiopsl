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

#include "fsl_dbg.h"
#include "fsl_platform.h"
#include "fsl_rcu.h"
#include "fsl_spinlock.h"

void rcu_test();
int app_early_init(void);
int rcu_test_check();

extern int64_t rcu_sync_count;
extern int64_t rcu_cb_count;

#define TEST_ITER	10

void rcu_test()
{
	int err;
	int i;

	atomic_incr64(&rcu_cb_count, 1);

	for (i = 0; i < TEST_ITER; i++) {
		pr_debug("####### rcu_synchronize = num %d #######\n",
		         rcu_sync_count);

		/* Unlock can be called even if there was no lock */
		rcu_read_unlock();
		rcu_read_unlock();
		rcu_read_unlock();
		
		/* Multi lock is the same as single lock */
		rcu_read_lock();
		rcu_read_lock();
		rcu_read_lock();

		/* The lock is released inside rcu_synchronize */
		err = rcu_synchronize();
		atomic_incr64(&rcu_sync_count, 1);
		ASSERT_COND(!err);
	}
}

int rcu_test_check()
{
	/* I can't have a good check here because it depends on timer */
	pr_debug("####### RCU test results = count %d #######\n",
	         rcu_sync_count);

	/* The lock is released inside rcu_synchronize */
	rcu_synchronize();
	rcu_synchronize();
	rcu_synchronize();
	rcu_synchronize();

	if (rcu_sync_count == (TEST_ITER * rcu_cb_count))
		return 0;

	return -1;
}



int app_early_init(void)
{
	return 0;
}
