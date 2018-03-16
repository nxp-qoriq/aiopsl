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
#include "apps.h"
#include "fsl_ste.h"

int app_early_init(void);
int app_init(void);
void app_free(void);
void cwapr_timout_cb(cwapr_timeout_arg_t arg, uint32_t flags);
static void print_cwapr_statistics(
		cwapr_instance_handle_t cwapr_instance_handle);

/* Global CWAPR var in Shared RAM */
cwapr_instance_handle_t cwapr_instance_handle;
uint64_t stats_mem_base_addr;

__HOT_CODE ENTRY_POINT static void app_process_packet(void)
{
	uint64_t tunnel_id = 0;
	int reassemble_status;
	int err;

	err = sl_prolog();
	if (err)
		fsl_print("ERROR = %d: sl_prolog()\n", err);

	if (PARSER_IS_CAPWAP_CONTROL_DEFAULT() ||
	    PARSER_IS_CAPWAP_DATA_DEFAULT()) {
		struct capwaphdr *capwap_hdr;

		capwap_hdr = (void *)(PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() +
				PRC_GET_SEGMENT_ADDRESS());

		if (capwap_hdr->bits_flags & NET_HDR_FLD_CAPWAP_F)
			fsl_print("CWAPR_DEMO:: Received CAPWAP fragment with length %d\n",
					LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
		else
			fsl_print("CWAPR_DEMO:: Received CAPWAP frame with length %d\n",
					LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
	} else {
		fsl_print("CWAPR_DEMO:: Received non-CAPWAP frame -> DROP frame\n");
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}

	reassemble_status = cwapr_reassemble(cwapr_instance_handle, tunnel_id);
	if ((reassemble_status == CWAPR_REASSEMBLY_SUCCESS) ||
	   (reassemble_status == CWAPR_REASSEMBLY_REGULAR)) {

		if (reassemble_status == CWAPR_REASSEMBLY_SUCCESS) {
			fsl_print
			("CWAPR_DEMO:: Send reassembled CAPWAP frame with length %d\n"
				, LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));

			print_cwapr_statistics(cwapr_instance_handle);
		} else
			fsl_print
			("CWAPR_DEMO:: Send CAPWAP frame with length %d\n"
				, LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));

		err = dpni_drv_send(task_get_receive_niid(),
				DPNI_DRV_SEND_MODE_NONE);
		if (err) {
			fsl_print("ERROR = %d: dpni_drv_send()\n", err);
			if (err == -ENOMEM)
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			else /* (err == -EBUSY) */
				ARCH_FDMA_DISCARD_FD();
		}
	} else if ((reassemble_status != CWAPR_REASSEMBLY_NOT_COMPLETED) &&
		(reassemble_status != -ETIMEDOUT)) {
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
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
	uint16_t dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t	mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint16_t    mfl = 0x2000; /* Maximum Frame Length */
	int err;

	UNUSED(generator_id);
	UNUSED(event_id);

	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure processing callback on NI %d\n", ni);
		return err;
	}
	err = dpni_drv_set_max_frame_length(ni, mfl);
	if (err) {
		pr_err("Cannot configure maximum frame length on NI %d\n", ni);
		return err;
	}
	err = dpni_drv_set_unicast_promisc(ni, TRUE);
	if (err) {
		pr_err("Cannot configure promiscuous mode on NI %d\n", ni);
		return err;
	}
	/* Get DPNI ID for current Network Interface ID */
	err = dpni_drv_get_dpni_id(ni, &dpni_id);
	if (err) {
		pr_err("Cannot get DPNI ID for NI %d\n", ni);
		return err;
	}
	/* Get DPNI MAC address  */
	err = dpni_drv_get_primary_mac_addr(ni, mac_addr);
	if (err) {
		pr_err("Cannot obtain primary MAC ADDR for NI %d\n", ni);
		return err;
	}
	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("Cannot enable NI %d for Rx/Tx\n", ni);
		return err;
	}

	fsl_print("CWAPR_DEMO:: Successfully configured ni%d (dpni.%d)\n",
		  ni, dpni_id);
	fsl_print("(MAC addr: %02x:%02x:%02x:%02x:%02x:%02x)\n",
		  mac_addr[0], mac_addr[1], mac_addr[2],
		  mac_addr[3], mac_addr[4], mac_addr[5]);
	return 0;
}

