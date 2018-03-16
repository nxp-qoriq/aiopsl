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

#include "fsl_types.h"
#include "fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_parser.h"
#include "fsl_l2.h"
#include "fsl_osm.h"
#include "fsl_dbg.h"
#include "fsl_evmng.h"
#include "fsl_stdlib.h"
#include "gso_demo.h"
#include "fsl_malloc.h"
#include "fsl_tman.h"

#define AIOP_APP_NAME		"GSO_demo"

/* L4 fields */
#define NH_FLD_L4_PORT_SRC	(1)
#define NH_FLD_L4_PORT_DST	(NH_FLD_L4_PORT_SRC << 1)
#define PRESENTATION_LENGTH 128

/* Added app to test the TCP GSO API. There are 2 scenarios for this test.
*
1. When GSO_DEMO_SG_PKT_TYPE is not defined: The application uses GSO to segment
the packet. If PRESENTATION_LENGTH
(the Segment Presentation Size) is set to less than 66 it leads to an error
(Fatal error encountered in file: fdma.c,
line: 262 function: fdma_modify_default_segment_data) because the length of
the TCP header only is 32 bytes and TCP header offset is 34.

2. When GSO_DEMO_SG_PKT_TYPE is defined: The application fragments, reassembles
and then uses GSO to segment the obtained packet. The reassemble is done by
keeping the original fragments so that the FD is of S/G type. With the HCL set
to a value equal or smaller than 8 (3 in this case) errors will occur
(error: Frame structural error (invalid S/G bits settings, hop limit).
*
*/
__TASK ipf_ctx_t ipf_context_addr
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
/* Performs frames processing */
ipr_instance_handle_t ipr_instance_val_1;

static __HOT_CODE ENTRY_POINT void app_gso_demo(void)
{
	int			err = 0;
	uint16_t		tcp_mss = 1220;
#ifdef GSO_DEMO_SG_PKT_TYPE
	uint32_t		enc_status = 0;
	/* Fragmentation */
	uint16_t		mtu;
	int			ipf_status;
	struct ipf_context	*ipf_ctx;
	tcp_gso_ctx_t tcp_gso_ctx;

	sl_prolog();
	tcp_gso_context_init(0, tcp_mss, tcp_gso_ctx);
	fsl_print("GSO_demo:tcp_gso_context_init done,TCP_MSS = %d\n", tcp_mss);

	if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
		fsl_print("GSO_demo: Core %d received IPv4 fragment\n",
			  core_get_id());
	} else {
		fsl_print("GSO_demo: Not IPV4 fragment on core %d\n",
			  core_get_id());
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}
	mtu = 256;
	ipf_context_init(0, mtu, ipf_context_addr);
	ipf_ctx = (struct ipf_context *)ipf_context_addr;
	do {
		ipf_status = ipf_generate_frag(ipf_context_addr);
		fsl_print("status = 0x%08x\n", ipf_status);
		fsl_print("Tx Fragment\n");
		reassemble_segment(ipr_instance_val_1, tcp_gso_ctx);
	} while (ipf_status != IPF_GEN_FRAG_STATUS_DONE);

#else
	uint32_t total_length = (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
	tcp_gso_ctx_t tcp_gso_ctx;
	int status;

	sl_prolog();
	if (total_length <= tcp_mss)
		return;
	tcp_gso_context_init(0, tcp_mss, tcp_gso_ctx);
	fsl_print("GSO_demo: tcp_gso_context_init done, TCP_MSS = %d\n",
		  tcp_mss);
	do {
		status = tcp_gso_generate_seg(tcp_gso_ctx);
		if (status == TCP_GSO_GEN_SEG_STATUS_SYN_RST_SET)
			return;
		err = dpni_drv_send(task_get_receive_niid(),
				    DPNI_DRV_SEND_MODE_NONE);
		if (err) {
			if (status == TCP_GSO_GEN_SEG_STATUS_IN_PROCESS)
				tcp_gso_discard_frame_remainder(tcp_gso_ctx);
			break;
		}
	} while (status == TCP_GSO_GEN_SEG_STATUS_IN_PROCESS);
#endif
	fdma_terminate_task();
}

static int app_dpni_link_up_cb(uint8_t generator_id, uint8_t event_id,
			       uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : NI %d link is UP\n", AIOP_APP_NAME, ni);
	return 0;
}

static int app_dpni_link_down_cb(uint8_t generator_id, uint8_t event_id,
				 uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : NI %d link is DOWN\n", AIOP_APP_NAME, ni);
	return 0;
}

