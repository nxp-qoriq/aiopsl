#include "general.h"
#include "common/types.h"
#include "../../../tests/bringup/bringup_tests.h"
//#include "fsl_spinlock.h"
#include "fsl_spinlock.h"

#include "sys.h"
#include "fsl_smp.h"
#include "fsl_string.h"
#include "fsl_errors.h"

#define SPIN_TEST_ITERATIONS 1000

struct spinlock_test_unit {
	uint32_t core_id; //0-none, 1-core#0, 2-core#1
	uint32_t test_id; //index in array
	uint32_t core_flag[16]; //1 if core i got spinlock;
};

struct spinlock_test_unit stest[SPIN_TEST_ITERATIONS];
uint32_t s_index;
uint32_t core_count;
uint8_t test_lock;

/*****************************************************************************/
int spinlock_standalone_init()
{
	//TODO what about prints (not initiated).
	//TODO ASSERT_COND has a print in it.

	if(sys_is_master_core()) {
		int err = 0;

//		err = sys_init_multi_processing();
//		ASSERT_COND(err == 0);

		s_index = 0;
		core_count = 0;
		test_lock = 0;
		memset(&stest[0], 0, SPIN_TEST_ITERATIONS*sizeof(struct spinlock_test_unit));
	}

	sys_barrier(); //TODO initiate
	
	return 0;
}

/*****************************************************************************/
int spinlock_test ()
{
	uint32_t i = 0, j = 0;
	int spin_val = 0;
	int err;
	volatile err_stop = 1;
	
	/* start the test */
	while(s_index < SPIN_TEST_ITERATIONS)
	{
		lock_spinlock(&test_lock);
		core_count ++;
		if(core_count != 1) return -EEXIST;

		stest[s_index].test_id = s_index;
		stest[s_index].core_id = core_get_id() + 1;
		stest[s_index].core_flag[core_get_id()] += 1;
		s_index++;

		core_count --;
		unlock_spinlock(&test_lock);
	}

//	if(sys_is_master_core()) {
//		spin_lock_free(test_lock); //TODO NOOOooo... !!!!!!!!!!!!!!
//	}

	sys_barrier();

	if(sys_is_master_core()) {
		int num_taken[16] = {0};

		for(i=0; i<SPIN_TEST_ITERATIONS; i++) {
			num_taken[stest[i].core_id - 1] ++;

			if(stest[i].test_id != i) 
				return -EEXIST;
			if(stest[i].core_id > 16) 
				return -EEXIST;
			if(stest[i].core_flag[stest[i].core_id - 1] != 1) 
				return -EEXIST;
			for(j=0; j<16; j++) {
				if(j != (stest[i].core_id - 1)) {
					if(stest[i].core_flag[j] == 1)
						return -EEXIST;
				}
			}
		}

//		fsl_os_print("%d spinlocks taken by core #%d\n", num_taken[0], 0);
//		fsl_os_print("%d spinlocks taken by core #%d\n", num_taken[1], 1);
//
//		fsl_os_print("Spinlocks validation... (PASSED)\n");
	}

	sys_barrier();
	
	return 0;
}

