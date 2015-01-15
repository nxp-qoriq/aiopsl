
#ifndef __BRINGUP_TESTS_H
#define __BRINGUP_TESTS_H

#define ON  1
#define OFF 0

#define TEST_MEM_ACCESS 	OFF
#define TEST_CONSOLE_PRINT	OFF
#define TEST_EXCEPTIONS		OFF
#define TEST_STACK_OVF		OFF
#define TEST_DPBP		OFF
#define TEST_AIOP_MC_CMD	OFF
#define TEST_SINGLE_CLUSTER	OFF
#define TEST_MULTI_CLUSTER	OFF
#define TEST_DPNI		OFF
#define TEST_BUFFER_POOLS	OFF

#if (TEST_MEM_ACCESS == ON)
/* memory access test */
int mem_standalone_init();
int mem_test();
#endif /* TEST_MEM_ACCESS */

#if (TEST_CONSOLE_PRINT == ON)
int console_print_init();
int console_print_test();
#endif /* TEST_CONSOLE_PRINT */

/* Those 2 tests can't be tested together */
#if (TEST_EXCEPTIONS == ON)
int exceptions_test();
#elif (TEST_STACK_OVF == ON)
int stack_ovf_test();
#endif

#if (TEST_SINGLE_CLUSTER == ON)
int single_cluster_test();
#endif

#if (TEST_MULTI_CLUSTER == ON)
int multi_cluster_test();
#endif

#if (TEST_AIOP_MC_CMD == ON)
int aiop_mc_cmd_test();
#endif /* TEST_AIOP_MC_CMD */

#if (TEST_BUFFER_POOLS == ON)
#undef TEST_DPBP
#undef TEST_DPNI
#define TEST_DPBP OFF
#define TEST_DPNI OFF
int dpbp_init();
int buffer_pool_init();
int buffer_pool_test();
#endif /* TEST_BUFFER_POOLS */

#if (TEST_DPBP == ON)
#undef TEST_DPNI
#define TEST_DPNI OFF
int dpbp_init();
int dpbp_test();
#endif /* TEST_DPBP */

#if (TEST_DPNI == ON)
int dpni_init();
int dpni_test();
#endif /* TEST_DPNI */


#endif /* __BRINGUP_TESTS_H */
