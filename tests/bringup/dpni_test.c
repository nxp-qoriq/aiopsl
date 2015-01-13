
#include "dpni_test.h"
#include "bringup_tests.h"
#include "fsl_dprc.h"
#include "fsl_dbg.h"
#include "fsl_sys.h"
#include "fsl_mc_init.h"
#include "fsl_string.h"
#include "aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_icontext.h"
#include "fsl_bman.h"
#include "fdma.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_dpbp.h"
#include "fsl_dpni_drv.h"


int dpni_init();
int dpni_test();
extern icontext_init();
struct mc_dprc g_mc_dprc_dpni;
extern struct icontext icontext_aiop;
int test_num_of_nis;
struct dpni_pools_cfg pools_params;

struct dpni_drv nis_table[64];

int dpni_init()
{

	void *p_vaddr;
	int err = 0;
	int container_id;
	struct mc_dprc *dprc = &g_mc_dprc_dpni;
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
	test_num_of_nis = 0;

	return 0;
}

__COLD_CODE static void print_dev_desc(struct dprc_obj_desc* dev_desc)
{
	pr_debug(" device %d\n", dev_desc->id);
	pr_debug("***********\n");
	pr_debug("vendor - %x\n", dev_desc->vendor);

	if (strcmp(dev_desc->type, "dpni") == 0)
	{
		pr_debug("type - DP_DEV_DPNI\n");
	}
	else if (strcmp(dev_desc->type, "dprc") == 0)
	{
		pr_debug("type - DP_DEV_DPRC\n");
	}
	else if (strcmp(dev_desc->type, "dpio") == 0)
	{
		pr_debug("type - DP_DEV_DPIO\n");
	}
	pr_debug("id - %d\n", dev_desc->id);
	pr_debug("region_count - %d\n", dev_desc->region_count);
	pr_debug("ver_major - %d\n", dev_desc->ver_major);
	pr_debug("ver_minor - %d\n", dev_desc->ver_minor);
	pr_debug("irq_count - %d\n\n", dev_desc->irq_count);

}


/*****************************************************************************/
int dpni_test()
{
	uint16_t dpbp;
	int i;
	int err = 0;
	int dev_count;
	int num_bpids = 0, dpbp_id = -1;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = &g_mc_dprc_dpni;
	struct dpbp_attr attr;
	struct icontext ic;
	dma_addr_t addr  = 0x4c00002000; /*PEB physical address*/

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

#if TEST_DPBP

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			/* TODO: print conditionally based on log level */
			pr_info("Found DPBP ID: %d\n",dev_desc.id);
			dpbp_id = dev_desc.id;
			break;
		}
	}

	if(dpbp_id == -1){
		pr_err("DPBP NOT FOUND\n");
		return -EINVAL;
	}

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

	for (i = 0; i < 10; i++) {
		fdma_release_buffer(ic.icid, ic.bdi_flags, (uint16_t)dpbp_id, addr);
		addr += 2048;
	}
	pools_params.num_dpbp = 1; /* for AIOP, can be up to 2 */
	pools_params.pools[0].dpbp_id = (uint16_t)dpbp_id; /*!< DPBPs object id */
	pools_params.pools[0].buffer_size = 2048;



#endif


	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpni") == 0) {
			/* TODO: print conditionally based on log level */
			print_dev_desc(&dev_desc);

			if ((err = test_dpni_drv_probe(dprc, (uint16_t)dev_desc.id, (uint16_t)i)) != 0) {
				pr_err("Failed to probe DPNI-%d.\n", i);
				return err;
			}
		}
	}

	if(test_num_of_nis)
	{
		pr_info("AIOP core %d: Test passed\n", core_get_id());
	}
	else
	{
		pr_err("Test failed\n");
		return -ENAVAIL;

	}
	return 0;
}

