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

#include "evm.h"
#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "fsl_malloc.h"
#include "fsl_string.h"
#include "fsl_spinlock.h"

struct evm *g_evm_irq_events_list;
struct evm *g_evm_events_list;
uint32_t *g_evm_b_pool_pointer;
uint32_t *g_evm_last_b_pool_pointer;
uint8_t g_evm_b_pool_spinlock;

static void add_event_registration(struct evm *evm_ptr, struct evm_priority_list *evmng_cb_list)
{
	struct evm_priority_list *evmng_cb_list_ptr;
	struct evm_priority_list *evmng_cb_list_tmp_ptr;

	/* Lock event entry in the EVM table*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	if(evm_ptr->head != NULL){
		evmng_cb_list_ptr = evm_ptr->head;
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
			if(evmng_cb_list_tmp_ptr != evm_ptr->head){
				evmng_cb_list_tmp_ptr->next = evmng_cb_list;

			}
			else{
				/*Become first in the list*/
				evm_ptr->head = evmng_cb_list;
			}
		}
		else{
			evmng_cb_list_ptr->next = evmng_cb_list;
		}
	}
	else{
		evm_ptr->head = evmng_cb_list;
	}
	evm_ptr->num_cbs ++;
	cdma_mutex_lock_release((uint64_t) evm_ptr);
}
/*****************************************************************************/

int evmng_irq_register(uint8_t generator_id, uint8_t event_id, uint8_t priority,
                       uint64_t app_ctx, evmng_cb cb)
{
	struct evm *evm_ptr = g_evm_irq_events_list;
	struct evm_priority_list *evmng_cb_list;

	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= NUM_OF_IRQ_EVENTS){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}

	/*Lock spinlock to take the next address for buffer to list
	 * of registration request*/
	lock_spinlock(&g_evm_b_pool_spinlock);
	/* If the number of allocated list reach the limit, return error*/
	if(*g_evm_b_pool_pointer == *g_evm_last_b_pool_pointer){
		unlock_spinlock(&g_evm_b_pool_spinlock);
		return -ENOMEM;
	}
	evmng_cb_list = (struct evm_priority_list *)*g_evm_b_pool_pointer;
	g_evm_b_pool_pointer ++;
	/*Unlock spinlock*/
	unlock_spinlock(&g_evm_b_pool_spinlock);

	evmng_cb_list->app_ctx = app_ctx;
	evmng_cb_list->cb = cb;
	evmng_cb_list->priority = priority;
	evmng_cb_list->next = NULL;

	evm_ptr += event_id;

	add_event_registration(evm_ptr, evmng_cb_list);
	return 0;
}
/*****************************************************************************/

int evmng_register(uint8_t generator_id, uint8_t event_id, uint8_t priority, uint64_t app_ctx, evmng_cb cb)
{
	struct evm *evm_ptr = g_evm_events_list;
	struct evm_priority_list *evmng_cb_list;

	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= MAX_EVENT_ID){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}


	/*Lock spinlock to take the next address for buffer to list
	 * of registration request*/
	lock_spinlock(&g_evm_b_pool_spinlock);
	/* If the number of allocated list reach the limit, return error*/
	if(*g_evm_b_pool_pointer == *g_evm_last_b_pool_pointer){
		unlock_spinlock(&g_evm_b_pool_spinlock);
		return -ENOMEM;
	}
	evmng_cb_list = (struct evm_priority_list *)*g_evm_b_pool_pointer;
	g_evm_b_pool_pointer ++;
	/*Unlock spinlock*/
	unlock_spinlock(&g_evm_b_pool_spinlock);

	evmng_cb_list->app_ctx = app_ctx;
	evmng_cb_list->cb = cb;
	evmng_cb_list->priority = priority;
	evmng_cb_list->next = NULL;

	evm_ptr += event_id;

	add_event_registration(evm_ptr, evmng_cb_list);
	return 0;
}
/*****************************************************************************/
static int remove_event_registration(struct evm *evm_ptr,
                                     uint8_t priority,
                                     uint64_t app_ctx,
                                     evmng_cb cb)
{
	struct evm_priority_list *evmng_cb_list_ptr;
	struct evm_priority_list *evmng_cb_list_tmp_ptr;
	int i = 0;

	/* Lock event entry in the EVM table*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	evmng_cb_list_ptr = evm_ptr->head;
	evmng_cb_list_tmp_ptr = evmng_cb_list_ptr;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		if(evmng_cb_list_ptr->cb == cb &&
			evmng_cb_list_ptr->priority == priority &&
			evmng_cb_list_ptr->app_ctx == app_ctx){
			break;
		}
		evmng_cb_list_tmp_ptr = evmng_cb_list_ptr;
		evmng_cb_list_ptr = evmng_cb_list_ptr->next;
	}

	if(i == evm_ptr->num_cbs){
		cdma_mutex_lock_release((uint64_t) evm_ptr);
		sl_pr_debug("Registration not found for given parameters\n");
		return -ENAVAIL;
	}

	if(evmng_cb_list_tmp_ptr != evm_ptr->head)
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
			evm_ptr->head = evmng_cb_list_ptr->next;
		}
		else{
			evm_ptr->head = NULL;
		}
	}

	/*Lock spinlock to take the next address for buffer to list
	 * of registration request*/
	lock_spinlock(&g_evm_b_pool_spinlock);
	/* Decrement buffer pool pointer to insert the memory pointer back
	 * to pool for future registrations*/
	g_evm_b_pool_pointer --;
	*g_evm_b_pool_pointer = (uint32_t)evmng_cb_list_ptr;
	/*Unlock spinlock*/
	unlock_spinlock(&g_evm_b_pool_spinlock);

	evm_ptr->num_cbs --;
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	return 0;
}

