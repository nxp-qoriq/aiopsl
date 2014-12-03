/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
#include "fsl_dpni_drv.h"
#include "kernel/fsl_spinlock.h"
#include "dplib/fsl_parser.h"
#include "fsl_net.h"

int dpni_drv_test(void);

extern uint8_t dpni_lock; /*lock to change dpni_ctr and dpni_broadcast_flag safely */
extern uint8_t dpni_ctr; /*counts number of packets received before removing broadcast address*/
extern uint8_t dpni_broadcast_flag; /*flag if packet with broadcast mac destination received during the test*/

int dpni_drv_test(void){
	int err = 0;
	int ni = 0;
	int local_test_error = 0;
	uint16_t ipv4hdr_offset = 0;
	char *eth_ptr;
	char *ip_ptr;
	int promisc;
	int i;
	struct ipv4hdr *ipv4header;

	/*DPNI test*/

	if(dpni_ctr == 38) /*disable mac after 39 injected packets, one of first 3 packets is broadcast*/
	{
		lock_spinlock(&dpni_lock);
		if(dpni_ctr == 38)
		{
			for(ni = 0; ni < dpni_get_num_of_ni(); ni ++)
			{
				/*Just to test functionality, because of promiscuous mode enabled - the packets will continue to receive*/
				err = dpni_drv_remove_mac_addr((uint16_t)ni,((uint8_t []){0x02,0x00,0xC0,0xA8,0x0B,0xFE}));
				if(err != 0) {
					fsl_os_print("dpni_drv_remove_mac_addr error 02:00:C0:A8:0B:FE for ni %d\n",ni);
					local_test_error |= err;
				}
				else {
					fsl_os_print("dpni_drv_remove_mac_addr 02:00:C0:A8:0B:FE for ni %d succeeded\n",ni);
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
		fsl_os_print("DST IP: ");
		for( int i = 0; i < NET_HDR_FLD_IPv4_ADDR_SIZE; i++)
		{
			fsl_os_print("%d.",*ip_ptr);
			ip_ptr ++;
		}
		fsl_os_print("\n");

	}

	eth_ptr = (char *)PARSER_GET_ETH_POINTER_DEFAULT();
	if (dpni_ctr == 12){
		if(*eth_ptr != 0x01)
			local_test_error |= 0x01;
	}

	for(i = 0; i < NET_HDR_FLD_ETH_ADDR_SIZE; i++) /*check if destination mac is broadcast*/
		if(*eth_ptr++ != 0xff)
			break;

	eth_ptr = (char *)PARSER_GET_ETH_POINTER_DEFAULT();
	fsl_os_print("DEST MAC: ");
	for(i = 0; i < NET_HDR_FLD_ETH_ADDR_SIZE; i++){

		fsl_os_print("%x ", *eth_ptr);
		eth_ptr ++;
	}
	fsl_os_print("\n");




	if(i == NET_HDR_FLD_ETH_ADDR_SIZE) /*check if all the destination MAC was broadcast FF:FF:FF:FF:FF:FF*/
	{
		lock_spinlock(&dpni_lock);
		dpni_broadcast_flag = 1;
		unlock_spinlock(&dpni_lock);
	}

	if(dpni_ctr == 10){
		if(dpni_broadcast_flag == 0) {
			fsl_os_print("dpni error - broadcast packets didn't received\n");
			local_test_error |= 0x01;
		}
		else {
			fsl_os_print("dpni success - broadcast packets received during the test\n");
		}

		for(ni = 0; ni < dpni_get_num_of_ni(); ni ++)
		{
			err = dpni_drv_set_multicast_promisc((uint16_t)ni, 1);
			if(err != 0) {
				fsl_os_print("dpni_drv_set_multicast_promisc error for ni %d\n",ni);
				local_test_error |= err;
			}
			else {
				fsl_os_print("dpni_drv_set_multicast_promisc for ni %d succeeded\n",ni);
			}
		}
		for(ni = 0; ni < dpni_get_num_of_ni(); ni ++)
		{
			err = dpni_drv_get_multicast_promisc((uint16_t)ni, &promisc);
			if(err != 0 || promisc != 1) {
				fsl_os_print("dpni_drv_get_multicast_promisc error for ni %d\n",ni);
				local_test_error |= err;
			}
			else {
				fsl_os_print("dpni_drv_get_multicast_promisc for ni %d succeeded\n",ni);
			}
		}

	}

	return local_test_error;
}
