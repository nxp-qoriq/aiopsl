#include "common/types.h"
#include "common/fsl_string.h"
#include "common/io.h"
#include "kernel/smp.h"
#include "kernel/platform.h"
#include "inc/sys.h"

extern int cmdif_srv_init(void);    extern void cmdif_srv_free(void);
extern int dpni_drv_init(void);     extern void dpni_drv_free(void);
extern int slab_module_init(void);  extern void slab_module_free(void);

extern int dpni_drv_probe(uint16_t	ni_id,
                          uint16_t	mc_portal_id,
                          fsl_handle_t	dpio,
                          fsl_handle_t	dpsp);

extern void build_apps_array(struct sys_module_desc *apps);


#define MEMORY_INFO                                                                                           \
{   /* Region ID                Memory partition ID             Phys. Addr.    Virt. Addr.  Size            */\
    {PLTFRM_MEM_RGN_WS,         MEM_PART_INVALID,               0x00000000,    0x00000000, (2   * KILOBYTE) },\
    {PLTFRM_MEM_RGN_IRAM,       MEM_PART_INVALID,               0x00fe0000,    0x00fe0000, (128 * KILOBYTE) },\
    {PLTFRM_MEM_RGN_MC_PORTALS, MEM_PART_INVALID,               0x80c000000LL, 0x08000000, (64  * MEGABYTE) },\
    {PLTFRM_MEM_RGN_AIOP,       MEM_PART_INVALID,               0x02000000,    0x02000000, (384 * KILOBYTE) },\
    {PLTFRM_MEM_RGN_CCSR,       MEM_PART_INVALID,               0x08000000,    0x0c000000, (1 * MEGABYTE)   },\
    {PLTFRM_MEM_RGN_SHRAM,      MEM_PART_SH_RAM,                0x01000400,    0x01000400, (255 * KILOBYTE) },\
    {PLTFRM_MEM_RGN_DDR1,       MEM_PART_1ST_DDR_NON_CACHEABLE, 0x58000000,    0x58000000, (128 * MEGABYTE) },\
    {PLTFRM_MEM_RGN_PEB,        MEM_PART_PEB,                   0x80000000,    0x80000000, (2 * MEGABYTE)   },\
}

#define GLOBAL_MODULES                     \
{                                          \
    {slab_module_init,  slab_module_free}, \
    {cmdif_srv_init,    cmdif_srv_free},   \
    {dpni_drv_init,     dpni_drv_free},    \
    {NULL, NULL} /* never remove! */       \
}

#define MAX_NUM_OF_APPS		10

int fill_system_parameters(t_sys_param *sys_param);
int global_init(void);
int global_post_init(void);
int run_apps(void);


#include "general.h"
/** Global task params */
__TASK struct aiop_default_task_params default_task_params;


int fill_system_parameters(t_sys_param *sys_param)
{
    struct platform_memory_info mem_info[] = MEMORY_INFO;

#ifndef DEBUG_NO_MC
    { /* TODO - temporary check boot register */
    	uintptr_t   tmp_reg = 0x00000000 + SOC_PERIPH_OFF_MC;
    	/* wait for MC command for boot */
    	while (!(ioread32be(UINT_TO_PTR(tmp_reg + 0x08)) & 0x1)) ;
    }
#endif /* DEBUG_NO_MC */

    sys_param->partition_id = 0;
    sys_param->partition_cores_mask = 0x1;
    sys_param->master_cores_mask = 0x1;
    sys_param->use_cli = 0;
    sys_param->use_ipc = 0;

    sys_param->platform_param->clock_in_freq_hz = 100000000;
    sys_param->platform_param->l1_cache_mode = E_CACHE_MODE_INST_ONLY;
    sys_param->platform_param->console_type = PLTFRM_CONSOLE_NONE;
    sys_param->platform_param->console_id = 0;
    memcpy(sys_param->platform_param->mem_info,
           mem_info,
           sizeof(struct platform_memory_info)*ARRAY_SIZE(mem_info));

    return 0;
}

int global_init(void)
{
    struct sys_module_desc modules[] = GLOBAL_MODULES;
    int                    i;

    for (i=0; i<ARRAY_SIZE(modules); i++)
        if (modules[i].init)
    	    modules[i].init();

    return 0;
}

int global_post_init(void)
{
	uintptr_t   tmp_reg =
	    sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,
	                                      0,
	                                      E_MAPPED_MEM_TYPE_GEN_REGS);

	/* Write AIOP boot status */
	iowrite32be((uint32_t)sys_get_cores_mask(), UINT_TO_PTR(tmp_reg + 0x98));

	return 0;
}

static void core_ready_for_tasks(void) {

    /* finished boot sequence; now wait for event .... */
    fsl_os_print("AIOP completed boot sequence; waiting for events ...\n");
    /* CTSEN = 1, finished boot, Core Task Scheduler Enable */
    booke_set_CTSCSR0(booke_get_CTSCSR0() | CTSCSR_ENABLE);
    asm ("wait  \n");
}

int run_apps(void)
{
    struct sys_module_desc apps[MAX_NUM_OF_APPS];
    int                    i;

	/* TODO - add initialization of global default DP-IO (i.e. call 'dpio_open', 'dpio_init');
	 * This should be mapped to ALL cores of AIOP and to ALL the tasks */
	/* TODO - add initialization of global default DP-SP (i.e. call 'dpsp_open', 'dpsp_init');
	 * This should be mapped to 3 buff-pools with sizes: 128B, 512B, 2KB;
	 * all should be placed in PEB. */
	/* TODO - need to scan the bus in order to retrieve the AIOP "Device list" */
	/* TODO - iterate through the device-list:
	 * call 'dpni_drv_probe(ni_id, mc_portal_id, dpio, dp-sp)'
	 */
    /* in this stage, all the NIC of AIOP are up and running */

    memset(apps, 0, sizeof(apps));
    build_apps_array(apps);

    for (i=0; i<MAX_NUM_OF_APPS; i++)
        if (apps[i].init)
            apps[i].init();

    core_ready_for_tasks();
    return 0;
}
