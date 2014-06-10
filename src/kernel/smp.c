
#include "common/types.h"
#include "common/errors.h"
#include "kernel/fsl_spinlock.h"
#include "kernel/smp.h"

#include "sys.h"

/* Global System Object */
extern t_system sys;


/*****************************************************************************/
int sys_init_multi_processing(void)
{
    /* Initialize the central program barrier */
    sys.barrier_lock = 0;
    sys.barrier_mask = sys.active_cores_mask;
 
    return 0;
}

/*****************************************************************************/
void sys_free_multi_processing(void)
{
    /* Stub */
}

/*****************************************************************************/
#pragma optimization_level 1
void sys_barrier(void)
{
    uint64_t core_mask = (uint64_t)(1 << core_get_id());

    lock_spinlock(&(sys.barrier_lock));
    /* Mark this core's presence */
    sys.barrier_mask &= ~(core_mask);

    if (sys.barrier_mask)
    {
        unlock_spinlock(&(sys.barrier_lock));
        /* Wait until barrier is reset */
        while (!(sys.barrier_mask & core_mask)) {}
    }
    else
    {
        /* Last core to arrive - reset the barrier */
        sys.barrier_mask = sys.active_cores_mask;
        unlock_spinlock(&(sys.barrier_lock));
    }
}
#pragma optimization_level reset

/*****************************************************************************/
int sys_is_core_active(uint32_t core_id)
{
    return (int)(((sys.active_cores_mask & (1ULL << core_id)))?1:0);
}

/*****************************************************************************/
int sys_is_master_core(void)
{
    return sys.is_tile_master[core_get_id()];
}

/*****************************************************************************/
int sys_is_cluster_master(void)
{
    return sys.is_cluster_master[core_get_id()];
}

/*****************************************************************************/
uint64_t sys_get_cores_mask(void)
{
    return sys.active_cores_mask;
}

