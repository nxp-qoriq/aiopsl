/**************************************************************************//**
@File		gso.c

@Description	This file contains the AIOP SW TCP GSO API implementation

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "fsl_gso.h"
#include "gso.h"



void gso_context_init(
		uint32_t flags,
		uint16_t mss,
		gso_ctx_t gso_ctx_addr)
{
	struct gso_context *gso_ctx = (struct gso_context *)&gso_ctx_addr;
	gso_ctx->first_seg = 1;
	gso_ctx->flags = flags;
	gso_ctx->mss = mss;
	gso_ctx->urgent_pointer = 0;
}
