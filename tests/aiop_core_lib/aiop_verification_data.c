/**************************************************************************//**
@File		aiop_verification_data.c

@Description	This file contains the AIOP SW Verification Data
		needed for the test

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "common/types.h"
#include "aiop_verification_data.h"
#include "aiop_verification.h"
#include "dplib/fsl_gso.h"
#include "dplib/fsl_ipf.h"
#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_parser.h"
#include "system.h"
#include "osm.h"

__VERIF_GLOBAL uint64_t verif_ipr_instance_handle;
__VERIF_GLOBAL uint8_t verif_prpid_valid;
__VERIF_GLOBAL uint8_t verif_prpid;
__VERIF_GLOBAL uint8_t tmi_id;

__VERIF_TLS ipf_ctx_t ipf_context_addr1
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS tcp_gso_ctx_t tcp_gso_context_addr1
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS int32_t status;
__VERIF_TLS int32_t status_gro;
__VERIF_TLS int32_t status_gso;
__VERIF_TLS int32_t status_ipf;
__VERIF_TLS int32_t status_ipr;

extern __TASK struct aiop_default_task_params default_task_params;

void init_verif()
{
	struct parse_result *pr;

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	if (!verif_prpid_valid){
		aiop_sl_init();
		aiop_verif_init_parser();
		/* This is a temporary function and has to be used only until
				* the ARENA will initialize the profile sram */
		init_profile_sram();
		verif_prpid_valid = 1;
		gro_timeout_cb_verif(0);
	}

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM);

	osm_task_init();
	default_task_params.parser_starting_hxs = 0;
	default_task_params.parser_profile_id = verif_prpid;
	parse_result_generate_default(0);

	status = 0;
	status_gro = 0;
	status_gso = 0;
	status_ipf = 0;
	status_ipr = 0;
	tcp_gso_context_addr1[0] = 0;
	ipf_context_addr1[0] = 0;
	tmi_id = 0;
}

__VERIF_PROFILE_SRAM struct  profile_sram profile_sram1;

void init_profile_sram()
{
	/* This is a temporary function and has to be used only until
			* the ARENA will initialize the profile sram */

	/* initialize profile sram */

		profile_sram1.ip_secific_sp_info = 0;
		profile_sram1.dl = 0;
		profile_sram1.reserved = 0;
		/* 0x0080 --> 0x8000 (little endian) */
		profile_sram1.dhr = 0x8000;
		/*profile_sram1.dhr = 0x0080; */
		profile_sram1.mode_bits1 = (mode_bits1_PTAR | mode_bits1_SGHR |
				mode_bits1_ASAR);
		profile_sram1.mode_bits2 = (mode_bits2_BS | mode_bits2_FF |
				mode_bits2_VA | mode_bits2_DLC);
		/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
		 * 0x0801 --> 0x0108 (little endian) */
		profile_sram1.pbs1 = 0x0108;
		/*profile_sram1.pbs1 = 0x0801;  */
		/* BPID=0 */
		profile_sram1.bpid1 = 0x0000;
		/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
		* 0x0801 --> 0x0108 (little endian) */
		profile_sram1.pbs2 = 0x0108;
		/*profile_sram1.pbs2 = 0x0801; */
		/* BPID=1, 0x0001 --> 0x0100 (little endian) */
		profile_sram1.bpid2 = 0x0100;
		/*profile_sram1.bpid2 = 0x0001; */
		profile_sram1.pbs3 = 0x0000;
		profile_sram1.bpid3 = 0x0000;
		profile_sram1.pbs4 = 0x0000;
		profile_sram1.bpid4 = 0x0000;
}
