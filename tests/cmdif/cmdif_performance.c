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
#include "apps.h"

extern int app_evm_register();

#ifndef CMDIF_TEST_WITH_MC_SRV
#warning "If you test with MC define CMDIF_TEST_WITH_MC_SRV inside cmdif.h\n"
#warning "If you test with GPP undef CMDIF_TEST_WITH_MC_SRV and delete #error\n"
#endif
#if (DEBUG_LEVEL > 0)
#warning "Set DEBUG_LEVEL to 0 before testing "
#endif
#if (APP_INIT_TASKS_PER_CORE != 1)
#warning "Set APP_INIT_TASKS_PER_CORE to 1 before testing "
#endif

#define TIMEOUT_IN_SECONDS	60

struct cmdif_desc cidesc;
/** Counters ***/
int32_t cmd_count = 0;
/** TMAN ***/
uint8_t timer_on = 0;
uint8_t tmi_id = 0;
uint32_t timer_handle = 0;
uint64_t tman_addr = 0;
volatile uint8_t timer_deleted = 1;
int app_init(void);
void app_free(void);

static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev);
	fsl_os_print("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	fsl_os_print("close_cb\n");
	return 0;
}

__HOT_CODE static void tman_cb(uint64_t opaque1, uint16_t opaque2)
{
	UNUSED(opaque1);
	UNUSED(opaque2);

	timer_on = 0;
	fsl_os_print("TIMEOUT %d seconds, cmd_count = 0x%x \n", 
	        TIMEOUT_IN_SECONDS, cmd_count);	
	/* Confirmation for running the timer again */
	tman_timer_completion_confirmation(timer_handle);
	timer_deleted = 1;
}

__HOT_CODE static int ctrl_cb0(void *dev, uint16_t cmd, 
                               uint32_t size, void *data)
{
	int err;

	UNUSED(dev);
	UNUSED(cmd);
	UNUSED(size);
	UNUSED(data);

	if (cmd == PERF_TEST_START) {
		/*
		 * The sequence for sending commands to AIOP should be
		 * 1. Send low priority no response PERF_TEST_START command,
		 * 2. Send high priority commands,
		 * 3. TIMEOUT - print counter since PERF_TEST_START
		 */
		/* Wait for timer deletion */
		do {} while(!timer_deleted);
		timer_deleted = 0;
		cmd_count     = 0;
		fsl_os_print("Starting TMAN timer ..\n");
		err = tman_create_timer(tmi_id/* tmi_id */,
		                        TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY | TMAN_CREATE_TIMER_ONE_SHOT /* flags */,
		                        TIMEOUT_IN_SECONDS/* duration */,
		                        tmi_id /* opaque_data1 */,
		                        tmi_id /* opaque_data2 */,
		                        tman_cb /* tman_timer_cb */,
		                        &timer_handle /* *timer_handle */);
		timer_on = 1; /* Count tman creation command too */
		if (err) {
			pr_err("TMAN timer create err = %d\n", err);
		}
	}

	if (timer_on) {
		atomic_incr32(&cmd_count, 1);
	}

	return 0;
}


int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	uint64_t buff = 0;
	char       module[10];
	int        i = 0;
	struct cmdif_module_ops ops;

	pr_debug("Running app_init()\n");

	err = app_evm_register();
	ASSERT_COND(!err);

	ops.close_cb = close_cb;
	ops.open_cb = open_cb;
	ops.ctrl_cb = ctrl_cb0; /* TEST0 is used for srv tests*/
	snprintf(module, sizeof(module), "TEST%d", i);
	err = cmdif_register_module(module, &ops);
	if (err) {
		pr_err("FAILED cmdif_register_module %s err = %d\n!",
		       module, err);
		return err;
	}
	fsl_os_print("Registered %s module\n", module);
	err = fsl_os_get_mem(1024, MEM_PART_DP_DDR, 64, &tman_addr);
	if (err || !tman_addr) {
		err = fsl_os_get_mem(1024, MEM_PART_SYSTEM_DDR, 64, &tman_addr);
		if (err || !tman_addr) {
			pr_err("FAILED fsl_os_get_mem err = %d\n!", err);
			return err;
		}
	}

	fsl_os_print("Using TMAN\n");
	err = tman_create_tmi(tman_addr, 10, &tmi_id);
	if (err) {
		pr_err("TMAN tmi create err = %d\n", err);
		return err;
	}

	return err;
}

void app_free(void)
{
	/* TODO - complete!*/
}

int app_early_init(void)
{
	return 0;
}
