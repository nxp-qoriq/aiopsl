/**************************************************************************//**
@File		system.c

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "system.h"
#include "id_pool.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"
extern void tman_timer_callback(void);
#else
#include "slab.h"
#include "kernel/platform.h"

#include "dbg.h"
#include "io.h"
#include "aiop_common.h"

extern void tman_timer_callback(void);

#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,            \
	                                   0,                          \
	                                   E_MAPPED_MEM_TYPE_GEN_REGS) \
	                                   + SOC_PERIPH_OFF_AIOP_WRKS);
#endif /* AIOP_VERIF */

/* Global parameters*/
uint64_t ext_prpid_pool_address;
uint64_t ext_keyid_pool_address;

int32_t sys_prpid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;
	int num_filled_buffs;


	status = slab_find_and_fill_bpid(1, (SYS_NUM_OF_PRPIDS+2), 2,
			MEM_PART_1ST_DDR_NON_CACHEABLE,
			&num_filled_buffs,&buffer_pool_id);
	if (status < 0)
		return status;


	status = id_pool_init(SYS_NUM_OF_PRPIDS, buffer_pool_id,
					&ext_prpid_pool_address);

		return status;
}


int32_t sys_keyid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;
	int num_filled_buffs;


	status = slab_find_and_fill_bpid(1, (SYS_NUM_OF_KEYIDS+2), 2,
			MEM_PART_1ST_DDR_NON_CACHEABLE,
			&num_filled_buffs,&buffer_pool_id);
	if (status < 0)
		return status;


	status = id_pool_init(SYS_NUM_OF_KEYIDS, buffer_pool_id,
					&ext_keyid_pool_address);
	return status;
}

int32_t aiop_sl_init(void)
{
	int32_t status = 0;

/* TODO - remove the AIOP_VERIF section when verification env will include 
 * the ARENA code */
#ifdef AIOP_VERIF
	/* TMAN EPID Init */
	__stwbr(1, 0, (void *)(SOC_PERIPH_OFF_AIOP_WRKS + 0xF8)); /* EPID = 1 */
	__stwbr((unsigned int)&tman_timer_callback,
			0,
			(void *)(SOC_PERIPH_OFF_AIOP_WRKS + 0x100)); /* EP_PC */
	__stwbr(0x00600040,
			0,
		(void *)(SOC_PERIPH_OFF_AIOP_WRKS + 0x108)); /* EP_FDPA */
	
	/* End of TMAN EPID Init */
#else
	/* TMAN EPID Init */
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;

	iowrite32(1, &wrks_addr->epas); /* EPID = 1 */
	iowrite32(PTR_TO_UINT(tman_timer_callback), &wrks_addr->ep_pc);
	
	pr_info("TMAN is setting EPID = 1\n");
	pr_info("ep_pc = 0x%x \n", ioread32(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x \n", ioread32(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x \n", ioread32(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x \n", ioread32(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x \n", ioread32(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x \n", ioread32(&wrks_addr->ep_spo));
	/* End of TMAN EPID Init */
#endif
	
	status = sys_prpid_pool_create();
	if (status)
		return status; /* TODO */

	status = sys_keyid_pool_create();

	return status; /* TODO */	
}

void aiop_sl_free(void)
{
/*	int32_t status = 0;*/

	cdma_release_context_memory(ext_prpid_pool_address);
	cdma_release_context_memory(ext_keyid_pool_address);
	
	/* TODO status ? */
}


