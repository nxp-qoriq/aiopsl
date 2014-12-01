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

int memory_test();
static int cdma_peb_test();
static int cdma_dpddr_test();
static int cdma_systemddr_test();

/*PEB_AIOP_OFFSET should be compatible with #define MEMORY_INFO that is defined in init.c */
#define PEB_AIOP_OFFSET 0x80000100 
#define DP_DDR_AIOP_OFFSET 0x40000100
#define SYSTEM_DDR_AIOP_OFFSET 0x20000100

static const uint32_t value = 0xdeadbeef;

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
	local_error = cdma_systemddr_test();
	if(!local_error)
		fsl_os_print("cdma_systemddr_test succeeded\n"); 
	else
		fsl_os_print("cdma_systemddr_test failed\n");
	error |= local_error;
	return error;
}


static int cdma_peb_test()
{
	uint32_t *addr = UINT_TO_PTR(PEB_AIOP_OFFSET);
	uint32_t *addr_offs_4 = UINT_TO_PTR(PEB_AIOP_OFFSET + 4);
	uint32_t variable_on_stack = 0; // variable on stack ( workspace) for cdma read/write.	
	dma_addr_t phys_addr = 0;
	phys_addr = fsl_os_virt_to_phys(addr);
	iowrite32(value, addr);
	cdma_read(&variable_on_stack,phys_addr, sizeof(value));
	if(ioread32(&variable_on_stack) != value)
		return EIO;
	cdma_write(phys_addr + 4, &variable_on_stack, sizeof(value));
	if(ioread32(addr_offs_4) != value)
		return EIO;	
	return 0;
}

static int cdma_dpddr_test()
{	
	uint32_t *addr = UINT_TO_PTR(DP_DDR_AIOP_OFFSET);
	uint32_t *addr_offs_4 = UINT_TO_PTR(DP_DDR_AIOP_OFFSET + 4);
	uint32_t variable_on_stack = 0; // variable on stack ( workspace) for cdma read/write.
	dma_addr_t phys_addr = 0;
	iowrite32(value, addr);
	phys_addr = fsl_os_virt_to_phys(addr);
	cdma_read(&variable_on_stack,phys_addr, sizeof(value));
	if(ioread32(&variable_on_stack) != value)
		return EIO;
	cdma_write(phys_addr + 4, &variable_on_stack, sizeof(value));
	if(ioread32(addr_offs_4) != value)
		return EIO;	
	return 0;
}

static int cdma_systemddr_test()
{
	uint32_t *addr = UINT_TO_PTR(SYSTEM_DDR_AIOP_OFFSET);
	uint32_t *addr_offs_4 = UINT_TO_PTR(SYSTEM_DDR_AIOP_OFFSET + 4);
	uint32_t variable_on_stack = 0; // variable on stack ( workspace) for cdma read/write.
	dma_addr_t phys_addr = 0;
	iowrite32(value, addr);
	phys_addr = fsl_os_virt_to_phys(addr);
	cdma_read(&variable_on_stack,phys_addr, sizeof(value));
	if(ioread32(&variable_on_stack) != value)
		return EIO;
	cdma_write(phys_addr + 4, &variable_on_stack, sizeof(value));
	if(ioread32(addr_offs_4) != value)
		return EIO;
	return 0;
}
