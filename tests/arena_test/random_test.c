/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

#include "fsl_errors.h"
#include "fsl_io_ccsr.h"
#include "fsl_platform.h"
#include "fsl_smp.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_aiop_common.h"

#if defined (AIOP) && defined (LS2085A)
#include "ls2085a_aiop/platform_aiop_spec.h"
#endif

int random_test(void);
int random_init(void);

#define MAX_NUM_OF_CORES	16
#define MAX_NUM_OF_TASKS	16
uint32_t rnd_seed[MAX_NUM_OF_CORES][MAX_NUM_OF_TASKS];
uint8_t rnd_lock;
int num_of_cores;
int num_of_tasks;


extern __TASK uint32_t	seed_32bit;




int random_test(void)
{
	int i, j;
	int core,task;

	if(seed_32bit == 0)
		return -EINVAL;

	core = (int)((seed_32bit & 0xff00) >> 8 ) - 1;
	task = (int)(seed_32bit & 0x00ff) - 1;

	if(core !=  core_get_id())
		return -EINVAL;
	if(core > 15 || task > 15 || core < 0 || task < 0)
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

int random_init(void)
{
	int i, l_num_of_cores = 0;

	if (sys_is_master_core()) {

		uint64_t cores_mask = sys_get_cores_mask();

		 struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *)
			                               sys_get_handle(FSL_MOD_AIOP_TILE, 1);

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


