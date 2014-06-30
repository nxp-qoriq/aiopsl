/**************************************************************************//**
@File		system.c

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "system.h"
#include "id_pool.h"
#include "dplib/fsl_cdma.h"
#include "fsl_platform.h"

#include "dplib/fsl_ipsec.h"
#include "ipsec.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"

extern void tman_timer_callback(void);
/* The offset of the Work Scheduler registers */
#define AIOP_WRKS_REGISTERS_OFFSET				0x0209d000

#else
#include "slab.h"
#include "platform.h"

#include "fsl_dbg.h"
#include "fsl_io.h"
#include "aiop_common.h"

extern void tman_timer_callback(void);
extern int ipr_init(void);
extern int aiop_snic_init(void);

#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,                 \
						0,                          \
						E_MAPPED_MEM_TYPE_GEN_REGS) \
						+ SOC_PERIPH_OFF_AIOP_WRKS)

#endif /* AIOP_VERIF */

/* Global parameters*/
__SHRAM uint64_t ext_prpid_pool_address;
__SHRAM uint64_t ext_keyid_pool_address;

/* IPsec Instance global parameters */
extern __SHRAM struct ipsec_global_instance_params ipsec_global_instance_params;

/* Storage profiles array */
__PROFILE_SRAM struct  storage_profile storage_profiles[NUM_OF_SP];

int sys_prpid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;
	int num_filled_buffs;


	status = slab_find_and_fill_bpid(1, (SYS_NUM_OF_PRPIDS+2), 2,
			MEM_PART_DP_DDR,
			&num_filled_buffs, &buffer_pool_id);
	if (status < 0)
		handle_fatal_error((char *)status); /*TODO Fatal error*/

	id_pool_init(SYS_NUM_OF_PRPIDS, buffer_pool_id,
					&ext_prpid_pool_address);

	return 0;
}


int sys_keyid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;
	int num_filled_buffs;


	status = slab_find_and_fill_bpid(1, (SYS_NUM_OF_KEYIDS+2), 2,
			MEM_PART_DP_DDR,
			&num_filled_buffs, &buffer_pool_id);
	if (status < 0)
		handle_fatal_error((char *)status); /*TODO Fatal error*/

	id_pool_init(SYS_NUM_OF_KEYIDS, buffer_pool_id,
					&ext_keyid_pool_address);
	return 0;
}

