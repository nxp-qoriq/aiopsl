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

#define RECEIVE_NIID	1
#define SEND_NIID		2

int dpni_test_init(void)
{
	int err;   
	
	fsl_os_print("DPNI TEST: Running app_init()\n");

	err = dpni_drv_register_rx_cb((uint16_t)RECEIVE_NIID,	/* ni_id */
				     0,										/* flow_id */
				     dpni_test_process_packet, 				/* callback for flow_id */
				     (dpni_drv_app_arg_t)0);				/* arg */
	if (err) {
		pr_err("Failed t register callback for DP-NI%d (Internal AIOP NI ID)\n", RECEIVE_NIID);
		return err;
	}
	
	return 0;
}


static void dpni_test_process_packet (dpni_drv_app_arg_t arg)
{
	int receive_niid;
	int send_niid;
	int receive_mac_addr;
	int send_mac_addr;
	
	receive_niid = dpni_get_receive_niid();
	dpni_set_send_niid(SEND_NIID); 
	send_niid = dpni_get_send_niid();
	receive_mac_addr = dpni_drv_get_primary_mac_addr(receive_niid);
	send_mac_addr = dpni_drv_get_primary_mac_addr(send_niid);
	
	fsl_os_print("DPNI TEST: Running dpni_test_process_packet(), Receive NIID = %d (MAC=0x%x), Send NIID = %d (MAC=0x%x), arg = %d\n",
				receive_niid, receive_mac_addr, send_niid, send_mac_addr, (int)arg);
}


void dpni_test_free(void)
{
	/* TODO - complete!*/
}

