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

#include "fsl_malloc.h"
#include "fsl_errors.h"
#include "fsl_stdio.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "fsl_malloc.h"
#include "fsl_slab.h"
#include "fsl_aiop_common.h" /* for struct aiop_init_info */
#include "fsl_mem_mng.h"



int malloc_test();
int slob_check_free_lists_after_free(e_memory_partition_id mem_part);

static int allocate_check_mem(int memory_partition,
		                      uint32_t num_iterations, uint32_t size,
		                      void** allocated_pointers );
static int get_mem_test();
static int check_non_valid_get_mem_partitions();
static  int check_returned_get_mem_address(uint64_t paddr,
                                           uint64_t size,
                                           uint64_t alignment,
                                   t_mem_mng_phys_addr_alloc_info* part_info);
static int shared_ram_allocate_check_mem(uint32_t num_iter, uint32_t size,
		                      void **allocated_pointers);

static int mem_depletion_test(e_memory_partition_id mem_partition,
                              const uint32_t max_alloc_size,
                              const uint32_t max_alignment_size);

static  int check_returned_address(uint64_t paddr,
                                           uint64_t size,
                                           uint64_t alignment,
                                           uint32_t mem_partition);

static  int check_returned_malloc_address(uint32_t vaddr,
                                           uint32_t size,
                                           uint32_t alignment,
                                           t_mem_mng_partition_info* part_info);

static t_mem_mng_partition_info sh_ram_info = {0};
static t_mem_mng_phys_addr_alloc_info sys_ddr_info = {0};
static t_mem_mng_phys_addr_alloc_info peb_info = {0};
static t_mem_mng_phys_addr_alloc_info dp_ddr_info = {0};

static uint32_t s_dp_ddr_mem_exists = 0;
static uint32_t s_system_ddr_mem_exists = 0;
static uint32_t s_peb_mem_exists = 0;
static uint32_t s_shared_ram_eixsts = 0;




extern struct aiop_init_info g_init_data;
/* Number of malloc allocations for each partition */
#define NUM_TEST_ITER 10
#define MAX_DEPLETION_ITER 25
/* Locks per each memory partition to let only one task perform depletion test */
static uint8_t s_shared_ram_lock = 0;
static uint8_t s_dp_ddr_lock = 0;
static uint8_t s_system_ddr_lock = 0;
static uint8_t s_peb_lock = 0;



static int check_get_mem_size_alignment(uint64_t size,uint64_t alignment)
{
	int rc = 0, local_error = 0;
	uint64_t paddr = 0;

	/* test get_mem() for MEM_PART_DP_DDR */
	local_error = fsl_get_mem(size,MEM_PART_DP_DDR,alignment,&paddr);
	if((local_error !=0 && s_dp_ddr_mem_exists) ||
		(local_error == 0 && !s_dp_ddr_mem_exists)	)
	{
		fsl_print("get_mem(): fsl_get_mem from MEM_PART_DP_DDR failed\n") ;
		rc |= local_error;
	}
	else
	{
		rc |= check_returned_get_mem_address(paddr,size,alignment,&dp_ddr_info);
		fsl_put_mem(paddr);
	}

    /* test get_mem() for MEM_PART_SYSTEM_DDR */
   local_error = fsl_get_mem(size,MEM_PART_SYSTEM_DDR,alignment,&paddr);
   if((local_error !=0 && s_system_ddr_mem_exists) ||
	 (local_error == 0 && !s_system_ddr_mem_exists))
   {
		fsl_print("get_mem(): fsl_get_mem from MEM_PART_SYSTEM_DDR failed\n") ;
		rc |= local_error;
   }
   else
   {
	   rc |= check_returned_get_mem_address(paddr,size,alignment,&sys_ddr_info);
	   fsl_put_mem(paddr);
   }

	/* test get_mem() for MEM_PART_PEB */

	local_error=fsl_get_mem(size,MEM_PART_PEB,alignment,&paddr);
	if((local_error !=0 && s_peb_mem_exists) ||
	   (local_error == 0 && !s_peb_mem_exists))
	{
		fsl_print("get_mem(): fsl_get_mem from MEM_PART_PEB failed\n") ;
		rc |= local_error;
	}
	else
	{
		rc |= check_returned_get_mem_address(paddr,size,alignment,&peb_info);
		fsl_put_mem(paddr);
    }
	return rc;
}

