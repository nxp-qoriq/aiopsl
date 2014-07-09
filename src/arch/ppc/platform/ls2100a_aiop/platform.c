#include "common/types.h"
#include "fsl_malloc.h"
#include "common/fsl_string.h"
#include "aiop_common.h"
#include "fsl_duart.h"
#include "inc/console.h"
#include "platform.h"
#include "ls2085_aiop/fsl_platform.h"
#include "fsl_smp.h"
#include "fsl_io.h"

#include "fsl_mem_mng.h"
#include "inc/fsl_sys.h"

#define __ERR_MODULE__  MODULE_SOC_PLATFORM


typedef struct t_platform_mem_region_info {
    uint64_t    start_addr;
    uint64_t    size;
    uint8_t     mem_ctrl_id;
} t_platform_mem_region_info;

typedef struct t_platform_mem_region_desc {
    e_platform_mem_region      mem_region;
    t_platform_mem_region_info  info;
} t_platform_mem_region_desc;

typedef struct t_sys_to_part_offset_map {
    enum fsl_os_module  module;
    uint32_t            id;
    e_mapped_mem_type   mapped_mem_type;
    uint32_t            offset;
} t_sys_to_part_offset_map;

typedef struct t_platform {
    /* Copy of platform parameters */
    struct platform_param   param;

    /* Platform-owned module handles */
    fsl_handle_t            h_part;

    /* Memory-related variables */
    int                     num_of_mem_parts;
    int                     registered_partitions[PLATFORM_MAX_MEM_INFO_ENTRIES];

    /* Console-related variables */
    fsl_handle_t            uart;
    uint32_t                duart_id;

    uintptr_t               aiop_base;
    uintptr_t               ccsr_base;
    uintptr_t               mc_portals_base;
} t_platform;


/* Module names for debug messages */
const char *module_strings[] = {
     "???"                      /* MODULE_UNKNOWN */
    ,"MEM"                      /* MODULE_MEM */
    ,"MM"                       /* MODULE_MM */
    ,"CORE"                     /* MODULE_CORE */
    ,"SoC"                      /* MODULE_LS2100A */
    ,"SoC-platform"             /* MODULE_LS2100A_PLATFORM */
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
extern __TASK uint32_t seed_32bit;

/*****************************************************************************/
static void print_platform_info(t_platform *pltfrm)
{
    char        buf[256];
    int         count = 0;
    uint32_t    sys_clock;
    int         is_master_core = sys_is_master_core();

    ASSERT_COND(pltfrm);

    if (is_master_core)
    {
        /*------------------------------------------*/
        /* Device enable/disable and status display */
        /* Power Save mode                          */
        /*------------------------------------------*/
        count += sprintf((char *)&buf[count], "\nfreescale ");

/*
        switch (ls2100a_get_rev_info(pltfrm->ccsr_base + SOC_PERIPH_OFFSET_GUTIL))
        {
            case E_LS2100A_REV_1_0:
                count += sprintf((char *)&buf[count], "LS2100A revision 1.0");
                break;
            default:
                count += sprintf((char *)&buf[count], "unknown chip revision! ");
                break;
        }
*/
        count += sprintf((char *)&buf[count], "LST4-Sim");

        count += sprintf((char *)&buf[count], "\n\nclocks: [IN:%d.%d] ",
                         (pltfrm->param.clock_in_freq_hz / 1000000),
                         ((pltfrm->param.clock_in_freq_hz % 1000000) / 100000));

        sys_clock = platform_get_system_bus_clk(pltfrm);
        count += sprintf((char *)&buf[count], "[SYS BUS:%d.%d] ",
                         (sys_clock / 1000000), ((sys_clock % 1000000) / 100000));

/*
        clock = platform_get_local_bus_clk(pltfrm);
        divider = sys_clock/clock;
        count += sprintf((char *)&buf[count], "[LBC:%d.%d] ",
                         (sys_clock /divider/1000000), ((sys_clock /divider % 1000000) / 100000));

        clock = platform_get_ddr_clk(pltfrm);
        count += sprintf((char *)&buf[count], "[DDR:%d.%d] ",
                         (clock / 1000000), ((clock % 1000000) / 100000));

        count += sprintf((char *)&buf[count], "\ncores info:");
*/

        fsl_os_print(buf);
        count = 0;
        memset(buf, 0, sizeof(buf));
    }
    sys_barrier();

/*
    clock = platform_get_core_clk_local(pltfrm,core_get_id());
    count += sprintf((char *)&buf[count], "\n[CORE %d:%d.%d] ",
                     core_get_id(),(clock / 1000000), ((clock % 1000000) / 100000));

    fsl_os_print(buf);
    count = 0;
    memset(buf, 0, sizeof(buf));
    sys_barrier();
*/

    if (!is_master_core)
        return;

    /*------------------------------------------*/
    /*      CACHE/MMU Status display            */
    /*------------------------------------------*/
    count = 0;
    memset(buf, 0, sizeof(buf));

    count += sprintf((char *)&buf[count], "\n\nPlatform status: ");
    count += sprintf((char *)&buf[count], "\nATU ICACHE TB\n");
#ifdef SIMULATOR
    /* TODO - ATU is temporary off!!! */
    count += sprintf((char *)&buf[count], "OFF ");
#else
    /* MMU is always ON */
    count += sprintf((char *)&buf[count], "ON  ");
#endif /* SIMULATOR */

    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY)
        count += sprintf((char *)&buf[count], "ON     ");
    else
        count += sprintf((char *)&buf[count], "OFF    ");

#ifdef SIMULATOR
    /* TODO - time-base is temporary off!!! */
    count += sprintf((char *)&buf[count], "OFF ");
#else
    /* Time-Base is always ON */
    count += sprintf((char *)&buf[count], "ON   ");
#endif /* SIMULATOR */

    count += sprintf((char *)&buf[count], "\n");
    fsl_os_print(buf);
}

