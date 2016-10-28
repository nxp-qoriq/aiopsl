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
#include "system.h"

#ifndef CMDIF_TEST_WITH_MC_SRV
#warning "If you test with MC define CMDIF_TEST_WITH_MC_SRV inside cmdif.h\n"
#warning "If you test with GPP undef CMDIF_TEST_WITH_MC_SRV and delete #error\n"
#endif

int app_init(void);
void app_free(void);
extern int app_evm_register();
extern int dprc_drv_scan(void);
extern void rcu_test();
extern int app_early_init(void);
extern int rcu_test_check();

#ifdef CMDIF_TEST_WITH_MC_SRV
#define TEST_DPCI_ID    (0) /* For MC use 0 */
#else
#define TEST_DPCI_ID    (4) /* For GPP use 4 */
#endif

extern struct dpci_mng_tbl g_dpci_tbl;

struct cmdif_desc cidesc;
struct cmdif_desc cidesc_arr[AIOP_CL_REGISTER_NUM];
struct dpci_attr attr = {0};
struct dpci_attr attr_c = {0};
uint64_t tman_addr;
uint64_t lbp;
uint64_t gpp_lbp;
int32_t async_count = 0;
int32_t dpci_add_count = 0;
int32_t dpci_rm_count = 0;
int32_t rcu_sync_count = 0;
int32_t rcu_cb_count = 0;

extern int32_t dpci_add_ev_count;
extern int32_t dpci_rm_ev_count;
extern int32_t dpci_up_ev_count;
extern int32_t dpci_down_ev_count;
extern __PROFILE_SRAM struct storage_profile 
			storage_profile[SP_NUM_OF_STORAGE_PROFILES];

static int dpci_dynamic_rm_test()
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	uint16_t token;
	int err;

	atomic_incr32(&dpci_rm_count, 1);

	/* Take 2 last DPCIs from dpci_dynamic_add_test */
	err = dpci_open(&dprc->io, 0, attr.id, &token);
	ASSERT_COND(!err);

	/* Link down event */
	err = dpci_drv_disable((uint32_t)attr.id);
	ASSERT_COND(!err);

	/*
	 * TODO get back when MC commands will be handled without polling
	 */
	do {
		pr_debug("waiting for link down rm_count = %d"
			"link_down_count = %d\n",
			dpci_rm_count, dpci_down_ev_count);

	} while ((volatile int32_t)dpci_down_ev_count < \
		(volatile int32_t)(dpci_rm_count * 2));

	err = dpci_destroy(&dprc->io, dprc->token, 0, token);
	ASSERT_COND(!err);

	err = dpci_close(&dprc->io, 0, token);
	ASSERT_COND(!err);

	/* Take 2 last DPCIs from dpci_dynamic_add_test */
	err = dpci_open(&dprc->io, 0, attr_c.id, &token);
	ASSERT_COND(!err);

	/* Link down event */
	err = dpci_drv_disable((uint32_t)attr_c.id);
	ASSERT_COND(!err);

	err = dpci_destroy(&dprc->io, dprc->token, 0, token);
	ASSERT_COND(!err);

	err = dpci_close(&dprc->io, 0, token);
	ASSERT_COND(!err);

	err = dprc_drv_scan();
	ASSERT_COND(!err);

	err = dpci_mng_find((uint32_t)attr_c.id);
	ASSERT_COND(err < 0);
	err = dpci_mng_find((uint32_t)attr.id);
	ASSERT_COND(err < 0);

	return 0;
}

