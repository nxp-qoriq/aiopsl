#include "common/errors.h"
#include "common/io.h"
#include "kernel/platform.h"
#include "platform_aiop_spec.h"
#include "kernel/smp.h"
#include "common/spinlock.h"
#include "sys.h"

int random_test();
int random_init();

#define MAX_NUM_OF_CORES	16
__SHRAM uint32_t rnd_seed[MAX_NUM_OF_CORES];
__SHRAM int rnd_ctr = 0;
__SHRAM uint8_t rnd_lock;
__SHRAM int num_of_cores;
__SHRAM int random_test_flag = 0;

extern __SHRAM t_system sys;
extern __TASK uint32_t	seed_32bit;




int random_test()
{
	int i;

	if(seed_32bit == 0)
		return -EINVAL;
	if(rnd_seed[core_get_id()] != 0)
		return 0;


	for (i = 0; i < num_of_cores; i ++)
	{
		if(rnd_seed[i] == seed_32bit)
			return -EINVAL;
	}

	lock_spinlock(&rnd_lock);
	rnd_seed[core_get_id()] = seed_32bit;
	rnd_ctr ++;
	if ( rnd_ctr == num_of_cores)
		random_test_flag = 1;
	unlock_spinlock(&rnd_lock);


	return 0;
}

int random_init()
{
	int i, l_num_of_cores = 0;

	if (sys_is_master_core()) {

		uint64_t cores_mask = sys_get_cores_mask();

		for(i = 0; i < MAX_NUM_OF_CORES; i++ ){
			if(cores_mask & 0x1){
				l_num_of_cores ++;
			}
			cores_mask = cores_mask >> 1;
		}

		num_of_cores = l_num_of_cores;
		if(l_num_of_cores <= 0 || l_num_of_cores > MAX_NUM_OF_CORES)
			return -EINVAL;
		else{
			rnd_ctr ++;
			rnd_seed[core_get_id()] = seed_32bit;
		}
	}
	return 0;
}


