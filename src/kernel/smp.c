
#include "common/types.h"
#include "common/errors.h"
#include "common/spinlock.h"
#include "kernel/smp.h"

#include "sys.h"

/* Global System Object */
extern t_system sys;


/*****************************************************************************/
int sys_init_multi_processing(void)
{
    if (sys_is_master_core())
    {
        /* Initialize the central program barrier */
	sys.barrier_lock = 0;
        
	sys.barrier_mask = sys.active_cores_mask;
    }
    else
    {
        /* Wait until system barrier is initialized */
        while (!sys.barrier_mask) ;
    }

    return E_OK;
}

/*****************************************************************************/
void sys_free_multi_processing(void)
{
    /* Stub */
}

/*****************************************************************************/
void sys_barrier(void)
{
#ifdef UNDER_CONSTRUCTION
    const uint64_t core_mask = (uint64_t)(1 << core_get_id());

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
        sys.barrier_mask = sys.partition_cores_mask;
        unlock_spinlock(&(sys.barrier_lock));
    }
#endif
}

/*****************************************************************************/
int sys_is_core_active(uint32_t core_id)
{
    return (int)(((sys.active_cores_mask & (1ULL << core_id)))?0:1);
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

