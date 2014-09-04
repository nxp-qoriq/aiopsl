#include "common/types.h"
#include "inc/fsl_gen.h"
/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_errors.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_fdma.h"
#include "dma.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "fsl_dma.h"

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
