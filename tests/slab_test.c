#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "dpni/drv.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_ip.h"
#include "fsl_cdma.h"
#include "fsl_slab.h"
#include "platform.h"
#include "ls2085_aiop/fsl_platform.h"
#include "fsl_io.h"

__SHRAM struct slab *slab_peb = 0;
__SHRAM struct slab *slab_ddr = 0;

int app_test_slab_init();
int slab_init();
int slab_test();
int app_test_slab(struct slab *slab, int num_times);

int slab_init()
{
	int err = 0;
	struct slab_debug_info slab_info;
	/* Test for slab initialization, ignore it unless it fails */
	err = app_test_slab_init();
	if (err) {
		fsl_os_print("ERROR = %d: app_test_slab_init()\n", err);
		return err;
	}

	/* DDR SLAB creation */
	err = slab_create(10, 0, 256, 0, 0, 4, MEM_PART_DP_DDR, 0,
			          NULL, &slab_ddr);
	if (err) return err;

	err = slab_debug_info_get(slab_ddr, &slab_info);
	if (err) {
		return err;
	} else {
		if ((slab_info.num_buffs != slab_info.max_buffs) ||
		    (slab_info.num_buffs == 0))
			return -ENODEV;
	}

	/* PEB SLAB creation */
	err = slab_create(5, 0, 100, 0, 0, 4, MEM_PART_PEB, 0, NULL, &slab_peb);
	if (err) return err;

	err = slab_debug_info_get(slab_peb, &slab_info);
	if(!err)
		if ((slab_info.num_buffs != slab_info.max_buffs) ||
			(slab_info.num_buffs == 0))
				return -ENODEV;
	return err;
}

int app_test_slab_init()
{
	int        err = 0;
	dma_addr_t buff = 0;
	struct slab *my_slab;

	err = slab_create(5, 0, 256, 0, 0, 4, MEM_PART_DP_DDR, 0,
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
	err = slab_create(1, 0, 256, 0, 0, 4, MEM_PART_DP_DDR, 0,
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

	cdma_write(buff, &data1, 8);
	cdma_read(&data2, buff, 8);

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

int slab_test()
{
	int err = 0;
	err |= app_test_slab(slab_peb, 4);
	if (err) {
		fsl_os_print("ERROR = %d: app_test_slab(slab_peb, 4)\n", err);
	}

	err |= app_test_slab(slab_ddr, 4);
	if (err) {
		fsl_os_print("ERROR = %d: app_test_slab(slab_ddr, 4)\n", err);
	}
	return err;
}
