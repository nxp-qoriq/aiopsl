/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
 @file          fsl_spinlock.h

 @Description   This file contains the AIOP SW Spinlock Operations API
*//***************************************************************************/

#ifndef __FSL_LIB_SPINLOCK_H
#define __FSL_LIB_SPINLOCK_H

#include "fsl_types.h"

/**************************************************************************//**
 @Group			Synchronization_g_id Synchronization

 @Description	System synchronization macros and functions

 @{
*//***************************************************************************/


/**************************************************************************//**
@Group			Spinlock_Functions Spin-lock functions 

@Description	AIOP Spin-lock operations functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	lock_spinlock

@Description	Lock the spinlock. Lock 1 double word in internal shared memory.
		Needed for locking of critical code sections across the flows
		where the critical sections cannot be protected using ordering
		flow locks. Can be use also for synchronization between tasks.

		The user has to allocate one double word (in the shared memory)
		per spinlock. This double word has to be initialized
		to "0" or "1" ("unlocked" or "locked" respectively).

		If the lock is available, the "locked" double word is set and
		the code continues into the critical section. If, instead,
		the lock has been taken by other task, the code goes into
		loop where it repeatedly checks the lock until it becomes
		available.

@Param[in]	spinlock - Pointer to a single double word
			   ("locked" double word) in the shared memory.

@Return		None.

@Cautions	It is not allowed to yield if the task holds a lock.
@Cautions	Every lock_spinlock function must be followed by an
		unlock_spinlock function to release the memory locking.
@Cautions	The spinlock must be aligned to a double word boundary
		otherwise an alignment exception will occur.

*//***************************************************************************/
void lock_spinlock(register uint64_t *spinlock);

/*************************************************************************//**
@Function	unlock_spinlock

@Description	Unlock the spinlock.
		Unlock 1 double word in internal shared memory.
		The spinlock "locked" double word is atomically unlocked.

@Param[in]	spinlock - Pointer to a single double word
			   ("locked" double word) in the shared memory.

@Return		None.

@Cautions	Every lock_spinlock function must be followed by an
		unlock_spinlock function to release the memory locking

*//***************************************************************************/
void unlock_spinlock(uint64_t *spinlock);

/** @} end of group Spinlock_Functions */

/**************************************************************************//**
@Group			atomic_operations Atomic operations

@Description	atomic increment / decrement operations

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	atomic_incr64

@Description	Atomic increment macro for 64 bit variable.

@Param[in,out]	var   - pointer to the variable to increment.
@Param[in]	value - increment value.

@Return		None.

@Cautions	The variable must be aligned to a double word boundary
		otherwise an alignment exception will occur.

*//***************************************************************************/
void atomic_incr64(int64_t *var, int64_t value);

/*************************************************************************//**
@Function       atomic_decr8

@Description    Atomic decrement macro for 64 bit variable.

@Param[in,out]  var   - pointer to the variable to decrement.
@Param[in]      value - decrement value.

@Return         None.

@Cautions	The variable must be aligned to a double word boundary
		otherwise an alignment exception will occur.

*//***************************************************************************/
void atomic_decr64(int64_t *var, int64_t value);

/** @} */ /* end of atomic_operations */
/** @} */ /* end of FSL_AIOP_SPINLOCK */

#endif /* __FSL_LIB_SPINLOCK_H */