int test_dpni_drv_probe(struct mc_dprc *dprc,
                        uint16_t mc_niid,
                        uint16_t aiop_niid)
{
	int i;
	uint32_t j;
	uint32_t ep_osc;
	int err = 0;
	uint16_t dpni = 0;
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint16_t qdid;
	uint16_t spid;
	struct dpni_attr attributes;

	struct aiop_ws_regs *wrks_addr =  (struct aiop_ws_regs *)(0x2080000 + SOC_PERIPH_OFF_AIOP_WRKS);
	/* TODO: replace 1024 w/ #define from Yulia */
	/* Search for NIID (mc_niid) in EPID table and prepare the NI for usage. */
	for (i = AIOP_EPID_DPNI_START; i < 1024; i++) {
		/* Prepare to read from entry i in EPID table - EPAS reg */
		iowrite32_ccsr((uint32_t)i, &wrks_addr->epas);

		/* Read Entry Point Param (EP_PM) which contains the MC NI ID */
		j = ioread32_ccsr(&wrks_addr->ep_pm);

		pr_debug("EPID[%d].EP_PM = %d\n", i, j);

		if (j == mc_niid) {

			/* Replace MC NI ID with AIOP NI ID */
			iowrite32_ccsr(aiop_niid, &wrks_addr->ep_pm);

			/* TODO: the mc_niid field will be necessary if we decide to close the DPNI at the end of Probe. */
			/* Register MC NI ID in AIOP NI table */
			nis_table[aiop_niid].dpni_id = mc_niid;

			if ((err = dpni_open(&dprc->io, mc_niid, &dpni)) != 0) {
				pr_err("Failed to open DP-NI%d\n.", mc_niid);
				return err;
			}

			/* Save dpni regs and authentication ID in internal AIOP NI table */
			nis_table[aiop_niid].dpni_drv_params_var.dpni = dpni;

			/* Register MAC address in internal AIOP NI table */
			if ((err = dpni_get_primary_mac_addr(&dprc->io, dpni, mac_addr)) != 0) {
				pr_err("Failed to get MAC address for DP-NI%d\n", mc_niid);
				return err;
			}
			memcpy(nis_table[aiop_niid].mac_addr, mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);


			if ((err = dpni_get_attributes(&dprc->io, dpni, &attributes)) != 0) {
				pr_err("Failed to get attributes of DP-NI%d.\n", mc_niid);
				return err;
			}

#if TEST_DPBP
			/* TODO: set nis[aiop_niid].starting_hxs according to the DPNI attributes.
			 * Not yet implemented on MC. Currently always set to zero, which means ETH. */
			if ((err = dpni_set_pools(&dprc->io, dpni, &pools_params)) != 0) {
				pr_err("Failed to set the pools to DP-NI%d.\n", mc_niid);
				return err;
			}
#endif
			/* Enable DPNI before updating the entry point function (EP_PC)
			 * in order to allow DPNI's attributes to be initialized.
			 * Frames arriving before the entry point function is updated will be dropped. */
			if ((err = dpni_enable(&dprc->io, dpni)) != 0) {
				pr_err("Failed to enable DP-NI%d\n", mc_niid);
				return -ENODEV;
			}

			/* Now a Storage Profile exists and is associated with the NI */

			/* Register QDID in internal AIOP NI table */
			if ((err = dpni_get_qdid(&dprc->io, dpni, &qdid)) != 0) {
				pr_err("Failed to get QDID for DP-NI%d\n", mc_niid);
				return -ENODEV;
			}
			nis_table[aiop_niid].dpni_drv_tx_params_var.qdid = qdid;

			/* Register SPID in internal AIOP NI table */
			if ((err = dpni_get_spid(&dprc->io, dpni, &spid)) != 0) {
				pr_err("Failed to get SPID for DP-NI%d\n", mc_niid);
				return -ENODEV;
			}
			nis_table[aiop_niid].dpni_drv_params_var.spid = (uint8_t)spid; /*TODO: change to uint16_t in nis table for the next release*/

			/* Store epid index in AIOP NI's array*/
			nis_table[aiop_niid].dpni_drv_params_var.epid_idx = (uint16_t)i;


			/* TODO: need to initialize additional NI table fields according to DPNI attributes */


			ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
			ep_osc &= ORDER_MODE_CLEAR_BIT;
			/*Set concurrent mode for NI in epid table*/
			iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);

			test_num_of_nis ++;
			return 0;
		}
	}

	pr_err("DP-NI%d not found in EPID table.\n", mc_niid);
	return(-ENODEV);
}
