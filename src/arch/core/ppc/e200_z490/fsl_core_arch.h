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

/**************************************************************************//**
 @File          fsl_core_arch.h

 @Description   BOOKE external definitions prototypes
                This file is not included by the BOOKE
                source file as it is an assembly file. It is used
                only for prototypes exposure, for inclusion
                by user and other modules.
                Specific for E200 z490 core
*//***************************************************************************/

#ifndef __FSL_CORE_ARCH_H
#define __FSL_CORE_ARCH_H

#include "fsl_types.h"

#define core_dcache_enable
#define core_dcache_disable  

uint32_t booke_get_CTSTWS(void);            /* [467]  CTS Task Watchdog Status Register. */
void booke_set_CTSTWS(uint32_t newvalue);   /* [467]  CTS Task Watchdog Status Register. */
uint32_t booke_get_TASKSCR0(void);          /* [476]  Task Control and Status Register 0 */


uint32_t booke_get_CTSCSR0(void);           /* [464]  CTS gen control and status reg 0. */
void booke_set_CTSCSR0(uint32_t newvalue);  /* [464]  CTS gen control and status reg 0. */


/* E200-AIOP special regs */
// Number of tasks as they defined by CTSCSR register.
#define CTSCSR_ENABLE 		(0x80000000)
#define CTSCSR_1_TASKS 		(0x00000000)  
#define CTSCSR_2_TASKS 		(0x01000000)
#define CTSCSR_4_TASKS 		(0x02000000)
#define CTSCSR_8_TASKS 		(0x03000000)
#define CTSCSR_16_TASKS 	(0x04000000)
#define CTSCSR_TASKS_MASK (CTSCSR_2_TASKS | CTSCSR_4_TASKS | CTSCSR_8_TASKS | CTSCSR_16_TASKS)

#endif /* __FSL_CORE_ARCH_H */
