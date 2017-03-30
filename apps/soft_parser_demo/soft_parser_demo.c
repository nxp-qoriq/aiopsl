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
uint8_t sparser_ex[48] __attribute__((aligned(4)));

#ifdef SP_RUNNING_SUM
/* Running Sum computation AIOP SL library routine */
uint8_t sparser_running_sum[388] __attribute__((aligned(4)));
#endif

/******************************************************************************/
/* Packet to be parsed
 *
 * The EType field in the Ethernet header is 0xEE00.
 * The EType field in the custom header is 0x0800.
 */
uint8_t parsed_packet[] = {
	/* Ethernet Header */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x10,
	0x94, 0x00, 0x00, 0x02, 0xee, 0x00,
	/* Custom Header */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x08, 0x00,
	/* IP Header */
	0x45, 0x00, 0x00, 0x3d, 0x00, 0x00, 0x00, 0x00,
	0xff, 0x11, 0x3a, 0x57, 0xc0, 0x55, 0x01, 0x02,
	0xc0, 0x00, 0x00, 0x01,
	/*UDP Header */
	0x04, 0x00, 0x04, 0x00, 0x00, 0x29, 0x65, 0x42,
	/* Payload (without FCS) */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20
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
	fsl_print("%03x: ", 0);
	pb = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	for (i = 0; i < frame_len && i < seg_len; i++) {
		fsl_print("%02x ", *pb++);
		if (!((i + 1) % 16))
			fsl_print("\n%03x: ", i + 1);
	}
	if (i % 16)
		fsl_print("\n");
}

/******************************************************************************/
static uint16_t running_sum_diff(uint16_t grs, uint16_t rs)
{
	uint32_t	sum;

	sum = grs;
	sum += (~rs) & 0xffff;
	/* Fold sum to 16 bits */
	sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)sum;
}

/******************************************************************************/
static uint16_t compute_running_sum(uint8_t *header, uint16_t len)
{
	uint16_t		i, *pval16;
	uint32_t		sum;

	if (!len || !header) {
		fsl_print("%s : Wrong parameters\n", __func__);
		return 0;
	}
	pval16 = (uint16_t *)header;
	for (sum = 0, i = 0; i < len; i += 2)
		sum += *pval16++;
	if (len % 2)
		sum += (uint16_t)(*((uint8_t *)pval16) << 8);
	/* Fold sum to 16 bits */
	sum = (sum & 0xFFFF) + (sum >> 16);
	/* Add carrier (0/1) to result */
	sum += (sum >> 16);
	return (uint16_t)sum;
}