/*****************************************************************************/
static int find_mem_region_index(t_platform_memory_info  *mem_info,
                                 e_platform_mem_region   mem_region)
{
    int i;

    ASSERT_COND(mem_info);

    for (i = 0; i < PLATFORM_MAX_MEM_INFO_ENTRIES; i++) {
        if (mem_info[i].size == 0)
            break;

        if ((mem_info[i].mem_region_id == mem_region) &&
            (mem_info[i].mem_partition_id == MEM_PART_INVALID))
            /* Found requested memory region */
            return i;
    }

    /* Not found */
    return -1;
}

/*****************************************************************************/
static int init_l1_cache(t_platform *pltfrm)
{
    ASSERT_COND(pltfrm);

    /* L1 Cache Init */
    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY) {
        booke_icache_enable();
    }

    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_DATA_ONLY)
        RETURN_ERROR(MAJOR, E_NOT_SUPPORTED, NO_MSG);

    return E_OK;
}

/*****************************************************************************/
static int disable_l1_cache(t_platform *pltfrm)
{
    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY)
        booke_icache_disable();

    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_DATA_ONLY)
        RETURN_ERROR(MAJOR, E_NOT_SUPPORTED, NO_MSG);

    return E_OK;
}


/*****************************************************************************/
static int console_print_cb(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size)
{
    int err;

    err = duart_tx(h_console_dev, p_data, size);
    if (err != E_OK)
        return 0;

    return (int)size;
}

/*****************************************************************************/
static int console_get_line_cb(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size)
{
    uint32_t count;

    count = duart_rx(h_console_dev, p_data, size);

    return (int)count;
}
#ifdef ARENA_LEGACY_CODE
/*****************************************************************************/
static void pltfrm_enable_local_irq_cb(fsl_handle_t h_platform)
{
    UNUSED(h_platform);

    msr_enable_ee();
    msr_enable_me();
    msr_enable_ce();
}

