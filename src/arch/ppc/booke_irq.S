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

/*

 External interrupt handler code
 Saves all and passes control to ExternalIntrHandler routine

 WARNING #1
    This code will be moved by initialization routine
    Do not use here instructions which depend on its
    placement (for example b/bl).

 WARNING #2
    Length of each handler here cannot exceed 256 bytes (64 instructions)
*/

#include "ppc/include/fsl_core_booke_regs.h"
#include "booke.h"


#define LEN_OF_REG      4     /* Length of each register in bytes     */


.extern booke_generic_isr /* void booke_generic_isr(unsigned long exceptNumber) */
.extern pic_isr
.extern booke_critical_isr
.extern dec_isr
.extern sys_bottom_half
/* .extern __reset */


/*****************************************************************************

    isr_reduced_gprs_store

    Save the general purpose registers for an interrupt service routine

*****************************************************************************/
isr_reduced_gprs_store:     .macro
    /* Saving registers (store registers 0, 3-12 in stack)
    store  rx,     offset_y     from    rz  */
    stw     0,  (2 + 0)*  LEN_OF_REG    (1)
    stw     3,  (2 + 1)*  LEN_OF_REG    (1)
    stw     4,  (2 + 2)*  LEN_OF_REG    (1)
    stw     5,  (2 + 3)*  LEN_OF_REG    (1)
    stw     6,  (2 + 4)*  LEN_OF_REG    (1)
    stw     7,  (2 + 5)*  LEN_OF_REG    (1)
    stw     8,  (2 + 6)*  LEN_OF_REG    (1)
    stw     9,  (2 + 7)*  LEN_OF_REG    (1)
    stw     10, (2 + 8)*  LEN_OF_REG    (1)
    stw     11, (2 + 9)*  LEN_OF_REG    (1)
    stw     12, (2 + 10)* LEN_OF_REG    (1)
    .endm

/*****************************************************************************

    isr_reduced_gprs_restore

    Restores the general purpose registers for an interrupt service routine

*****************************************************************************/
isr_reduced_gprs_restore:     .macro
    /* load registers 0, 3-12 from stack where they were saved.
    load   rx,     offset_y     from    rz */
    lwz     0,  (2 + 0)*  LEN_OF_REG    (1)
    lwz     3,  (2 + 1)*  LEN_OF_REG    (1)
    lwz     4,  (2 + 2)*  LEN_OF_REG    (1)
    lwz     5,  (2 + 3)*  LEN_OF_REG    (1)
    lwz     6,  (2 + 4)*  LEN_OF_REG    (1)
    lwz     7,  (2 + 5)*  LEN_OF_REG    (1)
    lwz     8,  (2 + 6)*  LEN_OF_REG    (1)
    lwz     9,  (2 + 7)*  LEN_OF_REG    (1)
    lwz     10, (2 + 8)*  LEN_OF_REG    (1)
    lwz     11, (2 + 9)*  LEN_OF_REG    (1)
    lwz     12, (2 + 10)* LEN_OF_REG    (1)
    .endm

/*****************************************************************************

    isr_sprs_store

    Save the special purpose registers for an interrupt service routine

*****************************************************************************/
isr_sprs_store:     .macro
    mfspr   7,  SRR0
    stw     7,  (2 + NUM_OF_GREG + 0)* LEN_OF_REG(1)
    mfspr   8,  SRR1
    stw     8,  (2 + NUM_OF_GREG + 1)* LEN_OF_REG(1)
    mfcr    9
    stw     9,  (2 + NUM_OF_GREG + 2)* LEN_OF_REG(1)
    mfspr   10, LR
    stw     10, (2 + NUM_OF_GREG + 3)* LEN_OF_REG(1)
    mfspr   11, CTR
    stw     11, (2 + NUM_OF_GREG + 4)* LEN_OF_REG(1)
    mfspr   12,  XER
    stw     12, (2 + NUM_OF_GREG + 5)* LEN_OF_REG(1)
    .endm

/*****************************************************************************

    isr_sprs_restore

    Restores the special purpose registers for an interrupt service routine

*****************************************************************************/
isr_sprs_restore:     .macro
    lwz     7,     (2 + NUM_OF_GREG + 0)* LEN_OF_REG(1)
    mtspr   SRR0,   7
    lwz     8,     (2 + NUM_OF_GREG + 1)* LEN_OF_REG(1)
    mtspr   SRR1,   8
    lwz     9,     (2 + NUM_OF_GREG + 2)* LEN_OF_REG(1)
    mtcr    9
    lwz     10,    (2 + NUM_OF_GREG + 3)* LEN_OF_REG(1)
    mtspr   LR,     10
    lwz     11,    (2 + NUM_OF_GREG + 4)* LEN_OF_REG(1)
    mtspr   CTR,    11
    lwz     12,    (2 + NUM_OF_GREG + 5)* LEN_OF_REG(1)
    mtspr   XER,    12
    .endm

