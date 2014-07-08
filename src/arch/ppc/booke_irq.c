/*
 *
 * @File          booke_irq.c
 * TODO rewrite
 * @Description   This is an E500 specific implementation. 
 *                This file contain the PPC general interrupt routines.
 *                The functions in this file export a general INT Service routine.
 *
 *
 * @Cautions
 *
 *
 * Author: Donio Ron .
 *
 */

#include "fsl_core_booke_regs.h"
#include "types.h"
#include "fsl_dbg.h"

#pragma push
#pragma section code_type ".interrupt_vector"
#pragma force_active on
#pragma function_align 256

void booke_init_interrupt_vector(void);

/*****************************************************************/
/* routine:       booke_generic_irq_init                         */
/*                                                               */
/* description:                                                  */
/*        On the BOOKE the Ivor's need to initialize to point to */
/*        right address.                                         */
/* arguments:                                                    */
/*    None.                                                      */
/*                                                               */
/*****************************************************************/
void booke_generic_irq_init(void)
{
    booke_init_interrupt_vector();
}


/*****************************************************************/
/* routine:       booke_critical_isr                             */
/*                                                               */
/* description:                                                  */
/*    Internal routine, called by the critical interrupt handler */
/*    to indicate the occurrence of an critical INT.             */
/*                                                               */
/* arguments:                                                    */
/*    intrEntry (In) - The interrupt handler original address.   */
/*                                                               */
/*****************************************************************/
void booke_critical_isr(uint32_t intr_entry)
{
    if (intr_entry != CRITICAL_INTR)
    {
        pr_err("core %d int: wrong call to CRITICAL int. handler\n", core_get_id());
        return;
    }
}

/*****************************************************************/
/* routine:       booke_generic_isr                              */
/*                                                               */
/* description:                                                  */
/*    Internal routine, called by the main interrupt handler     */
/*    to indicate the occurrence of an INT.                      */
/*                                                               */
/* arguments:                                                    */
/*    intrEntry (In) - The interrupt handler original address.   */
/*                                                               */
/*****************************************************************/
void booke_generic_isr(uint32_t intr_entry)
{
#pragma unused(intr_entry)
    //TODO complete
    pr_debug("core %d - generic interrupt\n", core_get_id());
}

