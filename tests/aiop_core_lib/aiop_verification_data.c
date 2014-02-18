/**************************************************************************//**
@File		aiop_verification_data.c

@Description	This file contains the AIOP SW Verification Data
		needed for the test

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "common/types.h"
#include "aiop_verification_data.h"
#include "dplib/fsl_gso.h"
#include "dplib/fsl_ldpaa.h"

__VERIF_GLOBAL uint64_t verif_ipr_instance_handle;

__VERIF_TLS tcp_gso_ctx_t tcp_gso_context_addr1 
	__attribute__((aligned(sizeof(struct ldpaa_fd))));
__VERIF_TLS int32_t status;
__VERIF_TLS int32_t status_gro;


void init_verif_tls()
{
	status = 0;
	status_gro = 0;
	tcp_gso_context_addr1[0] = 0;
}
