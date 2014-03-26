/**************************************************************************//**
@File		system.c

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "system.h"
#include "id_pool.h"

/* Global parameters*/
uint64_t ext_prpid_pool_address;
uint64_t ext_keyid_pool_address;

int32_t sys_ctlu_prpid_pool_create(void)
{
/*	uint16_t pool[SYS_PRPID_POOL_LENGTH];*/
	int32_t status;

	/* TODO Replace these temporal assignments with buffer_pool_id and
	 * buffer_size provided by ARENA function*/
	uint16_t buffer_pool_id = 1;
/*	uint32_t buffer_size = 256;*/

	status = id_pool_init(SYS_NUM_OF_PRPIDS, buffer_pool_id,
					&ext_prpid_pool_address);
	return status;
}


int32_t sys_ctlu_keyid_pool_create(void)
{
/*	uint16_t pool[SYS_KEYID_POOL_LENGTH];*/
	int32_t status;

	/* TODO Replace these temporal assignments with buffer_pool_id and
	 * buffer_size provided by ARENA function*/
	uint16_t buffer_pool_id = 2;
/*	uint32_t buffer_size = 1024;*/

	status = id_pool_init(SYS_NUM_OF_KEYIDS, buffer_pool_id,
					&ext_keyid_pool_address);
	return status;
}
