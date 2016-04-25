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

#include "fsl_spinlock.h"
#include "fsl_smp.h"
#include "fsl_errors.h"

void * memset(void * dst, int val, size_t n);

#define SPIN_TEST_ITERATIONS 600

struct spinlock_test_unit {
	uint32_t core_id; //0-none, 1-core#0, 2-core#1
	uint32_t test_id; //index in array
	uint32_t core_flag[TEST__TOTAL_CORE_NO]; //1 if core i got spinlock;
};

static struct spinlock_test_unit stest[SPIN_TEST_ITERATIONS];
static uint32_t s_index;
static uint32_t core_count;
static uint8_t test_lock;

static uint32_t num_taken[TEST__TOTAL_CORE_NO] = {0};

/*****************************************************************************/
int spinlock_standalone_init();
int spinlock_standalone_init()
{
	if (sys_is_master_core()) {
		s_index = 0;
		core_count = 0;
		test_lock = 0;
		memset(&stest[0], 0, SPIN_TEST_ITERATIONS * sizeof(struct spinlock_test_unit));
	}
	sys_barrier();
	return 0;
}

/*****************************************************************************/
int spinlock_test();
int spinlock_test()
{
	uint32_t i = 0, j = 0;
	volatile uint32_t* core_count_ptr = &core_count;
	volatile uint32_t* s_index_ptr = &s_index;
	uint32_t total_iter = 0;

	/* start the test */
	while (*s_index_ptr < SPIN_TEST_ITERATIONS)
	{
		lock_spinlock(&test_lock);
		(*core_count_ptr)++;

		if (*core_count_ptr != 1) {
			unlock_spinlock(&test_lock);
			return -EEXIST;
		}
		if (core_get_id() >= TEST__TOTAL_CORE_NO) {
			unlock_spinlock(&test_lock);
			return -EEXIST;
		}
		if (*s_index_ptr < SPIN_TEST_ITERATIONS) {
			stest[*s_index_ptr].test_id = *s_index_ptr;
			stest[*s_index_ptr].core_id = core_get_id() + 1;
			stest[*s_index_ptr].core_flag[core_get_id()] += 1;
			(*s_index_ptr)++;
		}

		(*core_count_ptr)--;
		unlock_spinlock(&test_lock);
	}
	sys_barrier();

	for (i = 0; i < SPIN_TEST_ITERATIONS; i++) {
		if (stest[i].test_id != i)
			return -EEXIST;
		if (stest[i].core_id > TEST__TOTAL_CORE_NO)
			return -EEXIST;
		if (stest[i].core_flag[stest[i].core_id - 1] != 1)
			return -EEXIST;

		for (j = 0; j < TEST__TOTAL_CORE_NO; j++) {
			if (j != (stest[i].core_id - 1)) {
				if (stest[i].core_flag[j] != 0)
					return -EEXIST;
			}
		}
	}

	if (sys_is_master_core()) {
		for (i = 0; i < SPIN_TEST_ITERATIONS; i++) {
			num_taken[stest[i].core_id - 1]++;
		}
	}
	sys_barrier();

	for (j = 0; j < TEST__TOTAL_CORE_NO; j++) {
		total_iter += num_taken[j];
	}
	if (total_iter != SPIN_TEST_ITERATIONS) {
		return -EEXIST;
	}
	return 0;
}
