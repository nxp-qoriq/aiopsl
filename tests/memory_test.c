#include "fsl_stdio.h"
#include "fsl_cdma.h"
#include "fsl_io.h"

int memory_test();
int cdma_peb_test();
int cdma_dpddr_test();
int cdma_systemddr_test();

#define PEB_AIOP_OFFSET 0x80000000
#define DP_DDR_AIOP_OFFSET 0x58000000

static const uint32_t value = 0xdeadbeef;

int memory_test()
{
	int err1 = 0,err2 = 0,err3 = 0;
	/*
	 * Temporary until finding out why simulator throws an assert in CDMA with physical address.
	err1 = cdma_peb_test();
	if(!err1)
		fsl_os_print("cdma_peb_test succeeded\n"); 
	else
		fsl_os_print("cdma_peb_test failed\n");	
		*/
	err2 = cdma_dpddr_test();
	if(!err2)
		fsl_os_print("cdma_dpddr_test succeeded\n"); 
	else
		fsl_os_print("cdma_dpddr_test failed\n");
	/*
	err3 = cdma_systemddr_test();
	if(!err3)
		fsl_os_print("cdma_systemddr_test succeeded\n"); 
	else
		fsl_os_print("cdma_systemddr_test failed\n");
		*/
	return err1 | err2 | err3;
}


int cdma_peb_test()
{
	uint32_t *addr = UINT_TO_PTR(PEB_AIOP_OFFSET);
	uint32_t *addr_offs_4 = UINT_TO_PTR(PEB_AIOP_OFFSET + 4);
	//uint32_t variable_on_stack = 0; // variable on stack ( workspace) for cdma read/write.
	uint32_t variable_on_stack = 0xdeadbeef; // variable on stack ( workspace) for cdma read/write.	
	dma_addr_t phys_addr = 0;
	phys_addr = fsl_os_virt_to_phys(addr);
	/*
	iowrite32(value, addr);
	phys_addr = fsl_os_virt_to_phys(addr);
	cdma_read(&variable_on_stack,phys_addr, sizeof(value));
	if(ioread32(&variable_on_stack) != value)
		return EIO;
	*/
	cdma_write(phys_addr + 4, &variable_on_stack, sizeof(value));
	if(ioread32(addr_offs_4) != value)
		return EIO;	
	return E_OK;
}

int cdma_dpddr_test()
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
	return E_OK;
}

int cdma_systemddr_test()
{
	return E_OK;
}
