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
 @File          fsl_core_spec_regs.h

 @Description   BOOKE external definitions prototypes
                This file is not included by the BOOKE
                source file as it is an assembly file. It is used
                only for prototypes exposure, for inclusion
                by user and other modules.
*//***************************************************************************/

#ifndef __FSL_CORE_SPEC_REGS_H
#define __FSL_CORE_SPEC_REGS_H

/*******************************************************
* interrupt vector
*
* The handler is fixed to this address.
********************************************************/
#define CRITICAL_INTR               0x00
#define MACHINE_CHECK_INTR          0x10
#define DATA_STORAGE_INTR           0x20
#define INSTRUCTION_STORAGE_INTR    0x30
#define EXTERNAL_INTR               0x40
#define ALIGNMENT_INTR              0x50
#define PROGRAM_INTR                0x60
#define PERF_MONITOR_INTR           0x70
#define SYSTEM_CALL_INTR            0x80
#define DEBUG_INTR                  0x90
#define EFPU_DATA_INTR              0xa0
#define EFPU_ROUND_INTR             0xb0
#define EFPU_NA_INTR                0xc0
#define CTS_WD_INTR                 0xf0

/*
 *
 * Implementation-Specific SPRs (by SPR Abbreviation)
 *
 */
#define CTSCSR0     464     /* CTS General Control and Status Registers 0 */
#define CTSTWS      467     /* CTS Task Watchdog Status Register */

#define TASKSCR0    476     /* Task Control and Status Register 0 */


#endif /* __FSL_CORE_SPEC_REGS_H */
