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

#include "fsl_types.h"
#include "fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_parser.h"
#include "fsl_general.h"
#include "fsl_time.h"
#include "fsl_slab.h"
#include "fsl_cdma.h"
#include "fsl_l2.h"
#include "fsl_cmdif_client.h"
#include "fsl_cmdif_server.h"
#include "fsl_icontext.h"
#include "fsl_shbp.h"
#include "fsl_stdlib.h"
#include "fsl_evmng.h"
#include "fsl_dpci_drv.h"
#include "fsl_dpci_event.h"
#include "fsl_rcu.h"

int app_early_init(void);
int app_init(void);
void app_free(void);
void stack_estimation(void);
extern void cmdif_srv_isr(void);
extern void cmdif_cl_isr(void);
extern void receive_cb(void);
extern int evmng_raise_irq_event_cb(void *dev, uint16_t cmd, uint32_t size, void *event_data);

__HOT_CODE ENTRY_POINT static void app_process_packet(void)
{
	/*Function used for stack estimation to sl routines*/
	stack_estimation();

}


static int app_dpni_event_added_cb(
	uint8_t generator_id,
	uint8_t event_id,
	uint64_t app_ctx,
	void *event_data)
{
	uint16_t ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);
	return 0;
}

void stack_estimation(void)
{
#pragma fn_ptr_candidates(stack_estimation)
	struct slab **my_slab = 0;
	uint32_t time;
	uint64_t time_since_epoch;
	uint64_t ctr_value;
	uint64_t buff = 0;
	struct cmdif_desc cidesc = {0};
	struct icontext ic = {0};
	struct dpni_drv_buf_layout layout = {0};
	struct dpkg_profile_cfg key_cfg = {0};
	struct ldpaa_fd fd = {0};
	struct dpni_drv_link_state link_state = {0};
	struct ep_init_presentation ep_init = {0};
	uint16_t ni = 0, dpni_id, spid, mfl = 0;
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE] = {0};
	int state = 0;
	rx_cb_t *cb = 0;
	uint64_t shbp = 0;

	/*sl_prolog must be called first when packet arrives*/
	sl_prolog();

	slab_create(5, 5, 256, 8, MEM_PART_DP_DDR, SLAB_DDR_MANAGEMENT_FLAG, NULL, my_slab);
	slab_acquire(*my_slab, &buff);
	slab_release(*my_slab, buff);
	slab_free(my_slab);
	fsl_print("Test\n");
	fsl_os_rand();
	fsl_get_time_since_epoch_ms(&time_since_epoch);
	fsl_get_time_ms(&time);

	/* CMDIF runtime functions */

	cmdif_srv_isr();
	cmdif_open(&cidesc, NULL, 0, NULL, 0);
	cmdif_cl_isr();
	cmdif_send(&cidesc, 0, 0, CMDIF_PRI_HIGH, NULL, NULL, NULL);
	cmdif_close(&cidesc);

	/* Isolation Context runtime API */
	icontext_get(5, &ic);
	icontext_aiop_get(&ic);
	icontext_cmd_get(&ic);
	icontext_acquire(&ic, 7, &buff);
	icontext_release(&ic, 7, buff);
	icontext_dma_read(&ic, 4, buff, &time);
	icontext_dma_write(&ic, 4, &time, buff);

	/* DPNI runtime functions */
	dpni_drv_register_rx_cb(ni, cb);
	dpni_drv_unregister_rx_cb(ni);
	dpni_get_receive_niid();
	dpni_set_send_niid(ni);
	dpni_get_send_niid();
	dpni_drv_get_primary_mac_addr(ni, mac_addr);
	dpni_drv_set_primary_mac_addr(ni, ((uint8_t []){0x02, 0x00 ,0xc0 ,0x0a8 ,0x0b ,0xfe }));
	dpni_drv_add_mac_addr(ni, mac_addr);
	dpni_drv_remove_mac_addr(ni, mac_addr);
	dpni_drv_set_max_frame_length(ni, mfl);
	dpni_drv_get_max_frame_length(ni, &mfl);
	dpni_drv_send(ni);
	dpni_drv_explicit_send(ni, &fd);
	dpni_drv_set_multicast_promisc(ni, state);
	dpni_drv_get_multicast_promisc(ni, &state);
	dpni_drv_set_unicast_promisc(ni, state);
	dpni_drv_get_unicast_promisc(ni, &state);
	dpni_drv_get_spid(ni, &spid);
	dpni_drv_get_spid_ddr(ni, &spid);
	/*This function supported in boot mode only*/
	/*dpni_drv_set_order_scope(ni, &key_cfg);*/
	dpni_drv_get_rx_buffer_layout(ni, &layout);
	dpni_drv_set_rx_buffer_layout(ni, &layout);
	dpni_drv_get_counter(ni, DPNI_DRV_CNT_ING_FRAME ,&ctr_value);
	dpni_drv_reset_counter(ni, DPNI_DRV_CNT_ING_FRAME);
	dpni_drv_get_dpni_id(ni, &dpni_id);
	dpni_drv_get_ni_id(dpni_id, &ni);
	dpni_drv_get_link_state(ni, &link_state);
	dpni_drv_clear_mac_filters(ni, 1, 1);
	dpni_drv_clear_vlan_filters(ni);
	dpni_drv_set_vlan_filters(ni, 1);
	dpni_drv_add_vlan_id(ni, (uint16_t)1515);
	dpni_drv_remove_vlan_id(ni, (uint16_t)1515);
	dpni_drv_enable(ni);
	dpni_drv_disable(ni);
	dpni_drv_set_initial_presentation(ni, &ep_init);
	dpni_drv_get_initial_presentation(ni, &ep_init);
	evmng_raise_event(1, 1, (void *) &time);
	evmng_register(1, 1, 1, (uint64_t)app_process_packet, app_dpni_event_added_cb);
	evmng_unregister(1, 1, 1, (uint64_t)app_process_packet, app_dpni_event_added_cb);
//	void *dev = NULL;
//	evmng_raise_irq_event_cb(dev,1,64,dev);
	/* SHBP Shared buffer pool */
	shbp_acquire(shbp, &ic);
	shbp_release(shbp, NULL, &ic);

	/* DPCI */
	dpci_drv_disable(0);
	//dpci_drv_enable(0);
	dpci_drv_get_initial_presentation(0, NULL);
	dpci_drv_set_initial_presentation(0, NULL);
	/*
	 * The stack of those functions should be measured with EVM
	 * All the code before user callback can be ignores
	 *
	 * dpci_event_assign(0);
	 * dpci_event_unassign(0);
	 * dpci_event_link_change(0);
	 *
	 */

	/* dpci_event_update(0); called only by SL in cmdif open command t
	 * the stack is not important */

	
	/* RCU does not exists in 1088*/
	/*
	rcu_synchronize(NULL, 9);
	rcu_read_lock();
	rcu_read_unlock();
        */
	
	/*After packet processing is done, fdma_terminate_task must be called.*/
	fdma_terminate_task();



}

int app_init(void)
{
	fsl_print("Running app_init()\n");
	return 0;
}

int app_early_init(void)
{
	/* Early initialization */
	return 0;
}

void app_free(void)
{
	/* free application resources*/
}