
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
extern void     __sys_start_secondary(void);
#ifdef CORE_E6500
extern void     __sys_start_secondary_guest(void);
#endif /* CORE_E6500 */


typedef struct t_sys_forced_object {
	fsl_handle_t        h_module;
} t_sys_forced_object_desc;

__SHRAM t_sys_forced_object_desc  sys_handle[FSL_OS_NUM_MODULES];

/* TODO: Think if it can be part of &(sys.forced_objects_list)*/

/*****************************************************************************/
static void sys_init_objects_registry(void)
{
	//memset(sys.modules_info, 0, sizeof(sys.modules_info));
	//memset(sys.sub_modules_info, 0, sizeof(sys.sub_modules_info));
	//INIT_LIST(&(sys.forced_objects_list));
	/*p_forced_object = (t_sys_forced_object_desc *)fsl_os_malloc(
			sizeof(t_sys_forced_object_desc));*/
	//sys_init_spinlock(&sys.object_mng_lock);
}

/*****************************************************************************/
static void sys_free_objects_management(void)
{
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
}

/*****************************************************************************/
fsl_handle_t sys_get_handle(enum fsl_os_module module, int num_of_ids, ...)
{
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0))
		return NULL;

	return sys_handle[module].h_module;

}

/*****************************************************************************/
int sys_add_handle(fsl_handle_t h_module, enum fsl_os_module module,
                   int num_of_ids, ... )
{
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0))
		return -EINVAL;

	sys_handle[module].h_module = h_module;

	return 0;


}

