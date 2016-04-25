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

#include "fsl_dbg.h"
#include "fsl_system.h"
#include "fsl_string.h"
#include "fsl_dpbp.h"
#include "fsl_dpni.h"
#include "fsl_cdma.h"
#include "fsl_sl_dprc_drv.h"

int __init();
int __dprc_drv_init();
int __configure_bpids();

extern struct dpni_pools_cfg pools_params__;

static int test_buffer(uint16_t hw_dpbp_id);
int buffer_pool_init();
int buffer_pool_test();

int buffer_pool_init()
{
	int err = 0;

	err = __init();
	if (err)
	{
		return err;
	}
	err = __dprc_drv_init();
	if (err)
	{
		return err;
	}
	memset(&pools_params__, 0, sizeof(struct dpni_pools_cfg));
	return __configure_bpids();
}


/*****************************************************************************/
int buffer_pool_test()
{
	int err = 0;
	unsigned int i;
	uint16_t dpbp = 0;
	struct dpbp_attr attr;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	if (dprc == NULL)
	{
		return -ENODEV;
	}
	if (pools_params__.num_dpbp < 1)
	{
		return -EINVAL;
	}

	for (i = 0; i < pools_params__.num_dpbp; i++)
	{
		if ((err = dpbp_open(&dprc->io, 0, pools_params__.pools[i].dpbp_id, &dpbp)) != 0) {
			pr_err("Failed to open DPBP-%d.\n", pools_params__.pools[i].dpbp_id);
			return err;
		}
		if ((err = dpbp_get_attributes(&dprc->io, 0, dpbp, &attr)) != 0) {
			pr_err("Failed to get attributes from DPBP-%d.\n", pools_params__.pools[i].dpbp_id);
			return err;
		}
		err = test_buffer(attr.bpid);
		if (err) {
			return err;
		}
	}
	return err;
}

static int test_buffer(uint16_t hw_bpid)
{
	uint64_t buff = 0;
	int err = 0;

	err = cdma_acquire_context_memory(hw_bpid, &buff);
	if (err) {
		pr_err("cdma_acquire_context_memory Failed: %d\n",err);
		return err;
	}
	cdma_refcount_increment(buff);

	uint32_t ws_src = 0x12345678;
	uint32_t ws_dst = 0;
	cdma_write(buff, &ws_src, sizeof(uint32_t));
	cdma_read(&ws_dst, buff, sizeof(uint32_t));
	pr_info("Wrote value=0x%08x, read_value=0x%08x\n", ws_src, ws_dst);

	err = cdma_refcount_decrement(buff);
	if (err) {
		return err;
	}
	cdma_release_context_memory(buff);

	if (ws_src != ws_dst) {
		return -EINVAL;
	}
	return err;
}
