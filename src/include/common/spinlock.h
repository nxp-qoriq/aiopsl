/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          spinlock.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_SPINLOCK_H
#define __FSL_SPINLOCK_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_os_g  FSL OS Interface (System call hooks)

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_os_spinlock_g  FSL OS Spinlock API

 @Description   TODO

 @{
*//***************************************************************************/

typedef fsl_handle_t spinlock_t;

/**************************************************************************//**
 @Function      spin_lock_create

 @Description   Creates a spinlock.

 @Return        A Spinlock handle is returned on success; NULL otherwise.
*//***************************************************************************/
spinlock_t spin_lock_create(void);

/**************************************************************************//**
 @Function      spin_lock_init

 @Description   Initialize a spinlock.

 @Param[in]     lock - A handle to a spinlock.
*//***************************************************************************/
void spin_lock_init(spinlock_t lock);

/**************************************************************************//**
 @Function      spin_lock_free

 @Description   Frees the memory allocated for the spinlock creation.

 @Param[in]     lock - A handle to a spinlock.
*//***************************************************************************/
void spin_lock_free(spinlock_t lock);

/**************************************************************************//**
 @Function      spin_lock

 @Description   Locks a spinlock.

 @Param[in]     lock - A handle to a spinlock.
*//***************************************************************************/
void spin_lock(spinlock_t lock);

/**************************************************************************//**
 @Function      spin_unlock

 @Description   Unlocks a spinlock.

 @Param[in]     lock - A handle to a spinlock.
*//***************************************************************************/
void spin_unlock(spinlock_t lock);

/**************************************************************************//**
 @Function      spin_lock_irqsave

 @Description   Locks a spinlock (interrupt safe).

 @Param[in]     lock - A handle to a spinlock.

 @Return        A value that represents the interrupts state before the
                operation, and should be passed to the matching
                spin_unlock_irqrestore() call.
*//***************************************************************************/
uint32_t spin_lock_irqsave(spinlock_t lock);

/**************************************************************************//**
 @Function      spin_unlock_irqrestore

 @Description   Unlocks a spinlock (interrupt safe).

 @Param[in]     lock  - A handle to a spinlock.
 @Param[in]     irqFlags   - A value that represents the interrupts state to
                              restore, as returned by the matching call for
                              spin_lock_irqsave().
*//***************************************************************************/
void spin_unlock_irqrestore(spinlock_t lock, uint32_t irq_flags);

/** @} */ /* end of fsl_os_spinlock_g group */
/** @} */ /* end of fsl_os_g group */


#endif /* __FSL_SPINLOCK_H */
