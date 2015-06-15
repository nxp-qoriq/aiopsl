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

#include "simple_bu_test.h"

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


int simple_bu_ilan_test(void)
{
	int        err  = 0;
	uint64_t tmi_mem_base_addr;
	struct ipr_params ipr_demo_params;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;
	struct ipr_instance	ipr_instance_read;
	struct slab *slab_handle = NULL;
	uint8_t prpid;
	uint64_t cipher_key_addr;
	uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	uint8_t cipher_key_read[16];
	int i;
	
	fsl_os_print("Running simple bring-up test\n");
	
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
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
		/*struct fdma_amq amq;
		uint16_t icid, flags = 0;
		uint8_t tmp;*/
		uint32_t frame_length;
		/* setting SPID = 0 */
		sl_tman_expiration_task_prolog(0);
		/*
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
		*(uint32_t */

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
		
		/* due to parser not aligned segment WA need to represent again*/
		fdma_close_default_segment();
		err = fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS, (void *)0x180, 0, 256);
		fsl_os_print("STATUS: fdma present default segment returned status is %d\n", err);
		
		frame_length = PRC_GET_SEGMENT_LENGTH();
		seg_addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
		
		fsl_os_print("frame length is 0x%x\n", frame_length);
		for (i=0; i<frame_length ; i++)
			fsl_os_print("frame read byte %d is %x\n", i, seg_addr[i]);

		parse_result_generate_default(0);
		
		fsl_os_print("parse result after create frame - \n");
		
		fsl_os_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
		
		fsl_os_print("vlan offset %d %x\n",
					PARSER_IS_ONE_VLAN_DEFAULT(), PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT());
		
		fsl_os_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
		
		fsl_os_print("udp offset %d %x\n", 
					PARSER_IS_UDP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());
		fsl_os_print(" FD length (by SW) is : 0x%x \n", LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));

		LDPAA_FD_SET_LENGTH(HWC_FD_ADDRESS, 0);
		fsl_os_print(" FD length (after SW zeroing it) is : 0x%x \n", LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
		err = fdma_store_default_frame_data();
		if (err)
			fsl_os_print("ERROR: fdma store default frame returned error is %d\n", err);
		
		for (i=0; i<8 ; i++)
			fsl_os_print("FD content arg %d is %x\n", i, *((uint32_t *)(0x60 + i*4)));
		
		fsl_os_print(" FD length is : 0x%x \n", LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
		fsl_os_print(" FD address LSB is : 0x%x \n", LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS));
		fsl_os_print(" FD address MSB is : 0x%x \n", ((uint64_t)LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS)) >> 32);

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
			fsl_os_print("Simple BU ERROR: frame data after HM vlan add is not correct\n");
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			return err;
		}
		else {
			fsl_os_print("**************************************************\n");
			fsl_os_print("Simple BU Test: fdma frame after HM vlan add is correct !!!\n");
			fsl_os_print("**************************************************\n");
		}
		
		/* Add vlan remove feature here! */
		err = fdma_store_default_frame_data();
		if (err)
			fsl_os_print("ERROR: fdma store default frame returned error is %d\n", err);
		
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
		l2_pop_vlan();
		
		/* due to parser not aligned segment WA need to represent again*/
		fdma_close_default_segment();
		err = fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS, (void *)0x180, 0, 256);
		fsl_os_print("STATUS: fdma present default segment returned status is %d\n", err);	
		err = 0;
		parse_status = parse_result_generate_default(PARSER_NO_FLAGS);
		if (parse_status)
		{
			fsl_os_print("ERROR: parser failed for simple BU test!\n");
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		}
		
		
		/* check frame presented */
		frame_presented = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
		frame_length = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
		fsl_os_print("actual frame length after vlan removed is 0x%x\n", frame_length);
		for (i=0; i<frame_length ; i++)
			fsl_os_print("actual frame read byte after vlan remove %d is %x\n", i, frame_presented[i]);
		
		for (i=0; i<FRAME_SIZE; i++)
			if (*(frame_presented+i) != frame_data[i])
			{
				fsl_os_print("***************** index %d\n", i);
				err = -EINVAL;
			}
		if (err)
		{
			fsl_os_print("Simple BU ERROR: frame data after HM vlan remove is not correct\n");
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			return err;
		}
		else {
			fsl_os_print("**************************************************\n");
			fsl_os_print("Simple BU Test: fdma frame after HM vlan remove is correct !!!\n");
			fsl_os_print("**************************************************\n");
		}
		
		/* CTLU */
		{
			struct table_rule rule __attribute__((aligned(16)));
			uint8_t	 keysize;
			int sr_status;
			struct table_lookup_result lookup_result
								__attribute__((aligned(16)));
			
			rule.options = 0;
			rule.result.type = TABLE_RESULT_TYPE_REFERENCE;
			rule.result.op0_rptr_clp.reference_pointer = 0x12345678;
			
			sr_status = keygen_gen_key(
				KEYGEN_ACCEL_ID_CTLU,
				ipr_global_parameters1.ipr_key_id_ipv4,
				0,
				&rule.key_desc,
				&keysize);
			if (sr_status)
				fsl_os_print("Simple BU ERROR: keygen_gen_key failed!\n");
			
			for (i=0; i<keysize; i++)
				fsl_os_print("Generated Key of size %d byte %d 0x%x\n", keysize, i, rule.key_desc.em.key[i]);
			
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
			
			if (sr_status)
				fsl_os_print("Simple BU ERROR: table_lookup_by_keyid_default_frame failed!\n");
			
			if (lookup_result.opaque0_or_reference != 0x12345678)
				fsl_os_print("Simple BU ERROR: table LU failed!\n");
			else
				fsl_os_print("Simple BU table LU success!!!\n");
		}
		
		/* look-up by key */
		{
			//struct kcr_builder kb
			//		__attribute__((aligned(16)));
			struct table_rule rule
							__attribute__((aligned(16)));
			struct table_rule rule_replace
							__attribute__((aligned(16)));
			union table_lookup_key_desc key_desc;
			struct table_create_params tbl_params;
			uint16_t table_location_attr;
			uint32_t timestamp;
			//uint8_t key_id;
			uint16_t table_id;
			
			uint16_t key1
										__attribute__((aligned(16)));
			uint16_t key2
										__attribute__((aligned(16)));
			uint16_t key3
										__attribute__((aligned(16)));
			struct table_result table_result;
			
			struct table_result new_miss_result; // Yariv
			struct table_result old_miss_result;
			
			uint32_t op0, op1, op2;

			/*
			miss_result.type = TABLE_RESULT_TYPE_OPAQUES;
			miss_result.reserved = 0;
			miss_result.opaque1 = 0x8765;
			miss_result.opaque2 = 0x43;
			miss_result.op0_rptr_clp.opaque0 = 0xfedc;
			*/
			
			key1=0xbeef;
			key2=0xdead;
			key3=0x1234;
			
			int sr_status;
			struct table_lookup_result lookup_result
							__attribute__((aligned(16)));

			/* keygen_kcr_builder_init(&kb);*/
			//keygen_kcr_builder_add_input_value_fec(6/*offset*/, 
			//			2/*extract size*/, NULL , &kb );
			/* err = keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU,
					  kb.kcr,
					  &key_id);
			if (err < 0)
			{
				fsl_os_print("key id kcr create failed!");
				return err;
			}
			*/
			
			tbl_params.committed_rules = 10;
			tbl_params.max_rules = 10;
			tbl_params.key_size = 2;
			
			// tbl_params.miss_result = miss_result; // Yariv
			tbl_params.miss_result.type = TABLE_RESULT_TYPE_REFERENCE;
			tbl_params.miss_result.reserved = 0;
			tbl_params.miss_result.opaque1 = 0x8765;
			tbl_params.miss_result.opaque2 = 0x43;
			tbl_params.miss_result.op0_rptr_clp.opaque0 = 0xfedc;
			
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_DP_DDR;

			tbl_params.attributes = TABLE_ATTRIBUTE_TYPE_EM | \
					table_location_attr | \
					//TABLE_ATTRIBUTE_MR_NO_MISS;
					TABLE_ATTRIBUTE_MR_MISS; // Yariv
			err = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params,
					&table_id);
			if (err != TABLE_STATUS_SUCCESS)
			{
				fsl_os_print("table create failed for key-id!");
				return err;
			}
			/* create rule */
			rule.options = 0;
			rule.result.type = TABLE_RESULT_TYPE_REFERENCE;
			rule.result.op0_rptr_clp.reference_pointer = 0x11223344;
			rule.key_desc.em.key[0] = 0xde;
			rule.key_desc.em.key[1] = 0xad;
			rule_replace = rule;
			rule_replace.result.op0_rptr_clp.reference_pointer = 0xaabbccdd;
			
			err = table_rule_create(TABLE_ACCEL_ID_CTLU, table_id, &rule, 2);
			if (err)
			{
				fsl_os_print("table rule create by key  failed");
				return err;
			}
			rule.result.op0_rptr_clp.reference_pointer = 0x55667788;
			rule.key_desc.em.key[0] = 0xbe;
			rule.key_desc.em.key[1] = 0xef;
			err = table_rule_create(TABLE_ACCEL_ID_CTLU, table_id, &rule, 2);
			if (err)
			{
				fsl_os_print("table rule create by key  failed");
				return err;
			}

			/* table_rule_query */
				
			err = table_rule_query(TABLE_ACCEL_ID_CTLU, table_id, &rule.key_desc, 2, &table_result, &timestamp);
			if (table_result.op0_rptr_clp.reference_pointer != 0x55667788)
			{
				fsl_os_print("Simple BU ERROR: table_rule_query failed!\n");
				return -EIO;
			}
			else
				fsl_os_print("Simple BU: table_rule_query succeeded!\n");
			/*********************************/
			
			
			/* rule create and rule delete */
			
			rule.result.op0_rptr_clp.reference_pointer = 0x55667788;
			rule.key_desc.em.key[0] = 0x12;
			rule.key_desc.em.key[1] = 0x34;
			err = table_rule_create(TABLE_ACCEL_ID_CTLU, table_id, &rule, 2);
			if (err)
			{
				fsl_os_print("table rule create by key  failed");
				return err;
			}
			
			err = table_rule_delete(TABLE_ACCEL_ID_CTLU, table_id, &rule.key_desc, 2 , &rule.result);
			if (err)
			{
				fsl_os_print("Simple BU ERROR: table_rule_delete failed!\n");
				return -EIO;
			}
			else
				fsl_os_print("Simple BU: table_rule_delete succeeded!\n");
			/*********************************/
			
	
			
			key_desc.em_key = &key1;
			sr_status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU, table_id, key_desc, 2, &lookup_result);
			if (sr_status)
			{
				fsl_os_print("Simple BU ERROR: table_lookup_by_key failed!\n");
				return -EIO;
			}
							
			if (lookup_result.opaque0_or_reference != 0x55667788)
			{
					fsl_os_print("Simple BU ERROR: table LU by key1 failed!\n");
					return -EIO;
			}
			else
					fsl_os_print("Simple BU table LU by Key1 success!!!\n");
			
			key_desc.em_key = &key2;
			sr_status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU, table_id, key_desc, 2, &lookup_result);
			if (sr_status)
			{
				fsl_os_print("Simple BU ERROR: table_lookup_by_key failed!\n");
				return -EIO;
			}
							
			if (lookup_result.opaque0_or_reference != 0x11223344)
			{
					fsl_os_print("Simple BU ERROR: table LU by key2 failed!\n");
					return -EIO;
			}
			else
					fsl_os_print("Simple BU table LU by Key2 success!!!\n");
			
			/* table_rule_replace for key 0xdead */
			
			err = table_rule_replace(TABLE_ACCEL_ID_CTLU, table_id, &rule_replace, 2, NULL);
			if (err)
			{
				fsl_os_print("Simple BU ERROR: table rule replace command failed!\n");
				return err;
			}
			else
				fsl_os_print("Simple BU: table rule replace command success!\n");
			
			key_desc.em_key = &key2;
			sr_status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU, table_id, key_desc, 2, &lookup_result);
			if (sr_status)
			{
				fsl_os_print("Simple BU ERROR: table_lookup_by_key failed!\n");
				return -EIO;
			}
							
			if (lookup_result.opaque0_or_reference != 0xaabbccdd)
			{
					fsl_os_print("Simple BU ERROR: table LU by key2 failed after replace!\n");
					return -EIO;
			}
			else
					fsl_os_print("Simple BU table LU by Key2 after replace success!!!\n");

			op0 = (uint32_t)lookup_result.opaque0_or_reference;
			op1 = (uint32_t)lookup_result.opaque1;
			op2 = (uint32_t)lookup_result.opaque2;
			fsl_os_print("\n");
			fsl_os_print("lookup_result.opaque0_or_reference = 0x%x\n", op0);
			fsl_os_print("lookup_result.opaque1 = 0x%x\n", op1);
			fsl_os_print("lookup_result.opaque2 = 0x%x\n", op2);
			fsl_os_print("\n");
			
			/**************************************/
			
			key_desc.em_key = &key3;
			sr_status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU, table_id, key_desc, 2, &lookup_result);
			if (!sr_status)
			{
				fsl_os_print("Simple BU ERROR: table_lookup_by_key failed since it should be miss!\n");
				return -EIO;
			}
			
			op0 = (uint32_t)lookup_result.opaque0_or_reference;
			op1 = (uint32_t)lookup_result.opaque1;
			op2 = (uint32_t)lookup_result.opaque2;
			fsl_os_print("\n");
			fsl_os_print("Original Miss Results:\n");
			fsl_os_print("lookup_result.opaque0_or_reference = 0x%x\n", op0);
			fsl_os_print("lookup_result.opaque1 = 0x%x\n", op1);
			fsl_os_print("lookup_result.opaque2 = 0x%x\n", op2);
			fsl_os_print("\n");
			
			/* table_replace_miss_result() */
			new_miss_result.type = TABLE_RESULT_TYPE_REFERENCE;
			new_miss_result.reserved = 0;
			new_miss_result.opaque1 = 0x9966;
			new_miss_result.opaque2 = 0xee;
			new_miss_result.op0_rptr_clp.opaque0 = 0xaabb;
						
			table_replace_miss_result(
					TABLE_ACCEL_ID_CTLU, /* enum table_hw_accel_id acc_id */
					table_id, /* uint16_t table_id */
					&new_miss_result, /*	struct table_result *new_miss_result */
					&old_miss_result); /* struct table_result *old_miss_result */
			
			key_desc.em_key = &key3;
			sr_status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU, table_id, key_desc, 2, &lookup_result);
			if (!sr_status)
			{
				fsl_os_print("Simple BU ERROR: table_lookup_by_key failed since it should be miss!\n");
				return -EIO;
			}
					
			op0 = (uint32_t)lookup_result.opaque0_or_reference;
			op1 = (uint32_t)lookup_result.opaque1;
			op2 = (uint32_t)lookup_result.opaque2;
			fsl_os_print("\n");
			fsl_os_print("New Miss Results:\n");
			fsl_os_print("lookup_result.opaque0_or_reference = 0x%x\n", op0);
			fsl_os_print("lookup_result.opaque1 = 0x%x\n", op1);
			fsl_os_print("lookup_result.opaque2 = 0x%x\n", op2);
			fsl_os_print("\n");
			
			if ((op0 == 0xaabb) && (op1 == 0x9966) && (op2 == 0xee)) {
				fsl_os_print("table_replace_miss_result() PASSED\n");
			} else {
				fsl_os_print("Simple BU ERROR: table_replace_miss_result() failed\n");
				return -EIO;
			}
				
			/* table_delete */
			table_delete(TABLE_ACCEL_ID_CTLU, table_id);
			
		}
		
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	}
	fsl_os_print("Simple bring-up test completed successfully\n");
	return 0;
}

void ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);
	fdma_terminate_task();
}

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