/*****************************************************************************

    isr_critical_sprs_store

    Save the special purpose registers for a critical interrupt service routine

*****************************************************************************/
isr_critical_sprs_store:     .macro
    mfspr   7,      CSRR0
    stw     7,      (2 + NUM_OF_GREG + 0)* LEN_OF_REG(1)
    mfspr   8,      CSRR1
    stw     8,      (2 + NUM_OF_GREG + 1)* LEN_OF_REG(1)
    mfcr    9
    stw     9,      (2 + NUM_OF_GREG + 2)* LEN_OF_REG(1)
    mfspr   10,     LR
    stw     10,     (2 + NUM_OF_GREG + 3)* LEN_OF_REG(1)
    mfspr   11,     CTR
    stw     11,     (2 + NUM_OF_GREG + 4)* LEN_OF_REG(1)
    mfspr   12,     XER
    stw     12,     (2 + NUM_OF_GREG + 5)* LEN_OF_REG(1)
    .endm

/*****************************************************************************

    isr_critical_sprs_restore

    Restores the special purpose registers for an interrupt service routine

*****************************************************************************/
isr_critical_sprs_restore:     .macro
    lwz     7,     (2 + NUM_OF_GREG + 0)* LEN_OF_REG(1)
    mtspr   CSRR0,   7
    lwz     8,     (2 + NUM_OF_GREG + 1)* LEN_OF_REG(1)
    mtspr   CSRR1,   8
    lwz     9,     (2 + NUM_OF_GREG + 2)* LEN_OF_REG(1)
    mtcr    9
    lwz     10,    (2 + NUM_OF_GREG + 3)* LEN_OF_REG(1)
    mtspr   LR,     10
    lwz     11,    (2 + NUM_OF_GREG + 4)* LEN_OF_REG(1)
    mtspr   CTR,    11
    lwz     12,    (2 + NUM_OF_GREG + 5)* LEN_OF_REG(1)
    mtspr   XER,     12
    .endm

/*****************************************************************************

    isr_machine_check_sprs_store

    Save the special purpose registers for a machine check interrupt service routine

*****************************************************************************/
isr_machine_check_sprs_store:     .macro
    mfspr   7,      MCSRR0
    stw     7,      (2 + NUM_OF_GREG + 0)* LEN_OF_REG(1)
    mfspr   8,      MCSRR1
    stw     8,      (2 + NUM_OF_GREG + 1)* LEN_OF_REG(1)
    mfcr    9
    stw     9,      (2 + NUM_OF_GREG + 2)* LEN_OF_REG(1)
    mfspr   10,     LR
    stw     10,     (2 + NUM_OF_GREG + 3)* LEN_OF_REG(1)
    mfspr   11,     CTR
    stw     11,     (2 + NUM_OF_GREG + 4)* LEN_OF_REG(1)
    mfspr   12,     XER
    stw     12,     (2 + NUM_OF_GREG + 5)* LEN_OF_REG(1)
    .endm

/*****************************************************************************

    isr_machine_check_sprs_restore

    Restores the special purpose registers for a machine check interrupt service routine

*****************************************************************************/
isr_machine_check_sprs_restore:     .macro
    lwz     7,     (2 + NUM_OF_GREG + 0)* LEN_OF_REG(1)
    mtspr   MCSRR0,   7
    lwz     8,     (2 + NUM_OF_GREG + 1)* LEN_OF_REG(1)
    mtspr   MCSRR1,   8
    lwz     9,     (2 + NUM_OF_GREG + 2)* LEN_OF_REG(1)
    mtcr    9
    lwz     10,    (2 + NUM_OF_GREG + 3)* LEN_OF_REG(1)
    mtspr   LR,     10
    lwz     11,    (2 + NUM_OF_GREG + 4)* LEN_OF_REG(1)
    mtspr   CTR,    11
    lwz     12,    (2 + NUM_OF_GREG + 5)* LEN_OF_REG(1)
    mtspr   XER,     12
    .endm

/*****************************************************************************
    isr_prologue

    Saves the necessary registers for an interrupt service routine
*****************************************************************************/
isr_prologue:     .macro
    /* move stack pointer (R1) to leave space for saved registers.
       and for the two registers the called function is goint to save
       (LR and SP) */
    addi    1, 1, -((NUM_OF_GREG + NUM_OF_SREG + 2) * LEN_OF_REG )

    isr_reduced_gprs_store
    isr_sprs_store