static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	struct ep_init_presentation init_presentation;

	UNUSED(generator_id);
	UNUSED(event_id);

	/* Get endpoint and skip DPNI-DPNI connection from app initialization */
	err = dpni_drv_get_connected_obj(ni, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		pr_err("Cannot get connected object for NI %d\n", ni);
		return err;
	}
	if (strcmp(dpni_ep_type, "dpni") == 0)
		return 0;

	/* Configure frame processing callback */
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure processing callback on NI %d\n", ni);
		return err;
	}

	/* Get DPNI ID for current Network Interface ID */
	err = dpni_drv_get_dpni_id(ni, &dpni_id);
	if (err) {
		pr_err("Cannot get DPNI ID for NI %d\n", ni);
		return err;
	}

	/* Default ordering mode should be Concurrent */
	err = dpni_drv_set_concurrent(ni);
	if (err) {
		pr_err("Cannot set Concurrent Execution on NI %d\n", ni);
		return err;
	}

	/* Get DPNI MAC address  */
	err = dpni_drv_get_primary_mac_addr(ni, mac_addr);
	if (err) {
		pr_err("Cannot obtain primary MAC ADDR for NI %d\n", ni);
		return err;
	}

	/* Set the initial segment presentation size */
	err = dpni_drv_get_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot get initial presentation for NI %d\n", ni);
		return err;
	}

	init_presentation.options = EP_INIT_PRESENTATION_OPT_SPS;
	init_presentation.sps = PRESENTATION_LENGTH;
	err = dpni_drv_set_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot set initial presentation for NI %d to %d\n",
		       ni, init_presentation.sps);
		return err;
	}
	err = dpni_drv_set_unicast_promisc(ni, 1);
	if (err != 0)
		fsl_print("dpni_drv_set_unicast_promisc error for ni %d\n", ni);
	/* Enable DPNI to receive frames */
	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("Cannot enable NI %d for Rx/Tx\n", ni);
		return err;
	}
	fsl_print("%s : Successfully configured ni%d (dpni.%d)\n",
		  AIOP_APP_NAME, ni, dpni_id);
	fsl_print("%s : dpni.%d <---connected---> %s.%d ",
		  AIOP_APP_NAME, dpni_id, dpni_ep_type, dpni_ep_id);
	fsl_print("(MAC addr: %02x:%02x:%02x:%02x:%02x:%02x)\n",
		  mac_addr[0], mac_addr[1], mac_addr[2],
		  mac_addr[3], mac_addr[4], mac_addr[5]);
	return 0;
}

/* Early initialization */
int app_early_init(void)
{
#ifdef GSO_DEMO_SG_PKT_TYPE
	int err;

	err = ipr_early_init(1, 100);
	return err;
#else
	return 0;
#endif
}

/* Initializes the application */
int app_init(void)
{
	int err;
#ifdef GSO_DEMO_SG_PKT_TYPE
	struct ipr_params ipr_demo_params;
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
			mem_pid = MEM_PART_DP_DDR;
	ipr_demo_params.max_open_frames_ipv4 = 0x10;
	ipr_demo_params.max_open_frames_ipv6 = 0x10;
	ipr_demo_params.max_reass_frm_size = 0xf000;
	ipr_demo_params.min_frag_size_ipv4 = 0x40;
	ipr_demo_params.min_frag_size_ipv6 = 0x40;
	ipr_demo_params.timeout_value_ipv4 = 0xffe0;
	ipr_demo_params.timeout_value_ipv6 = 0xffe0;
	ipr_demo_params.ipv4_timeout_cb = ipr_timout_cb;
	ipr_demo_params.ipv6_timeout_cb = ipr_timout_cb;
	ipr_demo_params.cb_timeout_ipv4_arg = 0;
	ipr_demo_params.cb_timeout_ipv6_arg = 0;
	ipr_demo_params.flags = IPR_MODE_TABLE_LOCATION_PEB | IPR_MODE_TMI;
	fsl_print("ipr_demo: Creating first IPR instance\n");
	err = ipr_create_instance(&ipr_demo_params, ipr_instance_ptr);
	if (err) {
		fsl_print("ERROR: Creating first IPR instance\n");
		return err;
	}
	ipr_instance_val_1 = ipr_instance;
#endif
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t)app_gso_demo, app_dpni_add_cb);
	if (err) {
		pr_err("Cannot register DPNI ADD event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_UP, 1,
			     (uint64_t)NULL, app_dpni_link_up_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK UP event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_DOWN, 1,
			     (uint64_t)NULL, app_dpni_link_down_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK DOWN event\n");
		return err;
	}

	return 0;
}

/* Frees application allocated resources */
void app_free(void)
{}

void ipr_timout_cb(ipr_timeout_arg_t arg, uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);
	/* Need to discard default frame */
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

void reassemble_segment(ipr_instance_handle_t ipr_instance_val_1,
			tcp_gso_ctx_t tcp_gso_ctx)
{
	int	reassemble_status;
	int	status;
	int	err = 0;

	reassemble_status = ipr_reassemble(ipr_instance_val_1);
	if (reassemble_status == IPR_REASSEMBLY_SUCCESS) {
		fsl_print("ipr_demo: Core %d reassembled IPv4 frame\n",
			  core_get_id());
		do {
			status = tcp_gso_generate_seg(tcp_gso_ctx);
			if (status == TCP_GSO_GEN_SEG_STATUS_SYN_RST_SET)
				return;
			err = dpni_drv_send(task_get_receive_niid(),
					    DPNI_DRV_SEND_MODE_NONE);
			if (err == -ENOMEM) {
				pr_err("%d : dpni_drv_send()\n", err);
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
				break;
			} else if (err) {
				pr_err("%d : dpni_drv_send()\n", err);
				ARCH_FDMA_DISCARD_FD();
				break;
			}
			if ((status == TCP_GSO_GEN_SEG_STATUS_IN_PROCESS) &&
			    (err != 0)) {
				tcp_gso_discard_frame_remainder(tcp_gso_ctx);
				break;
			}
		} while (status == TCP_GSO_GEN_SEG_STATUS_IN_PROCESS);
	} else if ((reassemble_status != IPR_REASSEMBLY_NOT_COMPLETED) &&
		  (reassemble_status != -ETIMEDOUT)) {
		fsl_print("ipr_demo: Reassemble not finished [0x%08x]\n",
			  reassemble_status);
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	}
}
