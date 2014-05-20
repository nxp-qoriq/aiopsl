#include "common/types.h"
#include "common/fsl_string.h"
#include "common/io.h"
#include "kernel/smp.h"
#include "kernel/platform.h"
#include "inc/fsl_sys.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpni.h"
#include "common/dbg.h"
#include "common/fsl_malloc.h"
#include "common/fsl_spinlock.h"
#include "io.h"
#include "../drivers/dplib/arch/accel/fdma.h"  /* TODO: need to place fdma_release_buffer() in separate .h file */
#include "dplib/fsl_dpbp.h"

__SHRAM uint8_t abcr_lock = 0;

extern int cmdif_srv_init(void);    extern void cmdif_srv_free(void);
extern int dpni_drv_init(void);     extern void dpni_drv_free(void);
extern int slab_module_init(void);  extern void slab_module_free(void);
extern int aiop_sl_init(void);      extern void aiop_sl_free(void);

/* TODO: move to hdr file */
extern int dpni_drv_probe(struct dprc	*dprc,
			  uint16_t	mc_ni_id,
			  uint16_t	aiop_ni_id,
                          struct dpni_attach_cfg *attach_params);

extern void build_apps_array(struct sys_module_desc *apps);


#define MEMORY_INFO                                                                                           \
{   /* Region ID                Memory partition ID             Phys. Addr.    Virt. Addr.  Size            */\
    {PLTFRM_MEM_RGN_WS,         MEM_PART_INVALID,               0x00000000,    0x00000000, (2   * KILOBYTE) },\
    {PLTFRM_MEM_RGN_IRAM,       MEM_PART_INVALID,               0x00fe0000,    0x00fe0000, (128 * KILOBYTE) },\
    {PLTFRM_MEM_RGN_MC_PORTALS, MEM_PART_INVALID,               0x80c000000LL, 0x08000000, (64  * MEGABYTE) },\
    {PLTFRM_MEM_RGN_AIOP,       MEM_PART_INVALID,               0x02000000,    0x02000000, (384 * KILOBYTE) },\
    {PLTFRM_MEM_RGN_CCSR,       MEM_PART_INVALID,               0x08000000,    0x0c000000, (1 * MEGABYTE)   },\
    {PLTFRM_MEM_RGN_SHRAM,      MEM_PART_SH_RAM,                0x01010000,    0x01010000, (192 * KILOBYTE) },\
    {PLTFRM_MEM_RGN_DDR1,       MEM_PART_1ST_DDR_NON_CACHEABLE, 0x58000000,    0x58000000, (128 * MEGABYTE) },\
    {PLTFRM_MEM_RGN_PEB,        MEM_PART_PEB,                   0x80000000,    0x80000000, (2 * MEGABYTE)   },\
}

#define GLOBAL_MODULES                     \
{                                          \
    {slab_module_init,  slab_module_free}, \
    {dpni_drv_init,     dpni_drv_free},    \
    {cmdif_srv_init,    cmdif_srv_free},   \
    {aiop_sl_init,      aiop_sl_free},     \
    {NULL, NULL} /* never remove! */       \
}

#define MAX_NUM_OF_APPS		10

int fill_system_parameters(t_sys_param *sys_param);
int global_init(void);
int global_post_init(void);
int tile_init(void);
int cluster_init(void);
int run_apps(void);
void core_ready_for_tasks(void);


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
#ifdef UNDER_CONSTRUCTION
    //TODO not needed: sys_param->partition_cores_mask |= 0x1 << core_get_id(); //TODO this is not protected
#endif
    sys_param->master_cores_mask = 0x1;
    sys_param->use_ipc = 0;

    sys_param->platform_param->clock_in_freq_hz = 100000000;
    sys_param->platform_param->l1_cache_mode = E_CACHE_MODE_INST_ONLY;
    sys_param->platform_param->console_type = PLTFRM_CONSOLE_DUART;
    sys_param->platform_param->console_id = 0;
    memcpy(sys_param->platform_param->mem_info,
           mem_info,
           sizeof(struct platform_memory_info)*ARRAY_SIZE(mem_info));

    return 0;
}

