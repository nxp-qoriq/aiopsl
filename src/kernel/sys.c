
#include "common/types.h"
#include "common/fsl_string.h"
#include "common/fsl_stdarg.h"
#include "common/fsl_malloc.h"
#include "kernel/platform.h"
#include "kernel/smp.h"

#include "sys.h"


/* Global System Object */
t_system sys;

extern void     __arena_start(register int argc, register char **argv, register char **envp);
extern void     __arena_start_secondary(void);
#ifdef CORE_E6500
extern void     __arena_start_secondary_guest(void);
#endif /* CORE_E6500 */


typedef struct t_sys_forced_object {
    enum fsl_os_module  module;
    uint8_t             num_of_ids;
    uint32_t            id[SYS_MAX_NUM_OF_MODULE_IDS];
    fsl_handle_t        h_module;
    list_t              node;
} t_sys_forced_object_desc;


/*****************************************************************************/
static void sys_init_objects_registry(void)
{
    //memset(sys.modules_info, 0, sizeof(sys.modules_info));
    //memset(sys.sub_modules_info, 0, sizeof(sys.sub_modules_info));
    INIT_LIST(&(sys.forced_objects_list));

    //sys_init_spinlock(&sys.object_mng_lock);
}

/*****************************************************************************/
static void sys_free_objects_management(void)
{
    t_sys_forced_object_desc   *p_forced_object;
    list_t                  *p_node, *p_temp;

    /* Freeing all network devices descriptors */
    LIST_FOR_EACH_SAFE(p_node, p_temp, &(sys.forced_objects_list)) {
        p_forced_object = LIST_OBJECT(p_node, t_sys_forced_object_desc, node);

        list_del(p_node);
        fsl_os_free(p_forced_object);
    }

#if 0 /* what is this??? */
    /* Free the settings cloning information */
    if (sys.p_clone_scratch_pad)
        fsl_os_free(sys.p_clone_scratch_pad);
#endif /* 0 */
}

/*****************************************************************************/
fsl_handle_t sys_get_handle(enum fsl_os_module module, ... )
{
    t_sys_forced_object_desc   *p_forced_object;
    va_list                 arguments;        //A place to store the list of arguments
    list_t                  *p_node;
    uint32_t                ids[SYS_MAX_NUM_OF_MODULE_IDS];
    uint8_t                 i;

    ASSERT_COND(sys.forced_objects_list.next && sys.forced_objects_list.prev);

    va_start(arguments, module); //initializing arguments to store all values
    for(i = 0; i < SYS_MAX_NUM_OF_MODULE_IDS; i++)
        ids[i] = va_arg(arguments, uint32_t);
    va_end(arguments);

    /* If id equals SYS_NULL_OBJECT_ID, we must return NULL */
    /* Search in the forced objects list. This list usually serves for
       storing specific objects that were initialized by the platform. */
    LIST_FOR_EACH(p_node, &(sys.forced_objects_list)) {
        p_forced_object = LIST_OBJECT(p_node, t_sys_forced_object_desc, node);

        if (p_forced_object->module == module) {
            for(i = 0; i < p_forced_object->num_of_ids; i++)
                if (p_forced_object->id[i] != ids[i])
                    break;

            if (i == p_forced_object->num_of_ids)
                return p_forced_object->h_module;
        }
    }

    return NULL;
}

/*****************************************************************************/
int sys_add_handle(fsl_handle_t h_module, enum fsl_os_module module, uint8_t num_of_ids, ... )
{
    t_sys_forced_object_desc   *p_forced_object;
    uint32_t                id;
    uint8_t                 i;
    va_list                 arguments;        //A place to store the list of arguments

    ASSERT_COND(num_of_ids <= SYS_MAX_NUM_OF_MODULE_IDS && num_of_ids > 0);

    ASSERT_COND((sys.forced_objects_list.next && sys.forced_objects_list.prev) ||
                (!sys.forced_objects_list.next && !sys.forced_objects_list.prev));

    if (!sys.forced_objects_list.next)
        INIT_LIST(&sys.forced_objects_list);

    va_start(arguments, num_of_ids); //initializing arguments to store all values
    p_forced_object = (t_sys_forced_object_desc *)fsl_os_malloc(sizeof(t_sys_forced_object_desc));
    if (!p_forced_object) {
        va_end(arguments);
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("forced object descriptor"));
    }
    p_forced_object->num_of_ids = num_of_ids;
    for(i = 0; i < num_of_ids; i++) {
        id = va_arg(arguments, uint32_t);
        p_forced_object->id[i] = id;
    }
    p_forced_object->module = module;
    p_forced_object->h_module = h_module;
    INIT_LIST(&(p_forced_object->node));
    list_add_to_tail(&(p_forced_object->node), &(sys.forced_objects_list));
    va_end(arguments);

    return E_OK;
}