static int dpci_dynamic_add_test()
{
	struct dpci_cfg dpci_cfg;
	uint16_t dpci = 0;
	uint32_t dpci_temp_id = 0;
	uint16_t dpci_c = 0;
	uint32_t dpci_temp_id_c = 0;
	struct dprc_endpoint endpoint1 ;
	struct dprc_endpoint endpoint2;
	struct dprc_connection_cfg connection_cfg = { 0 };
	uint8_t p = 0;
	int     err = 0;
	int     link_up = 0;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	pr_debug("Enter\n");

	atomic_incr32(&dpci_add_count, 1);

	memset(&attr, 0, sizeof(attr));
	memset(&attr_c, 0, sizeof(attr_c));

	/* DPCI 1 */
	dpci_cfg.num_of_priorities = 2;

	err = dpci_create(&dprc->io, dprc->token, 0, &dpci_cfg, &dpci_temp_id);
	ASSERT_COND(!err);

	err = dpci_open(&dprc->io, 0, (int)dpci_temp_id, &dpci);
	ASSERT_COND(!err);

	err = dpci_get_attributes(&dprc->io, 0, dpci, &attr);
	ASSERT_COND(!err);
	ASSERT_COND(attr.num_of_priorities == dpci_cfg.num_of_priorities);


	/* DPCI 2 */
	dpci_cfg.num_of_priorities = 2;

	err = dpci_create(&dprc->io, dprc->token, 0, &dpci_cfg, &dpci_temp_id_c);
	ASSERT_COND(!err);

	err = dpci_open(&dprc->io, 0, (int)dpci_temp_id_c, &dpci_c);
	ASSERT_COND(!err);

	err = dpci_get_attributes(&dprc->io, 0, dpci_c, &attr_c);
	ASSERT_COND(!err);
	ASSERT_COND(attr_c.num_of_priorities == dpci_cfg.num_of_priorities);

	/* Connect 2 DPCIs  */
	memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
	memset(&endpoint2, 0, sizeof(struct dprc_endpoint));

	endpoint1.id = attr_c.id;
	endpoint1.if_id = 0;
	strcpy(endpoint1.type, "dpci");

	endpoint2.id = attr.id;
	endpoint2.if_id = 0;
	strcpy(endpoint2.type, "dpci");

	pr_debug("Connect %d to %d\n", attr.id, attr_c.id);

	err = dprc_connect(&dprc->io, 0, dprc->token, &endpoint1, &endpoint2, 
	                   &connection_cfg);
	if (err) {
		pr_err("dprc_connect failed\n");
	}

	err = dpci_close(&dprc->io, 0, dpci);
	ASSERT_COND(!err);

	err = dpci_close(&dprc->io, 0, dpci_c);
	ASSERT_COND(!err);

	err = dprc_drv_scan();
	ASSERT_COND(!err);

	err = dpci_mng_find((uint32_t)endpoint2.id);
	ASSERT_COND(err >= 0);
	ASSERT_COND(g_dpci_tbl.tx_queue[err][0] != DPCI_FQID_NOT_VALID);
	err = dpci_mng_find((uint32_t)endpoint1.id);
	ASSERT_COND(err >= 0);
	ASSERT_COND(g_dpci_tbl.tx_queue[err][0] != DPCI_FQID_NOT_VALID);

	/*
	 * TODO get back when MC commands will be handled without polling
	 */

	do {
		pr_debug("waiting for link up add_count = %d"
			"link_up_count = %d\n",
			dpci_add_count, dpci_up_ev_count);

	} while ((volatile int32_t)dpci_up_ev_count < \
		(volatile int32_t)(dpci_add_count * 2));

	return 0;
}

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
	aiop_ws_check();

	return 0;
}

