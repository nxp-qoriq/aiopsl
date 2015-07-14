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

#include "fsl_dbg.h"
#include "common/types.h"
#include "fsl_malloc.h"
#include "common/fsl_string.h"
#include "fsl_aiop_common.h"
#include "fsl_duart.h"
#include "console.h"
#include "fsl_platform.h"
#include "fsl_malloc.h"
#include "fsl_smp.h"
#include "fsl_io_ccsr.h"
#include "fsl_mem_mng.h"
#include "fsl_sys.h"
#include "fsl_mem_mng.h"

#define __ERR_MODULE__  MODULE_SOC_PLATFORM
extern __TASK uint32_t seed_32bit;
extern struct aiop_init_info g_init_data;
extern const uint8_t AIOP_DDR_START[],AIOP_DDR_END[],_ssram_heap_start[],\
                     _ssram_addr[];

typedef struct t_platform {
	/* Copy of platform parameters */
	struct platform_param   param;

	/* Memory-related variables */
	int                     num_of_mem_parts;
	int                     registered_partitions[PLATFORM_MAX_MEM_INFO_ENTRIES];

	/* Platform clock in KHz */
	uint32_t                platform_clk;

	/* Console-related variables */
	fsl_handle_t            uart;
	uint32_t                duart_id;

	uintptr_t               aiop_base;
	uintptr_t               ccsr_base;
	uintptr_t               mc_portals_base;
} t_platform;

/* Global platform variable that holds platform definitions. Will reside in sram */
static struct t_platform  s_pltfrm = {0};

typedef struct t_sys_to_part_offset_map {
	enum fsl_os_module  module;
	uint32_t            id;
	e_mapped_mem_type   mapped_mem_type;
	uint32_t            offset;
} t_sys_to_part_offset_map;






/* Module names for debug messages */
const char *module_strings[] = {
                                "???"                      /* MODULE_UNKNOWN */
                                ,"MEM"                      /* MODULE_MEM */
                                ,"SLOB"                       /* MODULE_MM */
                                ,"CORE"                     /* MODULE_CORE */
                                ,"SoC"                      /* MODULE_LS2085A */
                                ,"SoC-platform"             /* MODULE_LS2085A_PLATFORM */
                                ,"PM"                       /* MODULE_PM */
                                ,"MMU"                      /* MODULE_MMU */
                                ,"PIC"                      /* MODULE_PIC */
                                ,"DUART"                    /* MODULE_DUART */
                                ,"SERDES"                   /* MODULE_SERDES */
                                ,"QM"                       /* MODULE_QM */
                                ,"BM"                       /* MODULE_BM */
                                ,"SEC"                      /* MODULE_SEC */
                                ,"LAW"                      /* MODULE_LAW */
                                ,"LBC"                      /* MODULE_LBC */
                                ,"MII"                      /* MODULE_MII */
                                ,"DMA"                      /* MODULE_DMA */
                                ,"SRIO"                     /* MODULE_SRIO */
                                ,"RMan"                     /* MODULE_RMAN */
};

static int build_mem_partitions_table(t_platform  *pltfrm);

/*****************************************************************************/
__COLD_CODE static int find_mem_partition_index(t_platform_memory_info  *mem_info,
                                                int32_t   mem_partition)
{
	int i;

	ASSERT_COND(mem_info);

	for (i = 0; i < PLATFORM_MAX_MEM_INFO_ENTRIES; i++) {
		if (mem_info[i].size == 0)
			break;

		if ((mem_info[i].mem_partition_id == mem_partition))
			/* Found requested memory region */
			return i;
	}

	/* Not found */
	return -1;
}
/*****************************************************************************/
__COLD_CODE static int init_l1_cache(t_platform *pltfrm)
{
	ASSERT_COND(pltfrm);

	/* L1 Cache Init */
	if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY) {
		booke_icache_enable();
	}

	if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_DATA_ONLY) {
		pr_err("unsupported operation");
		return -ENOTSUP;
	}

	return 0;
}

/*****************************************************************************/
__COLD_CODE static int disable_l1_cache(t_platform *pltfrm)
{
	if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY)
		booke_icache_disable();

	if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_DATA_ONLY) {
		pr_err("unsupported operation");
		return -ENOTSUP;
	}

	return 0;
}


/*****************************************************************************/
static int console_print_cb(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size)
{
	return duart_tx(h_console_dev, p_data, size);
}

