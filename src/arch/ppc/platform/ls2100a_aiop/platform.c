#include "common/types.h"
#include "common/fsl_malloc.h"
#include "common/fsl_string.h"
#include "drivers/fsl_duart.h"
#include "kernel/console.h"
#include "kernel/platform.h"
#include "kernel/timer.h"
#include "kernel/smp.h"

#include "inc/mem_mng.h"
#include "inc/sys.h"


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
    e_memory_partition_id   prog_runs_from;
    uint32_t                num_of_mem_info_entries;
    int                     registered_mem_partitions[PLATFORM_MAX_MEM_INFO_ENTRIES];

    /* Console-related variables */
    fsl_handle_t            uart;
    uint32_t                duart_id;

    /* Multi-core/multi-partition related settings */
    uint8_t                 partition_id;

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


/*****************************************************************************/
static void platform_print_info(t_platform *p_platform)
{
    char        buf[256];
    int         count = 0;
    uint32_t    sys_clock;
    int         is_master_core = sys_is_master_core();
    int         is_master_partition_master;    /* Master of master partition responsible
                                               for single resources initialization */

    is_master_partition_master = (int)(is_master_core && (sys_get_partition_id() == ARENA_MASTER_PART_ID));

    ASSERT_COND(p_platform);

    if (is_master_partition_master)
    {
        /*------------------------------------------*/
        /* Device enable/disable and status display */
        /* Power Save mode                          */
        /*------------------------------------------*/
        count += sprintf((char *)&buf[count], "\nfreescale ");

/*
        switch (ls2100a_get_rev_info(p_platform->ccsr_base + SOC_PERIPH_OFFSET_GUTIL))
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
                         (p_platform->param.clock_in_freq_hz / 1000000),
                         ((p_platform->param.clock_in_freq_hz % 1000000) / 100000));

        sys_clock = platform_get_system_bus_clk(p_platform);
        count += sprintf((char *)&buf[count], "[SYS BUS:%d.%d] ",
                         (sys_clock / 1000000), ((sys_clock % 1000000) / 100000));

/*
        clock = platform_get_local_bus_clk(p_platform);
        divider = sys_clock/clock;
        count += sprintf((char *)&buf[count], "[LBC:%d.%d] ",
                         (sys_clock /divider/1000000), ((sys_clock /divider % 1000000) / 100000));

        clock = platform_get_ddr_clk(p_platform);
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
    clock = platform_get_core_clk_local(p_platform,core_get_id());
    count += sprintf((char *)&buf[count], "\n[CORE %d:%d.%d] ",
                     core_get_id(),(clock / 1000000), ((clock % 1000000) / 100000));

    fsl_os_print(buf);
    count = 0;
    memset(buf, 0, sizeof(buf));
    sys_barrier();
*/

    if (!is_master_partition_master)
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

    if (p_platform->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY)
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
static int platform_identify_program_memory(t_platform_memory_info   *p_mem_info,
                                            e_memory_partition_id    *p_mem_part_id)
{
    uint64_t    running_address = PTR_TO_UINT(platform_identify_program_memory);
    int         i;

    /* NOTE:
       We assume that the program is running from one of the partitions in the table */
    for (i = 0; i < PLATFORM_MAX_MEM_INFO_ENTRIES; i++)
    {
        if (p_mem_info[i].size == 0)
            break;

        if ((running_address >= p_mem_info[i].virt_base_addr) &&
            (running_address <  p_mem_info[i].virt_base_addr + p_mem_info[i].size))
        {
            *p_mem_part_id = (e_memory_partition_id)p_mem_info[i].mem_partition_id;
            return E_OK;
        }
    }

    /* Not found - should not reach here ! */
    RETURN_ERROR(MAJOR, E_NOT_FOUND, NO_MSG);
}

/*****************************************************************************/
static int platform_find_mem_region_index(t_platform_memory_info  *mem_info,
                                          e_platform_mem_region   mem_region)
{
    int i;

    ASSERT_COND(mem_info);

    for (i = 0; i < PLATFORM_MAX_MEM_INFO_ENTRIES; i++) {
        if (mem_info[i].size == 0)
            break;

        if ((mem_info[i].mem_region_id == mem_region) &&
            (mem_info[i].mem_partition_id == E_MEM_INVALID))
            /* Found requested memory region */
            return i;
    }

    /* Not found */
    return -1;
}

/*****************************************************************************/
static int platform_init_l1_cache(t_platform *pltfrm)
{
    ASSERT_COND(pltfrm);

    /* L1 Cache Init */
    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY) {
        booke_icache_enable();
        booke_icache_flush();
    }

    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_DATA_ONLY)
        RETURN_ERROR(MAJOR, E_NOT_SUPPORTED, NO_MSG);

    return E_OK;
}