/*****************************************************************************/
static void pltfrm_disable_local_irq_cb(fsl_handle_t h_platform)
{
    UNUSED(h_platform);

    msr_disable_ee();
    msr_disable_me();
    msr_disable_ce();
}
#endif

/*****************************************************************************/
static int init_random_seed(uint32_t num_of_tasks)
{
	uint32_t *seed_mem_ptr = NULL;
	uint32_t core_and_task_id = 0;
	uint32_t seed = 0;
	uint32_t task_stack_size = 0;
	uint32_t sum_stack = 0;

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

	seed_32bit = (core_and_task_id << 16) | core_and_task_id;
	/*seed for task 0 is already allocated*/
	for (i = 0; i < num_of_tasks - 1; i ++)
	{
		sum_stack += task_stack_size; /*size of each task area*/
		seed_mem_ptr = &(seed_32bit) + sum_stack;
		core_and_task_id ++; /*increment the task id accordingly to its tls section*/
		seed = (core_and_task_id << 16) | core_and_task_id;
		iowrite32be(seed, seed_mem_ptr);
	}

	return 0;

}
/*****************************************************************************/
static int pltfrm_init_core_cb(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;
    int     err = 0, i = 0;
    uint32_t CTSCSR_value = 0;
    uint32_t WSCR_tasks_bit = 0;
    struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *) 
	                               sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);    
    
    if (pltfrm == NULL) {
	    return -EINVAL;
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
    WSCR_tasks_bit = LOAD_LE32_TO_CPU(&aiop_regs->cmgw_regs.wscr) & 0x000000ff;
    
    CTSCSR_value = (booke_get_CTSCSR0() & ~CTSCSR_TASKS_MASK) | \
    		                          (WSCR_tasks_bit << 24);

    booke_set_CTSCSR0(CTSCSR_value);

    /*------------------------------------------------------*/
    /* Initialize L1 Cache                                  */
    /*------------------------------------------------------*/
    err = init_l1_cache(pltfrm);
    if (err != E_OK)
	    RETURN_ERROR(MAJOR, err, NO_MSG);

    /*initialize random seeds*/
    err = init_random_seed(WSCR_tasks_bit);

    if (err != E_OK)
    	    RETURN_ERROR(MAJOR, err, NO_MSG);

    return E_OK;
}
/*****************************************************************************/
static int pltfrm_free_core_cb(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    ASSERT_COND(pltfrm);

    /* Disable L1 cache */
    disable_l1_cache(pltfrm);

    return E_OK;
}


/*****************************************************************************/
static int pltfrm_init_console_cb(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;
    int     err;

    ASSERT_COND(pltfrm);

    if (sys_is_master_core()) {
        /* Master partition - register DUART console */
        err = platform_enable_console(pltfrm);
        if (err != E_OK)
            RETURN_ERROR(MAJOR, err, NO_MSG);

        err = sys_register_console(pltfrm->uart, console_print_cb, console_get_line_cb);
        if (err != E_OK)
            RETURN_ERROR(MAJOR, err, NO_MSG);
    }

    return E_OK;
}

/*****************************************************************************/
static int pltfrm_free_console_cb(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    ASSERT_COND(pltfrm);

    if (sys_is_master_core())
        platform_disable_console(pltfrm);

    sys_unregister_console();

    return E_OK;
}

