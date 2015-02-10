/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common/fsl_string.h"
#include "fsl_io_ccsr.h"
#include "fsl_dprc.h"
#include "fsl_dpni.h"
#include "fsl_malloc.h"
#include "kernel/fsl_spinlock.h"
#include "../drivers/dplib/arch/accel/fdma.h"  /* TODO: need to place fdma_release_buffer() in separate .h file */
#include "fsl_dpbp.h"
#include "sys.h"
#include "fsl_io_ccsr.h"
#include "slab.h"
#include "cmgw.h"
#include "fsl_mc_init.h"
#include "fsl_dpni_drv.h"
#include "fsl_mem_mng.h"
#include "fsl_bman.h"
#include "platform.h"



/* Address of end of memory_data section */
extern const uint8_t AIOP_INIT_DATA[];
extern struct platform_app_params g_app_params;
extern struct aiop_init_info g_init_data;
/*********************************************************************/
extern int time_init();                   extern void time_free();
extern int mc_obj_init();                 extern void mc_obj_free();
extern int cmdif_client_init();           extern void cmdif_client_free();
extern int cmdif_srv_init(void);          extern void cmdif_srv_free(void);
extern int dpni_drv_init(void);           extern void dpni_drv_free(void);
extern int slab_module_early_init(void);  extern int slab_module_init(void);
extern void slab_module_free(void);
extern int aiop_sl_early_init(void);
extern int aiop_sl_init(void);            extern void aiop_sl_free(void);
extern int icontext_init();

extern void discard_rx_cb();
extern void tman_timer_callback(void);
extern void cmdif_cl_isr(void);
extern void cmdif_srv_isr(void);


extern void build_apps_array(struct sys_module_desc *apps);

// TODO remove hard-coded values from  MEM_PART_MC_PORTALS and MEM_PART_CCSR
#define MEMORY_PARTITIONS\
{   /* Memory partition ID                  Phys. Addr.  Virt. Addr.  Size , Attributes */\
	{MEM_PART_SYSTEM_DDR1_BOOT_MEM_MNG,  0xFFFFFFFF,  0xFFFFFFFF, g_boot_mem_mng_size,\
	        MEMORY_ATTR_NONE, "BOOT MEMORY MANAGER"},\
	{MEM_PART_DP_DDR,                    0xFFFFFFFF,  0xFFFFFFFF,  0xFFFFFFFF,\
		MEMORY_ATTR_PHYS_ALLOCATION,"DP_DDR"},\
	{MEM_PART_MC_PORTALS,                0xFFFFFFFF,  0xFFFFFFFF, (64  * MEGABYTE),\
		MEMORY_ATTR_NONE,"MC Portals"},\
	{MEM_PART_CCSR,                      0xFFFFFFFF,  0xFFFFFFFF, (64 * MEGABYTE),\
		MEMORY_ATTR_NONE,"SoC CCSR"  },\
	{MEM_PART_SH_RAM,                    0x01010800,   0x01010800,(190 * KILOBYTE),\
		MEMORY_ATTR_MALLOCABLE,"Shared-SRAM"},\
	{MEM_PART_PEB,                        0xFFFFFFFF,  0xFFFFFFFF,0xFFFFFFFF,\
		MEMORY_ATTR_PHYS_ALLOCATION,"PEB"},\
	{MEM_PART_SYSTEM_DDR,                 0xFFFFFFFF,  0xFFFFFFFF,0xFFFFFFFF,\
		MEMORY_ATTR_PHYS_ALLOCATION,"SYSTEM_DDR"},\
}

