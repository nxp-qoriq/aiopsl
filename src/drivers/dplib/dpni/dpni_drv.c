#include "common/types.h"
#include "dplib/dpni_drv.h"


#define __ERR_MODULE__  MODULE_DPNI

#define DPNI_DRV_FLG_ENABLED    0x80
#define DPNI_DRV_FLG_PARSE      0x40
#define DPNI_DRV_FLG_GEN_CS     0x20


void receive_cb (void);

int dpni_drv_init(void);
void dpni_drv_free(void);


struct dpni_drv {
    uint8_t             id;
    uint8_t             flags;
    uint16_t            spid;
    uint16_t            qdid;
    rx_cb_t             *rx_cbs[DPNI_DRV_MAX_NUM_FLOWS];
    dpni_drv_app_arg_t  args[DPNI_DRV_MAX_NUM_FLOWS];
};


void receive_cb (void)
{
    /* TODO - complete!!! */
}

int dpni_drv_send(uint16_t ni_id)
{
    /* TODO - complete!!! */
    return 0;
}


int dpni_drv_init(void)
{
    /* TODO - complete!!! */
    return 0;
}

void dpni_drv_free(void)
{
    /* TODO - complete!!! */
}
