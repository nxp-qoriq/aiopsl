/**************************************************************************//**
@File		gso.c

@Description	This file contains the AIOP SW TCP GSO API implementation

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_fdma.h"

#include "gso.h"


int32_t tcp_gso_generate_seg(
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	return 0; /* Todo - return valid status*/
}

int32_t tcp_gso_discard_frame_remainder(
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	return fdma_discard_frame(
			gso_ctx->rem_frame_handle, FDMA_DIS_NO_FLAGS);
}

void tcp_gso_context_init(
		uint32_t flags,
		uint16_t mss,
		tcp_gso_ctx_t tcp_gso_context_addr)
{
	struct tcp_gso_context *gso_ctx =
			(struct tcp_gso_context *)tcp_gso_context_addr;
	gso_ctx->first_seg = 1;
	gso_ctx->flags = flags;
	gso_ctx->split_size = mss;
	gso_ctx->urgent_pointer = 0;
}
