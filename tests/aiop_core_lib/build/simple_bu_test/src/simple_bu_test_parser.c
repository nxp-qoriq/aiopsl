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

#define FRAME_SIZE	70
#define	COPY_SIZE	16
#define AIOP_SP_BDI     0x00080000
#define SP_BP_PBS_MASK  0x3FFF
#define PRESENTATION_SIZE 128

#define BU_TMI_BUF_ALIGN 64 /* TMI address alignment (64 bytes) */
/**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#define BU_TMI_ALIGN_64(ADDRESS, ALIGNMENT)           \
        ((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1) & \
        								(~(((uint64_t)(ALIGNMENT)) - 1)))
#define BU_TMI_ADDR_ALIGN(ADDRESS) \
	BU_TMI_ALIGN_64((ADDRESS), BU_TMI_BUF_ALIGN)
/* Aligned Descriptor Address (parameters area start) */
#define BU_TMI_ADDR(ADDRESS) BU_TMI_ADDR_ALIGN(ADDRESS)

#define POP_VLAN 1
//#define PUSH_VLAN 1

void fdma_delete_default_segment_data_wrap(uint16_t to_offset,
uint16_t delete_target_size,
uint32_t flags);

void fdma_delete_default_segment_data_wrap(uint16_t to_offset,
uint16_t delete_target_size,
uint32_t flags)
{
	fdma_delete_default_segment_data(to_offset,
			delete_target_size,
			flags);
}

int fdma_insert_default_segment_data_wrap(uint16_t to_offset,
		void	 *from_ws_src,
		uint16_t insert_size,
		uint32_t flags);

int fdma_insert_default_segment_data_wrap(uint16_t to_offset,
		void	 *from_ws_src,
		uint16_t insert_size,
		uint32_t flags)
{
	 return (fdma_insert_default_segment_data(to_offset,
		from_ws_src,
		insert_size,
		flags));
}


int simple_bu_parser_test(void)
{
	int        err  = 0;
	uint8_t prpid;
	int i;

	struct   parse_result *pr =
				(struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct   presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	
	fsl_os_print("Running simple bring-up test\n");
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
	default_task_params.parser_starting_hxs = 0;
	
	default_task_params.current_scope_level = 1;
	default_task_params.scope_mode_level_arr[0] = EXCLUSIVE;

	// run create_frame on default frame
	{
		struct ldpaa_fd *fd = (struct ldpaa_fd *)HWC_FD_ADDRESS;

		
		uint8_t frame_data[FRAME_SIZE] = {0x00,0x00,0x01,0x00,0x00,0x01,0x02,0x00,
				0xc0,0xa8,0x0a,0x01,0x81,0x00,0xa1,0xb2,
				0x81,0x00,0xcd,0x8d,0x08,0x00,0x45,0x00,
				0x00,0x2e,0x00,0x02,0x00,0x00,0xff,0x06,
				0xd6,0x71,0xc0,0xa8,0x0a,0x01,0xc0,0xa8,
				0x5a,0x04,0xda,0x8e,0x00,0x50,0x00,0x00,
				0x10,0x00,0x00,0x00,0x00,0x00,0x50,0x02,
				0x20,0x00,0xbc,0x3e,0x00,0x00,0x12,0x34,
				0x56,0x78,0x9a,0xbc,0xde,0xff};
/*
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
*/

		uint8_t frame_handle;
		uint32_t vlan_tag = 0x8100c78d;

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
			fsl_os_print("ERROR: create frame failed with err = %x!\n", err);

		fsl_os_print("parse result after create frame - \n");
		
		fsl_os_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
				
		fsl_os_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
			

		fsl_os_print("tcp offset %d %x\n", 
					PARSER_IS_TCP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());

		for (i=0; i<16 ; i++)
			fsl_os_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));

		fdma_close_default_segment();

		err = fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS, (void *)0x180, 0, PRESENTATION_SIZE);
		if (err)
			fsl_os_print("STATUS: fdma present default segment returned status is %d\n", err);

		err = parse_result_generate_basic();
		if (err)
			fsl_os_print("STATUS: parser returned status is %d\n", err);

		fsl_os_print("parse result after new presentation - \n");
		
		fsl_os_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
				
		fsl_os_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
			

		fsl_os_print("tcp offset %d %x\n", 
					PARSER_IS_TCP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());

		for (i=0; i<16 ; i++)
			fsl_os_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));
	
	
