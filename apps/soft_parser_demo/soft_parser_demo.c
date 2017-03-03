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

/*******************************************************************************
 *                              - CAUTION -
 *
 * This code must not be distributed till we haven't a clear understanding
 * about what internals of the Parser may be exposed to customers.
 ******************************************************************************/

#include "fsl_types.h"
#include <__mem.h>
#include "fsl_evmng.h"
#include "fsl_dpni_drv.h"
#include "apps.h"

#include "fsl_sparser_gen.h"
#include "fsl_sparser_disa.h"
#include "fsl_sparser_drv.h"
#include "fsl_sparser_dump.h"
#include "fsl_sparser_gen.h"

/******************************************************************************/
/* Soft Parser Example :
 *
 * This SP parses a custom header placed after a Ethernet header. It is
 * identified by the "custom" EType 0xEE00. The custom header has 46 bytes in
 * length. The last 2 bytes in this header identifies the type of the header
 * that follows, in this example 0x0800, i.e. IPv4.
 *
 * The custom EType (0xEE00) and the custom header length, are given as
 * parameters in the Parameters Array of the Parse Profile configured on each
 * of the DPNI objects belonging to the AIOP container.
 *
 * If the EType field of the parsed packet is not the expected one (0xEE00) the
 * SP code returns to the calling HXS (Ethernet).
 */
uint8_t sparser_ex[44] __attribute__((aligned(4))) = {0x00, 0x00, 0x00, 0x00};

/******************************************************************************/
/* Packet to be parsed
 *
 * The EType field in the Ethernet header is 0xEE00.
 * The EType field in the custom header is 0x0800.
 */
uint8_t parsed_packet[] = {
	/* Ethernet Header */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x21, 0xee, 0x00,
	/* Custom Header */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x08, 0x00,
	/* IP Header */
	0x45, 0x00, 0x00, 0xc0, 0x00, 0x13, 0x00, 0x00,
	0xff, 0x11, 0x39, 0xc1, 0xc0, 0x55, 0x01, 0x02,
	0xc0, 0x00, 0x00, 0x01,
	/* UDP Header */
	0x04, 0x00, 0x04, 0x00, 0x00, 0xac, 0xff, 0xff,
	/* Payload (without FCS) */
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
};

/******************************************************************************/
static void sp_print_frame(void)
{
	uint16_t	seg_len;
	uint32_t	frame_len;
	uint8_t		*pb;
	int		i;

	seg_len = PRC_GET_SEGMENT_LENGTH();
	frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	fsl_print("Printing Frame. FD[len] = %d, Seg Len = %d\n",
		  frame_len, seg_len);
	pb = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	for (i = 0; i < frame_len && i < seg_len; i++) {
		if ((i % 16) == 0) {
			fsl_print("00");
			if (i < 16)
				fsl_print("0");
			fsl_print("%x  ", i);
		}
		if (*pb < 16)
			fsl_print("0");
		fsl_print("%x ", *pb);
		if ((i % 8) == 7)
			fsl_print(" ");
		if ((i % 16) == 15)
			fsl_print("\n");
		pb++;
	}
	if ((i % 16) != 0)
		fsl_print("\n");
}

/******************************************************************************/
static __HOT_CODE ENTRY_POINT void app_process_packet(void)
{
	int		err;

	sl_prolog();
	fsl_print("\n\nSP Demo: Core %d Received Frame\n", core_get_id());
	sp_print_frame();
	fsl_print("\n");
	sparser_parse_result_dump((struct sp_parse_result *)
				  HWC_PARSE_RES_ADDRESS);
	sparser_frame_attributes_dump((struct sp_parse_result *)
				      HWC_PARSE_RES_ADDRESS);
	sparser_parse_error_print((struct sp_parse_result *)
				  HWC_PARSE_RES_ADDRESS);
	err = dpni_drv_send(task_get_receive_niid(),
			    DPNI_DRV_SEND_MODE_NONE);
	if (err) {
		fsl_print("ERROR = %d: dpni_drv_send(ni_id)\n", err);
		if (err == -ENOMEM)
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		else /* (err == -EBUSY) */
			IPSEC_FDMA_DISCARD_FD();
	}
	fdma_terminate_task();
}

/******************************************************************************/
int app_early_init(void)
{
	return 0;
}

