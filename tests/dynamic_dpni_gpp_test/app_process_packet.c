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
#include "fsl_general.h"
#include "fsl_cdma.h"
#include "fsl_l2.h"
#include "fsl_osm.h"
#include "fsl_dbg.h"
#include "fsl_evmng.h"
#include "apps_arch.h"

int app_early_init(void);
int app_init(void);
void app_free(void);

__HOT_CODE ENTRY_POINT static void app_process_packet(void)
{
	int      err = 0;
	int local_test_error = 0;
	uint16_t ipv4hdr_length = 0;
	uint16_t ipv4hdr_offset = 0;
	uint16_t ni_id;
	uint8_t *p_ipv4hdr = 0;
	uint8_t *p_eth_hdr;
	uint32_t dst_addr = 0;// ipv4 dst_addr - will store original destination address
	uint8_t local_hw_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	struct ipv4hdr *ipv4header;

	sl_prolog();

	if (PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		p_ipv4hdr = (uint8_t *) PRC_GET_SEGMENT_ADDRESS() + ipv4hdr_offset ;

		ipv4header = (struct ipv4hdr *)p_ipv4hdr;
		ipv4hdr_length = (uint16_t)((ipv4header->vsn_and_ihl & 0x0F) << 2);

		pr_info("Received packet with ipv4 header:\n");
		pr_info("%08x %08x %08x %08x %08x\n",
		        ((uint32_t*)p_ipv4hdr)[0],
		        ((uint32_t*)p_ipv4hdr)[1],
		        ((uint32_t*)p_ipv4hdr)[2],
		        ((uint32_t*)p_ipv4hdr)[3],
		        ((uint32_t*)p_ipv4hdr)[4]);


		dst_addr = ipv4header->dst_addr;
		/*for reflection when switching between src and dst IP the checksum remains the same*/
		err = ip_set_nw_dst(ipv4header->src_addr);
		if (err) {
			pr_warn("ERROR = %d: ip_set_nw_dst(src_addr)\n", err);
			local_test_error |= err;
		}
		err = ip_set_nw_src(dst_addr);
		if (err) {
			pr_warn("ERROR = %d: ip_set_nw_src(dst_addr)\n", err);
			local_test_error |= err;
		}
	}

	ni_id = (uint16_t)task_get_receive_niid();
	/*switch between src and dst MAC addresses*/
	p_eth_hdr = PARSER_GET_ETH_POINTER_DEFAULT();
	p_eth_hdr += NET_HDR_FLD_ETH_ADDR_SIZE;
	l2_set_dl_dst(p_eth_hdr);
	dpni_drv_get_primary_mac_addr(ni_id, local_hw_addr);
	l2_set_dl_src(local_hw_addr);

	if (!local_test_error && PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		pr_info("Will send a modified packet with ipv4 header:\n");
		pr_info("%08x %08x %08x %08x %08x\n",
		        ((uint32_t*)p_ipv4hdr)[0],
		        ((uint32_t*)p_ipv4hdr)[1],
		        ((uint32_t*)p_ipv4hdr)[2],
		        ((uint32_t*)p_ipv4hdr)[3],
		        ((uint32_t*)p_ipv4hdr)[4]);

	}
	osm_scope_transition_to_exclusive_with_increment_scope_id();
	err = dpni_drv_send(ni_id, DPNI_DRV_SEND_MODE_NONE);
	if (err){
		pr_warn("ERROR = %d: dpni_drv_send(ni_id)\n",err);
		local_test_error |= err;
		if(err == -ENOMEM)
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		else /* (err == -EBUSY) */
			ARCH_FDMA_DISCARD_FD();
	}

	if(!local_test_error) /*No error found during injection of packets*/
		pr_info("Finished SUCCESSFULLY\n");
	else
		pr_warn("Finished with ERRORS\n");
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
	int err;
	uint8_t mac[6];

	UNUSED(generator_id);
	UNUSED(event_id);
	pr_info("Event obj added received for AIOP NI ID %d\n", ni);

	err = dpni_drv_register_rx_cb(ni/*ni_id*/,
	                              (rx_cb_t *)app_ctx);
	if (err){
		pr_err("dpni_drv_register_rx_cb for ni %d failed: %d\n", ni, err);
		return err;
	}
	err = dpni_drv_set_max_frame_length(ni/*ni_id*/,
	                                    0x2000 /* Max frame length*/);
	if (err){
		pr_err("dpni_drv_set_max_frame_length for ni %d failed: %d\n", ni, err);
		return err;
	}

	err = dpni_drv_get_primary_mac_addr(ni, mac);
	if (err){
		pr_err("dpni_drv_get_primary_mac_addr for ni %d failed: %d\n", ni, err);
		return err;
	}
	else{
		pr_info("MAC for NI %d: %x:%x:%x:%x:%x:%x\n",
		        ni,
		        (int)mac[0],
		        (int)mac[1],
		        (int)mac[2],
		        (int)mac[3],
		        (int)mac[4],
		        (int)mac[5]);
	}

	err = dpni_drv_enable(ni);
	if(err){
		pr_err("dpni_drv_enable for ni %d failed: %d\n", ni, err);
		return err;
	}

	return 0;
}

static int app_dpni_event_removed_cb(
			uint8_t generator_id,
			uint8_t event_id,
			uint64_t app_ctx,
			void *event_data)
{
	uint16_t ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(app_ctx);
	UNUSED(event_id);
	pr_info("Event obj removed received for AIOP NI ID %d\n", (int)ni);

	return 0;
}

static int app_dpni_event_link_change_cb(
			uint8_t generator_id,
			uint8_t event_id,
			uint64_t app_ctx,
			void *event_data)
{
	uint16_t ni = (uint16_t)((uint32_t)event_data);
	int i;
	uint64_t ctr_value = 0;
	UNUSED(generator_id);

	if(app_ctx){
		pr_info("Event %d received for AIOP NI ID %d for link UP!!!\n",(int)event_id, (int)ni);
	}
	else{
		pr_info("Event %d received for AIOP NI ID %d for link DOWN!\n",(int)event_id, (int)ni);
		for(i = 0; i < DPNI_DRV_CNT_EGR_FRAME_DISCARD + 1; i++){
			dpni_drv_get_counter(ni, (enum dpni_drv_counter) i, &ctr_value);
			pr_info("AIOP NI ID %d: ctr %d - value: %d\n",(int)ni, i, (int)ctr_value);
		}
		dpni_drv_disable(ni);
	}
	return 0;
}


int app_init(void)
{
	int err;

	pr_info("Running app_init()\n");

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,(uint64_t) app_process_packet, app_dpni_event_added_cb);
	if (err){
		pr_err("EVM registration for DPNI_EVENT_ADDED failed: %d\n", err);
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_REMOVED, 1, 0, app_dpni_event_removed_cb);
	if (err){
		pr_err("EVM registration for DPNI_EVENT_REMOVED failed: %d\n", err);
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_DOWN, 1,(uint64_t) 0, app_dpni_event_link_change_cb);
	if (err){
		pr_err("EVM registration for DPNI_EVENT_LINK_DOWN failed: %d\n", err);
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_UP, 1,(uint64_t) 1, app_dpni_event_link_change_cb);
	if (err){
		pr_err("EVM registration for DPNI_EVENT_LINK_UP failed: %d\n", err);
		return err;
	}

	pr_info("To start test inject packets: \"app_process_packet.pcap\" after AIOP boot complete.\n");

	return 0;
}

int app_early_init(void)
{
	/* Early initialization */
	return 0;
}

void app_free(void)
{
	/* free application resources*/
}
