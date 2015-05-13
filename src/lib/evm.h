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
@File		evm.h

@Description	Event Manager API
*//***************************************************************************/

#ifndef __EVM_H
#define __EVM_H

#include "fsl_evm.h"
#include "fsl_sl_dbg.h"
#include "types.h"


#define MINIMUM_PRIORITY 2
#define MAXIMUM_PRIORITY 8
#define EVM_SL_REGISTRATION_FLAG  0
#define EVM_APP_REGISTRATION_FLAG 1


enum evm_all_event_types {
	DPRC_EVENT_OBJ_ADDED = NUM_OF_USER_EVENTS,
	DPRC_EVENT_OBJ_REMOVED,
	DPNI_EVENT_LINK_CHANGE,
	DPCI_EVENT_LINK_CHANGE,
	NUM_OF_ALL_EVENTS
};

enum evm_objects {
	DPRC = 0,
	DPNI,
	DPCI,
	NUM_OF_EVM_OBJECTS
};


/**************************************************************************//**
@Function	evm_sl_register

@Description	This function is to register a callback function to listen for
		specific events.

@Param[in]	generator_id  Identifier of the application generating event.

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to 255.
		A unique combination of generator_id and event_id corresponds
		to a unique event in the system.

@Param[in]	priority  priority number of the callback function.
		The lesser value is considered as higher priority. For example,
		a callback function registered with priority 10 will be invoked
		before a callback function registered with priority 20.

@param[in]	cb  Callback function to be invoked.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evm_sl_register(
		uint8_t generator_id,
		uint8_t event_id,
		uint8_t priority,
		evm_cb cb);

/**************************************************************************//**
@Description Structure representing linked list per event_id sorted by priority.

*//***************************************************************************/
struct evm_priority_list {
	/** priority of the event*/
	uint8_t generator_id;
	/** priority of the event*/
	uint8_t priority;
	/** Callback function to be called when event arrived */
	evm_cb cb;
	/** Pointer to the next list with callback function for the same event
	 * and higher or equal priority*/
	struct evm_priority_list *next;
};

/**************************************************************************//**
@Description Structure for specific event id with a pointer to linked list of
		callback functions sorted by priority.

*//***************************************************************************/
struct evm{
	/** Identifier of the specific event */
	uint8_t event_id;
	/** Number of registered callback functions */
	uint8_t num_cbs;
	/** Pointer to the first list with callback function for the same event
	 * sorted by priority*/
	struct evm_priority_list *head;
};

int evm_early_init(void);
int evm_init(void);
void evm_free(void);

#endif /* __EVM_H */