/******************************************************************************/
static int app_dpni_event_added_cb(uint8_t generator_id, uint8_t event_id,
				   uint64_t app_ctx, void *event_data)
{
	uint16_t			ni, mfl;
	int				err;
	struct ep_init_presentation	init_presentation;

	UNUSED(generator_id);
	UNUSED(event_id);
	ni = (uint16_t)((uint32_t)event_data);
	mfl = 0x2000; /* Maximum Frame Length */
	pr_info("Event received for AIOP NI ID %d\n", ni);
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("dpni_drv_register_rx_cb for ni %d failed: %d\n",
		       ni, err);
		return err;
	}
	err = dpni_drv_set_max_frame_length(ni, mfl /* Max frame length*/);
	if (err) {
		pr_err("dpni_drv_set_max_frame_length for ni %d failed: %d\n",
		       ni, err);
		return err;
	}
	/* Set "Exclusive" execution. Packets are processed one by one in the
	 * default ordering scope. This is needed only for printing coherence
	 * output. */
	err = dpni_drv_set_exclusive(ni);
	if (err) {
		pr_err("Cannot set Excluseve execution on NI %d\n", ni);
		return err;
	}
	/* Set the initial segment presentation size to maximum */
	#define EP_INIT_PRESENTATION_OPT_SPS		0x0080
	#define PRESENTATION_LENGTH			256
	err = dpni_drv_get_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot get initial presentation for NI %d\n", ni);
		return err;
	}
	init_presentation.options = EP_INIT_PRESENTATION_OPT_SPS;
	init_presentation.sps = PRESENTATION_LENGTH;
	err = dpni_drv_set_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot set initial presentation for NI %d to %d\n",
		       ni, init_presentation.sps);
		return err;
	}
	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("dpni_drv_enable for ni %d failed: %d\n", ni, err);
		return err;
	}
	return 0;
}

/******************************************************************************/
#define ASM_LOOK_LIKE	1

