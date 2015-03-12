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

#ifndef __BUILD_FLAGS_H
#define __BUILD_FLAGS_H


#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL         0
#endif /* DEBUG_LEVEL */

#ifndef DEBUG_ERRORS
#define DEBUG_ERRORS        1
#endif

#define DEBUG_GLOBAL_LEVEL  (REPORT_LEVEL_WARNING + DEBUG_LEVEL)
#define ERROR_GLOBAL_LEVEL  (REPORT_LEVEL_MAJOR + DEBUG_LEVEL)

#ifndef STACK_OVERFLOW_DETECTION
#define STACK_OVERFLOW_DETECTION 1
#endif 

#if (DEBUG_LEVEL > 0)
#define DEBUG
#define DEBUG_FSL_OS_MALLOC

#else
#define DISABLE_SANITY_CHECKS
#define DISABLE_ASSERTIONS
#endif /* (DEBUG_LEVEL > 0) */

#define LS2085A
#define AIOP
#define SOC_PPC_CORE
/*#define SYS_SMP_SUPPORT*/
//#define SIMULATOR
#define DEBUG_NO_MC

//#define NO_DP_DDR

#define CDC_ROC

#endif /* __BUILD_FLAGS_H */