/*****************************************************************************/
static int console_print_cb_uart_disabled(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size)
{
	UNUSED(h_console_dev);
	UNUSED(p_data);
	UNUSED(size);
	return 0;
}

/*****************************************************************************/
static int console_get_line_cb(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size)
{
	uint32_t count;

	count = duart_rx(h_console_dev, p_data, size);

	return (int)count;
}

/*****************************************************************************/
__COLD_CODE static int init_random_seed(uint32_t num_of_tasks)
{
	volatile uint32_t *seed_mem_ptr = NULL;
	uint32_t core_and_task_id = 0;
	uint32_t seed = 0;
	uint32_t task_stack_size = 0;
	int i;
	/*------------------------------------------------------*/
	/* Initialize seeds for random function                 */
	/*------------------------------------------------------*/

	/* task stack size used for pointer calculation,
	       (original size = task_stack_size * 4)
	       num_of_tasks received as bit and translated
	       to integer in switch.
	 */
	switch(num_of_tasks) {
	case (0):
					    num_of_tasks = 1;
	break;
	case (1):
					    num_of_tasks = 2;
	task_stack_size = 0x1000;
	break;
	case (2):
					    num_of_tasks = 4;
	task_stack_size = 0x800;
	break;
	case (3):
					    num_of_tasks = 8;
	task_stack_size = 0x400;
	break;
	case (4):
					    num_of_tasks = 16;
	task_stack_size = 0x200;
	break;
	default:
		return -EINVAL;

	}

	core_and_task_id =  ((core_get_id() + 1) << 8);
	core_and_task_id |= 1; /*add task 0 id*/

	seed = (core_and_task_id << 16) | core_and_task_id;
	seed_mem_ptr = &(seed_32bit);
	iowrite32be(seed, seed_mem_ptr);
	/*seed for task 0 is already allocated*/
	for (i = 0; i < num_of_tasks - 1; i ++)
	{
		seed_mem_ptr += task_stack_size; /*size of each task area*/
		core_and_task_id ++; /*increment the task id accordingly to its tls section*/
		seed = (core_and_task_id << 16) | core_and_task_id;
		iowrite32be(seed, seed_mem_ptr);
	}

	return 0;
}
/*****************************************************************************/
__COLD_CODE static int pltfrm_init_core_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;
	int     err = 0;
	uint32_t WSCR_tasks_bit = 0;
	struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *)
	                        	       sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);

	if (pltfrm == NULL) {
		return -EINVAL;
	}

	if (aiop_regs == NULL) {
		return -EFAULT;
	}

	/*------------------------------------------------------*/
	/* Initialize PPC interrupts vector                     */
	/*------------------------------------------------------*/
	booke_generic_irq_init();

#ifndef DEBUG
	/* Enable the BTB - branches predictor */
	booke_set_spr_BUCSR(booke_get_spr_BUCSR() | 0x00000201);
#endif /* DEBUG */
	/* special AIOP registers */

	/* Workspace Control Register*/
	WSCR_tasks_bit = ioread32_ccsr(&aiop_regs->cmgw_regs.wscr) & 0x0000000f;

	/*------------------------------------------------------*/
	/* Initialize L1 Cache                                  */
	/*------------------------------------------------------*/
	err = init_l1_cache(pltfrm);
	if (err != 0) {
		pr_err("init l1 cache failed\n");
		return err;
	}

	/*initialize random seeds*/
	err = init_random_seed(WSCR_tasks_bit);
	if (err != 0) {
		pr_err("init random seed failed\n");
		return err;
	}

	return 0;
}
/*****************************************************************************/
__COLD_CODE static int pltfrm_free_core_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;

	ASSERT_COND(pltfrm);

	/* Disable L1 cache */
	disable_l1_cache(pltfrm);

	return 0;
}


/*****************************************************************************/
__COLD_CODE static int pltfrm_init_console_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;
	int     err;

	ASSERT_COND(pltfrm);

	if (sys_is_master_core()) {
		/* Master partition - register DUART console */
		err = platform_enable_console(pltfrm);
		if (err != 0){
			err = sys_register_console((fsl_handle_t) -1, console_print_cb_uart_disabled, NULL);

			/*Uart failed. the print will go only to buffer*/
			pr_warn("UART print failed, all debug data will be printed to buffer.\n");

			return err;
		}

		err = sys_register_console(pltfrm->uart, console_print_cb, console_get_line_cb);
		if (err != 0)
			return err;
	}

	return 0;
}

