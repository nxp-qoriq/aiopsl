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
#include "dplib/fsl_ipsec.h"

#include "simple_bu_test.h"

void bu_tman_callback(uint64_t opaque1, uint16_t opaque2);
void bu_tman_callback_oneshot(uint64_t opaque1, uint16_t opaque2);
int ipsec_app_init(uint16_t ni_id);
void ipsec_print_frame(void);
void ipsec_print_stats (ipsec_handle_t desc_handle);
void ipsec_print_sp (uint16_t ni_spid);
void ipsec_encr_decr(void);

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

#define BU_TMI_BUF_ALIGN 64 /* TMI address alignment (64 bytes) */
/**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#define BU_TMI_ALIGN_64(ADDRESS, ALIGNMENT)           \
        ((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1) & \
        								(~(((uint64_t)(ALIGNMENT)) - 1)))
#define BU_TMI_ADDR_ALIGN(ADDRESS) \
	BU_TMI_ALIGN_64((ADDRESS), BU_TMI_BUF_ALIGN)
/* Aligned Descriptor Address (parameters area start) */
#define BU_TMI_ADDR(ADDRESS) BU_TMI_ADDR_ALIGN(ADDRESS)

uint32_t global_timer_handle1;
uint32_t global_timer_handle2;
uint32_t global_timer_callback_counter = 0;

