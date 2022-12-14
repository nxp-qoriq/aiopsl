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
@File		fsl_evmng.h

@Description	Event Manager API
*//***************************************************************************/

#ifndef __FSL_EVMNG_H
#define __FSL_EVMNG_H

#include "fsl_types.h"

/**************************************************************************//**
@Group		evmng_g Event Manager

@Description	EVMNG API for registration to application and service layer
		events and the ability to raise application events by priority.

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		EVMNG_EVENT_TYPES Events provided by AIOPSL

 @Description	EVMNG events for application use.

 @{
 *//***************************************************************************/

enum evm_event_types {
	DPNI_EVENT_ADDED = 0,
	DPNI_EVENT_REMOVED,
	DPNI_EVENT_LINK_UP,
	DPNI_EVENT_LINK_DOWN,
	DPCI_EVENT_ADDED,
	DPCI_EVENT_REMOVED,
	DPCI_EVENT_LINK_UP,
	DPCI_EVENT_LINK_DOWN,
	NUM_OF_SL_DEFINED_EVENTS,
};

/** @} end of group EVMNG_EVENT_TYPES */

/**************************************************************************//**
 @Group		EVMNG_AIOPSL_GENERATOR_ID AIOPSL Generator ID

 @Description	Generator ID for AIOPSL events.

 @{
*//***************************************************************************/
/** AIOP service layer generator ID (0 means the entry is empty)*/
#define EVMNG_GENERATOR_AIOPSL                              1
/** @} end of group EVMNG_AIOPSL_GENERATOR_ID */

/**************************************************************************//**
@Description	Prototype of event manager callback function. An application
		provides a callback function of this prototype if it wants to
		listen for specific events.

@Param[in]	generator_id  Identifier of the application/SL generating the
		event

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to MAX_EVENT_ID -1.
		\ref EVMNG_EVENT_TYPES

@Param[in]	app_ctx  App/SL data saved during registration and passed to CB
		function when raising event.

@param[in]	event_data  A pointer to data specific for event

@Return		The return code is not interpreted by event manager.
		However callback function should return 0.
*//***************************************************************************/
typedef int (*evmng_cb)(uint8_t generator_id,
			uint8_t event_id,
			uint64_t app_ctx,
			void *event_data);

