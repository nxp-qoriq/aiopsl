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
#include "fsl_dpni_drv.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_parser.h"
#include "fsl_net.h"
#include "fsl_sys.h"
#include "fsl_dpni.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_sl_dbg.h"

int dpni_drv_test(void);
int dpni_drv_test_create(void);
int dpni_drv_test_destroy(uint16_t ni);
extern uint64_t dpni_lock; /*lock to change dpni_ctr and dpni_broadcast_flag safely */
extern uint8_t dpni_ctr; /*counts number of packets received before removing broadcast address*/
extern uint8_t dpni_broadcast_flag; /*flag if packet with broadcast mac destination received during the test*/

extern uint8_t num_of_nis_arena;

int dpni_drv_test(void){
	int err = 0;
	int ni = 0;
	int local_test_error = 0;
	uint16_t ipv4hdr_offset = 0;
	char *eth_ptr;
	char *ip_ptr;
	int i;
	struct ipv4hdr *ipv4header;

	/*DPNI test*/

	if(dpni_ctr == 38) /*disable mac after 39 injected packets, one of first 3 packets is broadcast*/
	{
		lock_spinlock(&dpni_lock);
		if(dpni_ctr == 38)
		{
			for(ni = 0; ni < num_of_nis_arena; ni ++)
			{
				/*Just to test functionality, because of promiscuous mode enabled - the packets will continue to receive*/
				err = dpni_drv_remove_mac_addr((uint16_t)ni,((uint8_t []){0x02,0x00,0xC0,0xA8,0x0B,0xFE}));
				if(err != 0) {
					fsl_print("dpni_drv_remove_mac_addr error 02:00:C0:A8:0B:FE for ni %d\n",ni);
					local_test_error |= err;
				}
				else {
					fsl_print("dpni_drv_remove_mac_addr 02:00:C0:A8:0B:FE for ni %d succeeded\n",ni);
				}
			}
			dpni_ctr ++; /*increase counter so the function will be called only once*/
		}
		unlock_spinlock(&dpni_lock);
	}
	else{
		dpni_ctr ++;
	}



	if (PARSER_IS_OUTER_IPV4_DEFAULT())
	{

		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ip_ptr = (char *) PRC_GET_SEGMENT_ADDRESS() + ipv4hdr_offset ;

		ipv4header = (struct ipv4hdr *)ip_ptr;

		ip_ptr = (char *)&(ipv4header->dst_addr);
		fsl_print("DST IP: ");
		for( int i = 0; i < NET_HDR_FLD_IPv4_ADDR_SIZE; i++)
		{
			fsl_print("%d.",*ip_ptr);
			ip_ptr ++;
		}
		fsl_print("\n");

	}

	eth_ptr = (char *)PARSER_GET_ETH_POINTER_DEFAULT();
#if !(!defined(DEBUG_ERRORS) || (DEBUG_ERRORS == 0))
	enable_print_protection();
	dbg_print("DEST MAC: ");
	for(i = 0; i < NET_HDR_FLD_ETH_ADDR_SIZE; i++){

		dbg_print("%x ", *eth_ptr);
		eth_ptr ++;
	}
	dbg_print("\n");
	disable_print_protection();
#endif
	eth_ptr = (char *)PARSER_GET_ETH_POINTER_DEFAULT();
	for(i = 0; i < NET_HDR_FLD_ETH_ADDR_SIZE; i++) /*check if destination mac is broadcast*/
		if(*eth_ptr++ != 0xff)
			break;

	if(i == NET_HDR_FLD_ETH_ADDR_SIZE) /*check if all the destination MAC was broadcast FF:FF:FF:FF:FF:FF*/
	{
		lock_spinlock(&dpni_lock);
		dpni_broadcast_flag = 1;
		unlock_spinlock(&dpni_lock);
	}

	if(dpni_ctr == 3){
		if(dpni_broadcast_flag == 0) {
			fsl_print("dpni error - broadcast packets didn't received\n");
			local_test_error |= 0x01;
		}
		else {
			fsl_print("dpni success - broadcast packets received during the test\n");
		}
	}

	return local_test_error;
}

static int dpni_drv_create(uint16_t *token)
{
	int err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_cfg cfg = {0};
	cfg.options = 0;
	uint32_t dpni_temp_id;

	err = dpni_create(&dprc->io,dprc->token, 0, &cfg, &dpni_temp_id);
	if(err){
		sl_pr_err("dpni_create failed\n");
		return err;
	}
	
	err = dpni_open(&dprc->io, 0, (int)dpni_temp_id, token);
	if(err){
		sl_pr_err("dpni_open failed\n");
		return err;
	}
	
	err = dpni_close(&dprc->io, 0, *token);
	if(err){
		sl_pr_err("dpni_close failed\n");
		return err;
	}

	return 0;
}
static int dpni_drv_destroy(uint16_t ni_id)
{
	uint16_t dpni;
	int err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int) nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_destroy(&dprc->io, dprc->token, 0, dpni);
	if(err){
		sl_pr_err("dpni_destroy failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	return 0;
}

int dpni_drv_test_create(void)
{
	int err;
	uint16_t token;
	err = dpni_drv_create(&token);
	if(err){
		fsl_print("Creating dpni failed %d\n",err);
	}
	else{
		fsl_print("DPNI created successfully with token %d\n",token);
	}
	return err;
}

int dpni_drv_test_destroy(uint16_t ni)
{
	int err;
	err = dpni_drv_destroy(ni);
	if (err) {
		fsl_print("ERROR = %d: dpni_drv_destroy failed\n", err);
	} else {
		fsl_print("dpni_drv_destroy passed for NI %d\n", ni);
	}
	return err;
}