/*****************************************************************************/
__COLD_CODE static int pltfrm_free_console_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;

	ASSERT_COND(pltfrm);

	if (sys_is_master_core())
		platform_disable_console(pltfrm);

	sys_unregister_console();

	return 0;
}

/*****************************************************************************/
__COLD_CODE static int pltfrm_init_mem_partitions_cb(fsl_handle_t h_platform)
{
	t_platform              *pltfrm = (t_platform *)h_platform;
	t_platform_memory_info  *p_mem_info;
	int                     err;
	uintptr_t               virt_base_addr;
	uint64_t                size;
	int                     i, index = 0;
	char                    name[32];

	ASSERT_COND(pltfrm);

	build_mem_partitions_table(pltfrm);

	err = sys_add_handle( (fsl_handle_t)pltfrm->mc_portals_base, FSL_OS_MOD_MC_PORTAL, 1, 0);
	if (err != 0)
	{
		pr_err("Couldn't add FSL_OS_MOD_MC_PORTAL using sys_add_handle()\n");
		return err;
	}

	for (i = 0; i < pltfrm->num_of_mem_parts; i++) {
		p_mem_info = &pltfrm->param.mem_info[i];
		virt_base_addr = p_mem_info->virt_base_addr;
		size = p_mem_info->size;

		if (p_mem_info->mem_attribute & MEMORY_ATTR_MALLOCABLE)
		{
			err = sys_register_mem_partition(p_mem_info->mem_partition_id,
			                                 virt_base_addr,
			                                 size,
			                                 p_mem_info->mem_attribute,
			                                 name,
#ifdef DEBUG
                                             1
#else
	0
#endif /* DEBUG */
			);
			if (err != 0)
				return err;
			pltfrm->registered_partitions[index++] = p_mem_info->mem_partition_id;
		}
		if(p_mem_info->mem_attribute & MEMORY_ATTR_PHYS_ALLOCATION){
			err = sys_register_phys_addr_alloc_partition(
				p_mem_info->mem_partition_id,
				p_mem_info->phys_base_addr,
				p_mem_info->size,
				p_mem_info->mem_attribute,
				p_mem_info->name
			);
			if (err != 0)
				return err;
		}// end of MEMORY_ATTR_PHYS_ALLOCATION
	}// for
    sys_mem_partitions_init_complete();
	return 0;
}