#if defined(__GNUC__)
    /* Set FP bit in MSR register */
    mfmsr       3
    ori         3, 3, MSR_FP
    oris        3, 3, MSR_SPE_HI /* enable altiVec */
    mtmsr       3
#endif /* defined(__GNUC__) */
    .endm

/*****************************************************************************
    isr_epilogue

    Restores the necessary registers for an interrupt service routine
*****************************************************************************/
isr_epilogue:   .macro
    isr_sprs_restore
    isr_reduced_gprs_restore

    /* advance the stack pointer to point to the valid stack frame! */
    addi    1, 1, ((NUM_OF_GREG + NUM_OF_SREG + 2) * LEN_OF_REG )

    rfi
    .endm

/*****************************************************************************
    isr_prologue_ci

    Saves the necessary registers for a critical interrupt service routine
*****************************************************************************/
isr_prologue_ci:     .macro
    /* move stack pointer (R1) to leave space for saved registers.
       and for the two registers the called function is goint to save
       (LR and SP) */
    addi    1, 1, -(( NUM_OF_GREG + NUM_OF_SREG + 2) * LEN_OF_REG)

    isr_reduced_gprs_store
    isr_critical_sprs_store

    # enable debug interrupts so we can work with CW jtag debugger
    mfmsr   3
    ori     3, 3, MSR_DE
#if defined(__GNUC__)
    ori     3, 3, MSR_FP
    oris    3, 3, MSR_SPE_HI /* enable altiVec */
#endif /* defined(__GNUC__) */
    mtmsr   3
    .endm

/*****************************************************************************
    isr_epilogue_ci

    Restores the necessary registers for a critical interrupt service routine
*****************************************************************************/
isr_epilogue_ci:   .macro
    isr_critical_sprs_restore
    isr_reduced_gprs_restore

    /* advance the stack pointer to point to the valid stack frame! */
    addi    1, 1, (( NUM_OF_GREG + NUM_OF_SREG + 2) * LEN_OF_REG)

    rfci
    .endm

/*****************************************************************************
    isr_prologue_mc

    Saves the necessary registers for a machine check interrupt service routine
*****************************************************************************/
isr_prologue_mc:     .macro
    /* move stack pointer (R1) to leave space for saved registers.
       and for the two registers the called function is goint to save
       (LR and SP) */
    addi    1, 1, -(( NUM_OF_GREG + NUM_OF_SREG + 2) * LEN_OF_REG)

    isr_reduced_gprs_store
    isr_machine_check_sprs_store

#if defined(__GNUC__)
    /* Set FP bit in MSR register */
    mfmsr       3
    ori         3, 3, MSR_FP
    oris        3, 3, MSR_SPE_HI /* enable altiVec */
    mtmsr       3
#endif /* defined(__GNUC__) */
    .endm

/*****************************************************************************
    isr_epilogue_mc

    Restores the necessary registers for machine check interrupt service routine
*****************************************************************************/
isr_epilogue_mc:   .macro
    isr_machine_check_sprs_restore
    isr_reduced_gprs_restore

    /* advance the stack pointer to point to the valid stack frame! */
    addi    1, 1, (( NUM_OF_GREG + NUM_OF_SREG + 2) * LEN_OF_REG)

    rfmci
    .endm

/*****************************************************************************
    run_bottom_halves

    Runs Bottom-Halves queues
*****************************************************************************/
run_bottom_halves:  .macro
    /* C Bottom-Half handler call */
    addis   10, 0, sys_bottom_half@h
    ori     10, 10, sys_bottom_half@l
    mtspr   LR, 10
    addi    1, 1, -(2 * LEN_OF_REG) /* C-routine writes old LR and
                                           stack frame back chain here */
    blr
    addi    1, 1, (2 * LEN_OF_REG)  /* But we do not need them */
    .endm

/* .section .interrupt_vector ,4,1,6 - align (4) ,progbits ,flags (6) */
#if defined(__MWERKS__) && !defined(__GNUC__)
.section .interrupt_vector,4,"x"
#else
.section .interrupt_vector,"ax",@progbits
#endif /* defined(__MWERKS__) && ... */
.align 2


/* Must be first */
interrupt_base:
    .long   0

/*****************************************************************************
    0x0100 System Reset (not used in e500)
*****************************************************************************/
/*
.org    SYSTEM_RESET_INTR
reset:
    lis     4,__reset@h
    ori     4,4,__reset@l
    mtlr    4
    blr
*/

/*****************************************************************************
    0x0100 Critical interrupt
*****************************************************************************/
.org  0x100
booke_critical_isr:
    isr_prologue_ci

    li      3,CRITICAL_INTR
    lis     4,booke_critical_isr@h
    ori     4,4,booke_critical_isr@l
    mtlr    4
    blrl

    isr_epilogue_ci

