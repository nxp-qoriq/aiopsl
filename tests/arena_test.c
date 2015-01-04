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

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_dpni_drv.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_time.h"
#include "fsl_ip.h"
#include "fsl_cdma.h"
#include "slab.h"
#include "platform.h"
#include "fsl_io.h"
#include "aiop_common.h"
#include "kernel/fsl_spinlock.h"
#include "dplib/fsl_parser.h"
#include "fsl_osm.h"
#include "fsl_dbg.h"

int app_early_init(void);
int app_init(void);
void app_free(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */


#define MAX_NUM_OF_CORES	16
#define MAX_NUM_OF_TASKS	16
#define NH_FLD_IP_VER                         (1)
#define NH_FLD_IP_SRC                         (NH_FLD_IP_VER << 5)

extern int slab_init(void);
extern int malloc_test();
extern int slab_test(void);
extern int random_init(void);
extern int random_test(void);
extern int pton_test(void);
extern int ntop_test(void);
extern int dpni_drv_test(void);

extern int num_of_cores;
extern int num_of_tasks;
extern uint32_t rnd_seed[MAX_NUM_OF_CORES][MAX_NUM_OF_TASKS];
extern __TASK uint32_t	seed_32bit;
extern struct slab_bman_pool_desc g_slab_bman_pools[SLAB_MAX_BMAN_POOLS_NUM];
uint8_t dpni_lock; /*lock to change dpni_ctr and dpni_broadcast_flag safely */
uint8_t dpni_ctr; /*counts number of packets received before removing broadcast address*/
uint8_t dpni_broadcast_flag; /*flag if packet with broadcast mac destination received during the test*/
uint8_t packet_number;
uint8_t packet_lock;
uint8_t time_lock;
uint64_t global_time;

int test_error;
uint8_t test_error_lock;

static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	int      err = 0, i, j;
	int core_id;
	uint64_t time_ms_since_epoch = 0, flc = 0;
	uint32_t time_ms = 0;
	uint64_t local_time;
	uint8_t local_packet_number;
	int local_test_error = 0;
	uint16_t spid_ddr;
	lock_spinlock(&packet_lock);
	local_packet_number = packet_number;
	packet_number++;
	unlock_spinlock(&packet_lock);
	core_id = (int)core_get_id();

	fsl_os_print("Arena test for packet number %d, on core %d\n", local_packet_number, core_id);
	err = dpni_drv_get_spid_ddr(APP_NI_GET(arg), &spid_ddr);
	if (err) {
		fsl_os_print("ERROR = %d: get spid_ddr failed in runtime phase()\n", err);
		local_test_error |= err;
	} else {
		fsl_os_print("spid_ddr is %d for packet %d\n",spid_ddr, local_packet_number);

	}
	err = dpni_drv_get_spid(APP_NI_GET(arg), &spid_ddr);
	if (err) {
		fsl_os_print("ERROR = %d: get spid failed in runtime phase()\n", err);
		local_test_error |= err;
	} else {
		fsl_os_print("spid is %d for packet %d\n",spid_ddr, local_packet_number);
	}


	err = pton_test();
	if (err) {
		fsl_os_print("ERROR = %d: pton_test failed in runtime phase()\n", err);
		local_test_error |= err;
	} else {
		fsl_os_print("pton_test passed in runtime phase()\n");
	}

	err = ntop_test();
	if (err) {
		fsl_os_print("ERROR = %d: ntop_test failed in runtime phase()\n", err);
		local_test_error |= err;
	} else {
		fsl_os_print("ntop_test passed in runtime phase()\n");
	}

	err = slab_test();
	if (err) {
		fsl_os_print("ERROR = %d: slab_test failed  in runtime phase \n", err);
		local_test_error |= err;
	} else {
		fsl_os_print("Slab test passed for packet number %d, on core %d\n", local_packet_number, core_id);
	}

	err = malloc_test();

	if (err) {
		fsl_os_print("ERROR = %d: malloc_test failed in runtime phase \n", err);
		local_test_error |= err;
	} else {
		fsl_os_print("Malloc test passed for packet number %d, on core %d\n", local_packet_number, core_id);
	}

	/*Random Test*/

	err = random_test();
	if (err) {
		fsl_os_print("ERROR = %d: random_test failed in runtime phase \n", err);
		if(seed_32bit != 0)/*TODO remove this condition after moving to sim f118*/
			local_test_error |= err;
	}
	else{
		fsl_os_print("seed %x\n",seed_32bit);
		fsl_os_print("Random test passed for packet number %d, on core %d\n", local_packet_number, core_id);
	}




	err = dpni_drv_test();
	if (err) {
		fsl_os_print("ERROR = %d: dpni_drv_test failed in runtime phase()\n", err);
		local_test_error |= err;
	} else {
		fsl_os_print("dpni_drv_test passed in runtime phase()\n");
	}

	flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	fsl_os_print("FLC: 0x%llx\n",flc);

	lock_spinlock(&time_lock);
	err = fsl_get_time_ms(&time_ms);
	err |= fsl_get_time_since_epoch_ms(&time_ms_since_epoch);

	if(err){
		fsl_os_print("ERROR = %d: fsl_os_gettimeofday failed  in runtime phase \n", err);
		local_test_error |= err;
		unlock_spinlock(&time_lock);
	}else {

		fsl_os_print("time ms is: %d milliseconds \n",time_ms);
		fsl_os_print("time since epoch is: %ll milliseconds\n",time_ms_since_epoch);


		local_time = time_ms_since_epoch;

		if(local_time >= global_time)
		{
			fsl_os_print("time test passed for packet number %d, on core %d\n", local_packet_number, core_id);
			global_time = local_time;
		}
		else
		{
			fsl_os_print("ERROR = %d: time test failed in runtime phase \n", err);
			local_test_error |= 0x01;
		}
		unlock_spinlock(&time_lock);
	}







	local_test_error |= dpni_drv_send(APP_NI_GET(arg));

	lock_spinlock(&test_error_lock);
	test_error |= local_test_error; /*mark if error occured during one of the tests*/
	unlock_spinlock(&test_error_lock);

	if(local_packet_number == 38 ){ /*40 packets (0 - 39) with one broadcast after the broadcast is dissabled */
		if (test_error == 0)
		{
			int not_active_task = 0;
			fsl_os_print("No errors were found during injection of 40 packets\n");
			fsl_os_print("1 packet was sent with removed MAC address\n");
			fsl_os_print("Only 39 (0-38) packets should be received\n");
			fsl_os_print("Test executed with %d cores and %d tasks per core\n", num_of_cores, num_of_tasks);
			fsl_os_print("Cores/Tasks processed packets during the test:\n");
			fsl_os_print("CORE/TASK ");
			for(i = 0; i < num_of_tasks; i++)
				fsl_os_print("  %d ",i);

			for(i = 0; i < num_of_cores; i++)
			{
				if(i < 10)
					fsl_os_print("\nCore  %d:  ", i);
				else
					fsl_os_print("\nCore %d:  ", i);
				for(j = 0; j < num_of_tasks; j++)
				{
					if(rnd_seed[i][j] == 0)
					{
						not_active_task ++;

						if(j < 10)
							fsl_os_print("  X ");
						else
							fsl_os_print("   X ");
					}
					else
					{
						if(j <10)
							fsl_os_print("  V ");
						else
							fsl_os_print("   V ");
					}
				}
			}

			if(not_active_task > 0){
				fsl_os_print("\nWARNING: Not all the tasks were active during the test!\n");

			}


			fsl_os_print("\nARENA Test Finished SUCCESSFULLY\n");
			for(i = 0; i < SLAB_MAX_BMAN_POOLS_NUM; i++){

				fsl_os_print("Slab bman pools status:\n");
				fsl_os_print("bman pool id: %d, remaining: %d\n",g_slab_bman_pools[i].bman_pool_id, g_slab_bman_pools[i].remaining);

			}

		}
		else {
			fsl_os_print("ARENA Test Finished with ERRORS\n");
		}
	}
}
int app_early_init(void){
	int err = 0;
	err |= slab_register_context_buffer_requirements(200,250,200,64,MEM_PART_SYSTEM_DDR,0, 0);
	err |= slab_register_context_buffer_requirements(200,250,200,64,MEM_PART_PEB,0, 0);
	err |= slab_register_context_buffer_requirements(200,250,504,64,MEM_PART_DP_DDR,0, 120);
	if(err)
		pr_err("slab_register_context_buffer_requirements failed: %d\n",err);

	return 0;
}

