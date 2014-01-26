#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_dpni.h"
#include "dplib/dpni_drv.h"
#include "dpni/drv.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_ip.h"
#include "fsl_parser.h"

int app_init(void);
void app_free(void);


static void app_process_packet (dpni_drv_app_arg_t arg)
{
	// Change the first 4 bytes of the frame
	*((uint32_t *) PRC_GET_SEGMENT_ADDRESS()) = 0xdeadbeef;
	fdma_modify_default_segment_data(0,4);
	/*
	ret = ip_set_nw_src(src_addr);
	if (!ret)
			fsl_os_print("AIOP test: Error while replacing header src address\n");

	 */
	dpni_drv_send((uint16_t)arg);
	fdma_terminate_task();
}


int app_init(void)
{
    int i, err = 0;
    
    // This code is supposed to tell linker to include function receive_cb() in elf file.
    volatile int always_zero = 0;
    if(always_zero)
    	receive_cb();

    //fsl_os_print("AIOP test: app_process_packet \n");

    err = dpni_drv_register_rx_cb(0/*ni_id*/, 
                                  0/*flow_id*/, 
                                  NULL/*dpio*/, 
                                  NULL /*dpsp*/, 
                                  app_process_packet, 
                                  0/*arg, nic number*/);

    return err;
}

void app_free(void)
{
    /* TODO - complete!*/
}
