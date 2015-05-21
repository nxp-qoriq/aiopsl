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

struct evm *g_evm_irq_events_list;
struct evm *g_evm_events_list;

static int add_event_registration(struct evm *evm_ptr, struct evm_priority_list *evm_cb_list)
{
	struct evm_priority_list *evm_cb_list_ptr;
	struct evm_priority_list *evm_cb_list_tmp_ptr;

	/* Lock event entry in the EVM table*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	if(evm_ptr->head != NULL){
		evm_cb_list_ptr = evm_ptr->head;
		evm_cb_list_tmp_ptr = evm_cb_list_ptr;

		while(evm_cb_list_ptr->next != NULL &&
			evm_cb_list_ptr->priority <= evm_cb_list->priority)
		{
			evm_cb_list_tmp_ptr = evm_cb_list_ptr;
			evm_cb_list_ptr = evm_cb_list_ptr->next;
		}

		if(evm_cb_list_ptr->priority > evm_cb_list->priority){
			/*insert the list with cb function between:
			 *  evm_cb_list_tmp_ptr->"evm_cb_list"->evm_cb_list_ptr*/
			evm_cb_list->next = evm_cb_list_ptr;
			if(evm_cb_list_tmp_ptr != evm_ptr->head){
				evm_cb_list_tmp_ptr->next = evm_cb_list;

			}
			else{
				/*Become first in the list*/
				evm_ptr->head = evm_cb_list;
			}
		}
		else{
			evm_cb_list_ptr->next = evm_cb_list;
		}
	}
	else{
		evm_ptr->head = evm_cb_list;
	}
	evm_ptr->num_cbs ++;
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	return 0;
}
/*****************************************************************************/

int evm_irq_register(uint8_t event_id, uint8_t priority, uint64_t app_ctx, evm_cb cb)
{
	struct evm *evm_ptr = g_evm_irq_events_list;
	struct evm_priority_list *evm_cb_list;

#ifdef DEBUG
	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= NUM_OF_IRQ_EVENTS){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}
#endif

	evm_cb_list = (struct evm_priority_list *)
				fsl_malloc(sizeof(struct evm_priority_list), 8);
	if(evm_cb_list == NULL){
		sl_pr_debug("Allocation of memory for evm cb list failed\n");
		return -ENOMEM;
	}

	evm_cb_list->app_ctx = app_ctx;
	evm_cb_list->cb = cb;
	evm_cb_list->priority = priority;
	evm_cb_list->next = NULL;

	evm_ptr += event_id;

	return add_event_registration(evm_ptr, evm_cb_list);

}
/*****************************************************************************/

int evm_register(uint8_t event_id, uint8_t priority, uint64_t app_ctx, evm_cb cb)
{
	struct evm *evm_ptr = g_evm_events_list;
	struct evm_priority_list *evm_cb_list;

#ifdef DEBUG
	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= MAX_EVENT_ID){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}
#endif


	evm_cb_list = (struct evm_priority_list *)
						fsl_malloc(sizeof(struct evm_priority_list), 8);
	if(evm_cb_list == NULL){
		sl_pr_debug("Allocation of memory for evm cb list failed\n");
		return -ENOMEM;
	}

	evm_cb_list->app_ctx = app_ctx;
	evm_cb_list->cb = cb;
	evm_cb_list->priority = priority;
	evm_cb_list->next = NULL;

	evm_ptr += event_id;

	return add_event_registration(evm_ptr, evm_cb_list);

}
/*****************************************************************************/
static int remove_event_registration(struct evm *evm_ptr,
                                     uint8_t priority,
                                     uint64_t app_ctx,
                                     evm_cb cb)
{
	struct evm_priority_list *evm_cb_list_ptr;
	struct evm_priority_list *evm_cb_list_tmp_ptr;
	int i = 0;

	/* Lock event entry in the EVM table*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	evm_cb_list_ptr = evm_ptr->head;
	evm_cb_list_tmp_ptr = evm_cb_list_ptr;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		if(evm_cb_list_ptr->cb == cb &&
			evm_cb_list_ptr->priority == priority &&
			evm_cb_list_ptr->app_ctx == app_ctx){
			break;
		}
		evm_cb_list_tmp_ptr = evm_cb_list_ptr;
		evm_cb_list_ptr = evm_cb_list_ptr->next;
	}

	if(i == evm_ptr->num_cbs){
		cdma_mutex_lock_release((uint64_t) evm_ptr);
		sl_pr_debug("Registration not found for given parameters\n");
		return -ENAVAIL;
	}

	if(evm_cb_list_tmp_ptr != evm_ptr->head)
	{
		if(evm_cb_list_ptr->next != NULL){
			evm_cb_list_tmp_ptr->next = evm_cb_list_ptr->next;
		}
		else{
			evm_cb_list_tmp_ptr->next = NULL;
		}
	}
	else{
		if(evm_cb_list_ptr->next != NULL){
			evm_ptr->head = evm_cb_list_ptr->next;
		}
		else{
			evm_ptr->head = NULL;
		}
	}

	fsl_free((void *) evm_cb_list_ptr);
	evm_ptr->num_cbs --;
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	return 0;
}

int evm_irq_unregister(uint8_t event_id, uint8_t priority, uint64_t app_ctx, evm_cb cb)
{
	struct evm *evm_ptr = g_evm_irq_events_list;
#ifdef DEBUG
	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= NUM_OF_IRQ_EVENTS ){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}
#endif
	evm_ptr += event_id;
	return remove_event_registration(evm_ptr, priority, app_ctx, cb);
}
/*****************************************************************************/

int evm_unregister(uint8_t event_id, uint8_t priority, uint64_t app_ctx, evm_cb cb)
{
	struct evm *evm_ptr = g_evm_events_list;
#ifdef DEBUG
	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= MAX_EVENT_ID ){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}
#endif
	evm_ptr += event_id;
	return remove_event_registration(evm_ptr, priority, app_ctx, cb);
}
/*****************************************************************************/