/*****************************************************************************/
int sys_remove_handle(enum fsl_os_module module, int num_of_ids, ... )
{
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
	uint32_t    core_id = core_get_id();

	if (sys.is_partition_master[core_id]) {
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

	if ((err == 0) && sys.is_partition_master[core_id]) {
		/* Do not change the sequence of calls in this section */

		if (sys.platform_ops.f_init_intr_ctrl)
			err = sys.platform_ops.f_init_intr_ctrl(
				sys.platform_ops.h_platform);

		if ((err == 0) && sys.is_master_partition_master[core_id]) {
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

	if (sys.is_partition_master[core_id]) {
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
	uint32_t    core_id = core_get_id();

	if (sys.platform_ops.f_free_private)
		err = sys.platform_ops.f_free_private(
			sys.platform_ops.h_platform);

	if (sys.is_partition_master[core_id])
	{
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

	if (sys.is_partition_master[core_id]) {
		/* Do not change the sequence of calls in this section */

		if (sys.is_master_partition_master) {
			if (sys.platform_ops.f_free_soc)
				err = sys.platform_ops.f_free_soc(
					sys.platform_ops.h_platform);
		}

		if (sys.platform_ops.f_free_intr_ctrl)
			err = sys.platform_ops.f_free_intr_ctrl(
				sys.platform_ops.h_platform);
	}

	sys_barrier();

	if (sys.platform_ops.f_free_core)
		err = sys.platform_ops.f_free_core(
			sys.platform_ops.h_platform);

	if (sys.is_partition_master[core_id]) {
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
	int       err;
	uint32_t        core_id = core_get_id();
#if (defined(SYS_SMP_SUPPORT) && defined(SYS_64BIT_ARCH))
	dma_addr_t   *p_master_start_addr;
#ifdef CORE_E6500
	dma_addr_t   *p_guest_start_addr;
#endif /* CORE_E6500 */
#endif /* SYS_SMP_SUPPORT && SYS_64BIT_ARCH */

	memset(&sys_param, 0, sizeof(sys_param));
	memset(&platform_param, 0, sizeof(platform_param));
	sys_param.platform_param = &platform_param;
	fill_system_parameters(&sys_param);

	sys.is_partition_master[core_id] = (int)(sys_param.master_cores_mask &
						(1ULL << core_id));
	sys.is_master_partition_master[core_id] = (int)(
		sys.is_partition_master[core_id] &&
		(sys_param.partition_id == 0));
	sys.is_core_master[core_id] = IS_CORE_MASTER(core_id,
					sys_param.partition_cores_mask);

	if (sys.is_partition_master[core_id]) {
		sys.partition_id         = sys_param.partition_id;
		sys.partition_cores_mask  = sys_param.partition_cores_mask;
		sys.master_cores_mask     = sys_param.master_cores_mask;
		//sys.ipc_enabled          = sys_param.use_ipc;
	}

#ifdef CORE_E6500
	platform_early_init(sys_param.platform_param);
#else
	if (sys.is_partition_master[core_id])
		platform_early_init(sys_param.platform_param);
#endif /* CORE_E6500 */

	/* reset boot_sync_flag */
	sys.boot_sync_flag = SYS_BOOT_SYNC_FLAG_DONE;

	if (sys.is_partition_master[core_id]) {

	/* Initialize memory management */
	err = sys_init_memory_management();
	if (err != 0) {
		pr_err("Failed sys_init_memory_management\n");
		return err;
	}

#ifdef SYS_SMP_SUPPORT
		/* Kick secondary cores on this partition */
#ifdef SYS_64BIT_ARCH
		/* In 64-bit ABI, function name points to function descriptor.
           This descriptor contains the function address. */
#ifdef CORE_E6500
	p_master_start_addr = (dma_addr_t *)(__sys_start_secondary);
	p_guest_start_addr = (dma_addr_t *)(__sys_start_secondary_guest);

	sys_kick_spinning_cores(sys.partition_cores_mask,
		(dma_addr_t)PTR_TO_UINT(p_master_start_addr),
		(dma_addr_t)PTR_TO_UINT(p_guest_start_addr));
#else
	p_master_start_addr = (dma_addr_t *)(__sys_start);

	sys_kick_spinning_cores(sys.partition_cores_mask,
		(dma_addr_t)PTR_TO_UINT(*p_master_start_addr),
		0);
#endif /* CORE_E6500 */

#else /*!SYS_64BIT_ARCH*/
#ifdef SYS_SECONDARY_START
#ifdef CORE_E6500
	sys_kick_spinning_cores(sys.partition_cores_mask,
		(dma_addr_t)PTR_TO_UINT(__sys_start_secondary),
		(dma_addr_t)PTR_TO_UINT(__sys_start_secondary_guest));
#else /*CORE_E6500*/
	sys_kick_spinning_cores(sys.partition_cores_mask,
		(dma_addr_t)PTR_TO_UINT(__sys_start_secondary),
		0);
#endif /* CORE_E6500 */
#else  /*!SYS_SECONDARY_START*/
#if 0
        sys_kick_spinning_cores(sys.partition_cores_mask, (dma_addr_t)PTR_TO_UINT(__sys_start), 0);
#endif /* 0 */
#endif /* SYS_SECONDARY_START */
#endif /* SYS_64BIT_ARCH */
#endif /* SYS_SMP_SUPPORT */

#if 0
		/* Initialize interrupt management */
		err = sys_init_interrupt_management();
		ASSERT_COND(err == 0);
#endif /* 0 */

		/* Initialize the objects registry structures */
		sys_init_objects_registry();
	}

    /* Initialize Multi-Processing services as needed */
    err = sys_init_multi_processing();
    if (err != 0) {
	    pr_err("Failed sys_init_multi_processing\n");
	    return err;
    }
    sys_barrier();

#if 0
	if (sys.is_master_partition_master[core_id] && sys_param.use_cli) {
		/* Initialize CLI */
		err = sys_init_cli();
		ASSERT_COND(err == 0);
	}
	sys_barrier();
#endif /* 0 */

	err = sys_init_platform(sys_param.platform_param);
	if (err != 0)
		return -1;

	if (!sys.console)
		/* If no platform console, register debugger console */
		sys_register_debugger_console();

	return 0;
}

/*****************************************************************************/
void sys_free(void)
{
	uint32_t core_id = core_get_id();

	sys_free_platform();

#if 0
	if (sys.is_partition_master[core_id])
		sys_free_cli();
	sys_barrier();
#endif /* 0 */

	sys_free_multi_processing();
	sys_barrier();

	if (sys.is_partition_master[core_id]) {
		/* Free objects management structures */
		sys_free_objects_management();

#if 0
		/* Free interrupt management module */
		sys_free_interrupt_management();
#endif /* 0 */

		/* Free memory management module */
		sys_free_memory_management();
	}
}
