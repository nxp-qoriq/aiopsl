#include "common/types.h"
<<<<<<< HEAD
=======
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "common/io.h"
#include "dplib/dpni_drv.h"
#include "kernel/platform.h"
#include "inc/sys.h"

>>>>>>> master


#define __ERR_MODULE__  MODULE_DPNI


int dpni_drv_init(void);
void dpni_drv_free(void);


<<<<<<< HEAD
=======
struct dpni_drv {
    uint8_t             id;
    uint8_t             flags;
    uint16_t            spid;
    uint16_t            qdid;
    rx_cb_t             *rx_cbs[DPNI_DRV_MAX_NUM_FLOWS];
    dpni_drv_app_arg_t  args[DPNI_DRV_MAX_NUM_FLOWS];
};


/* TODO - get rid */
struct dpni_drv *nis;


void receive_cb (void)
{
	struct dpni_drv *ni = (struct dpni_drv *)(PTR_TO_UINT(nis) + 10*sizeof(struct dpni_drv));

	/* TODO - temporary code for simulator */
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

	if (ni->rx_cbs[0])
		ni->rx_cbs[0](ni->args[0]);
	/* TODO - complete!!! */
}

int dpni_drv_send(uint16_t ni_id)
{
    /* TODO - complete!!! */
    return 0;
}


>>>>>>> master
int dpni_drv_init(void)
{
	uintptr_t   tmp_reg;

	nis = fsl_os_malloc(sizeof(struct dpni_drv)*100);
    if (!nis)
    	RETURN_ERROR(MAJOR, E_NO_MEMORY, ("NI objs!"));
    memset(nis, 0, sizeof(struct dpni_drv)*100);

	tmp_reg =
	    sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,
	                                      0,
	                                      E_MAPPED_MEM_TYPE_GEN_REGS);
	/* Write EPID-table EP_PC reg */
	iowrite32be(PTR_TO_UINT(receive_cb), UINT_TO_PTR(tmp_reg + 0x1d100));

	return 0;
}

void dpni_drv_free(void)
{
    /* TODO - complete!!! */
}
