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
#include "common/fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
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
#include "fsl_frame_operations.h"
#include "fsl_ipr.h"
#include "fsl_ipf.h"
#include "fsl_ste.h"
#include "ipr.h"
#include "fsl_l2.h"
#include "fsl_table.h"
#include "fsl_keygen.h"
#include "fsl_platform.h"
#include "simple_bu_test.h"

#include "fsl_aiop_common.h"

extern struct  ipr_global_parameters ipr_global_parameters1;
extern __PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];


#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

#define FRAME_SIZE	124
#define	COPY_SIZE	16
#define AIOP_SP_BDI     0x00080000
#define SP_BP_PBS_MASK  0x3FFF


int simple_bu_ipf_ipr_test(void)
{
	int        err  = 0;
	uint64_t tmi_mem_base_addr;
	struct ipr_params ipr_demo_params;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;
	struct ipr_instance	ipr_instance_read;
	struct slab *slab_handle = NULL;
	uint8_t prpid;
	int i;
	uint16_t mtu = 80;
	int ipf_status, reassemble_status;
	uint8_t *frame_presented;
	uint32_t frame_length;
	//uint32_t ste_status;
	
	// run create_frame on default frame
	struct ldpaa_fd *fd = (struct ldpaa_fd *)HWC_FD_ADDRESS;
	uint8_t frame_data[FRAME_SIZE] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,\
			0x10,0x94,0x00,0x00,0x02,0x08,0x00,0x45,0x00,\
			0x00,0x6e,0x00,0x00,0x00,0x00,0xff,0x11,0x3a,\
			0x26,0xc0,0x55,0x01,0x02,0xc0,0x00,0x00,0x01,\
			0x04,0x00,0x04,0x00,0x00,0x5a,0xff,0xff,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf5,0xd0,\
			0x64,0x51,0xac,0x9f,0x69,0xd4,0xd3,0xf7,0x39,\
			0x6e,0x20,0x0e,0x97,0xb7,0xe9,0xe4,0x56,0x3a};


	uint8_t frame_handle;
	ipf_ctx_t ipf_context_addr
		__attribute__((aligned(sizeof(struct ldpaa_fd))));
	

	struct fdma_amq amq;
	uint16_t icid, flags = 0;
	uint8_t tmp;
	
	
	fsl_print("Running simple bring-up test\n");
	
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
	default_task_params.parser_starting_hxs = 0;
	default_task_params.current_scope_level = 1;

	default_task_params.scope_mode_level_arr[0] = EXCLUSIVE;

	ipr_demo_params.max_open_frames_ipv4 = 0x10;
	ipr_demo_params.max_open_frames_ipv6 = 0x10;
	ipr_demo_params.max_reass_frm_size = 0xf000;
	ipr_demo_params.min_frag_size_ipv4 = 40;
	ipr_demo_params.min_frag_size_ipv6 = 40;
	ipr_demo_params.timeout_value_ipv4 = 0xffe0;
	ipr_demo_params.timeout_value_ipv6 = 0xffe0;
	ipr_demo_params.ipv4_timeout_cb = ipr_timout_cb;
	ipr_demo_params.ipv6_timeout_cb = ipr_timout_cb;
	ipr_demo_params.cb_timeout_ipv4_arg = 0x66;
	ipr_demo_params.cb_timeout_ipv6_arg = 0x66;
	ipr_demo_params.flags = IPR_MODE_TABLE_LOCATION_EXT1;
	fsl_get_mem( 0x20*64, MEM_PART_DP_DDR, 64, &tmi_mem_base_addr);

	tman_create_tmi(tmi_mem_base_addr , 0x20, &ipr_demo_params.tmi_id);

	fsl_print("bring-up test: Creating IPR instance\n");
	err = ipr_create_instance(&ipr_demo_params, ipr_instance_ptr);
	if (err)
	{
		fsl_print("ERROR: ipr_create_instance() failed %d\n",err);
		return err;
	}
	
	/* cdma read the data and check if it is correct */
	cdma_read(&ipr_instance_read, ipr_instance, IPR_INSTANCE_SIZE);
	if (ipr_instance_read.extended_stats_addr != ipr_demo_params.extended_stats_addr)
		err = -EINVAL;

	if (ipr_instance_read.max_reass_frm_size != ipr_demo_params.max_reass_frm_size)
		err = -EINVAL;
	if (ipr_instance_read.min_frag_size_ipv4 != ipr_demo_params.min_frag_size_ipv4)
			err = -EINVAL;
	if (ipr_instance_read.min_frag_size_ipv6 != ipr_demo_params.min_frag_size_ipv6)
			err = -EINVAL;
	if (ipr_instance_read.timeout_value_ipv4 != ipr_demo_params.timeout_value_ipv4)
			err = -EINVAL;
	if (ipr_instance_read.timeout_value_ipv6 != ipr_demo_params.timeout_value_ipv6)
			err = -EINVAL;
	if (ipr_instance_read.ipv4_timeout_cb != ipr_demo_params.ipv4_timeout_cb)
			err = -EINVAL;
	if (ipr_instance_read.ipv6_timeout_cb != ipr_demo_params.ipv6_timeout_cb)
			err = -EINVAL;
	if (ipr_instance_read.cb_timeout_ipv4_arg != ipr_demo_params.cb_timeout_ipv4_arg)
			err = -EINVAL;
	if (ipr_instance_read.cb_timeout_ipv6_arg != ipr_demo_params.cb_timeout_ipv6_arg)
			err = -EINVAL;
	if (ipr_instance_read.tmi_id != ipr_demo_params.tmi_id)
			err = -EINVAL;

	if (err)
	{
		fsl_print("ERROR:ipr instance was not read successfully\n");
		return err;
	}
	else
		fsl_print("Simple BU Test: ipr instance was  read successfully\n");

	/* setting SPID = 0 */
	*((uint8_t *)HWC_SPID_ADDRESS) = 0;
	icid = (uint16_t)(storage_profile[0].ip_secific_sp_info >> 48);
	icid = ((icid << 8) & 0xff00) | ((icid >> 8) & 0xff);
	tmp = (uint8_t)(storage_profile[0].ip_secific_sp_info >> 40);
	if (tmp & 0x08)
		flags |= FDMA_ICID_CONTEXT_BDI;
	if (tmp & 0x04)
		flags |= FDMA_ICID_CONTEXT_PL;
	if (storage_profile[0].mode_bits2 & sp1_mode_bits2_VA_MASK)
		flags |= FDMA_ICID_CONTEXT_VA;
	amq.icid = icid;
	amq.flags = flags;
	set_default_amq_attributes(&amq);
	*(uint32_t *)(&storage_profile[0].pbs2) = *(uint32_t *)(&storage_profile[0].pbs1);

	for (i=0; i<8 ; i++)
		fsl_print("storage profile arg %d: 0x%x \n", i, *((uint32_t *)(&(storage_profile[0]))+i));
	
	
	err = create_frame(fd, frame_data, FRAME_SIZE, &frame_handle);
	if (err)
		fsl_print("ERROR: create frame failed!\n");

	
	fsl_print("parse result before create frame - \n");
	
	fsl_print("ethernet offset %d %x\n", 
				PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
	
	fsl_print("vlan offset %d %x\n",
				PARSER_IS_ONE_VLAN_DEFAULT(), PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT());
	
	fsl_print("ipv4 offset %d %x\n", 
				PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	
	fsl_print("udp offset %d %x\n", 
				PARSER_IS_UDP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());

	for (i=0; i<16 ; i++)
		fsl_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));
	
	
	fdma_close_default_segment();
	err = fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS, (void *)0x180, 0, 256);
	fsl_print("STATUS: fdma present default segment returned status is %d\n", err);

	ipf_context_init(0, mtu, ipf_context_addr);
	fsl_print("ipf_demo: ipf_context_init done, MTU = %d\n", mtu);
	
	ipf_status = ipf_generate_frag(ipf_context_addr);
	if (ipf_status < 0)
	{
		fsl_print("ipf_demo: error returned from ipf\n");
		return ipf_status;
	}
	
	if (ipf_status == IPF_GEN_FRAG_STATUS_IN_PROCESS)
		fsl_print("ipf_demo: first fragment in process\n");
	else
	{
		fsl_print("ipf_demo: should be first fragment why completed - failed!\n");
		return ipf_status;
	}
	
	reassemble_status = ipr_reassemble(ipr_instance);
	/*ste_status = STE_GET_STATUS_REGISTER();
	fsl_print("ste status: STE_GET_STATUS_REGISTER = %x \n", ste_status);
	ste_status = STE_GET_ERR_CAP_ATTRIBUTES();
	fsl_print("ste status: STE_GET_ERR_CAP_ATTRIBUTES = %x \n", ste_status);*/
	
	if (reassemble_status == IPR_REASSEMBLY_NOT_COMPLETED)
	{
		fsl_print("ipr_demo: first fragment for IPR\n");
	}
	else
	{
		fsl_print("ipr_demo: should be first fragment for IPR instead of status %x - failed\n", reassemble_status);
		return  reassemble_status;
	}
	
	/* for second fragment */
	ipf_status = ipf_generate_frag(ipf_context_addr);
	if (ipf_status < 0)
	{
		fsl_print("ipf_demo: error returned from ipf\n");
		return ipf_status;
	}
	
	if (ipf_status == IPF_GEN_FRAG_STATUS_DONE)
		fsl_print("ipf_demo: second and last fragment in process\n");
	else
	{
		fsl_print("ipf_demo: should be last fragment and status returned is %x - failed!\n", ipf_status);
		return ipf_status;
	}
	
	reassemble_status = ipr_reassemble(ipr_instance);
	if (reassemble_status == IPR_REASSEMBLY_SUCCESS)
	{
		fsl_print("ipr_demo: last fragment for IPR\n");
	}
	else
	{
		fsl_print("ipr_demo: should be last fragment for IPR instead of status %x - failed\n", reassemble_status);
		return  reassemble_status;
	}
	
	/* check frame presented */
	frame_presented = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	frame_length = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	fsl_print("actual frame length is 0x%x\n", frame_length);
	if (frame_length != FRAME_SIZE)
	{	
		fsl_print("actual frame length after reassemble is wrong 0x%x\n", frame_length);
		return -EINVAL;
	}
	
	for (i=0; i<frame_length; i++)
		if (*(frame_presented+i) != frame_data[i])
		{
			fsl_print("Error in frame after reassembly:  index %d\n", i);
			err = -EINVAL;
		}
	
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	
	fsl_print("Simple bring-up test completed successfully\n");
	return 0;
}



