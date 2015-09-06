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
#include "fsl_ip.h"
#include "fsl_l4.h"
#include "fsl_malloc.h"
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
#define PRESENTATION_SIZE 128
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

/* Action enums for update pop/push vlan functions */
enum parser_action {
	POP_VLAN_UPDATE = 0,
	PUSH_VLAN_UPDATE = 1
};

enum parser_vlans_in_frame {
	NO_VLAN = 0,
	ONE_VLAN = 1,
	TWO_VLANS = 2,
	THREE_VLANS = 3,
	NUM_OF_VLANS_IN_FRAME
};

/* Wrapper for an inline function */
void fdma_delete_default_segment_data_wrapper(uint16_t to_offset, uint16_t delete_target_size, uint32_t flags);

void fdma_delete_default_segment_data_wrapper(uint16_t to_offset,
					uint16_t delete_target_size,
					uint32_t flags)
{
	fdma_delete_default_segment_data(to_offset, delete_target_size,	flags);
}

void fdma_insert_default_segment_data_wrapper(uint16_t to_offset, void *from_ws_src, uint16_t insert_size,uint32_t flags);

void fdma_insert_default_segment_data_wrapper(uint16_t to_offset,
					void *from_ws_src,
					uint16_t insert_size,
					uint32_t flags)
{	
	fdma_insert_default_segment_data(to_offset, from_ws_src, insert_size, flags);
}

void print_parse_result_table();
void print_parse_result_table() 
{
	int i;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	
	/* parse results */		
		fsl_print("Ethernet offset: %d %x\n", PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
		fsl_print("IPV4 offset: %d %x\n", PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
		fsl_print("TCP offset: %d %x\n", PARSER_IS_TCP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());

		for (i = 0; i < 16 ; i++)
			fsl_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));	
}

