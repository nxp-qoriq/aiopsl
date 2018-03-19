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

#include "evmng.h"
#include "evmng_common.h"
#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "fsl_malloc.h"
#include "fsl_string.h"
#include "fsl_spinlock.h"
#include "fsl_cdma.h"

struct evmng g_evmng_irq_events_list[NUM_OF_IRQ_EVENTS];
struct evmng g_evmng_events_list[EVMNG_MAX_NUM_OF_EVENTS];
int g_evmng_events_last_used_index;
uint32_t *g_evmng_b_pool_pointer;
uint32_t *g_evmng_first_b_pool_pointer;
uint32_t *g_evmng_last_b_pool_pointer;

/* The lock must be aligned to a double word boundary. */
uint64_t g_evmng_b_pool_spinlock __attribute__((aligned(8)));

extern int cmdif_srv_mc_evm_session_open();

static int add_event_registration(
	uint8_t priority, uint64_t app_ctx, evmng_cb cb,
	struct evmng *evmng_ptr)
{
	struct evmng_priority_list *evmng_cb_list;
	struct evmng_priority_list *evmng_cb_list_ptr;
	struct evmng_priority_list *evmng_cb_list_tmp_ptr;

	/*Lock spinlock to take the next address for buffer to list
	 * of registration request*/
	lock_spinlock(&g_evmng_b_pool_spinlock);
	/* If the number of allocated list reach the limit, return error*/
	if(g_evmng_b_pool_pointer == g_evmng_last_b_pool_pointer){
		unlock_spinlock(&g_evmng_b_pool_spinlock);
		return -ENOMEM;
	}
	evmng_cb_list = (struct evmng_priority_list *)*g_evmng_b_pool_pointer;
	g_evmng_b_pool_pointer ++;
	/*Unlock spinlock*/
	unlock_spinlock(&g_evmng_b_pool_spinlock);

	evmng_cb_list->app_ctx = app_ctx;
	evmng_cb_list->cb = cb;
	evmng_cb_list->priority = priority;
	evmng_cb_list->next = NULL;

	if(evmng_ptr->head != NULL){
		sl_pr_debug("head is not null\n");
		evmng_cb_list_ptr = evmng_ptr->head;
		evmng_cb_list_tmp_ptr = evmng_cb_list_ptr;

		while(evmng_cb_list_ptr->next != NULL &&
			evmng_cb_list_ptr->priority <= evmng_cb_list->priority)
		{
			evmng_cb_list_tmp_ptr = evmng_cb_list_ptr;
			evmng_cb_list_ptr = evmng_cb_list_ptr->next;
		}

		if(evmng_cb_list_ptr->priority > evmng_cb_list->priority){
			/*insert the list with cb function between:
			 *  evmng_cb_list_tmp_ptr->"evmng_cb_list"->evmng_cb_list_ptr*/
			evmng_cb_list->next = evmng_cb_list_ptr;
			if(evmng_cb_list_tmp_ptr != evmng_ptr->head){
				evmng_cb_list_tmp_ptr->next = evmng_cb_list;

			}
			else{
				/*Become first in the list*/
				evmng_ptr->head = evmng_cb_list;
			}
		}
		else{
			evmng_cb_list_ptr->next = evmng_cb_list;
		}
	}
	else{
		sl_pr_debug("head is null\n");
		evmng_ptr->head = evmng_cb_list;
	}
	sl_pr_debug("Registered successfully for event\n");
	return 0;
}
/*****************************************************************************/