/*****************************************************************************/
static int pltfrm_init_mem_partitions_cb(fsl_handle_t h_platform)
{
    t_platform              *pltfrm = (t_platform *)h_platform;
    t_platform_memory_info  *p_mem_info;
    int                     err;
    uintptr_t               virt_base_addr;
    uint64_t                size;
    uint32_t                attributes;
    int                     i, register_partition, index = 0;
    char                    name[32];

    ASSERT_COND(pltfrm);

//    if (pltfrm->param.user_init_hooks.f_init_memory_partitions)
//        return pltfrm->param.user_init_hooks.f_init_memory_partitions(pltfrm);

    for (i = 0; i < pltfrm->num_of_mem_parts; i++) {
        p_mem_info = &pltfrm->param.mem_info[i];
        virt_base_addr = p_mem_info->virt_base_addr;
        size = p_mem_info->size;
        attributes = 0;
        register_partition = 0;
        memset(name, 0, sizeof(name));

        switch (p_mem_info->mem_partition_id) {
        case MEM_PART_DP_DDR:
            sprintf(name, "%s", "DP_DDR");
            register_partition = 1;
            break;
            /*
        case MEM_PART_2ND_DDR_NON_CACHEABLE:
            sprintf(name, "%s", "DDR #2 (AIOP) non cacheable");
            register_partition = 1;
            break;
            */
        case MEM_PART_SH_RAM:
            sprintf(name, "%s", "Shared-SRAM");
            register_partition = 1;
            break;
        case MEM_PART_PEB:
            sprintf(name, "%s", "PEB");
            register_partition = 1;
            break;
        default:
            break;
        }

        err = sys_register_virt_mem_mapping(p_mem_info->virt_base_addr,
                                            p_mem_info->phys_base_addr,
                                            p_mem_info->size);
        if (err != E_OK)
            RETURN_ERROR(MAJOR, err, NO_MSG);

        if (register_partition) {
            err = sys_register_mem_partition(p_mem_info->mem_partition_id,
                                             virt_base_addr,
                                             size,
                                             attributes,
                                             name,
                                             NULL,
                                             NULL,
#ifdef DEBUG
                                             1
#else
                                             0
#endif /* DEBUG */
                                             );
            if (err != E_OK)
                RETURN_ERROR(MAJOR, err, NO_MSG);

            pltfrm->registered_partitions[index++] = p_mem_info->mem_partition_id;
        }
    }

    return E_OK;
}

/*****************************************************************************/
static int pltfrm_free_mem_partitions_cb(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;
    int         index;

    ASSERT_COND(pltfrm);

    index = (int)pltfrm->num_of_mem_parts;

    while ((--index) >= 0) {
        sys_unregister_virt_mem_mapping(pltfrm->param.mem_info[index].virt_base_addr);

        if (pltfrm->registered_partitions[index])
            sys_unregister_mem_partition(pltfrm->registered_partitions[index]);
    }

    return E_OK;
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

    return E_OK;
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

    return E_OK;
}
#endif

/*****************************************************************************/
int platform_early_init(struct platform_param *pltfrm_params)
{
    UNUSED(pltfrm_params);
    return E_OK;
}

int platform_init(struct platform_param    *pltfrm_param,
                  t_platform_ops           *pltfrm_ops)
{
    t_platform      *pltfrm;
    int             i, mem_index;

    SANITY_CHECK_RETURN_ERROR(pltfrm_param, ENODEV);
    SANITY_CHECK_RETURN_ERROR(pltfrm_ops, ENODEV);

    /* Allocate the platform's control structure */
    pltfrm = fsl_os_malloc(sizeof(t_platform));
    if (!pltfrm)
        RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("platform object"));
    memset(pltfrm, 0, sizeof(t_platform));

    /* Store configuration parameters */
    memcpy(&(pltfrm->param), pltfrm_param, sizeof(struct platform_param));

    /* Count number of valid memory partitions and check that
       user's partition definition is within actual physical
       addresses range. */
    for (i=0; i<PLATFORM_MAX_MEM_INFO_ENTRIES; i++) {
        /* t_platform_mem_region_info  mem_region_info; */
        t_platform_memory_info      *mem_info;

        mem_info = pltfrm->param.mem_info + i;
        if (!mem_info->size)
            break;
    }
    pltfrm->num_of_mem_parts = i;

#if 0 /*TODO Do we need this function???*/
    /* Identify the program memory */
    err = identify_program_memory(pltfrm->param.mem_info,
                                  &(pltfrm->prog_runs_from));
    ASSERT_COND(err == E_OK);
