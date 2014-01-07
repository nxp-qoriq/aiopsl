/**************************************************************************//**
@File		gro.c

@Description	This file contains the AIOP SW TCP GRO API implementation

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "gro.h"
#include "general.h"
#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_cdma.h"
#include "fdma.h"


	/* Shared memory global GRO parameters. */
struct gro_global_parameters gro_global_params;


void gro_init(uint32_t timeout_flags)
{
	gro_global_params.timeout_flags = timeout_flags;
}


/*int32_t tcp_gro_aggregate_seg(
		uint64_t tcp_gro_context_addr,
		struct tcp_gro_context_params *params,
		uint32_t flags)
{
	struct tcp_gro_context gro_ctx;
	cdma(&)
	return 0;
}

*/

int32_t tcp_gro_flush_aggregation(
		uint64_t tcp_gro_context_addr)
{
	struct tcp_gro_context gro_ctx;
	/* read GRO context*/
	cdma_read_with_mutex(tcp_gro_context_addr, CDMA_PREDMA_MUTEX_WRITE_LOCK,
			(void *)&gro_ctx, sizeof(struct tcp_gro_context));
	/* no aggregation */
	if (gro_ctx.metadata.seg_num == 0)
		return TCP_GRO_FLUSH_NO_AGG;
	/* write metadata to external memory */
	cdma_write_with_mutex((gro_ctx.params.metadata + METADATA_MEMBER1_SIZE), 
			0, &(gro_ctx.metadata.seg_num), 
			METADATA_MEMBER2_SIZE + METADATA_MEMBER3_SIZE);
	/* Copy aggregated FD to default FD location */
	*((struct ldpaa_fd *)HWC_FD_ADDRESS) = gro_ctx.agg_fd;
	PRC_SET_SEGMENT_ADDRESS((uint32_t)TLS_SECTION_END_ADDR + 
			SEGMENT_HEADOOM_SIZE);
	PRC_SET_SEGMENT_OFFSET(0);
	//PRC_SET_SR_BIT(0);
	//PRC_SET_ASA_SIZE(0);
	//PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
	
	
	
	
}