#define GLOBAL_MODULES                                                       \
	{    /* slab must be before any module with buffer request*/             \
	{icontext_init, NULL, NULL},                                             \
	{NULL, time_init,         time_free},                                    \
	{NULL, epid_drv_init,     epid_drv_free},                                \
	{NULL, mc_obj_init,       mc_obj_free},                                  \
	{slab_module_early_init, slab_module_init,  slab_module_free},           \
	{NULL, cmdif_client_init, cmdif_client_free}, /* must be before srv */   \
	{NULL, cmdif_srv_init,    cmdif_srv_free},                               \
	{aiop_sl_early_init, aiop_sl_init,      aiop_sl_free},                                 \
	{NULL, dpni_drv_init,     dpni_drv_free}, /*must be after aiop_sl_init*/ \
	{NULL, NULL, NULL} /* never remove! */                                   \
	}

void fill_platform_parameters(struct platform_param *platform_param);
int global_init(void);
void global_free(void);
int global_early_init(void);
int apps_early_init(void);
int global_post_init(void);
int tile_init(void);
int cluster_init(void);
int run_apps(void);
void core_ready_for_tasks(void);
void global_free(void);
int epid_drv_init(void);
void epid_drv_free(void);

#include "general.h"
/** Global task params */
extern __TASK struct aiop_default_task_params default_task_params;

__COLD_CODE void fill_platform_parameters(struct platform_param *platform_param)
{

	int err = 0;

	memset(platform_param, 0, sizeof(platform_param));

	platform_param->clock_in_freq_khz = g_init_data.sl_info.sys_clk; //TODO check value
	platform_param->l1_cache_mode = E_CACHE_MODE_INST_ONLY;
	platform_param->console_type = PLTFRM_CONSOLE_DUART;
	platform_param->console_id = 4;//(uint8_t)g_init_data.sl_info.uart_port_id;

	if(platform_param->clock_in_freq_khz == 0)
	{
		platform_param->clock_in_freq_khz = 400000;
		pr_warn("rcwsr return 0, clock frequency was set to 400000 KHz\n");
	}

	struct platform_memory_info mem_info[] = MEMORY_PARTITIONS;
	ASSERT_COND(ARRAY_SIZE(platform_param->mem_info) >
	ARRAY_SIZE(mem_info));
	memcpy(platform_param->mem_info, mem_info,
	       sizeof(struct platform_memory_info) * ARRAY_SIZE(mem_info));
	ASSERT_COND(err == 0);

}

__COLD_CODE int tile_init(void)
{
	struct aiop_tile_regs * aiop_regs = (struct aiop_tile_regs *)
				      sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	uint32_t val;

	if(aiop_regs) {
		/* ws enable */
		val = ioread32_ccsr(&aiop_regs->ws_regs.cfg);
		val |= 0x3; /* AIOP_WS_ENABLE_ALL - Enable work scheduler to receive tasks from both QMan and TMan */
		iowrite32_ccsr(val, &aiop_regs->ws_regs.cfg);
	}
	else {
		return -EFAULT;
	}

	return 0;
}

__COLD_CODE int cluster_init(void)
{
	return 0;
}

__COLD_CODE int global_init(void)
{
	struct sys_module_desc modules[] = GLOBAL_MODULES;
	int i, err;

	/* Verifying that MC saw the data at the beginning of special section
	 * and at fixed address
	 * TODO is it the right place to verify it ? Can't place it at sys_init()
	 * because it's too generic. */
	ASSERT_COND((((uint8_t *)(&g_init_data.sl_info)) == AIOP_INIT_DATA) &&
	            (AIOP_INIT_DATA == AIOP_INIT_DATA_FIXED_ADDR));

	for (i=0; i<ARRAY_SIZE(modules) ; i++)
	{
		if (modules[i].init)
		{
			err = modules[i].init();
			if(err) return err;
		}
	}

	return 0;
}

__COLD_CODE void global_free(void)
{
	struct sys_module_desc modules[] = GLOBAL_MODULES;
	int i;

	for (i = (ARRAY_SIZE(modules) - 1); i >= 0; i--)
		if (modules[i].free)
			modules[i].free();
}

__COLD_CODE int global_early_init(void)
{
	struct sys_module_desc modules[] = GLOBAL_MODULES;
	int i, err;

	for (i=0; i<ARRAY_SIZE(modules) ; i++)
	{
		if (modules[i].early_init)
		{
			err = modules[i].early_init();
			if(err) return err;
		}
	}

	return 0;
}