#endif
    /* Store CCSR base (for convenience) */
    mem_index = find_mem_region_index(pltfrm->param.mem_info, PLTFRM_MEM_RGN_CCSR);
    ASSERT_COND(mem_index != -1);
    pltfrm->ccsr_base = pltfrm->param.mem_info[mem_index].virt_base_addr;

    /* Store AIOP-peripherals base (for convenience) */
    mem_index = find_mem_region_index(pltfrm->param.mem_info, PLTFRM_MEM_RGN_AIOP);
    ASSERT_COND(mem_index != -1);
    pltfrm->aiop_base = pltfrm->param.mem_info[mem_index].virt_base_addr;

    /* Store MC-Portals bases (for convenience) */
    mem_index = find_mem_region_index(pltfrm->param.mem_info, PLTFRM_MEM_RGN_MC_PORTALS);
    if (mem_index != -1)
    	pltfrm->mc_portals_base = pltfrm->param.mem_info[mem_index].virt_base_addr;

    /* Initialize platform operations structure */
    pltfrm_ops->h_platform              = pltfrm;
    pltfrm_ops->f_init_core             = pltfrm_init_core_cb;
    pltfrm_ops->f_free_core             = pltfrm_free_core_cb;
    pltfrm_ops->f_init_intr_ctrl        = NULL;
    pltfrm_ops->f_free_intr_ctrl        = NULL;
    pltfrm_ops->f_init_soc              = NULL;
    pltfrm_ops->f_free_soc              = NULL;
    pltfrm_ops->f_init_timer            = NULL;
    pltfrm_ops->f_free_timer            = NULL;
    pltfrm_ops->f_init_ipc              = NULL;
    pltfrm_ops->f_free_ipc              = NULL;

    pltfrm_ops->f_init_console          = pltfrm_init_console_cb;
    pltfrm_ops->f_free_console          = pltfrm_free_console_cb;

    pltfrm_ops->f_init_mem_partitions   = pltfrm_init_mem_partitions_cb;
    pltfrm_ops->f_free_mem_partitions   = pltfrm_free_mem_partitions_cb;
#ifdef ARENA_LEGACY_CODE
    pltfrm_ops->f_init_private          = pltfrm_init_private_cb;
    pltfrm_ops->f_free_private          = pltfrm_free_private_cb;
#else
    pltfrm_ops->f_init_private          = NULL;
    pltfrm_ops->f_free_private          = NULL;
#endif
    pltfrm_ops->f_enable_cores          = NULL;
#ifdef ARENA_LEGACY_CODE
    pltfrm_ops->f_enable_local_irq      = pltfrm_enable_local_irq_cb;
    pltfrm_ops->f_disable_local_irq     = pltfrm_disable_local_irq_cb;
#else
    pltfrm_ops->f_enable_local_irq      = NULL;
    pltfrm_ops->f_disable_local_irq     = NULL;
#endif

    return 0;
}

/*****************************************************************************/
int platform_free(fsl_handle_t h_platform)
{
    if (h_platform)
        fsl_os_free(h_platform);

    return E_OK;
}

