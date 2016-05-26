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
#include "fsl_parser.h"
#include "fsl_l2.h"
#include "fsl_osm.h"
#include "fsl_dbg.h"
#include "fsl_evmng.h"
#include "fsl_stdlib.h"
#include "reflector.h"

#define AIOP_APP_NAME		"REFLECTOR"

/* L4 fields */
#define NH_FLD_L4_PORT_SRC	(1)
#define NH_FLD_L4_PORT_DST	(NH_FLD_L4_PORT_SRC << 1)

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg);
static inline void l2_ip_src_dst_swap(void);

#ifdef PRINT_FRAME_INFO
/* Prints DPNI on which frame was received, together with CORE and IP & MAC
 * source and destination addresses */
static uint64_t print_frame_mutex;
static void print_frame_info(void);
#else
	#define print_frame_info()
#endif

/* Performs frames processing */
__HOT_CODE ENTRY_POINT static void app_reflector(void)
{
	int	err = 0;

	sl_prolog();

	if (!PARSER_IS_OUTER_IPV4_DEFAULT()) {
		/* Discard non IPV4 frame and terminate task */
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}

	/* Print frame to be processed */
	print_frame_info();

	/* Swap source & destination addresses for L2 and IP protocols */
	l2_ip_src_dst_swap();

	/* Modify the data in the default Data Segment */
	fdma_modify_default_segment_full_data();

#ifdef EXCLUSIVE_MODE
	/* Restore order for frames from the same flow */

	/* Transition to mode EXCLUSIVE */
	osm_scope_transition_to_exclusive_with_increment_scope_id();
#endif

	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
	if (!err)
		fdma_terminate_task();

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

static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;

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

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t) app_reflector, app_dpni_add_cb);
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
	/* IP_SRC */
	kg_cfg->extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[0].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[0].extract.from_hdr.field = NET_HDR_FLD_IP_SRC;
	/* IP_DST */
	kg_cfg->extracts[1].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[1].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[1].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[1].extract.from_hdr.field = NET_HDR_FLD_IP_DST;
	/* PROTO */
	kg_cfg->extracts[2].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[2].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[2].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[2].extract.from_hdr.field = NET_HDR_FLD_IP_PROTO;
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

static inline void l2_ip_src_dst_swap(void)
{
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	uint8_t  *ethhdr = PARSER_GET_ETH_POINTER_DEFAULT();
	uint8_t  dst_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	struct   ipv4hdr *ipv4_hdr;
	uint32_t ip_src_addr;
	uint8_t	 *eth_src, *eth_dst;

	/* get ETH source and destination addresses */
	eth_dst = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT());
	eth_src = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT() +
					NET_HDR_FLD_ETH_ADDR_SIZE);

	/* store MAC_DST */
	*((uint32_t *)&dst_addr[0]) = *((uint32_t *)eth_dst);
	*((uint16_t *)&dst_addr[4]) = *((uint16_t *)(eth_dst + 4));

	/* set ETH destination address */
	*((uint32_t *)(&ethhdr[0])) = *((uint32_t *)(eth_src));
	*((uint16_t *)(&ethhdr[4])) = *((uint16_t *)(eth_src + 4));

	/* set ETH source address */
	*((uint32_t *)(&ethhdr[6])) = *((uint32_t *)&dst_addr[0]);
	*((uint16_t *)(&ethhdr[10])) = *((uint16_t *)&dst_addr[4]);

	/* get IPv4 header */
	ipv4_hdr = (struct ipv4hdr *)((uint8_t *)
			PRC_GET_SEGMENT_ADDRESS() +
			(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	/* store IP source address before changing it */
	ip_src_addr = ipv4_hdr->src_addr;
	/* swap IP source & destination addresses */
	ipv4_hdr->src_addr = ipv4_hdr->dst_addr;
	ipv4_hdr->dst_addr = ip_src_addr;

	/* we do not need to update nor the IP, nor the L4 checksum, because
	 * the IP source & destination addresses were swapped and not replaced
	 * with other values */

	/* Mark running sum as invalid */
	pr->gross_running_sum = 0;
}

#ifdef PRINT_FRAME_INFO
static void print_frame_info(void)
{
	struct ipv4hdr  *ipv4_hdr;
	uint8_t		*ip_src, *ip_dst, *eth_src, *eth_dst;

	/* Get IPv4 header */
	ipv4_hdr = (struct ipv4hdr *)((uint8_t *)
			PRC_GET_SEGMENT_ADDRESS() +
			(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	/* Lock mutex */
	cdma_mutex_lock_take(print_frame_mutex, CDMA_MUTEX_WRITE_LOCK);

	fsl_print("\nRX on NI %d | CORE:%d\n",
		(uint16_t)PRC_GET_PARAMETER(), core_get_id());

	/* Get Ethernet Source & Destination addresses */
	eth_dst = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT());
	eth_src = eth_dst + NET_HDR_FLD_ETH_ADDR_SIZE;

	fsl_print("  MAC_SA: %02x-%02x-%02x-%02x-%02x-%02x",
		eth_src[0], eth_src[1], eth_src[2],
		eth_src[3], eth_src[4], eth_src[5]);

	fsl_print(" MAC_DA: %02x-%02x-%02x-%02x-%02x-%02x\n",
		eth_dst[0], eth_dst[1], eth_dst[2],
		eth_dst[3], eth_dst[4], eth_dst[5]);

	/* Get IP Source & Destination addresses */
	ip_src = (void *)&ipv4_hdr->src_addr;
	ip_dst = (void *)&ipv4_hdr->dst_addr;
	fsl_print("  IP_SRC: %d.%d.%d.%d IP_DST: %d.%d.%d.%d\n",
		ip_src[0], ip_src[1], ip_src[2], ip_src[3],
		ip_dst[0], ip_dst[1], ip_dst[2], ip_dst[3]);

	/* Unlock mutex */
	cdma_mutex_lock_release(print_frame_mutex);
}
#endif
