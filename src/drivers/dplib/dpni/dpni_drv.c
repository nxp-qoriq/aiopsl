#include "common/types.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "common/io.h"
#include "dplib/dpni_drv.h"
#include "kernel/platform.h"
#include "inc/sys.h"


#define __ERR_MODULE__  MODULE_DPNI

#define DPNI_DRV_FLG_ENABLED    0x80
#define DPNI_DRV_FLG_PARSE      0x40
#define DPNI_DRV_FLG_GEN_CS     0x20


void receive_cb (void);



void receive_cb (void);

int dpni_drv_init(void);
void dpni_drv_free(void);


struct dpni_drv {
    uint8_t             id;    uint8_t             flags;	/** network interface ID */
    uint16_t            id;
    /** Storage profile ID */
    /* TODO - Need to store it in HW context */
    uint16_t            spid;
    /** Queueing destination for the enqueue. */
    uint16_t            qdid;
    /** MTU value needed for the \ref dpni_drv_send() function */
    /** TODO - if crossed need to update FD[ERR] what bit??? */
    uint16_t            mtu;
    /** starting HXS */
    uint16_t            starting_hxs;
    /** Parse Profile ID */
    uint8_t            prpid;
    /** \ref DPNI_DRV_DEFINES */
    uint8_t             flags;
    /** error mask for the \ref receive_cb() function FD
    * error check 0 - continue; 1 - discard */
    uint8_t             fd_err_mask;
    uint8_t             res[3];

    rx_cb_t             *rx_cbs[DPNI_DRV_MAX_NUM_FLOWS];
    dpni_drv_app_arg_t  args[DPNI_DRV_MAX_NUM_FLOWS];
};

#if 0
extern __TASK uint8_t CURRENT_SCOPE_LEVEL;
extern __TASK uint8_t SCOPE_MODE_LEVEL1;
extern __TASK uint8_t SCOPE_MODE_LEVEL2;
extern __TASK uint8_t SCOPE_MODE_LEVEL3;
extern __TASK uint8_t SCOPE_MODE_LEVEL4;
#endif /* 0 */

/* TODO - get rid */
struct dpni_drv *nis;


#if 0
static void osm_task_init(void)
{
    CURRENT_SCOPE_LEVEL = ((uint8_t)PRC_GET_OSM_SOURCE_VALUE());
        /**<    0- No order scope specified.\n
            1- Scope was specified for level 1 of hierarchy */
    SCOPE_MODE_LEVEL1 = ((uint8_t)PRC_GET_OSM_EXECUTION_PHASE_VALUE());
        /**<    0 = Exclusive mode.\n
            1 = Concurrent mode. */
    SCOPE_MODE_LEVEL2 = 0x00;
        /**<    Exclusive (default) Mode in level 2 of hierarchy */
    SCOPE_MODE_LEVEL3 = 0x00;
        /**<    Exclusive (default) Mode in level 3 of hierarchy */
    SCOPE_MODE_LEVEL4 = 0x00;
        /**<    Exclusive (default) Mode in level 4 of hierarchy */
}
#endif /* 0 */


void receive_cb (void)
{
	struct dpni_drv *ni = (struct dpni_drv *)(PTR_TO_UINT(nis) + 10*sizeof(struct dpni_drv));    struct dpni_drv *ni = (struct dpni_drv *)(PTR_TO_UINT(nis) + 10*sizeof(struct dpni_drv));

	/* TODO - temporary code for simulator */	__asm__ volatile (		"se_li r0,0x0 \n"		"se_li r2,32 \n"		"se_or r0,r2 \n"		"e_li r4,1553 \n"		"se_stw r4,0(r0) \n"		"se_li r2,36 \n"		"se_or r0,r2 \n"		"e_lis r4,0x00f0 \n"		"se_li r5,18 \n"		"se_or r4,r5 \n"		"se_stw r4,0(r0) \n"		"se_li r2,0 \n"		"e_hwacceli 0x000c \n"	);    /* TODO - temporary code for simulator */
    __asm__ volatile (
        "se_li r0,0x0 \n"
        "se_li r2,32 \n"
        "se_or r0,r2 \n"
        "e_li r4,1553 \n"
        "se_stw r4,0(r0) \n"
        "se_li r2,36 \n"
        "se_or r0,r2 \n"
        "e_lis r4,0x00f0 \n"
        "se_li r5,18 \n"
        "se_or r4,r5 \n"
        "se_stw r4,0(r0) \n"
        "se_li r2,0 \n"
        "e_hwacceli 0x000c \n"
    );

	if (ni->rx_cbs[0])		ni->rx_cbs[0](ni->args[0]);	/* TODO - complete!!! */    if (ni->rx_cbs[0])
        ni->rx_cbs[0](ni->args[0]);
    /* TODO - complete!!! */
}

int dpni_drv_send(uint16_t ni_id)
{
    /* TODO - complete!!! */
    return 0;
}


int dpni_drv_init(void)
{
	uintptr_t   tmp_reg;    uintptr_t   tmp_reg;

	nis = fsl_os_malloc(sizeof(struct dpni_drv)*100);    nis = fsl_os_malloc(sizeof(struct dpni_drv)*100);
    if (!nis)
    	RETURN_ERROR(MAJOR, E_NO_MEMORY, ("NI objs!"));        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("NI objs!"));
    memset(nis, 0, sizeof(struct dpni_drv)*100);

	tmp_reg =	    sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,	                                      0,	                                      E_MAPPED_MEM_TYPE_GEN_REGS);	/* Write EPID-table EP_PC reg */	iowrite32be(PTR_TO_UINT(receive_cb), UINT_TO_PTR(tmp_reg + 0x1d100));    tmp_reg =
        sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,
                                          0,
                                          E_MAPPED_MEM_TYPE_GEN_REGS);
    /* Write EPID-table EP_PC reg */
    iowrite32be(PTR_TO_UINT(receive_cb), UINT_TO_PTR(tmp_reg + 0x1d100));

	return 0;    return 0;
}

void dpni_drv_free(void)
{
    /* TODO - complete!!! */
}