/* Global IPsec vars in Shared RAM */
ipsec_instance_handle_t ipsec_instance_handle;
ipsec_handle_t ipsec_sa_desc_outbound;
ipsec_handle_t ipsec_sa_desc_inbound;
uint32_t frame_number = 0;

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
	uint32_t data_arr_32bit[8];
	uint64_t tmi_buffer_handle, tmi_timer_addr;
	uint8_t tmi_id;
	uint32_t timer_handle1;
	uint32_t timer_handle2;

	fsl_os_print("Running simple bring-up test\n");
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
	default_task_params.parser_starting_hxs = 0;
	
	default_task_params.current_scope_level = 1;
	default_task_params.scope_mode_level_arr[0] = EXCLUSIVE;
	
	/* Allocate buffers for the Keys */
	err = slab_create(
			10, /* uint32_t    num_buffs */
			10, /* uint32_t    max_buffs */
			512, /* uint16_t    buff_size */
			8, /*uint16_t    alignment */
			//64, /*uint16_t    alignment */
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
		
		/* CDMA Workspace Init Test */ 
		fsl_os_print("Testing cdma_ws_memory_init() function\n");
		cdma_ws_memory_init(
				data_arr_32bit, /* void *ws_dst */
				(8*4), /* uint16_t size */
				0x1b2b3b4b); /* uint32_t data_pattern */
		
		fsl_os_print("CDMA read = 0x");
		for (i=0; i<8 ; i++) {
				fsl_os_print("%x\n",data_arr_32bit[i]);
				if (data_arr_32bit[i] != 0x1b2b3b4b) {
					fsl_os_print("ERROR: data mismatch\n");
					err = 1;
				}	
		}
		fsl_os_print("\n");
		
		if (err)
		{
			fsl_os_print("ERROR: cdma_ws_memory_init() failed\n");
		} else {
			fsl_os_print("cdma_ws_memory_init() PASSED :-)\n");
		}
		/* End of CDMA Workspace Init Test */ 

#define RUN_TMAN_TEST
#ifdef RUN_TMAN_TEST
		/* TMAN Test */ 
		fsl_os_print("\nTesting TMAN\n");
		
		for (i=0; i<8 ; i++) {
			data_arr_32bit[i] = 0;
		}	
		
		fsl_os_print("Testing tman_get_timestamp()\n");

		tman_get_timestamp(
				(uint64_t *)(&data_arr_32bit[0])); /* uint64_t *timestamp) */
		fsl_os_print("First tman_get_timestamp() high = %d\n", data_arr_32bit[0]);
		fsl_os_print("First tman_get_timestamp() low  = %d\n", data_arr_32bit[1]);
		tman_get_timestamp(
				(uint64_t *)(&data_arr_32bit[2])); /* uint64_t *timestamp) */
		fsl_os_print("Second tman_get_timestamp() high = %d\n", data_arr_32bit[2]);
		fsl_os_print("Second tman_get_timestamp() low  = %d\n", data_arr_32bit[3]);

		if (*((uint64_t *)(&data_arr_32bit[2])) > *((uint64_t *)(&data_arr_32bit[0]))) {
			fsl_os_print("tman_get_timestamp() PASSED :-)\n");
		} else {
			fsl_os_print("ERROR: tman_get_timestamp() failed\n");
		}

		// tman_create_tmi - need change to Skyblue instead of CTSI, TKT254640
		//-       tmi_state_ptr = (unsigned int*)((unsigned int)TMAN_TMSTATE_ADDRESS
		//+       tmi_state_ptr = (unsigned int*)((unsigned int)0x020a2018
		
		/* Allocate a buffer for TMI */
		err = slab_acquire(
				slab_handle, /* struct slab *slab */
				&tmi_buffer_handle /* uint64_t *buff */
				);
		
		tmi_timer_addr = BU_TMI_ADDR(tmi_buffer_handle); /* Align to 64 bytes */

		fsl_os_print("Testing tman_create_tmi()\n");
	
		err = tman_create_tmi(
				tmi_timer_addr, /* uint64_t tmi_mem_base_addr */
				5, /* uint32_t max_num_of_timers */
				&tmi_id); /* uint8_t *tmi_id */
				
		if (err) {
			fsl_os_print("ERROR: tman_create_tmi() failed\n");
		} else {
			fsl_os_print("tman_create_tmi() PASSED :-)\n");
		}

		fsl_os_print("Testing tman_create_timer()\n");
		fsl_os_print("Calling tman_create_timer() for periodic timer\n");
		err = tman_create_timer(
				tmi_id, /* uint8_t tmi_id */
				TMAN_CREATE_TIMER_MODE_10_MSEC_GRANULARITY, /* uint32_t flags */
					/* 10 mSec timer ticks*/
				100, /*	uint16_t duration; 100*10ms = 1 sec */
				0x11, /* tman_arg_8B_t opaque_data1 */
				0x12, /* tman_arg_2B_t opaque_data2 */ 
				&bu_tman_callback, /* tman_cb_t tman_timer_cb */
				&timer_handle1); /*	uint32_t *timer_handle */
		
		global_timer_handle1 = timer_handle1;
		
		if (err) {
			fsl_os_print("ERROR: tman_create_timer() failed\n");
		} else {
			fsl_os_print("tman_create_timer() PASSED :-)\n");
		}
		
		fsl_os_print("Calling tman_create_timer() for one shot timer\n");
		err = tman_create_timer(
				tmi_id, /* uint8_t tmi_id */
				TMAN_CREATE_TIMER_MODE_10_MSEC_GRANULARITY |
					TMAN_CREATE_TIMER_ONE_SHOT, /* uint32_t flags */
					/* 10 mSec timer ticks*/
				//100, /*	uint16_t duration; 100*10ms = 1 sec */
				600, /*	uint16_t duration; 600*10ms = 6 sec */
				0x33, /* tman_arg_8B_t opaque_data1 */
				0x44, /* tman_arg_2B_t opaque_data2 */ 
				&bu_tman_callback_oneshot, /* tman_cb_t tman_timer_cb */
				&timer_handle2); /*	uint32_t *timer_handle */
		
		global_timer_handle2 = timer_handle2;
		
		if (err) {
			fsl_os_print("ERROR: tman_create_timer() failed\n");
		} else {
			fsl_os_print("tman_create_timer() PASSED :-)\n");
		}
		/* End of TMAN Test */ 
#endif

#ifdef RUN_IPSEC_TEST
		/* IPsec Test */
		err = ipsec_app_init(0); /* Call with NI ID = 0 */
		if (err) {
			fsl_os_print("ERROR: IPsec initialization failed\n");
		}
		
		
		fsl_os_print("\nCalling ipsec_encr_decr()\n");

		ipsec_encr_decr();
		fsl_os_print("ipsec_encr_decr() completed\n");
#endif
		
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		
	}
	
	if (!err) {
		fsl_os_print("Simple bring-up test completed successfully\n");
	}
	
	return err;
}

