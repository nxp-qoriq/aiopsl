
#ifndef __BRINGUP_TESTS_H
#define __BRINGUP_TESTS_H

#define ON  1
#define OFF 0

#define TEST_MEM_ACCESS 	OFF
#define TEST_CONSOLE_PRINT	ON
#define TEST_DPBP		ON

#if (TEST_MEM_ACCESS == ON)
/* memory access test */
int mem_standalone_init();
int mem_test();
#endif /* TEST_MEM_ACCESS */

#if (TEST_CONSOLE_PRINT == ON)
int console_print_init();
int console_print_test();
#endif /* TEST_CONSOLE_PRINT */

#if (TEST_DPBP == ON)
int dpbp_init();
int dpbp_test();
#endif /* TEST_DPBP */

#endif /* __BRINGUP_TESTS_H */
