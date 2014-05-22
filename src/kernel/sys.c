
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
static int sys_init_platform(struct platform_param *platform_param)
{
#define SYS_PLATFORM_INIT_FAIL_CHECK()  \
	do  {                               \
		if (err != 0)                \
		return err;                 \
		if (sys.init_fail_count)        \
		return E_NO_DEVICE;         \
	} while (0)

	int     err = 0;
	int is_master_core = sys_is_master_core();

	if (is_master_core) {
		err = platform_init(platform_param, &(sys.platform_ops));
		if (err == 0)
			ASSERT_COND(sys.platform_ops.h_platform);

		err = sys_add_handle(sys.platform_ops.h_platform,
			FSL_OS_MOD_SOC, 1, 0);
		if (err != 0)
			RETURN_ERROR(MAJOR, err, NO_MSG);
	}

	if (err != 0)
		sys.init_fail_count++;

	sys_barrier();

	SYS_PLATFORM_INIT_FAIL_CHECK();

	if (sys.platform_ops.f_disable_local_irq)
		sys.platform_ops.f_disable_local_irq(
			sys.platform_ops.h_platform);

	if (sys.platform_ops.f_init_core)
		err = sys.platform_ops.f_init_core(sys.platform_ops.h_platform);

	if (err != 0)
		sys.init_fail_count++;

	sys_barrier();

	SYS_PLATFORM_INIT_FAIL_CHECK();

	if (sys.platform_ops.f_init_timer)
		err = sys.platform_ops.f_init_timer(
			sys.platform_ops.h_platform);

	if ((err == 0) && is_master_core) {
		/* Do not change the sequence of calls in this section */

		if (sys.platform_ops.f_init_intr_ctrl)
			err = sys.platform_ops.f_init_intr_ctrl(
				sys.platform_ops.h_platform);

		if ((err == 0) && is_master_core) {
			if (sys.platform_ops.f_init_soc)
				err = sys.platform_ops.f_init_soc(
					sys.platform_ops.h_platform);
		}
	}

	if (err != 0)
		sys.init_fail_count++;

	sys_barrier();

	SYS_PLATFORM_INIT_FAIL_CHECK();

	if (sys.platform_ops.f_enable_local_irq)
		sys.platform_ops.f_enable_local_irq(
			sys.platform_ops.h_platform);

	if (is_master_core) {
#if 0
		/* Do not change the sequence of calls in this section */
		if ((err == 0) && sys.ipc_enabled &&
			sys.platform_ops.f_init_ipc)
			err = sys.platform_ops.f_init_ipc(
				sys.platform_ops.h_platform);
#endif /* 0 */

		if ((err == 0) && sys.platform_ops.f_init_console)
			err = sys.platform_ops.f_init_console(
				sys.platform_ops.h_platform);

		if (sys.platform_ops.f_init_mem_partitions)
			err = sys.platform_ops.f_init_mem_partitions(
				sys.platform_ops.h_platform);

	}

	if (err != 0)
		sys.init_fail_count++;

	sys_barrier();

	SYS_PLATFORM_INIT_FAIL_CHECK();

	if (sys.platform_ops.f_init_private)
		err = sys.platform_ops.f_init_private(
			sys.platform_ops.h_platform);

	if (err != 0)
		sys.init_fail_count++;

	sys_barrier();

	SYS_PLATFORM_INIT_FAIL_CHECK();

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

		/*
		if (sys.ipc_enabled && sys.platform_ops.f_free_ipc)
		err = sys.platform_ops.f_free_ipc(sys.platform_ops.h_platform);
		 */
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

/*****************************************************************************/
int sys_init(void)
{
	t_sys_param             sys_param;
	struct platform_param   platform_param;
	int       err, is_master_core;
	uint32_t        core_id = core_get_id();

	memset(&sys_param, 0, sizeof(sys_param));
	memset(&platform_param, 0, sizeof(platform_param));
	sys_param.platform_param = &platform_param;
	fill_system_parameters(&sys_param);

	sys.is_tile_master[core_id] = (int)(sys_param.master_cores_mask &
		(1ULL << core_id));
	sys.is_cluster_master[core_id] = ! (core_id % 4); //TODO make sure this is correct

	is_master_core = sys_is_master_core();

	if(is_master_core) {
		uintptr_t reg_base = (uintptr_t)(SOC_PERIPH_OFF_AIOP_TILE + SOC_PERIPH_OFF_AIOP_CMGW + 0x02000000);/* PLTFRM_MEM_RGN_AIOP */
		uint32_t abrr_val = ioread32(UINT_TO_PTR(reg_base + 0x90));

		sys.active_cores_mask  = abrr_val;
		/*sys.ipc_enabled          = sys_param.use_ipc;*/
	} else {
		while(!sys.active_cores_mask) {}
	}

#ifdef CORE_E6500
	platform_early_init(sys_param.platform_param);
#else
	if (is_master_core)
		platform_early_init(sys_param.platform_param);
#endif /* CORE_E6500 */

	/* reset boot_sync_flag */
	sys.boot_sync_flag = SYS_BOOT_SYNC_FLAG_DONE;

	if (is_master_core) {

		/* Initialize memory management */
		err = sys_init_memory_management();
		if (err != 0) {
			pr_err("Failed sys_init_memory_management\n");
			return err;
		}

#if 0
	/* Initialize interrupt management */
	err = sys_init_interrupt_management();
	ASSERT_COND(err == 0);
#endif /* 0 */

	}

	/* Initialize Multi-Processing services as needed */
	err = sys_init_multi_processing();
	if (err != 0) {
		pr_err("Failed sys_init_multi_processing\n");
		return err;
	}
	sys_barrier();

	err = sys_init_platform(sys_param.platform_param);
	if (err != 0)
		return -1;

	if (is_master_core) {
		if (!sys.console)
			/* If no platform console, register debugger console */
			sys_register_debugger_console();
	}
	return 0;
}

/*****************************************************************************/
void sys_free(void)
{
	sys_free_platform();

	sys_free_multi_processing();
	sys_barrier();

	if (sys_is_master_core()) {

#if 0
		/* Free interrupt management module */
		sys_free_interrupt_management();
#endif /* 0 */

		/* Free memory management module */
		sys_free_memory_management();
	}
}
