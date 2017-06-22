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

#ifndef LS2085A_REV1
/* The test with the custom header being the first header in the packet works
 * only on Rev2 platforms. To run this test, on Rev2 platforms, define the
 * following macro as 1. */
	#define CUSTOM_HEADER_FIRST	0
#endif

#if (CUSTOM_HEADER_FIRST == 0)
/*******************************************************************************
 * Soft Parser Example :
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
 * SP code returns to the calling HXS (Ethernet). */
uint8_t sparser_ex[48] __attribute__((aligned(4)));

/*******************************************************************************
 * This is the soft parser to be loaded onto the WRIOP Parser and activated
 * for each DPNI belonging to the AIOP container.
 * The byte-code is identical with that generated for the AIOP parser. */
uint8_t wriop_sparser_ex[48] __attribute__((aligned(4))) = {
	0xb7, 0x9e, 0x10, 0x13, 0x00, 0x79, 0x07, 0xfe, 0x33, 0x21,
	0x00, 0x81, 0x00, 0x02, 0x10, 0x20, 0x00, 0x52, 0x00, 0x10,
	0x00, 0x80, 0xbf, 0x9f, 0x29, 0x23, 0x33, 0x21, 0x28, 0x41,
	0x10, 0x20, 0x29, 0x02, 0x00, 0x4c, 0x28, 0x65, 0x03, 0xe0,
	0x18, 0x00, 0x87, 0x3c, 0x00, 0x44, 0x00, 0x00
};

/*******************************************************************************
 * Packet to be parsed
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
#else
/*******************************************************************************
 * Soft Parser Example :
 * This SP parses a custom Ethernet header placed as the first header in the
 * packet. The custom header has 18 bytes in length : 4 custom bytes are placed
 * before the EType field. */
uint8_t sparser_ex[136] __attribute__((aligned(4)));

/*******************************************************************************
 * This is the soft parser to be loaded onto the WRIOP Parser and activated
 * for each DPNI belonging to the AIOP container.
 * The byte-code is identical with that generated for the AIOP parser. */
uint8_t wriop_sparser_ex[136] __attribute__((aligned(4))) = {
	0xb7, 0x9e, 0x02, 0x7f, 0x08, 0x00, 0x86, 0xdd, 0x81, 0x00,
	0x88, 0xa8, 0x00, 0x30, 0x40, 0x3c, 0x40, 0x3c, 0x40, 0x3c,
	0x40, 0x3c, 0x02, 0x7f, 0x88, 0x64, 0x08, 0x06, 0x88, 0x47,
	0x88, 0x48, 0x00, 0x30, 0x40, 0x32, 0x40, 0x32, 0x40, 0x32,
	0x40, 0x32, 0xbf, 0x9e, 0x02, 0x1f, 0xaa, 0xaa, 0x00, 0x78,
	0x40, 0x2a, 0x03, 0xe0, 0x03, 0x8a, 0x97, 0xde, 0x02, 0x5f,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x72, 0x00, 0x79,
	0x40, 0x05, 0x03, 0x8c, 0x18, 0x00, 0x40, 0x0d, 0x97, 0xde,
	0x02, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x79,
	0x40, 0x05, 0x03, 0x8d, 0x18, 0x00, 0x40, 0x03, 0x03, 0x8b,
	0x00, 0x04, 0x28, 0x46, 0x10, 0x00, 0x28, 0x64, 0x29, 0x02,
	0x00, 0x52, 0x00, 0x02, 0x28, 0x4e, 0x00, 0x52, 0x00, 0x0e,
	0x00, 0x80, 0xbf, 0x9f, 0x29, 0x23, 0x18, 0x00, 0x87, 0x3c,
	0x00, 0x44, 0x18, 0x00, 0x00, 0x00
};

/*******************************************************************************
 * Packet to be parsed : 4 bytes (0xaa, 0xbb, 0xcc, 0xdd) are placed before
 * the EType field of a Ethernet header. */