static void check_counters()
{
	pr_debug("dpci_add_count %d\n", dpci_add_count);
	pr_debug("dpci_rm_count %d\n", dpci_rm_count);
	pr_debug("dpci_add_event_count %d\n", dpci_add_ev_count);
	pr_debug("dpci_rm_event_count %d\n", dpci_rm_ev_count);
	pr_debug("dpci_up_event_count %d\n", dpci_up_ev_count);
	pr_debug("dpci_down_event_count %d\n", dpci_down_ev_count);

	/*
	 * TODO get back when MC commands will be handled without polling
	 */
	ASSERT_COND(dpci_add_count > 0);
	ASSERT_COND(dpci_rm_count > 0);
	ASSERT_COND(dpci_add_ev_count >= (dpci_add_count * 2));
	ASSERT_COND(dpci_rm_ev_count == (dpci_rm_count * 2));
	ASSERT_COND(dpci_up_ev_count >= (dpci_add_count * 2));
	ASSERT_COND(dpci_down_ev_count >= (dpci_rm_count * 2));
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

static int ctrl_cb0(void *dev, uint16_t cmd, uint32_t size,
                              void *data)
{
	int err = 0;
	int i   = 0;
	uint64_t p_data = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	struct icontext ic;
	struct icontext ic_cmd;
	uint16_t dpci_id = 0;
	uint16_t bpid = 3;
	uint8_t tmi_id = 0;
	uint32_t timer_handle = 0;
	struct shbp_test *shbp_test;
	uint64_t temp64;
	char     module[10];
	struct fdma_amq amq;

	UNUSED(dev);
	pr_debug("ctrl_cb0 cmd = 0x%x, size = %d, data  0x%x\n",
	         cmd,
	         size,
	         (uint32_t)data);

	ASSERT_COND(PRC_GET_SEGMENT_HANDLE() == 0);
	ASSERT_COND(PRC_GET_FRAME_HANDLE() == 0);

	aiop_ws_check();

	switch (cmd) {
	case RCU_SYNC_TEST:
		rcu_test();
		break;
	case RCU_SYNC_CHECK:
		err = rcu_test_check();
		break;
	case DPCI_ADD:
		err = dpci_dynamic_add_test();
		/* Done indication for MC*/
		err |= cmdif_send(&cidesc, 0xb | CMDIF_NORESP_CMD, 0,
		                 CMDIF_PRI_HIGH, 0, NULL, NULL);
		break;
	case DPCI_RM:
		err = dpci_dynamic_rm_test();
		check_counters();
		/* Done indication for MC*/
		err |= cmdif_send(&cidesc, 0xb | CMDIF_NORESP_CMD, 0,
		                 CMDIF_PRI_HIGH, 0, NULL, NULL);
		break;
	case SHBP_TEST_GPP:
		pr_debug("Testing GPP SHBP...\n");
		shbp_test = data;
		dpci_id = shbp_test->dpci_id;
		gpp_lbp = CPU_TO_SRV64(shbp_test->shbp);
		ASSERT_COND(gpp_lbp);
		err = icontext_get(dpci_id, &ic);
		ASSERT_COND(!err && (ic.icid != ICONTEXT_INVALID));

		temp64 = shbp_acquire(gpp_lbp, &ic);
		ASSERT_COND(temp64 == 0);
		shbp_test->shbp = 0; /* For test on GPP */

		/* gpp_lbp is taken from metadata
		 * p_data is presented in presentation as data */
		err = shbp_release(gpp_lbp, p_data, &ic);
		ASSERT_COND(!err);

		fdma_modify_default_segment_data(0, (uint16_t)PRC_GET_SEGMENT_LENGTH());
		pr_debug("Released buffer into GPP SHBP\n");
		break;
	case SHBP_TEST:
		shbp_test = data;
		dpci_id = shbp_test->dpci_id;
		lbp = CPU_TO_SRV64(shbp_test->shbp);
		ASSERT_COND(lbp);
		err = icontext_get(dpci_id, &ic);
		ASSERT_COND(!err && (ic.icid != ICONTEXT_INVALID));
		p_data = shbp_acquire(lbp, &ic);
		while (p_data != 0) {
			i++;
			err = shbp_release(lbp, p_data, &ic);
			ASSERT_COND(!err);
			p_data = shbp_acquire(lbp, &ic);
		}
		pr_debug("Acquired and released %d buffers from SHBP\n", i);
		break;
	case SHBP_TEST_AIOP:
		shbp_test = data;
		dpci_id = shbp_test->dpci_id;
		lbp = CPU_TO_SRV64(shbp_test->shbp);
		ASSERT_COND(lbp);
		err = icontext_get(dpci_id, &ic);
		ASSERT_COND(!err && (ic.icid != ICONTEXT_INVALID));
		p_data = shbp_acquire(lbp, &ic);
		pr_debug("Acquired %d buffers from SHBP, sending no response command\n", 1);
		icontext_dma_write(&ic, sizeof(struct shbp_test), shbp_test, p_data);
		err = cmdif_send(&cidesc, 0xa | CMDIF_NORESP_CMD, size,
		                 CMDIF_PRI_LOW, p_data, NULL, cidesc.regs);
		break;
	case TMAN_TEST:
		err |= tman_create_tmi(tman_addr /* uint64_t tmi_mem_base_addr */,
		                       10 /* max_num_of_timers */,
		                       &tmi_id/* tmi_id */);
		err |= tman_create_timer(tmi_id/* tmi_id */,
		                         TMAN_CREATE_TIMER_MODE_USEC_GRANULARITY | TMAN_CREATE_TIMER_ONE_SHOT /* flags */,
		                         12/* duration */,
		                         0x12345 /* opaque_data1 */,
		                         0x1616 /* opaque_data2 */,
		                         verif_tman_cb /* tman_timer_cb */,
		                         &timer_handle /* *timer_handle */);
		pr_debug("TMAN timer created err = %d\n", err);
		break;
	case OPEN_CMD:
		cidesc.regs = (void *)TEST_DPCI_ID;
		/* GPP will send DPCI id at the first byte of the data */
		if (size > 0) {
			cidesc.regs = (void *)(((uint8_t *)data)[0]);
		}
		pr_debug("Testing AIOP client against GPP DPCI%d\n",
		         (uint32_t)cidesc.regs);
		err = cmdif_open(&cidesc, "IRA", 0, NULL, 0);
		break;
	case CLOSE_CMD:
		err = cmdif_close(&cidesc);
		check_counters();
		break;
	case OPEN_N_CMD:
		dpci_id = (uint16_t)(TEST_DPCI_ID);
		if (size > 0) {
			dpci_id = (((uint8_t *)data)[0]);
		}
		pr_debug("Testing AIOP client against GPP DPCI%d\n", dpci_id);
		for (i = 0; i < AIOP_CL_REGISTER_NUM; i++) {
			cidesc_arr[i].regs = (void *)dpci_id;
			snprintf(module, sizeof(module), "IRA%d", i);
			err |= cmdif_open(&cidesc_arr[i], module, 0, NULL, 0);
			if (err) {
				pr_debug("failed to cmdif_open(%s)\n", module);
				return err;
			}
		}
		break;
	case NORESP_CMD:
		err = cmdif_send(&cidesc, 0xa | CMDIF_NORESP_CMD, size,
		                 CMDIF_PRI_LOW, p_data, aiop_async_cb, cidesc.regs);
		break;
	case ASYNC_CMD:
		p_data += size;
		pr_debug("AIOP is sending asynchronous command with the "
			"following parameters\n");
		pr_debug("Addr high = 0x%x low = 0x%x size = 0x%x\n",
		         (uint32_t)((p_data & 0xFF00000000) >> 32),
		         (uint32_t)(p_data & 0xFFFFFFFF),
		         AIOP_SYNC_BUFF_SIZE);
		err = cmdif_send(&cidesc, 0xa | CMDIF_ASYNC_CMD, AIOP_SYNC_BUFF_SIZE,
		                 CMDIF_PRI_LOW, p_data, aiop_async_cb, cidesc.regs);
		break;
	case ASYNC_N_CMD:
		p_data += size;
		pr_debug("AIOP is sending asynchronous command with the "
			"following parameters\n");
		pr_debug("Addr high = 0x%x low = 0x%x size = 0x%x\n",
		         (uint32_t)((p_data & 0xFF00000000) >> 32),
		         (uint32_t)(p_data & 0xFFFFFFFF),
		         AIOP_SYNC_BUFF_SIZE);
		ASSERT_COND(AIOP_ASYNC_N_NUM <= AIOP_CL_REGISTER_NUM);
		for (i = 0 ; i < AIOP_ASYNC_N_NUM; i++) {
			err |= cmdif_send(&cidesc_arr[i], (uint16_t)(i + 1) | CMDIF_ASYNC_CMD, AIOP_SYNC_BUFF_SIZE,
			                  CMDIF_PRI_LOW, p_data, aiop_async_cb, cidesc_arr[i].regs);
			ASSERT_COND(!err);
		}
		break;
	case SYNC_CMD:
		err = cmdif_send(&cidesc, 0xa, size, CMDIF_PRI_LOW, p_data,
		                 aiop_async_cb, cidesc.regs);
		break;
	case IC_TEST:
		ASSERT_COND(size >= sizeof(struct icontext));
		dpci_id = (((uint8_t *)data)[0]);
		bpid =  (uint16_t)(((uint8_t *)data)[1]);
		pr_debug("Isolation context test dpci %d bpid %d:\n", dpci_id, bpid);

		err = icontext_get(dpci_id, &ic);
		ASSERT_COND(err == 0);

		pr_debug("ICID %d:\n dma flags 0x%x \n bdi flags 0x%x \n",
		         ic.icid,
		         ic.dma_flags,
		         ic.bdi_flags);
		icontext_dma_write(&ic, sizeof(struct icontext), &ic, p_data);
		icontext_dma_read(&ic, sizeof(struct icontext), p_data, data);
		ASSERT_COND(ic.icid == ((struct icontext *)data)->icid);
		ASSERT_COND(ic.dma_flags == ((struct icontext *)data)->dma_flags);
		ASSERT_COND(ic.bdi_flags == ((struct icontext *)data)->bdi_flags);

		/* Note: MC and AIOP have the same AMQ and BDI settings */
		p_data = NULL;
		err = icontext_acquire(&ic, bpid, &p_data);
		if (!err) {
			ASSERT_COND(p_data != 0);
			err = icontext_release(&ic, bpid, p_data);
			ASSERT_COND(err == 0);
			pr_debug("Addr high = 0x%x low = 0x%x \n",
			         (uint32_t)((p_data & 0xFF00000000) >> 32),
			         (uint32_t)(p_data & 0xFFFFFFFF));
		} else {
			pr_debug("FAILED icontext_acquire BPID"
				" 0x%x is empty\n", bpid);
		}
		/* Must be after icontext_get(&ic) */
		icontext_cmd_get(&ic_cmd);

		ASSERT_COND(ic_cmd.icid != ICONTEXT_INVALID);
		ASSERT_COND(ic_cmd.icid == ic.icid);
		ASSERT_COND(ic_cmd.bdi_flags == ic.bdi_flags);
		ASSERT_COND(ic_cmd.dma_flags == ic.dma_flags);
		pr_debug("PASSED icontext_cmd_get\n");

		icontext_aiop_get(&ic);
		ASSERT_COND(ic.dma_flags);
		ASSERT_COND(ic.bdi_flags);
		ASSERT_COND(ic.icid != ic_cmd.icid);
		pr_debug("AIOP ICID = 0x%x bdi flags = 0x%x\n", ic.icid, \
		         ic.bdi_flags);
		pr_debug("AIOP ICID = 0x%x dma flags = 0x%x\n", ic.icid, \
		         ic.dma_flags);

		icontext_ws_set(&ic);
		get_default_amq_attributes(&amq);
		ASSERT_COND(ic.icid == amq.icid);

		fsl_print("Storage profile dump, must be all zeros\n");
		mem_disp((uint8_t *)&storage_profile[AIOP_SPID_CMDIF], sizeof(storage_profile[0]));

		break;
	default:
		if ((size > 0) && (data != NULL)) {

			/* check bringup test */
			icontext_cmd_get(&ic_cmd);
			/* modify from presentation */
			for (i = 0; i < MIN(size, 64); i++) {
				((uint8_t *)data)[i] = 0xDA;
			}
		}
		fdma_modify_default_segment_data(0, (uint16_t)PRC_GET_SEGMENT_LENGTH());
		break;
	}

	ASSERT_COND(PRC_GET_SEGMENT_HANDLE() == 0); /* WS check */
	ASSERT_COND(PRC_GET_FRAME_HANDLE() == 0); /* WS check */

	return err;
}

static int app_dpci_test()
{
	uint8_t flags = DPCI_DRV_EP_SERVER;
	struct ep_init_presentation init_presentation1 = {0};
	struct ep_init_presentation init_presentation2 = {0};
	int err = 0;

	init_presentation1.options =
		(EP_INIT_PRESENTATION_OPT_PTA	|
			EP_INIT_PRESENTATION_OPT_ASAPA	|
			EP_INIT_PRESENTATION_OPT_ASAPO	|
			EP_INIT_PRESENTATION_OPT_ASAPS	|
			EP_INIT_PRESENTATION_OPT_SPA	|
			EP_INIT_PRESENTATION_OPT_SPS	|
			EP_INIT_PRESENTATION_OPT_SPO	|
			EP_INIT_PRESENTATION_OPT_SR	|
			EP_INIT_PRESENTATION_OPT_NDS);
	init_presentation2.options = init_presentation1.options;

	err = dpci_drv_get_initial_presentation(flags, &init_presentation1);
	ASSERT_COND(!err);
	err = dpci_drv_set_initial_presentation(flags, &init_presentation1);
	ASSERT_COND(!err);
	err = dpci_drv_get_initial_presentation(flags, &init_presentation2);
	ASSERT_COND(!err);

	if (init_presentation1.adpca != init_presentation2.adpca)
		err |= 1;
	if (init_presentation1.asapa != init_presentation2.asapa)
		err |= 2;
	if (init_presentation1.asapo != init_presentation2.asapo)
		err |= 4;
	if (init_presentation1.asaps != init_presentation2.asaps)
		err |= 8;
	if (init_presentation1.fdpa != init_presentation2.fdpa)
		err |= 16;
	if (init_presentation1.nds != init_presentation2.nds)
		err |= 32;
	if (init_presentation1.ptapa != init_presentation2.ptapa)
		err |= 64;
	if (init_presentation1.spa != init_presentation2.spa)
		err |= 128;
	if (init_presentation1.spo != init_presentation2.spo)
		err |= 256;
	if (init_presentation1.sps != init_presentation2.sps)
		err |= 512;
	if (init_presentation1.sr != init_presentation2.sr)
		err |= 1024;

	if (err) {
		pr_err("server get and set ep err = %d\n");
	}

	flags = DPCI_DRV_EP_CLIENT;
	err |= dpci_drv_get_initial_presentation(flags, &init_presentation2);
	if (init_presentation1.adpca != init_presentation2.adpca)
		err |= 1;
	if (init_presentation1.asapa != init_presentation2.asapa)
		err |= 2;
	if (init_presentation1.asapo != init_presentation2.asapo)
		err |= 4;
	if (init_presentation1.asaps != init_presentation2.asaps)
		err |= 8;
	if (init_presentation1.fdpa != init_presentation2.fdpa)
		err |= 16;
	if (init_presentation1.nds != init_presentation2.nds)
		err |= 32;
	if (init_presentation1.ptapa != init_presentation2.ptapa)
		err |= 64;
	if (init_presentation1.spa != init_presentation2.spa)
		err |= 128;
	if (init_presentation1.spo != init_presentation2.spo)
		err |= 256;
	if (init_presentation1.sps != init_presentation2.sps)
		err |= 512;
	if (init_presentation1.sr != init_presentation2.sr)
		err |= 1024;

	if (err) {
		pr_err("client and server ep have differences err = %d\n");
	}

	return err;
}

#if 0
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
#endif

int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	uint64_t   buff = 0;
	char       module[10];
	int        i = 0;
	struct cmdif_module_ops ops;

	pr_debug("Running app_init()\n");

#ifdef CMDIF_SET_CONCURRENT_MODE
	dpci_drv_set_concurrent(DPCI_DRV_EP_SERVER);
	dpci_drv_set_concurrent(DPCI_DRV_EP_CLIENT);
#endif

	err = app_evm_register();
	ASSERT_COND(!err);

	for (i = 0; i < 20; i++) {
		ops.close_cb = close_cb;
		ops.open_cb = open_cb;
		if (i == 0)
			ops.ctrl_cb = ctrl_cb0; /* TEST0 is used for srv tests*/
		else
			ops.ctrl_cb = ctrl_cb0;
		snprintf(module, sizeof(module), "TEST%d", i);
		err = cmdif_register_module(module, &ops);
		if (err) {
			pr_err("FAILED cmdif_register_module %s\n!",
			             module);
			return err;
		}
	}

	err = fsl_get_mem(1024, MEM_PART_DP_DDR, 64, &tman_addr);
	if (err || (tman_addr == 0)) {
		err = fsl_get_mem(1024, MEM_PART_SYSTEM_DDR, 64, &tman_addr);
		ASSERT_COND(!err && tman_addr);
	}

	/*
	 * Remove it because it is no longer part of DPCI testing
	 * TODO when enabling, it cause issues in MC server cmdif test during
	 * cmdif_open()
	 */
//#ifdef CMDIF_TEST_WITH_MC_SRV
//	mc_intr_set(g_dpci_tbl.mc_dpci_id);
//#endif

	err = app_dpci_test();
	ASSERT_COND(!err);

#if 0
#ifdef CMDIF_TEST_WITH_MC_SRV
		err = dpci_dynamic_add_test();
		ASSERT_COND(!err);
#endif
#endif

	return err;
}

void app_free(void)
{
	/* TODO - complete!*/
}