__COLD_CODE int apps_early_init(void)
{
	int i, err;
	uint16_t app_arr_size = g_app_params.app_arr_size;
	struct sys_module_desc *apps = \
		fsl_malloc(app_arr_size * sizeof(struct sys_module_desc), 1);

	if(apps == NULL) {
		return -ENOMEM;
	}

	memset(apps, 0, app_arr_size * sizeof(struct sys_module_desc));
	build_apps_array(apps);

	for (i=0; i<app_arr_size; i++)
	{
		if (apps[i].early_init)
		{
			err = apps[i].early_init();
			if(err) {
				fsl_free(apps);
				return err;
			}
		}
	}

	fsl_free(apps);

	return 0;
}

__COLD_CODE int global_post_init(void)
{
	return 0;
}

#if (STACK_OVERFLOW_DETECTION == 1)
__COLD_CODE static inline void config_runtime_stack_overflow_detection()
{
	switch(cmgw_get_ntasks())
	{
	case 0: /* 1 Task */
		booke_set_spr_DAC2(0x8000);
		break;
	case 1: /* 2 Tasks */
		booke_set_spr_DAC2(0x4000);
		break;
	case 2: /* 4 Tasks */
		booke_set_spr_DAC2(0x2000);
		break;
	case 3: /* 8 Tasks */
		booke_set_spr_DAC2(0x1000);
		break;
	case 4: /* 16 Tasks */
		booke_set_spr_DAC2(0x800);
		break;
	default:
		//TODO complete
		break;
	}
}
#endif /* STACK_OVERFLOW_DETECTION */

__COLD_CODE void core_ready_for_tasks(void)
{
	/*
	 * CTSCSR_ntasks mast be a 'register' in order to prevent stack access
	 * after stack overflow detection is enabled
	 */
	register uint32_t CTSCSR_ntasks;

	/*  finished boot sequence; now wait for event .... */
	pr_info("AIOP core %d completed boot sequence\n", core_get_id());

	sys_barrier();

	if(sys_is_master_core()) {
		/* At this stage, all the NIC of AIOP are up and running */
		pr_info("AIOP boot finished; ready for tasks...\n");
	}

	sys_barrier();

	sys.runtime_flag = 1;

	cmgw_update_core_boot_completion();

	CTSCSR_ntasks = (cmgw_get_ntasks() << 24) & CTSCSR_TASKS_MASK;

	//TODO write following code in assembly to ensure stack is not accessed

#if (STACK_OVERFLOW_DETECTION == 1)
	/*
	 *  NOTE:
	 *  Any access to the stack (read/write) following this line will cause
	 *  a stack-overflow violation and an exception will occur.
	 */
	config_runtime_stack_overflow_detection();
#endif

	/* CTSEN = 1, finished boot, Core Task Scheduler Enable */
	booke_set_CTSCSR0(booke_get_CTSCSR0() | CTSCSR_ENABLE | CTSCSR_ntasks);
	__e_hwacceli(YIELD_ACCEL_ID); /* Yield */
}


__COLD_CODE static void print_dev_desc(struct dprc_obj_desc* dev_desc)
{
	pr_debug(" device %d\n", dev_desc->id);
	pr_debug("***********\n");
	pr_debug("vendor - %x\n", dev_desc->vendor);

	if (strcmp(dev_desc->type, "dpni") == 0)
	{
		pr_debug("type - DP_DEV_DPNI\n");
	}
	else if (strcmp(dev_desc->type, "dprc") == 0)
	{
		pr_debug("type - DP_DEV_DPRC\n");
	}
	else if (strcmp(dev_desc->type, "dpio") == 0)
	{
		pr_debug("type - DP_DEV_DPIO\n");
	}
	pr_debug("id - %d\n", dev_desc->id);
	pr_debug("region_count - %d\n", dev_desc->region_count);
	pr_debug("ver_major - %d\n", dev_desc->ver_major);
	pr_debug("ver_minor - %d\n", dev_desc->ver_minor);
	pr_debug("irq_count - %d\n\n", dev_desc->irq_count);

}

