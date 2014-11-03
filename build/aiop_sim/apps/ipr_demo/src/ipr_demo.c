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

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "platform.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
/*#include "fsl_cmdif_server.h"*/
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_ip.h"
#include "dplib/fsl_l4.h"
#include "ls2085_aiop/fsl_platform.h"
#include "fsl_tman.h"

int app_init(void);
void app_free(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

#define IPR_DEMO_WITH_HM	0x80
#define IPR_DEMO_WITHOUT_HM	0x00

/* Global IPR var in Shared RAM */
__SHRAM ipr_instance_handle_t ipr_instance_val;

static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	const uint16_t ipv4hdr_length = sizeof(struct ipv4hdr);
	uint16_t ipv4hdr_offset = 0;
	uint8_t *p_ipv4hdr = 0;
	int local_test_error = 0;
	struct ipv4hdr *ipv4_hdr;
	uint32_t fd_length;

	uint8_t ipr_demo_flags = IPR_DEMO_WITH_HM;

	uint32_t ip_dst_addr = 0x73bcdc90; // new ipv4 dst_addr
	uint16_t udp_dst_port = 0xd720; //new udp dest port
	int reassemble_status, hm_status;
	
	parse_result_generate_basic();

	if (PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		fsl_os_print
		("ipr_demo: Core %d received fragment with ipv4 header:\n",
					core_get_id());

		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		p_ipv4hdr = UINT_TO_PTR((ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS()));
		for( int i = 0; i < ipv4hdr_length ;i ++)
		{
			fsl_os_print(" %x",p_ipv4hdr[i]);
		}
		fsl_os_print("\n");
	}

	if (ipr_demo_flags == IPR_DEMO_WITH_HM)
	{
		hm_status = ip_set_nw_dst(ip_dst_addr);
		if (hm_status)
		{
			fsl_os_print("ERROR = %d: ip_set_nw_src\n", hm_status);
			local_test_error |= 1;
		}
		else
			fsl_os_print
			("ipr_demo: Core %d modified fragment's IP dest to 0x%x\n",
				core_get_id(), ip_dst_addr);
	}

	reassemble_status = ipr_reassemble(ipr_instance_val);

	if (reassemble_status == IPR_REASSEMBLY_SUCCESS)
	{
		if (ipr_demo_flags == IPR_DEMO_WITH_HM)
		{
			hm_status = l4_udp_header_modification(
				(L4_UDP_MODIFY_MODE_L4_CHECKSUM | L4_UDP_MODIFY_MODE_UDPDST),
				0, udp_dst_port);
			if (hm_status)
			{
				fsl_os_print("ERROR = %d: l4_udp_header_modification\n", hm_status);
				local_test_error |= 1;
			}
			else
				fsl_os_print
				("ipr_demo: Core %d modified reassembled frame udp dest to 0x%x\n",
					core_get_id(), udp_dst_port);
		}

		fsl_os_print
		("ipr_demo: Core %d will send a reassembled frame with ipv4 header:\n"
					, core_get_id());


		for( int i = 0; i < ipv4hdr_length ;i ++)
		{
			fsl_os_print(" %x",p_ipv4hdr[i]);
		}
		fsl_os_print("\n");

		ipv4_hdr = (struct ipv4hdr *)p_ipv4hdr;
		if (ipv4_hdr->total_length != 0x118b)
		{
			fsl_os_print("ERROR = %x: reassembled frame total length is incorrect\n", ipv4_hdr->total_length);
			local_test_error |= 1;
		}
		if (ipv4_hdr->id != 1)
		{
			fsl_os_print("ERROR = %d: reassembled frame id is incorrect\n", ipv4_hdr->id);
			local_test_error |= 1;
		}
		if (ipv4_hdr->flags_and_offset != 0)
		{
			fsl_os_print("ERROR = %d: reassembled frame flags and offset are incorrect\n", ipv4_hdr->flags_and_offset);
			local_test_error |= 1;
		}
		if (ipv4_hdr->protocol != 17)
		{
			fsl_os_print("ERROR = %d: reassembled frame protocol is incorrect\n", ipv4_hdr->protocol);
			local_test_error |= 1;
		}
		if (ipv4_hdr->src_addr != 0xad793f9f)
		{
			fsl_os_print("ERROR = %x: reassembled frame src address is incorrect\n", ipv4_hdr->src_addr);
			local_test_error |= 1;
		}
		if (ipv4_hdr->dst_addr != 0x73bcdc90)
		{
			fsl_os_print("ERROR = %x: reassembled frame dst address is incorrect\n", ipv4_hdr->dst_addr);
			local_test_error |= 1;
		}
		fd_length = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
		if (fd_length != 4505)
		{
			fsl_os_print("ERROR = %x: reassembled frame length error!\n", fd_length);
			local_test_error |= 1;
		}
		dpni_drv_send(APP_NI_GET(arg));
		
		if(!local_test_error) /*No error found during injection of packets*/
			fsl_os_print("Finished SUCCESSFULLY\n");
		else
			fsl_os_print("Finished with ERRORS\n");
	}

	fdma_terminate_task();
}

#ifdef AIOP_STANDALONE
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
	iowrite32(0, &wrks_addr->epas);
	iowrite32((uint32_t)receive_cb, &wrks_addr->ep_pc);
}
#endif /* AIOP_STANDALONE */



int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	dma_addr_t buff = 0;
	uint64_t tmi_mem_base_addr; 

	struct ipr_params ipr_demo_params;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;

	fsl_os_print("Running app_init()\n");

	ipr_demo_params.max_open_frames_ipv4 = 0x10;
	ipr_demo_params.max_open_frames_ipv6 = 0x10;
	ipr_demo_params.max_reass_frm_size = 0x1000;
	ipr_demo_params.min_frag_size_ipv4 = 0x40;
	ipr_demo_params.min_frag_size_ipv6 = 0x40;
	ipr_demo_params.timeout_value_ipv4 = 0x1000;
	ipr_demo_params.timeout_value_ipv6 = 0x1000;
	ipr_demo_params.ipv4_timeout_cb = 0;
	ipr_demo_params.ipv6_timeout_cb = 0;
	ipr_demo_params.cb_timeout_ipv4_arg = 0;
	ipr_demo_params.cb_timeout_ipv6_arg = 0;
	ipr_demo_params.flags = IPR_MODE_TABLE_LOCATION_PEB;
	tmi_mem_base_addr = fsl_os_virt_to_phys(fsl_os_xmalloc( 0x20*64, MEM_PART_DP_DDR, 64));
	
	tman_create_tmi(tmi_mem_base_addr , 0x20, &ipr_demo_params.tmi_id);

	fsl_os_print("ipr_demo: Creating IPR instance\n");
	err = ipr_create_instance(&ipr_demo_params, ipr_instance_ptr);
	if (err)
	{
		fsl_os_print("ERROR: ipr_create_instance() failed\n");
		return err;
	}

	ipr_instance_val = ipr_instance;

#ifdef AIOP_STANDALONE
	/* This is temporal WA for stand alone demo only */
	epid_setup();
#endif /* AIOP_STANDALONE */

	err = dpni_drv_register_rx_cb(1,
				      app_process_packet_flow0,
				      1);
	
	if (err)
		return err;
	/* inject frag1.pcap till frag4.pcap */
	fsl_os_print("To start test inject packets: \"frag.pcap\"\n");

	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}
