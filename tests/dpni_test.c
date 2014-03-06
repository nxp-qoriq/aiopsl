#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "common/io.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpni.h"
#include "stdlib.h"
#include "dplib/dpni_drv.h"
#include "common/dbg.h"

/*
#include "kernel/platform.h"
#include "../kernel/inc/mem_mng.h"
#include "../drivers/dplib/dpni/drv.h" // TODO: place file elsewhere
*/

//extern __SHRAM struct dpni_drv *nis; // TODO: put in sys struct

int dpni_test_init(void);
void dpni_test_free(void);
static void dpni_test_process_packet (dpni_drv_app_arg_t);
	
int dpni_test_init(void)
{
/*
	int portal_id = 0, err = 0, dev_count, tmp = 0, i;
	uint64_t child_portal_paddr = 0;
	void *portal_vaddr;
	struct dprc dprc = { 0 };
	struct dprc child_dprc = { 0 };
	int container_id;
	struct dprc_dev_desc dev_desc;
	struct dprc_res_ids_range_desc range_desc = { 0 };
	struct dpni dpni = { 0 };
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint16_t qdid;
	struct dprc_region_desc region_desc;
	int epid_idx;
*/
	int err;    
	uint32_t ni;

	fsl_os_print("DPNI TEST: Running app_init()\n");

	/* TODO: discover NI's and register accordingly */
	for (ni = 0; ni < SOC_MAX_NUM_OF_DPNI; ni++) {
		err = dpni_drv_register_rx_cb((uint16_t)ni,	/*ni_id*/
				      0,			/*flow_id*/
				      dpni_test_process_packet, /* callback for flow_id*/
				      (dpni_drv_app_arg_t)0);			/*arg*/
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
	fsl_os_print("DPNI TEST: Running dpni_test_process_packet(), arg = %d\n", (int)arg);
}
