#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_dpni.h"
#include "dplib/dpni_drv.h"
#include "dpni/drv.h"


int app_init(void);
void app_free(void);


static void app_process_packet (dpni_drv_app_arg_t arg)
{
	dpni_drv_send((uint16_t)arg);
}


int app_init(void)
{
    int i, err = 0;
    
    // This code is supposed to tell linker to include function receive_cb() in elf file.
    volatile int always_zero = 0;
    if(always_zero)
    	receive_cb();

    fsl_os_print("AIOP test: app_process_packet \n");

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
