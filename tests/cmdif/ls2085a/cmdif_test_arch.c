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

#include "fsl_types.h"
#include "common/fsl_stdio.h"
#include "fsl_platform.h"
#include "fsl_io.h"
#include "general.h"
#include "fsl_dbg.h"
#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "cmdif.h"
#include "cmdif_client.h"
#include "fsl_fdma.h"
#include "fdma.h"
#include "fsl_ldpaa.h"
#include "fsl_icontext.h"
#include "fsl_tman.h"
#include "fsl_malloc.h"
#include "fsl_malloc.h"
#include "fsl_shbp.h"
#include "fsl_spinlock.h"
#include "cmdif_test_common.h"
#include "fsl_dpci_drv.h"
#include "fsl_dpci_mng.h"
#include "fsl_sl_evmng.h"
#include "fsl_dprc.h"
#include "fsl_string.h"
#include "fsl_rcu.h"
#include "rcu.h"


void rcu_test();
int app_early_init(void);
int rcu_test_check();

extern struct rcu g_rcu;
extern int32_t rcu_sync_count;
extern int32_t rcu_cb_count;


static void rcu_sync_cb(uint64_t param)
{
	atomic_incr32(&rcu_cb_count, 1);
	pr_debug("####### param = 0x%x count = %d #######\n",
	         (uint32_t)param, rcu_cb_count);
}

void rcu_test()
{
	int err;
	int i;

	for (i = 0; i < 10; i++) {
		atomic_incr32(&rcu_sync_count, 1);
		pr_debug("####### rcu_synchronize_nb = num %d #######\n",
		         rcu_sync_count);
		err = rcu_synchronize_nb(rcu_sync_cb, 0x7);
		ASSERT_COND(!err);
	}
}

int rcu_test_check()
{
	/* I can't have a good check here because it depends on timer */
	pr_debug("####### RCU test results = count %d == %d #######\n",
	         rcu_sync_count, rcu_cb_count);

	if (rcu_sync_count == rcu_cb_count)
		return 0;

	return -1;
}


int app_early_init(void)
{
	int err = 0;

	err = rcu_early_init(5, 10, 15);
	ASSERT_COND(!err);
	err = rcu_early_init(10, 64, 128);
	ASSERT_COND(!err);
	err = rcu_early_init(20, 64, 128);
	ASSERT_COND(!err);

	ASSERT_COND(g_rcu.committed >= (64 + 64 + 10));
	ASSERT_COND(g_rcu.max >= (128 - 64));
	ASSERT_COND(g_rcu.delay <= 5);

	return 0;

}