int evmng_irq_register(
	uint8_t generator_id, uint8_t event_id,
	uint8_t priority, uint64_t app_ctx, evmng_cb cb)
{
	int err;

	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= NUM_OF_IRQ_EVENTS || generator_id != EVMNG_GENERATOR_AIOPSL){
		sl_pr_debug("event or generator id value is out of bounds\n");
		return -EINVAL;
	}

	/* Find the index in table which has the same generator and event id or
	 * find the first empty index.
	 * The table should be locked.*/
	/* Lock EVM table*/
	cdma_mutex_lock_take((uint64_t) g_evmng_irq_events_list, CDMA_MUTEX_WRITE_LOCK);
	err = add_event_registration(priority, app_ctx, cb, &g_evmng_irq_events_list[event_id]);
	cdma_mutex_lock_release((uint64_t) g_evmng_irq_events_list);
	return err;
}
/*****************************************************************************/

int evmng_register(
	uint8_t generator_id, uint8_t event_id,
	uint8_t priority, uint64_t app_ctx, evmng_cb cb)
{
	struct evmng *evmng_ptr;
	int i, empty_index = -1, err;

	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}

	/* Find the index in table which has the same generator and event id or
	 * find the first empty index.
	 * The table should be locked.*/
	/* Lock EVM table*/
	cdma_mutex_lock_take((uint64_t) g_evmng_events_list, CDMA_MUTEX_WRITE_LOCK);

	for(i = 0; i < g_evmng_events_last_used_index; i++ )
	{
		if(g_evmng_events_list[i].generator_id == generator_id &&
			g_evmng_events_list[i].event_id == event_id){
			break;
		}
		if(g_evmng_events_list[i].generator_id == NULL &&
			empty_index == -1){
			empty_index = i;
		}
	}

	if(i < g_evmng_events_last_used_index)
	{
		evmng_ptr = &g_evmng_events_list[i];
	}
	/* empty entry was found during search of used indexes*/
	else if(empty_index >= 0)
	{
		g_evmng_events_list[empty_index].generator_id = generator_id;
		g_evmng_events_list[empty_index].event_id = event_id;
		evmng_ptr = &g_evmng_events_list[empty_index];
	}
	else if(g_evmng_events_last_used_index < EVMNG_MAX_NUM_OF_EVENTS)
	{
		empty_index = g_evmng_events_last_used_index;
		g_evmng_events_list[empty_index].generator_id = generator_id;
		g_evmng_events_list[empty_index].event_id = event_id;
		evmng_ptr = &g_evmng_events_list[empty_index];
		g_evmng_events_last_used_index ++;
	}
	else
	{
		cdma_mutex_lock_release((uint64_t) g_evmng_events_list);
		return -ENOMEM;
	}

	err = add_event_registration(priority, app_ctx, cb, evmng_ptr);

	/* if err != 0, check if the generator ID need to be cleared again */
	/* i == EVMNG_MAX_NUM_OF_EVENTS: The event was not found at the list.*/
	if(err && i == EVMNG_MAX_NUM_OF_EVENTS && empty_index >= 0)
	{
		g_evmng_events_list[empty_index].generator_id = NULL;

	}
	cdma_mutex_lock_release((uint64_t) g_evmng_events_list);
	return err;
}
/*****************************************************************************/