__COLD_CODE int run_apps(void)
{
	int i;
	int err = 0;
	int dev_count;
	int num_bpids = 0;
	/* TODO: replace with memset */
	uint16_t dpbp = 0;
	struct dprc_obj_desc dev_desc;
	int dpbp_id[DPNI_DRV_NUM_USED_BPIDS];
	struct dpbp_attr attr;
	struct dpni_pools_cfg pools_params[DPNI_DRV_NUM_USED_BPIDS];
	uint16_t buffer_size = (uint16_t)g_app_params.dpni_buff_size;
	uint16_t num_buffs = (uint16_t)g_app_params.dpni_num_buffs;
	uint16_t alignment;
	uint8_t mem_pid[] = {DPNI_DRV_FAST_MEMORY, DPNI_DRV_DDR_MEMORY};
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	uint16_t app_arr_size = g_app_params.app_arr_size;
	struct sys_module_desc *apps = \
		fsl_malloc(app_arr_size * sizeof(struct sys_module_desc), 1);

	if(apps == NULL) {
		return -ENOMEM;
	}

	/* TODO - add initialization of global default DP-IO (i.e. call 'dpio_open', 'dpio_init');
	 * This should be mapped to ALL cores of AIOP and to ALL the tasks */
	/* TODO - add initialization of global default DP-SP (i.e. call 'dpsp_open', 'dpsp_init');
	 * This should be mapped to 3 buff-pools with sizes: 128B, 512B, 2KB;
	 * all should be placed in PEB. */
	/* TODO - need to scan the bus in order to retrieve the AIOP "Device list" */
	/* TODO - iterate through the device-list:
	 * call 'dpni_drv_probe(ni_id, mc_portal_id, dpio, dp-sp)' */


	if(IS_POWER_VALID_ALLIGN(g_app_params.dpni_drv_alignment,buffer_size))
		alignment = (uint16_t)g_app_params.dpni_drv_alignment;
	else
	{
		pr_err("Given alignment is not valid (not power of 2 or <= buffer size)\n");
	}
	if (dprc == NULL)
	{
		pr_err("Don't find AIOP root container \n");
		return -ENODEV;
	}
	/* TODO: replace the following dpbp_open&init with dpbp_create when available */


	if ((err = dprc_get_obj_count(&dprc->io, dprc->token,
	                              &dev_count)) != 0) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			/* TODO: print conditionally based on log level */
			pr_info("Found DPBP ID: %d, will be used for frame buffers\n",dev_desc.id);
			dpbp_id[num_bpids]= dev_desc.id;
			num_bpids ++;

			if(num_bpids == DPNI_DRV_NUM_USED_BPIDS)
				break;
		}
	}



	if(num_bpids < DPNI_DRV_NUM_USED_BPIDS){
		pr_err("Not enough DPBPs found in the container.\n");
		return -ENAVAIL;
	}

	for(i = 0; i < DPNI_DRV_NUM_USED_BPIDS; i++)
	{
		if ((err = dpbp_open(&dprc->io, dpbp_id[i], &dpbp)) != 0) {
			pr_err("Failed to open DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = dpbp_enable(&dprc->io, dpbp)) != 0) {
			pr_err("Failed to enable DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = dpbp_get_attributes(&dprc->io, dpbp, &attr)) != 0) {
			pr_err("Failed to get attributes from DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = bman_fill_bpid(num_buffs,
		                          buffer_size,
		                          alignment,
		                          (enum memory_partition_id) mem_pid[i],
		                          attr.bpid)) != 0) {
			pr_err("Failed to fill DPBP-%d (BPID=%d) with buffer size %d.\n",
			       dpbp_id[i], attr.bpid, buffer_size);
			return err;
		}

		/* Prepare parameters to attach to DPNI object */
		pools_params[i].num_dpbp = 1; /* for AIOP, can be up to 2 */
		pools_params[i].pools[0].dpbp_id = (uint16_t)dpbp_id[i]; /*!< DPBPs object id */
		pools_params[i].pools[0].buffer_size = buffer_size;

		/* Enable all DPNI devices */
	}
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpni") == 0) {
			/* TODO: print conditionally based on log level */
			print_dev_desc(&dev_desc);

			if ((err = dpni_drv_probe(dprc, (uint16_t)dev_desc.id, (uint16_t)i, pools_params)) != 0) {
				pr_err("Failed to probe DPNI-%d.\n", i);
				return err;
			}
		
		}
	}

	fsl_os_print("Boot DONE\n");


	memset(apps, 0, (app_arr_size * sizeof(struct sys_module_desc)));
	build_apps_array(apps);

	for (i=0; i<app_arr_size; i++) {
		if (apps[i].init)
			apps[i].init();
	}

	fsl_free(apps);

	return 0;
}