int tile_init(void)
{
	return 0;
}

int cluster_init(void)
{
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
	return 0;
}

void core_ready_for_tasks(void)
{
    uint32_t abcr_val;
    uintptr_t   tmp_reg =
	    sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,
	                                      0,
	                                      E_MAPPED_MEM_TYPE_GEN_REGS);

    void* abcr = UINT_TO_PTR(tmp_reg + 0x98);

    if(sys_is_master_core()) {
	void* abrr = UINT_TO_PTR(tmp_reg + 0x90);
	uint32_t abrr_val = ioread32(abrr) & \
		(~((uint32_t)sys_get_cores_mask()));
	while(ioread32(abcr) != abrr_val) {asm{nop}}
    }

    /* Write AIOP boot status (ABCR) */
    lock_spinlock(&abcr_lock);
    abcr_val = ioread32(abcr);
    abcr_val |= (uint32_t)sys_get_cores_mask();
    iowrite32(abcr_val, abcr);
    unlock_spinlock(&abcr_lock);

    {
	void* abrr = UINT_TO_PTR(tmp_reg + 0x90);
	while(ioread32(abcr) != ioread32(abrr)) {asm{nop}}
    }

#if (STACK_OVERFLOW_DETECTION == 1)
    booke_set_spr_DAC2(0x800);
#endif

    /*  finished boot sequence; now wait for event .... */
    pr_info("AIOP %d completed boot sequence; waiting for events ...\n", core_get_id());

    /* CTSEN = 1, finished boot, Core Task Scheduler Enable */
    booke_set_CTSCSR0(booke_get_CTSCSR0() | CTSCSR_ENABLE);
    __e_hwacceli(YIELD_ACCEL_ID); /* Yield */
}


static void print_dev_desc(struct dprc_dev_desc* dev_desc)
{
	pr_debug(" device %d\n");
	pr_debug("***********\n");
	pr_debug("vendor - %x\n", dev_desc->vendor);
	if (dev_desc->type == DP_DEV_DPNI)
		pr_debug("type - DP_DEV_DPNI\n");
	else if (dev_desc->type == DP_DEV_DPRC)
		pr_debug("type - DP_DEV_DPRC\n");
	else if (dev_desc->type == DP_DEV_DPIO)
		pr_debug("type - DP_DEV_DPIO\n");
	pr_debug("id - %d\n", dev_desc->id);
	pr_debug("region_count - %d\n", dev_desc->region_count);
	pr_debug("rev_major - %d\n", dev_desc->rev_major);
	pr_debug("rev_minor - %d\n", dev_desc->rev_minor);
	pr_debug("irq_count - %d\n\n", dev_desc->irq_count);
}


/* TODO: Need to replace this temporary workaround with the actual function.
/*****************************************************************************/
static int fill_bpid(uint16_t num_buffs,
                              uint16_t buff_size,
                              uint16_t alignment,
                              uint8_t  mem_partition_id,
                              uint16_t bpid)
{
    int        i = 0;
    dma_addr_t addr  = 0;

    for (i = 0; i < num_buffs; i++) {
        addr = fsl_os_virt_to_phys(fsl_os_xmalloc(buff_size, mem_partition_id, alignment));
        /* Here, we pass virtual BPID, therefore BDI = 0 */
        if (fdma_release_buffer(0, FDMA_RELEASE_NO_FLAGS, bpid, addr)) {
            fsl_os_xfree(fsl_os_phys_to_virt(addr));
            return -ENAVAIL;
        }
    }
    return 0;
}

