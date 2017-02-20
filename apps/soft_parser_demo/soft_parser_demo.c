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
uint8_t sparser_example[] __attribute__((aligned(4))) = {
	/**********************************************************************/
	/*                  Reads the EType from the Packet                   */
	/**********************************************************************/
	/*------------------- WR0 = Packet EType field -----------------------*/
	0xB7, 0x9E,
	/*
	* Load_Bits_FW_to_WR(m-n,m)
	*
	* 1-0-1101111-001111-0
	*                    |-> W = 0 (WR0);
	*            |-> n[0:5] = 0f = 15
	*     |-> m[0:6] = 6f = 111
	*  |-> S = 0 (No Shift)
	* |-> OpCode
	*
	* Load 16 bits into the WR0 from the bit positions 96 to 111 of
	* Frame Window.
	*
	* Byte offset = 96 / 8 = 12. Loads bytes 12 and 13, i.e. the EType
	* field, from the input packet into WR0.
	*
	* WR0 = FW[96:111] = Packet[12:13] = 0xEE00;
	*
	*/

	/**********************************************************************/
	/*              Reads the EType expected by this Software Parser      */
	/**********************************************************************/
	/*-------------------- WR1 = 0xEE00 (from parameters) ----------------*/
	0x10, 0x13,
	/*
	* Load_Bytes_PA_to_WR
	* 00010-0-000001-001-1
	*                    |-> W = 1 (WR1)
	*                |-> k[0:2] = 1
	*         |-> j[0:5] = 1
	*       |-> S = 0 (No Shift)
	* |-> OpCode
	*
	* Load the 2 bytes from byte positions 0 to 1 of the Parameter
	* Array into WR1. Since load to WRx is always 64-bits, the bytes to the
	* left of the 2 bytes are zeroed.
	*
	* WR1 = Parameter Array[0:1] = 0x0000EE00
	*
	*/

	/**********************************************************************/
	/*              Compare the 2 EType values                            */
	/**********************************************************************/
	/*--------- Return to ETH HXS if EType != 0xEE00 ---------------------*/
	/*--------- Continue if EType == 0xEE00 ------------------------------*/
	0x00, 0x79, 0x87, 0xFE,
	/*
	* Compare Working Regs WR0 <?> WR1
	* 15:Compare Working Regs WR0 <?> WR1
	* 0000000001111-001 : G[0]L[1]JumpDest[5:15]
	*               |-> c[0:2] = 1 (!=)
	*                     |-> 1000.0111.1111.1111 = 0x87 0xFF
	* |-> OpCode
	*
	* If (WR0 != WR1) then jump to 0x7FE (Return to Hard HXS).
	*
	* G[0]L[1]JumpDest[5:15]
	* 1-0-000-11111111110
	*         |-> JumpDest[5:15] = 0x7FE
	*     |-> Unused ?
	*   |-> L = 0; JumpDest is absolute
	* |-> G = 1; Jump is treated as a gosub
	*/

	/**********************************************************************/
	/*                     Advance to the Custom Header                   */
	/**********************************************************************/
	/*----------------- WR1 = NxtHdrOffset -------------------------------*/
	/*
	* Load_Bytes_RA_to_WR
	* 0011-0-0110010-000-1
	*                    |-> W = 1 (WR1)
	*                |-> k[0:2] = 000
	*        |-> j[0:6] = 0x32 = 50
	*      |-> S = 0 ; Not shifted
	* |-> OpCode
	*
	* Load the 1 (k+1) byte from byte positions 50 (j-k) to (50) j of the
	* Parse Array into WR1. Since loads to the working registers are always
	* 64-bits, the bytes to the left of the 1 (k+1) byte are zeroed in WR1.
	*
	* WR1 = Parse Array[50] = NxtHdrOffset = 14
	*
	*/
	0x33, 0x21,

	/*----------------------- WO = NxtHdrOffset --------------------------*/
	/*
	* Modify WO by WR
	* 0000.0000.1000.00-0-1
	*                     |-> W = 1 (WR1)
	*                   |-> A = 0
	* |-> OpCode
	*
	* Loads (A=0) the least significant 8b of WR0 into the Window Offset
	* register.
	*
	* Window Offset = WR1 = NxtHdrOffset = 14
	*
	*/
	0x00, 0x81,

	/*-------------- HB += NxtHdrOffset; WO = 0 --------------------------*/
	/*
	* Advance_HB_by_WO
	*
	* Add the value of Window Offset to Header Base & reset the value of WO
	* to 0. Used within an HXS to advance to HB for the following HXS.
	* It is appropriate to apply when WO is pointing to the first byte that
	* the next HXS is to examine.
	*
	* HB += 14;
	* WO = 0;
	* Note : On Custom header start.
	*
	*/
	0x00, 0x02,

	/**********************************************************************/
	/*                     Update NxtHdr field                            */
	/**********************************************************************/
	/*---------------- WR0 = Custom Header Length = 0x2E -----------------*/
	/*
	* Load_Bytes_PA_to_WR
	* 00010-0-000010-000-0
	*                    |-> W = 0 (WR0)
	*                |-> k[0:2] = 0
	*         |-> j[0:5] = 2
	*       |-> S = 0 (No Shift)
	* |-> OpCode
	*
	* Load the 1 (k+1) byte from byte positions 2 (j-k) to 2 (j) of the
	* Soft Examination Parameter Array into WR0. Since loads to the working
	* registers are always 64-bits, the bytes to the left of the 1 (k+1)
	* bytes are zeroed in WR0.
	*
	* WR0 = 0x2E
	*/
	0x10, 0x20,

	/*----------------------------- WR0 -= 16 ----------------------------*/
	/*
	* AddSub_WR_IV_to_WR IV3 IV2
	* 000000000101-0-0-1-0
	*                    |-> W = 0 (WR0)
	*                  |-> O = 1 (Subtraction)
	*                |-> V = 0 (WR0)
	*              |-> S = 0 (16 bits)
	* |-> OpCode
	*
	* The least significant 32b of WR0 is the first operand and 16b (S=0)
	* of immediate data is the second operand in a 32b subtraction (O=1)
	* operation. The 32b result is stored in the least significant 32b of
	* WR0 (V=0). No carries or underflow bits are captured. The most
	* significant 32b of WR0 are not affected by this instruction (they hold
	* their value).
	*
	* WR0 -= 16;
	*
	*/
	0x00, 0x52, 0x00, 0x10,

	/*----------------------------- WO = WR0 -----------------------------*/
	/*
	* Modify WO by WR
	* 0000.0000.1000.00-0-0
	*                     |-> W = 0 (WR0)
	*                   |-> A = 0 (Load)
	* |-> OpCode
	*
	* Loads (A=0) the least significant 8b of WR0 into the Window Offset
	* register.
	*
	* WO = WR0;
	*
	* Window Offset is set on the last 16 bytes of the Custom Header.
	*
	*/
	0x00, 0x80,

	/*------------------ WR1 = Custom Header EType -----------------------*/
	/*
	* Load_Bits_FW_to_WR(m-n,m)
	* 1-0-1111111-001111-1
	*                    |-> W = 1 (WR1)
	*             |-> n[0:5] = 0f = 15
	*     |-> m[0:6] = 7f = 127
	*   |-> S = 0 (No Shift)
	* |-> OpCode
	*
	* Load 16 (n+1) bits into the WR1 from the bit positions 112 (m-n) to
	* 127 (m) of Frame Window (was just move to + 30)
	*
	* 112 is byte 14 in the current Window. Window Offset is 30 and the
	* Header Base is on the custom header start.
	*
	* The byte 14 in the current window is the byte 30 + 14 = 44 in the
	* current custom header. It points to the EType field of the custom
	* header.
	*
	* WR1 = CustomHeader[44:45] = EType in Custom Header = 0x0800 (IPv4)
	*
	*/
	0xBF, 0x9F,

	/*----------- NextHdr = RA[16:17] = WR1 = Custom header ETYpe --------*/
	/*
	* Store_WR_to_RA
	* 00101-001-0010001-1
	*                   |-> W=1 (WR1)
	*           |-> t[0:6] = 0x11 = 17
	*       |-> s[0:2] = 1
	* |-> OpCode
	*
	* Stores the 2 (s+1) leftmost bytes (least significant) of WR1 into the
	* Parse Array at byte positions 26 (t-s) to 27 (t).
	*
	* Parse Array[16:17] = NxtHdr = PA[16:17] = WR1 = 0x0800;
	*
	*/
	0x29, 0x23,

	/*----------------- WR1 = NxtHdrOffset (ETH) -------------------------*/
	/*
	* Load_Bytes_RA_to_WR
	* 0011-0-0110010-000-1
	*                    |-> W = 1 (WR1)
	*                |-> k[0:2] = 000
	*        |-> j[0:6] = 0x32 = 50
	*      |-> S = 0 ; Not shifted
	* |-> OpCode
	*
	* Load the 1 (k+1) byte from byte positions 50 (j-k) to (50) j of the
	* Parse Array into WR1. Since loads to the working registers are always
	* 64-bits, the bytes to the left of the 1 (k+1) byte are zeroed in WR1.
	*
	* WR1 = Parse Array[50] = NxtHdrOffset = 14
	*
	*/
	0x33, 0x21,

	/**********************************************************************/
	/*                     Update ShimOffset_1 field                      */
	/**********************************************************************/
	/*--------------- ShimOffset_1 = RA[32] = WR1 ------------------------*/
	/*
	* Store_WR_to_RA
	* 00101-000-0100000-1
	*                   |-> W=1 (WR1)
	*           |-> t[0:6] = 0x20 = 32
	*       |-> s[0:2] = 0
	* |-> OpCode
	*
	* Stores the 1 (s+1) leftmost bytes (least significant) of WR1 into the
	* Parse Array at byte positions 32 (t-s) to 32 (t).
	*
	* ShimOffset_1 = Parse Array[32] = WR1;
	*
	*/
	0x28, 0x41,

	/**********************************************************************/
	/*                     Update NxtHdrOffset field                      */
	/**********************************************************************/
	/*---------------- WR0 = Custom Header Length = 0x2E -----------------*/
	/*
	* Load_Bytes_PA_to_WR
	* 00010-0-000010-000-0
	*                    |-> W = 0 (WR0)
	*                |-> k[0:2] = 0
	*         |-> j[0:5] = 2
	*       |-> S = 0 (No Shift)
	* |-> OpCode
	*
	* Load the 1 (k+1) byte from byte positions 2 (j-k) to 2 (j) of the
	* Soft Examination Parameter Array into WR0. Since loads to the working
	* registers are always 64-bits, the bytes to the left of the 1 (k+1)
	* bytes are zeroed in WR0.
	*
	* WR0 = 0x2E
	*/
	0x10, 0x20,

	/*--------------- WR1 += WR0 -----------------------------------------*/
	/*
	* AddSub_WR_WR_to_WR
	* 0000.0000.0100.1-100
	*                    |-> L = 0 (WR0)
	*                   |-> O = 0 (Addition)
	*                  |-> V = 1 (WR1)
	* |-> OpCode
	*
	* The least significant 32b of WR0 (L=0) is the first operand and the
	* least significant 32b of WR1 is the second operand in a 32b addition
	* (O=0) operation. The 32b result is stored in the least significant
	* 32b of WR1 (V=1).
	*
	*/
	0x00, 0x4C,

	/*--------------- NxtHdrOffset = RA[50] = WR1 ------------------------*/
	/*
	* Store_WR_to_RA
	* 00101-000-0110010-1
	*                   |-> W=1 (WR1)
	*           |-> t[0:6] = 0x32 = 50
	*       |-> s[0:2] = 0
	* |-> OpCode
	*
	* Stores the 1 (s+1) leftmost bytes (least significant) of WR1 into the
	* Parse Array at byte positions 50 (t-s) to 50 (t).
	*
	* NxtHdrOffset = Parse Array[50] = WR1;
	*
	*/
	0x28, 0x65,

	/*--------------- Set Bit 0 in FAF -----------------------------------*/
	/*
	* Set_Clr_FAF
	* 0000.0011-1-000.0000
	*             |-> j[0:6] = 0x000 = 0 (User Defined leftmost bit)
	*           |-> c = 1 (Set)
	*       |-> s[0:2] = 0
	* |-> OpCode
	*
	* Set (c=1) the Frame Attribute Flags at the indicated index
	* 96 (j[0:6]).
	*
	*/
	0x03, 0x80,	/* FAF (User Defined leftmost bit) */

	/*----------------------- WO = WR0 = 46 ------------------------------*/
	/*
	* Modify WO by WR
	* 0000.0000.1000.00-0-0
	*                     |-> W = 0 (WR0)
	*                   |-> A = 0
	* |-> OpCode
	*
	* Loads (A=0) the least significant 8b of WR0 into the Window Offset
	* register.
	*
	* Window Offset = WR0 = 46
	*
	*/
	0x00, 0x80,

#if 1
	/**********************************************************************/
	/*       Jump to the protocol found in the EType field of the         */
	/*                  Custom Header (IPv4 HXS)                          */
	/**********************************************************************/
	/*----------------- Jump to protocol 0x0800 --------------------------*/
	/*
	* Jump_Protocol
	* 0000.0000.0100.01-00
	*                   |-> P = 0
	* |-> OpCode
	*
	* Execution jumps to the HXS decoded in the NxtHdr field of the Parse
	* Array. Used to jump to the next HXS based on a protocol code.
	* The jump is treated as an advancement to a new HXS (the HB is updated
	* to HB+WO, WO and WRs are reset).
	* If the P code is 0, NxtHdr is evaluated against the following
	* EtherTypes:
	*	-0x05DC of less: jump to LLC-SNAP
	*	-0x0800: jump to IPv4
	*	-0x0806: jump to ARP
	*	-0x86dd: jump to IPv6
	*	-0x8847, 0x8848: jump to MPLS-0x8100, 0x88A8,ConfigTPID1,
	*		ConfigTPID2: jump to VLAN
	*	-0x8864: jump to PPPoE+PPP
	*	-unknown: jump to Other L3 Shell
	*/
	0x00, 0x44,
#else
	/*
	 * In order to see the mandatory fields are correctly updated by this
	 * Software Parser, just jump to Parse End.
	 *
	 * The FAF bit #0 was set at PC = 16.
	 *
	 */
	/*------------------------- Jumps to Parse End -----------------------*/
	0x0C, 0x80, 0x87, 0xFF,
	/*
	* Jump_FAF
	* 0000.1100. 1 j[0:6]
	*              |-> j[0:6] = 0000000; FAF (User Defined leftmost bit)
	*            |-> A = 1
	* |-> OpCode
	*
	* Jump to 0x7FF (End Parsing) ib Bit #0 is set in FAF.
	*
	* G[0]L[1]JumpDest[5:15]
	* 1-0-000-11111111111
	*         |-> JumpDest[5:15] = 0x7FF
	*     |-> Unused ?
	*   |-> L = 0; JumpDest is absolute
	* |-> G = 1; Jump is treated as a gosub
	*
	*/
#endif
	/*--------------------------------------------------------------------*/
	0x00, 0x00,
	/*
	* NOP; NOP needed so that the size is a 4 multiple
	*/

};

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
	uint8_t *eth_pointer_byte = 0;
	int i;
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

	fsl_print("Printing Frame. FD[len] = %d, Seg Len = %d\n",
		  frame_len, seg_len);
	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	for (i = 0; i < frame_len && i < seg_len; i++) {
		if ((i % 16) == 0) {
			fsl_print("00");
			if (i < 16)
				fsl_print("0");
			fsl_print("%x  ", i);
		}
		if (*eth_pointer_byte < 16)
			fsl_print("0");
		fsl_print("%x ", *eth_pointer_byte);
		if ((i % 8) == 7)
			fsl_print(" ");
		if ((i % 16) == 15)
			fsl_print("\n");
		eth_pointer_byte++;
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
static int soft_parser_develop_debug(void)
{
	int				err;
	struct sp_parse_array		ra;
	uint8_t				prpid = 0;
	uint8_t				pa[3];
	struct sparser_info		sp_info;
	struct dpni_drv_sparser_param	sp_param;

	/****************************/
	/* Soft Parser disassembler */
	/****************************/
	err = sparser_disa(0x20, &sparser_example[0], sizeof(sparser_example));
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
	err = sparser_sim(0x20, &sparser_example[0], sizeof(sparser_example));
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
	sp_info.byte_code = &sparser_example[0];
	sp_info.size = sizeof(sparser_example);
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
	/* 5. Activate the SP */
	pa[0] = 0xEE;	/* Expected EType = 0xEE00 */
	pa[1] = 0x00;
	pa[2] = 46;	/* Custom Header Length in bytes */
	sp_param.link_hxs = PARSER_ETH_STARTING_HXS;
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
