#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_dpni.h"
#include "dplib/dpni_drv.h"
#include "dpni/drv.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_ip.h"
#include "fsl_cdma.h"
#include "common/fsl_slab.h"
#include "kernel/platform.h"

int app_init(void);
void app_free(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))          
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16) 
/**< Get flow id from callback argument, it's demo specific macro */

uint32_t   slab = 0;

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
    uint64_t buff = 0;
    uint64_t data1 = 0xAABBCCDD;
    uint64_t data2 = 0;
    int err       = 0;
    
    if (slab_acquire(slab, &buff)) {
        fdma_terminate_task();
    } 
    
    err = cdma_write(buff, &data1, 8);        
    err = cdma_read(&data2, buff, 8);
    if (data1 != data2) {
        slab_release(slab, buff);
        fdma_terminate_task();              
    } else {
        slab_release(slab, buff);        
    }

    app_process_packet_flow0 (arg);
}

int app_init(void)
{
    int        err  = 0;    
    uint32_t   ni   = 0;
    dma_addr_t buff = 0;

    fsl_os_print("Running app_init()\n");
    
    for (ni = 0; ni < 6; ni++)
    {
        uint32_t flow_id = 1;
        err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/, 
                                      (uint16_t)flow_id/*flow_id*/, 
                                      NULL/*dpio*/, 
                                      NULL /*dpsp*/, 
                                      app_process_packet_flow1, /* callback for flow_id*/
                                      (ni | (flow_id << 16)) /*arg, nic number*/);
        if (err) return err;
        flow_id = 0;
        err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/, 
                                      (uint16_t)flow_id/*flow_id*/, 
                                      NULL/*dpio*/, 
                                      NULL /*dpsp*/, 
                                      app_process_packet_flow0, /* callback for flow_id*/
                                      (ni | (flow_id << 16))/*arg, nic number*/);
        if (err) return err;
    }
    
    err = slab_create(5, 1, 100, 0, 0, 4, MEM_PART_PEB, 0, NULL, &slab);
    if (err) return err;
    fsl_os_print("--------------------slab_create() passed --------------------\n");

    err = slab_acquire(slab, &buff);
    if (err) return err;
    fsl_os_print("--------------------slab_acquire() passed --------------------\n");

    err = slab_release(slab, buff);
    if (err) return err;
    fsl_os_print("--------------------slab_release() passed --------------------\n");

    return err;
}

void app_free(void)
{
    /* TODO - complete!*/
    slab_free(slab);
    fsl_os_print("--------------------slab_free() passed --------------------\n");
}
