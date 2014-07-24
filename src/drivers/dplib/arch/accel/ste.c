/**************************************************************************//**
@File		ste.c

@Description	This file contains the AIOP SW Statistics API implementation.

*//***************************************************************************/

#include "general.h"
#include "ste.h"
#include "dplib/fsl_ste.h"

void ste_set_32bit_counter(uint64_t counter_addr, uint32_t value)
{
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_SET + STE_MODE_32_BIT_CNTR_SIZE;

	__stqw(cmd_type, (uint32_t)(value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);
}

void ste_set_64bit_counter(uint64_t counter_addr, uint64_t value)
{
	uint32_t mem_ptr;

	mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR + STE_CTR_8B_SET_CMD_MEM_ADDR;

	__stqw((uint32_t)(value>>32), (uint32_t)(value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);
}

void ste_inc_counter(uint64_t counter_addr,
				      uint32_t inc_value, uint32_t flags)
{
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_ADD + flags;

	__stqw(cmd_type, (uint32_t)(inc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

void ste_dec_counter(uint64_t counter_addr,
				      uint32_t dec_value, uint32_t flags)
{
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_SUB + flags;

	__stqw(cmd_type, (uint32_t)(dec_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

void ste_inc_and_acc_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_INC_ADD + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

void ste_inc_and_sub_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_INC_SUB + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

void ste_dec_and_acc_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_DEC_ADD + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

void ste_dec_and_sub_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_DEC_SUB + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}


void ste_barrier()
{
	uint32_t cmd_type = (uint32_t)STE_CMDTYPE_SYNC;
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;

	/* A command to the TST to set the scheduling inhibit for the task */
	__e_osmcmd(STE_OSM_REQ_TYPE, 0);
	/* call STE sync command. */
	__stqw(cmd_type, 0, 0, 0, 0, (uint32_t *)mem_ptr);
	/* YIELD. */
	__e_hwacceli(YIELD_ACCEL_ID);
}

