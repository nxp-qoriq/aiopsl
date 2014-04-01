/**************************************************************************//**
@File		system.c

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "system.h"
#include "id_pool.h"
#include "slab_stub.h" /* TODO remove when ARENA is included */
/* TODO include the followings when ARENA will be included
#include "slab.h"
#include "kernel/platform.h"
*/

/* Global parameters*/
uint64_t ext_prpid_pool_address;
uint64_t ext_keyid_pool_address;

int32_t sys_ctlu_prpid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;
	int num_filled_buffs;


	status = slab_find_and_fill_bpid(1, (SYS_NUM_OF_PRPIDS+1), 2, 
			MEM_PART_1ST_DDR_NON_CACHEABLE,
			&num_filled_buffs,&buffer_pool_id);
	if (status < 0)
		return status;


	status = id_pool_init(SYS_NUM_OF_PRPIDS, buffer_pool_id,
					&ext_prpid_pool_address);

		return status;
}


int32_t sys_ctlu_keyid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;
	int num_filled_buffs;


	status = slab_find_and_fill_bpid(1, (SYS_NUM_OF_KEYIDS+1), 2, 
			MEM_PART_1ST_DDR_NON_CACHEABLE,
			&num_filled_buffs,&buffer_pool_id);
	if (status < 0)
		return status;


	status = id_pool_init(SYS_NUM_OF_KEYIDS, buffer_pool_id,
					&ext_keyid_pool_address);
	return status;
}
