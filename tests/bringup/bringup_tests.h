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
#ifndef __BRINGUP_TESTS_H
#define __BRINGUP_TESTS_H

#define ON  1
#define OFF 0

#if (TEST_MEM_ACCESS == ON)
/* memory access test */
int mem_standalone_init();
int mem_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_MEM_ACCESS"
#endif /* TEST_MEM_ACCESS */

int console_print_init();
#if (TEST_CONSOLE_PRINT == ON)
int console_print_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_CONSOLE_PRINT"
#endif /* TEST_CONSOLE_PRINT */

/* Those 2 tests can't be tested together */
#if (TEST_EXCEPTIONS == ON)
int exceptions_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_EXCEPTIONS"
#elif (TEST_STACK_OVF == ON)
int stack_ovf_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_STACK_OVF"
#endif

#if (TEST_SINGLE_CLUSTER == ON)
int single_cluster_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_SINGLE_CLUSTER"
#endif

#if (TEST_MULTI_CLUSTER == ON)
int multi_cluster_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_MULTI_CLUSTER"
#endif

#if (TEST_SPINLOCKS == ON)
int spinlock_standalone_init();
int spinlock_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_SPINLOCKS"
#endif

#if (TEST_AIOP_MC_CMD == ON)
int aiop_mc_cmd_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_AIOP_MC_CMD"
#endif /* TEST_AIOP_MC_CMD */

#if (TEST_BUFFER_POOLS == ON)
#undef TEST_DPBP
#define TEST_DPBP OFF
#if (TEST_DPNI == ON)
	#define buffer_pool_init() 0
#else
int buffer_pool_init();
#endif
int buffer_pool_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_BUFFER_POOLS"
#endif /* TEST_BUFFER_POOLS */

#if (TEST_DPNI == ON)
#undef TEST_DPBP
#define TEST_DPBP OFF
int dpni_init();
int dpni_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_DPNI"
#endif /* TEST_DPNI */

#if (TEST_DPBP == ON)
int dpbp_init();
int dpbp_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_DPBP"
#endif /* TEST_DPBP */

#if (TEST_GPP_DDR == ON)
int gpp_sys_ddr_init();
int gpp_sys_ddr_test();
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_GPP_DDR"
#endif

#if (TEST_BIG == ON)
#undef _TEST_NAME_
#define _TEST_NAME_ "TEST_BIG"
#endif

#endif /* __BRINGUP_TESTS_H */