int aiop_sl_init(void)
{
	int32_t status = 0;
	//extern struct ipsec_global_instance_params ipsec_global_instance_params;

#ifdef AIOP_VERIF
	/* TMAN EPID Init params*/
	uint32_t val;
	uint32_t *addr;
#endif
	
	/* Initialize IPsec instance global parameters */
	ipsec_global_instance_params.instance_count = 0;
	ipsec_global_instance_params.spinlock = 0;
	
	/* initialize profile sram */

#ifdef AIOP_VERIF
	/* Default Storage Profile */
	storage_profiles[SP_DEFAULT].ip_secific_sp_info = 0;
	storage_profiles[SP_DEFAULT].dl = 0;
	storage_profiles[SP_DEFAULT].reserved = 0;
	/* 0x0080 --> 0x8000 (little endian) */
	storage_profiles[SP_DEFAULT].dhr = 0x8000;
	/*storage_profiles[SP_DEFAULT].dhr = 0x0080; */
	storage_profiles[SP_DEFAULT].mode_bits1 = (mode_bits1_PTAR | mode_bits1_SGHR |
			mode_bits1_ASAR);
	storage_profiles[SP_DEFAULT].mode_bits2 = (mode_bits2_BS | mode_bits2_FF |
			mode_bits2_VA | mode_bits2_DLC);
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	 * 0x0801 --> 0x0108 (little endian) */
	storage_profiles[SP_DEFAULT].pbs1 = 0x0108;
	/* BPID=0 */
	storage_profiles[SP_DEFAULT].bpid1 = 0x0000;
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	* 0x0801 --> 0x0108 (little endian) */
	storage_profiles[SP_DEFAULT].pbs2 = 0x0108;
	/* BPID=0 */
	storage_profiles[SP_DEFAULT].bpid2 = 0x0000;
	storage_profiles[SP_DEFAULT].pbs3 = 0x0000;
	storage_profiles[SP_DEFAULT].bpid3 = 0x0000;
	storage_profiles[SP_DEFAULT].pbs4 = 0x0000;
	storage_profiles[SP_DEFAULT].bpid4 = 0x0000;
	
	/*****************************************************/
	/* IPsec Storage Profile */
	/*****************************************************/
	storage_profiles[SP_IPSEC].ip_secific_sp_info = 0;
	storage_profiles[SP_IPSEC].dl = 0;
	storage_profiles[SP_IPSEC].reserved = 0;
	/* 0x0080 --> 0x8000 (little endian) */
	storage_profiles[SP_IPSEC].dhr = 0x0000;
		
	/*storage_profiles[SP_IPSEC].dhr = 0x0080; */
	storage_profiles[SP_IPSEC].mode_bits1 = (mode_bits1_PTAR | 
			mode_bits1_SGHR ); /* No ASA */
	storage_profiles[SP_IPSEC].mode_bits2 = (mode_bits2_BS | mode_bits2_FF |
			mode_bits2_VA | mode_bits2_DLC);
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	 * 0x0801 --> 0x0108 (little endian) */
	storage_profiles[SP_IPSEC].pbs1 = 0x0108;
	/* BPID=0 */
	storage_profiles[SP_IPSEC].bpid1 = 0x0000;
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	* 0x0801 --> 0x0108 (little endian) */
	storage_profiles[SP_IPSEC].pbs2 = 0x0108;
	/* BPID=0 */
	storage_profiles[SP_IPSEC].bpid2 = 0x0000;
	storage_profiles[SP_IPSEC].pbs3 = 0x0000;
	storage_profiles[SP_IPSEC].bpid3 = 0x0000;
	storage_profiles[SP_IPSEC].pbs4 = 0x0000;
	storage_profiles[SP_IPSEC].bpid4 = 0x0000;
#endif


/* TODO - remove the AIOP_VERIF section when verification env will include
 * the ARENA code */
#ifdef AIOP_VERIF
	/* TMAN EPID Init */

	val = 1;
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0xF8);
	*addr = (uint32_t)(((val & 0x000000ff) << 24) |
			((val & 0x0000ff00) <<  8) |
			((val & 0x00ff0000) >>  8) |
			((val & 0xff000000) >> 24));

	val = (uint32_t)&tman_timer_callback;
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0x100);
	*addr = (uint32_t)(((val & 0x000000ff) << 24) |
			((val & 0x0000ff00) <<  8) |
			((val & 0x00ff0000) >>  8) |
			((val & 0xff000000) >> 24));

	val = 0x00600040;
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0x108);
	*addr = (uint32_t)(((val & 0x000000ff) << 24) |
			((val & 0x0000ff00) <<  8) |
			((val & 0x00ff0000) >>  8) |
			((val & 0xff000000) >> 24));
	val = 0x02000000; /*SET NDS bit*/
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0x118);
	*addr = (uint32_t)(((val & 0x000000ff) << 24) |
			((val & 0x0000ff00) <<  8) |
			((val & 0x00ff0000) >>  8) |
			((val & 0xff000000) >> 24));
#if 0 /*TODO - need to delete the above code and enable the bellow if 0
	when ENGR00310809 will be fixed */
	/* TODO - need to change the constant below to -
	 *define EPID_TIMER_EVENT_IDX	1 */
	__stwbr(1,
		0,
		(void *)(AIOP_WRKS_REGISTERS_OFFSET + 0xF8)); /* EPID = 1 */
	__stwbr((unsigned int)&tman_timer_callback,
		0,
		(void *)(AIOP_WRKS_REGISTERS_OFFSET + 0x100)); /* EP_PC */
	__stwbr(0x00600040,
		0,
		(void *)(AIOP_WRKS_REGISTERS_OFFSET + 0x108)); /* EP_FDPA */
	__stwbr(0x02000000,
		0,
		(void *)(AIOP_WRKS_REGISTERS_OFFSET + 0x118)); /* SET NDS bit*/

	/* End of TMAN EPID Init */
#endif
#else
	/* TMAN EPID Init */
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;

	/* TODO - need to change the constant below to -
	 *define EPID_TIMER_EVENT_IDX	1 */
	iowrite32(1, &wrks_addr->epas); /* EPID = 1 */
	iowrite32(PTR_TO_UINT(tman_timer_callback), &wrks_addr->ep_pc);
	iowrite32(0x02000000, &wrks_addr->ep_spo); /* SET NDS bit */

	pr_info("TMAN is setting EPID = 1\n");
	pr_info("ep_pc = 0x%x\n", ioread32(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x\n", ioread32(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x\n", ioread32(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x\n", ioread32(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x\n", ioread32(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x\n", ioread32(&wrks_addr->ep_spo));
	/* End of TMAN EPID Init */
#endif

	sys_prpid_pool_create();

#ifdef AIOP_VERIF
	sys_keyid_pool_create();
#else
	sys_keyid_pool_create();

	status = ipr_init();
	if (status)
		return status; /* TODO */

	status = aiop_snic_init();
	return status;
#endif
	return 0;
}

void aiop_sl_free(void)
{
/*	int32_t status = 0;*/

	cdma_release_context_memory(ext_prpid_pool_address);
	cdma_release_context_memory(ext_keyid_pool_address);

	/* TODO status ? */
}


