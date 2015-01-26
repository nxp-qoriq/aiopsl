#include "fsl_errors.h"
#include "fsl_io.h"
#include "platform.h"
#include "ls2085_mc/fsl_platform.h"
#include "fsl_smp.h"
#include "drivers/fsl_mc.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"

#define MEM_TEST_VAL 		(0x12345678)
#define MEM_TEST_STUB_VAL	(0x24685791)

#define TEST_FLG_READ  0x1
#define TEST_FLG_WRITE 0x2

static void memInit(const uint32_t param);
int mem_standalone_init();
int mem_test();

typedef struct {
	volatile void *vaddr;
	void (*memInit)(uint32_t param);	/* Init function, in case a window needs to be configured  */
	uint32_t init_param;				/* parameter to pass to memInit() */
	uint32_t flags;
}mem_access_test_t;

mem_access_test_t mem_tests[] = {
		/* Addr      	memInit 	param	flags */
		{0x01000010, 	memInit,	0, 		TEST_FLG_READ | TEST_FLG_WRITE}, /* shared ram */
		{0x40210010, 	memInit,	0, 		TEST_FLG_READ | TEST_FLG_WRITE}, /* dp-ddr (heap) */
		{NULL, 			NULL, 		0, 		0} /* Stub (end of list) */
};

/*****************************************************************************/
static void memInit(const uint32_t param)
{
	UNUSED(param);
	//TODO
}

/*****************************************************************************/
int mem_standalone_init()
{
	//XXX Not needed for this test (yet)
	return 0;
}

/*****************************************************************************/
static int test_access(const mem_access_test_t *test) 
{	
	volatile void *addr = test->vaddr;
	int read = test->flags & TEST_FLG_READ;
	int write = test->flags & TEST_FLG_WRITE;
	volatile uint32_t read_val = MEM_TEST_STUB_VAL;
	
	if(test->memInit)
		test->memInit(test->init_param);
	
	if(write)
		iowrite32(MEM_TEST_VAL, addr);
	
	if(read) {
		l1dcache_block_invalidate(addr);
		read_val = ioread32(addr);
		
		if(read_val == MEM_TEST_STUB_VAL)
			return -EACCES;
	}
	
	if(read && write) {
		if(read_val == MEM_TEST_VAL) {
			return 0;
		} else {
			return -EACCES;
		}
	}
	
	return 0;
}

/*****************************************************************************/
int mem_test()
{
	int i=0;
	int err;
	
	while(mem_tests[i].vaddr != NULL) {
		err = test_access(&mem_tests[i]);
		if(err) 
			return err;
		
		i++;
	}
	
    return 0;
}