/*****************************************************************************/
__COLD_CODE static int build_mem_partitions_table(t_platform  *pltfrm)
{
	t_platform_memory_info  *p_mem_info;
	int                     i;
	uint32_t                aiop_lcf_ddr_size;
	aiop_lcf_ddr_size =  (uint32_t)(AIOP_DDR_END) - (uint32_t)(AIOP_DDR_START);

	for (i = 0; i < pltfrm->num_of_mem_parts; i++) {
		p_mem_info = &pltfrm->param.mem_info[i];
		ASSERT_COND(p_mem_info);
		switch (p_mem_info->mem_partition_id) {
		case MEM_PART_DP_DDR:
#ifndef NO_DP_DDR
			p_mem_info->virt_base_addr = (uint32_t)g_init_data.sl_info.dp_ddr_vaddr +
			aiop_lcf_ddr_size + g_boot_mem_mng_size ;
			p_mem_info->phys_base_addr = g_init_data.sl_info.dp_ddr_paddr +
				aiop_lcf_ddr_size + g_boot_mem_mng_size;
			p_mem_info->size = g_init_data.app_info.dp_ddr_size -
				aiop_lcf_ddr_size - g_boot_mem_mng_size;
#else
			p_mem_info->virt_base_addr = (uint32_t)g_init_data.sl_info.dp_ddr_vaddr;
			p_mem_info->phys_base_addr = g_init_data.sl_info.dp_ddr_paddr;
			p_mem_info->size = g_init_data.app_info.dp_ddr_size;
#endif
			pr_debug("MEM_PART_DP_DDR:virt_add=0x%x,phys_add=0x%x%08x,size=0x%x\n",
			         p_mem_info->virt_base_addr,
			         (uint32_t)(p_mem_info->phys_base_addr >> 32),
			         (uint32_t)(p_mem_info->phys_base_addr),
			         (uint32_t)(p_mem_info->size));
			break;
		case MEM_PART_PEB:
			p_mem_info->virt_base_addr = (uint32_t)g_init_data.sl_info.peb_vaddr;
			p_mem_info->phys_base_addr = g_init_data.sl_info.peb_paddr;
			p_mem_info->size = g_init_data.app_info.peb_size;
			pr_debug("MEM_PART_PEB:virt_add=0x%x,phys_add=0x%x%08x,size=0x%x\n",
			         p_mem_info->virt_base_addr,
			         (uint32_t)(p_mem_info->phys_base_addr >> 32),
			         (uint32_t)(p_mem_info->phys_base_addr),
			         (uint32_t)(p_mem_info->size));
			break;
		case  MEM_PART_SYSTEM_DDR:
#ifndef NO_DP_DDR
			p_mem_info->virt_base_addr = (uint32_t)g_init_data.sl_info.sys_ddr1_vaddr;
			p_mem_info->phys_base_addr = g_init_data.sl_info.sys_ddr1_paddr;
			p_mem_info->size = g_init_data.app_info.sys_ddr1_size;
#else
			p_mem_info->virt_base_addr = (uint32_t)g_init_data.sl_info.sys_ddr1_vaddr
				                   + aiop_lcf_ddr_size + g_boot_mem_mng_size;
			p_mem_info->phys_base_addr = g_init_data.sl_info.sys_ddr1_paddr
		                            + aiop_lcf_ddr_size + g_boot_mem_mng_size;
			p_mem_info->size = g_init_data.app_info.sys_ddr1_size
				           - aiop_lcf_ddr_size - g_boot_mem_mng_size;
#endif

			pr_debug("MEM_PART_SYSTEM_DDR:virt_add=0x%x,phys_add=0x%x%08x,size=0x%x\n",
			         p_mem_info->virt_base_addr,
			         (uint32_t)(p_mem_info->phys_base_addr >> 32),
			         (uint32_t)(p_mem_info->phys_base_addr),
			         (uint32_t)(p_mem_info->size));
			break;
		case MEM_PART_MC_PORTALS:
			p_mem_info->virt_base_addr =
				(uint32_t)g_init_data.sl_info.mc_portals_vaddr;
			p_mem_info->phys_base_addr = g_init_data.sl_info.mc_portals_paddr;
			// TODO fill all the rest fields from g_init_data.sl_info
			/* Store MC-Portals bases (for convenience) */
			pltfrm->mc_portals_base =  p_mem_info->virt_base_addr;
			pr_debug("MEM_PART_MC_PORTALS:virt_add=0x%x,phys_add=0x%x%08x\n",
			         p_mem_info->virt_base_addr,
			         (uint32_t)(p_mem_info->phys_base_addr >> 32),
			         (uint32_t)(p_mem_info->phys_base_addr));
			break;
		case MEM_PART_CCSR:
			p_mem_info->virt_base_addr =
				(uint32_t)g_init_data.sl_info.ccsr_vaddr;
			p_mem_info->phys_base_addr = g_init_data.sl_info.ccsr_paddr;
			// TODO fill all the rest fields from g_init_data.sl_info
			/* Store CCSR base (for convenience) */
			pltfrm->ccsr_base =  p_mem_info->virt_base_addr;
			pr_debug("MEM_PART_CCSR:virt_add= 0x%x,phys_add=0x%x%08x\n",
			         p_mem_info->virt_base_addr,
			         (uint32_t)(p_mem_info->phys_base_addr >> 32),
			         (uint32_t)(p_mem_info->phys_base_addr));
			break;
		case MEM_PART_SH_RAM:
			uint32_t shared_ram_non_heap_size = (uint32_t)_ssram_heap_start -
							                     (uint32_t)_ssram_addr;
			p_mem_info->virt_base_addr = (uint32_t)_ssram_heap_start;
			p_mem_info->phys_base_addr =  p_mem_info->virt_base_addr;
			p_mem_info->size = SHARED_RAM_SIZE - shared_ram_non_heap_size;
			pr_debug("MEM_PART_SH_RAM:virt_add= 0x%x,phys_add=0x%x%08x,size=0x%x\n",
						         p_mem_info->virt_base_addr,
						         (uint32_t)(p_mem_info->phys_base_addr >> 32),
						         (uint32_t)(p_mem_info->phys_base_addr),
						         (uint32_t)(p_mem_info->size));
		    break;
		}
	}
	return 0;
}

/*****************************************************************************/
__COLD_CODE static int pltfrm_free_mem_partitions_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;
	int         index;

	ASSERT_COND(pltfrm);

	index = (int)pltfrm->num_of_mem_parts;

	while ((--index) >= 0) {
		/*sys_unregister_virt_mem_mapping(pltfrm->param.mem_info[index].virt_base_addr);*/

		if (pltfrm->registered_partitions[index])
			sys_unregister_mem_partition(pltfrm->registered_partitions[index]);
	}

	return 0;
}