/*****************************************************************************/
uintptr_t platform_get_memory_mapped_module_base(fsl_handle_t        h_platform,
                                             enum fsl_os_module     module,
                                             uint32_t               id,
                                             e_mapped_mem_type mapped_mem_type)
{
    t_platform  *pltfrm = (t_platform *)h_platform;
    int         i;

    t_sys_to_part_offset_map part_offset_map[] =
    {
        /* module                     id   mappedMemType                  offset
           ------                     --   -------------                  ------                      */
        { FSL_OS_MOD_UART,            0,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART1       },
        { FSL_OS_MOD_UART,            1,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART2       },
        { FSL_OS_MOD_UART,            2,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART3       },
        { FSL_OS_MOD_UART,            3,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART4       },
        { FSL_OS_MOD_CMGW,            0,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_AIOP_TILE+SOC_PERIPH_OFF_AIOP_CMGW},
    };

    SANITY_CHECK_RETURN_VALUE(pltfrm, ENODEV, 0);

    if (module == FSL_OS_MOD_MC_PORTAL)
        return (uintptr_t)(pltfrm->mc_portals_base + SOC_PERIPH_OFF_PORTALS_MC(id));
    if (module == FSL_OS_MOD_CMGW) {
        for (i = 0; i < ARRAY_SIZE(part_offset_map); i++)
            if ((part_offset_map[i].module        == module)  &&
                (part_offset_map[i].id            == id)      &&
                (part_offset_map[i].mapped_mem_type == mapped_mem_type))
                return (uintptr_t)(pltfrm->aiop_base + part_offset_map[i].offset);
    } else {
        for (i = 0; i < ARRAY_SIZE(part_offset_map); i++)
        	if ((part_offset_map[i].module        == module)  &&
                (part_offset_map[i].id            == id)      &&
                (part_offset_map[i].mapped_mem_type == mapped_mem_type))
                return (uintptr_t)(pltfrm->ccsr_base + part_offset_map[i].offset);
    }

    REPORT_ERROR(MAJOR, E_NOT_FOUND, ("module base"));
    return 0;
}

/*****************************************************************************/
uint32_t platform_get_system_bus_clk(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    SANITY_CHECK_RETURN_VALUE(pltfrm, ENODEV, 0);

    return (pltfrm->param.clock_in_freq_hz * 4);
}


/*****************************************************************************/
int platform_enable_console(fsl_handle_t h_platform)
{
    t_platform          *pltfrm = (t_platform *)h_platform;
    t_duart_uart_param  duart_uart_param;
    fsl_handle_t        uart;
    int           err = E_OK;

    SANITY_CHECK_RETURN_ERROR(pltfrm, ENODEV);

    if (pltfrm->param.console_type == PLTFRM_CONSOLE_NONE)
        return E_OK;

    SANITY_CHECK_RETURN_ERROR((pltfrm->param.console_type == PLTFRM_CONSOLE_DUART), E_NOT_SUPPORTED);

    /* Fill DUART configuration parameters */
    /*TODO: the base address is hard coded to uart 0, should be modified*/
    duart_uart_param.base_address       = SOC_PERIPH_OFF_DUART1;
    duart_uart_param.system_clock_mhz   = (platform_get_system_bus_clk(pltfrm) / 1000000);
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
    if (!uart)
        RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("DUART"));

    /* Configure polling mode */
    err = duart_config_poll_mode(uart, 1);
    if (err != E_OK)
        RETURN_ERROR(MAJOR, err, NO_MSG);

    /* Convert end-of-line indicators */
    err = duart_config_poll_lf2crlf(uart, 1);
    if (err != E_OK)
        RETURN_ERROR(MAJOR, err, NO_MSG);

    /* Prevent blocking */
    err = duart_config_rx_timeout(uart, 0);
    if (err != E_OK)
        RETURN_ERROR(MAJOR, err, NO_MSG);

    err = duart_init(uart);
    if (err != E_OK)
        RETURN_ERROR(MAJOR, err, NO_MSG);

    /* Lock DUART handle in system */
    /*TODO: sys_get_handle in aiop does not support num_of_id > 0
     * change FSL_OS_MOD_UART to FSL_OS_MOD_UART_0 */
    err = sys_add_handle(uart, FSL_OS_MOD_UART, 1, pltfrm->param.console_id);
    if (err != E_OK)
        RETURN_ERROR(MAJOR, err, NO_MSG);

    pltfrm->uart = uart;
    pltfrm->duart_id = pltfrm->param.console_id;

    return E_OK;
}

/*****************************************************************************/
int platform_disable_console(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    SANITY_CHECK_RETURN_ERROR(pltfrm, ENODEV);

    /* Unregister platform console */
   /* errCode = SYS_UnregisterConsole();
    if (errCode != E_OK)
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

    return E_OK;
}

