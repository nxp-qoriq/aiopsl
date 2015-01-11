
#ifndef __BRINGUP_TESTS_H
#define __BRINGUP_TESTS_H

#define ON  1
#define OFF 0

#define TEST_MEM_ACCESS 	OFF
#define TEST_HW_SEM 		OFF
#define TEST_SPINLOCK		OFF
#define TEST_MPIC			OFF
#define TEST_CONSOLE_PRINT	OFF
#define TEST_MPU			OFF

#define TEST_AIOP			OFF


#if (TEST_MEM_ACCESS == ON)
/* memory access test */
int mem_standalone_init();
int mem_test();
#endif /* TEST_MEM_ACCESS */

#if (TEST_HW_SEM == ON)
int hw_sem_standalone_init();
int hw_sem_test();
#endif /* TEST_HW_SEM */

#if (TEST_SPINLOCK == ON)
int spinlock_standalone_init();
int spinlock_test ();
#endif /* TEST_SPINLOCK */

#if (TEST_CONSOLE_PRINT == ON)
int console_print_init();
int console_print_test();
#endif /* TEST_CONSOLE_PRINT */

#if (TEST_MPIC == ON)
int mpic_standalone_init();
int mpic_test(); 
#endif /* TEST_MPIC */

#if (TEST_MPU == ON)
int mpu_standalone_init();
int mpu_test();
#endif /* TEST_MPU */

#if (TEST_AIOP == ON)
//TODO
#endif /* TEST_AIOP */

#endif /* __BRINGUP_TESTS_H */