int run_apps(void)
{
	struct sys_module_desc apps[MAX_NUM_OF_APPS];
	int i;
	int err = 0, tmp = 0;
#ifndef AIOP_STANDALONE
	int dev_count;
	void *portal_vaddr;
	/* TODO: replace with memset */
	struct dprc dprc = { 0 };
	struct dpbp dpbp = { 0 };
	int container_id;
	struct dprc_dev_desc dev_desc;
	uint16_t dpbp_id;	// TODO: replace by real dpbp creation
	struct dpbp_attr attr;
	uint8_t region_index = 0;
	struct dpni_attach_cfg attach_params;
#endif


	/* TODO - add initialization of global default DP-IO (i.e. call 'dpio_open', 'dpio_init');
	* This should be mapped to ALL cores of AIOP and to ALL the tasks */
	/* TODO - add initialization of global default DP-SP (i.e. call 'dpsp_open', 'dpsp_init');
	* This should be mapped to 3 buff-pools with sizes: 128B, 512B, 2KB;
	* all should be placed in PEB. */
	/* TODO - need to scan the bus in order to retrieve the AIOP "Device list" */
	/* TODO - iterate through the device-list:
	* call 'dpni_drv_probe(ni_id, mc_portal_id, dpio, dp-sp)' */

#ifndef AIOP_STANDALONE
	/* TODO: replace hard-coded portal address 10 with configured value */
	/* TODO : layout file must contain portal ID 10 in order to work. */
	/* TODO : in this call, can 3rd argument be zero? */
	/* Get virtual address of MC portal */
	portal_vaddr = UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
    	                                 (uint32_t)1, E_MAPPED_MEM_TYPE_MC_PORTAL));

	/* Open root container in order to create and query for devices */
	dprc.cidesc.regs = portal_vaddr;
	if ((err = dprc_get_container_id(&dprc, &container_id)) != 0) {
		pr_err("Failed to get AIOP root container ID.\n");
		return(err);
	}
	if ((err = dprc_open(&dprc, container_id)) != 0) {
		pr_err("Failed to open AIOP root container DP-RC%d.\n", container_id);
		return(err);
	}

	/* TODO: replace the following dpbp_open&init with dpbp_create when available */

	/* TODO: Currently creating a stub DPBP with ID=1.
	 * Open and init calls will be replaced by 'create' when available at MC.
	 * At that point, the DPBP ID will be provided by MC. */
	dpbp_id = 0;

	dpbp.cidesc.regs = portal_vaddr;

	if ((err = dpbp_open(&dpbp, dpbp_id)) != 0) {
		pr_err("Failed to open DP-BP%d.\n", dpbp_id);
		return err;
	}

	if ((err = dpbp_enable(&dpbp)) != 0) {
		pr_err("Failed to enable DP-BP%d.\n", dpbp_id);
		return err;
	}

	if ((err = dpbp_get_attributes(&dpbp, &attr)) != 0) {
		pr_err("Failed to get attributes from DP-BP%d.\n", dpbp_id);
		return err;
	}

	/* TODO: number and size of buffers should not be hard-coded */
	if ((err = fill_bpid(100, attr.buffer_size, 64, MEM_PART_PEB, attr.bpid)) != 0) {
		pr_err("Failed to fill DP-BP%d (BPID=%d) with buffer size %d.\n",
				dpbp_id, attr.bpid, attr.buffer_size);
		return err;
	}

	/* Prepare parameters to attach to DPNI object */
	memset (&attach_params, 0, sizeof(attach_params));
	attach_params.num_dpbp = 1; /* for AIOP, can be up to 2 */
	attach_params.dpbp_id[0] = dpbp_id; /*!< DPBPs object id */

	if ((err = dprc_get_device_count(&dprc, &dev_count)) != 0) {
	    pr_err("Failed to get device count for AIOP root container DP-RC%d.\n", container_id);
	    return err;
	}

	/* Enable all DPNI devices */
	for (i = 0; i < dev_count; i++) {
		dprc_get_device(&dprc, i, &dev_desc);
		if (dev_desc.type == DP_DEV_DPNI) {
			/* TODO: print conditionally based on log level */
			print_dev_desc(&dev_desc);

			if ((err = dpni_drv_probe(&dprc, (uint16_t)dev_desc.id, (uint16_t)i, &attach_params)) != 0) {
				pr_err("Failed to probe DP-NI%d.\n", i);
				return err;
			}
		}
	}
#endif

	/* At this stage, all the NIC of AIOP are up and running */

	memset(apps, 0, sizeof(apps));
	build_apps_array(apps);

	for (i=0; i<MAX_NUM_OF_APPS; i++) {
		if (apps[i].init)
			apps[i].init();
	}

	return 0;
}