/******************************************************************************/
#if (ASM_LOOK_LIKE == 0)
static void soft_parser_example_gen(void)
{
	memset(&sparser_ex[0], 0, sizeof(sparser_ex));
	/* Start byte-code write */
	sparser_begin_bytecode_wrt(0x20, &sparser_ex[0], sizeof(sparser_ex));
	/* Soft parser is called at the end of the Ethernet HXS execution.
	 *	HB = 0 (it is not changed by the call of soft parser)
	 *	WO = 0 is cleared
	 *	WR0 = 0 is cleared
	 *	WR1 = 0 is cleared
	 *	Frame Window (FW) points to the first 16 bytes in the received
	 *	packet
	 **********************************************************************
	 * Compare the EType field in the received packet with the expected
	 * EType.
	 **********************************************************************
	 * WR0 = Packet EType field
	 *	offset : 12 bytes from FW start
	 *	size   : 2 bytes) */
	sparser_gen_ld_fw_to_wr0(96, 16);
	/* WR1 = Expected Packet type from SP parameters (0xEE00).
	 *	offset : 0 bytes from Parameters Array start
	 *	size   : 2 bytes) */
	sparser_gen_ld_pa_to_wr1(0, 2);
	/* Compares the 2 values on non equality.
	 *
	 * If comparison result is true then the received packet does not
	 * contain the expected custom header and a return to the calling HXS
	 * (Ethernet) is performed. Soft parser execution ends here.
	 *
	 * If comparison result is false then the received packet contains the
	 * expected custom header and the soft parser execution continues with
	 * the next instruction. */
	sparser_gen_cmp_wr0_ne_wr1(SP_RETURN_TO_HXS_DST);
	/**********************************************************************
	 * Advance HB on the custom header start
	 **********************************************************************
	 * WR1 = NxtHdrOffset from the Parse Array. NxtHdrOffset was set by the
	 * Ethernet HXS.
	 *	offset : 50 bytes from Parse Array start
	 *	size   : 1 byte)
	 * WR1 = 14 */
	sparser_gen_ld_ra_to_wr1(50, 1);
	/* Prepare the advancement of HB by setting the Window Offset value :
	 *	WO = WR1 = 14 */
	sparser_gen_ld_wr1_to_wo();
	/* Advance HB to the custom header offset :
	 *	HB = 14
	 *	WO = 0 is cleared
	 *	WR0 = 0 is cleared
	 *	WR1 = 0 is cleared
	 */
	sparser_gen_advance_hb_by_wo();
	/**********************************************************************
	 * Soft Parsers must set the NxtHdr in the Parse Array. NxtHdr is Parse
	 * Array[16:17]. To do that the EType field of the custom header must be
	 * read.
	 **********************************************************************
	 * Set the WO to point on the last 16 bytes of the custom header.
	 * WR0 = Custom header size from SP parameters (46)
	 *	offset : 2 bytes from Parameters Array start
	 *	size   : 1 byte)
	 * WR0 = 46 */
	sparser_gen_ld_pa_to_wr0(2, 1);
	/* Subtract 16 from the custom header length :
	 *	WR0 = 36 */
	sparser_gen_sub32_wr0_imm_to_wr0(0x10, SP_IMM_16);
	/* Set WO on the last 16 bytes of the custom header :
	 *	WO = 36 */
	sparser_gen_ld_wr0_to_wo();
	/* WR0 = EType field in the custom header (0x0800)
	 *	offset : 14 bytes from FW start
	 *	size   : 2 bytes) */
	sparser_gen_ld_fw_to_wr1(112, 16);
	/* Set the "NxtHdr" in the Parse Array :
	 *	offset : 16 bytes from Parse Array start
	 *	size   : 2 bytes)
	 *
	 * NxtHdr = Parse Array[16:17] = 0x0800
	 */
	sparser_gen_st_wr1_to_ra(16, 2);
	/**********************************************************************
	 * Custom header offset is stored in ShimOffset_1 field of the Parse
	 * Array[32]
	 **********************************************************************
	 * WR1 =  NxtHdrOffset from the Parse Array.
	 *	offset : 50 bytes from Parse Array start
	 *	size   : 1 byte)
	 * WR1 = 14 */
	sparser_gen_ld_ra_to_wr1(50, 1);
	/* ShimOffset_1 = Parse Array[32] = 14 */
	sparser_gen_st_wr1_to_ra(32, 1);
	/**********************************************************************
	 * Soft Parsers must set the NxtHdrOffst in the Parse Array.
	 * NxtHdrOffst = current NxtHdrOffst + custom header length.
	 **********************************************************************
	 * WR0 = custom header length = 46 */
	sparser_gen_ld_pa_to_wr0(2, 1);
	/* WR1 = Ethernet NxtHdrOffset = 14
	 * WR0 = custom header length = 46
	 *
	 * WR1 = 46 + 14 = 60 */
	sparser_gen_add32_wr0_to_wr1();
	/* NxtHdrOffst = Parse Array[50] = 60 */
	sparser_gen_st_wr1_to_ra(50, 1);
	/**********************************************************************
	 * Custom header presence is marked in bit #0 of the Frame Attribute
	 * Flags extension.
	 *********************************************************************/
	sparser_gen_set_faf_bit(SP_FAF_UD_SOFT_PARSER_0);
	/**********************************************************************
	 * Before calling the HXS processing the next header, a soft parser must
	 * prepare the Header Base advancement.	HB is advanced with the current
	 * value of WO.
	 *
	 * Set the WO to the length of the custom header.
	 *	WO = 46
	 *********************************************************************/
	sparser_gen_ld_wr0_to_wo();
	/* HB = HB + WO = 14 + 46 = 60
	 * WO = 0
	 * WR0 = 0
	 * WR1 = 0
	 *
	 * Jump to IPv4 HXS because now
	 *	NxtHdr = Parse Array[16:17] = 0x0800 */
	sparser_gen_jump_to_l2_protocol();
	/* NOP to make the SP size a multiple of 4 */
	sparser_gen_nop();
	/* End byte-code write */
	sparser_end_bytecode_wrt();
}

#else	/* ASM_LOOK_LIKE = 1 */

