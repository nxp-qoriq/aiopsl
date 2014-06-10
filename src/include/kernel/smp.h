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
 @Function      sys_is_cluster_master

 @Description   Checks if the current core is the cluster master core.

                In SMP configuration, the cluster master core is marked by the 
                user in the system initialization parameters.

 @Cautions      This macro may be interpreted to a function call. When using
                it more than once in a code section, consider saving it into a
                local variable.
*//***************************************************************************/
int sys_is_cluster_master(void);

/**************************************************************************//**
 @Function      sys_get_cores_mask

 @Description   Returns the system's active cores in a mask format.

                Core 0 is marked by the least significant bit, core 1 is marked
                by the next bit to the left, and so on.
*//***************************************************************************/
uint64_t sys_get_cores_mask(void);

/* @} */ /* end of general collection */

/**************************************************************************//**
 @Collection    Barrier Application Programming Interface

 @Description   Definitions and functions for central program barrier.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      sys_barrier

 @Description   Forces the program to perform busy-wait cycles until all cores
                reach this point in the flow.

 @Param         None.
*//***************************************************************************/
void sys_barrier(void);

/** @} */ /* end of sys_mp_grp */
/** @} */ /* end of sys_g */


#endif /* __SYS_MP_H */
