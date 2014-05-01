/**************************************************************************//**
@File		slab_stub.c

@Description	To Be Removed !!!

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "slab_stub.h"
#include "system.h"

int slab_find_and_fill_bpid(uint32_t num_buffs,
                            uint16_t buff_size,
                            uint16_t alignment,
                            uint8_t  mem_partition_id,
                            int      *num_filled_buffs,
                            uint16_t *bpid)
{
	/* Assuming fixed value for IPR is 1000 buffers. */
	UNUSED(alignment);
	UNUSED(mem_partition_id);

	if ((buff_size == (SYS_NUM_OF_PRPIDS+2)) || \
			(num_buffs == 1000))
		*bpid = 1;
	else
		*bpid = 2;

	*num_filled_buffs = (int)num_buffs;

	return 0;
}