void bu_tman_callback(uint64_t opaque1, uint16_t opaque2)
{
	
	uint32_t opaque1_32bit = (uint32_t)opaque1;
	uint32_t opaque2_32bit = (uint32_t)opaque2;
	int err;
	
	global_timer_callback_counter ++;
	
	fsl_os_print("\nIn bu_tman_callback (counter = %d)\n", global_timer_callback_counter);
	fsl_os_print("opaque1 = %x,  opaque2 = %x\n", opaque1_32bit, opaque2_32bit);

	if (global_timer_callback_counter == 4) {
		fsl_os_print("Doing tman_delete_timer() in bu_tman_callback\n");
		err = tman_delete_timer(
				global_timer_handle1, /* uint32_t timer_handle */
				TMAN_TIMER_DELETE_MODE_WAIT_EXP); /*uint32_t flags */
		if (!err) {
			fsl_os_print("tman_delete_timer() SUCCESS\n");
		} else {
			fsl_os_print("ERROR: tman_delete_timer() returned with %d\n", err);
		}
	}
	
	fsl_os_print("Doing tman_timer_completion_confirmation() in bu_tman_callback\n");
	tman_timer_completion_confirmation(global_timer_handle1);

	fsl_os_print("bu_tman_callback() completed\n");
}

void bu_tman_callback_oneshot(uint64_t opaque1, uint16_t opaque2)
{
	
	uint32_t opaque1_32bit = (uint32_t)opaque1;
	uint32_t opaque2_32bit = (uint32_t)opaque2;
	
	fsl_os_print("\nIn bu_tman_callback_oneshot \n");
	fsl_os_print("opaque1 = %x,  opaque2 = %x\n", opaque1_32bit, opaque2_32bit);

	fsl_os_print("Doing tman_timer_completion_confirmation() in bu_tman_callback_oneshot\n");
	tman_timer_completion_confirmation(global_timer_handle2);

	//fsl_os_print("\nbu_tman_callback_oneshot(): Calling ipsec_encr_decr()\n");
	//ipsec_encr_decr();
	//fsl_os_print("bu_tman_callback_oneshot(): ipsec_encr_decr() completed\n");
	
	fsl_os_print("bu_tman_callback_oneshot() completed\n");
}

