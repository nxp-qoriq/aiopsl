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

#include "fsl_io.h"
#include "fsl_errors.h"
#include "fsl_malloc.h"
#include "fsl_platform.h"
#include "fsl_smp.h"
#include "fsl_dbg.h"


#define MEM_TEST_VAL 		(0x12345678)
#define MEM_TEST_STUB_VAL	(0x24685791)

#define TEST_FLG_READ  0x1
#define TEST_FLG_WRITE 0x2

static void memInit(const uint32_t param);
int mem_standalone_init();
int mem_test();

typedef struct {
	e_memory_partition_id mem_pid;
	volatile void *vaddr;
	void (*memInit)(uint32_t param);	/* Init function, in case a window needs to be configured  */
	uint32_t init_param;				/* parameter to pass to memInit() */
	uint32_t flags;
} mem_access_test_t;

static mem_access_test_t mem_tests[] = {
	/*						Addr					memInit 	param	flags */
	{MEM_PART_DP_DDR, 		(volatile void *)NULL,	memInit,	0,		TEST_FLG_READ | TEST_FLG_WRITE},
	{MEM_PART_SYSTEM_DDR,	(volatile void *)NULL,	memInit,	0,		TEST_FLG_READ | TEST_FLG_WRITE},
	{MEM_PART_SH_RAM, 		(volatile void *)NULL,	memInit,	0,		TEST_FLG_READ | TEST_FLG_WRITE},
	{MEM_PART_PEB, 			(volatile void *)NULL,	memInit,	0,		TEST_FLG_READ | TEST_FLG_WRITE},
	{MEM_PART_LAST, 		(volatile void *)NULL,	NULL, 		0,		0} /* (end of list) */
};

/*****************************************************************************/
static void memInit(const uint32_t param)
{
	UNUSED(param);
	// TODO
}

/*****************************************************************************/
static struct platform_memory_info g_mem_info[] =
	{{MEM_PART_DP_DDR}, {MEM_PART_SYSTEM_DDR}, {MEM_PART_SH_RAM}, {MEM_PART_PEB}};

void __get_mem_partitions_addr(int size, struct platform_memory_info* mem_info);
int mem_standalone_init()
{
	int i = 0, j = 0;

	if (sys_is_master_core()) {
		__get_mem_partitions_addr(ARRAY_SIZE(g_mem_info), g_mem_info);

		for (; mem_tests[i].mem_pid != MEM_PART_LAST; i++) {
			for (int j = 0; j < ARRAY_SIZE(g_mem_info); j++) {
				if ((mem_tests[i].mem_pid == g_mem_info[j].mem_partition_id) &&
					(g_mem_info[j].size > 0)) {
					mem_tests[i].vaddr = (volatile void *)g_mem_info[j].virt_base_addr;
					break;
				}
			}
		}
	}
	sys_barrier();
	return 0;
}

/*****************************************************************************/
static int test_access(const mem_access_test_t *test) 
{	
	volatile void *addr = test->vaddr;
	int read = (int)(test->flags & TEST_FLG_READ);
	int write = (int)(test->flags & TEST_FLG_WRITE);
	volatile uint32_t read_val = MEM_TEST_STUB_VAL;

	if (test->memInit)
		test->memInit(test->init_param);

	if (write)
		iowrite32(MEM_TEST_VAL, addr);

	if (read) {
		read_val = ioread32(addr);

		if (read_val == MEM_TEST_STUB_VAL)
			return -EACCES;
	}
	
	if (read && write) {
		if (read_val == MEM_TEST_VAL) {
			return 0;
		} else {
			return -EACCES;
		}
	}

	return 0;
}

/*****************************************************************************/
static volatile uint32_t g_test_sync_flag = 0;
int mem_test()
{
	int err = -EINVAL;
	int i = 0;
	uint32_t core_id = core_get_id();

	// Busy waiting for its turn so that we can reuse the same addresses
	while (core_id != g_test_sync_flag) {
	}

	while (mem_tests[i].mem_pid != MEM_PART_LAST) {
		if (mem_tests[i].vaddr != NULL) {
			err = test_access(&mem_tests[i]);
			if (err) {
				pr_info("Mem_test failed on step %d\n", i);
				g_test_sync_flag++;
				return err;
			}
		}
		i++;
	}
	g_test_sync_flag++;
	return err;
}