int simple_bu_parser_test(uint8_t generator_id,
						uint8_t event_id,
						uint64_t app_ctx,
						void *event_data)
{
	UNUSED(generator_id);
	UNUSED(event_id);
		
	int err = 0, i, fail_status;
	uint8_t prpid;
	uint16_t spid = 0, ni;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct presentation_context *prc = (struct presentation_context *) HWC_PRC_ADDRESS;
	uint16_t to_offset = (uint16_t)pr->eth_offset + 2*NET_HDR_FLD_ETH_ADDR_SIZE;

	/* enable ni */
	ni = (uint16_t)((uint32_t)event_data);

	pr_info("Event received for AIOP NI ID %d\n",ni);

	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("dpni_drv_register_rx_cb for ni %d failed: %d\n", ni, err);
		return err;
	}

	err = dpni_drv_set_max_frame_length(ni, 0x2000 /* Max frame length */);
	if (err) {
		pr_err("dpni_drv_set_max_frame_length for ni %d failed: %d\n", ni, err);
		return err;
	}

	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("dpni_drv_enable for ni %d failed: %d\n", ni, err);
		return err;
	}
	
	/* parser test */
	/* choose the wanted action (pop vlan/push vlan) */
	enum parser_action parser_action = PUSH_VLAN_UPDATE;
	
	fsl_print("Running simple bring-up test\n");
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
	default_task_params.parser_starting_hxs = 0; /* starting from ethernet */
	default_task_params.current_scope_level = 1;
	default_task_params.scope_mode_level_arr[0] = EXCLUSIVE;

	struct ldpaa_fd *fd = (struct ldpaa_fd *)HWC_FD_ADDRESS;
	
	/* several frames with different amount of VLAN headers */	
	uint8_t frame_data[NUM_OF_VLANS_IN_FRAME][FRAME_SIZE] = {
		{/* 0 VLANS frame */
		0x00,0x00,0x01,0x00,0x00,0x01,0x02,0x00,
		0xc0,0xa8,0x0a,0x01,0x08,0x00,0x45,0x00,
		0x00,0x2e,0x00,0x02,0x00,0x00,0xff,0x06,
		0xd6,0x71,0xc0,0xa8,0x0a,0x01,0xc0,0xa8,
		0x5a,0x04,0xda,0x8e,0x00,0x50,0x00,0x00,
		0x10,0x00,0x00,0x00,0x00,0x00,0x50,0x02,
		0x20,0x00,0xbc,0x3e,0x00,0x00,0x12,0x34,
		0x56,0x78,0x9a,0xbc,0xde,0xff
		},
		{/* 1 VLANS frame */
		0x00,0x00,0x01,0x00,0x00,0x01,0x02,0x00,
		0xc0,0xa8,0x0a,0x01,0x81,0x00,0xa1,0xb2,
		0x08,0x00,0x45,0x00,0x00,0x2e,0x00,0x02,
		0x00,0x00,0xff,0x06,0xd6,0x71,0xc0,0xa8,
		0x0a,0x01,0xc0,0xa8,0x5a,0x04,0xda,0x8e,
		0x00,0x50,0x00,0x00,0x10,0x00,0x00,0x00,
		0x00,0x00,0x50,0x02,0x20,0x00,0xbc,0x3e,
		0x00,0x00,0x12,0x34,0x56,0x78,0x9a,0xbc,
		0xde,0xff
		}, 
		{/* 2 VLANS frame */
		0x00,0x00,0x01,0x00,0x00,0x01,0x02,0x00,
		0xc0,0xa8,0x0a,0x01,0x81,0x00,0xa1,0xb2,
		0x81,0x00,0xcd,0x8d,0x08,0x00,0x45,0x00,
		0x00,0x2e,0x00,0x02,0x00,0x00,0xff,0x06,
		0xd6,0x71,0xc0,0xa8,0x0a,0x01,0xc0,0xa8,
		0x5a,0x04,0xda,0x8e,0x00,0x50,0x00,0x00,
		0x10,0x00,0x00,0x00,0x00,0x00,0x50,0x02,
		0x20,0x00,0xbc,0x3e,0x00,0x00,0x12,0x34,
		0x56,0x78,0x9a,0xbc,0xde,0xff	
		},
		{/* 3 VLANS frame */
		0x00,0x00,0x01,0x00,0x00,0x01,0x02,0x00,
		0xc0,0xa8,0x0a,0x01,0x81,0x00,0xa1,0xb2,
		0x81,0x00,0xa1,0xb2,0x81,0x00,0xcd,0x8d,
		0x08,0x00,0x45,0x00,0x00,0x2e,0x00,0x02,
		0x00,0x00,0xff,0x06,0xd6,0x71,0xc0,0xa8,
		0x0a,0x01,0xc0,0xa8,0x5a,0x04,0xda,0x8e,
		0x00,0x50,0x00,0x00,0x10,0x00,0x00,0x00,
		0x00,0x00,0x50,0x02,0x20,0x00,0xbc,0x3e,
		0x00,0x00,0x12,0x34,0x56,0x78,0x9a,0xbc,
		0xde,0xff			
		}
	};	

	uint8_t frame_handle, tmp;
	uint16_t icid, flags = 0;
	struct fdma_amq amq;
	
	if (dpni_drv_get_spid(ni, &spid))
			pr_err("get spid failed\n");		
	else
		fsl_print("The spid for ni %d is %d\n", (int)ni, (int) spid);
	
	/* setting SPID */
	*((uint8_t *)HWC_SPID_ADDRESS) = (uint8_t)spid;
	icid = (uint16_t)(storage_profile[spid].ip_secific_sp_info >> 48);
	icid = ((icid << 8) & 0xff00) | ((icid >> 8) & 0xff);
	tmp = (uint8_t)(storage_profile[spid].ip_secific_sp_info >> 40);
	
	if (tmp & 0x08)
		flags |= FDMA_ICID_CONTEXT_BDI;
	if (tmp & 0x04)
		flags |= FDMA_ICID_CONTEXT_PL;
	if (storage_profile[spid].mode_bits2 & sp1_mode_bits2_VA_MASK)
		flags |= FDMA_ICID_CONTEXT_VA;
	
	amq.icid = icid;
	amq.flags = flags;
	set_default_amq_attributes(&amq);
	*(uint32_t *)(&storage_profile[spid].pbs2) = *(uint32_t *)(&storage_profile[spid].pbs1);

	for (i = 0; i < 8 ; i++)
		fsl_print("storage profile arg %d: 0x%x\n", i, *((uint32_t *)(&(storage_profile[spid]))+i));
	
	/* create frame and parse it */
	err = create_frame(fd, &frame_data[TWO_VLANS], FRAME_SIZE, &frame_handle);
	if (err)
		fsl_print("ERROR: create frame failed with err = %x!\n", err);
	
	/* print parse results */
	fsl_print("Parse result before action: \n");	
	print_parse_result_table();
	
	/* close the presented segment */
	fdma_close_default_segment();

	/* present default frame segment again (enables to change PRESENTATION_SIZE) */
	err = fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS, (void *)0x180, 0, PRESENTATION_SIZE);
	if (err)
		fsl_print("STATUS: fdma present default segment returned status is %d\n", err);

	uint16_t vlan_offset = (uint16_t)(PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT()) - PARSER_TCI_DIST_FROM_START_OF_VLAN;
	
	/* POP VLAN */
	if (parser_action == POP_VLAN_UPDATE) {
		fsl_print("Starting pop action\n");
				
		/* save the removed VLAN header */
		uint32_t removed_vlan = *(uint32_t *)(vlan_offset + prc->seg_address);
	
		/* Remove VLAN header */
		fdma_delete_default_segment_data_wrapper(vlan_offset, 
				sizeof(struct vlanhdr), 
				FDMA_REPLACE_SA_REPRESENT_BIT);
		
		/* parse segment using Parser */
		err = parse_result_generate_basic();
		if (err)
			fsl_print("STATUS: parser returned status is %d\n", err);
		
		/* save parse result table after parser */
		uint32_t parse_result_table_copy[16] = {0};
		for (i = 0; i < 16 ; i++)
			parse_result_table_copy[i] = *((uint32_t *)(0x80)+i);
		
		/* print parse results */
		fsl_print("Parse result after parser: \n");
		print_parse_result_table();
		
		/* Add the removed VLAN header back to the frame */
		fdma_insert_default_segment_data_wrapper(to_offset, 
				&removed_vlan,
				sizeof(struct vlanhdr),
				FDMA_REPLACE_SA_REPRESENT_BIT);
		
		/* parse segment using Parser for reversing parse result table */
		err = parse_result_generate_basic();
		if (err)
			fsl_print("STATUS: parser returned status is %d\n", err);

		/* print parse results */
		fsl_print("Parse result after reversing action: \n");
		print_parse_result_table();
		
		/* Remove VLAN header */
		fdma_delete_default_segment_data_wrapper(vlan_offset, 
				sizeof(struct vlanhdr), 
				FDMA_REPLACE_SA_REPRESENT_BIT);
		
		/* parse segment using SW update function */
		parser_pop_vlan_update();
		
		/* print parse results */
		fsl_print("Parse result after parser_pop_vlan_update(): \n");
		print_parse_result_table();
		
		/* compare parse result table results between parser and SW function */
		fail_status = 0;
		for (i = 0; i < 16 ; i++)
			if (i != 9) /* arg9 is running_sum, therefore those parameters will be different anyway */
				if (parse_result_table_copy[i] != *((uint32_t *)(0x80)+i)) {
					fsl_print("FAILURE!\n difference in i = %d\n", i);
					fail_status = 1;
					break;
				}
		if (!fail_status)
			fsl_print("SUCCESS! Simple bring-up test completed successfully\n");
	}
	
	/* PUSH VLAN */
	else if (parser_action == PUSH_VLAN_UPDATE) {
		fsl_print("Starting push action\n");
		//uint32_t vlan =0x8100a1b2;
		uint32_t vlan =0x88a8a1b2;	/* TPID=0x88a8 */
		//uint32_t vlan =0x81000000; /* VID=0 + CFI=1 */
		//uint32_t vlan =0x81001222; /* CFI=1 */
		
		/* Add a VLAN header to the frame */
		fdma_insert_default_segment_data_wrapper(to_offset, 
				&vlan,
				sizeof(struct vlanhdr),
				FDMA_REPLACE_SA_REPRESENT_BIT);
		
		/* parse segment using Parser */
		err = parse_result_generate_basic();
		if (err)
			fsl_print("STATUS: parser returned status is %d\n", err);
		
		/* print parse results */
		fsl_print("Parse result after parser: \n");
		print_parse_result_table();
		
		/* save parse result table after parser */
		uint32_t parse_result_table_copy[16] = {0};
		for (i = 0; i < 16 ; i++)
			parse_result_table_copy[i] = *((uint32_t *)(0x80)+i);
		
		/* Remove VLAN header */
		fdma_delete_default_segment_data_wrapper(to_offset, 
				sizeof(struct vlanhdr), 
				FDMA_REPLACE_SA_REPRESENT_BIT);
		
		/* parse segment using Parser for reversing parse result table */
		err = parse_result_generate_basic();
		if (err)
			fsl_print("STATUS: parser returned status is %d\n", err);
		
		/* print parse results */
		fsl_print("Parse result after reversing action: \n");
		print_parse_result_table();
		
		fdma_insert_default_segment_data_wrapper(to_offset, 
				&vlan,
				sizeof(struct vlanhdr),
				FDMA_REPLACE_SA_REPRESENT_BIT);
		
		/* parse segment using SW update function */
		parser_push_vlan_update();
		
		/* print parse results */
		fsl_print("Parse result after parser_push_vlan_update(): \n");
		print_parse_result_table();
		
		/* compare parse result table results between parser and SW function */
		fail_status = 0;
		for (i = 0; i < 16 ; i++)
			if (i != 9) /* arg9 is running_sum, therefore those parameters will be different anyway */
				if (parse_result_table_copy[i] != *((uint32_t *)(0x80)+i)) {
					fsl_print("FAILURE!\n difference in i = %d\n", i);
					fail_status = 1;
					break;
				}
		if (!fail_status)
			fsl_print("SUCCESS! Simple bring-up test completed successfully\n");
	}
	
	/* remove presented frame */
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		
	return err;
}
