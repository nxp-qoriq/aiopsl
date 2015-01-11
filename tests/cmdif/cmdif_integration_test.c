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
#include "cmdif_client_aiop.h"
#include "fsl_fdma.h"
#include "fdma.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_icontext.h"
#include "fsl_tman.h"
#include "fsl_malloc.h"
#include "fsl_platform.h"
#include "fsl_shbp_aiop.h"

#ifndef CMDIF_TEST_WITH_MC_SRV
#warning "If you test with MC define CMDIF_TEST_WITH_MC_SRV inside cmdif.h\n"
#warning "If you test with GPP undef CMDIF_TEST_WITH_MC_SRV and delete #error\n"
#endif

int app_init(void);
void app_free(void);


#define OPEN_CMD	0x100
#define NORESP_CMD	0x101
#define ASYNC_CMD	0x102
#define SYNC_CMD 	(0x103 | CMDIF_NORESP_CMD)
#define ASYNC_N_CMD	0x104
#define OPEN_N_CMD	0x105
#define IC_TEST		0x106
#define CLOSE_CMD	0x107
#define TMAN_TEST	0x108
#define SHBP_TEST	0x109
#define SHBP_TEST_GPP	0x110

#define AIOP_ASYNC_CB_DONE	5  /* Must be in sync with MC ELF */
#define AIOP_SYNC_BUFF_SIZE	80 /* Must be in sync with MC ELF */

#ifdef CMDIF_TEST_WITH_MC_SRV
#define TEST_DPCI_ID    (void *)0 /* For MC use 0 */
#else
#define TEST_DPCI_ID    (void *)4 /* For GPP use 4 */
#endif

struct shbp_test {
	uint64_t shbp;
	uint8_t dpci_id;
};

struct cmdif_desc cidesc;
uint64_t tman_addr;
struct shbp_aiop lbp;
struct shbp_aiop gpp_lbp;

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
	fsl_os_print("ICID in WS is %d\n", ic.icid);
}