int malloc_test()
{
	uint32_t num_iter = NUM_TEST_ITER, size = 0x100;
	void* allocated_pointers[NUM_TEST_ITER];

	int err = 0, local_error = 0, rc = -1;

	s_dp_ddr_mem_exists = (uint32_t)fsl_mem_exists(MEM_PART_DP_DDR);
	s_system_ddr_mem_exists = (uint32_t)fsl_mem_exists(MEM_PART_SYSTEM_DDR);
	s_peb_mem_exists = (uint32_t)fsl_mem_exists(MEM_PART_PEB);
	s_shared_ram_eixsts = (uint32_t)fsl_mem_exists(MEM_PART_SH_RAM);


	rc = sys_get_phys_addr_alloc_partition_info(MEM_PART_DP_DDR,&dp_ddr_info);
	if((rc != 0 && s_dp_ddr_mem_exists) ||
	   (rc == 0 && !s_dp_ddr_mem_exists))
	{
		fsl_print("sys_get_phys_addr_alloc_partition_info for MEM_PART_DP_DDR failed\n");
		return rc;
	}

	rc = sys_get_phys_addr_alloc_partition_info(MEM_PART_SYSTEM_DDR,&sys_ddr_info);
	if((rc != 0 && s_system_ddr_mem_exists) ||
	   (rc == 0 && !s_system_ddr_mem_exists))
	{
		fsl_print("sys_get_phys_addr_alloc_partition_info for MEM_PART_SYSTEM_DDR failed\n");
		return rc;
	}



	rc = sys_get_phys_addr_alloc_partition_info(MEM_PART_PEB,&peb_info);
	if((rc != 0 && s_peb_mem_exists) ||
	   (rc == 0 && !s_peb_mem_exists))
	{
		fsl_print("sys_get_phys_addr_alloc_partition_info for MEM_PART_PEB failed\n");
		return rc;
	}

	rc = sys_get_mem_partition_info(MEM_PART_SH_RAM,&sh_ram_info);
	if((rc != 0 && s_shared_ram_eixsts) ||
	   (rc == 0 && !s_shared_ram_eixsts))
	{
		fsl_print("sys_get_mem_partition_info for MEM_PART_SH_RAM failed\n");
		return rc;
	}


	local_error = get_mem_test();
	err |= local_error;

	/* Check depletion through fsl_malloc() function */
	cdma_mutex_lock_take((uint64_t)&s_shared_ram_lock,CDMA_MUTEX_WRITE_LOCK);
	local_error = shared_ram_allocate_check_mem(num_iter,size,allocated_pointers);
	cdma_mutex_lock_release((uint64_t)&s_shared_ram_lock);
	if((local_error != 0 && s_shared_ram_eixsts) ||
	   (local_error == 0 && !s_shared_ram_eixsts))
	{
		fsl_print("shared_ram_allocate_check_mem from  SHARED RAM failed\n");
		err |= local_error ;
	}
	else
		fsl_print("shared_ram_allocate_check_mem from  SHARED RAM succeeded\n");



	cdma_mutex_lock_take((uint64_t)&s_peb_lock,CDMA_MUTEX_WRITE_LOCK);
	local_error = mem_depletion_test(MEM_PART_PEB,4*MEGABYTE,MEGABYTE);
	cdma_mutex_lock_release((uint64_t)&s_peb_lock);
	if((local_error != 0 && s_peb_mem_exists) ||
	   (local_error == 0 && !s_peb_mem_exists))
	{
		fsl_print("mem_depletion_test from  PEB failed\n");
		err |= local_error;
	}
	else
		fsl_print("mem_depletion_test from  PEB succeeded\n");



	cdma_mutex_lock_take((uint64_t)&s_system_ddr_lock,CDMA_MUTEX_WRITE_LOCK);
	local_error = mem_depletion_test(MEM_PART_SYSTEM_DDR,4*MEGABYTE,MEGABYTE);
	cdma_mutex_lock_release((uint64_t)&s_system_ddr_lock);
	if((local_error != 0 && s_system_ddr_mem_exists) ||
	   (local_error == 0 && !s_system_ddr_mem_exists))
	{
		fsl_print("mem_depletion_test from  SYSTEM_DDR  failed\n");
		err |= local_error;
	}
	else
		fsl_print("mem_depletion_test from  SYSTEM_DDR succeeded\n");



	cdma_mutex_lock_take((uint64_t)&s_dp_ddr_lock,CDMA_MUTEX_WRITE_LOCK);
	local_error = mem_depletion_test(MEM_PART_DP_DDR,4*MEGABYTE,4*MEGABYTE);
	cdma_mutex_lock_release((uint64_t)&s_dp_ddr_lock);
	if((local_error != 0 && s_dp_ddr_mem_exists) ||
	   (local_error == 0 && !s_dp_ddr_mem_exists))
	{
		fsl_print("mem_depletion_test from  DP_DDR  failed\n");
		err |= local_error;
	}
	else
		fsl_print("mem_depletion_test from  DP_DDR succeeded\n");

	cdma_mutex_lock_take((uint64_t)&s_shared_ram_lock,CDMA_MUTEX_WRITE_LOCK);
	local_error = mem_depletion_test(MEM_PART_SH_RAM,MEGABYTE,MEGABYTE);
	cdma_mutex_lock_release((uint64_t)&s_shared_ram_lock);
	if((local_error != 0 && s_shared_ram_eixsts) ||
	   (local_error == 0 && !s_shared_ram_eixsts))
	{
		fsl_print("mem_depletion_test from  SHARED RAM  failed\n");
		err |= local_error;
	}
	else
		fsl_print("mem_depletion_test from  SHARED RAM succeeded\n");

#ifdef SL_DEBUG
	virt_mem_partition_dump(MEM_PART_SH_RAM);
#endif

	return err;
}

