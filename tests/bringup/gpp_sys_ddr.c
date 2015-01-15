#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_icontext.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_io_ccsr.h"
#include "aiop_common.h"

#define TEST_DDR_SIZE	64
#define TEST_DDR_VAL	0xbe

struct icontext ic_aiop = {0};

int gpp_ddr_check(struct icontext *ic, uint64_t iova, uint16_t size);
int gpp_sys_ddr_init();
int gpp_sys_ddr_test(uint64_t iova, uint16_t size);

int gpp_sys_ddr_init()
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

	if (!ic_aiop.bdi_flags) return 1;
	if (!ic_aiop.dma_flags) return 2;

	return 0;
}

int gpp_ddr_check(struct icontext *ic, uint64_t iova, uint16_t size)
{
	int i = 0;
	uint8_t  buf[TEST_DDR_SIZE];

	if ((size > TEST_DDR_SIZE) || (size == 0))
		return 4;

	/* Clean */
	for (i = 0; i < size; i++) {
		buf[i] = 0;
	}
	icontext_dma_write(ic, size, &buf[0], iova);

	/* Write */
	for (i = 0; i < size; i++) {
		buf[i] = TEST_DDR_VAL;
	}
	icontext_dma_write(ic, size, &buf[0], iova);

	/* Check */
	for (i = 0; i < size; i++) {
		buf[i] = 0;
	}
	icontext_dma_read(ic, size, iova, &buf[0]);
	for (i = 0; i < size; i++) {
		if (buf[i] != TEST_DDR_VAL)
			return 8;
	}

	return 0;
}

int gpp_sys_ddr_test(uint64_t iova, uint16_t size)
{
	int err = 0;
	struct icontext ic;

	/* Each SW context has different icid:
	 * icid 0 - goes to MC
	 * icid 1 and up - goes to AIOP and GPP */
	ic.icid = (ic_aiop.icid == 1 ? 2 : 1);
	ic.dma_flags = 0; /* Should not change */
	ic.bdi_flags = 0; /* Should not change */

	/* Virtual addr of GPP may change */
	err |= gpp_ddr_check(&ic, iova, size);
	err |= gpp_ddr_check(&ic, iova, size);

	return err;
}
