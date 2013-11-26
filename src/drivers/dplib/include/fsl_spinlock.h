/**************************************************************************//**
@File		fsl_spinlock.h

@Description	This file contains the AIOP SW Spinlock Operations API
*//***************************************************************************/

#ifndef __FSL_LIB_SPINLOCK_H
#define __FSL_LIB_SPINLOCK_H

#include "general.h"

/**************************************************************************//**
 @Group		FSL_AIOP_SPINLOCK

 @Description	FSL AIOP Spinlock macros and functions

 @{
*//***************************************************************************/


/**************************************************************************//**
@Group			Spinlock_Functions

@Description	AIOP Spinlock operations functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	lock_spinlock

@Description	Lock the spinlock. Lock 1 byte in internal shared memory.

		Needed for locking of critical code sections across the flows
		where the critical sections cannot be protected using ordering
		flow locks. Can be use also for synchronization between tasks.

		The user has to allocate one byte (in the shared memory) per
		spinlock. This byte has to be initialized to "0" or "1"
		("unlocked" or "locked" respectively).

		If the lock is available, the "locked" byte is set and the code
		continues into the critical section. If, instead, the lock has
		been taken by other task, the code goes into a loop where it
		repeatedly checks the lock until it becomes available.

@Param[in]	spinlock - Pointer to a single byte ("locked" byte) in the
		shared memory.

@Return		None.

@Cautions	It is not allowed to yield if the task holds a lock.
@Cautions	Every lock_spinlock function must be followed by an
		unlock_spinlock function to release the memory locking.

*//***************************************************************************/
void lock_spinlock(register uint8_t *spinlock);

/*************************************************************************//**
@Function	unlock_spinlock

@Description	Unlock the spinlock. Unlock 1 byte in internal shared memory.

		The spinlock "locked" byte is atomically unlocked.

@Param[in]	spinlock - Pointer to a single byte ("locked" byte) in the
		shared memory.

@Return		None.

@Cautions	Every lock_spinlock function must be followed by an
		unlock_spinlock function to release the memory locking

*//***************************************************************************/
void unlock_spinlock(uint8_t *spinlock);

/*************************************************************************//**
@Function	aiop_atomic_incr8

@Description	Atomic increment macro for 8 bit variable.

@Param[in]	var - pointer to the variable to increment.
@Param[in]	value - increment value.

@Return		None.

*//***************************************************************************/
void aiop_atomic_incr8(int8_t *var, int8_t value);

/*************************************************************************//**
@Function	aiop_atomic_incr16

@Description	Atomic increment macro for 16 bit variable.

@Param[in]	var - pointer to the variable to increment.
@Param[in]	value - increment value.

@Return		None.

*//***************************************************************************/
void aiop_atomic_incr16(int16_t *var, int16_t value);

/*************************************************************************//**
@Function	aiop_atomic_incr32

@Description	Atomic increment macro for 32 bit variable.

@Param[in]	var - pointer to the variable to increment.
@Param[in]	value - increment value.

@Return		None.

*//***************************************************************************/
void aiop_atomic_incr32(int32_t *var, int32_t value);

/*************************************************************************//**
@Function	aiop_atomic_decr8

@Description	Atomic decrement macro for 8 bit variable.

@Param[in]	var - pointer to the variable to decrement.
@Param[in]	value - decrement value.

@Return		None.

*//***************************************************************************/
void aiop_atomic_decr8(int8_t *var, int8_t value);

/*************************************************************************//**
@Function	aiop_atomic_decr16

@Description	Atomic decrement macro for 16 bit variable.

@Param[in]	var - pointer to the variable to decrement.
@Param[in]	value - decrement value.

@Return		None.

*//***************************************************************************/
void aiop_atomic_decr16(int16_t *var, int16_t value);

/*************************************************************************//**
@Function	aiop_atomic_decr32

@Description	Atomic decrement macro for 32 bit variable.

@Param[in]	var - pointer to the variable to decrement.
@Param[in]	value - decrement value.

@Return		None.

*//***************************************************************************/
void aiop_atomic_decr32(int32_t *var, int32_t value);


/** @} end of group Spinlock_Functions */

/** @} */ /* end of FSL_AIOP_SPINLOCK */

#endif /* __FSL_LIB_SPINLOCK_H */

