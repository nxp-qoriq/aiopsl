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
#include "fsl_cwapf.h"
//#include "fsl_platform.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "fsl_general.h"
#include "fsl_dbg.h"
/*#include "fsl_cmdif_server.h"*/
#include "fsl_cdma.h"
#include "fsl_evmng.h"
#include "fsl_osm.h"
#include "apps.h"


int app_init(void);
int app_early_init(void);
void app_free(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

__TASK cwapf_ctx_t cwapf_context_addr
	__attribute__((aligned(sizeof(struct ldpaa_fd))));


__HOT_CODE ENTRY_POINT static void app_process_packet(void)
{
	int      err = 0;
	const uint16_t ipv4hdr_length = sizeof(struct ipv4hdr);
	uint16_t ipv4hdr_offset = 0;
	uint8_t *p_ipv4hdr = 0;
	uint16_t mtu;
	int cwapf_status;

/*	cwapf_ctx_t cwapf_context_addr __attribute__((aligned(sizeof(struct ldpaa_fd))));*/

	sl_prolog();

	mtu = 100;

	if (PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		fsl_print
		("cwapf_demo:Core %d received packet with ipv4 header:\n",
	    core_get_id());
		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		p_ipv4hdr = UINT_TO_PTR((ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS()));
		for( int i = 0; i < ipv4hdr_length ;i ++)
		{
			fsl_print(" %x",p_ipv4hdr[i]);
		}
		fsl_print("\n");
	}

	cwapf_context_init(mtu, cwapf_context_addr);
	fsl_print("cwapf_demo: cwapf_context_init done, MTU = %d\n", mtu);

	do {
		cwapf_status = cwapf_generate_frag(cwapf_context_addr);

		if (cwapf_status > 0){
			fsl_print
			("cwapf_demo: Core %d will send a fragment with ipv4 header:\n"
				, core_get_id());

			ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
			p_ipv4hdr = UINT_TO_PTR((ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS()));

			for( int i = 0; i < ipv4hdr_length ;i ++)
			{
				fsl_print(" %x",p_ipv4hdr[i]);
			}
			fsl_print("\n");

		}
		err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
		if (err){
			fsl_print("ERROR = %d: dpni_drv_send()\n",err);
			if(err == -ENOMEM)
			{
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			}
			else /* (err == -EBUSY) */
				ARCH_FDMA_DISCARD_FD();

			if (cwapf_status == CWAPF_GEN_FRAG_STATUS_IN_PROCESS)
				cwapf_discard_frame_remainder(cwapf_context_addr);
			break;
		}
	} while (cwapf_status != CWAPF_GEN_FRAG_STATUS_DONE);

	fsl_print
		("cwapf_demo: Core %d will send last fragment with ipv4 header:\n"
			, core_get_id());
	for( int i = 0; i < ipv4hdr_length ;i ++)
	{
		fsl_print(" %x",p_ipv4hdr[i]);
	}
		fsl_print("\n");

	/*MUST call fdma_terminate task in the end of cb function*/
	fdma_terminate_task();
}


#ifdef AIOP_STANDALONE
/* This is temporal WA for stand alone demo only */
#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_MOD_CMGW,            \
	                                   0,                          \
	                                   E_MAPPED_MEM_TYPE_GEN_REGS) \
	                                   + SOC_PERIPH_OFF_AIOP_WRKS);
static void epid_setup()
{
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;

	/* EPID = 0 is saved for cmdif, need to set it for stand alone demo */
	iowrite32(0, &wrks_addr->epas);
	iowrite32((uint32_t)receive_cb, &wrks_addr->ep_pc);
}
#endif /* AIOP_STANDALONE */

/*static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev);
	fsl_print("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	fsl_print("close_cb\n");
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, uint64_t data)
{
	UNUSED(dev);
	UNUSED(size);
	UNUSED(data);
	fsl_print("ctrl_cb cmd = 0x%x, size = %d, data high= 0x%x data low= 0x%x\n",
	             cmd,
	             size,
	             (uint32_t)((data & 0xFF00000000) >> 32),
	             (uint32_t)(data & 0xFFFFFFFF));
	return 0;
}

static struct cmdif_module_ops ops = {
                               .open_cb = open_cb,
                               .close_cb = close_cb,
                               .ctrl_cb = ctrl_cb
};
*/


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

    /* Enable promiscuous mode */
    err = dpni_drv_set_unicast_promisc(ni, 1);
    if (err) {
            pr_err("Cannot enable promiscuous mode on NI %d\n", ni);
            return err;
    }

	err = dpni_drv_set_max_frame_length(ni/*ni_id*/,
	                                    mfl /* Max frame length*/);
	if (err){
		pr_err("dpni_drv_set_max_frame_length for ni %d failed: %d\n", ni, err);
		return err;
	}
	err = dpni_drv_enable(ni);
	if(err){
		pr_err("dpni_drv_enable for ni %d failed: %d\n", ni, err);
		return err;
	}
	return 0;
}

int app_init(void)
{
	int        err  = 0;

	fsl_print("Running app_init()\n");
#ifdef AIOP_STANDALONE
	/* This is temporal WA for stand alone demo only */
	epid_setup();
#endif /* AIOP_STANDALONE */

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,(uint64_t) app_process_packet, app_dpni_event_added_cb);
	if (err){
		pr_err("EVM registration for DPNI_EVENT_ADDED failed: %d\n", err);
		return err;
	}

/*
	err = cmdif_register_module("TEST0", &ops);
	if (err)
		fsl_print("FAILED cmdif_register_module\n!");
*/
	fsl_print("To start test inject packets: \"reassembled_frame.pcap\" after AIOP boot complete.\n");
	return 0;
}

int app_early_init(void)
{
	/* Early initialization */
	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}
