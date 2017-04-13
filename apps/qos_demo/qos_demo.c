/* Copyright 2017 NXP
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


/* AIOP Classification headers */
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
#include "qos_demo.h"

#define AIOP_APP_NAME		"QoS_demo"

/* L4 fields */
#define NH_FLD_L4_PORT_SRC	(1)
#define NH_FLD_L4_PORT_DST	(NH_FLD_L4_PORT_SRC << 1)

#define PRESENTATION_LENGTH 64

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg);


/* Performs frames processing */
__HOT_CODE ENTRY_POINT static void app_frame_cb(void)
{
	int	err = 0;

	sl_prolog();
	uint8_t tc = task_get_tx_tc();

	fsl_print("\n%s: RX on NI %d | CORE:%d | TC = %d | %s | %s\n",
		AIOP_APP_NAME,
		(uint16_t)PRC_GET_PARAMETER(), core_get_id(), tc,
		PARSER_IS_OUTER_IPV4_DEFAULT() ? "IPv4" :
		(PARSER_IS_OUTER_IPV6_DEFAULT() ? "IPv6" :
		(PARSER_IS_ARP_DEFAULT() ? "ARP" : "unknown")),
		PARSER_IS_TCP_DEFAULT() ||
		PARSER_IS_TCP_OPTIONS_DEFAULT() ||
		PARSER_IS_TCP_CONTROLS_6_11_SET_DEFAULT() ||
		PARSER_IS_TCP_CONTROLS_3_5_SET_DEFAULT() ? "TCP" :
		(PARSER_IS_UDP_DEFAULT() ? "UDP" :
		(PARSER_IS_ICMP_DEFAULT() ? "ICMP" : "unknown")));

	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_FLAGS);

	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();

	pr_err("Failed to send frame\n");
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

extern uint8_t order_scope_buf[256];
static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	struct ep_init_presentation init_presentation;
	uint8_t			*tmp_buf = order_scope_buf;
	struct			dpni_drv_qos_tbl qos_cfg = {0};
	struct			dpni_drv_qos_rule qos_rule = {0};
	uint16_t		mfl = 0x2000; /* Maximum Frame Length */

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

	/* Configure key generation for initial ordering scope */
	app_fill_kg_profile(&kg_cfg);
	err = dpni_drv_set_order_scope(ni, &kg_cfg);
	if (err) {
		pr_err("Cannot set order scope on NI %d\n", ni);
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

	if (ni == 1) {
		/* QoS settings */
		memset(tmp_buf, 0, 256);
		err = dpni_drv_prepare_key_cfg(&kg_cfg, tmp_buf);
		if (err) {
			pr_err("Cannot prepare key_cfg\n", ni);
			return err;
		}

		qos_cfg.key_cfg_iova = (uint64_t)tmp_buf;
		qos_cfg.discard_on_miss = 0;
		qos_cfg.default_tc = 2;
		err = dpni_drv_set_qos_table(ni, &qos_cfg);
		if (err) {
			pr_err("Cannot set qos table on NI %d\n", ni);
			return err;
		}

		/* TCP QoS enty */
		qos_rule.key_iova = (uint64_t)tmp_buf;
		qos_rule.mask_iova = NULL;
		qos_rule.key_size = 1;
		tmp_buf[0] = 0x6;
		err = dpni_drv_add_qos_entry(ni, &qos_rule, 0, 0);
		if (err) {
			pr_err("Cannot add 1st qos entry on NI %d\n", ni);
			return err;
		}

		/* UDP QoS enty */
		tmp_buf[0] = 0x11;
		err = dpni_drv_add_qos_entry(ni, &qos_rule, 1, 1);
		if (err) {
			pr_err("Cannot add 2nd qos entry on NI %d\n", ni);
			return err;
		}
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
	return 0;
}

/* Initializes the application */
int app_init(void)
{
	int err;

	/* Register DPNI events */
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			(uint64_t)app_frame_cb, app_dpni_add_cb);
	if (err) {
		pr_err("Cannot register DPNI ADD event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_UP, 1,
			     (uint64_t) NULL, app_dpni_link_up_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK UP event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_DOWN, 1,
			     (uint64_t) NULL, app_dpni_link_down_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK DOWN event\n");
		return err;
	}

	return 0;
}

/* Frees application allocated resources */
void app_free(void)
{}

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg)
{
	/* Configure Initial Order Scope */
	memset(kg_cfg, 0x0, sizeof(struct dpkg_profile_cfg));

	kg_cfg->num_extracts = 5;
	/* PROTO */
	kg_cfg->extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[0].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[0].extract.from_hdr.field = NET_HDR_FLD_IP_PROTO;
	/* IP_SRC */
	kg_cfg->extracts[1].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[1].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[1].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[1].extract.from_hdr.field = NET_HDR_FLD_IP_SRC;
	/* IP_DST */
	kg_cfg->extracts[2].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[2].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[2].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[2].extract.from_hdr.field = NET_HDR_FLD_IP_DST;
	/* L4 SRC PORT */
	kg_cfg->extracts[3].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[3].extract.from_hdr.prot = NET_PROT_TCP;
	kg_cfg->extracts[3].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[3].extract.from_hdr.field = NH_FLD_L4_PORT_SRC;
	/* L4 DST PORT */
	kg_cfg->extracts[4].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[4].extract.from_hdr.prot = NET_PROT_TCP;
	kg_cfg->extracts[4].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[4].extract.from_hdr.field = NH_FLD_L4_PORT_DST;
}
