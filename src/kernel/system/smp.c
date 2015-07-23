/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "fsl_types.h"
#include "fsl_errors.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_smp.h"

#include "sys.h"



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
void sys_barrier(void)
{
#ifndef SINGLE_CORE_WA
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
#endif /* SINGLE_CORE_WA */
}

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

/*****************************************************************************/
uint32_t sys_get_num_of_cores(void)
{
    return sys.num_of_active_cores;
}

/*****************************************************************************/
uint32_t sys_get_max_num_of_cores(void)
{
    return INTG_MAX_NUM_OF_CORES;
}
