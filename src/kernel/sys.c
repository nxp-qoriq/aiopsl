
#include "common/types.h"
#include "common/fsl_string.h"
#include "common/fsl_stdarg.h"
#include "common/fsl_malloc.h"
#include "kernel/fsl_spinlock.h"
#include "kernel/platform.h"
#include "kernel/smp.h"

#include "sys.h"
#include "dbg.h"

/* Global System Object */
__SHRAM t_system sys;

#define NUM_OF_HANDLES 5
extern void     __sys_start(register int argc, register char **argv,
				register char **envp);

typedef struct t_sys_forced_object {
	fsl_handle_t        h_module;
} t_sys_forced_object_desc;

__SHRAM t_sys_forced_object_desc  sys_handle[FSL_OS_NUM_MODULES];


/*****************************************************************************/
fsl_handle_t sys_get_handle(enum fsl_os_module module, int num_of_ids, ...)
{
	UNUSED(num_of_ids);
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0))
		return NULL;

	return sys_handle[module].h_module;

}

/*****************************************************************************/
int sys_add_handle(fsl_handle_t h_module, enum fsl_os_module module,
				int num_of_ids, ...)
{
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0) || (num_of_ids > 1))
		return -EINVAL;

	sys_handle[module].h_module = h_module;

	return 0;
}

/*****************************************************************************/
int sys_remove_handle(enum fsl_os_module module, int num_of_ids, ...)
{
	UNUSED(num_of_ids);
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0))
		return -EINVAL;


	sys_handle[module].h_module = NULL;

	return -ENAVAIL;
}

/*****************************************************************************/
static int sys_init_platform(void)
{
	int     err = 0;
	int is_master_core = sys_is_master_core();

	if (sys.platform_ops.f_disable_local_irq)
		sys.platform_ops.f_disable_local_irq(
			sys.platform_ops.h_platform);

	if (sys.platform_ops.f_init_core) {
		err = sys.platform_ops.f_init_core(sys.platform_ops.h_platform);
		if (err != 0) return -1;
	}
	
	if (sys.platform_ops.f_init_timer) {
		err = sys.platform_ops.f_init_timer(
			sys.platform_ops.h_platform);
		if (err != 0) return -1;
	}

	if (is_master_core) {
		/* Do not change the sequence of calls in this section */

		if (sys.platform_ops.f_init_intr_ctrl) {
			err = sys.platform_ops.f_init_intr_ctrl(
				sys.platform_ops.h_platform);
			if (err != 0) return -1;
		}

		if (sys.platform_ops.f_init_soc) {
			err = sys.platform_ops.f_init_soc(
				sys.platform_ops.h_platform);
			if (err != 0) return -1;
		}
	}

	if (sys.platform_ops.f_enable_local_irq)
		sys.platform_ops.f_enable_local_irq(
			sys.platform_ops.h_platform);

	if (is_master_core) {
		/* Do not change the sequence of calls in this section */
		if (sys.platform_ops.f_init_ipc) {
			err = sys.platform_ops.f_init_ipc(
				sys.platform_ops.h_platform);
			if (err != 0) return -1;
		}

		if (sys.platform_ops.f_init_console) {
			err = sys.platform_ops.f_init_console(
				sys.platform_ops.h_platform);
			if (err != 0) return -1;
		}
		
		if (!sys.console) {
			/* If no platform console, register debugger console */
			sys_register_debugger_console();
		}

		if (sys.platform_ops.f_init_mem_partitions) {
			err = sys.platform_ops.f_init_mem_partitions(
				sys.platform_ops.h_platform);
			if (err != 0) return -1;
		}

	}

	if (sys.platform_ops.f_init_private) {
		err = sys.platform_ops.f_init_private(
			sys.platform_ops.h_platform);
		if (err != 0) return -1;
	}

	return 0;
}


