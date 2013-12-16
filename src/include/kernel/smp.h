/**

 @File          smp.h

 @Description   Definitions and functions for initializing and operating
                multi-processing services.

*//***************************************************************************/
#ifndef __SYS_MP_H
#define __SYS_MP_H

#include "common/types.h"
#include "common/fsl_core.h"


/**************************************************************************//**
 @Group         sys_g     System Interfaces

 @Description   Bare-board system programming interfaces.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         sys_mp_grp      System Multi-Processing Interfaces

 @Description   System interfaces for Multi-Processing.

                The system Multi-Processing interfaces include:
                - General functions
                - Spinlocks
                - Barrier

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Collection    General Multi-Processing Application Programming Interface

 @Description   Spinlock definitions and functions.

 @{
*//***************************************************************************/

#define SYS_ANY_CORE    ((uint32_t)(~0)) /**< Assignment to any valid core (for use
                                              in system layout object descriptors) */

/**************************************************************************//**
 @Function      sys_is_core_active

 @Description   Checks if a given core is active in current configuration
                (according to the partition cores mask).

                This macro evaluates to 1 if the respective bit of current
                core is set.
*//***************************************************************************/
int sys_is_core_active(uint32_t core_id);

/**************************************************************************//**
 @Function      sys_is_master_core

 @Description   Checks if the current core is the master core.

                In SMP configuration, the master core is marked by the user in
                the masterCoresMask variable in the system initialization parameters.

 @Cautions      This macro may be interpreted to a function call. When using
                it more than once in a code section, consider saving it into a
                local variable.
*//***************************************************************************/
int sys_is_master_core(void);

/**************************************************************************//**
 @Function      sys_is_master_thread

 @Description   Checks if the current core is the master core.

                In multi-thread configuration, the master core is assigned by
                the software at run-time.

 @Cautions      This macro may be interpreted to a function call. When using
                it more than once in a code section, consider saving it into a
                local variable.
*//***************************************************************************/
int sys_is_master_thread(void);

/**************************************************************************//**
 @Function      sys_get_core_secondary_threads_mask

 @Description   Returns active threads mask belong to the current core
                It is assumed Master Thread is making the call
*//***************************************************************************/
uint32_t sys_get_core_secondary_threads_mask(void);

/**************************************************************************//**
 @Function      sys_get_partition_id

 @Description   Returns the system's partition ID, as provided in the system's
                initialization parameters.
*//***************************************************************************/
uint8_t sys_get_partition_id(void);

/**************************************************************************//**
 @Function      sys_get_cores_mask

 @Description   Returns the system's active cores in a mask format.

                Core 0 is marked by the least significant bit, core 1 is marked
                by the next bit to the left, and so on.
*//***************************************************************************/
uint64_t sys_get_cores_mask(void);

/**************************************************************************//**
 @Function      sys_get_masters_mask

 @Description   Returns the mask of all cores that are masters of their partition.

                Core 0 is marked by the least significant bit, core 1 is marked
                by the next bit to the left, and so on. Note that each partition
                has only one master core.

                This function returns the true masters mask only when called
                in the master partition. Other partitions may ignore this value.
*//***************************************************************************/
uint64_t sys_get_masters_mask(void);

/**************************************************************************//**
 @Function      sys_kick_guest_partition

 @Description   TODO

 @Param[in]     coresMask       - TODO
 @Param[in]     physBaseAddr    - TODO

 @Return        E_OK on success; Error code otherwise.
*//***************************************************************************/
int sys_kick_guest_partition(uint64_t cores_mask, dma_addr_t phys_base_addr);

/* @} */ /* end of general collection */

/**************************************************************************//**
 @Collection    Spinlock Application Programming Interface

 @Description   Spinlock definitions and functions.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Spinlock structure.
*//***************************************************************************/
struct spinlock {
    volatile int    lock;   /**< Lock indicator */
};


/**************************************************************************//**
 @Function      sys_init_spinlock

 @Description   Initialize spinlock structure.

                The routine initializes the given spinlock structure as unlocked.

 @Param         p_Spinlock  -  (In) Pointer to spinlock structure to be initialize.
*//***************************************************************************/
static __inline__ void sys_init_spinlock(struct spinlock *slock)
{
    slock->lock = 0;
}

