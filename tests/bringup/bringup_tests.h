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

#define TEST_MEM_ACCESS 	OFF
#define TEST_CONSOLE_PRINT	ON
#define TEST_EXCEPTIONS		OFF
#define TEST_STACK_OVF		OFF
#define TEST_DPBP			OFF
#define TEST_AIOP_MC_CMD	OFF
#define TEST_SINGLE_CLUSTER	OFF
#define TEST_MULTI_CLUSTER	OFF
#define TEST_DPNI			OFF
#define TEST_BUFFER_POOLS	OFF
#define TEST_GPP_DDR		OFF
#define TEST_SPINLOCK		OFF

#if (TEST_SPINLOCKS == ON)
/* memory access test */
int spinlock_standalone_init();
int spinlock_test();
#endif /* TEST_MEM_ACCESS */

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

#if (TEST_GPP_DDR == ON)
int gpp_sys_ddr_init();
int gpp_sys_ddr_test(uint64_t iova, uint16_t size);
#endif

#endif /* __BRINGUP_TESTS_H */