static int shared_ram_allocate_check_mem(uint32_t num_iter, uint32_t size,
		                      void **allocated_pointers)
{
	int i = 0;
	uint32_t alignment = 16;
	uint32_t value = 0xdeadbeef,expected_value = 0xdeadbeef;
	for(i = 0 ; i < num_iter; i++)
	{
		allocated_pointers[i] = fsl_malloc(size,alignment);
		if(NULL == allocated_pointers[i])
		{
		    /*fsl_print("fsl_malloc() from shared ram  failed\n");*/
		    return -ENOMEM;
		}
		if(check_returned_malloc_address((uint32_t)allocated_pointers[i],
		                                 size,
		                                 alignment,
		                                 &sh_ram_info)){
		   /*fsl_print("fsl_malloc from  shared ram failed, address %x\n",
                                 PTR_TO_UINT(allocated_pointers[i]));*/
		   return -EFAULT;
		}
		iowrite32(value,allocated_pointers[i]);
		value = ioread32(allocated_pointers[i]);
		if(value != expected_value) {
			/*fsl_print("fsl_malloc from  shared ram failed, address %x\n",
					    PTR_TO_UINT(allocated_pointers[i]));*/
		    return -ENOMEM;
		}
	}
	for(i = 0 ; i < num_iter; i++)
	{
		fsl_free(allocated_pointers[i]);
	}
	return 0;
}

/* fsl_malloc()/fsl_free() is not relevant any more,
 migrated to fsl_get_mem()/fsl_free()
 */

