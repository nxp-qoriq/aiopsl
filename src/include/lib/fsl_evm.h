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
@File		fsl_evm.h

@Description	Event Manager API
*//***************************************************************************/

#ifndef __FSL_EVM_H
#define __FSL_EVM_H

#include "types.h"

/**************************************************************************//**
@Group		evm_g Event Manager

@Description	Contains initialization APIs and runtime control APIs for Event
		Manager

@{
*//***************************************************************************/

enum evm_types {
	AIOP_DPRC_ASSIGN = 0,
	AIOP_DPRC_UNASSIGN,
	AIOP_DPNI_LINK_CHANGE,
	AIOP_DPCI_LINK_CHANGE,
	AIOP_NUM_OF_EVENTS
};

enum evm_objects {
	AIOP_DPRC = 0,
	AIOP_DPNI,
	AIOP_DPCI,
	AIOP_NUM_OF_OBJECTS
};

/**************************************************************************//**
@Description	Prototype of event manager callback function. An application
		provides a callback function of this prototype if it wants to
		listen for specific events.

@Param[in]	generator_id  Identifier of the application generating event.

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to 255.
		A unique combination of generator_id and event_id corresponds
		to a unique event in the system.

@param[in]	size  size of event data.

@param[in]	event_data  A pointer to structure specific for event


@Return		The return code is not interpreted by event manager.
		However callback function should return 0.
*//***************************************************************************/
typedef int (*evm_cb)(
			uint8_t generator_id,
			uint16_t event_id,
			uint32_t size,
			void *event_data);

/**************************************************************************//**
@Function	evm_app_register

@Description	This function is to register a callback function to listen for
		specific events.

@Param[in]	generator_id  Identifier of the application generating event.

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to 255.
		A unique combination of generator_id and event_id corresponds
		to a unique event in the system.

@Param[in]	priority  priority number of the callback function.
		This value ranges from MINIMUM_PRIORITY to MAXIMUM_PRIORITY.
		The lesser value is considered as higher priority. For example,
		a callback function registered with priority 10 will be invoked
		before a callback function registered with priority 20.

@param[in]	cb  Callback function to be invoked.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evm_app_register(
		uint8_t generator_id,
		enum evm_types event_id,
		uint8_t priority,
		evm_cb cb);

/**************************************************************************//**
@Function	evm_unregister

@Description	This function is to unregister previously callback function.

@Param[in]	generator_id  Identifier of the application given at the time
		of registration.

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to 255.
		A unique combination of generator_id and event_id corresponds
		to a unique event in the system.

@Param[in]	priority  priority number of the callback function.
		This value ranges from MINIMUM_PRIORITY to MAXIMUM_PRIORITY.
		The lesser value is considered as higher priority. For example,
		a callback function registered with priority 10 will be invoked
		before a callback function registered with priority 20.

@param[in]	cb  Callback function to be invoked.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evm_unregister(
		uint8_t generator_id,
		enum evm_types event_id,
		uint8_t priority,
		evm_cb cb);

/** @} */ /* end of evm_g Event Manager group */
#endif /* __FSL_EVM_H */