static int aiop_async_cb(void *async_ctx, int err, uint16_t cmd_id,
             uint32_t size, void *data)
{
	UNUSED(cmd_id);
	UNUSED(async_ctx);
	
	aiop_ws_check();
	
	fsl_os_print("PASSED AIOP ASYNC CB cmd_id = 0x%x\n" ,cmd_id);
	fsl_os_print("ASYNC CB data 0x%x size = 0x%x\n", (uint32_t)data , size);
	fsl_os_print("Default segment handle = 0x%x size = 0x%x\n", 
	             PRC_GET_SEGMENT_HANDLE(), PRC_GET_SEGMENT_LENGTH());
	ASSERT_COND(PRC_GET_SEGMENT_HANDLE() == 0);
	ASSERT_COND(PRC_GET_FRAME_HANDLE() == 0);

	if (err != 0) {
		fsl_os_print("ERROR inside aiop_async_cb\n");
	}
	if ((size > 0) && (data != NULL)) {
		fsl_os_print("Setting first byte of data with val = 0x%x\n", 
		             AIOP_ASYNC_CB_DONE);
		fsl_os_print("Default segment handle = 0x%x\n", 
		             PRC_GET_SEGMENT_HANDLE());
		((uint8_t *)data)[0] = AIOP_ASYNC_CB_DONE;
		fsl_os_print("Default segment handle = 0x%x\n", 
		             PRC_GET_SEGMENT_HANDLE());
		fdma_modify_default_segment_data(0, (uint16_t)PRC_GET_SEGMENT_LENGTH());
	} else {
		fsl_os_print("No data inside aiop_async_cb\n");
	}
	return err;
}

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

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size,
                              void *data)
{
	int err = 0;

	UNUSED(dev);
	fsl_os_print("ctrl_cb cmd = 0x%x, size = %d, data 0x%x\n",
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
	fsl_os_print("Inside verif_tman_cb \n");
	aiop_ws_check();
	ASSERT_COND(opaque1 == 0x12345);
	ASSERT_COND(opaque2 == 0x1616);
	fsl_os_print("PASSED verif_tman_cb \n");
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
	
	UNUSED(dev);
	fsl_os_print("ctrl_cb0 cmd = 0x%x, size = %d, data  0x%x\n",
	             cmd,
	             size,
	             (uint32_t)data);

	ASSERT_COND(PRC_GET_SEGMENT_HANDLE() == 0);
	ASSERT_COND(PRC_GET_FRAME_HANDLE() == 0);

	aiop_ws_check();
	
	switch (cmd) {
	case SHBP_TEST_GPP:
		fsl_os_print("Testing GPP SHBP...\n");
		shbp_test = data;
		dpci_id = shbp_test->dpci_id;
		err = shbp_enable(dpci_id, shbp_test->shbp, &gpp_lbp);
		ASSERT_COND(gpp_lbp.ic.icid != ICONTEXT_INVALID);
		
		temp64 = shbp_acquire(&gpp_lbp);
		ASSERT_COND(temp64 == 0);
		shbp_test->shbp = 0; /* For test on GPP */
		
		err = shbp_release(&gpp_lbp, p_data);
		ASSERT_COND(!err);
		
		fdma_modify_default_segment_data(0, (uint16_t)PRC_GET_SEGMENT_LENGTH());
		fsl_os_print("Released buffer into GPP SHBP\n");
		break;
	case SHBP_TEST:
		shbp_test = data;
		dpci_id = shbp_test->dpci_id;
		err = shbp_enable(dpci_id, shbp_test->shbp, &lbp);
		ASSERT_COND(lbp.ic.icid != ICONTEXT_INVALID);
		p_data = shbp_acquire(&lbp);
		while (p_data != 0) {
			i++;
			shbp_write(&lbp, sizeof(uint64_t), &p_data, p_data);
			shbp_read(&lbp, sizeof(uint64_t), p_data, &temp64);
			ASSERT_COND(temp64 == p_data);
			err = shbp_release(&lbp, p_data);
			ASSERT_COND(!err);
			p_data = shbp_acquire(&lbp);
		}
		fsl_os_print("Acquired and released %d buffers from SHBP\n", i);
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
		fsl_os_print("TMAN timer created err = %d\n", err);
		break;
	case OPEN_CMD:
		cidesc.regs = TEST_DPCI_ID; /* DPCI 0 is used by MC */
		/* GPP will send DPCI id at the first byte of the data */
		if (size > 0) {
			cidesc.regs = (void *)(((uint8_t *)data)[0]);
		}
		fsl_os_print("Testing AIOP client against GPP DPCI%d\n",
		             (uint32_t)cidesc.regs);
		err = cmdif_open(&cidesc, "IRA", 0, NULL, 0);
		break;
	case CLOSE_CMD:
		err = cmdif_close(&cidesc);
		break;
	case OPEN_N_CMD:
		cidesc.regs = TEST_DPCI_ID; /* DPCI 0 is used by MC */
		if (size > 0) {
			cidesc.regs = (void *)(((uint8_t *)data)[0]);
		}
		fsl_os_print("Testing AIOP client against GPP DPCI%d\n",
		             (uint32_t)cidesc.regs);
		err |= cmdif_open(&cidesc, "IRA0", 0, NULL, 0);
		err |= cmdif_open(&cidesc, "IRA3", 0, NULL, 0);
		err |= cmdif_open(&cidesc, "IRA2", 0, NULL, 0);
		if (err) {
			fsl_os_print("failed to cmdif_open()\n");
			return err;
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
		err |= cmdif_send(&cidesc, 0x1 | CMDIF_ASYNC_CMD, AIOP_SYNC_BUFF_SIZE,
		                  CMDIF_PRI_LOW, p_data, aiop_async_cb, cidesc.regs);
		err |= cmdif_send(&cidesc, 0x2 | CMDIF_ASYNC_CMD, AIOP_SYNC_BUFF_SIZE,
		                  CMDIF_PRI_HIGH, p_data, aiop_async_cb, cidesc.regs);
		err |= cmdif_send(&cidesc, 0x3 | CMDIF_ASYNC_CMD, AIOP_SYNC_BUFF_SIZE,
		                  CMDIF_PRI_LOW, p_data, aiop_async_cb, cidesc.regs);
		err |= cmdif_send(&cidesc, 0x4 | CMDIF_ASYNC_CMD, AIOP_SYNC_BUFF_SIZE,
		                  CMDIF_PRI_HIGH, p_data, aiop_async_cb, cidesc.regs);
		break;
	case SYNC_CMD:
		err = cmdif_send(&cidesc, 0xa, size, CMDIF_PRI_LOW, p_data,
		                 aiop_async_cb, cidesc.regs);
		break;
	case IC_TEST:
		ASSERT_COND(size >= sizeof(struct icontext));
		dpci_id = (((uint8_t *)data)[0]);
		bpid =  (uint16_t)(((uint8_t *)data)[1]);

		err = icontext_get(dpci_id, &ic);
#ifdef CMDIF_TEST_WITH_MC_SRV
		ic.bdi_flags |= FDMA_ENF_BDI_BIT;
#endif
		ASSERT_COND(err == 0);
		fsl_os_print("Isolation context test dpci %d bpid %d:\n", dpci_id, bpid);
		fsl_os_print("ICID %d:\n dma flags 0x%x \n bdi flags 0x%x \n",
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
			fsl_os_print("Addr high = 0x%x low = 0x%x \n",
			             (uint32_t)((p_data & 0xFF00000000) >> 32),
			             (uint32_t)(p_data & 0xFFFFFFFF));
		} else {
			fsl_os_print("FAILED icontext_acquire BPID"
						" 0x%x is empty\n", bpid);
		}
		/* Must be after icontext_get(&ic) */
		icontext_cmd_get(&ic_cmd);
#ifdef CMDIF_TEST_WITH_MC_SRV
		ic_cmd.bdi_flags |= FDMA_ENF_BDI_BIT;
#endif		
		ASSERT_COND(ic_cmd.icid != ICONTEXT_INVALID);
		ASSERT_COND(ic_cmd.icid == ic.icid);
		ASSERT_COND(ic_cmd.bdi_flags == ic.bdi_flags);
		ASSERT_COND(ic_cmd.dma_flags == ic.dma_flags);
		fsl_os_print("PASSED icontext_cmd_get\n");
		
		icontext_aiop_get(&ic);
		ASSERT_COND(ic.dma_flags);
		ASSERT_COND(ic.bdi_flags);
		fsl_os_print("AIOP ICID = 0x%x bdi flags = 0x%x\n", ic.icid, \
		             ic.bdi_flags);
		fsl_os_print("AIOP ICID = 0x%x dma flags = 0x%x\n", ic.icid, \
		             ic.dma_flags);
		break;
	default:
		if ((size > 0) && (data != NULL)) {
			for (i = 0; i < MIN(size, 64); i++) {
				((uint8_t *)data)[i] = 0xDA;
			}
		}
		fdma_modify_default_segment_data(0, (uint16_t)PRC_GET_SEGMENT_LENGTH());
		break;
	}
	
	ASSERT_COND(PRC_GET_SEGMENT_HANDLE() == 0);
	ASSERT_COND(PRC_GET_FRAME_HANDLE() == 0);

	return err;
}


int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	dma_addr_t buff = 0;
	char       module[10];
	int        i = 0;
	struct cmdif_module_ops ops;

	fsl_os_print("Running app_init()\n");

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
			fsl_os_print("FAILED cmdif_register_module %s\n!",
			             module);
			return err;
		}
	}

	err = fsl_os_get_mem(1024, MEM_PART_DP_DDR, 64, &tman_addr);
	ASSERT_COND(!err && tman_addr);
	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}
