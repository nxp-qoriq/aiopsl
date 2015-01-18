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
#include "common/fsl_string.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_cdma.h"
#include "platform.h"
#include "cmdif_srv.h"
#include "fsl_io.h"
#include "fsl_io_ccsr.h"
#include "aiop_common.h"

extern void app_receive_cb (void);

int app_init(void);
void app_free(void);

//#define REFLECTOR_DEMO

#ifdef REFLECTOR_DEMO
uint32_t    sync_done;

/* Client STUB */
static int my_cmdif_open(struct cmdif_desc *cidesc,
		const char *module_name,
		uint8_t instance_id,
		cmdif_cb_t async_cb,
		void *async_ctx)
{
	struct   cmdif_srv *srv = sys_get_handle(FSL_OS_MOD_CMDIF_SRV, 0);
	UNUSED(cidesc);
	UNUSED(module_name);
	UNUSED(instance_id);
	UNUSED(async_cb);
	UNUSED(async_ctx);
	srv->inst_dev[0] = (void *)0x22222222;
	srv->sync_done[0] = (uint64_t)&sync_done;
	return 0;
}
#endif

static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(instance_id);
	UNUSED(dev);
	if (instance_id == 0)
		fsl_os_print("FAILED: open_cb inst_id = 0x%x\n", instance_id);
	fsl_os_print("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	fsl_os_print("close_cb\n");
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, uint64_t data)
{
	UNUSED(dev);
	UNUSED(cmd);
	UNUSED(size);
	UNUSED(data);
	fsl_os_print("ctrl_cb cmd = 0x%x\n", cmd);
	return 0;
}

/* This is temporal WA for stand alone demo only */
#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,            \
	                                   0,                          \
	                                   E_MAPPED_MEM_TYPE_GEN_REGS) \
	                                   + SOC_PERIPH_OFF_AIOP_WRKS);
static void epid_setup()
{
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;

	/* EPID = 0 is saved for cmdif, need to set it for stand alone demo */
	iowrite32_ccsr(0, &wrks_addr->epas);
#ifdef REFLECTOR_DEMO
	iowrite32_ccsr((uint32_t)cmdif_srv_isr, &wrks_addr->ep_pc);
#else
	iowrite32_ccsr((uint32_t)app_receive_cb, &wrks_addr->ep_pc);
#endif
}

int app_init(void)
{
	const char * module = "ABCABC";
	struct cmdif_module_ops ops;
	int err = 0;

	ops.open_cb  = open_cb;
	ops.close_cb = close_cb;
	ops.ctrl_cb  = ctrl_cb;

	err = cmdif_register_module(module, &ops);
	if (err) return err;
	err = cmdif_register_module(module, &ops);
	if (!err) return err;
	err = cmdif_register_module(module, &ops);
	if (!err) return err;
	err = cmdif_register_module("ABCDEF", &ops);
	if (err) return err;
	err = cmdif_register_module("TTTTTTTTTTT", &ops);
	if (err) return err;
	err = cmdif_unregister_module("ABCDEF");
	if (err) return err;
	err = cmdif_unregister_module("TTTTTTTTTTT");
	if (err) return err;

#ifdef REFLECTOR_DEMO
	err = my_cmdif_open(NULL, module, 0, NULL, NULL);
#endif
	epid_setup();


	return err;
}

void app_free(void)
{

}