/*****************************************************************************/
static int platform_disable_l1_cache(t_platform *pltfrm)
{
    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_INST_ONLY)
        booke_icache_disable();

    if (pltfrm->param.l1_cache_mode & E_CACHE_MODE_DATA_ONLY)
        RETURN_ERROR(MAJOR, E_NOT_SUPPORTED, NO_MSG);

    return E_OK;
}

/*****************************************************************************/
static void platform_enable_local_irq(fsl_handle_t h_platform)
{
    UNUSED(h_platform);

    msr_enable_ee();
    msr_enable_me();
    msr_enable_ce();
}

/*****************************************************************************/
static void platform_disable_local_irq(fsl_handle_t h_platform)
{
    UNUSED(h_platform);

    msr_disable_ee();
    msr_disable_me();
    msr_disable_ce();
}

/*****************************************************************************/
static int platform_init_core(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;
    int     err;

    ASSERT_COND(pltfrm);

    booke_disable_time_base();
    booke_address_broadcast_enable();
    booke_address_bus_streaming_enable();

    /*------------------------------------------------------*/
    /* Initialize PPC interrupts vector                     */
    /*------------------------------------------------------*/
    //booke_generic_irq_init();

#ifndef DEBUG
    /* Enable the BTB - branches predictor */
    booke_set_spr_BUCSR(booke_get_spr_BUCSR() | 0x00000201);
#endif /* DEBUG */

#if 0 /* TODO - complete! */
    /*------------------------------------------------------*/
    /* Initialize MMU                                       */
    /*------------------------------------------------------*/
    if (pltfrm->param.user_init_hooks.f_init_mmu)
        err = pltfrm->param.user_init_hooks.f_init_mmu(pltfrm);
    else
        err = platform_init_mmu(pltfrm);

    if (err != E_OK)
        RETURN_ERROR(MAJOR, err, NO_MSG);
#endif /* 0 */

    /*------------------------------------------------------*/
    /* Initialize L1 Cache                                  */
    /*------------------------------------------------------*/
    err = platform_init_l1_cache(pltfrm);
    if (err != E_OK)
        RETURN_ERROR(MAJOR, err, NO_MSG);

    return E_OK;
}

/*****************************************************************************/
static int platform_free_core(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    ASSERT_COND(pltfrm);

    /* Disable L1 cache */
    platform_disable_l1_cache(pltfrm);

    return E_OK;
}

/*****************************************************************************/
int platform_enable_console(fsl_handle_t h_platform)
{
	t_platform          *pltfrm = (t_platform *)h_platform;
    t_duart_uart_param  duart_uart_param;
    fsl_handle_t        uart;
    int           err = E_OK;

    SANITY_CHECK_RETURN_ERROR(pltfrm, ENODEV);

    if (pltfrm->param.console_type == E_PLATFORM_CONSOLE_NONE)
        return E_OK;

    SANITY_CHECK_RETURN_ERROR((pltfrm->param.console_type == E_PLATFORM_CONSOLE_DUART), E_NOT_SUPPORTED);

    /* Fill DUART configuration parameters */
    duart_uart_param.base_address       = platform_get_memory_mapped_module_base(pltfrm, FSL_OS_MOD_UART, pltfrm->param.console_id, E_MAPPED_MEM_TYPE_GEN_REGS);
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
        sys_remove_handle(FSL_OS_MOD_UART, pltfrm->duart_id);

        /* Free DUART driver */
        duart_free(pltfrm->uart);
        pltfrm->uart = NULL;
    }

    return E_OK;
}

