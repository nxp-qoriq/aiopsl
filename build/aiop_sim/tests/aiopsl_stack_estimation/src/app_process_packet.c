/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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

#include "types.h"
#include "fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_parser.h"
#include "fsl_general.h"
#include "fsl_time.h"
#include "fsl_slab.h"
#include "fsl_cdma.h"
#include "fsl_l2.h"
#include "fsl_cmdif_client.h"
#include "fsl_cmdif_server.h"
#include "fsl_icontext.h"

int app_init(void);
void app_free(void);
void stack_estimation(void);
extern void cmdif_srv_isr(void);
extern void cmdif_cl_isr(void);
extern void receive_cb(void);

static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	UNUSED(arg);
	stack_estimation();
}

void stack_estimation(void)
{
#pragma fn_ptr_candidates(stack_estimation)
	struct slab **my_slab = 0;
	uint32_t time;
	uint64_t time_since_epoch;
	uint64_t buff = 0;
	struct cmdif_desc cidesc = {0};
	struct icontext ic = {0};
	
	slab_create(5, 0, 256, 0, 0, 4, 0, 1, NULL, my_slab);
	slab_acquire(*my_slab, &buff);
	slab_release(*my_slab, buff);
	slab_free(my_slab);
	fsl_os_print("Test\n");
	fsl_os_rand();
	fsl_get_time_since_epoch_ms(&time_since_epoch);
	fsl_get_time_ms(&time);
	
	/* CMDIF runtime functions */
	cmdif_srv_isr();
	cmdif_cl_isr();
	cmdif_send(&cidesc, 0, 0, CMDIF_PRI_HIGH, NULL, NULL, NULL);
	cmdif_open(&cidesc, NULL, 0, NULL, 0);
	cmdif_close(&cidesc);
	
	/* Isolation Context runtime API */
	icontext_get(5, &ic);
	icontext_acquire(&ic, 7, &buff);
	icontext_release(&ic, 7, buff);
	icontext_dma_read(&ic, 4, buff, &time);
	icontext_dma_write(&ic, 4, &time, buff);
	
	/* DPNI runtime functions */
	receive_cb();
}

int app_init(void)
{
	int        err  = 0;

	fsl_os_print("Running app_init()\n");

	err = dpni_drv_register_rx_cb(1,
				      app_process_packet_flow0,
				      1);
	if (err)
		return err;


	fsl_os_print("To start test inject packets: \"eth_ipv4_udp.pcap\"\n");

	return 0;
}

void app_free(void)
{
	/* free application resources*/
}