/**************************************************************************//**
@Function	evmng_register

@Description	This function is to register a callback function to listen for
		specific events.

@Param[in]	generator_id  Identifier of the application/SL generating the
		event

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to MAX_EVENT_ID -1.
		\ref EVMNG_EVENT_TYPES.
		To use application defined events, the provided event id must be
		in range of [NUM_OF_SL_DEFINED_EVENTS , MAX_EVENT_ID -1]

@Param[in]	priority  priority number of the callback function.
		This value ranges from 1 - 127.
		The lesser value is considered as higher priority. For example,
		a callback function registered with priority 10 will be invoked
		before a callback function registered with priority 20.

@Param[in]	app_ctx  App data that can be passed to CB function when
		raising event.

@param[in]	cb  Callback function to be invoked.

@Cautions	The application can register for event only in app_init or at
		runtime.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evmng_register(
		uint8_t generator_id,
		uint8_t event_id,
		uint8_t priority,
		uint64_t app_ctx,
		evmng_cb cb);

/**************************************************************************//**
@Function	evmng_unregister

@Description	This function is to unregister previously callback function.

@Param[in]	generator_id  Identifier of the application/SL generating the
		event

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to MAX_EVENT_ID -1.
		\ref EVMNG_EVENT_TYPES

@Param[in]	priority  priority number of the callback function.
		This value ranges from 1 - 127.
		The lesser value is considered as higher priority. For example,
		a callback function registered with priority 10 will be invoked
		before a callback function registered with priority 20.

@Param[in]	app_ctx App data that can be passed to CB function when
		raising event.

@param[in]	cb  Callback function to be invoked.

@Cautions	The application can unregister from event only in app_init or at
		runtime.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evmng_unregister(
		uint8_t generator_id,
		uint8_t event_id,
		uint8_t priority,
		uint64_t app_ctx,
		evmng_cb cb);

/**************************************************************************//**
@Function	evmng_raise_event

@Description	This function raises a specific event and launches the callback
		functions registered to it.

@Param[in]	generator_id  Identifier of the application/SL generating the
		event

@Param[in]	event_id  Identifier of the event specific to the application
		generating event.
		The value can range from NUM_OF_SL_DEFINED_EVENTS to
		MAX_EVENT_ID -1. See \ref EVMNG_EVENT_TYPES

@Param[in]	event_data  A pointer to data specific for event

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evmng_raise_event(uint8_t generator_id, uint8_t event_id, void *event_data);

/**************************************************************************//**
@Function	bpscn_callback

@Description	Function is called when an AIOP task is created upon reception
		of a BPSCN (Buffer Pool State Change Notification) message.
		Once a task has been created to handle a depletion state
		transition, another task will not be created, for the same BPID,
		(Buffer Pool ID) until the current one has terminated.
		Depletion state changes that occur while the task is operating
		are tracked and will influence the next task creation action.

		The AIOP SL implementation prints an warning message before the
		BPSCN task is terminated.

		To perform the application depending BPSCN processing, this
		function must be overridden by the application.
		To get the BPSCN notification one must configure the buffer
		pool depletion parameters by calling the API function
			dpni_drv_set_pool_depletion()
		in the application initialization phase, on the depletion
		tracked buffer pools.  BPSNC task are created only if the
		DPNI_DRV_DEPL_NOTIF_OPT_AIOP option is set.

		When a BPSCN task is created, the following parameters are set
		in its hardware environment :
			PRC : NDS = 1
			ADQ : BDI = AIOP BDI
			ADQ : ICID = AIOP ICID
			FD : SL = 1
			FD : FMT = 3 (BPSCN task)
			FD : FLC
				Bit 48 : Depletion state
				Bits 45-32 : BPID
				Bits 9-0 : EPID = 0x3FE
		BPSCN tasks are executed in the exclusive mode. The ordering
		scope ID is BPID << 16.
*//***************************************************************************/
__declspec(weak) __HOT_CODE void bpscn_callback(void);

/**************************************************************************//**
@Function	cscn_callback

@Description	Function is called when an AIOP task is created upon reception
		of a CSCN (Congestion State Change Notification) message.
		Once a task has been created to handle a congestion state
		transition, another task will not be created, for the same CGID,
		(Congestion Group ID) until the current one has terminated.
		Congestion state changes that occur while the task is operating
		are tracked and will influence the next task creation action.

		The AIOP SL implementation prints an warning message before the
		CSCN task is terminated.

		To perform the application depending CSCN processing, this
		function must be overridden by the application.
		To get the CSCN notification one must configure the congestion
		parameters by calling the API function
			dpni_drv_set_congestion_notification()
		in the application initialization phase, on the RX queues of the
		congestion tracked DPNIs. CSNC task are created only if the
		DPNI_CONG_OPT_NOTIFY_AIOP option is set.

		When a CSCN task is created, the following parameters are set in
		its hardware environment :
			PRC : NDS = 1
			ADQ : BDI = AIOP BDI
			ADQ : ICID = AIOP ICID
			FD : SL = 1
			FD : FMT = 2 (CSCN task)
			FD : FLC
				Bit 48 : Congestion state
				Bits 47-32 : CGID
				Bits 9-0 : EPID = 0x3FF
		CSCN tasks are executed in the exclusive mode. The ordering
		scope ID is CGID << 16.
*//***************************************************************************/
__declspec(weak) __HOT_CODE void cscn_callback(void);

/** @} */ /* end of evmng_g Event Manager group */
#endif /* __FSL_EVMNG_H */
