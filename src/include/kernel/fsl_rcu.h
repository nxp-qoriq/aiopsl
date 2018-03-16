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
@Function	rcu_synchronize

@Description	Wait until all the tasks that are holding resources are done;  
		Use rcu_read_lock() to declare your task as a resource holder.

		This is a blocking function. When this function is called, 
		the RCU lock is automatically released for this task.

@Return		0 on succees, POSIX error code otherwise  \ref error_g

@Cautions	In this function the task yields
*//***************************************************************************/
int rcu_synchronize();

/**************************************************************************//**
@Function	rcu_read_unlock

@Description	Remove task from readers list.

		When task terminates, 
		the RCU lock is automatically released for this task.

@Cautions	This function should only be called if the calling task
		does not need any resources(e.g. allocated buffers, DP objects).
		Once this function is called, there is no guarantee that these
		resources will remain valid.
*//***************************************************************************/
void rcu_read_unlock();


/**************************************************************************//**
@Function	rcu_read_lock

@Description	Add the task back to the readers list.

@Cautions	AIOP task needs to call rcu_read_lock() deliberately in order
		to declare that is uses some of the resources
		(e.g. allocated buffers, DP objects).
@Cautions	This function may result in a fatal error. Do not call it from
		the AIOP initialization task.

*//***************************************************************************/
void rcu_read_lock();

/** @} */ /* end of rcu_g */

#endif /* __FSL_RCU_H */