static int remove_event_registration(struct evmng *evmng_ptr,
                                     uint8_t priority,
                                     uint64_t app_ctx,
                                     evmng_cb cb)
{
	struct evmng_priority_list *evmng_cb_list_ptr;
	struct evmng_priority_list *evmng_cb_list_tmp_ptr;

	if(evmng_ptr->head == NULL)
		return -ENAVAIL;

	evmng_cb_list_ptr = evmng_ptr->head;
	evmng_cb_list_tmp_ptr = evmng_cb_list_ptr;

	while(evmng_cb_list_ptr != NULL){
		if(evmng_cb_list_ptr->cb == cb &&
			evmng_cb_list_ptr->priority == priority &&
			evmng_cb_list_ptr->app_ctx == app_ctx){
			break;
		}
		evmng_cb_list_tmp_ptr = evmng_cb_list_ptr;
		evmng_cb_list_ptr = evmng_cb_list_ptr->next;
	}

	if(evmng_cb_list_ptr == NULL){
		sl_pr_err("Event registration was not found\n");
		return -ENAVAIL;
	}

	if(evmng_cb_list_ptr != evmng_ptr->head)
	{
		if(evmng_cb_list_ptr->next != NULL){
			evmng_cb_list_tmp_ptr->next = evmng_cb_list_ptr->next;
		}
		else{
			evmng_cb_list_tmp_ptr->next = NULL;
		}
	}
	else{
		if(evmng_cb_list_ptr->next != NULL){
			evmng_ptr->head = evmng_cb_list_ptr->next;
		}
		else{
			evmng_ptr->head = NULL;
			evmng_ptr->generator_id = NULL;
		}
	}

	/*Lock spinlock to take the next address for buffer to list
	 * of registration request*/
	lock_spinlock(&g_evmng_b_pool_spinlock);
	/* Decrement buffer pool pointer to insert the memory pointer back
	 * to pool for future registrations*/
	g_evmng_b_pool_pointer --;
	*g_evmng_b_pool_pointer = (uint32_t)evmng_cb_list_ptr;
	/*Unlock spinlock*/
	unlock_spinlock(&g_evmng_b_pool_spinlock);
	return 0;
}
/*****************************************************************************/

int evmng_irq_unregister(uint8_t generator_id, uint8_t event_id, uint8_t priority, uint64_t app_ctx, evmng_cb cb)
{
	int err;
	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= NUM_OF_IRQ_EVENTS || generator_id != EVMNG_GENERATOR_AIOPSL){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}
	cdma_mutex_lock_take((uint64_t) g_evmng_irq_events_list, CDMA_MUTEX_WRITE_LOCK);
	err = remove_event_registration(&g_evmng_irq_events_list[event_id], priority, app_ctx, cb);
	cdma_mutex_lock_release((uint64_t) g_evmng_irq_events_list);
	return err;
}
/*****************************************************************************/

int evmng_unregister(uint8_t generator_id, uint8_t event_id, uint8_t priority, uint64_t app_ctx, evmng_cb cb)
{
	int i, err;

	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}

	cdma_mutex_lock_take((uint64_t) g_evmng_events_list, CDMA_MUTEX_WRITE_LOCK);
	for(i = 0; i < g_evmng_events_last_used_index; i++ )
	{
		if(g_evmng_events_list[i].generator_id == generator_id &&
			g_evmng_events_list[i].event_id == event_id){
			break;
		}
	}
	/* Check if entry with generator and event id was found */
	if(i == g_evmng_events_last_used_index){
		cdma_mutex_lock_release((uint64_t) g_evmng_events_list);
		return -ENAVAIL;
	}
	/* Check if the removed index is the last used one */
	else if(i + 1 == g_evmng_events_last_used_index){
		g_evmng_events_last_used_index --;
	}

	err = remove_event_registration(&g_evmng_events_list[i], priority, app_ctx, cb);
	cdma_mutex_lock_release((uint64_t) g_evmng_events_list);
	return err;
}
/*****************************************************************************/