static int get_mem_test()
{
	 int error = 0, local_error = 0;
	/* Check for validity of mem_partition
	For non-valid partitions get_mem() should fail.*/
	if((local_error = check_non_valid_get_mem_partitions()) != 0){
	    fsl_print("get_mem(): check_non_valid_get_mem_partitions  failed\n") ;
	}
	else
	    fsl_print("get_mem(): check_non_valid_get_mem_partitions succeeded\n") ;
	error |= local_error;
	if((local_error = check_get_mem_size_alignment(1*KILOBYTE,1*KILOBYTE)) != 0){
		fsl_print("get_mem(): check_get_mem_size_alignment failed\n") ;
	}
	else
		fsl_print("get_mem(): check_get_mem_size_alignment succeeded\n") ;
	/*
	if((local_error = check_get_mem_size_alignment(0,1*KILOBYTE)) != 0){
		fsl_print("get_mem(): check_get_mem_size_alignment failed\n") ;
	}
	else
		fsl_print("get_mem(): check_get_mem_size_alignment succeeded\n") ;
	*/
	if((local_error = check_get_mem_size_alignment(1*KILOBYTE,0)) != 0){
	    fsl_print("get_mem(): check_get_mem_size_alignment failed\n") ;
	}
	else
	    fsl_print("get_mem(): check_get_mem_size_alignment succeeded\n") ;
	error |= local_error;
	return error;
}

static int check_non_valid_get_mem_partitions()
{
    int rc = 0, local_error = 0;
    uint64_t size = 0x10, alignment = 4, paddr;
    // The following tests should fail.
    if((rc = fsl_get_mem(size,MEM_PART_MC_PORTALS,alignment,&paddr)) != -EINVAL)
	    local_error = 1;
    if((rc = fsl_get_mem(size,MEM_PART_CCSR,alignment,&paddr)) != -EINVAL)
	    local_error = 1;
    if((rc = fsl_get_mem(size,MEM_PART_SH_RAM,alignment,&paddr)) != -EINVAL)
	    local_error = 1;
    if(local_error)
	    return -EINVAL;
    return 0;
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
    if(alignment >= 1)
        error |=  (int)(paddr & (uint64_t)(alignment-1));
    return error;
}

static  int check_returned_malloc_address(uint32_t vaddr,
                                           uint32_t size,
                                           uint32_t alignment,
                                           t_mem_mng_partition_info* part_info)
{
    int error = 0;
    // check if returned address is in boundaries;
    error = (vaddr < part_info->base_address) ||
	    (vaddr > part_info->base_address + part_info->size - size);
    // check alignment;
    if(alignment >= 1)
        error |=  (int)(vaddr & (uint64_t)(alignment-1));
    return error;
}

static int mem_depletion_test(e_memory_partition_id mem_partition,
                              const uint32_t max_alloc_size,
                              const uint32_t max_alignment_size)
{
	uint32_t size =  0x10;
	uint32_t prev_size =  0x10;
	uint32_t alignment = size;
	uint32_t prev_alignment = alignment ;
	uint32_t count = 0;
	void *prev_addr = NULL,*curr_addr = NULL;
	uint64_t prev_addr_64 = 0, curr_addr_64 = 0;
	void* shram_addresses[MAX_DEPLETION_ITER] = {0};
	uint64_t adresses[MAX_DEPLETION_ITER] = {0};
	if(mem_partition == MEM_PART_SH_RAM)
	{
		while((curr_addr = fsl_malloc(size,alignment)) != NULL)
		{
			shram_addresses[count++] = curr_addr;
			if(check_returned_address((uint64_t)curr_addr,size,alignment,mem_partition) != 0)
			    return -1;
			prev_addr = curr_addr;
			prev_size = size;
			prev_alignment = alignment;
			size = size << 1;
			alignment = (size > max_alignment_size)? max_alignment_size : size;
			if(0 != max_alloc_size && size > max_alloc_size)
				break;
		}
		fsl_free(prev_addr);
		if((curr_addr = fsl_malloc(prev_size,prev_alignment)) == NULL)
		{
			/*fsl_print("mem_depletion_test from  SHARED RAM failed\n");*/
			return -ENAVAIL;
		}
		if(check_returned_address((uint64_t)curr_addr,prev_size,prev_alignment,mem_partition) != 0)
		    return -1;
		fsl_free(curr_addr);
		for(int i = 0 ; i < count-1 ; i++)
		{
			fsl_free(shram_addresses[i]);
		}
		/*fsl_print("mem_depletion_test from  SHARED RAM succeeded \n");*/
		return 0;
	}
	else
	{
		while((fsl_get_mem(size,mem_partition,alignment,&curr_addr_64)) == 0)
		{
			adresses[count++] = curr_addr_64;
			if(check_returned_address(curr_addr_64,size,alignment,mem_partition) != 0)
				return -1;
			prev_addr_64 = curr_addr_64;
			prev_size = size;
			prev_alignment = alignment;
			size = size << 1;
			alignment = (size > max_alignment_size)? max_alignment_size : size;
			if(0 != max_alloc_size && size > max_alloc_size)
				break;
		}
		fsl_put_mem(prev_addr_64);
		if(fsl_get_mem(prev_size,mem_partition,prev_alignment,&curr_addr_64) != 0)
		{
			/*fsl_print("mem_depletion_test from %d failed\n",mem_partition);*/
			return -ENAVAIL;
		}
		if(check_returned_address(curr_addr_64,prev_size,prev_alignment,mem_partition) != 0)
						return -1;
		fsl_put_mem(curr_addr_64);
		for(int i = 0 ; i < count-1 ; i++)
		{
			fsl_put_mem(adresses[i]);
		}
		/*fsl_print("mem_depletion_test from %d succeeded\n",mem_partition);*/
		return 0;
	}
	return 0;
}

