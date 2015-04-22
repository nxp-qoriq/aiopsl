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

#ifndef __CMDIF_TEST_COMMON_H
#define __CMDIF_TEST_COMMON_H

/**************************************************************************//**
@File          cmdif_test_common.h

@Description   This file contains the common settings for all cmdif tests.
*//***************************************************************************/

#define OPEN_CMD	0x100
#define NORESP_CMD	0x101
#define ASYNC_CMD	0x102
#define SYNC_CMD 	(0x103 | CMDIF_NORESP_CMD)
#define ASYNC_N_CMD	0x104
#define OPEN_N_CMD	0x105
#define IC_TEST		0x106
#define CLOSE_CMD	0x107
#define TMAN_TEST	0x108
#define PERF_TEST_START	(TMAN_TEST | CMDIF_NORESP_CMD)
#define SHBP_TEST	0x109
#define SHBP_TEST_GPP	0x110
#define SHBP_TEST_AIOP	0x111

#define AIOP_ASYNC_CB_DONE	5  /* Must be in sync with MC ELF */
#define AIOP_SYNC_BUFF_SIZE	80 /* Must be in sync with MC ELF */


struct shbp_test {
	uint64_t shbp;
	uint8_t dpci_id;
};


#endif /* __CMDIF_TEST_COMMON_H */