asm static void branch_table(void) {
  nofralloc

    b  critical_irq /* Critical Input Interrupt (Offset 0x00) */
    nop
    nop
    nop
    b  mechinecheck_irq /* Machine Check Interrupt (Offset 0x10) */
    nop
    nop
    nop
    b  data_stor_irq /* Data Storage Interrupt (Offset 0x20) */
    nop
    nop
    nop
    b  inst_stor_irq /* Instruction Storage Interrupt (Offset 0x30) */
    nop
    nop
    nop
    b  ext_irq /* External Input Interrupt (Offset 0x40) */
    nop
    nop
    nop
    b  alignment_irq /* Alignment Interrupt (Offset 0x50) */
    nop
    nop
    nop
    b  program_irq /* Program Interrupt (Offset 0x60) */
    nop
    nop
    nop
    b  dbg_irq /* TODO: should be perf-mon */
    nop
    nop
    nop
    b  sys_call_irq /* System Call Interrupt (Offset 0x80) */
    nop
    nop
    nop
    b  dbg_irq /* Debug Interrupt (Offset 0x90) */
    nop
    nop
    nop
    b fp_data_irq /* Embedded Floating-point Data Interrupt (Offset 0xA0) */
    nop
    nop
    nop
    b fp_round_irq /* Embedded Floating-point Round Interrupt (Offset 0xB0) */
    nop
    nop
    nop
    b cts_irq /* CTS Task Watchdog Timer Interrupt (Offset 0xF0) */
    
    /* Critical Input Interrupt (Offset 0x00) */
    .align 0x100
critical_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfspr    r0,58
    stw      r0,28(rsp)
    mfspr    r0,59
    stw      r0,32(rsp)

    li  r3,CRITICAL_INTR
    lis r4,booke_critical_isr@h
    ori r4,r4,booke_critical_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtspr    59,r0
    lwz      r0,28(rsp)
    mtspr    58,r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfci

    /* Machine Check Interrupt (Offset 0x10) */
    .align 0x100
mechinecheck_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfspr    r0,570
    stw      r0,28(rsp)
    mfspr    r0,571
    stw      r0,32(rsp)

    li  r3,MACHINE_CHECK_INTR //TODO this is wrong
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtspr    571,r0
    lwz      r0,28(rsp)
    mtspr    570,r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfmci

    /* Data Storage Interrupt (Offset 0x20) */
    .align 0x100
data_stor_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,DATA_STORAGE_INTR
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi

    /* Instruction Storage Interrupt (Offset 0x30) */
    .align 0x100
inst_stor_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,INSTRUCTION_STORAGE_INTR
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi

    /* External Input Interrupt (Offset 0x40) */
    .align 0x100
ext_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,EXTERNAL_INTR
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

#if 0 /* TODO - put back!!! */
#ifndef DISABLE_TASKLETS
    addis   r10, 0, sys_bottom_half@h
    ori     r10, r10, sys_bottom_half@l
    mtspr   LR, r10
    addi    r1, r1, -(2 * LEN_OF_REG)
    blr
    addi    r1, r1, (2 * LEN_OF_REG)
#endif /* DISABLE_TASKLETS */
#endif /* 0 */

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi

    /* Alignment Interrupt (Offset 0x50) */
    .align 0x100
alignment_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,ALIGNMENT_INTR
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi

    /* Program Interrupt (Offset 0x60) */
    .align 0x100
program_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,PROGRAM_INTR
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi

    //TODO Performance Monitor Interrupt (Offset 0x70)
    
    /* System Call Interrupt (Offset 0x80) */
    .align 0x100
sys_call_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,0x0900
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi
    
    /* Debug Interrupt (Offset 0x90) */
    .align 0x100
dbg_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfspr    r0,58
    stw      r0,28(rsp)
    mfspr    r0,59
    stw      r0,32(rsp)

    li  r3,0x1000
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtspr    59,r0
    lwz      r0,28(rsp)
    mtspr    58,r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfci
    
    /* Embedded Floating-point Data Interrupt (Offset 0xA0) */
    .align 0x100
fp_data_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,0x1200
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi
    
    /* Embedded Floating-point Round Interrupt (Offset 0xB0) */
    .align 0x100
fp_round_irq:
    stwu     rsp,-80(rsp)
    stw      r0,8(rsp)
    mfctr    r0
    stw      r0,12(rsp)
    mfxer    r0
    stw      r0,16(rsp)
    mfcr     r0
    stw      r0,20(rsp)
    mflr     r0
    stw      r0,24(rsp)
    stw      r3,40(rsp)
    stw      r4,44(rsp)
    stw      r5,48(rsp)
    stw      r6,52(rsp)
    stw      r7,56(rsp)
    stw      r8,60(rsp)
    stw      r9,64(rsp)
    stw      r10,68(rsp)
    stw      r11,72(rsp)
    stw      r12,76(rsp)
    mfsrr0   r0
    stw      r0,28(rsp)
    mfsrr1   r0
    stw      r0,32(rsp)

    li  r3,0x1300
    lis r4,booke_generic_isr@h
    ori r4,r4,booke_generic_isr@l
    mtlr    r4
    blrl

    lwz      r0,32(rsp)
    mtsrr1   r0
    lwz      r0,28(rsp)
    mtsrr0   r0
    lwz      r3,40(rsp)
    lwz      r4,44(rsp)
    lwz      r5,48(rsp)
    lwz      r6,52(rsp)
    lwz      r7,56(rsp)
    lwz      r8,60(rsp)
    lwz      r9,64(rsp)
    lwz      r10,68(rsp)
    lwz      r11,72(rsp)
    lwz      r12,76(rsp)
    lwz      r0,24(rsp)
    mtlr     r0
    lwz      r0,20(rsp)
    mtcrf    0xff,r0
    lwz      r0,16(rsp)
    mtxer    r0
    lwz      r0,12(rsp)
    mtctr    r0
    lwz      r0,8(rsp)
    addi     rsp,rsp,80
    rfi
    
    /* Core Task Scheduler (CTS) Task Watchdog Timer Interrupt (Offset 0xF0) */
    .align 0x100
cts_irq:
    se_illegal
}

asm void booke_init_interrupt_vector(void)
{
    li      r3, 0x0000                   /* load up 3 with 0x00000000 */
    lis     r3,branch_table@h
    mtspr   IVPR,r3
}

#pragma pop
