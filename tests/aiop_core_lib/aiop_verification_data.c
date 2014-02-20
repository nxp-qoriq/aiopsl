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

__VERIF_GLOBAL uint64_t verif_ipr_instance_handle;
__VERIF_GLOBAL uint8_t verif_prpid;

__VERIF_TLS ipf_ctx_t ipf_context_addr1 
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS tcp_gso_ctx_t tcp_gso_context_addr1 
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS int32_t status;
__VERIF_TLS int32_t status_gro;
__VERIF_TLS int32_t status_ipf;


extern __TASK struct aiop_default_task_params default_task_params;

void init_verif()
{
	aiop_verif_init_parser();
	default_task_params.parser_starting_hxs = 0;
	default_task_params.parser_profile_id = verif_prpid;
	parse_result_generate_default(0);
	
	status = 0;
	status_gro = 0;
	tcp_gso_context_addr1[0] = 0;
	ipf_context_addr1[0] = 0;
}