/******************************************************************************/
static void soft_parser_example_gen(void)
{
#define SP_EXAMPLE							  \
	do {								  \
		SPARSER_BEGIN(0x20, &sparser_ex[0], sizeof(sparser_ex));  \
			/* 0x020 */ LD_FW_TO_WR0(96, 16);		  \
			/* 0x021 */ LD_PA_TO_WR1(0, 2);			  \
			/* 0x022 */ CMP_WR0_NE_WR1(SP_RETURN_TO_HXS_DST); \
			/* 0x024 */ LD_RA_TO_WR1(50, 1);		  \
			/* 0x025 */ LD_WR1_TO_WO;			  \
			/* 0x026 */ ADVANCE_HB_BY_WO;			  \
			/* 0x027 */ LD_PA_TO_WR0(2, 1);			  \
			/* 0x028 */ SUB32_WR0_IMM_TO_WR0(0x10, SP_IMM_16);\
			/* 0x02a */ LD_WR0_TO_WO;			  \
			/* 0x02b */ LD_FW_TO_WR1(112, 16);		  \
			/* 0x02c */ ST_WR1_TO_RA(16, 2);		  \
			/* 0x02d */ LD_RA_TO_WR1(50, 1);		  \
			/* 0x02e */ ST_WR1_TO_RA(32, 1);		  \
			/* 0x02f */ LD_PA_TO_WR0(2, 1);			  \
			/* 0x030 */ ADD32_WR0_TO_WR1;			  \
			/* 0x031 */ ST_WR1_TO_RA(50, 1);		  \
			/* 0x032 */ SET_FAF_BIT(SP_FAF_UD_SOFT_PARSER_0); \
			/* 0x033 */ LD_WR0_TO_WO;			  \
			/* 0x034 */ JMP_TO_L2_PROTOCOL;			  \
			/* 0x035 */ NOP;				  \
		SPARSER_END;						  \
	} while (0)

	SP_EXAMPLE;
}
#endif	/* ASM_LOOK_LIKE */