/*****************************************************************************/
static int platform_console_print(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size)
{
    int err;

    err = duart_tx(h_console_dev, p_data, size);
    if (err != E_OK)
        return 0;

    return (int)size;
}

/*****************************************************************************/
static int platform_console_get(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size)
{
    uint32_t count;

    count = duart_rx(h_console_dev, p_data, size);

    return (int)count;
}

/*****************************************************************************/
static int platform_init_console(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;
    int     err;

    ASSERT_COND(pltfrm);

    if (pltfrm->partition_id == ARENA_MASTER_PART_ID)
    {
        /* Master partition - register DUART console */
        err = platform_enable_console(pltfrm);
        if (err != E_OK)
            RETURN_ERROR(MAJOR, err, NO_MSG);

        err = sys_register_console(pltfrm->uart, platform_console_print, platform_console_get);
        if (err != E_OK)
            RETURN_ERROR(MAJOR, err, NO_MSG);
    }

    return E_OK;
}

/*****************************************************************************/
static int platform_free_console(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    ASSERT_COND(pltfrm);

    if (pltfrm->partition_id == ARENA_MASTER_PART_ID)
        platform_disable_console(pltfrm);

    sys_unregister_console();

    return E_OK;
}

/*****************************************************************************/
static int platform_init_private(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    ASSERT_COND(pltfrm);

    if (sys_is_master_core() && (pltfrm->partition_id == ARENA_MASTER_PART_ID))
    {
#if 0
    	/* Register Platform CLI commands */
        if (fsl_os_cli_is_available())
            platform_register_cli_commands(pltfrm);
#endif /* 0 */
    }

    /* Print platform information */
    platform_print_info(pltfrm);

    return E_OK;
}

/*****************************************************************************/
static int platform_free_private(fsl_handle_t h_platform)
{
    t_platform  *pltfrm = (t_platform *)h_platform;

    ASSERT_COND(pltfrm);

    if (sys_is_master_core() && (pltfrm->partition_id == ARENA_MASTER_PART_ID))
    {
#if 0
    	/* Unregister Platform CLI commands */
        if (fsl_os_cli_is_available())
            platform_unregister_cli_commands(pltfrm);
#endif /* 0 */
    }

    return E_OK;
}


/*****************************************************************************/
int platform_early_init(struct platform_param *p_platform_params)
{
    /* TODO - complete! */
UNUSED(p_platform_params);
    return E_OK;
}

int platform_init(struct platform_param    *p_platform_param,
                  t_platform_ops           *p_platform_ops)
{
    t_platform  *pltfrm;
    int       	err;
    //uint32_t  i;
    int         mem_index;

    SANITY_CHECK_RETURN_ERROR(p_platform_param, ENODEV);
    SANITY_CHECK_RETURN_ERROR(p_platform_ops, ENODEV);

    /* Allocate the platform's control structure */
    pltfrm = fsl_os_malloc(sizeof(t_platform));
    if (!pltfrm)
        RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("platform object"));
    memset(pltfrm, 0, sizeof(t_platform));

    /* Store configuration parameters */
    memcpy(&(pltfrm->param), p_platform_param, sizeof(struct platform_param));

