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

/**************************************************************************//**
@File		fsl_rcu.h

@Description	This file contains the API for Read-Copy-Update
		synchronization mechanism

*//***************************************************************************/

#ifndef __FSL_RCU_H
#define __FSL_RCU_H

#include "fsl_types.h"

/**************************************************************************//**
@Group		rcu_g RCU

@Description	Contains the API for RCU synchronize functionality

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	rcu_early_init

@Description	RCU parameters that should be passed by each application
		at early init

@Param[in]	delay	Delay in milliseconds until the first RCU task creation.
  	  	  	The longer delay the more RCU jobs will be handled by
  	  	  	one RCU task.

@Param[in]	committed	Committed size of the RCU jobs list
@Param[in]	max		RCU list can get up to max size if committed
				is not enough

@Return		0 on succees, POSIX error code otherwise  \ref error_g

*//***************************************************************************/
int rcu_early_init(uint16_t delay, uint32_t committed, uint32_t max);

/**************************************************************************//**
@Description	Callback to be called after all the current tasks are done

@Param[in]	param   User parameter as passed to rcu_synchronize()
 *//***************************************************************************/
typedef void (rcu_cb_t)(uint64_t param);

/**************************************************************************//**
@Function	rcu_synchronize

@Description	Activate the callback after all the active AIOP tasks
		(except those that called rcu_read_unlock) are done

@Param[in]	cb	Callback to be called after all the active readers
			are done
@Param[in]	param	Callback parameter

@Return		0 on succees, POSIX error code otherwise  \ref error_g

@Cautions	Set APP_RCU_COMMITTED, APP_RCU_MAX, APP_RCU_TIMER_DURATION at
		apps.h before using the RCU module
*//***************************************************************************/
int rcu_synchronize(rcu_cb_t *cb, uint64_t param);

/**************************************************************************//**
@Function	rcu_read_unlock

@Description	Remove task from readers list

@Cautions	This function should only be called if the calling task
		does not need any resources(e.g. allocated buffers, DP objects).
		Once this function is called, there is no guarantee that these
		resources will remain valid.

		Every AIOP task that is created automatically holds an
		RCU read lock.
*//***************************************************************************/
void rcu_read_unlock();


/**************************************************************************//**
@Function	rcu_read_lock

@Description	Add the task back to the readers list;

@Cautions	Every AIOP task that is created automatically holds an
		RCU read lock. This function reverts the rcu_read_unlock().
*//***************************************************************************/
void rcu_read_lock();

/** @} */ /* end of rcu_g */

#endif /* __FSL_RCU_H */