static int cmdif_epid_setup(struct aiop_ws_regs *wrks_addr,
                            uint32_t epid,
                            void (*isr_cb)(void))
{
	uint32_t data = 0;
	int      err = 0;

	iowrite32_ccsr(epid, &wrks_addr->epas); /* EPID = 2 */
	iowrite32_ccsr(PTR_TO_UINT(isr_cb), &wrks_addr->ep_pc);

	/* no PTA presentation is required (even if there is a PTA)*/
	iowrite32_ccsr(0x0000ffc0, &wrks_addr->ep_ptapa);
	/* set epid ASA presentation size to 0 */
	iowrite32_ccsr(0x00000000, &wrks_addr->ep_asapa);
	/* Set mask for hash to 16 low bits OSRM = 5 */
	iowrite32_ccsr(0x11000005, &wrks_addr->ep_osc);
	data = ioread32_ccsr(&wrks_addr->ep_osc);
	if (data != 0x11000005)
		err |= -EINVAL;

	pr_info("CMDIF is setting EPID = %d\n", epid);
	pr_info("ep_pc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spo));
	pr_info("ep_osc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_osc));

	if (err) {
		pr_err("Failed to setup EPID %d\n", epid);
		/* No return err here in order to setup the rest of EPIDs */
	}
	return err;
}

int epid_drv_init(void)
{
	int i = 0;
	int err = 0;

	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)
			(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,
			                                   0,
			                                   E_MAPPED_MEM_TYPE_GEN_REGS)
			                                   + SOC_PERIPH_OFF_AIOP_WRKS);

	/* CMDIF server epid initialization here*/
	err |= cmdif_epid_setup(wrks_addr, AIOP_EPID_CMDIF_SERVER, cmdif_srv_isr);

	/* TMAN epid initialization */
	iowrite32_ccsr(AIOP_EPID_TIMER_EVENT_IDX, &wrks_addr->epas); /* EPID = 1 */
	iowrite32_ccsr(PTR_TO_UINT(tman_timer_callback), &wrks_addr->ep_pc);
	iowrite32_ccsr(0x02000000, &wrks_addr->ep_spo); /* SET NDS bit */

	pr_info("TMAN is setting EPID = %d\n", AIOP_EPID_TIMER_EVENT_IDX);
	pr_info("ep_pc = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_spo));


	/* CMDIF interface client epid initialization here*/
	err |= cmdif_epid_setup(wrks_addr, AIOP_EPID_CMDIF_CLIENT, cmdif_cl_isr);

	/* Initialize EPID-table with discard_rx_cb for all NI's entries (EP_PC field) */
	for (i = AIOP_EPID_DPNI_START; i < AIOP_EPID_TABLE_SIZE; i++) {
		/* Prepare to write to entry i in EPID table - EPAS reg */
		iowrite32_ccsr((uint32_t)i, &wrks_addr->epas);

		iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	}

	return err;
}

void epid_drv_free(void)
{
}