#if 0 /* TODO - complete! */
    /* Count number of valid memory partitions and check that
       user's partition definition is within actual physical
       addresses range. */
    for (i=0; i<PLATFORM_MAX_MEM_INFO_ENTRIES; i++) {
        t_platform_mem_region_info  mem_region_info;
        t_platform_memory_info      *mem_info;

        mem_info = pltfrm->param.mem_info + i;
        if (!mem_info->size)
            break;

        /* Check the range  - only verify start address fits into the real region bounds,
           don't check the size. It is possible to define region of larger than actual size
           to save number of MMU TLB entries. */
        err = platform_get_mem_region_info((e_platform_mem_region)mem_info->mem_region_id, &mem_region_info);
        ASSERT_COND(err == E_OK);
        ASSERT_COND(mem_info->phys_base_addr >= mem_region_info.start_addr);
    }
    pltfrm->num_of_mem_info_entries = i;
#endif /* 0 */

    pltfrm->partition_id = sys_get_partition_id();

    /* Identify the program memory */
    err = platform_identify_program_memory(pltfrm->param.mem_info,
                                           &(pltfrm->prog_runs_from));
    ASSERT_COND(err == E_OK);

    /* Store CCSR base (for convenience) */
    mem_index = platform_find_mem_region_index(pltfrm->param.mem_info, E_PLATFORM_MEM_RGN_CCSR);
    ASSERT_COND(mem_index != -1);
    pltfrm->ccsr_base = pltfrm->param.mem_info[mem_index].virt_base_addr;

    /* Store BM, FM portals bases (for convenience) */
    mem_index = platform_find_mem_region_index(pltfrm->param.mem_info, E_PLATFORM_MEM_RGN_MC_PORTALS);
    if (mem_index != -1)
    	pltfrm->mc_portals_base = pltfrm->param.mem_info[mem_index].virt_base_addr;

    /* Initialize platform operations structure */
    p_platform_ops->h_platform              = pltfrm;
    p_platform_ops->f_init_core             = platform_init_core;
    p_platform_ops->f_free_core             = platform_free_core;
    p_platform_ops->f_init_intr_ctrl        = NULL;
    p_platform_ops->f_free_intr_ctrl        = NULL;
    p_platform_ops->f_init_soc              = NULL;
    p_platform_ops->f_free_soc              = NULL;
    p_platform_ops->f_init_timer            = NULL;
    p_platform_ops->f_free_timer            = NULL;
    p_platform_ops->f_init_ipc              = NULL;
    p_platform_ops->f_free_ipc              = NULL;
    p_platform_ops->f_init_console          = platform_init_console;
    p_platform_ops->f_free_console          = platform_free_console;
    p_platform_ops->f_init_mem_partitions   = NULL;
    p_platform_ops->f_free_mem_partitions   = NULL;
    p_platform_ops->f_init_private          = platform_init_private;
    p_platform_ops->f_free_private          = platform_free_private;

    p_platform_ops->f_enable_cores          = NULL;
    p_platform_ops->f_enable_local_irq      = platform_enable_local_irq;
    p_platform_ops->f_disable_local_irq     = platform_disable_local_irq;

    return E_OK;
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
        /* module                           id   mappedMemType                  offset
           ------                           --   -------------                  ------                      */
        { FSL_OS_MOD_UART,            0,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART1       },
        { FSL_OS_MOD_UART,            1,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART2       },
        { FSL_OS_MOD_UART,            2,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART3       },
        { FSL_OS_MOD_UART,            3,  E_MAPPED_MEM_TYPE_GEN_REGS,     SOC_PERIPH_OFF_DUART4       },
    };

    SANITY_CHECK_RETURN_VALUE(pltfrm, ENODEV, 0);

    if (module == FSL_OS_MOD_MC_PORTAL)
        return (uintptr_t)(pltfrm->mc_portals_base + SOC_PERIPH_OFF_PORTALS_MC(id));

    for (i = 0; i < ARRAY_SIZE(part_offset_map); i++)
        if ((part_offset_map[i].module        == module)  &&
            (part_offset_map[i].id            == id)      &&
            (part_offset_map[i].mapped_mem_type == mapped_mem_type))
            return (uintptr_t)(pltfrm->ccsr_base + part_offset_map[i].offset);

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
