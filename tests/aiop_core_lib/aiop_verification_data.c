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

/**************************************************************************//**
@File		aiop_verification_data.c

@Description	This file contains the AIOP SW Verification Data
		needed for the test

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
#include "kernel/fsl_spinlock.h"

__VERIF_GLOBAL uint64_t verif_ipr_instance_handle[16];
__VERIF_GLOBAL uint8_t verif_prpid_valid;
__VERIF_GLOBAL volatile uint8_t verif_only_1_task_complete;
__VERIF_GLOBAL uint8_t verif_spin_lock;
__VERIF_GLOBAL uint8_t verif_prpid;
__VERIF_GLOBAL uint8_t tmi_id;

__VERIF_TLS uint8_t  slab_parser_error;
__VERIF_TLS uint8_t  slab_keygen_error;
__VERIF_TLS uint8_t  slab_general_error;
__VERIF_TLS uint64_t initial_ext_address; /* Initial External Data Address */

__VERIF_TLS ipf_ctx_t ipf_context_addr1
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS ipf_ctx_t ipf_context_addr2
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS tcp_gso_ctx_t tcp_gso_context_addr1
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS int32_t status_gro;
__VERIF_TLS int32_t status_gso;
__VERIF_TLS int32_t status_ipf1;
__VERIF_TLS int32_t status_ipf2;
__VERIF_TLS int32_t status_ipr;
__VERIF_TLS int32_t status_ipsec_encr;
__VERIF_TLS int32_t status_ipsec_decr;
__VERIF_TLS int32_t status_keygen;
__VERIF_TLS int32_t status_parser;
__VERIF_TLS int32_t status_cdma;


extern __TASK struct aiop_default_task_params default_task_params;
extern __TASK uint32_t seed_32bit;

void init_verif()
{
	struct parse_result *pr;

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	lock_spinlock(&verif_spin_lock);

	if (!verif_prpid_valid){
		verif_prpid_valid = 1;
		unlock_spinlock(&verif_spin_lock);
		aiop_sl_init();
		aiop_verif_init_parser();
		/* This is a temporary function and has to be used only until
				* the ARENA will initialize the profile sram */
/*		init_profile_sram();*/
		timeout_cb_verif(0);
		tmi_id = 0;
		verif_only_1_task_complete = 1;
		
		/* an initialization so we will not have the value 0 */
		seed_32bit = 5;
	}
	else {
		unlock_spinlock(&verif_spin_lock);
		if (!verif_only_1_task_complete) {
			do {
				__e_hwacceli_(YIELD_ACCEL_ID);
			} while (!verif_only_1_task_complete);
		}
	}

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);

	osm_task_init();
	default_task_params.parser_starting_hxs = 0;
	default_task_params.parser_profile_id = verif_prpid;
	parse_result_generate_default(0);

	status_gro = 0;
	status_gso = 0;
	status_ipf1 = 0;
	status_ipf2 = 0;
	status_ipr = 0;
	status_ipsec_encr = 0;
	status_ipsec_decr = 0;
	tcp_gso_context_addr1[0] = 0;
	ipf_context_addr1[0] = 0;
	status_keygen = 0;
	status_parser = 0;
	status_cdma = 0;
}

//__VERIF_PROFILE_SRAM struct  profile_sram profile_sram1;

/*
void init_profile_sram()
{
	 This is a temporary function and has to be used only until
			* the ARENA will initialize the profile sram

	 initialize profile sram

		profile_sram1.ip_secific_sp_info = 0;
		profile_sram1.dl = 0;
		profile_sram1.reserved = 0;
		 0x0080 --> 0x8000 (little endian)
		profile_sram1.dhr = 0x8000;
		profile_sram1.dhr = 0x0080;
		profile_sram1.mode_bits1 = (mode_bits1_PTAR | mode_bits1_SGHR |
				mode_bits1_ASAR);
		profile_sram1.mode_bits2 = (mode_bits2_BS | mode_bits2_FF |
				mode_bits2_VA | mode_bits2_DLC);
		 buffer size is 2048 bytes, so PBS should be 32 (0x20).
		 * 0x0801 --> 0x0108 (little endian)
		profile_sram1.pbs1 = 0x0108;
		profile_sram1.pbs1 = 0x0801;
		 BPID=0
		profile_sram1.bpid1 = 0x0000;
		 buffer size is 2048 bytes, so PBS should be 32 (0x20).
		* 0x0801 --> 0x0108 (little endian)
		profile_sram1.pbs2 = 0x0108;
		profile_sram1.pbs2 = 0x0801;
		 BPID=1, 0x0001 --> 0x0100 (little endian)
		profile_sram1.bpid2 = 0x0100;
		profile_sram1.bpid2 = 0x0001;
		profile_sram1.pbs3 = 0x0000;
		profile_sram1.bpid3 = 0x0000;
		profile_sram1.pbs4 = 0x0000;
		profile_sram1.bpid4 = 0x0000;
}
*/
