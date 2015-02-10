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
#include "fsl_slab.h"
#include "fsl_malloc.h"
#include "fsl_frame_operations.h"
#include "fsl_ipr.h"
#include "ipr.h"
#include "fsl_l2.h"
#include "fsl_table.h"
#include "fsl_keygen.h"

extern struct  ipr_global_parameters ipr_global_parameters1;
extern __PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];

int app_early_init(void);
int app_init(void);
void app_free(void);
void ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

#define FRAME_SIZE	124

int app_early_init(void){
	//slab_register_context_buffer_requirements(100, 100, 2688, 64, MEM_PART_DP_DDR, 0, 0);
	return 0;
}

int parser_init(uint8_t *prpid);

int parser_init(uint8_t *prpid)
{
    struct parse_profile_input parse_profile1 __attribute__((aligned(16)));
    int i;

	/* Init basic parse profile */
	parse_profile1.parse_profile.eth_hxs_config = 0x0;
	parse_profile1.parse_profile.llc_snap_hxs_config = 0x0;
	parse_profile1.parse_profile.vlan_hxs_config.en_erm_soft_seq_start = 0x0;
	parse_profile1.parse_profile.vlan_hxs_config.configured_tpid_1 = 0x0;
	parse_profile1.parse_profile.vlan_hxs_config.configured_tpid_2 = 0x0;
	/* No MTU checking */
	parse_profile1.parse_profile.pppoe_ppp_hxs_config = 0x0;
	parse_profile1.parse_profile.mpls_hxs_config.en_erm_soft_seq_start= 0x0;
	/* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
	parse_profile1.parse_profile.mpls_hxs_config.lie_dnp = PARSER_PRP_MPLS_HXS_CONFIG_LIE;
	parse_profile1.parse_profile.arp_hxs_config = 0x0;
	parse_profile1.parse_profile.ip_hxs_config = 0x0;
	parse_profile1.parse_profile.ipv4_hxs_config = 0x0;
	/* Routing header is ignored and the destination address from
	 * main header is used instead */
	parse_profile1.parse_profile.ipv6_hxs_config = PARSER_PRP_IPV6_HXS_CONFIG_RHE;
	parse_profile1.parse_profile.gre_hxs_config = 0x0;
	parse_profile1.parse_profile.minenc_hxs_config = 0x0;
	parse_profile1.parse_profile.other_l3_shell_hxs_config= 0x0;
	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.tcp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.udp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	parse_profile1.parse_profile.ipsec_hxs_config = 0x0;
	parse_profile1.parse_profile.sctp_hxs_config = 0x0;
	parse_profile1.parse_profile.dccp_hxs_config = 0x0;
	parse_profile1.parse_profile.other_l4_shell_hxs_config = 0x0;
	parse_profile1.parse_profile.gtp_hxs_config = 0x0;
	parse_profile1.parse_profile.esp_hxs_config = 0x0;
	parse_profile1.parse_profile.l5_shell_hxs_config = 0x0;
	parse_profile1.parse_profile.final_shell_hxs_config = 0x0;
	/* Assuming no soft examination parameters */
	for(i=0; i<16; i++)
	    parse_profile1.parse_profile.soft_examination_param_array[i] = 0x0;

	return parser_profile_create(&(parse_profile1), prpid);
}