uint8_t parsed_packet[] = {
	/* Ethernet Header */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x10,
	0x94, 0x00, 0x00, 0x02, 0xaa, 0xbb, 0xcc, 0xdd,
	0x08, 0x00,
	/* IP Header */
	0x45, 0x00, 0x00, 0x3d, 0x00, 0x07, 0x00, 0x00,
	0xff, 0x11, 0x3a, 0x50, 0xc0, 0x55, 0x01, 0x02,
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
#endif

/******************************************************************************/
/* Data segment presentation parameters */
#define EP_INIT_PRESENTATION_OPT_SPS		0x0080
#define PRESENTATION_LENGTH			256

/* WRIOP Parse result ASA offset */
#define WRIOP_ASA_PR_OFFSET			0x10

#ifdef SP_RUNNING_SUM
/* Running Sum computation AIOP SL library routine */
uint8_t sparser_running_sum[388] __attribute__((aligned(4)));
#endif

/******************************************************************************/
static void sp_print_frame(void)
{
	uint16_t	seg_len;
	uint32_t	frame_len;
	uint8_t		*pb;
	int		i;

	seg_len = PRC_GET_SEGMENT_LENGTH();
	frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	fsl_print("FD[len] = %d, Seg Len = %d\n", frame_len, seg_len);
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
	for (sum = 0, i = 0; i < len / 2; i++)
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
static void print_wriop_parse_result(void)
{
	int		err;
	uint8_t		fd_asal;
	uint16_t	asa_length = 0;
	uint64_t	val64, *pval64;
	uint8_t		asa_bytes[64], *asa_pres;

	fd_asal = LDPAA_FD_GET_ASAL(HWC_FD_ADDRESS);
	if (!fd_asal) {
		pr_warn("Packet doesn't contain ASA information\n");
		return;
	}
	/* Bring ASA information into workspace */
	asa_pres = &asa_bytes[0];
#ifndef LS2085A_REV1
	err = fdma_read_default_frame_asa(asa_pres, 0, 1, &asa_length);
#else
	err = fdma_read_default_frame_asa(asa_pres, 0, 1);
#endif
	if (err) {
		pr_warn("Can't get ASA information\n");
		return;
	}
	fsl_print("<<< WRIOP Parse Result >>>\n");
	/* Convert WRIOP Parse result into BE byte order */
	pval64 = (uint64_t *)(asa_pres + WRIOP_ASA_PR_OFFSET);
	val64 = LLLDW_SWAP(0, pval64);
	*pval64++ = val64;
	val64 = LLLDW_SWAP(0, pval64);
	*pval64++ = val64;
	val64 = LLLDW_SWAP(0, pval64);
	*pval64++ = val64;
	val64 = LLLDW_SWAP(0, pval64);
	*pval64++ = val64;
	val64 = LLLDW_SWAP(0, pval64);
	*pval64++ = val64;
	val64 = LLLDW_SWAP(0, pval64);
	*pval64 = val64;
	sparser_parse_result_dump((struct sp_parse_result *)
				  (asa_pres + WRIOP_ASA_PR_OFFSET));
	sparser_frame_attributes_dump((struct sp_parse_result *)
				      (asa_pres + WRIOP_ASA_PR_OFFSET));
	sparser_parse_error_print((struct sp_parse_result *)
				  (asa_pres + WRIOP_ASA_PR_OFFSET));
}

/******************************************************************************/
static __HOT_CODE ENTRY_POINT void app_process_packet(void)
{
	int			err;
	struct parse_result	*pr;

#if (CUSTOM_HEADER_FIRST == 0)
	sl_prolog();
#else
	sl_prolog_with_custom_header(0x20);
#endif
	fsl_print("\n\nSP Demo: Core %d Received Frame\n", core_get_id());
	sp_print_frame();
	/* Show AIOP Parse result */
	fsl_print("<<< AIOP Parse Result >>>\n");
	sparser_parse_result_dump((struct sp_parse_result *)
				  HWC_PARSE_RES_ADDRESS);
	sparser_frame_attributes_dump((struct sp_parse_result *)
				      HWC_PARSE_RES_ADDRESS);
	sparser_parse_error_print((struct sp_parse_result *)
				  HWC_PARSE_RES_ADDRESS);
	/* Show WRIOP Parse result */
	print_wriop_parse_result();
	/* Validate the checksum on the received packet */
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	err = parse_result_generate_default(PARSER_VALIDATE_L4_CHECKSUM);
	if (err)
		pr_err("[%d] : Validate L4 checksum\n", err);
#if (CUSTOM_HEADER_FIRST == 0)
	if (PARSER_IS_UD_SOFT_PARSER_BIT_0_SET()) {
		uint8_t		*custom_hdr, custom_off, custom_len;

		/* 1. Replace EType in Ethernet header with EType in the custom
		 * header */
		custom_hdr = (uint8_t *)PARSER_GET_SHIM1_POINTER_DEFAULT();
		custom_off = PARSER_GET_SHIM1_OFFSET_DEFAULT();
		custom_len = PARSER_GET_OUTER_IP_OFFSET_DEFAULT() - custom_off;
		*((uint16_t *)(custom_hdr - 2)) =
				*((uint16_t *)(custom_hdr + custom_len - 2));
		fdma_modify_default_segment_data((uint16_t)(custom_off - 2), 2);
		/* 2. Remove custom header and represent the packet */
		err = fdma_delete_default_segment_data
			(custom_off, custom_len, FDMA_REPLACE_SA_REPRESENT_BIT);
		if (err && err != FDMA_STATUS_UNABLE_PRES_DATA_SEG) {
			pr_err("[%d] : Remove custom header\n", err);
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			fdma_terminate_task();
		}
		/* After header manipulation, the custom part is removed.
		 * Switch to the "egress" parsing profile. */
		task_switch_to_egress_parse_profile(PARSER_ETH_STARTING_HXS);
		pr->gross_running_sum = 0;
		/* Validate L4 checksum on the transmitted packet. When packet
		 * is updated, in order to validate the checksum one must :
		 *	- represent the updated packet in the workspace,
		 *	- invalidate the gross running sum */
		err = parse_result_generate_default
					(PARSER_VALIDATE_L4_CHECKSUM);
		if (err)
			pr_err("[%d] : Validate L4 checksum\n", err);
	}
#else
	if (PARSER_IS_UD_SOFT_PARSER_BIT_0_SET()) {
		uint8_t		custom_off;

		/* Remove custom header and represent the packet */
		custom_off = PARSER_GET_ETH_OFFSET_DEFAULT();
		err = fdma_delete_default_segment_data
			((uint16_t)(custom_off + 12), 4,
			 FDMA_REPLACE_SA_REPRESENT_BIT);
		if (err && err != FDMA_STATUS_UNABLE_PRES_DATA_SEG) {
			pr_err("[%d] : Remove custom header\n", err);
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			fdma_terminate_task();
		}
		/* After header manipulation, the custom part is removed.
		 * Switch to the "egress" parsing profile. */
		task_switch_to_egress_parse_profile(PARSER_ETH_STARTING_HXS);
		pr->gross_running_sum = 0;
		/* Validate L4 checksum on the transmitted packet. When packet
		 * is updated, in order to validate the checksum one must :
		 *	- represent the updated packet in the workspace,
		 *	- invalidate the gross running sum */
		err = parse_result_generate_default
					(PARSER_VALIDATE_L4_CHECKSUM);
		if (err)
			pr_err("[%d] : Validate L4 checksum\n", err);
	}
#endif
	fsl_print("\n\nSP Demo: Core %d Transmitted Frame\n", core_get_id());
	sp_print_frame();
	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
	if (err) {
		pr_err("Send packet\n");
		if (err == -ENOMEM)
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		else /* (err == -EBUSY) */
			IPSEC_FDMA_DISCARD_FD();
	}
	fdma_terminate_task();
}

/******************************************************************************/
static int wriop_soft_parser_load(void)
{
	int				err;
	struct dpni_drv_sparser_param	sp_param;

	sp_param.start_pc = 0x20;
	sp_param.byte_code = &wriop_sparser_ex[0];
	sp_param.size = sizeof(wriop_sparser_ex);
	err = dpni_drv_load_wriop_ingress_soft_parser(&sp_param);
	if (err) {
		pr_err("WRIOP Soft Parser loading failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
static int wriop_soft_parser_enable(uint16_t ni_id)
{
	int				err;
	uint8_t				pa[3];
	struct dpni_drv_sparser_param	sp_param;

#if (CUSTOM_HEADER_FIRST == 0)
	pa[0] = 0xEE;	/* Expected EType = 0xEE00 */
	pa[1] = 0x00;
	pa[2] = 46;	/* Custom Header Length in bytes */
	sp_param.custom_header_first = 0;
	sp_param.param_offset = 0;
	sp_param.param_size = 3;
	sp_param.link_to_hard_hxs = PARSER_ETH_STARTING_HXS;
#else
	pa[0] = 18;	/* Custom Header Length in bytes */
	/* First header may be set only on Rev2 platforms */
	sp_param.custom_header_first = 1;
	sp_param.param_offset = 0;
	sp_param.param_size = 1;
#endif
	sp_param.start_pc = 0x20;
	sp_param.param_array = (uint8_t *)&pa[0];
	err = dpni_drv_enable_wriop_ingress_soft_parser(ni_id, &sp_param);
	if (err) {
		pr_err("WRIOP Soft Parser activation failed\n");
		return err;
	}
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
	/* Configure data segment presentation */
	err = dpni_drv_get_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot get initial presentation for NI %d\n", ni);
		return err;
	}
	init_presentation.options = EP_INIT_PRESENTATION_OPT_SPS;
	init_presentation.sps = PRESENTATION_LENGTH;
	err = dpni_drv_set_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot set initial presentation on NI %d\n", ni);
		return err;
	}
	if (!ni) {
		/* Load SP onto WRIOP Ingress Parser */
		err = wriop_soft_parser_load();
		if (err)
			return err;
	}
	/* Enable WRIOP SP on the ingress flow of every interface */
	err = wriop_soft_parser_enable(ni);
	if (err)
		return err;
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
#if (CUSTOM_HEADER_FIRST == 0)
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
#else
#define SP_EXAMPLE							  \
	do {								  \
		SPARSER_BEGIN(0x20, &sparser_ex[0], sizeof(sparser_ex));  \
			/* 1. Check for a known Ethernet type */	  \
			/* WR0 = EType at offset 12 */			  \
			/* 0x020 */ LD_FW_TO_WR0(96, 16);		  \
			/* 1.1 Check if IPv4 (0800), IPv6(86dd),	  \
			 * VLAN(8100, 88a8) */				  \
			/* 0x021 */ LD_IMM_BITS_TO_WR1(64,		  \
					0x88a8810086dd0800, sp_imm_64);	  \
			/* 0x026 */ CASE4_DC_WR_to_WR(sp_label_16,	  \
					sp_label_16, sp_label_16,	  \
					sp_label_16);			  \
			/* 1.2 Check if PPPoE+PPP(8864), ARP(0806),	  \
			 * MPLS(8847, 8848) */				  \
			/* 0x02b */ LD_IMM_BITS_TO_WR1(64,		  \
					0x8848884708068864, sp_imm_64);	  \
			/* 0x030 */ CASE4_DC_WR_to_WR(sp_label_16,	  \
					sp_label_16, sp_label_16,	  \
					sp_label_16);			  \
			/* 1.3 Check if LLC+SNAP (0xAAAA at offset 14) */ \
			/* 0x035 */ LD_FW_TO_WR0(112, 16);		  \
			/* 0x036 */ LD_IMM_BITS_TO_WR1(16, 0xaaaa,	  \
							sp_imm_16);	  \
			/* 0x038 */ CMP_WR0_EQ_WR1(sp_label_16);	  \
			/***************************/			  \
			/* Not known Ethernet type */			  \
			/***************************/			  \
			/* 2. Set user defined FAF bit #0 */		  \
			/* 0x03a */ SET_FAF_BIT(sp_faf_ud_soft_parser_0); \
			/* 3. Set Ethernet MAC Present FAF bit */	  \
			/* 0x03b */ SET_FAF_BIT(sp_faf_eth_mac_present);  \
			/* 4. Check if MAC DA is Multicast */		  \
			/* WR0 = MAC DA */				  \
			/* 0x03c */ LD_FW_TO_WR0(0, 48);		  \
			/* 0x03d */ LD_IMM_BITS_TO_WR1(48, 0x010000000000,\
						sp_imm_48);		  \
			/* 0x041 */ AND_WR0_WR1_TO_WR0;			  \
			/* 0x042 */ CMP_WR0_NE_WR1(sp_label_1);		  \
			/* Set Ethernet Multicast FAF bit */		  \
			/* 0x044 */ SET_FAF_BIT(sp_faf_eth_multicast);	  \
			/* 4.1 Here one must check if the multicast	  \
			 * address is a :				  \
			 *	- BPDU (01:80:C2:00:00:00)		  \
			 *	- Slow (01:80:C2:00:00:00 to		  \
			 *			01:80:C2:00:00:00:FF) */  \
			/* 0x045 */ JMP(sp_label_3);			  \
			/* 5. Check if MAC DA is Broadcast */		  \
		SP_LABEL(sp_label_1);					  \
			/* 0x047 */ LD_FW_TO_WR0(0, 48);		  \
			/* 0x048 */ LD_IMM_BITS_TO_WR1(48, 0xffffffffffff,\
						sp_imm_48);		  \
			/* 0x04c */ CMP_WR0_NE_WR1(sp_label_2);		  \
			/* Set Ethernet Broadcast FAF bit */		  \
			/* 0x04e */ SET_FAF_BIT(sp_faf_eth_broadcast);	  \
			/* 0x04f */ JMP(sp_label_3);			  \
			/* 6. MAC DA is Unicast */			  \
		SP_LABEL(sp_label_2);					  \
			/* Set Ethernet Unicast FAF bit */		  \
			/* 0x051 */ SET_FAF_BIT(sp_faf_eth_unicast);	  \
		SP_LABEL(sp_label_3);					  \
			/* 7. Set EthOffset to 0 */			  \
			/* 0x052 */ CLR_WR0;				  \
			/* 0x053 */ ST_WR0_TO_RA(sp_ra_pr_eth_offset, 1); \
			/* 8. Set NxtHdrOffset to custom header length */ \
			/* Get header length from parameters. Must be	  \
			 * greater than 16 */				  \
			/* 0x054 */ LD_PA_TO_WR0(0, 1);			  \
			/* 0x055 */ ST_WR0_TO_RA(			  \
					sp_ra_pr_nxt_hdr_offset, 1);	  \
			/* 9. Store custom header length in GPRV0 (for	  \
			 * Running Sum computation */			  \
			/* 0x056 */ ST_WR0_TO_RA(sp_ra_gprv_0, 2);	  \
			/* 10. Set LastEtypeOffset to "custom header	  \
			 * length - 2" */				  \
			 /* 0x057 */ SUB32_WR0_IMM_TO_WR0(2, sp_imm_16);  \
			 /* 0x059 */ ST_WR0_TO_RA(			  \
					 sp_ra_pr_last_etype_offset, 1);  \
			/* 11. Set NxtHdr to custom header EType */	  \
			/* 0x05a */ SUB32_WR0_IMM_TO_WR0(14, sp_imm_16);  \
			/* WO is on last 16 bytes of the custom header */ \
			/* 0x05c */ LD_WR0_TO_WO;			  \
			/* Get EType from the custom header */		  \
			/* 0x05d */ LD_FW_TO_WR1(112, 16);		  \
			/* 0x05e */ ST_WR1_TO_RA(sp_ra_nxt_hdr, 2);	  \
			/* 12. Call computing running sum routine */	  \
			/* 0x05f */ JMP(GF | sp_compute_running_sum);	  \
			/* 13. Jump to protocol set in NxtHdr field */	  \
			/* 0x061 */ JMP_TO_L2_PROTOCOL;			  \
		SP_LABEL(sp_label_16);					  \
			/*************************/			  \
			/* Known Ethernet type */			  \
			/*************************/			  \
			/* 0x064 */ JMP(sp_eth_hxs_dst);		  \
		SPARSER_END;						  \
	} while (0)

	SP_EXAMPLE;
#endif
}

#endif	/* ASM_LOOK_LIKE */

/******************************************************************************/
static int soft_parser_run_on_simulator(uint16_t pc, uint8_t *byte_code,
					int sp_size)
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
#if (CUSTOM_HEADER_FIRST == 0)
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
#else
	/* 5. Set needed fields in the Parse Array. This step is needed to
	 * change the default values. */
	/* Gross Running Sum on the packet */
	UNUSED(rs);
	ra.pr.gross_running_sum = compute_running_sum(&parsed_packet[0],
						      sizeof(parsed_packet));
	err = sparser_sim_set_parse_array(&ra);
	if (err)
		return err;
	/* 6. Sets the Parameter Array. Parameter Array must contain the values
	 * expected by the soft parser. If the soft parser expects no
	 * parameters, this step is not needed. */
	pa[0] = 18;	/* Custom Header Length in bytes */
	err = sparser_sim_set_parameter_array(&pa[0], 0, 1);
	if (err)
		return err;
#endif
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
	err = sparser_sim(pc, byte_code, sp_size);
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
static int aiop_soft_parser_develop_debug(void)
{
	int				err;
	uint8_t				pa[3];
	struct sparser_info		sp_info;
	struct dpni_drv_sparser_param	sp_param;

	/* AIOP Soft Parser byte-code generate */
	soft_parser_example_gen();
	/* Soft Parser byte-code dump */
	sparser_bytecode_dump();
	/* Soft Parser disassembler */
	err = sparser_disa(0x20, &sparser_ex[0], sizeof(sparser_ex));
	if (err)
		return err;
	/* Soft Parser built-in simulator */
	err = soft_parser_run_on_simulator(0x20, &sparser_ex[0],
					   sizeof(sparser_ex));
	if (err)
		return err;
	/* AIOP Soft Parser load/activate */
	/* Dump parser registers and instructions memory */
	/*
	sparser_drv_regs_dump();
	sparser_drv_memory_dump(PARSER_MIN_PC, PARSER_MAX_PC + 3);
	*/
	/* Adjust the Parser Cycle Limit value */
	sparser_drv_set_pclim(PARSER_CYCLE_LIMIT_MAX);
	/* Load Soft Parser */
	sp_info.pc = PARSER_MIN_PC;
	sp_info.byte_code = &sparser_ex[0];
	sp_info.size = sizeof(sparser_ex);
	sp_info.param_off = 0;
	sp_info.param_size = 3;
	err = sparser_drv_load_ingress_parser(&sp_info);
	if (err) {
		fsl_print("Soft Parser loading failed\n");
		return err;
	}
	/* Dump parser registers and instructions memory */
	/*
	sparser_drv_regs_dump();
	sparser_drv_memory_dump(sp_info.pc, sp_info.pc + sp_info.size / 2);
	sparser_drv_memory_dump(PARSER_MIN_PC, PARSER_MAX_PC + 3);
	*/
	/* Enable soft parser */
#if (CUSTOM_HEADER_FIRST == 0)
	pa[0] = 0xEE;	/* Expected EType = 0xEE00 */
	pa[1] = 0x00;
	pa[2] = 46;	/* Custom Header Length in bytes */
	sp_param.param_offset = 0;
	sp_param.param_size = 3;
	sp_param.custom_header_first = 0;
	sp_param.link_to_hard_hxs = PARSER_ETH_STARTING_HXS;
#else
	pa[0] = 18;	/* Custom Header Length in bytes */
	sp_param.param_offset = 0;
	/* First header may be set only on Rev2 platforms */
	sp_param.param_size = 1;
	sp_param.custom_header_first = 1;
#endif
	sp_param.start_pc = 0x20;
	sp_param.param_array = (uint8_t *)&pa[0];
	err = dpni_drv_enable_ingress_soft_parser(&sp_param);
	if (err) {
		fsl_print("Soft Parser activation failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
int app_init(void)
{
	int	err;

	fsl_print("Running app_init()\n");
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t)app_process_packet,
			     app_dpni_event_added_cb);
	if (err) {
		pr_err("EVM registration for DPNI_EVENT_ADDED failed: %d\n",
		       err);
		return err;
	}
	err = aiop_soft_parser_develop_debug();
	if (err)
		return err;
	fsl_print("To start test inject packets ...\n");
	return 0;
}

/******************************************************************************/
int app_early_init(void)
{
	int		err;
	uint32_t	frame_anno;

	/* To validate the WRIOP parsing, configure the DPNIs to bring into
	 * AIOP, the WRIOP parse result annotation. WRIOP parse result is
	 * written into the ASA presentation area at offset 0x10.
	 */
	frame_anno = DPNI_DRV_FA_PARSER_RESULT;
	/* On LS2085 platforms, because of ERR009354 errata, the minimum value
	 * of the head-room must be 256. */
	err = dpni_drv_register_rx_buffer_layout_requirements(256, 0, 0,
							      frame_anno);
	return 0;
}

/******************************************************************************/
void app_free(void)
{
	/* TODO - complete ! */
}
