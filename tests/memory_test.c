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

#include "fsl_stdio.h"
#include "fsl_cdma.h"
#include "fsl_io.h"
#include "aiop_common.h" /* for struct aiop_init_info */
#include "fsl_malloc.h"
#include "fsl_platform.h"

int memory_test();
static int cdma_peb_test();
static int cdma_dpddr_test();
static int cdma_systemddr_test();


static const uint32_t value = 0xdeadbeef;
static int cdma_test(uint64_t paddr);
extern struct aiop_init_info g_init_data;


#if 0
int memory_test()
{

	int local_error = 0,error = 0;
	local_error = cdma_peb_test();
	if(!local_error)
		fsl_os_print("cdma_peb_test succeeded\n"); 
	else
		fsl_os_print("cdma_peb_test failed\n");	
	error |= local_error;
	local_error = cdma_dpddr_test();
	if(!local_error)
		fsl_os_print("cdma_dpddr_test succeeded\n"); 
	else
		fsl_os_print("cdma_dpddr_test failed\n");
	error |= local_error;
	if(g_init_data.app_info.sys_ddr1_size){
	    local_error = cdma_systemddr_test();
	    if(!local_error)
	        fsl_os_print("cdma_systemddr_test succeeded\n"); 
	    else
		    fsl_os_print("cdma_systemddr_test failed\n");
	}
	error |= local_error;
	return error;

	return 0;
}



static int cdma_peb_test()
{
	uint64_t paddr = 0;
	int error = 0;
	if(fsl_os_get_mem(8,MEM_PART_PEB,4,&paddr)){
		 fsl_os_print("cdma_peb_test: fsl_os_get_mem from MEM_PART_PEB  failed\n"); 
		return EIO;	
	}		 
	error = cdma_test(paddr);
	fsl_os_put_mem(paddr);
	return error;
}

static int cdma_dpddr_test()
{	
	uint64_t paddr = 0;
	int error = 0;
	if(fsl_os_get_mem(8,MEM_PART_DP_DDR,4,&paddr)){
		 fsl_os_print("cdma_peb_test: fsl_os_get_mem from MEM_PART_DP_DDR  failed\n"); 
		return EIO;	
	}		 
	error = cdma_test(paddr);
	fsl_os_put_mem(paddr);
	return error;		
}

static int cdma_systemddr_test()
{
	uint64_t paddr = 0;
	int error = 0;
	if(fsl_os_get_mem(8,MEM_PART_SYSTEM_DDR,4,&paddr)){
		 fsl_os_print("cdma_peb_test: fsl_os_get_mem from MEM_PART_SYSTEM_DDR  failed\n"); 
		return EIO;
	}		 
	error = cdma_test(paddr);
	fsl_os_put_mem(paddr);
	return error;		
}

static int cdma_test(uint64_t paddr)
{
	// variable on stack ( workspace) for cdma read/write.
	uint32_t variable_on_stack = 0,old_value = 0,old_value_off_4 = 0;
	uint32_t addr = 0,addr_offs_4 = 0;
	
	addr = PTR_TO_UINT(fsl_os_phys_to_virt(paddr));
	addr_offs_4 = addr + 4;
	 
	/* store old values to be restored later */
	old_value = ioread32(UINT_TO_PTR(addr));
	old_value_off_4 = ioread32(UINT_TO_PTR(addr_offs_4));
	
	iowrite32(value, UINT_TO_PTR(addr));
	cdma_read(&variable_on_stack,paddr, sizeof(value));
	if(ioread32(&variable_on_stack) != value)
		return EIO;
	cdma_write(paddr + 4, &variable_on_stack, sizeof(value));
	if(ioread32(UINT_TO_PTR(addr_offs_4)) != value)
		return EIO;
	/* restore the original values */
	iowrite32(old_value, UINT_TO_PTR(addr));
	iowrite32(old_value_off_4, UINT_TO_PTR(addr_offs_4));
	return 0;
}
#endif