/******************************************************************************/
static __HOT_CODE ENTRY_POINT void app_process_packet(void)
{
	int			err;
	struct parse_result	*pr;

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
	/* Validate L4 checksum */
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	/* If packet is updated, in order to validate the checksum one must
	 * invalidate the gross running sum */
	/*pr->gross_running_sum = 0;*/
	err = parse_result_generate_default(PARSER_VALIDATE_L4_CHECKSUM);
	if (err)
		fsl_print("PARSER_VALIDATE_L4_CHECKSUM : ERROR = %d\n", err);
	else
		fsl_print("PARSER_VALIDATE_L4_CHECKSUM : SUCCEEDED\n");
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

#ifdef SP_RUNNING_SUM
/******************************************************************************/
static void soft_parser_running_sum(void)
{
	/* This routine computes the "Running Sum" on a header of a given
	 * length. The length of the header must be set by the calling soft
	 * parser into the GPRV0 register of the Parse Array, using the
	 * instruction :
	 *	ST_IMM_BYTES_TO_RA(0, 2, <length>, sp_imm_16);
	 * The routine code is relocatable. It reserves, for internal
	 * computations needs, the GPRV1 and GPRV2 registers. The WR registers
	 * are not preserved.
	 * The "Running Sum" value in the Parse Array is updated with the
	 * computed value. The "Window Offset" register is set to point at the
	 * beginning of the next header. Calling soft parser may advance to the
	 * next header just after the routine returns, using the instruction
	 * ADVANCE_HB_BY_WO or other instructions advancing the HB
	 */
#define SP_COMPUTE_RUNNING_SUM						  \
	do {								  \
		SPARSER_BEGIN(0x0038, &sparser_running_sum[0],		  \
					sizeof(sparser_running_sum));	  \
			/* WO = 0 */					  \
			/* 0x038 */ LD_SV_TO_WO(0x00);			  \
			/* GPRV1 = RA[2:3] = Length / 16 */		  \
			/* 0x039 */ LD_RA_TO_WR0(sp_ra_gprv_0, 2);	  \
			/* 0x03a */ SHR_WR0_BY_SV(4);			  \
			/* 0x03b */ ST_WR0_TO_RA(sp_ra_gprv_1, 2);	  \
			/* GPRV2 = RA[4:5] = Length % 16 */		  \
			/* 0x03c */ SHL_WR0_BY_SV(4);			  \
			/* 0x03d */ LD_RA_TO_WR1(sp_ra_gprv_0, 2);	  \
			/* 0x03e */ SUB32_WR0_FROM_WR1_TO_WR0;		  \
			/* 0x03f */ ST_WR0_TO_RA(sp_ra_gprv_2, 2);	  \
			/**************************************/	  \
			/* Checksum on a multiple of 16 bytes */	  \
			/**************************************/	  \
		SP_LABEL(sp_label_1);					  \
			/* Remained more than 16 bytes */		  \
			/* WR0 = GPRV1 = Full FWs */			  \
			/* 0x040 */ LD_RA_TO_WR0(sp_ra_gprv_1, 2);	  \
			/* 0x041 */ CLR_WR1;				  \
			/* If WR0 == 0 jump to remainder */		  \
			/* 0x042 */ CMP_WR0_EQ_WR1(sp_label_2);		  \
			/* Not finished integer part */			  \
			/* GPRV1 -= 1 */				  \
			/* 0x044 */ SUB32_WR0_IMM_TO_WR0(0x0001,	  \
							sp_imm_16);	  \
			/* 0x046 */ ST_WR0_TO_RA(sp_ra_gprv_1, 2);	  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x047 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x048 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..63] = first 8 bytes in the FW */	  \
			/* 0x04d */ LD_FW_TO_WR1(0, 64);		  \
			/* WR0 += SUM16 */				  \
			/* 0x04e */ CHECKSUM16_WR1_TO_WR0;		  \
			/* WR1 = FW[64..127] = next 8 bytes in the FW */  \
			/* 0x04f */ LD_FW_TO_WR1(64, 64);		  \
			/* WR0 += SUM16 */				  \
			/* 0x050 */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x051 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x056 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Advance the WO to the next FW */		  \
			/* 0x057 */ ADD_SV_TO_WO(16);			  \
			/* Continue with the next FW */			  \
			/* 0x058 */ JMP(sp_label_1);			  \
			/**********************************/		  \
			/* Checksum on less than 16 bytes */		  \
			/**********************************/		  \
		SP_LABEL(sp_label_2);					  \
			/* WR0 = GPRV2 = Remainder FW */		  \
			/* 0x05a */ LD_RA_TO_WR0(sp_ra_gprv_2, 2);	  \
			/* WR1 = 8 */					  \
			/* 0x05b */ LD_IMM_BITS_TO_WR1(16, 0x0008,	  \
							sp_imm_16);	  \
			/* 0x05d */ CMP_WR0_LT_WR1(sp_label_3);		  \
			/* Remained more than than 8 bytes */		  \
			/* GPRV2 -= 8 */				  \
			/* 0x05f */ SUB32_WR1_FROM_WR0_TO_WR0;		  \
			/* 0x060 */ ST_WR0_TO_RA(sp_ra_gprv_2, 2);	  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x061 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x062 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..63] = 8 bytes from the FW */	  \
			/* 0x067 */ LD_FW_TO_WR1(0, 64);		  \
			/* WR0 += SUM16 */				  \
			/* 0x068 */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x069 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x06e */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Advance the WO to the next FW */		  \
			/* 0x06f */ ADD_SV_TO_WO(8);			  \
			/*********************************/		  \
			/* Checksum on less than 8 bytes */		  \
			/*********************************/		  \
		SP_LABEL(sp_label_3);					  \
			/* Remained less than 8 bytes */		  \
			/* 0x070 */ LD_RA_TO_WR0(sp_ra_gprv_2, 2);	  \
			/* Remained is 0, 1, 2, 3 ? */			  \
			/* 0x071 */ LD_IMM_BITS_TO_WR1(64,		  \
					0x0003000200010000, sp_imm_64);	  \
			/* 0x076 */ CASE4_DC_WR_to_WR(sp_label_4,	  \
					sp_label_5, sp_label_6,		  \
					sp_label_7);			  \
			/* Remained is 4, 5, 6, 7 ? */			  \
			/* 0x07b */ LD_IMM_BITS_TO_WR1(64,		  \
					0x0007000600050004, sp_imm_64);	  \
			/* 0x080 */ CASE4_DC_WR_to_WR(sp_label_8,	  \
					sp_label_9, sp_label_10,	  \
					sp_label_11);			  \
		SP_LABEL(sp_label_4); /* 0 bytes */			  \
			/* 0x085 */ RETURN_SUB;				  \
		SP_LABEL(sp_label_5); /* 1 byte */			  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x086 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x087 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..7] = 1 byte from the FW */	  \
			/* 0x08c */ LD_FW_TO_WR1(0, 8);			  \
			/* 0x08d */ SHL_WR1_BY_SV(8);			  \
			/* 0x08e */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x08f */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x094 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* 0x095 */ ADD_SV_TO_WO(1);			  \
			/* 0x096 */ RETURN_SUB;				  \
		SP_LABEL(sp_label_6); /* 2 bytes */			  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x097 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x098 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..7] = 2 bytes from the FW */	  \
			/* 0x09d */ LD_FW_TO_WR1(0, 16);		  \
			/* 0x09e */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x09f */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x0a4 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* 0x0a5 */ ADD_SV_TO_WO(2);			  \
			/* 0x0a6 */ RETURN_SUB;				  \
		SP_LABEL(sp_label_7); /* 3 bytes */			  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x0a7 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x0a8 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..7] = 3 bytes from the FW */	  \
			/* 0x0ad */ LD_FW_TO_WR1(0, 24);		  \
			/* 0x0ae */ SHL_WR1_BY_SV(8);			  \
			/* 0x0af */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x0b0 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x0b5 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* 0x0b6 */ ADD_SV_TO_WO(3);			  \
			/* 0x0b7 */ RETURN_SUB;				  \
		SP_LABEL(sp_label_8); /* 4 bytes */			  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x0b8 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x0b9 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..7] = 4 bytes from the FW */	  \
			/* 0x0be */ LD_FW_TO_WR1(0, 32);		  \
			/* 0x0bf */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x0c0 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x0c5 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* 0x0c6 */ ADD_SV_TO_WO(4);			  \
			/* 0x0c7 */ RETURN_SUB;				  \
		SP_LABEL(sp_label_9); /* 5 bytes */			  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x0c8 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x0c9 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..7] = 5 bytes from the FW */	  \
			/* 0x0ce */ LD_FW_TO_WR1(0, 40);		  \
			/* 0x0cf */ SHL_WR1_BY_SV(8);			  \
			/* 0x0d0 */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x0d1 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x0d6 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* 0x0d7 */ ADD_SV_TO_WO(5);			  \
			/* 0x0d8 */ RETURN_SUB;				  \
		SP_LABEL(sp_label_10); /* 6 bytes */			  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x0d9 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x0da */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..7] = 6 bytes from the FW */	  \
			/* 0x0df */ LD_FW_TO_WR1(0, 48);		  \
			/* 0x0e0 */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x0e1 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x0e6 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* 0x0e7 */ ADD_SV_TO_WO(6);			  \
			/* 0x0e8 */ RETURN_SUB;				  \
		SP_LABEL(sp_label_11); /* 7 bytes */			  \
			/* WR0 = RA[54:55] = current Running Sum */	  \
			/* 0x0e9 */ LD_RA_TO_WR0(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* Invert the Running Sum */			  \
			/* 0x0ea */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* WR1 = FW[0..7] = 7 bytes from the FW */	  \
			/* 0x0ef */ LD_FW_TO_WR1(0, 56);		  \
			/* 0x0f0 */ SHL_WR1_BY_SV(8);			  \
			/* 0x0f1 */ CHECKSUM16_WR1_TO_WR0;		  \
			/* Invert the Running Sum */			  \
			/* 0x0f2 */ XOR_WR0_IMM_TO_WR0(0xFFFFFFFFFFFFFFFF,\
							sp_imm_64);	  \
			/* RA[54:55] = WR0 = current Running Sum */	  \
			/* 0x0f7 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_running_sum_offset, 2);  \
			/* 0x0f8 */ ADD_SV_TO_WO(7);			  \
			/* 0x0f9 */ RETURN_SUB;				  \
		SPARSER_END;						  \
	} while (0)
}
#endif

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
	sparser_gen_cmp_wr0_ne_wr1(sp_return_to_hxs_dst);
	/**********************************************************************
	 * Advance HB on the custom header start
	 **********************************************************************
	 * WR1 = NxtHdrOffset from the Parse Array. NxtHdrOffset was set by the
	 * Ethernet HXS.
	 *	offset : 50 bytes from Parse Array start
	 *	size   : 1 byte)
	 * WR1 = 14 */
	sparser_gen_ld_ra_to_wr1(sp_ra_pr_nxt_hdr_offset, 1);
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
	sparser_gen_sub32_wr0_imm_to_wr0(0x10, sp_imm_16);
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
	sparser_gen_st_wr1_to_ra(sp_ra_nxt_hdr, 2);
	/**********************************************************************
	 * Custom header offset is stored in ShimOffset_1 field of the Parse
	 * Array[32]
	 **********************************************************************
	 * WR1 =  NxtHdrOffset from the Parse Array.
	 *	offset : 50 bytes from Parse Array start
	 *	size   : 1 byte)
	 * WR1 = 14 */
	sparser_gen_ld_ra_to_wr1(sp_ra_pr_nxt_hdr_offset, 1);
	/* ShimOffset_1 = Parse Array[32] = 14 */
	sparser_gen_st_wr1_to_ra(sp_ra_pr_shim_offset_1, 1);
	/**********************************************************************
	 * Soft Parsers must set the NxtHdrOffst in the Parse Array.
	 * NxtHdrOffst = current NxtHdrOffst + custom header length.
	 **********************************************************************
	 * WR0 = custom header length = 46 */
	sparser_gen_ld_pa_to_wr0(2, 1);
	/* Store header length in GPRV0 */
	sparser_gen_st_wr0_to_ra(sp_ra_gprv_0, 2);

	/* WR1 = Ethernet NxtHdrOffset = 14
	 * WR0 = custom header length = 46
	 *
	 * WR1 = 46 + 14 = 60 */
	sparser_gen_add32_wr1_wr0_to_wr1();
	/* NxtHdrOffst = Parse Array[50] = 60 */
	sparser_gen_st_wr1_to_ra(sp_ra_pr_nxt_hdr_offset, 1);
	/**********************************************************************
	 * Custom header presence is marked in bit #0 of the Frame Attribute
	 * Flags extension.
	 *********************************************************************/
	sparser_gen_set_faf_bit(sp_faf_ud_soft_parser_0);
	/**********************************************************************
	 * Before calling the HXS processing the next header, a soft parser must
	 * prepare the Header Base advancement.	HB is advanced with the current
	 * value of WO.
	 *
	 * Set the WO to the length of the custom header.
	 *	WO = 46
	 *********************************************************************/
	/* Call computing running sum routine. The WO is updated by the
	 * called routine.
	 *
	 * RA[54:55] = Updated "Running Sum"
	 * */
	sparser_gen_jmp(GF | sp_compute_running_sum);
	/* HB = HB + WO = 14 + 46 = 60
	 * WO = 0
	 * WR0 = 0
	 * WR1 = 0
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
			/* Check for the custom EType */		  \
			/* 0x022 */ CMP_WR0_NE_WR1(sp_return_to_hxs_dst); \
			/* 0x024 */ LD_RA_TO_WR1(			  \
					sp_ra_pr_nxt_hdr_offset, 1);	  \
			/* 0x025 */ LD_WR1_TO_WO;			  \
			/* Advance to the custom header */		  \
			/* 0x026 */ ADVANCE_HB_BY_WO;			  \
			/* WO on the last 16 bytes in the custom header */\
			/* 0x027 */ LD_PA_TO_WR0(2, 1);			  \
			/* 0x028 */ SUB32_WR0_IMM_TO_WR0(0x10, sp_imm_16);\
			/* 0x02a */ LD_WR0_TO_WO;			  \
			/* Get EType from the custom header */		  \
			/* 0x02b */ LD_FW_TO_WR1(112, 16);		  \
			/* Set "NxtHdr" field in Parse Array */		  \
			/* 0x02c */ ST_WR1_TO_RA(sp_ra_nxt_hdr, 2);	  \
			/* 0x02d */ LD_RA_TO_WR1(			  \
					sp_ra_pr_nxt_hdr_offset, 1);	  \
			/* Set "ShimOffset_1" field in Parse Array */	  \
			/* 0x02e */ ST_WR1_TO_RA(			  \
					sp_ra_pr_shim_offset_1, 1);	  \
			/* 0x02f */ LD_PA_TO_WR0(2, 1);			  \
			/* Store header length in GPRV0 */		  \
			/* 0x030 */ ST_WR0_TO_RA(sp_ra_gprv_0, 2);	  \
			/* 0x031 */ ADD32_WR1_WR0_TO_WR1;		  \
			/* Set "NxtHdrOffset" field in Parse Array */	  \
			/* 0x032 */ ST_WR1_TO_RA(			  \
					sp_ra_pr_nxt_hdr_offset, 1);	  \
			/* Set user defined FAF bit #0 in Parse Array */  \
			/* 0x033 */ SET_FAF_BIT(sp_faf_ud_soft_parser_0); \
			/* Call computing running sum routine. The WO is  \
			 * updated by the called routine. 		  \
			 * RA[54:55] = Updated "Running Sum" */		  \
			/* 0x034 */ JMP(GF | sp_compute_running_sum);	  \
			/* 0x036 */ JMP_TO_L2_PROTOCOL;			  \
			/* NOP to make the SP size a multiple of 4 */	  \
			/* 0x38 */ NOP;					  \
		SPARSER_END;						  \
	} while (0)

	SP_EXAMPLE;
}
#endif	/* ASM_LOOK_LIKE */