int evmng_raise_irq_event_cb(void *dev, uint16_t cmd, uint32_t size, void *event_data)
{
	struct evmng_priority_list *evmng_cb_list_ptr;
	struct evmng_irq_params *evmng_irq_cfg;
	/* TODO: remove, when integration with MC will be completed*/
#ifdef SL_DEBUG
	uint32_t *mem_dump_ptr;
	int i;
#endif
	UNUSED(dev);

	sl_pr_debug("Event received from MC\n");

	if((cmd & (~CMDIF_NORESP_CMD)) != EVMNG_EVENT_SEND ||
		size < sizeof(struct evmng_irq_params)){
		return -EINVAL;
	}

	evmng_irq_cfg = (struct evmng_irq_params *)event_data;

#ifdef SL_DEBUG
	mem_dump_ptr = (uint32_t *)event_data;


	for(i = 0; i < size; i += 4)
	{
		fsl_print("0x%08x\n", *mem_dump_ptr);
		mem_dump_ptr ++;
	}
#endif
	if(evmng_irq_cfg->addr >= NUM_OF_IRQ_EVENTS)
	{
		sl_pr_err("Event %d not supported\n",evmng_irq_cfg->addr);
		return -ENOTSUP;
	}

	sl_pr_debug("addr %d, val %d\n", (int)evmng_irq_cfg->addr, (int)evmng_irq_cfg->val);
	/*Only one event can be processed at a time*/
	if(g_evmng_irq_events_list[evmng_irq_cfg->addr].head == NULL)
	{
		sl_pr_debug("No registered CB's for event %d\n",evmng_irq_cfg->addr);
		return 0;
	}

	evmng_cb_list_ptr = g_evmng_irq_events_list[evmng_irq_cfg->addr].head;
#pragma fn_ptr_candidates(dprc_drv_evmng_cb)
	while(evmng_cb_list_ptr != NULL){
		evmng_cb_list_ptr->cb(
				EVMNG_GENERATOR_AIOPSL,
				(uint8_t)evmng_irq_cfg->addr,
				evmng_cb_list_ptr->app_ctx,
				(void *)evmng_irq_cfg->val);
		evmng_cb_list_ptr = evmng_cb_list_ptr->next;
	}
	return 0;
}
/*****************************************************************************/

EV_MNG_CODE_PLACEMENT static void raise_event(uint8_t generator_id, uint8_t event_id, void *event_data)
{
	struct evmng_priority_list *evmng_cb_list_ptr;
	int i;

	/* More than one event can be processed at a time*/
	/* Find the index in table which has the same generator and event id.
	 * The table should be locked.*/
	/* Lock EVM table*/
	cdma_mutex_lock_take((uint64_t) g_evmng_events_list, CDMA_MUTEX_READ_LOCK);

	for(i = 0; i < g_evmng_events_last_used_index; i++){
		if(g_evmng_events_list[i].generator_id == generator_id &&
			g_evmng_events_list[i].event_id == event_id){
			break;
		}
	}

	if(i == g_evmng_events_last_used_index){
		cdma_mutex_lock_release((uint64_t) g_evmng_events_list);
		return;
	}

	if(g_evmng_events_list[i].head == NULL)
	{
		cdma_mutex_lock_release((uint64_t) g_evmng_events_list);
		return;
	}
	evmng_cb_list_ptr = g_evmng_events_list[i].head;
#pragma fn_ptr_candidates(app_dpni_event_added_cb)
	while(evmng_cb_list_ptr != NULL){
		evmng_cb_list_ptr->cb(
				generator_id,
				event_id,
				evmng_cb_list_ptr->app_ctx,
				event_data);
		evmng_cb_list_ptr = evmng_cb_list_ptr->next;
	}
	cdma_mutex_lock_release((uint64_t) g_evmng_events_list);
}
/*****************************************************************************/

EV_MNG_CODE_PLACEMENT int evmng_sl_raise_event(uint8_t generator_id, uint8_t event_id, void *event_data)
{
	if(event_id >= NUM_OF_SL_DEFINED_EVENTS ||
		generator_id != EVMNG_GENERATOR_AIOPSL)
	{
		sl_pr_err("Event %d and generator %d are not supported\n",
				event_id, generator_id);
		return -ENOTSUP;
	}
	raise_event(generator_id, event_id, event_data);
	return 0;
}

int evmng_raise_event(uint8_t generator_id, uint8_t event_id, void *event_data)
{
	if(event_id < NUM_OF_SL_DEFINED_EVENTS &&
		generator_id == EVMNG_GENERATOR_AIOPSL)
	{
		sl_pr_err("Event %d and generator %d are not supported\n",
				event_id, generator_id);
		return -ENOTSUP;
	}
	raise_event(generator_id, event_id, event_data);
	return 0;
}
/*****************************************************************************/