int app_early_init(void)
{
	int err;

	err = cwapr_early_init(1, 100);
	return err;
}


int app_init(void)
{
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;
	struct cwapr_params cwapr_params;
	struct cwapr_stats_cntrs cwapr_stats;
	uint64_t tmi_mem_base_addr;
	uint32_t ni   = 0;
	uint64_t buff = 0;
	int err  = 0;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	cwapr_instance_handle_t cwapr_instance = 0;
	cwapr_instance_handle_t *cwapr_instance_ptr = &cwapr_instance;

	cwapr_params.max_open_frames = 0x10;
	cwapr_params.max_reass_frm_size = 0xf000;
	cwapr_params.timeout_value = 2000; /* 200 * 10ms */
	cwapr_params.timeout_cb = cwapr_timout_cb;
	cwapr_params.cb_timeout_arg = 0;
	cwapr_params.flags = CWAPR_MODE_TABLE_LOCATION_PEB |
			CWAPR_MODE_EXTENDED_STATS_EN;
	fsl_get_mem(0x21*64, mem_pid, 64, &tmi_mem_base_addr);
	tman_create_tmi(tmi_mem_base_addr, 0x20, &cwapr_params.tmi_id);

	/* Obtain memory for statistics data structure */
	fsl_get_mem(sizeof(struct cwapr_stats_cntrs),
			mem_pid, 64, &stats_mem_base_addr);

	memset(&cwapr_stats, 0, sizeof(struct cwapr_stats_cntrs));
	cdma_write(stats_mem_base_addr, &cwapr_stats,
			sizeof(struct cwapr_stats_cntrs));

	cwapr_params.extended_stats_addr = stats_mem_base_addr;

	fsl_print("CWAPR_DEMO:: Creating CWAPR instance\n");
	err = cwapr_create_instance(&cwapr_params, cwapr_instance_ptr);
	if (err) {
		pr_err("Cannot create CWAPR instance: %d\n", err);
		return err;
	}

	cwapr_instance_handle = cwapr_instance;

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			(uint64_t)app_process_packet, app_dpni_event_added_cb);
	if (err) {
		pr_err("Cannot register DPNI ADD event: %d\n", err);
		return err;
	}

	fsl_print("CWAPR_DEMO:: Successfully finished initialization\n");

	return 0;
}

void app_free(void)
{
}

void cwapr_timout_cb(cwapr_timeout_arg_t arg, uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);

	fsl_print("CWAPR_DEMO:: Fragment timeout -----------------------\n");

	/* Need to discard default frame */
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

static void print_cwapr_statistics(
				cwapr_instance_handle_t cwapr_instance_handle)
{
	struct cwapr_stats_cntrs stats;
	uint32_t reass_frm_cntr;

	ste_barrier();
	cwapr_get_reass_frm_cntr(cwapr_instance_handle, &reass_frm_cntr);

#ifdef LS2085A_REV1
	cdma_read(&stats,
		stats_mem_base_addr,
		sizeof(struct cwapr_stats_cntrs));
#else
	cdma_read_with_no_cache(&stats,
		stats_mem_base_addr,
		sizeof(struct cwapr_stats_cntrs));
#endif

	fsl_print("CWAPR_DEMO:: Statistics -----------------------------\n");
	fsl_print("CWAPR_DEMO:: reass_frm_cntr = %d\n",  reass_frm_cntr);
	fsl_print("CWAPR_DEMO:: valid_frags_cntr = %d\n",
			stats.valid_frags_cntr);
	fsl_print("CWAPR_DEMO:: malformed_frags_cntr = %d\n",
			stats.malformed_frags_cntr);
	fsl_print("CWAPR_DEMO:: open_reass_frms_exceed_cntr = %d\n",
			stats.open_reass_frms_exceed_cntr);
	fsl_print("CWAPR_DEMO:: exceed_max_reass_frm_size = %d\n",
			stats.exceed_max_reass_frm_size);
	fsl_print("CWAPR_DEMO:: more_than_64_frags_cntr = %d\n",
			stats.more_than_64_frags_cntr);
	fsl_print("CWAPR_DEMO:: time_out_cntr = %d\n",
			stats.time_out_cntr);
	fsl_print("CWAPR_DEMO:: ----------------------------------------\n");

}
