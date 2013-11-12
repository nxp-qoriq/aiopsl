#include "common/types.h"
#include "common/fsl_string.h"
#include "common/io.h"
#include "kernel/smp.h"
#include "kernel/platform.h"
#include "inc/sys.h"


extern int dpni_drv_init(void);extern void dpni_drv_free(void);

extern void build_apps_array(struct sys_module_desc *apps);


#define MEMORY_INFO                                                                                            \
{   /* Region ID                    Memory partition ID          Phys. Addr.    Virt. Addr.  Size            */\
    {E_PLATFORM_MEM_RGN_WS,         E_MEM_INVALID,               0x00000000,    0x00000000, (2   * KILOBYTE) },\
    {E_PLATFORM_MEM_RGN_IRAM,       E_MEM_INVALID,               0x00800000,    0x00800000, (32  * KILOBYTE) },\
    {E_PLATFORM_MEM_RGN_SHRAM,      E_MEM_INT_RAM,               0x01000000,    0x01000000, (256 * KILOBYTE) },\
    {E_PLATFORM_MEM_RGN_DDR1,       E_MEM_1ST_DDR_NON_CACHEABLE, 0x40000000,    0x40000000, (128 * MEGABYTE) },\
    {E_PLATFORM_MEM_RGN_CCSR,       E_MEM_INVALID,               0xfe000000,    0xfe000000, (16  * MEGABYTE) },\
    {E_PLATFORM_MEM_RGN_MC_PORTALS, E_MEM_INVALID,               0x08000000,    0x08000000, (32  * MEGABYTE) },\
}

#define GLOBAL_MODULES                  \
{                                       \
    {dpni_drv_init, dpni_drv_free},     \
    {NULL, NULL} /* never remove! */    \
}

#define MAX_NUM_OF_APPS		10


int fill_system_parameters(t_sys_param *sys_param);
int global_init(void);
int global_post_init(void);
int run_apps(void);


int fill_system_parameters(t_sys_param *sys_param)
{
    struct platform_memory_info mem_info[] = MEMORY_INFO;

    { /* TODO - temporary check boot register */
    	uintptr_t   tmp_reg = 0xfe000000 + SOC_PERIPH_OFF_AIOP;
    	/* wait for MC command for boot */
    	while (!(ioread32be(UINT_TO_PTR(tmp_reg + 0x90)) & (0x1 << core_get_id()))) ;
    }

    sys_param->partition_id = 0;
    sys_param->partition_cores_mask = 0x3;
    sys_param->master_cores_mask = 0x1;
    sys_param->use_cli = 0;
    sys_param->use_ipc = 0;

    sys_param->platform_param->clock_in_freq_hz = 100000000;
    sys_param->platform_param->l1_cache_mode = E_CACHE_MODE_INST_ONLY;
    sys_param->platform_param->console_type = E_PLATFORM_CONSOLE_NONE;
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

int run_apps(void)
{
    struct sys_module_desc apps[MAX_NUM_OF_APPS];
    int                    i;

    memset(apps, 0, sizeof(apps));
    build_apps_array(apps);
    
    for (i=0; i<MAX_NUM_OF_APPS; i++)
        if (apps[i].init)
            apps[i].init();

    return 0;
}
