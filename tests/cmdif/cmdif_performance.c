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

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "platform.h"
#include "fsl_io.h"
#include "general.h"
#include "fsl_dbg.h"
#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "cmdif.h"
#include "cmdif_client.h"
#include "fsl_fdma.h"
#include "fdma.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_icontext.h"
#include "fsl_tman.h"
#include "fsl_malloc.h"
#include "fsl_platform.h"
#include "fsl_shbp.h"
#include "fsl_spinlock.h"

#define CMDIF_PERF_COUNT
#include "cmdif_test_common.h"

#ifndef CMDIF_TEST_WITH_MC_SRV
#warning "If you test with MC define CMDIF_TEST_WITH_MC_SRV inside cmdif.h\n"
#warning "If you test with GPP undef CMDIF_TEST_WITH_MC_SRV and delete #error\n"
#endif

int app_init(void);
void app_free(void);
extern int gpp_sys_ddr_init();
extern int gpp_ddr_check(struct icontext *ic, uint64_t iova, uint16_t size);


#ifdef CMDIF_TEST_WITH_MC_SRV
#define TEST_DPCI_ID    (void *)0 /* For MC use 0 */
#else
#define TEST_DPCI_ID    (void *)4 /* For GPP use 4 */
#endif

struct cmdif_desc cidesc;
uint64_t tman_addr;
uint64_t lbp;
uint64_t gpp_lbp;
int32_t async_count = 0;

static void aiop_ws_check()
{
	struct icontext ic;
	uint16_t pl_icid = PL_ICID_GET;

	icontext_aiop_get(&ic);
	ASSERT_COND(ICID_GET(pl_icid) == ic.icid);

	if (ic.bdi_flags) {
		ASSERT_COND(BDI_GET);
	}
	if (ic.dma_flags & FDMA_DMA_PL_BIT) {
		ASSERT_COND(PL_GET(pl_icid));
	}
	if (ic.dma_flags & FDMA_DMA_eVA_BIT) {
		ASSERT_COND(VA_GET);
	}
	pr_debug("ICID in WS is 0x%x\n", ic.icid);
}

static int aiop_async_cb(void *async_ctx, int err, uint16_t cmd_id,
             uint32_t size, void *data)
{
	UNUSED(cmd_id);
	UNUSED(async_ctx);

	aiop_ws_check();

	pr_debug("ASYNC CB data 0x%x size = 0x%x\n", (uint32_t)data , size);
	pr_debug("Default segment handle = 0x%x size = 0x%x\n",
	             PRC_GET_SEGMENT_HANDLE(), PRC_GET_SEGMENT_LENGTH());
	ASSERT_COND(PRC_GET_SEGMENT_HANDLE() == 0);
	ASSERT_COND(PRC_GET_FRAME_HANDLE() == 0);

	if (err != 0) {
		pr_err("ERROR inside aiop_async_cb\n");
	}
	if ((size > 0) && (data != NULL)) {
#ifdef CMDIF_TEST_WITH_MC_SRV
		pr_debug("Setting first byte of data with val = 0x%x\n",
		             AIOP_ASYNC_CB_DONE);
		pr_debug("Default segment handle = 0x%x\n",
		             PRC_GET_SEGMENT_HANDLE());
		((uint8_t *)data)[0] = AIOP_ASYNC_CB_DONE;
		pr_debug("Default segment handle = 0x%x\n",
		             PRC_GET_SEGMENT_HANDLE());
		fdma_modify_default_segment_data(0, (uint16_t)PRC_GET_SEGMENT_LENGTH());
#endif
	} else {
		pr_debug("No data inside aiop_async_cb\n");
	}
	atomic_incr32(&async_count, 1);
	pr_debug("PASSED AIOP ASYNC CB[%d] cmd_id = 0x%x\n",
	         async_count, cmd_id);

	return err;
}

static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev);
	pr_debug("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	pr_debug("close_cb\n");
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size,
                              void *data)
{
	int err = 0;

	UNUSED(dev);
	pr_debug("ctrl_cb cmd = 0x%x, size = %d, data 0x%x\n",
	             cmd,
	             size,
	             (uint32_t)data);
	/*
	 * TODO add more test scenarios for AIOP server
	 * 1. async response with error
	 * 2. high low priority, high must be served before low
	 * 3. verify data modified by server & client
	 * TODO add more test scenarios for AIOP client
	 * 1. verify data modified by server & client
	 * */
	return 0;
}

static void verif_tman_cb(uint64_t opaque1, uint16_t opaque2)
{
	pr_debug("Inside verif_tman_cb \n");
	ASSERT_COND(opaque1 == 0x12345);
	ASSERT_COND(opaque2 == 0x1616);
	pr_debug("PASSED verif_tman_cb \n");
}

#if (DEBUG_LEVEL > 0)
#warning "Set DEBUG_LEVEL to 0 before testing "
#endif

int32_t cmd_count = 0;
uint8_t timeout_done = 0;

static void perf_tman_cb(uint64_t opaque1, uint16_t opaque2)
{
	timeout_done = 1;
	fsl_os_print("TIMEOUT cmd_count = 0x%x \n", cmd_count);
}

static int ctrl_cb0(void *dev, uint16_t cmd, uint32_t size,
                              void *data)
{
	int err;
	uint8_t tmi_id = 0;
	uint32_t timer_handle = 0;

	/* Don't count tman creation command */
	if (cmd == TMAN_TEST) {
		
		fsl_os_print("Using TMAN\n");

		err = tman_create_tmi(tman_addr /* uint64_t tmi_mem_base_addr */,
		                       10 /* max_num_of_timers */,
		                       &tmi_id/* tmi_id */);
		if (err) {
			pr_err("TMAN tmi create err = %d\n", err);
		}
		
		fsl_os_print("Starting TMAN timer ..\n");
		err = tman_create_timer(tmi_id/* tmi_id */,
		                        TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY | TMAN_CREATE_TIMER_ONE_SHOT /* flags */,
		                        60/* duration */,
		                        tmi_id /* opaque_data1 */,
		                        tmi_id /* opaque_data2 */,
		                        perf_tman_cb /* tman_timer_cb */,
		                        &timer_handle /* *timer_handle */);
		if (err) {
			pr_err("TMAN timer create err = %d\n", err);
		}
		
		return 0;
	}
	
	if (!timeout_done) {
		atomic_incr32(&cmd_count, 1);
	}
	
	return 0;	
}


int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	dma_addr_t buff = 0;
	char       module[10];
	int        i = 0;
	struct cmdif_module_ops ops;

	pr_debug("Running app_init()\n");

	for (i = 0; i < 20; i++) {
		ops.close_cb = close_cb;
		ops.open_cb = open_cb;
		if (i == 0)
			ops.ctrl_cb = ctrl_cb0; /* TEST0 is used for srv tests*/
		else
			ops.ctrl_cb = ctrl_cb;
		snprintf(module, sizeof(module), "TEST%d", i);
		err = cmdif_register_module(module, &ops);
		if (err) {
			pr_err("FAILED cmdif_register_module %s\n!",
			             module);
			return err;
		}
	}

	err = fsl_os_get_mem(1024, MEM_PART_DP_DDR, 64, &tman_addr);
	ASSERT_COND(!err && tman_addr);

	err = gpp_sys_ddr_init();
	return err;
}

void app_free(void)
{
	/* TODO - complete!*/
}