/*****************************************************************************
    0x200 Machine Check
*****************************************************************************/
.org  0x200
machine_check_intr:
    isr_prologue_mc

    li      3,MACHINE_CHECK_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue_mc

/*****************************************************************************
    0x0300 Data Storage
*****************************************************************************/
.org  0x300
data_storage_intr:
    isr_prologue

    li      3,DATA_STORAGE_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x0400 Instruction Storage
*****************************************************************************/
.org  0x400
instruction_storage_intr:
    isr_prologue

    li      3,INSTRUCTION_STORAGE_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x500 External interrupt
*****************************************************************************/
.org  0x500
external_intr:
    isr_prologue

    /* Fast handler call directly to the C function without going
       through the generic interrupt handler. */
    addis   10, 0,  pic_isr@h
    ori     10, 10, pic_isr@l
    mtlr    10
    blrl

#ifndef DISABLE_TASKLETS
    run_bottom_halves
#endif /* DISABLE_TASKLETS */

    isr_epilogue

/*****************************************************************************
    0x0600 Alignment
*****************************************************************************/
.org  0x600
alignment_intr:
    isr_prologue

    li      3,ALIGNMENT_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x0700 Program
*****************************************************************************/
.org  0x700
program_intr:
    isr_prologue

    li      3,PROGRAM_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x0800 Floating Point Unavailable
*****************************************************************************/
/*.org  0x800
floating_point_intr:
    isr_prologue

    li      3,FLOATING_POINT_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue
*/

#if 0 /* no DEC support for now */
/*****************************************************************************
    0x900 Decrementer
*****************************************************************************/
.org 0x900
decrementer_intr:
    isr_prologue

    li      3,DECREMENTER_INTR
    lis     4,dec_isr@h
    ori     4,4,dec_isr@l
    mtlr    4
    blrl

#ifndef DISABLE_TASKLETS
    run_bottom_halves
#endif /* DISABLE_TASKLETS */

    isr_epilogue
#endif /* 0 */

/*****************************************************************************
    0xb00 Watchdog timer interrupt offset
*****************************************************************************/
.org 0xb00
watchdog_timer_intr:
    isr_prologue

    li      3,WATCHDOG_TIMER_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0xc00 System Call
*****************************************************************************/
.org 0xc00
system_call_intr:
    isr_prologue

    li      3,SYSTEM_CALL_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0xa00 Auxiliary processor unavailable interrupt offset
*****************************************************************************/
/*.org  0xa00
auxiliary_intr:
    isr_prologue

    li      3,AUXILIARY_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue
*/

/*****************************************************************************
    0xf00 Fixed-interval timer interrupt offset
*****************************************************************************/
.org 0xf00
fixed_interval_intr:
    isr_prologue

    li      3,FIXED_INTERVAL_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x1000  Instruction TLB error interrupt offset
*****************************************************************************/
.org 0x1000
i_tlb_error_intr:
    isr_prologue

    li      3,I_TLB_ERROR_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x1100 Data TLB error interrupt offset
*****************************************************************************/
.org 0x1100
d_tlb_error_intr:
    isr_prologue

    li      3,D_TLB_ERROR_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x1600  SPE APU unavailable interrupt offset
*****************************************************************************/
.org  0x1600
spe_apu_intr:
    isr_prologue

    li      3,SPE_APU_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x1700  SPE floating-point data exception interrupt offset
*****************************************************************************/
.org  0x1700
spe_flt_data_intr:
    isr_prologue

    li      3,SPE_FLT_DATA_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x1800 SPE floating-point round exception interrupt offset
*****************************************************************************/
.org  0x1800
spe_flt_round_intr:
    isr_prologue

    li      3,SPE_FLT_ROUND_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x1900 Performance monitor
*****************************************************************************/
.org  0x1900
perf_monitor_intr:
    isr_prologue

    li      3,PERF_MONITOR_INTR
    lis     4,booke_generic_isr@h
    ori     4,4,booke_generic_isr@l
    mtlr    4
    blrl

    isr_epilogue

/*****************************************************************************
    0x1a00 Auxiliary processor unavailable (Not supported on the e500)
*****************************************************************************/
.org  0x1a00
    nop

/*****************************************************************************
    0x1b00 Reserved
*****************************************************************************/
.org  0x1b00
    nop

/*****************************************************************************
    0x1c00 Reserved
*****************************************************************************/
.org  0x1c00
    nop

/*****************************************************************************
    0x1d00 Reserved
*****************************************************************************/
.org  0x1d00
    nop

