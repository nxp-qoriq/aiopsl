#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "common/io.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpni.h"
#include "stdlib.h"
#include "dplib/dpni_drv.h"
#include "common/dbg.h"

int dpni_test_init(void);
static void dpni_test_process_packet (dpni_drv_app_arg_t);
void dpni_test_free(void);


int dpni_test_init(void)
{
	int err;   
	
	fsl_os_print("DPNI TEST: Running app_init()\n");

	err = dpni_drv_register_rx_cb(1,						/* ni_id */
				     0,										/* flow_id */
				     dpni_test_process_packet, 				/* callback for flow_id */
				     (dpni_drv_app_arg_t)0);				/* arg */
	if (err) {
		pr_err("Failed t register callback for DP-NI%d (Internal AIOP NI ID)\n", 1);
		return err;
	}
	
	return 0;
}


static void dpni_test_process_packet (dpni_drv_app_arg_t arg)
{
	int receive_niid;
	int send_niid;
	uint8_t receive_mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint8_t send_mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	
	receive_niid = dpni_get_receive_niid();
	send_niid = 3 - receive_niid;
	
	dpni_set_send_niid((uint16_t)send_niid); 
	send_niid = dpni_get_send_niid();
	
#if 0
	dpni_drv_send((uint16_t)send_niid);
#endif
	
	dpni_drv_get_primary_mac_addr((uint16_t)receive_niid, receive_mac_addr);
	dpni_drv_get_primary_mac_addr((uint16_t)send_niid, send_mac_addr);
	
	fsl_os_print("DPNI TEST: Running dpni_test_process_packet(), Receive NIID = %d (MAC=0x%02X%02X%02X%02X%02X%02X), Send NIID = %d (MAC=0x%02X%02X%02X%02X%02X%02X), arg = %d\n",
				receive_niid, receive_mac_addr[0], receive_mac_addr[1], receive_mac_addr[2], receive_mac_addr[3], receive_mac_addr[4], receive_mac_addr[5],
				send_niid, send_mac_addr[0], send_mac_addr[1], send_mac_addr[2], send_mac_addr[3], send_mac_addr[4], send_mac_addr[5], (int)arg);
}


void dpni_test_free(void)
{
	/* TODO - complete!*/
}

