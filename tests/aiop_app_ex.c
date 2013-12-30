#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "dplib/dpni_drv.h"


int aiop_app_init(void);
void aiop_app_free(void);


static void /*__noreturn*/ rflct_rx_cb (dpni_drv_app_arg_t arg)
{
	if (!dpni_drv_is_up((uint16_t)arg))
		return;

	/* TODO - add here:
	 * 1. print frame content */
	/* TODO - add here swap on headers:
	 * swap_eth_hdr((uint32_t *)p_data);
	 * swap_ip_hdr((uint32_t *)(PTR_TO_UINT(p_data)+26));
	 */
	dpni_drv_send((uint16_t)arg);
}


int aiop_app_init(void)
{
    int i, err = 0;

    fsl_os_print("AIOP test: NIC\n");

    for (i=0; i<dpni_get_num_of_ni(); i++)
	    err = dpni_drv_register_rx_cb(i, 0, NULL, NULL, rflct_rx_cb, i);

    return err;
}

void aiop_app_free(void)
{
    /* TODO - complete!*/
}