/* IPsec Initialization */
int ipsec_app_init(uint16_t ni_id)
{
	int err  = 0;
	struct ipsec_descriptor_params params;
	uint32_t outer_ip_header[10];
	struct slab *slab_handle = NULL;
	uint32_t handle_high, handle_low;
	int i;

	uint64_t cipher_key_addr;
	uint64_t auth_key_addr;
	//uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	uint8_t cipher_key[16] = "1122334455667788";
	//uint8_t auth_key[128];
	uint8_t auth_key[128] = "12345678123456781234";
	uint8_t auth_key_id = 0;
	
	uint32_t cipher_alg;
	uint32_t cipher_keylen;
	uint32_t auth_alg;
	uint32_t auth_keylen;
	uint32_t algs;
	uint32_t outer_header_ip_version;
	uint16_t ni_spid;
	uint32_t tunnel_transport_mode;

	enum key_types {
		NULL_ENCRYPTION = 0,
		AES128_SHA1,
		AES128_SHA256,
		AES128_NULL
	};

	/**********************************************************/
	/*                    Control Parameters                  */
	/**********************************************************/
	/* Set the required algorithms here */
	algs = NULL_ENCRYPTION;
	//algs = AES128_SHA256;
	//algs = AES128_SHA1;

	/* Set the outer IP header type here */
	outer_header_ip_version = 4; /* 4 or 6 */
	
	auth_key_id = 0; /* Keep the initial key array value */ 
	//auth_key_id = 1; /* Overwrite the initial key array value */ 
	
	//tunnel_transport_mode = IPSEC_FLG_TUNNEL_MODE; /* Tunnel Mode */
	tunnel_transport_mode = 0; /* Transport Mode */
	
	/**********************************************************/

	ipsec_instance_handle_t ws_instance_handle = 0;
	ipsec_handle_t ws_desc_handle_outbound = 0;
	ipsec_handle_t ws_desc_handle_inbound = 0;

	if (auth_key_id == 1) {
		for (i=0; i<128; i++) {
			auth_key[i] = (uint8_t)i;
		}
	}

	frame_number = 0;

	fsl_os_print("\n++++\n  IPsec Demo: Doing IPsec Initialization\n+++\n");

	dpni_drv_get_spid(
		ni_id, /* uint16_t ni_id */
		&ni_spid /* uint16_t *spid */
		);
	
	fsl_os_print("IPsec Demo: Overriding NI SPID to 0\n");
	ni_spid = 0;
	
	fsl_os_print("IPsec Demo: SPID = %d\n", ni_spid);

#define IPSEC_DEBUG_PRINT_SP
#ifdef IPSEC_DEBUG_PRINT_SP
	ipsec_print_sp (ni_spid);
#endif

	err = ipsec_create_instance(
			10, /* committed sa num */
			20, /* max sa num */
			0, /* instance flags */
			0, /* tmi id */
			&ws_instance_handle);
	if (err) {
		fsl_os_print("ERROR: ipsec_create_instance() failed\n");
		fsl_os_print("ipsec_create_instance return status = %d (0x%x)\n",
				err, err);
	} else {
		fsl_os_print("ipsec_create_instance() completed successfully\n");
	}

	ipsec_instance_handle = ws_instance_handle;

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
	err = 0;
	err = slab_acquire(
			slab_handle, /* struct slab *slab */
			&cipher_key_addr /* uint64_t *buff */
			);

	if (err)
		fsl_os_print("ERROR: slab_acquire() failed\n");
	else
		fsl_os_print("slab_acquire() completed successfully\n");

	/* Acquire the Authentication key buffer */
	err = 0;
	err = slab_acquire(
			slab_handle, /* struct slab *slab */
			&auth_key_addr /* uint64_t *buff */
			);

	if (err)
		fsl_os_print("ERROR: slab_acquire() failed\n");
	else
		fsl_os_print("slab_acquire() completed successfully\n");



	switch (algs) {
		case NULL_ENCRYPTION:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_NULL\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_MD5_96\n");
			cipher_alg = IPSEC_CIPHER_NULL;
			cipher_keylen = 0x0;
			auth_alg = IPSEC_AUTH_HMAC_MD5_96;
			auth_keylen = 16;
			break;
		case AES128_SHA1:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA1_96\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA1_96;
			auth_keylen = 20;
			break;
		case AES128_SHA256:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA2_256_128\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA2_256_128;
			auth_keylen = 64;
			break;
		case AES128_NULL:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_NULL\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_NULL;
			auth_keylen = 0;
			break;
		default:
			fsl_os_print("Cipher Algorithm (default): IPSEC_CIPHER_NULL\n");
			fsl_os_print("Authentication Algorithm (default): IPSEC_AUTH_HMAC_MD5_96\n");
			cipher_alg = IPSEC_CIPHER_NULL;
			cipher_keylen = 0x0;
			auth_alg = IPSEC_AUTH_HMAC_MD5_96;
			auth_keylen = 16;
	}

	/* Copy the Keys to external memory with CDMA */
	cdma_write(
			cipher_key_addr, /* ext_address */
			&cipher_key, /* ws_src */
			16); /* uint16_t size */

	cdma_write(
			auth_key_addr, /* ext_address */
			&auth_key, /* ws_src */
			(uint16_t)auth_keylen); /* uint16_t size */

	/* Outer IP header */
	if (outer_header_ip_version == 4) {
		outer_ip_header[0] = 0x45db0014;
		outer_ip_header[1] = 0x12340000;
		outer_ip_header[2] = 0xff32386f;
		outer_ip_header[3] = 0x45a4e14c;
		outer_ip_header[4] = 0xed035c45;

		params.encparams.ip_hdr_len = 0x14; /* outer header length is 20 bytes */
	}
	else if (outer_header_ip_version == 6) {

		outer_ip_header[0] = 0x60000000;
		outer_ip_header[1] = 0x002032ff;
		outer_ip_header[2] = 0xfe800000;
		outer_ip_header[3] = 0x00000000;
		outer_ip_header[4] = 0x021125ff;
		outer_ip_header[5] = 0xfe8295b5;
		outer_ip_header[6] = 0xff020000;
		outer_ip_header[7] = 0x00000000;
		outer_ip_header[8] = 0x00000001;
		outer_ip_header[9] = 0xff8295b5;

		params.encparams.ip_hdr_len = 0x28; /* outer header length is 40 bytes */
	}

	/* Outbound (encryption) parameters */
	params.direction = IPSEC_DIRECTION_OUTBOUND; /**< Descriptor direction */
	//params.flags = IPSEC_FLG_TUNNEL_MODE |
	params.flags = tunnel_transport_mode |
			IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
			/**< Miscellaneous control flags */
	
	
	//params.encparams.ip_nh = 0x0;
	params.encparams.options = 0x0;
	params.encparams.seq_num_ext_hi = 0x0;
	params.encparams.seq_num = 0x0;
	params.encparams.spi = 0x0;
	params.encparams.outer_hdr = (uint32_t *)&outer_ip_header;

	for (i=0; i<sizeof(params.encparams.cbc.iv); i++) {
		params.encparams.cbc.iv[i] = 0;
	}

	params.cipherdata.algtype = cipher_alg;
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = cipher_keylen;
	params.cipherdata.key_enc_flags = 0x0;

	params.authdata.algtype = auth_alg;
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = auth_keylen;
	params.authdata.key_enc_flags = 0x0;

	params.soft_kilobytes_limit = 0xffffffffffffffff;
	params.hard_kilobytes_limit = 0xffffffffffffffff;
	params.soft_packet_limit = 0xffffffffffffffff;
	params.hard_packet_limit = 0xffffffffffffffff;
	params.soft_seconds_limit = 0x0;
	params.hard_seconds_limit = 0x0;

	params.lifetime_callback = NULL;
	params.callback_arg = NULL;

	params.spid = ni_spid;

	/* Create Outbound (encryption) Descriptor */
	err = ipsec_add_sa_descriptor(
			&params,
			ws_instance_handle,
			&ws_desc_handle_outbound);

	handle_high =
			(uint32_t)((ws_desc_handle_outbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_outbound & 0x00000000ffffffff);

	if (err) {
		fsl_os_print("ERROR: ipsec_add_sa_descriptor(encryption) failed\n");
		fsl_os_print("ipsec_add_sa_descriptor return status = %d (0x%x)\n",
				err, err);
	} else {
		fsl_os_print("ipsec_add_sa_descriptor(encryption) succeeded\n");
		fsl_os_print("Encryption handle = 0x%x_%x\n", handle_high, handle_low);
	}

	ipsec_sa_desc_outbound = ws_desc_handle_outbound;
	
	/* Inbound (decryption) parameters */
	params.direction = IPSEC_DIRECTION_INBOUND; /**< Descriptor direction */
	//params.flags = IPSEC_FLG_TUNNEL_MODE |
	params.flags = tunnel_transport_mode |
			IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
			/**< Miscellaneous control flags */

	params.decparams.options = 0x0;
	//params.decparams.options = IPSEC_DEC_OPTS_ARS32; /* Anti Replay 32 bit enabled */

	params.decparams.seq_num_ext_hi = 0x0;
	params.decparams.seq_num = 0x0;

	params.cipherdata.algtype = cipher_alg;
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = cipher_keylen;
	params.cipherdata.key_enc_flags = 0x0;

	params.authdata.algtype = auth_alg;
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = auth_keylen;
	params.authdata.key_enc_flags = 0x0;

	params.soft_kilobytes_limit = 0xffffffffffffffff;
	params.hard_kilobytes_limit = 0xffffffffffffffff;
	params.soft_packet_limit = 0xffffffffffffffff;
	params.hard_packet_limit = 0xffffffffffffffff;
	params.soft_seconds_limit = 0x0;
	params.hard_seconds_limit = 0x0;

	params.lifetime_callback = NULL;
	params.callback_arg = NULL;

	params.spid = ni_spid;

	/* Create Inbound (decryption) Descriptor */
	err = ipsec_add_sa_descriptor(
			&params,
			ws_instance_handle,
			&ws_desc_handle_inbound);

	handle_high = (uint32_t)((ws_desc_handle_inbound & 0xffffffff00000000)>>32);
	handle_low = (uint32_t)(ws_desc_handle_inbound & 0x00000000ffffffff);

	if (err) {
		fsl_os_print("ERROR: ipsec_add_sa_descriptor(decryption) failed\n");
		fsl_os_print("ipsec_add_sa_descriptor return status = %d (0x%x)\n",
				err, err);
	} else {
		fsl_os_print("ipsec_add_sa_descriptor(decryption) succeeded\n");
		fsl_os_print("Decryption handle = 0x%x_%x\n", handle_high, handle_low);
	}

	ipsec_sa_desc_inbound = ws_desc_handle_inbound;

	if (!err)
		fsl_os_print("IPsec Demo: IPsec Initialization completed\n");

	return err;
} /* End of ipsec_app_init */

/* Print the frame in a Wireshark-like format */
void ipsec_print_frame(void) {
	uint8_t *eth_pointer_byte = 0;
	int i;
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

	fsl_os_print("Printing Frame. FD[len] = %d, Seg Len = %d\n",
			frame_len, seg_len);

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();

	for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
	{
		if ((i%16) == 0) {
			fsl_os_print("00");
			if (i<16)
				fsl_os_print("0");
			fsl_os_print("%x  ",(i));
		}

		if ((*eth_pointer_byte) < 16)
			fsl_os_print("0");

		fsl_os_print("%x ", *eth_pointer_byte);

		if ((i%8) == 7)
			fsl_os_print(" ");

		if ((i%16) == 15)
			fsl_os_print("\n");

		eth_pointer_byte++;
	}

	if ((i%16) != 0)
		fsl_os_print("\n");
} /* End of ipsec_print_frame */

void ipsec_print_stats (ipsec_handle_t desc_handle) {
	int err = 0;
	uint64_t kilobytes;
	uint64_t packets;
	uint32_t sec;
	uint32_t sequence_number;
	uint32_t extended_sequence_number;
	uint32_t anti_replay_bitmap[4];
	uint32_t val_high, val_low;

	/* Read statistics */
	err = ipsec_get_lifetime_stats(
		desc_handle,
		&kilobytes,
		&packets,
		&sec);
	fsl_os_print("IPsec Demo: ipsec_get_lifetime_stats():\n");

	val_high =
		(uint32_t)((kilobytes & 0xffffffff00000000)>>32);
	val_low =
		(uint32_t)(kilobytes & 0x00000000ffffffff);
	fsl_os_print("kilobytes = 0x%x_0x%x,", val_high, val_low);

	val_high =
		(uint32_t)((packets & 0xffffffff00000000)>>32);
	val_low =
		(uint32_t)(packets & 0x00000000ffffffff);
	fsl_os_print("packets = 0x%x_0x%x, seconds = %d\n",
		val_high, val_low, sec);

	err = ipsec_get_seq_num(
		desc_handle,
		&sequence_number,
		&extended_sequence_number,
		anti_replay_bitmap);
	fsl_os_print("IPsec Demo: ipsec_get_seq_num():\n");
	fsl_os_print("sequence_number = 0x%x, esn = 0x%x\n",
			sequence_number, extended_sequence_number);
	fsl_os_print("bitmap[0:3] = 0x%x, 0x%x, 0x%x, 0x%x\n",
		anti_replay_bitmap[0], anti_replay_bitmap[1],
		anti_replay_bitmap[2], anti_replay_bitmap[3]);
} /* End of ipsec_print_stats */

void ipsec_print_sp (uint16_t ni_spid) {
	extern struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];
	struct storage_profile *sp_addr = &storage_profile[0];

	/* Debug - Print the storage profile */
	sp_addr += ni_spid; /* TMP for printing the SP */

	fsl_os_print("*** Debug: storage_profile (0): 0x%x\n", *((uint32_t *)sp_addr + 0));
	fsl_os_print("*** Debug: storage_profile (1): 0x%x\n", *((uint32_t *)sp_addr + 1));
	fsl_os_print("*** Debug: storage_profile (2): 0x%x\n", *((uint32_t *)sp_addr + 2));
	fsl_os_print("*** Debug: storage_profile (3): 0x%x\n", *((uint32_t *)sp_addr + 3));
	fsl_os_print("*** Debug: storage_profile (4): 0x%x\n", *((uint32_t *)sp_addr + 4));
	fsl_os_print("*** Debug: storage_profile (5): 0x%x\n", *((uint32_t *)sp_addr + 5));
	fsl_os_print("*** Debug: storage_profile (6): 0x%x\n", *((uint32_t *)sp_addr + 6));
	fsl_os_print("*** Debug: storage_profile (7): 0x%x\n", *((uint32_t *)sp_addr + 7));
} /* End of ipsec_print_sp */