/**************************************************************************//**
 @Function      sys_lock_spinlock

 @Description   Lock the spinlock.

 @Param[in]     p_Spinlock - Pointer to spinlock structure.

 @Return        None.
*//***************************************************************************/
static __inline__ void sys_lock_spinlock(struct spinlock *slock)
{
#ifdef SYS_SMP_SUPPORT
    /* The implementation below avoids looping on atomic reservations.
       First try to acquire the lock. If the lock is taken, loop until
       the lock seems to be free, and try again. */
    while (!core_test_and_set(&(slock->lock)))
        while (slock->lock != 0) ;

    /* Wait for all previous instructions to complete */
    core_instruction_sync();
#else  /* not SYS_SMP_SUPPORT */
    /* Single core version: do nothing */
    UNUSED(slock);
#endif /* not SYS_SMP_SUPPORT */
}

/**************************************************************************//**
 @Function      sys_unlock_spinlock

 @Description   Unlock the spinlock.

 @Param[in]     p_Spinlock - Pointer to spinlock structure.

 @Return        None.
*//***************************************************************************/
static __inline__ void sys_unlock_spinlock(struct spinlock *slock)
{
#ifdef SYS_SMP_SUPPORT
    /* Memory barrier is required before spinlock is released */
    core_memory_barrier();
    slock->lock = 0;
#else  /* not SYS_SMP_SUPPORT */
    UNUSED(slock);
#endif /* not SYS_SMP_SUPPORT */
}

/**************************************************************************//**
 @Function      sys_lock_intr_spinlock

 @Description   Lock the spinlock and disable interrupts on local processor.

 @Param[in]     p_Spinlock - Pointer to spinlock structure.

 @Return        A value that represents the interrupts state before the
                operation, and should be passed to the matching
                SYS_UnlockIntrSpinlock() call.
*//***************************************************************************/
static __inline__ uint32_t sys_lock_intr_spinlock(struct spinlock *slock)
{
#ifdef SYS_SMP_SUPPORT
    register uint32_t irq_flags;

    /* The implementation below avoids looping on atomic reservations while
       interrupts are disabled. First try to acquire the lock. If the lock is
       taken, loop with interrupts enabled until the lock seems to be free,
       and try again. */
    while (1)
    {
        irq_flags = core_local_irq_save();

        if (core_test_and_set(&(slock->lock)))
            break; /* Exit with interrupts disabled */

        core_local_irq_restore(irq_flags);

        while (slock->lock != 0) ;
    }

    /* Wait for all previous instructions to complete */
    core_instruction_sync();
    return irq_flags;
#else  /* not SYS_SMP_SUPPORT */
    /* Single core version: simply disable interrupts */
    UNUSED(slock);

    return core_local_irq_save();
#endif /* not SYS_SMP_SUPPORT */
}

/**************************************************************************//**
 @Function      sys_unlock_intr_spinlock

 @Description   Unlock the spinlock and restore interrupts on local processor.

 @Param[in]     p_Spinlock  - Pointer to spinlock structure.
 @Param[in]     intrFlags   - A value that represents the interrupts state to
                              restore, as returned by the matching call for
                              SYS_LockIntrSpinlock().

 @Return        None.
*//***************************************************************************/
static __inline__ void sys_unlock_intr_spinlock(struct spinlock *slock, uint32_t irq_flags)
{
#ifdef SYS_SMP_SUPPORT
    /* Memory barrier is required before spinlock is released */
    core_memory_barrier();
    slock->lock = 0;
#else  /* not SYS_SMP_SUPPORT */
    UNUSED(slock);
#endif /* not SYS_SMP_SUPPORT */

    /* Restore interrupts */
    core_local_irq_restore(irq_flags);
}
/* @} */ /* end of spinlock collection */


/**************************************************************************//**
 @Collection    Barrier Application Programming Interface

 @Description   Definitions and functions for central program barrier.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      core_memory_barrier

 @Description   Forces the program to perform busy-wait cycles until all cores
                reach this point in the flow.

 @Param         None.
*//***************************************************************************/
#ifdef SYS_SMP_SUPPORT
void sys_barrier(void);
#else  /* not SYS_SMP_SUPPORT */
/* Single-core mode (not needed) */
#define sys_barrier()   core_memory_barrier()
#endif /* SYS_SMP_SUPPORT */

/** @} */ /* end of sys_mp_grp */
/** @} */ /* end of sys_g */


#endif /* __SYS_MP_H */