static  int check_returned_address(uint64_t paddr,
                                           uint64_t size,
                                           uint64_t alignment,
                                           uint32_t mem_partition)
{
    switch(mem_partition)
    {
        case MEM_PART_PEB:
	    if(check_returned_get_mem_address(paddr,size,alignment,&peb_info))
	    {
                fsl_print("check_returned_address failed for MEM_PART_PEB\n") ;
		return -1;
	    }
	    break;
       case MEM_PART_SYSTEM_DDR:
	    if(check_returned_get_mem_address(paddr,size,alignment,&sys_ddr_info))
	    {
                fsl_print("check_returned_address failed for MEM_PART_SYSTEM_DDR\n") ;
		return -1;
	    }
	    break;
	case MEM_PART_DP_DDR:
	    if(check_returned_get_mem_address(paddr,size,alignment,&dp_ddr_info))
	    {
		fsl_print("check_returned_address failed for MEM_PART_SYSTEM_DDR\n");
		return -1;
	    }
	    break;
	case MEM_PART_SH_RAM:
            if(check_returned_malloc_address((uint32_t)paddr,(uint32_t)size,
                                             (uint32_t)alignment,&sh_ram_info))
            {
	        fsl_print("check_returned_address failed for MEM_PART_SH_RAM\n");
	        return -1;
            }
	    break;
    }
    return 0;
}

int slob_check_free_lists_after_free(e_memory_partition_id mem_part)
{
	int rc;
	uint64_t p0; /* algn=64, size=63 */
	uint64_t p1; /* algn=64, size=31 */
	uint64_t old_p1;
	uint64_t p2; /* algn=32, size= 4 */

	/* Tested scenario:
	.....p0.........p1=p0+64...p2=p1+32.........
	___________________________________________
	.....AAAAAAAAAA.BBBBBBBBBB.CCCC.............
			fsl_put(p1)
	.....AAAAAAAAAA.!!!!!!!!!!.CCCC.............
			fsl_get(algn=64, size=31)
	.....AAAAAAAAAA.!!!!!!!!!!.CCCC....BBBBBB...
			end of scenario           */

	rc = fsl_get_mem(0x3f, mem_part, 0x40, &p0);
	rc = fsl_get_mem(0x1f, mem_part, 0x40, &p1);
	old_p1 = p1;
	rc = fsl_get_mem(0x4, mem_part, 0x20, &p2);
	fsl_put_mem(p1);
	rc = fsl_get_mem(0x10, mem_part, 0x40, &p1);

	if (p1 != old_p1) /* if the just allocated block is not available */
		return -1;

	return 0;
}