static int evmng_open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev); UNUSED(instance_id);
	sl_pr_debug("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}
/*****************************************************************************/

static int evmng_close_cb(void *dev)
{
	UNUSED(dev);
	sl_pr_debug("close_cb\n");
	return 0;
}
/*****************************************************************************/

int evmng_early_init(void)
{
	int i;
	struct evmng_priority_list *evmng_list_ptr;
	uint32_t *evmng_b_pool_pointer;
	uint16_t num_evmng_registartions = (EVMNG_MAX_NUM_OF_EVENTS + NUM_OF_IRQ_EVENTS) *
		EVMNG_NUM_OF_REGISTRATIONS_PER_EVENT;

	memset(g_evmng_irq_events_list, 0, NUM_OF_IRQ_EVENTS * sizeof(struct evmng));

	/* Initialize events available for service layer and applications */
	for(i = 0; i < NUM_OF_IRQ_EVENTS; i++){
		g_evmng_irq_events_list[i].generator_id = EVMNG_GENERATOR_AIOPSL;
		g_evmng_irq_events_list[i].event_id = (uint8_t) i;
	}


	memset(g_evmng_events_list, 0, EVMNG_MAX_NUM_OF_EVENTS *  sizeof(struct evmng));

	g_evmng_events_last_used_index = 0;

	/* allocate memory for buffer pool and the registrations for events
	 * used in linked list*/
	evmng_b_pool_pointer = (uint32_t *)fsl_malloc(
		num_evmng_registartions *
		(sizeof(uint32_t *) + sizeof(struct evmng_priority_list)), 1);

	if(evmng_b_pool_pointer == NULL){
		pr_err("memory allocation for evmng failed\n");
		return -ENOMEM;
	}

	g_evmng_b_pool_pointer = evmng_b_pool_pointer;
	g_evmng_first_b_pool_pointer = evmng_b_pool_pointer;
	g_evmng_last_b_pool_pointer = evmng_b_pool_pointer;
	g_evmng_last_b_pool_pointer += num_evmng_registartions;


	/*allocate memory to registrations for events*/
	evmng_list_ptr = (struct evmng_priority_list *) (g_evmng_last_b_pool_pointer);

	for(i = 0; i < num_evmng_registartions; i++){
		*evmng_b_pool_pointer = (uint32_t)evmng_list_ptr;
		evmng_b_pool_pointer++;
		evmng_list_ptr++;
	}

	g_evmng_b_pool_spinlock = 0;

	pr_info("\n"
		"EVMNG pool ptr:0x%x\n"
		"EVMNG buf first ptr:0x%x\n"
		"EVMNG last ptr:0x%x\n"
		"EVMNG list size: %d\n"
		"EVMNG num lists: %d\n",
		g_evmng_b_pool_pointer,
		*g_evmng_b_pool_pointer,
		g_evmng_last_b_pool_pointer,
		sizeof(struct evmng_priority_list),
		num_evmng_registartions);


	return 0;
}
/*****************************************************************************/

int evmng_init(void)
{
	struct cmdif_module_ops evmng_ops;
	int err;

	evmng_ops.open_cb = (open_cb_t *)evmng_open_cb;
	evmng_ops.close_cb = (close_cb_t *)evmng_close_cb;
	evmng_ops.ctrl_cb = (ctrl_cb_t *)evmng_raise_irq_event_cb;
	pr_info("register with cmdif module!\n");
	err = cmdif_register_module("EVMNG", &evmng_ops);
	if(err) {
		pr_err("Failed to register with cmdif module!\n");
		return err;
	}

	err = cmdif_srv_mc_evm_session_open();
	ASSERT_COND(!err);

	return 0;
}
/*****************************************************************************/

void evmng_free(void)
{
	pr_info("Free memory used by EVMNG\n");
	fsl_free(g_evmng_first_b_pool_pointer);
}
/*****************************************************************************/