/*****************************************************************************/
void sys_remove_handle(enum fsl_os_module module, uint32_t id, ... )
{
#pragma unused (id, module)

    REPORT_ERROR(MAJOR, E_NOT_SUPPORTED, NO_MSG);
}

/*****************************************************************************/
static int sys_init_platform(struct platform_param *platform_param)
{
#define SYS_PLATFORM_INIT_FAIL_CHECK()  \
    do  {                               \
        if (err != E_OK)                \
            return err;                 \
        if (sys.init_fail_count)        \
            return E_NO_DEVICE;         \
    } while (0)

    int     err = E_OK;
    uint32_t    core_id = core_get_id();

    if (sys.is_partition_master[core_id]) {
        err = platform_init(platform_param, &(sys.platform_ops));
        if (err == E_OK)
            ASSERT_COND(sys.platform_ops.h_platform);

        err = sys_add_handle(sys.platform_ops.h_platform, FSL_OS_MOD_SOC, 1, 0);
        if (err != E_OK)
            RETURN_ERROR(MAJOR, err, NO_MSG);
    }

    if (err != E_OK)
        sys.init_fail_count++;

    sys_barrier();

    SYS_PLATFORM_INIT_FAIL_CHECK();

    if (sys.platform_ops.f_disable_local_irq)
        sys.platform_ops.f_disable_local_irq(sys.platform_ops.h_platform);

    if (sys.platform_ops.f_init_core)
        err = sys.platform_ops.f_init_core(sys.platform_ops.h_platform);

    if (err != E_OK)
        sys.init_fail_count++;

    sys_barrier();

    SYS_PLATFORM_INIT_FAIL_CHECK();

    if (sys.platform_ops.f_init_timer)
        err = sys.platform_ops.f_init_timer(sys.platform_ops.h_platform);

    if ((err == E_OK) && sys.is_partition_master[core_id]) {
        /* Do not change the sequence of calls in this section */

        if (sys.platform_ops.f_init_intr_ctrl)
            err = sys.platform_ops.f_init_intr_ctrl(sys.platform_ops.h_platform);

        if ((err == E_OK) && sys.is_master_partition_master[core_id]) {
            if (sys.platform_ops.f_init_soc)
                err = sys.platform_ops.f_init_soc(sys.platform_ops.h_platform);
        }
    }

    if (err != E_OK)
        sys.init_fail_count++;

    sys_barrier();

    SYS_PLATFORM_INIT_FAIL_CHECK();

    if (sys.platform_ops.f_enable_local_irq)
        sys.platform_ops.f_enable_local_irq(sys.platform_ops.h_platform);

    if (sys.is_partition_master[core_id]) {
#if 0
    	/* Do not change the sequence of calls in this section */
        if ((err == E_OK) && sys.ipc_enabled && sys.platform_ops.f_init_ipc)
            err = sys.platform_ops.f_init_ipc(sys.platform_ops.h_platform);
#endif /* 0 */

        if ((err == E_OK) && sys.platform_ops.f_init_console)
            err = sys.platform_ops.f_init_console(sys.platform_ops.h_platform);

        if (sys.platform_ops.f_init_mem_partitions)
            err = sys.platform_ops.f_init_mem_partitions(sys.platform_ops.h_platform);

    }

    if (err != E_OK)
        sys.init_fail_count++;

    sys_barrier();

    SYS_PLATFORM_INIT_FAIL_CHECK();

    if (sys.platform_ops.f_init_private)
        err = sys.platform_ops.f_init_private(sys.platform_ops.h_platform);

    if (err != E_OK)
        sys.init_fail_count++;

    sys_barrier();

    SYS_PLATFORM_INIT_FAIL_CHECK();

#if 0
    /* For AMP system: kick off other partitions */
    if (sys.is_master_partition_master[core_id])
    {
        sys_kick_spinning_cores(sys.master_cores_mask);
//todo - remove the following lines (must use appropriate cfg)
        if (sys.platform_ops.f_enable_cores)
            sys.platform_ops.f_enable_cores(sys.platform_ops.h_platform, sys.master_cores_mask);
    }
#endif /* 0 */

    return E_OK;
}