/******************************************************************************/
static int soft_parser_develop_debug(void)
{
	int				err;
	struct sp_parse_array		ra;
	uint8_t				prpid = 0;
	uint8_t				pa[3];
	struct sparser_info		sp_info;
	struct dpni_drv_sparser_param	sp_param;

	/************************/
	/* Soft Parser generate */
	/************************/
	soft_parser_example_gen();
	/*****************************/
	/* Soft Parser bytecode dump */
	/*****************************/
	sparser_bytecode_dump();
	/****************************/
	/* Soft Parser disassembler */
	/****************************/
	err = sparser_disa(0x20, &sparser_ex[0], sizeof(sparser_ex));
	if (err)
		return err;
	/**********************************/
	/* Soft Parser built-in simulator */
	/**********************************/
	/* 1. Initializes the built-in simulator (Mandatory) */
	sparser_sim_init();
	/* 2. Sets the packet to be parsed */
	err = sparser_sim_set_parsed_pkt(&parsed_packet[0],
					 sizeof(parsed_packet));
	if (err)
		return err;
	/* 3. Initializes the Parse Array with the default values */
	err = sparser_sim_init_parse_array(&ra);
	if (err)
		return err;
	/* 4. Set needed fields in the Parse Array. This step is needed to
	 * change the default values. */
	/* Set relevant fields in the Parse Array as after ETH HXS execution */
	ra.pr.nxt_hdr = 0xee00;		/* NxtHeader */
	ra.pr.last_etype_offset = 12;	/* Last EType Offset */
	ra.pr.nxt_hdr_offset = 14;	/* Next Header Offset */
	/* TODO : Set the Gross Running Sum and the Running Sum
	 * These values should be computed on the "Packet to be parsed" */
	err = sparser_sim_set_parse_array(&ra);
	if (err)
		return err;
	/* 5. Sets the Parameter Array. Parameter Array must contain the values
	 * expected by the soft parser. If the soft parser expects no
	 * parameters, this step is not needed. */
	pa[0] = 0xEE;	/* Expected EType = 0xEE00 */
	pa[1] = 0x00;
	pa[2] = 46;	/* Custom Header Length in bytes */
	err = sparser_sim_set_parameter_array(&pa[0], 0, 3);
	if (err)
		return err;
	/* 6. Sets the Header Base. If the Header Base of the custom header to
	 * be parsed is computed by the soft parser based on the provided
	 * input (Parse Array or Parameter Array), this step is not needed. */
	err = sparser_sim_set_header_base(0);
	if (err)
		return err;
	/* 7. Sets the PC limit count. Default is 0, i.e. limit count check is
	 * disabled. */
	err = sparser_sim_set_pc_limit(SP_SIM_MAX_CYCLE_LIMIT);
	if (err)
		return err;
	/* 8. Call the built-in simulator */
	err = sparser_sim(0x20, &sparser_ex[0], sizeof(sparser_ex));
	if (err)
		return err;
	/* 9. Dump Parse Results */
	sparser_sim_parse_result_dump();
	/* 10. Dump Frame Attribute Flags */
	sparser_sim_frame_attributes_dump();
	/* 11. Dump Parse Status/Error */
	sparser_sim_parse_error_print();
	/*****************************/
	/* Soft Parser load/activate */
	/*****************************/
	/* 1. Dump parser registers and instructions memory */
	/*
	sparser_drv_regs_dump();
	sparser_drv_memory_dump(PARSER_MIN_PC, PARSER_MAX_PC + 3);
	*/
	/* 2. Adjust the Parser Cycle Limit value */
	sparser_drv_set_pclim(PARSER_CYCLE_LIMIT_MAX);
	/* 3. Load Soft Parser */
	sp_info.pc = PARSER_MIN_PC;
	sp_info.byte_code = &sparser_ex[0];
	sp_info.size = sizeof(sparser_ex);
	sp_info.param_off = 0;
	sp_info.param_size = 3;
	/* SPs of SPARSER_MC_WRIOP and SPARSER_MC_WRIOP_AIOP types are sent
	 * to MC. MC is responsible for SP managing/loading. The SP are loaded
	 * by MC in the WRIOP Parser :
	 *	- the SPARSER_MC_WRIOP SP is intended to parse the packets
	 *	going to GPP. This is just an extension of the AIOP application
	 *	allowing development of the MC only belonging SPs from this
	 *	AIOP application.
	 *	- the SPARSER_MC_WRIOP_AIOP SP is intended to parse the packets
	 *	going to AIOP.
	 *
	 * The SPARSER_MC_AIOP type must not be used for SP loading.
	 */
	sp_info.type = SPARSER_AIOP_AIOP;
	/* Now, all DPNIs share the same Parse Profile. Get the ID of the Parse
	 * Profile configured on the first DPNI */
	dpni_drv_get_parse_profile_id(0, &prpid);
	sp_info.prpid = prpid;
	err = sparser_drv_load_parser(&sp_info);
	if (err) {
		fsl_print("Soft Parser loading failed\n");
		return err;
	}
	fsl_print("Soft Parser loading succeeded\n");
	/* 4. Dump parser registers and instructions memory */
	/*
	sparser_drv_regs_dump();
	sparser_drv_memory_dump(sp_info.pc, sp_info.pc + sp_info.size / 2);
	sparser_drv_memory_dump(PARSER_MIN_PC, PARSER_MAX_PC + 3);
	*/
	/* 5. Activate the SP and stores its parameters in the parse profile.
	 * Only the custom headers following a known header are activated.
	 * For a custom header placed in the first position in the packet :
	 *	- call this function only if the soft parser has parameters
	 *	- call the sl_prolog_with_custom_header() function instead of
	 *	sl_prolog() in the packets processing function. */
	pa[0] = 0xEE;	/* Expected EType = 0xEE00 */
	pa[1] = 0x00;
	pa[2] = 46;	/* Custom Header Length in bytes */
	/* First header may be set only on Rev2 platforms */
	sp_param.first_header = 0;
	/* If "first_header" is set "link_to_hard_hxs" does not matter */
	sp_param.link_to_hard_hxs = PARSER_ETH_STARTING_HXS;
	sp_param.start_pc = PARSER_MIN_PC;
	sp_param.param_array = (uint8_t *)&pa[0];
	sp_param.param_offset = 0;
	sp_param.param_size = 3;
	sp_param.parser = PARSER_AIOP;
	err = dpni_drv_activate_soft_parser(prpid, &sp_param);
	if (err) {
		fsl_print("Soft Parser activation failed\n");
		return err;
	}
	fsl_print("Soft Parser activation succeeded\n");
	return 0;
}

/******************************************************************************/
int app_init(void)
{
	int				err;

	fsl_print("Running app_init()\n");
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t)app_process_packet,
			     app_dpni_event_added_cb);
	if (err) {
		pr_err("EVM registration for DPNI_EVENT_ADDED failed: %d\n",
		       err);
		return err;
	}
	err = soft_parser_develop_debug();
	if (err)
		return err;
	fsl_print("To start test inject packets: \"eth_ipv4_udp.pcap\"\n");
	return 0;
}

/******************************************************************************/
void app_free(void)
{
	/* TODO - complete ! */
}
