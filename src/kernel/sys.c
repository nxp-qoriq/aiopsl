
#include "common/types.h"
#include "common/fsl_string.h"
#include "common/fsl_stdarg.h"
#include "common/fsl_malloc.h"
#include "common/spinlock.h"
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


/* TODO: Think if it can be part of &(sys.forced_objects_list)*/

/*****************************************************************************/
static void sys_init_objects_registry(void)
{
#ifdef ARENA_LEGACY_CODE
	/*memset(sys.modules_info, 0, sizeof(sys.modules_info));
	memset(sys.sub_modules_info, 0, sizeof(sys.sub_modules_info));
	INIT_LIST(&(sys.forced_objects_list));
	p_forced_object = (t_sys_forced_object_desc *)fsl_os_malloc(
			sizeof(t_sys_forced_object_desc));
	sys_init_spinlock(&sys.object_mng_lock);*/
#endif
}

/*****************************************************************************/
static void sys_free_objects_management(void)
{
#ifdef ARENA_LEGACY_CODE
	/*t_sys_forced_object_desc   *p_forced_object;
	list_t                  *p_node, *p_temp;*/

	/* Freeing all network devices descriptors */
	/*LIST_FOR_EACH_SAFE(p_node, p_temp, &(sys.forced_objects_list)) {
	p_forced_object = LIST_OBJECT(p_node, t_sys_forced_object_desc, node);

	list_del(p_node);
	fsl_os_free(p_forced_object);
	}*/

#if 0 /* what is this??? */
	/* Free the settings cloning information */
	if (sys.p_clone_scratch_pad)
		fsl_os_free(sys.p_clone_scratch_pad);
#endif /* 0 */
#endif
}

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

	ASSERT_COND(platform_param);
	
	if (is_master_core) {
		err = platform_init(platform_param, &(sys.platform_ops));
		if (err != 0)
			RETURN_ERROR(MAJOR, err, NO_MSG);
		
		err = sys_add_handle(sys.platform_ops.h_platform,
			FSL_OS_MOD_SOC, 1, 0);
		if (err != 0)
			RETURN_ERROR(MAJOR, err, NO_MSG);
	}

	/* 
	 * TODO critical point:
	 * platform_init() must be completed by mastercore before any other 
	 * core does the following code.
	 */
	
	//TODO why do I need this parameter ??
	if (err != 0)
		sys.init_fail_count++; 

	SYS_PLATFORM_INIT_FAIL_CHECK(); //TODO can I replace this with a simple check ??

	if (sys.platform_ops.f_disable_local_irq)
		sys.platform_ops.f_disable_local_irq(
			sys.platform_ops.h_platform);

	if (sys.platform_ops.f_init_core)
		err = sys.platform_ops.f_init_core(sys.platform_ops.h_platform);

	if (err != 0)
		sys.init_fail_count++; //TODO why

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

	SYS_PLATFORM_INIT_FAIL_CHECK();

	if (sys.platform_ops.f_init_private)
		err = sys.platform_ops.f_init_private(
			sys.platform_ops.h_platform);

	if (err != 0)
		sys.init_fail_count++;

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
	struct platform_param   platform_param; //TODO only master-core needs it !!
	int       err, is_master_core;
	uint32_t        core_id = core_get_id();

	sys.is_tile_master[core_id] = (int)(0x1 & (1ULL << core_id)); //TODO make it more flexible ...
	sys.is_cluster_master[core_id] = ! (core_id % 4);
	
	is_master_core = sys_is_master_core();
	
	if(is_master_core) {
		uintptr_t reg_base = (uintptr_t)(SOC_PERIPH_OFF_AIOP_TILE \
			+ SOC_PERIPH_OFF_AIOP_CMGW \
			+ 0x02000000);/* PLTFRM_MEM_RGN_AIOP */
		uint32_t abrr_val = ioread32(UINT_TO_PTR(reg_base + 0x90));
		
		fill_system_parameters(&platform_param);
		
		sys.active_cores_mask  = abrr_val;
		
		/*sys.ipc_enabled          = sys_param.use_ipc;*/ //TODO remove all IPC, keep skeleton
	} else {
		while(!sys.active_cores_mask) {} //TODO remove
	}

	if (is_master_core)
		platform_early_init(&platform_param);

	if (is_master_core) {

		/* Initialize memory management */
		err = sys_init_memory_management();
		if (err != 0) {
			pr_err("Failed sys_init_memory_management\n"); //XXX remove
			return err;
		}

		/* Initialize the objects registry structures */
		sys_init_objects_registry(); //TODO do nothing, Alex will remove this !!
	}

	/* Initialize Multi-Processing services as needed */
	err = sys_init_multi_processing();
	if (err != 0) {
		pr_err("Failed sys_init_multi_processing\n"); //TODO remove - ask ALEX
		return err;
	}
	sys_barrier(); //TODO remove

	err = sys_init_platform(&platform_param);
	if (err != 0)
		return -1;

	if (is_master_core) {
		if (!sys.console) {
			/* If no platform console, register debugger console */
			sys_register_debugger_console();
		}
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
		/* Free objects management structures */
		sys_free_objects_management();

		/* Free memory management module */
		sys_free_memory_management();
	}
}
