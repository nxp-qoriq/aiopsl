
#include "../../../tests/bringup/buffer_pool_test.h"
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
#include "../../../tests/bringup/bringup_tests.h"

int buffer_pool_init();
int buffer_pool_test();
extern int dpbp_init();

extern struct mc_dprc g_mc_dprc;
extern struct icontext icontext_aiop;
extern int dpbp_init();

int buffer_pool_init()
{
	return dpbp_init();
}


/*****************************************************************************/
int buffer_pool_test()
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
	uint32_t buff_size = 2048;
	dma_addr_t addr  = 0x6002000000;/*DP-DDR phys address*/

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
				fdma_release_buffer(ic.icid, ic.bdi_flags, attr.bpid, addr);
				addr += buff_size;
			}

			err = test_buffer(attr.bpid);
			if(err) return err;


		}
	}
	pr_info("AIOP: Test passed\n", core_get_id());
	return 0;
}

int test_buffer(uint16_t dpbp_id)
{
	uint64_t buff = 0;
	int err;
	err = cdma_acquire_context_memory( dpbp_id,
		&buff);
	if(err) {
		pr_err("cdma_acquire_context_memory Failed: %d\n",err);
		return err;
	}
	else
	{
		fsl_os_print("Acquired memory from bpid: %d\n",dpbp_id);
	}
	cdma_refcount_increment(buff);
	cdma_refcount_decrement(buff);
	cdma_release_context_memory(buff);
	return 0;
}
