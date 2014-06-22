#include "fsl_malloc.h"
#include "common/errors.h"
#include "fsl_io.h"
#include "kernel/platform.h"
#include "common/fsl_slab.h"

#if defined (LS2100A) && defined (AIOP) 
#include "ls2100a_aiop/platform_aiop_spec.h"
#endif

int malloc_test();

static int allocate_check_mem(int memory_partition, 
		                      uint32_t num_iterations, uint32_t size,
		                      void** allocated_pointers );
/* Number of malloc allocations for each partition */
#define NUM_TEST_ITER 10

int malloc_test()
{
	uint32_t num_iter = NUM_TEST_ITER, size = 0x100;
	void* allocated_pointers[NUM_TEST_ITER];
	int err = 0;
	err = allocate_check_mem(0,num_iter,size,allocated_pointers); // allocate from the heap
	err = allocate_check_mem(MEM_PART_DP_DDR,num_iter,size,
			                 allocated_pointers);
	err = allocate_check_mem(MEM_PART_SH_RAM,num_iter,size,allocated_pointers);
	err = allocate_check_mem(MEM_PART_PEB,num_iter,size,allocated_pointers);	
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
				return -E_NO_MEMORY;
			iowrite32(value,allocated_pointers[i]);
			value = ioread32(allocated_pointers[i]);
			if(value != expected_value) {
				fsl_os_print("malloc from the heap has failed, address %x\n", 
						PTR_TO_UINT(allocated_pointers[i]));
				return -E_NO_MEMORY;
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
				return -E_NO_MEMORY;
			iowrite32(value,allocated_pointers[i]);
			value = ioread32(allocated_pointers[i]);
			if(value != expected_value) {
				fsl_os_print("malloc from mem part %d has failed, address %x\n", 
						memory_partition,PTR_TO_UINT(allocated_pointers[i]));
				return -E_NO_MEMORY;
			}	
		}
		for(i = 0 ; i < num_iter; i++)
		{
			fsl_os_xfree(allocated_pointers[i]);
		}
	}
	return E_OK;		
}
