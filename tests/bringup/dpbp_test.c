
#include "../../../tests/bringup/dpbp_test.h"
#include "fsl_dprc.h"
#include "fsl_dbg.h"
#include "fsl_sys.h"
#include "fsl_mc_init.h"
#include "fsl_string.h"
#include "aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_icontext.h"
#include "../../../drivers/qbman/include/fsl_bman.h"
#include "fdma.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_dpbp.h"

int dpbp_init();
int dpbp_test();

struct mc_dprc g_mc_dprc;
extern struct icontext icontext_aiop;

int dpbp_init()
{

	void *p_vaddr;
	int err = 0;
	int container_id;
	struct mc_dprc *dprc = &g_mc_dprc;
	extern struct aiop_init_info g_init_data;

	uint32_t cdma_cfg;
	struct aiop_tile_regs *ccsr = (struct aiop_tile_regs *) 0x2080000;


	ASSERT_COND(ccsr);

	cdma_cfg = ioread32_ccsr(&ccsr->cdma_regs.cfg);

	icontext_aiop.icid = (uint16_t)(cdma_cfg & CDMA_ICID_MASK);

	icontext_aiop.bdi_flags = 0;
	if (cdma_cfg & CDMA_BDI_BIT)
		icontext_aiop.bdi_flags |= FDMA_ENF_BDI_BIT;

	icontext_aiop.dma_flags = 0;
	if (cdma_cfg & CDMA_BMT_BIT)
		icontext_aiop.dma_flags |= FDMA_DMA_BMT_BIT;
	if (cdma_cfg & CDMA_PL_BIT)
		icontext_aiop.dma_flags |= FDMA_DMA_PL_BIT;
	if (cdma_cfg & CDMA_VA_BIT)
		icontext_aiop.dma_flags |= FDMA_DMA_eVA_BIT;

	pr_debug("CDMA CFG register = 0x%x addr = 0x%x\n", cdma_cfg, \
	         (uint32_t)&ccsr->cdma_regs.cfg);
	pr_debug("AIOP ICID = 0x%x bdi flags = 0x%x\n", icontext_aiop.icid, \
	         icontext_aiop.bdi_flags);
	pr_debug("AIOP ICID = 0x%x dma flags = 0x%x\n", icontext_aiop.icid, \
	         icontext_aiop.dma_flags);

	ASSERT_COND(icontext_aiop.bdi_flags); /* BDI bit is set */
	ASSERT_COND(icontext_aiop.dma_flags); /* PL bit is set */

	if(err){
		pr_err("icontext init failed: %d\n",err);
	}
	if (dprc == NULL) {
		pr_err("No memory for AIOP Root Container \n");
		return -ENOMEM;
	}
	memset(dprc, 0, sizeof(struct mc_dprc));

	/* TODO : in this call, can 3rd argument be zero? */
	/* Get virtual address of MC portal */
	p_vaddr = (void *) 0xc030000;

	pr_debug("MC portal ID[%d] addr = 0x%x\n", g_init_data.sl_info.mc_portal_id, (uint32_t)p_vaddr);

	/* Open root container in order to create and query for devices */
	dprc->io.regs = p_vaddr;
	if ((err = dprc_get_container_id(&dprc->io, &container_id)) != 0) {
		pr_err("Failed to get AIOP root container ID.\n");
		return err;
	}
	if ((err = dprc_open(&dprc->io, container_id, &dprc->token)) != 0) {
		pr_err("Failed to open AIOP root container DP-RC%d.\n",
		       container_id);
		return err;
	}

	return 0;
}


/*****************************************************************************/
int dpbp_test()
{
	uint16_t dpbp;
	int j, i;
	int err = 0;
	int dev_count;
	int num_bpids = 0, dpbp_id;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = &g_mc_dprc;
	struct dpbp_attr attr;
	struct icontext ic;
	dma_addr_t addr  = 0x6001000000;/*DP-DDR phys address*/

	if (dprc == NULL)
	{
		pr_err("AIOP root container not found.\n");
		return -ENODEV;
	}
	/* TODO: replace the following dpbp_open&init with dpbp_create when available */


	if ((err = dprc_get_obj_count(&dprc->io, dprc->token,
	                              &dev_count)) != 0) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			/* TODO: print conditionally based on log level */
			pr_info("Found DPBP ID: %d\n",dev_desc.id);
			dpbp_id = dev_desc.id;
			num_bpids ++;


			if ((err = dpbp_open(&dprc->io, dpbp_id, &dpbp)) != 0) {
				pr_err("Failed to open DPBP-%d.\n", dpbp_id);
				return err;
			}

			if ((err = dpbp_enable(&dprc->io, dpbp)) != 0) {
				pr_err("Failed to enable DPBP-%d.\n", dpbp_id);
				return err;
			}

			if ((err = dpbp_get_attributes(&dprc->io, dpbp, &attr)) != 0) {
				pr_err("Failed to get attributes from DPBP-%d.\n", dpbp_id);
				return err;
			}


			icontext_aiop_get(&ic);

			for (j = 0; j < 10; j++) {
				fdma_release_buffer(ic.icid, ic.bdi_flags, (uint16_t)dpbp_id, addr);
				addr += 2048;
			}

			pr_info("AIOP: Test passed\n", core_get_id());
		}
	}

	return 0;
}
