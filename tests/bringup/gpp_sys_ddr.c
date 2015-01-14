#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "bringup_tests.h"
#include "fsl_soc.h"
#include "fsl_icontext.h"

#if (TEST_GPP_SYS_DDR == ON)

#define TEST_DDR_SIZE	10
#define TEST_DDR_VAL	0xbe

struct icontext ic_aiop = {0};

static int aiop_ic_init()
{
	uint32_t cdma_cfg;
	struct aiop_tile_regs *ccsr = \
		(struct aiop_tile_regs *)
		(AIOP_PERIPHERALS_OFF +
		 SOC_PERIPH_OFF_AIOP_TILE +
		 SOC_PERIPH_OFF_AIOP_CMGW);

	cdma_cfg = ioread32_ccsr(&ccsr->cdma_regs.cfg);

	ic_aiop.icid = (uint16_t)(cdma_cfg & CDMA_ICID_MASK);

	ic_aiop.bdi_flags = 0;
	if (cdma_cfg & CDMA_BDI_BIT)
		ic_aiop.bdi_flags |= FDMA_ENF_BDI_BIT;

	ic_aiop.dma_flags = 0;
	if (cdma_cfg & CDMA_BMT_BIT)
		ic_aiop.dma_flags |= FDMA_DMA_BMT_BIT;
	if (cdma_cfg & CDMA_PL_BIT)
		ic_aiop.dma_flags |= FDMA_DMA_PL_BIT;
	if (cdma_cfg & CDMA_VA_BIT)
		ic_aiop.dma_flags |= FDMA_DMA_eVA_BIT;

	if (!ic_aiop.bdi_flags) return -EINVAL;
	if (!ic_aiop.dma_flags) return -EINVAL;

	return 0;
}

static int ddr_check(struct icontext *ic, uint64_t iova)
{
	int i = 0;
	uint8_t  buff[TEST_DDR_SIZE];

	/* Write */
	for (i = 0; i < TEST_DDR_SIZE; i++) {
		buf[i] = TEST_DDR_VAL;
	}
	icontext_dma_write(ic, TEST_DDR_SIZE, &buf[0], iova);

	/* Clean */
	for (i = 0; i < TEST_DDR_SIZE; i++) {
		buf[i] = 0;
	}

	/* Check */
	icontext_dma_read(ic, TEST_DDR_SIZE, iova, &buf[0]);
	for (i = 0; i < TEST_DDR_SIZE; i++) {
		if (buf[i] != TEST_DDR_VAL)
			return -EINVAL;
	}
}

int gpp_sys_ddr_test()
{
	int err = 0;
	struct icontext ic;

	er = aiop_ic_init();
	if (err) return err;


	/* Each SW context has different icid:
	 * icid 0 - goes to MC
	 * icid 1 and up - goes to AIOP and GPP */
	ic.icid = (ic_aiop.icid == 1 ? 2 : 1);
	ic.dma_flags = 0; /* Should not change */
	ic.bdi_flags = 0; /* Should not change */

	/* Virtual addr of GPP may change */
	err |= ddr_check(&ic, 0x2000000400);
	err |= ddr_check(&ic, 0x2000000100);
	err |= ddr_check(&ic, 0x2000000200);
	err |= ddr_check(&ic, 0x2000000300);
	return err;
}

#endif /* (TEST_GPP_SYS_DDR == ON) */