/*****************************************************************************/
static int sys_free_platform(void)
{
    int     err = E_OK;
    uint32_t    core_id = core_get_id();

    if (sys.platform_ops.f_free_private)
        err = sys.platform_ops.f_free_private(sys.platform_ops.h_platform);

    if (sys.is_partition_master[core_id])
    {
        /* Do not change the sequence of calls in this section */

        if (sys.platform_ops.f_free_mem_partitions)
            err = sys.platform_ops.f_free_mem_partitions(sys.platform_ops.h_platform);

        if (sys.platform_ops.f_free_console)
            err = sys.platform_ops.f_free_console(sys.platform_ops.h_platform);

/*
        if (sys.ipc_enabled && sys.platform_ops.f_free_ipc)
            err = sys.platform_ops.f_free_ipc(sys.platform_ops.h_platform);
*/
    }

    sys_barrier();

    if (sys.platform_ops.f_free_timer)
        err = sys.platform_ops.f_free_timer(sys.platform_ops.h_platform);

    if (sys.is_partition_master[core_id]) {
        /* Do not change the sequence of calls in this section */

        if (sys.is_master_partition_master) {
            if (sys.platform_ops.f_free_soc)
                err = sys.platform_ops.f_free_soc(sys.platform_ops.h_platform);
        }

        if (sys.platform_ops.f_free_intr_ctrl)
            err = sys.platform_ops.f_free_intr_ctrl(sys.platform_ops.h_platform);
    }

    sys_barrier();

    if (sys.platform_ops.f_free_core)
        err = sys.platform_ops.f_free_core(sys.platform_ops.h_platform);

    if (sys.is_partition_master[core_id]) {
        err = platform_free(sys.platform_ops.h_platform);
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
#if (defined(ARENA_SMP_SUPPORT) && defined(ARENA_64BIT_ARCH))
    dma_addr_t   *p_master_start_addr;
#ifdef CORE_E6500
    dma_addr_t   *p_guest_start_addr;
#endif /* CORE_E6500 */
#endif /* ARENA_SMP_SUPPORT && ARENA_64BIT_ARCH */

    memset(&sys_param, 0, sizeof(sys_param));
    memset(&platform_param, 0, sizeof(platform_param));
    sys_param.platform_param = &platform_param;
    fill_system_parameters(&sys_param);

    sys.is_partition_master[core_id]       = (int)(sys_param.master_cores_mask & (1ULL << core_id));
    sys.is_master_partition_master[core_id] = (int)(sys.is_partition_master[core_id] && (sys_param.partition_id == ARENA_MASTER_PART_ID));
    sys.is_core_master[core_id]            = IS_CORE_MASTER(core_id, sys_param.partition_cores_mask);

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
#ifdef ARENA_SMP_SUPPORT
        /* Kick secondary cores on this partition */
#ifdef ARENA_64BIT_ARCH
        /* In 64-bit ABI, function name points to function descriptor.
           This descriptor contains the function address. */
#ifdef CORE_E6500
            p_master_start_addr = (dma_addr_t *)(__arena_start_secondary);
            p_guest_start_addr = (dma_addr_t *)(__arena_start_secondary_guest);

            sys_kick_spinning_cores(sys.partition_cores_mask,
                                 (dma_addr_t)PTR_TO_UINT(p_master_start_addr),
                                 (dma_addr_t)PTR_TO_UINT(p_guest_start_addr));
#else
        p_master_start_addr = (dma_addr_t *)(__arena_start);

        sys_kick_spinning_cores(sys.partition_cores_mask,
                             (dma_addr_t)PTR_TO_UINT(*p_master_start_addr),
                             0);
#endif /* CORE_E6500 */

#else /*!ARENA_64BIT_ARCH*/
#ifdef ARENA_SECONDARY_START
#ifdef CORE_E6500
        sys_kick_spinning_cores(sys.partition_cores_mask,
                     (dma_addr_t)PTR_TO_UINT(__arena_start_secondary),
                     (dma_addr_t)PTR_TO_UINT(__arena_start_secondary_guest));
#else /*CORE_E6500*/
        sys_kick_spinning_cores(sys.partition_cores_mask,
                             (dma_addr_t)PTR_TO_UINT(__arena_start_secondary),
                             0);
#endif /* CORE_E6500 */
#else  /*!ARENA_SECONDARY_START*/
        sys_kick_spinning_cores(sys.partition_cores_mask, (dma_addr_t)PTR_TO_UINT(__arena_start), 0);
#endif /* ARENA_SECONDARY_START */
#endif /* ARENA_64BIT_ARCH */
#endif /* ARENA_SMP_SUPPORT */

        /* Initialize memory management */
        err = sys_init_memory_management();
        ASSERT_COND(err == E_OK);

#if 0
        /* Initialize interrupt management */
        err = sys_init_interrupt_management();
        ASSERT_COND(err == E_OK);
#endif /* 0 */

        /* Initialize the objects registry structures */
        sys_init_objects_registry();
    }

    /* Initialize Multi-Processing services as needed */
    err = sys_init_multi_processing();
    ASSERT_COND(err == E_OK);
    sys_barrier();

#if 0
    if (sys.is_master_partition_master[core_id] && sys_param.use_cli) {
        /* Initialize CLI */
        err = sys_init_cli();
        ASSERT_COND(err == E_OK);
    }
    sys_barrier();
#endif /* 0 */

    err = sys_init_platform(sys_param.platform_param);
    if (err != E_OK)
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