#ifdef ARENA_LEGACY_CODE
/*****************************************************************************/
static int pltfrm_init_private_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;

	ASSERT_COND(pltfrm);

	if (sys_is_master_core() && (pltfrm->partition_id == SYS_MASTER_PART_ID)) {
#if 0
		/* Register Platform CLI commands */
		if (fsl_os_cli_is_available())
			register_cli_commands(pltfrm);
#endif /* 0 */
	}

	/* Print platform information */
	print_platform_info(pltfrm);

	return 0;
}

/*****************************************************************************/
static int pltfrm_free_private_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;

	ASSERT_COND(pltfrm);

	if (sys_is_master_core() && (pltfrm->partition_id == SYS_MASTER_PART_ID)) {
#if 0
		/* Unregister Platform CLI commands */
		if (fsl_os_cli_is_available())
			unregister_cli_commands(pltfrm);
#endif /* 0 */
	}

	return 0;
}
#endif

/*****************************************************************************/
__COLD_CODE int platform_early_init(struct platform_param *pltfrm_params)
{
	UNUSED(pltfrm_params);
	return 0;
}

__COLD_CODE int platform_init(struct platform_param    *pltfrm_param,
                              t_platform_ops           *pltfrm_ops)
{
	int             i;

	if((!pltfrm_param) || (!pltfrm_ops)) {
		pr_crit("Null pointer passed to platform_init");
		return -ENODEV;
	}


	memset(&s_pltfrm, 0, sizeof(t_platform));

	/* Store configuration parameters */
	memcpy(&(s_pltfrm.param), pltfrm_param, sizeof(struct platform_param));

	/* Count number of valid memory partitions and check that
       user's partition definition is within actual physical
       addresses range. */
	for (i=0; i<PLATFORM_MAX_MEM_INFO_ENTRIES; i++) {
		t_platform_memory_info      *mem_info;

		mem_info = s_pltfrm.param.mem_info + i;
		if (!mem_info->size)
			break;
	}
	s_pltfrm.num_of_mem_parts = i;

	/* Read clocks :
	 * s_pltfrm.platform_clk == 0 will not happen on SIMULATOR
	 * because the clock comes from MC
	 * */
	s_pltfrm.platform_clk = g_init_data.sl_info.platform_clk;

	/* Store AIOP-peripherals base (for convenience) */
	s_pltfrm.aiop_base = AIOP_PERIPHERALS_OFF;
	/* Initialize platform operations structure */
	pltfrm_ops->h_platform              = &s_pltfrm;

	memset(pltfrm_ops->modules, NULL, sizeof(struct pltform_module_desc) * PLTFORM_NUM_OF_INIT_MODULES);
	i = 0;

	/*
	 * Note: order of execution is according to the array placement
	 */
	pltfrm_ops->modules[i++] = (struct pltform_module_desc){.init = pltfrm_init_core_cb, .free = pltfrm_free_core_cb, .is_single_core = PLATFORM_MULTI_CORE};
	pltfrm_ops->modules[i++] = (struct pltform_module_desc){.init = pltfrm_init_mem_partitions_cb, .free = pltfrm_free_mem_partitions_cb, .is_single_core = PLATFORM_SINGLE_CORE};
	pltfrm_ops->modules[i++] = (struct pltform_module_desc){.init = pltfrm_init_console_cb, .free = pltfrm_free_console_cb, .is_single_core = PLATFORM_SINGLE_CORE};
#ifdef ARENA_LEGACY_CODE
	pltfrm_ops->modules[i++] = (struct pltform_module_desc){.init = pltfrm_init_private_cb, .free = pltfrm_free_private_cb, .is_single_core = PLATFORM_MULTI_CORE};
#endif
	ASSERT_COND(i <= PLTFORM_NUM_OF_INIT_MODULES);

return 0;
}

/*****************************************************************************/
__COLD_CODE int platform_free(fsl_handle_t h_platform)
{
	UNUSED(h_platform);
	return 0;
}

/*****************************************************************************/
uint32_t platform_get_clk(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;
	if(!pltfrm) {
		pr_crit("no device");
		return 0;
	}

	return (pltfrm->platform_clk);
}


