/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __APPS_ARCH_H
#define __APPS_ARCH_H

#define ARCH_INIT_TASKS_PER_CORE_SIZE	4

#define ARCH_DP_DDR_SIZE				(0)
#define ARCH_CTLU_DP_DDR_NUM_ENTRIES	(0)
#define ARCH_MFLU_DP_DDR_NUM_ENTRIES	(0)

#define ARCH_MEM_PEB_SIZE				(512 * KILOBYTE)
#define ARCH_MEM_SYS_DDR1_SIZE			(32 * MEGABYTE)

#define ARCH_DPPNI_BUF_SIZE				512
#define ARCH_DPNI_BUFS_IN_POOL_NUM		1024

/* fdma_discard_fd API is different for rev1 and rev2 */
#define ARCH_FDMA_DISCARD_FD() \
	fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS, 0, FDMA_DIS_AS_BIT)

#endif /* __APPS_ARCH_H */
