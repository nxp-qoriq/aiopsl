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

struct evm *event_data;

static int add_event_registration(uint8_t generator_id,
                                  enum evm_types event_id,
                                  uint8_t priority,
                                  evm_cb cb,
                                  uint8_t priority_flag)
{
	struct evm *evm_ptr = event_data;
	struct evm_priority_list *evm_cb_list;
	struct evm_priority_list *evm_cb_list_ptr;
	struct evm_priority_list *evm_cb_list_tmp_ptr;

#ifdef DEBUG
	if(generator_id == NULL){
		sl_pr_debug("Generator ID is NULL\n");
		return -EINVAL;
	}
	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= AIOP_NUM_OF_EVENTS || event_id < 0){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}
#endif
	if((priority < MINIMUM_PRIORITY || priority > MAXIMUM_PRIORITY)
		&& priority_flag == EVM_APP_REGISTRATION_FLAG){
		sl_pr_debug("priority value is out of bounds\n");
		return -EINVAL;
	}

	evm_cb_list = (struct evm_priority_list *)
				fsl_malloc(sizeof(struct evm_priority_list), 8);
	if(evm_cb_list == NULL){
		sl_pr_debug("Allocation of memory for evm cb list failed\n");
		return -ENOMEM;
	}

	evm_cb_list->generator_id = generator_id;
	evm_cb_list->cb = cb;
	evm_cb_list->priority = priority;
	evm_cb_list->next = NULL;


	evm_ptr += event_id;
	/* Lock event entry in the EVM table*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	if(evm_ptr->head != NULL){
		evm_cb_list_ptr = evm_ptr->head;
		evm_cb_list_tmp_ptr = evm_cb_list_ptr;

		while(evm_cb_list_ptr->next != NULL &&
			evm_cb_list_ptr->priority <= priority)
		{
			evm_cb_list_tmp_ptr = evm_cb_list_ptr;
			evm_cb_list_ptr = evm_cb_list_ptr->next;
		}

		if(evm_cb_list_ptr->priority > priority){
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

int evm_sl_register(uint8_t generator_id, enum evm_types event_id, uint8_t priority, evm_cb cb){
	return add_event_registration(generator_id, event_id, priority, cb, EVM_SL_REGISTRATION_FLAG);
}


int evm_app_register(uint8_t generator_id, enum evm_types event_id, uint8_t priority, evm_cb cb)
{
	return add_event_registration(generator_id, event_id, priority, cb, EVM_APP_REGISTRATION_FLAG);
}

int evm_unregister(uint8_t generator_id, enum evm_types event_id,
                   uint8_t priority, evm_cb cb)
{
	struct evm *evm_ptr = event_data;
	struct evm_priority_list *evm_cb_list_ptr;
	struct evm_priority_list *evm_cb_list_tmp_ptr;
	int i = 0;

#ifdef DEBUG
	if(generator_id == NULL){
		sl_pr_debug("Generator ID is NULL\n");
		return -EINVAL;
	}
	if(cb == NULL){
		sl_pr_debug("CB is NULL\n");
		return -EINVAL;
	}
	if(event_id >= AIOP_NUM_OF_EVENTS || event_id < 0){
		sl_pr_debug("event_id value is out of bounds\n");
		return -EINVAL;
	}
#endif

	evm_ptr += event_id;
	/* Lock event entry in the EVM table*/
	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	evm_cb_list_ptr = evm_ptr->head;
	evm_cb_list_tmp_ptr = evm_cb_list_ptr;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		if(evm_cb_list_ptr->cb == cb &&
			evm_cb_list_ptr->generator_id == generator_id &&
			evm_cb_list_ptr->priority == priority){
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
/**************************************************************************//**
@Function	evm_raise_event_cb

@Description	Function to generate an event.

@Param[in]	generator_id  Identifier of the application generating event.

@Param[in]	event_id  Identifier of the specific event.
		The value can range from 0 to 255.
		A unique combination of generator_id and event_id corresponds
		to a unique event in the system.

@param[in]	event_data  A pointer to structure specific for event


@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
static int evm_raise_event_cb(void *dev, uint16_t cmd, uint32_t size, void *data)
{
	struct evm *evm_ptr = event_data;
	struct evm_priority_list *evm_cb_list_ptr;
	int i;
	UNUSED(dev);
	cmd &= ~CMDIF_NORESP_CMD;
	if(cmd >= AIOP_NUM_OF_EVENTS || cmd < 0)
	{
		sl_pr_err("Event %d not supported\n",cmd);
		return -ENOTSUP;
	}
	evm_ptr += cmd;

	cdma_mutex_lock_take((uint64_t) evm_ptr, CDMA_MUTEX_WRITE_LOCK);
	if(evm_ptr->num_cbs == 0)
	{
		cdma_mutex_lock_release((uint64_t) evm_ptr);
		sl_pr_debug("No registered CB's for event %d\n",cmd);
		return 0;
	}

	evm_cb_list_ptr = evm_ptr->head;

	for(i = 0; i < evm_ptr->num_cbs; i++){
		evm_cb_list_ptr->cb(evm_cb_list_ptr->generator_id,
		                    (enum evm_types)cmd,
		                    size,
		                    data);
		evm_cb_list_ptr = evm_cb_list_ptr->next;
	}
	cdma_mutex_lock_release((uint64_t) evm_ptr);
	pr_info("EVM: Event received: %d\n",cmd);

	return 0;
}

static int evm_open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev); UNUSED(instance_id);
	sl_pr_debug("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}
static int evm_close_cb(void *dev)
{
	UNUSED(dev);
	sl_pr_debug("close_cb\n");
	return 0;
}

int evm_init(void)
{
	struct evm *evm_ptr;
	struct cmdif_module_ops evm_ops;
	int i, err;
	event_data = (struct evm *) fsl_malloc(AIOP_NUM_OF_EVENTS *
	                                       sizeof(struct evm),
	                                       64);
	if(event_data == NULL) {
		pr_err("memory allocation failed\n");
		return -ENOMEM;
	}
	evm_ptr = event_data;

	memset(event_data, 0, AIOP_NUM_OF_EVENTS * sizeof(struct evm));

	/*Register for the events in MC*/
	for(i = 0; i < AIOP_NUM_OF_EVENTS; i++){
		evm_ptr->event_id = (enum evm_types) i;
	}



	evm_ops.open_cb = (open_cb_t *)evm_open_cb;
	evm_ops.close_cb = (close_cb_t *)evm_close_cb;
	evm_ops.ctrl_cb = (ctrl_cb_t *)evm_raise_event_cb;
	pr_info("EVM: register with cmdif module!\n");
	err = cmdif_register_module("EVM", &evm_ops);
	if(err) {
		pr_err("EVM: Failed to register with cmdif module!\n");
		return err;
	}
	return 0;
}


void evm_free(void)
{

}

