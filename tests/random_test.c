#include "fsl_errors.h"
#include "fsl_io_ccsr.h"
#include "platform.h"
#include "fsl_smp.h"
#include "kernel/fsl_spinlock.h"
#include "sys.h"

#if defined (AIOP) && defined (LS2085A)
#include "ls2085a_aiop/platform_aiop_spec.h"
#endif

int random_test();
int random_init();

#define MAX_NUM_OF_CORES	16
#define MAX_NUM_OF_TASKS	16
__SHRAM uint32_t rnd_seed[MAX_NUM_OF_CORES][MAX_NUM_OF_TASKS];
__SHRAM uint8_t rnd_lock;
__SHRAM int num_of_cores;
__SHRAM int num_of_tasks;

extern __SHRAM t_system sys;
extern __TASK uint32_t	seed_32bit;




int random_test()
{
	int i, j;
	int core,task;

	if(seed_32bit == 0)
		return -EINVAL;

	core = (int)((seed_32bit & 0xff00) >> 8 ) - 1;
	task = (int)(seed_32bit & 0x00ff) - 1;

	if(core !=  core_get_id())
		return -EINVAL;

	if(rnd_seed[core][task] != 0)
		return 0;


	for (i = 0; i < num_of_cores; i ++)
	{
		for (j = 0; i < num_of_tasks; i ++)
		{
			if(rnd_seed[i][j] == seed_32bit)
				return -EINVAL;
		}
	}

	lock_spinlock(&rnd_lock);

	rnd_seed[core][task] = seed_32bit;

	unlock_spinlock(&rnd_lock);


	return 0;
}

int random_init()
{
	int i, l_num_of_cores = 0;

	if (sys_is_master_core()) {

		uint64_t cores_mask = sys_get_cores_mask();

		 struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *)
			                               sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);

		for(i = 0; i < MAX_NUM_OF_CORES; i++ ){
			if(cores_mask & 0x1){
				l_num_of_cores ++;
			}
			cores_mask = cores_mask >> 1;
		}


		num_of_tasks = (int) ioread32_ccsr(&aiop_regs->cmgw_regs.wscr) & 0x000000ff;

		switch(num_of_tasks) {
		case (0):
						    num_of_tasks = 1;
		break;
		case (1):
						    num_of_tasks = 2;
		break;
		case (2):
						    num_of_tasks = 4;
		break;
		case (3):
						    num_of_tasks = 8;
		break;
		case (4):
						    num_of_tasks = 16;
		break;
		default:
			return -EINVAL;
		}

		num_of_cores = l_num_of_cores;
		if(l_num_of_cores <= 0 || l_num_of_cores > MAX_NUM_OF_CORES)
			return -EINVAL;
	}
	return 0;
}