int evm_raise_irq_event_cb(void *dev, uint16_t cmd, uint32_t size, void *event_data)
{
	struct evm *evm_ptr = g_evm_irq_events_list;
	struct evm_priority_list *evm_cb_list_ptr;
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

	evm_cb_list_ptr = evm_ptr->head;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		evm_cb_list_ptr->cb((uint8_t)cmd,
		                    evm_cb_list_ptr->app_ctx,
		                    event_data);
		evm_cb_list_ptr = evm_cb_list_ptr->next;
	}
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	sl_pr_debug("EVM: Event received: %d\n",cmd);

	return 0;
}
/*****************************************************************************/

static int raise_event(uint8_t event_id, void *event_data)
{
	struct evm *evm_ptr = g_evm_events_list;
	struct evm_priority_list *evm_cb_list_ptr;
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

	evm_cb_list_ptr = evm_ptr->head;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		evm_cb_list_ptr->cb((uint8_t)event_id,
		                    evm_cb_list_ptr->app_ctx,
		                    event_data);
		evm_cb_list_ptr = evm_cb_list_ptr->next;
	}
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	return 0;
}


int evm_sl_raise_event(uint8_t event_id, void *event_data)
{
	if(event_id >= MAX_EVENT_ID)
	{
		sl_pr_err("Event %d not supported\n",event_id);
		return -ENOTSUP;
	}
	return raise_event(event_id, event_data);
}

int evm_raise_event(uint8_t event_id, void *event_data)
{
	if(event_id < NUM_OF_SL_DEFINED_EVENTS || event_id >= MAX_EVENT_ID)
	{
		sl_pr_err("Event %d not supported\n",event_id);
		return -ENOTSUP;
	}
	return raise_event(event_id, event_data);
}
/*****************************************************************************/

static int evm_open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev); UNUSED(instance_id);
	sl_pr_debug("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}
/*****************************************************************************/

static int evm_close_cb(void *dev)
{
	UNUSED(dev);
	sl_pr_debug("close_cb\n");
	return 0;
}
/*****************************************************************************/

int evm_early_init(void)
{
	int i;
	g_evm_irq_events_list = (struct evm *) fsl_malloc(NUM_OF_IRQ_EVENTS *
	                                       sizeof(struct evm),
	                                       64);
	if(g_evm_irq_events_list == NULL) {
		pr_err("memory allocation for sl events failed\n");
		return -ENOMEM;
	}

	memset(g_evm_irq_events_list, 0, NUM_OF_IRQ_EVENTS * sizeof(struct evm));

	/* Initialize events available for service layer and applications */
	for(i = 0; i < NUM_OF_IRQ_EVENTS; i++){
		g_evm_irq_events_list->event_id = (uint8_t) i;
	}

	g_evm_events_list = (struct evm *)
		fsl_malloc(MAX_EVENT_ID * sizeof(struct evm), 64);

	if(g_evm_events_list == NULL) {
		pr_err("memory allocation for events failed\n");
		return -ENOMEM;
	}

	memset(g_evm_events_list, 0, MAX_EVENT_ID * sizeof(struct evm));

	/* Initialize events created by / allowed to  applications */
	for(i = 0; i < MAX_EVENT_ID; i++){
		g_evm_events_list->event_id = (uint8_t) i;
	}

	return 0;
}
/*****************************************************************************/

int evm_init(void)
{
	struct cmdif_module_ops evm_ops;
	int err;

	evm_ops.open_cb = (open_cb_t *)evm_open_cb;
	evm_ops.close_cb = (close_cb_t *)evm_close_cb;
	evm_ops.ctrl_cb = (ctrl_cb_t *)evm_raise_irq_event_cb;
	pr_info("EVM: register with cmdif module!\n");
	err = cmdif_register_module("EVM", &evm_ops);
	if(err) {
		pr_err("EVM: Failed to register with cmdif module!\n");
		return err;
	}
	return 0;
}
/*****************************************************************************/

void evm_free(void)
{
	int i;
	struct evm *evm_ptr;
	struct evm_priority_list *evm_cb_list_ptr;
	struct evm_priority_list *evm_cb_list_next_ptr;

	pr_info("Free memory used by EVM\n");

	evm_ptr = g_evm_events_list;
	for(i = 0; i < MAX_EVENT_ID; i++, evm_ptr ++ )
	{
		evm_cb_list_ptr = evm_ptr->head;
		if(evm_cb_list_ptr)
		{
			do{
				evm_cb_list_next_ptr = evm_cb_list_ptr->next;
				fsl_free(evm_cb_list_ptr);
				evm_cb_list_ptr = evm_cb_list_next_ptr;
			}while(evm_cb_list_ptr != NULL);
		}
	}
	fsl_free(g_evm_events_list);

	evm_ptr = g_evm_irq_events_list;
	for(i = 0; i < NUM_OF_IRQ_EVENTS; i++, evm_ptr ++ )
	{
		evm_cb_list_ptr = evm_ptr->head;
		if(evm_cb_list_ptr)
		{
			do{
				evm_cb_list_next_ptr = evm_cb_list_ptr->next;
				fsl_free(evm_cb_list_ptr);
				evm_cb_list_ptr = evm_cb_list_next_ptr;
			}while(evm_cb_list_ptr != NULL);
		}
	}
	fsl_free(g_evm_irq_events_list);
}
/*****************************************************************************/


