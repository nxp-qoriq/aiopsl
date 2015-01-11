
#ifndef __BRINGUP_TESTS_H
#define __BRINGUP_TESTS_H

#define ON  1
#define OFF 0

#define TEST_MEM_ACCESS 	OFF

#if (TEST_MEM_ACCESS == ON)
/* memory access test */
int mem_standalone_init();
int mem_test();
#endif /* TEST_MEM_ACCESS */

#endif /* __BRINGUP_TESTS_H */
