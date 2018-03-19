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

#include "common/fsl_string.h"
#include "fsl_sys.h"
#include "fsl_dbg.h"
#include "apps.h"
#include "fsl_dprc.h"
#include "fsl_dpci_drv.h"
#include "fsl_dpci_event.h"
#include "fsl_dpci_mng.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_evmng.h"
#include "fsl_dpni_drv.h"
#include "fsl_string.h"
#include "fsl_icontext.h"
#include "fsl_spinlock.h"

int app_evm_register();

extern struct dpci_mng_tbl g_dpci_tbl;
extern struct icontext icontext_aiop;
int64_t dpci_add_ev_count __attribute__((aligned(8))) = 0;
int64_t dpci_rm_ev_count __attribute__((aligned(8))) = 0;
int64_t dpci_up_ev_count __attribute__((aligned(8))) = 0;
int64_t dpci_down_ev_count __attribute__((aligned(8))) = 0;

extern int app_init(void); extern void app_free(void);
extern int app_early_init(void);

#define APPS                            	\
{                                       	\
	{app_early_init, app_init, app_free},	\
	{NULL, NULL, NULL} /* never remove! */	\
}

void build_apps_array(struct sys_module_desc *apps);

void build_apps_array(struct sys_module_desc *apps)
{
	struct sys_module_desc apps_tmp[] = APPS;

	ASSERT_COND(ARRAY_SIZE(apps_tmp) <= APP_INIT_APP_MAX_NUM);
	memcpy(apps, apps_tmp, sizeof(apps_tmp));
}

static int app_evmng_cb(uint8_t generator_id, uint8_t event_id,
                    uint64_t app_ctx, void *event_data)
{
	int err = 0;
	int up = 0;

	UNUSED(generator_id);
	UNUSED(app_ctx);

	pr_debug("Event 0x%x event_data 0x%x\n", event_id, (uint32_t)event_data);
	switch (event_id) {
	case DPNI_EVENT_ADDED:
		pr_debug("************DPNI_EVENT_ADDED************\n");
		break;
	case DPNI_EVENT_REMOVED:
		pr_debug("************DPNI_EVENT_REMOVED************\n");
		break;
	case DPNI_EVENT_LINK_DOWN:
		pr_debug("************DPNI_EVENT_LINK_DOWN************\n");
		break;
	case DPNI_EVENT_LINK_UP:
		pr_debug("************DPNI_EVENT_LINK_UP************\n");
		break;
	case DPCI_EVENT_ADDED:
		pr_debug("************DPCI_EVENT_ADDED************\n");
		pr_debug("Before enable DP-CI%d\n", (int)event_data);
		err |= dpci_drv_enable((uint32_t)event_data);
		err |= dpci_drv_linkup((uint32_t)event_data, &up);
		pr_debug("DPCI link state is %d\n", up);
		atomic_incr64(&dpci_add_ev_count, 1);
		break;
	case DPCI_EVENT_REMOVED:
		pr_debug("************DPCI_EVENT_REMOVED************\n");
		atomic_incr64(&dpci_rm_ev_count, 1);
		break;
	case DPCI_EVENT_LINK_DOWN:
		pr_debug("************DPCI_EVENT_LINK_DOWN************\n");
		atomic_incr64(&dpci_down_ev_count, 1);
		break;
	case DPCI_EVENT_LINK_UP:
		pr_debug("************DPCI_EVENT_LINK_UP************\n");
		atomic_incr64(&dpci_up_ev_count, 1);
		break;
	default:
		pr_err("************Unknown event id 0x%x************\n", event_id);
		err = -EINVAL;
		break;
	}

	pr_debug("Event done err = %d\n", err);
	return err;
}

int app_evm_register()
{
	int err = 0;
	uint8_t i = 0;

	for (i = DPNI_EVENT_ADDED; i < NUM_OF_SL_DEFINED_EVENTS; i++) {
		err = evmng_register(EVMNG_GENERATOR_AIOPSL,
		                     i,
		                     1,
		                     (uint64_t)NULL, app_evmng_cb);
		if (err){
			pr_err("EVM registration event %d failed: %d\n", i, err);
			return err;
		}
	}
	return 0;
}