/*****************************************************************************/
__COLD_CODE int platform_enable_console(fsl_handle_t h_platform)
{
	t_platform          *pltfrm = (t_platform *)h_platform;
	t_duart_uart_param  duart_uart_param;
	fsl_handle_t        uart;
	int           err = 0;
	uint32_t uart_port_offset[] = {
	                               0,
	                               SOC_PERIPH_OFF_DUART1,
	                               SOC_PERIPH_OFF_DUART2,
	                               SOC_PERIPH_OFF_DUART3,
	                               SOC_PERIPH_OFF_DUART4
	};

	if(!pltfrm) {
		pr_crit("Null pointer");
		return -ENODEV;
	}

	if(pltfrm->param.console_type == PLTFRM_CONSOLE_NONE)/*if console id is 0, print to buffer*/
		return -ENAVAIL;

	if(pltfrm->param.console_type != PLTFRM_CONSOLE_DUART) {
		pr_crit("Console not supported");
		return -ENOTSUP;
	}

	if( pltfrm->param.console_id > 4 ) {
		pr_err("resource is unavailable: DUART");
		return -EAGAIN;
	}

	/* Fill DUART configuration parameters */
	duart_uart_param.irq                = NO_IRQ;
	duart_uart_param.base_address       = pltfrm->ccsr_base + uart_port_offset[ pltfrm->param.console_id];
	/* Each UART is clocked by the platform clock/2 */
	duart_uart_param.system_clock_mhz   = (platform_get_clk(pltfrm) / 1000 /*convert to MHz*/) / 2;
	duart_uart_param.baud_rate          = 115200;
	duart_uart_param.parity             = E_DUART_PARITY_NONE;
	duart_uart_param.data_bits          = E_DUART_DATA_BITS_8;
	duart_uart_param.stop_bits          = E_DUART_STOP_BITS_1;
	duart_uart_param.flow_control       = E_DUART_NO_FLOW_CONTROL;
	duart_uart_param.h_app              = NULL;
	duart_uart_param.f_low_space_alert  = NULL;
	duart_uart_param.f_exceptions       = NULL;
	duart_uart_param.f_tx_conf          = NULL;

	/* Configure and initialize DUART driver */
	uart = duart_config(&duart_uart_param);
	if (!uart) {
		pr_err("resource is unavailable: DUART");
		return -EAGAIN;
	}

	/* Configure polling mode */
	err = duart_config_poll_mode(uart, 1);
	if (err != 0) {
		pr_err("duart poll\n");
		return err;
	}

	/* Convert end-of-line indicators */
	err = duart_config_poll_lf2crlf(uart, 1);
	if (err != 0) {
		pr_err("duart poll\n");
		return err;
	}

	/* Prevent blocking */
	err = duart_config_rx_timeout(uart, 0);
	if (err != 0) {
		pr_err("duart rx timeout\n");
		return err;
	}

	err = duart_init(uart);
	if (err != 0) {
		pr_err("duart init\n");
		return err;
	}

	/* Lock DUART handle in system */
	/*TODO: sys_get_handle in aiop does not support num_of_id > 0
	 * change FSL_OS_MOD_UART to FSL_OS_MOD_UART_0 */

	err = sys_add_handle(uart, FSL_OS_MOD_UART, 1, pltfrm->param.console_id);
	if (err != 0) {
		pr_err("add handle failed\n");
		return err;
	}

	pltfrm->uart = uart;
	pltfrm->duart_id = pltfrm->param.console_id;

	/*Try to print one character - if failed, disable uart*/
	if(duart_tx(uart, (uint8_t *) "$", 1)) /*Error while trying to print character*/
	{
		return -ENAVAIL;
	}

	return 0;
}

/*****************************************************************************/
__COLD_CODE int platform_disable_console(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;

	if(!pltfrm) {
		pr_crit("Null pointer");
		return -ENODEV;
	}

	/* Unregister platform console */
	/* errCode = SYS_UnregisterConsole();
    if (errCode != 0)
        RETURN_ERROR(MAJOR, errCode, NO_MSG);*/

	if (pltfrm->uart)
	{
		/* Unlock DUART handle in system */
		/*TODO: sys_get_handle in aiop does not support num_of_id > 0
		 * change FSL_OS_MOD_UART to FSL_OS_MOD_UART_0 */
		sys_remove_handle(FSL_OS_MOD_UART, 1, pltfrm->duart_id);

		/* Free DUART driver */
		duart_free(pltfrm->uart);
		pltfrm->uart = NULL;
	}

	return 0;
}

