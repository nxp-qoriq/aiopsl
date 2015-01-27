#include "general.h"
#include "common/types.h"
#include "../../../tests/bringup/bringup_tests.h"
//#include "fsl_spinlock.h"
#include "fsl_spinlock.h"

#include "sys.h"
#include "fsl_smp.h"
#include "fsl_string.h"

#define SPIN_TEST_ITERATIONS 1000

struct spinlock_test_unit {
	uint32_t core_id; //0-none, 1-core#0, 2-core#1
	uint32_t test_id; //index in array
	uint32_t core_flag[2]; //1 if core i got spinlock;
};

struct spinlock_test_unit stest[SPIN_TEST_ITERATIONS];
uint32_t s_index;
uint32_t core_count;

/*****************************************************************************/
int spinlock_standalone_init()
{
	//TODO sys_barrier should work
	//TODO sys_barrier is using a spinlock ! ! ! ! ! ! !
	//TODO what about prints (not initiated).
	//TODO ASSERT_COND has a print in it.

	if(sys_is_master_core()) {
		int err = 0;

		err = sys_init_multi_processing();
		ASSERT_COND(err == 0);

		s_index = 0;
		core_count = 0;
		memset(&stest[0], 0, SPIN_TEST_ITERATIONS*sizeof(struct spinlock_test_unit));
	}

	return 0;
}

/*****************************************************************************/
int spinlock_test ()
{
	uint32_t i = 0;
	int spin_val = 0;

	uint8_t test_lock = 0;

	//TODO return error value (not only assert)

	/* start the test */
	while(s_index < SPIN_TEST_ITERATIONS)
	{
		unlock_spinlock(&test_lock);
		core_count ++;
		ASSERT_COND(core_count == 1);

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

	sys_barrier(); //TODO initiate

	if(sys_is_master_core()) {
		int num_taken[2] = {0,0};

		for(i=0; i<SPIN_TEST_ITERATIONS; i++) {
			num_taken[stest[i].core_id - 1] ++;

			ASSERT_COND(stest[i].test_id == i);
			ASSERT_COND((stest[i].core_id == 1) || (stest[i].core_id == 2));
			ASSERT_COND((stest[i].core_flag[0] == 1) || (stest[i].core_flag[1] == 1));
			if(stest[i].core_id == 1)
			{
				ASSERT_COND(stest[i].core_flag[0] == 1);
				ASSERT_COND(stest[i].core_flag[1] == 0);
			} else { //core_id == 2
				ASSERT_COND(stest[i].core_flag[0] == 0);
				ASSERT_COND(stest[i].core_flag[1] == 1);
			}
		}

		fsl_os_print("%d spinlocks taken by core #%d\n", num_taken[0], 0);
		fsl_os_print("%d spinlocks taken by core #%d\n", num_taken[1], 1);

		fsl_os_print("Spinlocks validation... (PASSED)\n");
	}

	sys_barrier(); //TODO initiate
}

