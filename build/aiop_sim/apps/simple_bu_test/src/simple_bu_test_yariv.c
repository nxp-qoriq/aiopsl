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


int simple_bu_yariv_test(void)
{
	int        err  = 0;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;
	struct slab *slab_handle = NULL;
	uint8_t prpid;
	uint64_t cipher_key_addr;
	uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	uint8_t cipher_key_read[16];
	int i;
	uint8_t new_l2_dst[6] = {0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
	uint8_t new_l2_src[6] = {0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
	uint32_t ref_cnt_val = 0xFF;
	uint32_t expected_ref_cnt_val;
	
	fsl_os_print("Running simple bring-up test\n");
	
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
	default_task_params.parser_starting_hxs = 0;

	
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
		fsl_os_print("ERROR: CDMA read mismatch\n");
		return err;
	}
	else
		fsl_os_print("Simple BU Test: CDMA read successfully\n");

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
		//int parse_status;
		//uint8_t *frame_presented;
		//uint8_t *seg_addr;
		struct fdma_amq amq;
		uint16_t icid, flags = 0;
		uint8_t tmp;
		//uint32_t frame_length;
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
		
		
				
		/* Header Modification L2 DST/SRC set Test */ 

		fsl_os_print("\nTesting l2_set_dl_dst\n");
		
		err = 0;
		fsl_os_print("Original L2 dest = ");
		for (i=0; i<6 ; i++) {
			fsl_os_print("0x%x", *((uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT() + (uint8_t)i));
		}
		fsl_os_print("\n");
		
		l2_set_dl_dst(new_l2_dst); /* (uint8_t *dst_addr) */
		
		fsl_os_print("New L2 dest = ");
		for (i=0; i<6 ; i++) {
			fsl_os_print("0x%x", *((uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT() + (uint8_t)i));
			if (*((uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT() + (uint8_t)i) != new_l2_dst[i]) {
				err = 1;
				fsl_os_print("\nERROR: L2 DST mismatch = 0x%x\n");
			}
		}
		fsl_os_print("\n");
		
		fsl_os_print("\nTesting l2_set_dl_src\n");
		fsl_os_print("Original L2 src = ");
		for (i=0; i<6 ; i++) {
			fsl_os_print("0x%x", *((uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT() + (uint8_t)i + 6));
		}
		fsl_os_print("\n");
		
		l2_set_dl_src(new_l2_src); /* uint8_t *src_addr */

		fsl_os_print("New L2 src = ");
		for (i=0; i<6 ; i++) {
			fsl_os_print("0x%x", *((uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT() + (uint8_t)i + 6));
			if (*((uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT() + (uint8_t)i + 6) != new_l2_src[i]) {
				err = 1;
				fsl_os_print("\nERROR: L2 SRC mismatch = 0x%x\n");
			}
		}
		fsl_os_print("\n");
		
		if (err) {
			fsl_os_print("ERROR:  Header Modification L2 DST/SRC set test failed \n");
		} else {
			fsl_os_print("Header Modification L2 DST/SRC set test PASSED :-)\n");
		}
		
		/* End of Header Modification L2 DST/SRC set Test */ 

		
		/* CDMA Reference Count Increment/Decrement Test */ 
		err = 0;
		fsl_os_print("\nTesting CDMA Reference Count Increment/Decrement\n");
		
		/* Using the "cipher_key_addr" buffer which was allocated earlier */
		
		cdma_refcount_get(
				cipher_key_addr, /* uint64_t context_address */
				&expected_ref_cnt_val); /* uint32_t *refcount_value) */
				
		fsl_os_print("Original Ref count value = %d\n", expected_ref_cnt_val);

		/* Increment Twice */
		cdma_refcount_increment(cipher_key_addr); /* uint64_t context_address) */
		cdma_refcount_increment(cipher_key_addr); /* uint64_t context_address) */
		
		expected_ref_cnt_val = expected_ref_cnt_val + 2;
		
		cdma_refcount_get(
				cipher_key_addr, /* uint64_t context_address */
				&ref_cnt_val); /* uint32_t *refcount_value) */
				
		fsl_os_print("Ref count value after 2 increments = %d\n", ref_cnt_val);
		if (ref_cnt_val != expected_ref_cnt_val) {
			err = 1;
			fsl_os_print("ERROR: expected Ref count value after 2 increments = %d\n",
					expected_ref_cnt_val);
		}
		
		cdma_refcount_decrement(cipher_key_addr); /* uint64_t context_address) */
		
		expected_ref_cnt_val = expected_ref_cnt_val - 1;

		cdma_refcount_get(
				cipher_key_addr, /* uint64_t context_address */
				&ref_cnt_val); /* uint32_t *refcount_value) */
		
		fsl_os_print("Ref count value after 1 decrement = %d\n", ref_cnt_val);
		if (ref_cnt_val != expected_ref_cnt_val) {
			err = 1;
			fsl_os_print("ERROR: expected Ref count value after 1 decrement = %d\n",
					expected_ref_cnt_val);
		}
		
		if (err) {
			fsl_os_print("ERROR: Ref count test failed \n");
		} else {
			fsl_os_print("Ref count test PASSED :-)\n");

		}
		/* End of CDMA Reference Count Increment/Decrement Test */ 
		
		/* ************  CDMA Mutex Test ****************** */ 
		
		fsl_os_print("\nTesting CDMA Mutex\n");
		
		//uint8_t new_l2_dst[6] = {0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
		//uint8_t new_l2_src[6] = {0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
			
		fsl_os_print("Doing CDMA cdma_mutex_lock_take (CDMA_MUTEX_WRITE_LOCK)\n");

		cdma_mutex_lock_take(
				cipher_key_addr, /* uint64_t mutex_id */
				CDMA_MUTEX_WRITE_LOCK); /* uint32_t flags */
		

		fsl_os_print("Doing CDMA cdma_mutex_lock_release()\n");
		cdma_mutex_lock_release(
				cipher_key_addr);/* uint64_t mutex_id */
		
		
		/* Lock and write external memory with CDMA */
		fsl_os_print("Doing cdma_write_with_mutex(CDMA_PREDMA_MUTEX_WRITE_LOCK)\n");
		cdma_write_with_mutex(
				cipher_key_addr, /*uint64_t ext_address */
				CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
				new_l2_dst, /* void *ws_src */
				6); /* uint16_t size */
		
		/* Read and release */
		fsl_os_print("Doing cdma_read_with_mutex(CDMA_POSTDMA_MUTEX_RM_BIT)\n");
		cdma_read_with_mutex(
				cipher_key_addr, /*uint64_t ext_address */
				CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
				cipher_key_read, /* void *ws_dst */
				6); /* uint16_t size */

		/* Compare read data */
		fsl_os_print("Comparing data from cdma_read_with_mutex()\n");
		fsl_os_print("CDMA read = 0x");
		for (i=0; i<6 ; i++) {
				fsl_os_print("%x ",cipher_key_read[i]);
				if (cipher_key_read[i] != new_l2_dst[i]) {
					fsl_os_print("\nERROR: CDMA read mismatch\n");
					err = 1;
				}	
		}
		fsl_os_print("\n");

		if (err)
		{
			fsl_os_print("ERROR: CDMA mutex test failed\n");
		} else {
			fsl_os_print("CDMA mutex test PASSED :-)\n");
		}


		/* End of CDMA Mutex Test */ 

		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	}
	
	if (!err) {
		fsl_os_print("Simple bring-up test completed successfully\n");
	}
	
	return 0;
}