/******************************************************************************/
static int soft_parser_run_on_simulator(void)
{
	int				err;
	struct sp_parse_array		ra;
	uint8_t				pa[3];
	uint16_t			rs;

	/* 1. Initializes the built-in simulator (Mandatory). Use soft parser
	 * already loaded into the AIOP Parser instructions memory */
	err = sparser_sim_init();
	if (err)
		return err;
	/* 2. Dump AIOP SIM Parser instructions memory */
	/*sparser_sim_memory_dump(PARSER_MIN_PC, PARSER_MAX_PC + 3);*/
	/* 3. Sets the packet to be parsed */
	err = sparser_sim_set_parsed_pkt(&parsed_packet[0],
					 sizeof(parsed_packet));
	if (err)
		return err;
	/* 4. Initializes the Parse Array with the default values */
	err = sparser_sim_init_parse_array(&ra);
	if (err)
		return err;
	/* 5. Set needed fields in the Parse Array. This step is needed to
	 * change the default values. */
	/* Set relevant fields in the Parse Array as after ETH HXS execution */
	ra.pr.nxt_hdr = 0xee00;		/* NxtHeader */
	ra.pr.last_etype_offset = 12;	/* Last EType Offset */
	ra.pr.nxt_hdr_offset = 14;	/* Next Header Offset */
	/* Gross Running Sum on the packet */
	ra.pr.gross_running_sum = compute_running_sum(&parsed_packet[0],
						      sizeof(parsed_packet));
	/* Running Sum after Ethernet header is parsed */
	rs = compute_running_sum(&parsed_packet[0], 14);
	ra.pr.running_sum = running_sum_diff(ra.pr.gross_running_sum, rs);
	err = sparser_sim_set_parse_array(&ra);
	if (err)
		return err;
	/* 6. Sets the Parameter Array. Parameter Array must contain the values
	 * expected by the soft parser. If the soft parser expects no
	 * parameters, this step is not needed. */
	pa[0] = 0xEE;	/* Expected EType = 0xEE00 */
	pa[1] = 0x00;
	pa[2] = 46;	/* Custom Header Length in bytes */
	err = sparser_sim_set_parameter_array(&pa[0], 0, 3);
	if (err)
		return err;
	/* 7. Sets the Header Base. If the Header Base of the custom header to
	 * be parsed is computed by the soft parser based on the provided
	 * input (Parse Array or Parameter Array), this step is not needed. */
	err = sparser_sim_set_header_base(0x00);
	if (err)
		return err;
	/* 8. Sets the PC limit count. Default is 0, i.e. limit count check is
	 * disabled. */
	err = sparser_sim_set_pc_limit(SP_SIM_MAX_CYCLE_LIMIT);
	if (err)
		return err;
	/* 9. Call the built-in simulator */
	err = sparser_sim(0x20, &sparser_ex[0], sizeof(sparser_ex));
	if (err)
		return err;
	/* 10. Dump Parse Results */
	sparser_sim_parse_result_dump();
	/* 11. Dump Frame Attribute Flags */
	sparser_sim_frame_attributes_dump();
	/* 12. Dump Parse Status/Error */
	sparser_sim_parse_error_print();
	return 0;
}

/******************************************************************************/
static int soft_parser_develop_debug(void)
{
	int				err;
	uint8_t				prpid = 0;
	uint8_t				pa[3];
	struct sparser_info		sp_info;
	struct dpni_drv_sparser_param	sp_param;

	/**********************************/
	/* Soft Parser byte-code generate */
	/**********************************/
	soft_parser_example_gen();
	/* Soft Parser byte-code dump. Dumped bytes may be imported in the
	 * final AIOP application. Following line may be commented. */
	sparser_bytecode_dump();
	/* Soft Parser disassembler. Following lines may be commented. */
	err = sparser_disa(0x20, &sparser_ex[0], sizeof(sparser_ex));
	if (err)
		return err;
	/* Soft Parser built-in simulator. Following line may be commented. */
	err = soft_parser_run_on_simulator();
	if (err)
		return err;
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