int evmng_irq_unregister(uint8_t generator_id, uint8_t event_id, uint8_t priority, uint64_t app_ctx, evmng_cb cb)
{
	struct evm *evm_ptr = g_evm_irq_events_list;

	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= NUM_OF_IRQ_EVENTS ){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}

	evm_ptr += event_id;
	return remove_event_registration(evm_ptr, priority, app_ctx, cb);
}
/*****************************************************************************/

int evmng_unregister(uint8_t generator_id, uint8_t event_id, uint8_t priority, uint64_t app_ctx, evmng_cb cb)
{
	struct evm *evm_ptr = g_evm_events_list;

	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= MAX_EVENT_ID ){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}

	evm_ptr += event_id;
	return remove_event_registration(evm_ptr, priority, app_ctx, cb);
}
/*****************************************************************************/

int evmng_raise_irq_event_cb(void *dev, uint16_t cmd, uint32_t size, void *event_data)
{
	struct evm *evm_ptr = g_evm_irq_events_list;
	struct evm_priority_list *evmng_cb_list_ptr;
	int i;
	UNUSED(dev);
	UNUSED(size);

	if(cmd & CMDIF_NORESP_CMD)
		cmd &= ~CMDIF_NORESP_CMD;

	if(cmd >= NUM_OF_IRQ_EVENTS || cmd < 0)
	{
		sl_pr_err("Event %d not supported\n",cmd);
		return -ENOTSUP;
	}
	evm_ptr += cmd;
	/*Only one event can be processed at a time*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	if(evm_ptr->num_cbs == 0)
	{
		cdma_mutex_lock_release((uint64_t) evm_ptr);
		sl_pr_debug("No registered CB's for event %d\n",cmd);
		return 0;
	}

	evmng_cb_list_ptr = evm_ptr->head;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		evmng_cb_list_ptr->cb(
				EVM_GENERATOR_AIOPSL,
				(uint8_t)cmd,
				evmng_cb_list_ptr->app_ctx,
				event_data);
		evmng_cb_list_ptr = evmng_cb_list_ptr->next;
	}
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	sl_pr_debug("EVM: Event received: %d\n",cmd);

	return 0;
}
/*****************************************************************************/

static int raise_event(uint8_t generator_id, uint8_t event_id, void *event_data)
{
	struct evm *evm_ptr = g_evm_events_list;
	struct evm_priority_list *evmng_cb_list_ptr;
	int i;

	sl_pr_debug("EVM: Event received: %d\n",event_id);
	evm_ptr += event_id;
	/*Only one event can be processed at a time*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	if(evm_ptr->num_cbs == 0)
	{
		cdma_mutex_lock_release((uint64_t) evm_ptr);
		sl_pr_debug("No registered CB's for event %d\n",event_id);
		return 0;
	}

	evmng_cb_list_ptr = evm_ptr->head;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		evmng_cb_list_ptr->cb(
				generator_id,
				event_id,
				evmng_cb_list_ptr->app_ctx,
				event_data);
		evmng_cb_list_ptr = evmng_cb_list_ptr->next;
	}
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	return 0;
}


int evmng_sl_raise_event(uint8_t generator_id, uint8_t event_id, void *event_data)
{
	if(event_id >= MAX_EVENT_ID)
	{
		sl_pr_err("Event %d not supported\n",event_id);
		return -ENOTSUP;
	}
	return raise_event(generator_id, event_id, event_data);
}

int evmng_raise_event(uint8_t generator_id, uint8_t event_id, void *event_data)
{
	if(event_id < NUM_OF_SL_DEFINED_EVENTS || event_id >= MAX_EVENT_ID)
	{
		sl_pr_err("Event %d not supported\n",event_id);
		return -ENOTSUP;
	}
	return raise_event(generator_id, event_id, event_data);
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
	struct evm_priority_list *evm_list_ptr;
	uint32_t *evm_b_pool_pointer;
	uint16_t num_evm_registartions = (MAX_EVENT_ID + NUM_OF_IRQ_EVENTS) *
		EVM_NUM_OF_REGISTRATIONS_PER_EVENT;
	g_evm_irq_events_list = (struct evm *) fsl_malloc(NUM_OF_IRQ_EVENTS *
	                                       sizeof(struct evm),
	                                       1);
	if(g_evm_irq_events_list == NULL) {
		pr_err("memory allocation for evm sl events failed\n");
		return -ENOMEM;
	}

	memset(g_evm_irq_events_list, 0, NUM_OF_IRQ_EVENTS * sizeof(struct evm));

	/* Initialize events available for service layer and applications */
	for(i = 0; i < NUM_OF_IRQ_EVENTS; i++){
		g_evm_irq_events_list->event_id = (uint8_t) i;
	}

	g_evm_events_list = (struct evm *)
		fsl_malloc(MAX_EVENT_ID * sizeof(struct evm), 1);

	if(g_evm_events_list == NULL) {
		pr_err("memory allocation for events failed\n");
		return -ENOMEM;
	}

	memset(g_evm_events_list, 0, MAX_EVENT_ID * sizeof(struct evm));

	/* Initialize events created by / allowed to  applications */
	for(i = 0; i < MAX_EVENT_ID; i++){
		g_evm_events_list->event_id = (uint8_t) i;
	}

	/*allocate memory to registrations for events*/

	evm_list_ptr = (struct evm_priority_list *)
		fsl_malloc(
			num_evm_registartions *
			sizeof(struct evm_priority_list),
			1);
	if(evm_list_ptr == NULL){
		pr_err("memory allocation for evm lists failed\n");
		return -ENOMEM;
	}

	evm_b_pool_pointer = (uint32_t *)fsl_malloc(
		num_evm_registartions * sizeof(uint32_t *), 1);
	g_evm_b_pool_pointer = evm_b_pool_pointer;

	if(g_evm_b_pool_pointer == NULL){
		pr_err("memory allocation for buffer pool to evm failed\n");
		return -ENOMEM;
	}

	for(i = 0; i < num_evm_registartions; i++){
		*evm_b_pool_pointer = (uint32_t)evm_list_ptr;
		evm_b_pool_pointer++;
		evm_list_ptr++;
	}

	*g_evm_last_b_pool_pointer = (uint32_t)evm_list_ptr;

	g_evm_b_pool_spinlock = 0;

	pr_info("\n"
		"EVM pool ptr:0x%x\n"
		"EVM last ptr:0x%x\n"
		"EVM list size: %d\n"
		"EVM num lists: %d\n",
		*g_evm_b_pool_pointer,
		*g_evm_last_b_pool_pointer,
		sizeof(struct evm_priority_list),
		num_evm_registartions);


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
	pr_info("EVM: register with cmdif module!\n");
	err = cmdif_register_module("EVM", &evmng_ops);
	if(err) {
		pr_err("EVM: Failed to register with cmdif module!\n");
		return err;
	}
	return 0;
}
/*****************************************************************************/

void evmng_free(void)
{
	int i;
	struct evm *evm_ptr;
	struct evm_priority_list *evmng_cb_list_ptr;
	struct evm_priority_list *evmng_cb_list_next_ptr;

	pr_info("Free memory used by EVM\n");

	evm_ptr = g_evm_events_list;
	for(i = 0; i < MAX_EVENT_ID; i++, evm_ptr ++ )
	{
		evmng_cb_list_ptr = evm_ptr->head;
		if(evmng_cb_list_ptr)
		{
			do{
				evmng_cb_list_next_ptr = evmng_cb_list_ptr->next;
				fsl_free(evmng_cb_list_ptr);
				evmng_cb_list_ptr = evmng_cb_list_next_ptr;
			}while(evmng_cb_list_ptr != NULL);
		}
	}
	fsl_free(g_evm_events_list);

	evm_ptr = g_evm_irq_events_list;
	for(i = 0; i < NUM_OF_IRQ_EVENTS; i++, evm_ptr ++ )
	{
		evmng_cb_list_ptr = evm_ptr->head;
		if(evmng_cb_list_ptr)
		{
			do{
				evmng_cb_list_next_ptr = evmng_cb_list_ptr->next;
				fsl_free(evmng_cb_list_ptr);
				evmng_cb_list_ptr = evmng_cb_list_next_ptr;
			}while(evmng_cb_list_ptr != NULL);
		}
	}
	fsl_free(g_evm_irq_events_list);
}
/*****************************************************************************/


