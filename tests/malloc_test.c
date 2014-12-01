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

#include "fsl_malloc.h"
#include "fsl_errors.h"
#include "fsl_io.h"
#include "platform.h"
#include "ls2085_aiop/fsl_platform.h"
#include "fsl_slab.h"
#include "ls2085a_aiop/platform_aiop_spec.h"
#include "aiop_common.h" /* for struct aiop_init_info */



int malloc_test();

static int allocate_check_mem(int memory_partition,
		                      uint32_t num_iterations, uint32_t size,
		                      void** allocated_pointers );
extern struct aiop_init_info g_init_data;
/* Number of malloc allocations for each partition */
#define NUM_TEST_ITER 10

int malloc_test()
{
	uint32_t num_iter = NUM_TEST_ITER, size = 0x100;
	void* allocated_pointers[NUM_TEST_ITER];
	int err = 0, local_error = 0;
	 // allocate from the heap
	if(!(local_error = allocate_check_mem(0,num_iter,size,allocated_pointers)))
	    fsl_os_print("malloc_test from the heap succeeded\n");
	err |= local_error ;
	if(!(local_error = allocate_check_mem(MEM_PART_DP_DDR,num_iter,size,
			                 allocated_pointers)))
	    fsl_os_print("malloc_test from DP-DDR succeeded\n");
	err |= local_error ;
	if(!(local_error = allocate_check_mem(MEM_PART_SH_RAM,num_iter,size,allocated_pointers)))
		fsl_os_print("malloc_test from Shared RAM succeeded\n"); 
	err |= local_error ;
	if(!(local_error = allocate_check_mem(MEM_PART_PEB,num_iter,size,allocated_pointers)))
	    fsl_os_print("malloc_test from PEB succeeded\n");
	err |= local_error ;
	if(g_init_data.app_info.sys_ddr1_size)
	{/* user has requested to allocate from system_ddr, 
	  it should be available*/ 
	    if(!(local_error = allocate_check_mem(MEM_PART_SYSTEM_DDR,num_iter,size,
                                  allocated_pointers)))	
                fsl_os_print("malloc_test from  System DDR succeeded\n");
	    err |= local_error;
	}		
	return err;
}

static int allocate_check_mem(int  memory_partition,
		                      uint32_t num_iter, uint32_t size,
		                      void **allocated_pointers)
{

	int i = 0;
	uint32_t value = 0xdeadbeef,expected_value = 0xdeadbeef;
	if(memory_partition == 0)
	{
		// allocate from the heap
		for(i = 0 ; i < num_iter; i++)
		{
			allocated_pointers[i] = fsl_os_malloc(size);
			if(NULL == allocated_pointers[i])
				return ENOMEM;
			iowrite32(value,allocated_pointers[i]);
			value = ioread32(allocated_pointers[i]);
			if(value != expected_value) {
				fsl_os_print("malloc from the heap has failed, address %x\n",
						PTR_TO_UINT(allocated_pointers[i]));
				return ENOMEM;
			}
		}
		for(i = 0 ; i < num_iter; i++)
		{
			fsl_os_free(allocated_pointers[i]);
		}

	}
	else // xmalloc case
	{
		for(i = 0 ; i < num_iter; i++)
		{
			if(MEM_PART_SH_RAM == memory_partition){
			    allocated_pointers[i] = fsl_malloc(size,4);
			}
			else{
			allocated_pointers[i] = fsl_os_xmalloc(size,memory_partition,4);
			}
			if(NULL == allocated_pointers[i])
				return ENOMEM;
			iowrite32(value,allocated_pointers[i]);
			value = ioread32(allocated_pointers[i]);
			if(value != expected_value) {
				fsl_os_print("malloc from mem part %d has failed, address %x\n",
						memory_partition,PTR_TO_UINT(allocated_pointers[i]));
				return ENOMEM;
			}
		}
		for(i = 0 ; i < num_iter; i++)
		{
			fsl_os_xfree(allocated_pointers[i]);
		}
	}
	return 0;
}
