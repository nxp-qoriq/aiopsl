#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_fdma.h"
#include "dma.h"
#include "sys.h"
#include "fsl_malloc.h"
#include "fsl_dbg.h"

int dma_get_icontext(uint16_t icid, void **icontext)
{
	ASSERT_COND(icontext != NULL);

	/* find in icid table */
	/* copy pointer from icid table */
	*icontext = NULL;
	return 0;
}

int dma_read(void *icontext, uint16_t size, uint64_t src, void *dest)
{
	ASSERT_COND(dest != NULL);
	ASSERT_COND(src != NULL);

	fdma_dma_data(size,
	              ((struct dma_icontext *)icontext)->icid,
	              dest,
	              src,
	              ((struct dma_icontext *)icontext)->read_flags);

	return 0;
}

int dma_write(void *icontext, uint16_t size, void *src, uint64_t dest)
{
	ASSERT_COND(src != NULL);
	ASSERT_COND(dest != NULL);

	fdma_dma_data(size,
	              ((struct dma_icontext *)icontext)->icid,
	              src,
	              dest,
	              ((struct dma_icontext *)icontext)->write_flags);
	return 0;
}
