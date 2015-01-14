#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_core_booke.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_smp.h"
#ifdef ARENA_TEST
#include "fsl_dbg.h"
#endif

#ifdef ARENA_TEST
#define NUM_TESTED_CORES	1 /* In ARENA test only master core runs it */
#else
#define NUM_TESTED_CORES	4
#endif

#define WAITING_TIMEOUT		0x1000000

uint32_t core_arr[NUM_TESTED_CORES] = {0};
int8_t  counter = 0;

int single_cluster_test();

int single_cluster_test()
{
	uint32_t core_id =  (get_cpu_id() >> 4);
	int      i;
	int      done;
	int      t = 0;

#ifdef ARENA_TEST
	pr_debug("sys_is_master_core() %d \n", sys_is_master_core());
	pr_debug("sys_is_core_active() %d \n", sys_is_core_active(core_id));
	pr_debug("sys_get_num_of_cores() %d \n", sys_get_num_of_cores());
#endif

	/* Check smp.c API */
	if (sys_get_num_of_cores() < NUM_TESTED_CORES)
		return -EINVAL;

	/* Check fsl_spinlock.h API */
	atomic_incr8(&counter, 1);

	/* Check that all n=<4 cores get here use CORE_ID */
	core_arr[core_id] = core_id;
	do {
		done = 1;
		for (i = 0; i < NUM_TESTED_CORES; i++) {
			if (core_arr[i] != i) {
				done = 0;
				break;
			}
		}
		t++;
	} while((!done) && (t < WAITING_TIMEOUT));

	if (!done) return -ETIMEDOUT;

	/* Check smp.c API */
	for (i = 0; i < NUM_TESTED_CORES; i++) {
		if (!sys_is_core_active((uint32_t)i))
			return -EINVAL;
	}

	/* Check fsl_spinlock.h API */
	if (counter < NUM_TESTED_CORES)
		return -EINVAL;

	return 0;
}