#ifdef POP_VLAN
		
			uint16_t vlan_offset = (uint16_t)(PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT()) - 2;
			
			/* Remove VLAN header */
			fdma_delete_default_segment_data_wrap(vlan_offset,
								4,
								FDMA_REPLACE_SA_REPRESENT_BIT);

			// Update parse results manually
			// (1 vlan is left in the frame)
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_VLAN_N_MASK;
			pr->vlan_tcin_offset = pr->vlan_tci1_offset;
			pr->last_etype_offset = pr->last_etype_offset - 4;
			pr->ip_pid_offset = pr->ip_pid_offset - 4;
			pr->ip1_or_arp_offset = pr->ip1_or_arp_offset - 4;
			pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
			pr->l4_offset = pr->l4_offset - 4;
			pr->nxt_hdr_offset = pr->nxt_hdr_offset - 4;
			//pr->gross_running_sum = 0;
#endif
#ifdef PUSH_VLAN
			/* Add VLAN header */
			fdma_insert_default_segment_data_wrap(12,
							&vlan_tag,
							4,
							FDMA_REPLACE_SA_REPRESENT_BIT);
			
			fsl_os_print("parse result after push/pop vlan - \n");

/*
			err = parse_result_generate_basic();
			if (err)
				fsl_os_print("STATUS: parser returned status is %d\n", err);

*/

			// Update parse results manually
			// (Now 3 vlan's exist in the frame)
			if ((pr->l4_offset + TCP_HDR_LENGTH) < PRC_GET_SEGMENT_LENGTH()) {
				// (all TCP header is presented)
				pr->ip_pid_offset = pr->ip_pid_offset + 4;
				pr->vlan_tcin_offset = pr->vlan_tcin_offset + 4;
				pr->last_etype_offset = pr->last_etype_offset + 4;
				pr->ip1_or_arp_offset = pr->ip1_or_arp_offset + 4;
				pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
				pr->l4_offset = pr->l4_offset + 4;
				pr->nxt_hdr_offset = pr->nxt_hdr_offset + 4;
				//pr->gross_running_sum = 0;
			} else {
				// (not all TCP header is presented and therefore not parsed)
				pr->nxt_hdr = TCP_PROTOCOL; 
				pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_PARSING_ERROR_MASK; //required???
				pr->frame_attribute_flags_3 = 0;
				pr->ip_pid_offset = pr->ip_pid_offset + 4;
				pr->vlan_tcin_offset = pr->vlan_tcin_offset + 4;
				pr->last_etype_offset = pr->last_etype_offset + 4;
				pr->ip1_or_arp_offset = pr->ip1_or_arp_offset + 4;
				pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
				pr->l4_offset = 0xFF;
				pr->nxt_hdr_offset = pr->nxt_hdr_offset + 4 - TCP_HDR_LENGTH;
				pr->parse_error_code = PARSER_EXCEED_BLOCK_LIMIT; // required???
				//pr->gross_running_sum = 0;
			}
#endif

			for (i=0; i<16 ; i++)
				fsl_os_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));

			
			
			//	l2_push_and_set_vlan(vlan);
/*
		err = l2_pop_vlan();
		if (err)
				fsl_os_print("ERROR: pop vlan failed with err = %x!\n", err);

		fsl_os_print("parse result after push/pop vlan - \n");
		
		fsl_os_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
				
		fsl_os_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
			

		fsl_os_print("tcp offset %d %x\n", 
					PARSER_IS_TCP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());

		for (i=0; i<16 ; i++)
			fsl_os_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));
*/

		
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		
	}
	
	if (!err) {
		fsl_os_print("Simple bring-up test completed successfully\n");
	}
	
	return err;
}



