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
#include "mem_mng_util.h"
#include "fsl_mem_mng.h"



int malloc_test();

static int allocate_check_mem(int memory_partition,
		                      uint32_t num_iterations, uint32_t size,
		                      void** allocated_pointers );
static int get_mem_test();
static int check_non_valid_get_mem_partitions();
static int check_get_mem_size_alignment();
static  int check_returned_get_mem_address(uint64_t paddr, 
                                           uint64_t size,
                                           uint64_t alignment,
                                   t_mem_mng_phys_addr_alloc_info* part_info);
static int shared_ram_allocate_check_mem(uint32_t num_iter, uint32_t size,
		                      void **allocated_pointers);

extern struct aiop_init_info g_init_data;
/* Number of malloc allocations for each partition */
#define NUM_TEST_ITER 10

int malloc_test()
{
	uint32_t num_iter = NUM_TEST_ITER, size = 0x100;
	void* allocated_pointers[NUM_TEST_ITER];
	int err = 0, local_error = 0;
	/* fsl_os_malloc()/fsl_os_free() is not relevant any more, 
	 migrated to fsl_os_get_mem()/fsl_os_free()
	 */
	/*
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
	{ 
	    if(!(local_error = allocate_check_mem(MEM_PART_SYSTEM_DDR,num_iter,size,
                                  allocated_pointers)))
                fsl_os_print("malloc_test from  System DDR succeeded\n");
	    err |= local_error;
	}
	*/
	/* Check fsl_malloc() function */
	if(!(local_error = shared_ram_allocate_check_mem(num_iter,size,allocated_pointers)))
			fsl_os_print("malloc_test from  SHARED RAM succeeded\n");
	err |= local_error ;
	local_error = get_mem_test();
	err |= local_error;
	return err;
}

static int shared_ram_allocate_check_mem(uint32_t num_iter, uint32_t size,
		                      void **allocated_pointers)
{
	int i = 0;
	uint32_t value = 0xdeadbeef,expected_value = 0xdeadbeef;
	for(i = 0 ; i < num_iter; i++)
	{		
		allocated_pointers[i] = fsl_malloc(size,4);
		if(NULL == allocated_pointers[i])
			return ENOMEM;
		iowrite32(value,allocated_pointers[i]);
		value = ioread32(allocated_pointers[i]);
		if(value != expected_value) {
			fsl_os_print("fsl_malloc from  shared ram has failed, address %x\n",
					    PTR_TO_UINT(allocated_pointers[i]));
		    return ENOMEM;
		}
	}
	for(i = 0 ; i < num_iter; i++)
	{
		fsl_free(allocated_pointers[i]);
	}
	return 0;
}

/* fsl_os_malloc()/fsl_os_free() is not relevant any more, 
 migrated to fsl_os_get_mem()/fsl_os_free()
 */
#if 0
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
			allocated_pointers[i] = fsl_os_xmalloc(size,memory_partition,4);
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

#endif

static int get_mem_test()
{
	 int error = 0, local_error = 0;
	/* Check for validity of mem_partition
	For non-valid partitions get_mem() should fail.*/
	if((local_error = check_non_valid_get_mem_partitions()) != 0){
	    fsl_os_print("get_mem(): check_non_valid_get_mem_partitions  failed\n") ;
	}
	else
	    fsl_os_print("get_mem(): check_non_valid_get_mem_partitions succeeded\n") ;
	if((error = check_get_mem_size_alignment()) != 0){
		fsl_os_print("get_mem(): check_get_mem_size_alignment failed\n") ;
		local_error = 1;
	}
	else
		fsl_os_print("get_mem(): check_get_mem_size_alignment succeeded\n") ;
	error |= local_error;
	return error;
}

static int check_non_valid_get_mem_partitions()
{
    int rc = 0, local_error = 0;
    uint64_t size = 0x10, alignment = 4, paddr;
    // The following tests should fail.
    if((rc = fsl_os_get_mem(size,MEM_PART_MC_PORTALS,alignment,&paddr)) != -EINVAL)
	    local_error = 1;
    if((rc = fsl_os_get_mem(size,MEM_PART_CCSR,alignment,&paddr)) != -EINVAL)
	    local_error = 1;
    if((rc = fsl_os_get_mem(size,MEM_PART_SH_RAM,alignment,&paddr)) != -EINVAL)
	    local_error = 1;   
    if(local_error)
	    return -EINVAL;
    return 0;
}

static int check_get_mem_size_alignment()
{
	int rc = 0, local_error = 0;
	t_mem_mng_phys_addr_alloc_info sys_ddr_info = {0};
	t_mem_mng_phys_addr_alloc_info peb_info = {0};
	t_mem_mng_phys_addr_alloc_info dp_ddr_info = {0};

	uint64_t size = 1*KILOBYTE,  paddr = 0;
	uint64_t alignment =  size;

	if((rc = sys_get_phys_addr_alloc_partition_info(MEM_PART_DP_DDR,
													&dp_ddr_info)) != 0)
		return rc;
	if((rc = sys_get_phys_addr_alloc_partition_info(MEM_PART_SYSTEM_DDR,
													&sys_ddr_info)) != 0)
		return rc;
	if((rc = sys_get_phys_addr_alloc_partition_info(MEM_PART_PEB,
													&peb_info)) != 0)
		return rc;
	
	/* test get_mem() for MEM_PART_DP_DDR */
	if((local_error = fsl_os_get_mem(size,MEM_PART_DP_DDR,alignment,&paddr)) != 0){
		fsl_os_print("get_mem(): fsl_os_get_mem from MEM_PART_DP_DDR failed\n") ; 
	}
	rc |= local_error;
	rc |= check_returned_get_mem_address(paddr,size,alignment,&dp_ddr_info);
	fsl_os_put_mem(paddr);

    /* test get_mem() for MEM_PART_SYSTEM_DDR */
    if(g_init_data.app_info.sys_ddr1_size){
	    if((local_error = fsl_os_get_mem(size,MEM_PART_SYSTEM_DDR,alignment,&paddr)) != 0){
		    fsl_os_print("get_mem(): fsl_os_get_mem from MEM_PART_SYSTEM_DDR failed\n") ;
	    }
	    rc |= local_error;
	    rc |= check_returned_get_mem_address(paddr,size,alignment,&sys_ddr_info);
	    fsl_os_put_mem(paddr);
    }

	/* test get_mem() for MEM_PART_PEB */
	if((local_error=fsl_os_get_mem(size,MEM_PART_PEB,alignment,&paddr)) != 0){
		fsl_os_print("get_mem(): fsl_os_get_mem from MEM_PART_PEB failed\n") ;
	}
	rc |= local_error;
	rc |= check_returned_get_mem_address(paddr,size,alignment,&peb_info);
	fsl_os_put_mem(paddr);

	return rc;
}

static  int check_returned_get_mem_address(uint64_t paddr,
                                           uint64_t size,
                                           uint64_t alignment,
                                   t_mem_mng_phys_addr_alloc_info* part_info)
{
    int error = 0;
    // check if returned address is in boundaries;
    error = (paddr < part_info->base_paddress) || 
	    (paddr > part_info->base_paddress + part_info->size - size);
    // check alignment;
    error |=  (int)(paddr & (uint64_t)(alignment-1));
    return error;
}
