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
#include "fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_cwapr.h"
#include "fsl_platform.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
/*#include "fsl_cmdif_server.h"*/
#include "fsl_cdma.h"
#include "fsl_fdma.h"
#include "fsl_l4.h"
#include "fsl_malloc.h"
#include "fsl_tman.h"
#include "fsl_slab.h"
#include "fsl_malloc.h"
#include "fsl_evmng.h"

int app_early_init(void);
int app_init(void);
void app_free(void);
void cwapr_timout_cb(cwapr_timeout_arg_t arg, uint32_t flags);


/* Global CWAPR var in Shared RAM */
cwapr_instance_handle_t cwapr_instance_val;

__HOT_CODE ENTRY_POINT static void app_process_packet(void)
{
	uint64_t tunnel_id = 0;
	int reassemble_status;
	int err;

	err = sl_prolog();
	if (err)
		fsl_print("ERROR = %d: sl_prolog()\n",err);
		
	reassemble_status = cwapr_reassemble(cwapr_instance_val, tunnel_id);
	if (reassemble_status == CWAPR_REASSEMBLY_SUCCESS)
	{
		fsl_print
		("cwapr: Core %d will send a reassembled frame with ipv4 header:\n"
					, core_get_id());

		err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);

		if (err){
			fsl_print("ERROR = %d: dpni_drv_send()\n",err);
			if(err == -ENOMEM)
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			else /* (err == -EBUSY) */
				fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS, FDMA_DIS_NO_FLAGS);
		}

		if(!err) /*No error found during injection of packets*/
			fsl_print("Finished SUCCESSFULLY\n");
		else
			fsl_print("Finished with ERRORS\n");
	}
	/*MUST call fdma_terminate task in the end of cb function*/
	fdma_terminate_task();
}

static int app_dpni_event_added_cb(
			uint8_t generator_id,
			uint8_t event_id,
			uint64_t app_ctx,
			void *event_data)
{
	uint16_t ni = (uint16_t)((uint32_t)event_data);
	uint16_t    mfl = 0x2000; /* Maximum Frame Length */
	int err;

	UNUSED(generator_id);
	UNUSED(event_id);
	pr_info("Event received for AIOP NI ID %d\n",ni);
	err = dpni_drv_register_rx_cb(ni/*ni_id*/,
	                              (rx_cb_t *)app_ctx);
	if (err){
		pr_err("dpni_drv_register_rx_cb for ni %d failed: %d\n", ni, err);
		return err;
	}
	err = dpni_drv_set_max_frame_length(ni/*ni_id*/,
	                                    mfl /* Max frame length*/);
	if (err){
		pr_err("dpni_drv_set_max_frame_length for ni %d failed: %d\n", ni, err);
		return err;
	}
	err = dpni_drv_set_unicast_promisc(ni/*ni_id*/, TRUE);
	if (err){
		pr_err("dpni_drv_set_unicast_promisc for ni %d failed: %d\n", ni, err);
		return err;
	}
	err = dpni_drv_enable(ni);
	if(err){
		pr_err("dpni_drv_enable for ni %d failed: %d\n", ni, err);
		return err;
	}
	return 0;
}



int app_early_init(void){
	int err;

	err = cwapr_early_init(1, 100);
	return err;
}


int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	uint64_t buff = 0;
	uint64_t tmi_mem_base_addr;

	struct cwapr_params cwapr_params;
	
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	cwapr_instance_handle_t cwapr_instance = 0;
	cwapr_instance_handle_t *cwapr_instance_ptr = &cwapr_instance;

	fsl_print("Running app_init()\n");

	cwapr_params.max_open_frames = 0x10;
	cwapr_params.max_reass_frm_size = 0xf000;
	cwapr_params.timeout_value = 0xffe0;
	cwapr_params.timeout_cb = cwapr_timout_cb;
	cwapr_params.cb_timeout_arg = 0;
	cwapr_params.flags = CWAPR_MODE_TABLE_LOCATION_PEB;
	fsl_get_mem( 0x20*64, mem_pid, 64, &tmi_mem_base_addr);

	tman_create_tmi(tmi_mem_base_addr , 0x20, &cwapr_params.tmi_id);

	fsl_print("cwapr: Creating CWAPR instance\n");
	err = cwapr_create_instance(&cwapr_params, cwapr_instance_ptr);
	if (err)
	{
		fsl_print("ERROR: cwapr_create_instance() failed %d\n",err);
		return err;
	}

	cwapr_instance_val = cwapr_instance;

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,(uint64_t) app_process_packet, app_dpni_event_added_cb);
	if (err){
		pr_err("EVM registration for DPNI_EVENT_ADDED failed: %d\n", err);
		return err;
	}

	fsl_print("To start test inject packets after AIOP boot complete.\n");

	return 0;
}

void app_free(void)
{
}

void cwapr_timout_cb(cwapr_timeout_arg_t arg,
		uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);
	/* Need to discard default frame */
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}
