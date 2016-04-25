/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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
#include "fsl_types.h"
#include "fsl_icontext.h"
#include "icontext.h"

#define TEST_DDR_SIZE	64
#define TEST_DDR_VAL	0xbe

extern struct icontext icontext_aiop;

static int gpp_ddr_check(struct icontext *ic, uint64_t iova, uint16_t size);
static int gpp_sys_ddr_test_(uint64_t iova, uint16_t size);

int gpp_sys_ddr_init();
int gpp_sys_ddr_test();

int gpp_sys_ddr_init()
{
	// sys_init()
	return icontext_init();
}

static int gpp_ddr_check(struct icontext *ic, uint64_t iova, uint16_t size)
{
	int i = 0;
	uint8_t buf[TEST_DDR_SIZE];

	if ((size > TEST_DDR_SIZE) || (size == 0))
		return -EINVAL;

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
			return -EACCES;
	}
	return 0;
}

static int gpp_sys_ddr_test_(uint64_t iova, uint16_t size)
{
	int err = 0;
	struct icontext ic;

	/* Each SW context has different icid:
	 * icid 0 - goes to MC
	 * icid 1 and up - goes to AIOP and GPP */
	ic.icid = (icontext_aiop.icid == 1 ? 2 : 1);
	ic.dma_flags = 0; /* Should not change */
	ic.bdi_flags = 0; /* Should not change */

	/* Virtual addr of GPP may change */
	err = gpp_ddr_check(&ic, iova, size);
	if (err) {
		return err;
	}
	return gpp_ddr_check(&ic, iova, size);
}

#define SYSTEM_DDR_PHY_ADDR 0x83d3000000

/* change the address if needed
 * ICID is set inside to 1 or 2 */
int gpp_sys_ddr_test()
{
	int err = 0;

#ifdef EXPERIMENTAL
	err = gpp_sys_ddr_test_(0x80000000, 16);
	if (err) {
		return err;
	}

	err = gpp_sys_ddr_test_(0xE0000000, 32);
	if (err) {
		return err;
	}

	err = gpp_sys_ddr_test_(0x2000000400, 16);
	if (err) {
		return err;
	}

	err = gpp_sys_ddr_test_(0x2000000300, 32);
	if (err) {
		return err;
	}
#endif

	err = gpp_sys_ddr_test_(SYSTEM_DDR_PHY_ADDR, 32);
	if (err) {
		return err;
	}
	return err;
}