int app_init(void)
{
	int        err  = 0;
	uint64_t tmi_mem_base_addr;
	struct ipr_params ipr_demo_params;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;
	struct ipr_instance	ipr_instance_read;
	struct slab *slab_handle = NULL;
	uint8_t *prpid;
	uint64_t cipher_key_addr;
	uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	uint8_t cipher_key_read[16];
	int i;
	
	fsl_os_print("Running simple bring-up test\n");
	
	
	parser_init(prpid);

	default_task_params.parser_profile_id = *prpid;
	default_task_params.parser_starting_hxs = 0;

	ipr_demo_params.max_open_frames_ipv4 = 0x10;
	ipr_demo_params.max_open_frames_ipv6 = 0x10;
	ipr_demo_params.max_reass_frm_size = 0xf000;
	ipr_demo_params.min_frag_size_ipv4 = 0x40;
	ipr_demo_params.min_frag_size_ipv6 = 0x40;
	ipr_demo_params.timeout_value_ipv4 = 0xffe0;
	ipr_demo_params.timeout_value_ipv6 = 0xffe0;
	ipr_demo_params.ipv4_timeout_cb = ipr_timout_cb;
	ipr_demo_params.ipv6_timeout_cb = ipr_timout_cb;
	ipr_demo_params.cb_timeout_ipv4_arg = 0x66;
	ipr_demo_params.cb_timeout_ipv6_arg = 0x66;
	ipr_demo_params.flags = IPR_MODE_TABLE_LOCATION_PEB;
	fsl_os_get_mem( 0x20*64, MEM_PART_DP_DDR, 64, &tmi_mem_base_addr);

	//tman_create_tmi(tmi_mem_base_addr , 0x20, &ipr_demo_params.tmi_id);

	fsl_os_print("bring-up test: Creating IPR instance\n");
	err = ipr_create_instance(&ipr_demo_params, ipr_instance_ptr);
	if (err)
	{
		fsl_os_print("ERROR: ipr_create_instance() failed %d\n",err);
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
		fsl_os_print("ERROR:ipr instance was not read successfully\n");
		return err;
	}
	else
		fsl_os_print("Simple BU Test: ipr instance was  read successfully\n");
	
	/* Allocate buffers for the Keys */
	err = slab_create(
			10, /* uint32_t    num_buffs */
			10, /* uint32_t    max_buffs */
			512, /* uint16_t    buff_size */
			8, /*uint16_t    alignment */
			MEM_PART_DP_DDR, /* uint8_t     mem_partition_id */
			0, /* uint32_t    flags */
			NULL, /* slab_release_cb_t release_cb */
			&slab_handle /* struct slab **slab */
			);

	if (err)
		fsl_os_print("ERROR: slab_create() failed\n");	
	else
		fsl_os_print("slab_create() completed successfully\n");	
	
	/* Acquire the Cipher key buffer */
	err = slab_acquire(
			slab_handle, /* struct slab *slab */
			&cipher_key_addr /* uint64_t *buff */
			);

	/* Copy the Keys to external memory with CDMA */
	cdma_write(
			cipher_key_addr, /* ext_address */
			&cipher_key, /* ws_src */
			16); /* uint16_t size */
	cdma_read(&cipher_key_read, cipher_key_addr, 16);
	
	for (i=0; i<16 ; i++)
		if (cipher_key_read[i] != cipher_key[i])
			err = -EINVAL;
	if (err)
	{
		fsl_os_print("ERROR: key info was not read successfully\n");
		return err;
	}
	else
		fsl_os_print("Simple BU Test: IPsec key was  read successfully\n");

	// run create_frame on default frame
	{
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
		uint8_t frame_data_read[FRAME_SIZE+4] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,\
				0x10,0x94,0x00,0x00,0x02,0x81,0x00,0xaa,0xbb,0x08,0x00,0x45,0x00,\
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
		uint32_t vlan = 0x8100aabb;
		int parse_status;
		uint8_t *frame_presented;
		uint8_t *seg_addr;
		struct fdma_amq amq;
		uint16_t icid, flags = 0;
		uint8_t tmp;
		uint32_t frame_length;
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
			fsl_os_print("storage profile arg %d: 0x%x \n", i, *((uint32_t *)(&(storage_profile[0]))+i));
		
		
		err = create_frame(fd, frame_data, FRAME_SIZE, &frame_handle);
		if (err)
			fsl_os_print("ERROR: create frame failed!\n");

		
		fsl_os_print("parse result before create frame - \n");
		
		fsl_os_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
		
		fsl_os_print("vlan offset %d %x\n",
					PARSER_IS_ONE_VLAN_DEFAULT(), PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT());
		
		fsl_os_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
		
		fsl_os_print("udp offset %d %x\n", 
					PARSER_IS_UDP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());

		for (i=0; i<16 ; i++)
			fsl_os_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));
		
		
		fdma_close_default_segment();
		err = fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS, (void *)0x180, 0, 256);
		fsl_os_print("STATUS: fdma present default segment returned status is %d\n", err);
		l2_push_and_set_vlan(vlan);
		
		frame_length = PRC_GET_SEGMENT_LENGTH();
		seg_addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
		
		fsl_os_print("frame length is 0x%x\n", frame_length);
		for (i=0; i<frame_length ; i++)
			fsl_os_print("frame read byte %d is %x\n", i, seg_addr[i]);

		parse_result_generate(PARSER_ETH_STARTING_HXS, 0, PARSER_NO_FLAGS);
		
		fsl_os_print("parse result after create frame - \n");
		
		fsl_os_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
		
		fsl_os_print("vlan offset %d %x\n",
					PARSER_IS_ONE_VLAN_DEFAULT(), PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT());
		
		fsl_os_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
		
		fsl_os_print("udp offset %d %x\n", 
					PARSER_IS_UDP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());
		
		err = fdma_store_default_frame_data();
		if (err)
			fsl_os_print("ERROR: fdma store default frame returned error is %d\n", err);
		
		fsl_os_print(" FD length is : 0x%x \n", LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
		fsl_os_print(" FD address is LSB : 0x%x \n", LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS));
		fsl_os_print(" FD address is MSB : 0x%x \n", LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS) >> 32);

		err = fdma_present_default_frame();
		if (err < 0)
			fsl_os_print("ERROR: fdma present default frame returned error is %d\n", err);
		else
			if (err)
				fsl_os_print("STATUS: fdma present default frame returned status is %d\n", err);
		parse_status = parse_result_generate_default(PARSER_NO_FLAGS);
		if (parse_status)
		{
			fsl_os_print("ERROR: parser failed for simple BU test!\n");
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		}
		/* check frame presented */
		frame_presented = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
		frame_length = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
		fsl_os_print("actual frame length is 0x%x\n", frame_length);
		for (i=0; i<frame_length ; i++)
			fsl_os_print("actual frame read byte %d is %x\n", i, frame_presented[i]);
		
		for (i=0; i<(FRAME_SIZE+4); i++)
			if (*(frame_presented+i) != frame_data_read[i])
				err = -EINVAL;
		if (err)
		{
			fsl_os_print("Simple BU ERROR: frame data after HM is not correct\n");
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			return err;
		}
		else
			fsl_os_print("Simple BU Test: fdma frame after HM is correct\n");
		/* CTLU */
		{
			struct table_rule rule;
			uint8_t	 keysize;
			int sr_status;
			struct table_lookup_result lookup_result;
			
			rule.options = 0;
			rule.result.type = TABLE_RESULT_TYPE_REFERENCE;
			rule.result.op0_rptr_clp.reference_pointer = 0x12345678;
			
			keygen_gen_key(
				KEYGEN_ACCEL_ID_CTLU,
				ipr_global_parameters1.ipr_key_id_ipv4,
				0,
				&rule.key_desc,
				&keysize);
			sr_status = table_rule_create(
					TABLE_ACCEL_ID_CTLU,
					ipr_instance_read.table_id_ipv4,
					&rule,
					keysize);
			
			if (sr_status)
				fsl_os_print("Simple BU ERROR: rule create failed!\n");

			sr_status = table_lookup_by_keyid_default_frame(
				TABLE_ACCEL_ID_CTLU,
				ipr_instance_read.table_id_ipv4,
				ipr_global_parameters1.ipr_key_id_ipv4,
				&lookup_result);
			
			if (lookup_result.opaque0_or_reference != 0x12345678)
				fsl_os_print("Simple BU ERROR: table LU failed!\n");
			else
				fsl_os_print("Simple BU table LU success!!!\n");
		}
		
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	}
	fsl_os_print("Simple bring-up test completed successfully\n");
	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}

void ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);
	fdma_terminate_task();
}