void ipsec_encr_decr(void)
{
	int      err = 0;
	uint32_t enc_status = 0;
	uint32_t dec_status = 0;
	int i = 0;
	uint8_t frame_before_encr[256] = {0};
	uint8_t *eth_pointer_byte = 0;
	uint32_t handle_high, handle_low;
	int local_test_error = 0;
	uint32_t original_frame_len;

	//sl_prolog();

	fsl_os_print("In ipsec_encr_decr()\n");

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	original_frame_len = frame_len;
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	uint16_t original_seg_addr = PRC_GET_SEGMENT_ADDRESS();

	ipsec_handle_t ws_desc_handle_outbound = ipsec_sa_desc_outbound;
	ipsec_handle_t ws_desc_handle_inbound = ipsec_sa_desc_inbound;

	frame_number ++;

	fsl_os_print("IPsec Demo: Core %d Received Frame number %d\n",
			core_get_id(), frame_number);

	handle_high =
			(uint32_t)((ws_desc_handle_outbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_outbound & 0x00000000ffffffff);
	fsl_os_print("Encryption handle = 0x%x_%x\n", handle_high, handle_low);

	handle_high =
			(uint32_t)((ws_desc_handle_inbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_inbound & 0x00000000ffffffff);
	fsl_os_print("Decryption handle = 0x%x_%x\n", handle_high, handle_low);

	/* preserve original frame */
	for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
	{
		frame_before_encr[i] =  *eth_pointer_byte;
		eth_pointer_byte++;
	}

	fsl_os_print("IPSEC: frame header before encryption\n");
	/* Print header */
	ipsec_print_frame();

	fsl_os_print("\n");

	fsl_os_print("IPSEC: Starting Encryption\n");
	err = ipsec_frame_encrypt(
			ws_desc_handle_outbound,
			&enc_status
			);

	if (err)
	{
		fsl_os_print("ERROR: ipsec_frame_encrypt() failed\n");
		local_test_error |= err;
	}
	else
		fsl_os_print("ipsec_frame_encrypt() completed successfully\n");

	if (enc_status)
	{
		fsl_os_print("ERROR: SEC Encryption Failed (enc_status = 0x%x)\n",
				enc_status);
		local_test_error |= enc_status;
	}

	fsl_os_print("IPSEC: frame header after encryption\n");
	/* Print header */
	ipsec_print_frame();
	fsl_os_print("\n");

	fsl_os_print("IPSEC: Starting Decryption\n");
	err = ipsec_frame_decrypt(
			ws_desc_handle_inbound,
			&dec_status
			);

	if (err)
	{
		fsl_os_print("ERROR: ipsec_frame_decrypt() failed\n");
		local_test_error |= err;
	}
	else
		fsl_os_print("ipsec_frame_decrypt() completed successfully\n");

	if (dec_status)
	{
		fsl_os_print("ERROR: SEC Decryption Failed (dec_status = 0x%x)\n",
				dec_status);
		local_test_error |=dec_status;
	}

    /* Due to parser not aligned segment WA need to represent again*/
    fdma_close_default_segment();
    
    err = fdma_present_default_frame_segment(
    		FDMA_PRES_NO_FLAGS, /* uint32_t flags */
    		(void *)original_seg_addr, /* void *ws_dst */
    		0, /* uint16_t offset */
    		seg_len); /* uint16_t present_size */
    
   	fsl_os_print("STATUS: fdma_present_default_frame_segment returned %d\n", err);
 	
	fsl_os_print("IPSEC: frame header after decryption\n");
	/* Print header */
	ipsec_print_frame();
	fsl_os_print("\n");

	/* Compare decrypted frame to original frame */
	err = 0;
	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

	if (frame_len != original_frame_len) {
		fsl_os_print("ERROR: incorrect frame length (FD[length] = %d)\n",
				frame_len);
		err = 1;
		local_test_error |= err;
	} else {
		for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
		{
			if (frame_before_encr[i] !=  *eth_pointer_byte) {
				fsl_os_print(
						"ERROR: frame after decryption differ from origin\n");
				err = 1;
				local_test_error |= err;
				break;
			}
			eth_pointer_byte++;
		}
	}

	if(!local_test_error) /* No error found during injection of packets*/
	{
		fsl_os_print("Finished SUCCESSFULLY\n");
		fsl_os_print("\nFrame after decryption the same as origin\n\n");
	}
	else
		fsl_os_print("Finished with ERRORS\n");

	/* Read statistics */
	fsl_os_print("IPsec Demo: Encryption Statistics:\n");
	ipsec_print_stats(ws_desc_handle_outbound);

	fsl_os_print("IPsec Demo: Decryption Statistics:\n");
	ipsec_print_stats(ws_desc_handle_inbound);

	fsl_os_print("IPsec Demo: Core %d Sending Frame number %d\n",
			core_get_id(), frame_number);

	//fdma_terminate_task();
}