int app_init(void)
{
	int        err  = 0;
	uint16_t spid = 0;
	uint32_t   ni   = 0;
	uint16_t   ni2  = 0;
	dma_addr_t buff = 0;
	int ep, state = -1;
	struct dpkg_profile_cfg dist_key_cfg = {0};
	struct aiop_psram_entry *sp_addr;
//	struct dpni_buffer_layout layout = {0};

	dist_key_cfg.num_extracts = 1;
	dist_key_cfg.extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	dist_key_cfg.extracts[0].extract.from_hdr.prot = NET_PROT_IP;
	dist_key_cfg.extracts[0].extract.from_hdr.field = NET_HDR_FLD_IP_SRC;
	dist_key_cfg.extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;


	fsl_os_print("Running AIOP arena app_init()\n");

	for (ni = 0; ni < dpni_get_num_of_ni(); ni++)
	{
		err = dpni_drv_add_mac_addr((uint16_t)ni, ((uint8_t []){0x02, 0x00 ,0xc0 ,0x0a8 ,0x0b ,0xfe }));

		if (err){
			fsl_os_print("dpni_drv_add_mac_addr failed %d\n", err);
			test_error |= 0x01;
		}
		else{
			fsl_os_print("dpni_drv_add_mac_addr succeeded in boot\n");
			fsl_os_print("MAC 02:00:C0:A8:0B:FE added for ni %d\n",ni);
		}
		dpni_drv_set_exclusive((uint16_t)ni);
		err = dpni_drv_set_order_scope((uint16_t)ni,&dist_key_cfg);
		if (err){
			fsl_os_print("dpni_drv_set_order_scope failed %d\n", err);
					return err;
		}

		err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/,
		                              app_process_packet_flow0, /* callback */
		                              ni /*arg, nic number*/);

		if (err)
			return err;

		ep = dpni_drv_get_ordering_mode((uint16_t)ni);
		fsl_os_print("initial order scope execution phase for tasks %d\n",ep);

	}

	err = slab_init();
	if (err) {
		fsl_os_print("ERROR = %d: slab_init failed  in init phase()\n", err);
		test_error |= err;
	}
	else
		fsl_os_print("slab_init  succeeded  in init phase()\n", err);

	err = malloc_test();
	if (err) {
		fsl_os_print("ERROR = %d: malloc_test failed in init phase()\n", err);
		test_error |= err;
	}
	else
		fsl_os_print("malloc_test succeeded  in init phase()\n", err);
	err = random_init();
	if (err) {
		fsl_os_print("ERROR = %d: random_test failed in init phase()\n", err);
		test_error |= err;
	} else {
		fsl_os_print("random_test passed in init phase()\n");
	}

	err = pton_test();
	if (err) {
		fsl_os_print("ERROR = %d: pton_test failed in init phase()\n", err);
		test_error |= err;
	} else {
		fsl_os_print("pton_test passed in init phase()\n");
	}

	err = ntop_test();
	if (err) {
		fsl_os_print("ERROR = %d: ntop_test failed in init phase()\n", err);
		test_error |= err;
	} else {
		fsl_os_print("ntop_test passed in init phase()\n");
	}

	for(ni = 0; ni < dpni_get_num_of_ni(); ni++)
	{
		err = dpni_drv_get_connected_dpni_id((uint16_t)ni, &ni2, &state);
		fsl_os_print("Given NI: %d, Connected NI: %d, Status: %d\n",ni,ni2,state);
		if(err){
			fsl_os_print("Error: dpni_drv_get_connected_dpni_id: error %d\n",err);
			test_error |= 0x01;
		}
#if 0/*Remove if 0 when support for setting SP configuration*/
		layout.options =  DPNI_BUF_LAYOUT_OPT_DATA_HEAD_ROOM |
			DPNI_BUF_LAYOUT_OPT_DATA_TAIL_ROOM;
		layout.data_head_room = 0x20;
		layout.data_tail_room = 0x30;
		err = dpni_drv_set_rx_buffer_layout((uint16_t)ni,&layout );
		if(err){
			fsl_os_print("Error: dpni_drv_get_rx_buffer_layout: error %d\n",err);
			test_error |= 0x01;
		}

		layout.options = DPNI_BUF_LAYOUT_OPT_DATA_HEAD_ROOM |
			DPNI_BUF_LAYOUT_OPT_DATA_TAIL_ROOM;
		layout.data_head_room = 0;
		layout.data_tail_room = 0;

		err = dpni_drv_get_rx_buffer_layout((uint16_t)ni,&layout );
		if(err){
			fsl_os_print("Error: dpni_drv_get_rx_buffer_layout: error %d\n",err);
			test_error |= 0x01;
		}
		fsl_os_print("Buffer Layout:\n");
		fsl_os_print("Options: 0x%x\n",layout.options);
		fsl_os_print("data_head_room: 0x%x\n\n\n", layout.data_head_room);
		fsl_os_print("data_tail_room: 0x%x\n\n\n", layout.data_tail_room);
		if(layout.data_head_room != 0x20 || layout.data_tail_room != 0x30){
			fsl_os_print("Error: dpni_drv_get/set_rx_buffer_layout finished with incorrect values\n");
			test_error |= 0x01;
		}
#endif


		sp_addr = (struct aiop_psram_entry *)
						(AIOP_PERIPHERALS_OFF + AIOP_STORAGE_PROFILE_OFF);
		err = dpni_drv_get_spid((uint16_t)ni, &spid);
		if (err) {
			fsl_os_print("ERROR = %d: dpni_drv_get_spid failed\n", err);
			test_error |= err;
		} else {
			fsl_os_print("NI %d - spid is %d\n", ni, spid);
		}

		sp_addr += spid;
		if(sp_addr->bp1 == 0){
			fsl_os_print("Error: spid bp1 is 0\n");
			test_error |= 0x01;
		}

		sp_addr = (struct aiop_psram_entry *)
			(AIOP_PERIPHERALS_OFF + AIOP_STORAGE_PROFILE_OFF);
		err = dpni_drv_get_spid_ddr((uint16_t)ni, &spid);
		if (err) {
			fsl_os_print("ERROR = %d: dpni_drv_get_spid_ddr failed\n", err);
			test_error |= err;
		} else {
			fsl_os_print("NI %d - spid DDR is %d\n", ni, spid);
		}

		sp_addr += spid;
		if(sp_addr->bp1 == 0){
			fsl_os_print("Error: spid ddr bp1 is 0\n");
			test_error |= 0x01;
		}


	}

	fsl_os_print("To start test inject packets: \"arena_test_40.pcap\" after AIOP boot complete.\n");
	return 0;
}

void app_free(void)
{

}
