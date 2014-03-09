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
void dpni_test_free(void);
static void dpni_test_process_packet (dpni_drv_app_arg_t);
	
int dpni_test_init(void)
{
	uint32_t ni;
	int err;   
	
	fsl_os_print("DPNI TEST: Running app_init()\n");

	/* TODO: discover NI's and register accordingly */
	for (ni = 0; ni < SOC_MAX_NUM_OF_DPNI; ni++) {
		err = dpni_drv_register_rx_cb((uint16_t)ni,	/*ni_id*/
				      0,							/*flow_id*/
				      dpni_test_process_packet, 	/* callback for flow_id*/
				      (dpni_drv_app_arg_t)0);		/*arg*/
		if (err) {
			pr_err("Failed t register callback for DP-NI%d (Internal AIOP NI ID)\n", ni);
			return err;
		}
	}

	return 0;
}

void dpni_test_free(void)
{
	/* TODO - complete!*/
}

static void dpni_test_process_packet (dpni_drv_app_arg_t arg)
{
	int receive_niid;
	int send_niid;
	
	receive_niid = DPNI_GET_RECEIVE_NIID();
	DPNI_SET_SEND_NIID(2); 
	send_niid = DPNI_GET_SEND_NIID();
	
	fsl_os_print("DPNI TEST: Running dpni_test_process_packet(), Receive NIID = %d, Send NIID = %d, arg = %d\n",
				receive_niid, send_niid, (int)arg);
}