/*****************************************************************************/
static int sys_free_platform(void)
{
	int     err = 0;
	int is_master_core = sys_is_master_core();

	if (sys.platform_ops.f_free_private)
		err = sys.platform_ops.f_free_private(
			sys.platform_ops.h_platform);

	if (is_master_core) {
		/* Do not change the sequence of calls in this section */

		if (sys.platform_ops.f_free_mem_partitions)
			err = sys.platform_ops.f_free_mem_partitions(
				sys.platform_ops.h_platform);

		if (sys.platform_ops.f_free_console)
			err = sys.platform_ops.f_free_console(
				sys.platform_ops.h_platform);

		if (sys.platform_ops.f_free_ipc)
			err = sys.platform_ops.f_free_ipc(
				sys.platform_ops.h_platform);
		
	}

	sys_barrier();

	if (sys.platform_ops.f_free_timer)
		err = sys.platform_ops.f_free_timer(
			sys.platform_ops.h_platform);

	if (is_master_core) {
		/* Do not change the sequence of calls in this section */

		if (sys.platform_ops.f_free_soc)
			err = sys.platform_ops.f_free_soc(
				sys.platform_ops.h_platform);

		if (sys.platform_ops.f_free_intr_ctrl)
			err = sys.platform_ops.f_free_intr_ctrl(
				sys.platform_ops.h_platform);
	}

	sys_barrier();

	if (sys.platform_ops.f_free_core)
		err = sys.platform_ops.f_free_core(
			sys.platform_ops.h_platform);

	if (is_master_core) {
		err = platform_free(
			sys.platform_ops.h_platform);
		sys.platform_ops.h_platform = NULL;
	}

	sys_barrier();

	return err;
}

static void update_active_cores_mask(void)
{
	uintptr_t reg_base = (uintptr_t)(SOC_PERIPH_OFF_AIOP_TILE \
		+ SOC_PERIPH_OFF_AIOP_CMGW \
		+ 0x02000000);/* PLTFRM_MEM_RGN_AIOP */
	uint32_t abrr_val = ioread32(UINT_TO_PTR(reg_base + 0x90));
	
	sys.active_cores_mask  = abrr_val;
}

static int global_sys_init(void)
{
	struct platform_param platform_param;
	int err = 0;
	ASSERT_COND(sys_is_master_core());
	
	update_active_cores_mask();
	
	fill_system_parameters(&platform_param);
	
	platform_early_init(&platform_param);

	/* Initialize memory management */
	err = sys_init_memory_management();
	if (err != 0) return err;
	
	/* Initialize Multi-Processing services as needed */
	err = sys_init_multi_processing();
	if (err != 0) return err;
	
	/* Platform init */
	err = platform_init(&(platform_param), &(sys.platform_ops));
	if (err != 0) return err;
	
	err = sys_add_handle(sys.platform_ops.h_platform,
		FSL_OS_MOD_SOC, 1, 0);
	if (err != 0) return err;
	
	return E_OK;
}

/*****************************************************************************/
int sys_init(void)
{
	int       err, is_master_core;
	uint32_t        core_id = core_get_id();

	sys.is_tile_master[core_id] = (int)(AIOP_TILE_MASTERS_MASK \
							& (1ULL << core_id));
	sys.is_cluster_master[core_id] = (int)(AIOP_CLUSTER_MASTER_MASK \
							& (1ULL << core_id));

	is_master_core = sys_is_master_core();

	if(is_master_core) {
		err = global_sys_init();
		if (err != 0)
			return -1;

		/* signal all other cores that global initiation is done */
		sys.boot_sync_flag = 1;
	} else {
		while(!sys.boot_sync_flag) {}
	}

	err = sys_init_platform();
	if (err != 0)
		return -1;

	return 0;
}

/*****************************************************************************/
void sys_free(void)
{
	sys_free_platform();

	sys_free_multi_processing();
	sys_barrier();

	if (sys_is_master_core()) {

		/* Free memory management module */
		sys_free_memory_management();
	}
}
