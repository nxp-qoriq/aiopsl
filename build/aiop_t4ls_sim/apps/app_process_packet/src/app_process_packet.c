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
	uint32_t src_addr = 0x10203040;// new ipv4 src_addr
	int32_t ret = SUCCESS;
	ret = ip_set_nw_src(src_addr);
	if (ret != SUCCESS)
		fsl_os_print("AIOP test: Error while replacing header src address\n");
	dpni_drv_send((uint16_t)arg);
	fdma_terminate_task();
}

int app_init(void)
{
    int err = 0;
    
    // This code is supposed to tell linker to include function receive_cb() in elf file.
    volatile int always_zero = 0;
    if(always_zero)
        receive_cb();

    err = dpni_drv_register_rx_cb(0/*ni_id*/, 
                                  0/*flow_id*/, 
                                  NULL/*dpio*/, 
                                  NULL /*dpsp*/, 
                                  app_process_packet, /* callback for flow_id*/
                                  0/*arg, nic number*/);
    
    return err;
}

void app_free(void)
{
    /* TODO - complete!*/
}
