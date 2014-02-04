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

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))          
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16) 
/**< Get flow id from callback argument, it's demo specific macro */

static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	uint32_t src_addr = 0x10203040;// new ipv4 src_addr
	int32_t ret = SUCCESS;
	ret = ip_set_nw_src(src_addr);
	if (ret != SUCCESS)
		fsl_os_print("AIOP test: Error while replacing header src address\n");
	dpni_drv_send(APP_NI_GET(arg));
	fdma_terminate_task();
}

static void app_process_packet_flow1 (dpni_drv_app_arg_t arg)
{    
    app_process_packet_flow0 (arg);
}

int app_init(void)
{
    int err = 0;    
    uint32_t ni = 0;
    
    for (ni = 0; ni < 6; ni++)
    {
        uint32_t flow_id = 1;
        err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/, 
                                      (uint16_t)flow_id/*flow_id*/, 
                                      NULL/*dpio*/, 
                                      NULL /*dpsp*/, 
                                      app_process_packet_flow1, /* callback for flow_id*/
                                      (ni | (flow_id << 16)) /*arg, nic number*/);
        if (err > 0) return err;
        flow_id = 0;
        err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/, 
                                      (uint16_t)flow_id/*flow_id*/, 
                                      NULL/*dpio*/, 
                                      NULL /*dpsp*/, 
                                      app_process_packet_flow0, /* callback for flow_id*/
                                      (ni | (flow_id << 16))/*arg, nic number*/);
        if (err > 0) return err;
    }
    return err;
}

void app_free(void)
{
    /* TODO - complete!*/
}
