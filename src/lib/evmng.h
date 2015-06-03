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
@File		evmng.h

@Description	Event Manager API
*//***************************************************************************/

#ifndef __EVMNG_H
#define __EVMNG_H

#include "fsl_evmng.h"
#include "fsl_sl_dbg.h"
#include "types.h"


enum evm_irq_event_types {
	DPRC_EVENT = 0,
	DPNI_EVENT,
	DPCI_EVENT,
	NUM_OF_IRQ_EVENTS
};

/** Number of average registrations allowed per event*/
#define EVMNG_NUM_OF_REGISTRATIONS_PER_EVENT                1
/** Sum of all events in the system */
#define EVMNG_MAX_NUM_OF_EVENTS (NUM_OF_SL_DEFINED_EVENTS + 128)

/**************************************************************************//**
@Function	evmng_irq_register

@Description	This function is to register a callback function to listen for
		specific events.

@Param[in]	generator_id  Identifier of the application/SL generating the
		event

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to:
		NUM_OF_IRQ_EVENTS -1


@Param[in]	priority  priority number of the callback function.
		The lesser value is considered as higher priority. For example,
		a callback function registered with priority 10 will be invoked
		before a callback function registered with priority 20.

@Param[in]	app_ctx User/SL data that can be passed to CB function when raising
		event.

@param[in]	cb  Callback function to be invoked.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evmng_irq_register(
		uint8_t generator_id,
		uint8_t event_id,
		uint8_t priority,
		uint64_t app_ctx,
		evmng_cb cb);

/**************************************************************************//**
@Function	evmng_irq_unregister

@Description	This function is to unregister a callback function from
		listening for specific events.

@Param[in]	generator_id  Identifier of the application/SL generating the
		event

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to:
		NUM_OF_IRQ_EVENTS -1

@Param[in]	priority  priority number of the callback function.
		The lesser value is considered as higher priority. For example,
		a callback function registered with priority 10 will be invoked
		before a callback function registered with priority 20.

@Param[in]	app_ctx User/SL data that can be passed to CB function when raising
		event.

@param[in]	cb  Callback function to be invoked.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evmng_irq_unregister(
		uint8_t generator_id,
		uint8_t event_id,
		uint8_t priority,
		uint64_t app_ctx,
		evmng_cb cb);

/**************************************************************************//**
@Description Structure representing linked list per event_id sorted by priority.

*//***************************************************************************/
struct evmng_priority_list {
	/** priority of the event*/
	uint8_t priority;
	/** data to be passed with CB (can be user / SL data)*/
	uint64_t app_ctx;
	/** Callback function to be called when event arrived */
	evmng_cb cb;
	/** Pointer to the next list with callback function for the same event
	 * and higher or equal priority*/
	struct evmng_priority_list *next;
};

/**************************************************************************//**
@Description Structure for specific event id with a pointer to linked list of
		callback functions sorted by priority.

*//***************************************************************************/
struct evmng{
	/** Identifier of the application/module*/
	uint8_t generator_id;
	/** Identifier of the specific event */
	uint8_t event_id;
	/** Pointer to the first list with callback function for the same event
	 * sorted by priority*/
	struct evmng_priority_list *head;
};

/**************************************************************************//**
@Description Structure representing IRQ parameters passed with cmdif.

*//***************************************************************************/
struct evmng_irq_params {
	/** addr - address for the interrupt, provided by IRQ*/
	uint64_t addr;
	/** val - interrupt value provided via IRQ API*/
	uint32_t val;
};

/**************************************************************************//**
@Function	evmng_raise_irq_event_cb

@Description	event manager callback function to process registered events.

@Param[in]	dev  Identifier of module generating event.

@Param[in]	cmd  Identifier of the specific event

@param[in]	size  size of event data.

@param[in]	event_data  data specific for event

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evmng_raise_irq_event_cb(void *dev, uint16_t cmd, uint32_t size, void *event_data);

/**************************************************************************//**
@Function	evmng_sl_raise_event

@Description	This function raises a specific event and launches the callback
		functions registered to it.

@Param[in]	generator_id  Identifier of the application/SL generating the
		event

@Param[in]	event_id  Identifier of the event specific to the application
		generating event. The value can range from 0 to MAX_EVENT_ID -1
		\ref EVM_EVENT_TYPES

@Param[in]	event_data  A pointer to data specific for event

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int evmng_sl_raise_event(uint8_t generator_id, uint8_t event_id, void *event_data);

int evmng_early_init(void);
int evmng_init(void);
void evmng_free(void);

#endif /* __EVMNG_H */
