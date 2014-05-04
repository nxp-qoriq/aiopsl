#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
//#include "fsl_dpni.h"
#include "dplib/dpni_drv.h"
#include "dpni/drv.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_ip.h"
#include "fsl_cdma.h"
#include "common/fsl_slab.h"
#include "kernel/platform.h"
#include "io.h"
#include "aiop_common.h"

__SHRAM struct slab *slab_peb = 0;
__SHRAM struct slab *slab_ddr = 0;

int app_test_slab_init();
int app_test_slab(struct slab *slab, int num_times);

int app_test_slab_init() 
{
	int        err = 0;
	dma_addr_t buff = 0;
	struct slab *my_slab;
	
	err = slab_create(5, 0, 256, 0, 0, 4, MEM_PART_1ST_DDR_NON_CACHEABLE, 0, 
	                  NULL, &my_slab);
	if (err) return err;
	
	err = slab_acquire(my_slab, &buff);
	if (err) return err;
	err = slab_release(my_slab, buff);
	if (err) return err;

	err = slab_free(&my_slab);
	if (err) return err;
	
	/* Must fail because my_slab was freed  */
	err = slab_acquire(my_slab, &buff);
	if (!err) return -EEXIST;
	
	/* Reuse slab handle test  */
	err = slab_create(1, 0, 256, 0, 0, 4, MEM_PART_1ST_DDR_NON_CACHEABLE, 0, 
	                  NULL, &my_slab);
	if (err) return err;
	
	err = slab_free(&my_slab);
	if (err) return err;

	return 0;
}

static int app_write_buff_and_release(struct slab *slab, uint64_t buff)
{
	uint64_t data1 = 0xAABBCCDD00001122;
	uint64_t data2 = 0;
	int      err = 0;

	err = cdma_write(buff, &data1, 8);
	if (err) return err;
	err = cdma_read(&data2, buff, 8);
	if (err) return err;

	if (data1 != data2) {
		slab_release(slab, buff);
		return -EPERM;
	}

	err = slab_release(slab, buff);
	if (err) return err;

	return 0;
}

int app_test_slab(struct slab *slab, int num_times)
{
	uint64_t buff[3] = {0, 0, 0};
	int      err = 0;
	int      i = 0;

	for (i = 0; i < num_times; i++) {

		err = slab_acquire(slab, &buff[0]);
		if (err || (buff == NULL)) return -ENOMEM;
		err = slab_acquire(slab, &buff[1]);
		if (err || (buff == NULL)) return -ENOMEM;
		err = slab_acquire(slab, &buff[2]);
		if (err || (buff == NULL)) return -ENOMEM;

		err = app_write_buff_and_release(slab, buff[1]);
		if (err) return err;
		err = app_write_buff_and_release(slab, buff[2]);
		if (err) return err;
		err = app_write_buff_and_release(slab, buff[0]);
		if (err) return err;
	}

	return 0;
}
