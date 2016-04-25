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

__HOT_CODE static int func_in_iram()
{
	int err = 0;

	err = -1;
	return err;
}

static int recursion_func(int i)
{
	int arr[0x1000];
	int j = i % 0x1000;

	arr[0] = i + 1;
	arr[1] = i + 2;
	arr[0xfff] = i + 3;
	arr[j] = i + 1;

	if (i == 1000000)
			return 0;

	return recursion_func(arr[j]);
}

int stack_ovf_test();
int stack_ovf_test()
{
	recursion_func(1);
	return -EINVAL;
}

static int exceptions_test_()
{
	int err = 0;
	uint8_t *iram_ptr = (uint8_t *)((void *)func_in_iram);

	/* Write to IRAM */
	iram_ptr[0] = 0xff;
	iram_ptr[1] = 0xff;
	if ((iram_ptr[1] != 0xff) || (iram_ptr[1] != 0xff))
		return -EINVAL;

	/* Trigger exception */
	err = func_in_iram();

	return -EINVAL;
}

int exceptions_test();
int exceptions_test()
{
	void (*f1_ptr)() = (void (*)())(0xffffffff);

	(*f1_ptr)();
	return -EINVAL;
}
