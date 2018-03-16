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
@File		evmng.h

@Description	Event Manager API
*//***************************************************************************/

#ifndef __EVMNG_H
#define __EVMNG_H

#include "fsl_sl_evmng.h"
#include "fsl_sl_dbg.h"
#include "fsl_types.h"

/** Number of average registrations allowed per event*/
#define EVMNG_NUM_OF_REGISTRATIONS_PER_EVENT                1
/** Sum of all events in the system */
#define EVMNG_MAX_NUM_OF_EVENTS (NUM_OF_SL_DEFINED_EVENTS + 128)

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
	uint64_t val;
};

#endif /* __EVMNG_H */
