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


#include "include/fsl_core_booke_regs.h"
#include "booke.h"


/* .section .text ,4,1,6 - align (4) ,progbits ,flags (6) */
#if defined(__MWERKS__) && !defined(__GNUC__)
.section .text,4,"x"
#else
.section .text,"ax",@progbits
#endif /* defined(__MWERKS__) && ... */
.align 2


/*****************************************************************************
    MSR Manipulation
*****************************************************************************/
.global asm_prefix(get_msr)
.global asm_prefix(msr_restore)
.global asm_prefix(msr_enable_ee)
.global asm_prefix(msr_disable_ee)
.global asm_prefix(msr_clear_ee)
.global asm_prefix(msr_enable_me)
.global asm_prefix(msr_disable_me)
.global asm_prefix(msr_enable_ce)
.global asm_prefix(msr_disable_ce)
.global asm_prefix(msr_enable_fp)
.global asm_prefix(msr_disable_fp)
.global asm_prefix(msr_cleare_eand_ce)
.global asm_prefix(msr_enable_de)
.global asm_prefix(msr_disable_de)
.global asm_prefix(msr_enable_pmm)
.global asm_prefix(msr_disable_pmm)
.global asm_prefix(msr_restoree_eand_ce)
.global asm_prefix(msr_enable_pr)
.global asm_prefix(msr_disable_pr)

/* Threads Management for dual-thread cores*/
.global asm_prefix(booke_disable_secondary_threads)
.global asm_prefix(booke_enable_secondary_threads)

/*  Clear second thread in TENC */
/*  void booke_disable_secondary_threads(uint32_t secondaryThreadsMask) */
asm_prefix(booke_disable_secondary_threads):
        mtspr   TENC, 3
1:
        mfspr       4, TENSR
        and.        4, 4, 3
        bne         1b
        isync
        blr

/*  Turn on second thread in TENC */
/*  void booke_enable_secondary_threads(uint32_t secondaryThreadsMask) */
asm_prefix(booke_enable_secondary_threads):
        mtspr   TENS, 3
        isync
        blr

/*  Get MSR */
/*  uint32_t get_msr(void) */
asm_prefix(get_msr):
    mfmsr       3
    blr

/*  Restore MSR */
/*  void msr_restore(uint32_t msr) */
asm_prefix(msr_restore):
    msync
    mtmsr       3
    isync
    blr

/*  Restore MSR EE and CE bits */
/*  void MSR_RestoreEEandCE(uint32_t msr) */
asm_prefix(msr_restoree_eand_ce):
    rlwinm      3, 3, 0, MSR_CE_BN,MSR_EE_BN    /* create a mask from the EE and CE bits */
    rlwinm      3, 3, 0, MSR_EE_BN,MSR_CE_BN
    mfmsr       4                               /* Read MSR */
    or          4, 3, 4                         /* Set CE and EE based on mask */
    mtmsr       4                               /* Write to MSR */
    blr

/*  Set EE bit in the MSR */
/*  void MSR_EnableEE(void) */
asm_prefix(msr_enable_ee):
    mfmsr       3
    ori         4, 3, MSR_EE
    mtmsr       4
    blr

/*  Clear EE bit in MSR and return old MSR value */
/*  unsigned int MSR_DisableEE(void) */
asm_prefix(msr_disable_ee):
asm_prefix(msr_clear_ee):
    mfmsr       3
    addis       4, 0, not_MSR_EE@h      /* load not_MSR_EE mask to 4 */
    ori         4, 4, not_MSR_EE@l
    and         4, 3, 4
    mtmsr       4
    blr

/*  Clear EE and CE bits in MSR and return old MSR value */
/*  unsigned int MSR_ClearEEandCE(void) */
asm_prefix(msr_cleare_eand_ce):
    mfmsr       3
    addis       4, 0, not_MSR_EECE@h    /* load not_MSR_EECE mask to 4 */
    ori         4, 4, not_MSR_EECE@l
    and         4, 3, 4
    mtmsr       4
    blr

/*  Set ME bit in the MSR */
/*  void MSR_EnableME(void) */
asm_prefix(msr_enable_me):
    mfspr       3, HID0                 /* Read HID0 */
    oris        3, 3, HID0_EMCP_SHIFTED /* Set EMCP */
    isync
    mtspr       HID0,3                  /* Write HID0 */
    isync

    mfmsr       3
    ori         4, 3, MSR_ME
    mtmsr       4
    isync
    msync
    blr

/*  Clear ME bit in the MSR */
/*  void MSR_DisableME(void) */
asm_prefix(msr_disable_me):
    mfmsr       3
    addis       4, 0, not_MSR_ME@h      /* load not_MSR_ME mask to 4 */
    ori         4, 4, not_MSR_ME@l
    and         4, 3, 4
    mtmsr       4
    isync
    msync
    blr

/*  Set CE bit in the MSR */
/*  void MSR_EnableCE(void) */
asm_prefix(msr_enable_ce):
    mfmsr       3
    addis       4, 0, MSR_CE@h          /* load MSR_CE mask to 4 */
    ori         4, 4, MSR_CE@l
    or          4, 3, 4
    mtmsr       4
    blr

/*  Clear CE bit in the MSR */
/*  void MSR_DisableCE(void) */
asm_prefix(msr_disable_ce):
    mfmsr       3
    addis       4, 0, not_MSR_CE@h      /* load not_MSR_CE mask to 4 */
    ori         4, 4, not_MSR_CE@l
    and         4, 3, 4
    mtmsr       4
    blr

/*  Set FP bit in the MSR */
/*  void MSR_EnableFP(void) */
asm_prefix(msr_enable_fp):
    mfmsr       3
    addis       4, 0, MSR_FP@h          /* load MSR_FP mask to 4 */
    ori         4, 4, MSR_FP@l
    or          4, 3, 4
    mtmsr       4
    blr

/*  Clear FP bit in the MSR */
/*  void MSR_DisableFP(void) */
asm_prefix(msr_disable_fp):
    mfmsr       3
    addis       4, 0, not_MSR_FP@h      /* load not_MSR_FP mask to 4 */
    ori         4, 4, not_MSR_FP@l
    and         4, 3, 4
    mtmsr       4
    blr

/*  Set DE bit in the MSR */
/*  void MSR_EnableDE(void) */
asm_prefix(msr_enable_de):
    mfmsr       3
    addis       4, 0, MSR_DE@h          /* load MSR_DE mask to 4 */
    ori         4, 4, MSR_DE@l
    or          4, 3, 4
    mtmsr       4
    isync
    msync
    blr

/*  Clear DE bit in the MSR */
/*  void MSR_DisableDE(void) */
asm_prefix(msr_disable_de):
    mfmsr       3
    addis       4, 0, not_MSR_DE@h      /* load not_MSR_CE mask to 4 */
    ori         4, 4, not_MSR_DE@l
    and         4, 3, 4
    mtmsr       4
    isync
    msync
    blr

/*  Set PMM bit in the MSR */
/*  void MSR_EnablePMM(void) */
asm_prefix(msr_enable_pmm):
    isync
    msync
    mfmsr       3
    addis       4, 0, MSR_PMM@h         /* load MSR_PMM mask to 4 */
    ori         4, 4, MSR_PMM@l
    or          4, 3, 4
    mtmsr       4
    isync
    msync
    blr

/*  Clear PMM bit in the MSR */
/*  void MSR_DisablePMM(void) */
asm_prefix(msr_disable_pmm):
    mfmsr       3
    addis       4, 0, not_MSR_PMM@h     /* load not_MSR_PMM mask to 4 */
    ori         4, 4, not_MSR_PMM@l
    and         4, 3, 4
    mtmsr       4
    isync
    msync
    blr

/*  void MSR_DisablePR(void) */
asm_prefix(msr_disable_pr):
    mfmsr       3
    addis       4, 0, not_MSR_PR@h     /* load not_MSR_PR mask to 4 */
    ori         4, 4, not_MSR_PR@l
    and         4, 3, 4
    mtmsr       4
    isync
    msync
    blr

/*  void MSR_EnablePR(void) */
asm_prefix(msr_enable_pr):
    mfmsr       3
    addis       4, 0, MSR_PR@h          /* load MSR_PR mask to 4 */
    ori         4, 4, MSR_PR@l
    or          4, 3, 4
    mtmsr       4
    isync
    msync
    blr

/*****************************************************************************
    L1 Cache manipulation
*****************************************************************************/
.global asm_prefix(l1dcache_enable),        asm_prefix(l1dcache_disable)
.global asm_prefix(l1icache_enable),        asm_prefix(l1icache_disable)
.global asm_prefix(l1dcache_invalidate),    asm_prefix(l1icache_invalidate)
.global asm_prefix(l1dcache_flush),         asm_prefix(l1icache_flush)
.global asm_prefix(l1icache_is_enabled),         asm_prefix(l1icache_is_enabled)
.global asm_prefix(l1dcache_is_enabled),         asm_prefix(l1dcache_is_enabled)

.global asm_prefix(l1dcache_line_lock),      asm_prefix(l1dcache_line_un_lock)
.global asm_prefix(l1icache_line_lock),      asm_prefix(l1icache_line_un_lock)
.global asm_prefix(l1dcache_lock),          asm_prefix(l1icache_lock)
.global asm_prefix(l1cache_broad_cast_enable),asm_prefix(l1cache_broad_cast_disable)


/* Data cache enable */
asm_prefix(l1dcache_enable):
    msync
    isync
    mfspr       3, L1CSR0
    ori         3, 3, L1CSR0_CE       /* set enable bit at L1CSR0 */
    mtspr       L1CSR0, 3
    isync
    blr

/* Data cache disable */
asm_prefix(l1dcache_disable):
    msync
    isync
    mfspr       3, L1CSR0
    addis       4, 0, not_L1CSR0_CE@h   /* load not_MSR_EECE mask to 4 */
    ori         4, 4, not_L1CSR0_CE@l
    and         3, 3, 4
    mtspr       L1CSR0, 3
    isync
    blr

/* Is Instruction cache enabled? */
asm_prefix(l1icache_is_enabled):
    mfspr       3, L1CSR1
    andi. 		3, 3, L1CSR1_ICE
    isync
    msync
    blr

/* Is Data cache enabled? */
asm_prefix(l1dcache_is_enabled):
    mfspr       3, L1CSR0
    andi. 		3, 3, L1CSR0_CE
    isync
    msync
    blr

/* Instruction cache enable */
asm_prefix(l1icache_enable):
    mfspr       3, L1CSR1
    ori         3, 3, L1CSR1_ICE      /* set enable bit at L1CSR1 */
    mtspr       L1CSR1, 3
    isync
    msync
    blr

/* Instruction cache disable */
asm_prefix(l1icache_disable):
    mfspr       3, L1CSR1
    addis       4, 0, not_L1CSR1_ICE@h
    ori         4, 4, not_L1CSR1_ICE@l
    and         3, 3, 4
    mtspr       L1CSR1, 3
    isync
    blr

/* Data cache invalidate */
asm_prefix(l1dcache_invalidate):
    msync
    isync
    mfspr       3, L1CSR0
    ori         3, 3, L1CSR0_CFI       /* set invalidate bit at L1CSR0 */
    mtspr       L1CSR0, 3
1:
    mfspr       3, L1CSR0
    andi.       3, 3, L1CSR0_CFI
    bne         1b
    isync
    blr

/* Instruction cache invalidate */
asm_prefix(l1icache_invalidate):
    isync
    mfspr       3, L1CSR1
    ori         3, 3, L1CSR1_ICFI      /* set invalidate bit at L1CSR1 */
    mtspr       L1CSR1, 3
1:
    mfspr       3, L1CSR1
    andi.       3, 3, L1CSR1_ICFI
    bne         1b
    isync
    blr

/* Data Cache Flush */
asm_prefix(l1dcache_flush):
    li          4, (2 * CACHE_NWAYS * CACHE_NLINES)
    mtctr       4
    lis         5, 0@h
l1dcache_flush_l1:
    lwz         3, 0(5)               /* Load one word from every line */
    addi        5, 5, L1_CACHE_LINE_SIZE
    bdnz        l1dcache_flush_l1
    blr

/* Instruction Cache Flush */
asm_prefix(l1icache_flush):
    mfspr       3,L1CSR1                       /* Get L1CSR1 */
    ori         3,3,L1CSR1_ICFI|L1CSR1_ICLFR  /* Set the Flahs bits */
    mtspr       L1CSR1,3                       /* Dp The Flash */
    isync                                       /* Sync Required */
    blr

/* Enable broadcast */
asm_prefix(l1cache_broad_cast_enable):
    mfspr       3,HID1
    ori         3, 3, HID1_ABE            /* set Broadcast enable bit at HID1 */
    msync
    mtspr       HID1,3
    isync
    blr

/* Disable broadcast */
asm_prefix(l1cache_broad_cast_disable):
    mfspr       3, HID1
    addis       4, 0, not_HID1_ABE@h        /* clear Broadcast enable bit at HID1 */
    ori         4, 4, not_HID1_ABE@l
    and         3, 3, 4

/*  andi.       3, 3, not_HID1_ABE */     /* clear Broadcast enable bit at HID1 */
    msync
    mtspr       HID1,3
    isync
    blr


/* Data cache single line lock */
/* 3 must contain the data address as passed by user.
   3 returns 0 on success, or lock failure bits. */
asm_prefix(l1dcache_line_lock):
    dcbtls      0, 0, 3       /* lock line in L1 */
    msync
/*    mfL1CSR0    3*/              /* check if cache is full */

    addis       4, 0, L1CSR_ALL_LOCK@h
    ori         4, 4, L1CSR_ALL_LOCK@l
    and         3, 3, 4

    cmplwi      3, 0
    bne         d_lock_failed
    blr                         /* return 3 = 0 */
d_lock_failed:
    not         4, 3
/*    mfL1CSR0    5*/
    and.        4, 4, 5
/*    mtL1CSR0    4*/              /* clear sticky bits of lock failure */
    blr                         /* return - 3 contains lock failure bits */


/* Data cache unlock a single line */
/* 3 must contain the data address to be locked, as passed by user */
asm_prefix(l1dcache_line_unlock):
    dcblc       0, 0, 3       /* lock line in L1 */
    msync
    blr


/* Instruction cache single line lock */
/* 3 must contain the data address as passed by user.
   3 returns 0 on success, or lock failure bits. */
asm_prefix(l1icache_line_lock):
    icbtls      0, 0, 3       /* lock line in L1 */
    msync
    mfspr       3, L1CSR1      /* check if cache is full */
    addis       4, 0, L1CSR_ALL_LOCK@h
    ori         4, 4, L1CSR_ALL_LOCK@l
    and         3, 3, 4

    cmplwi      3, 0
    bne         i_lock_failed
    blr                         /* return 3 = 0 */
i_lock_failed:
    not         4, 3
    mfspr       5, L1CSR1
    and.        4, 4, 5
    mtspr       L1CSR1, 4      /* clear sticky bits of lock failure */
    msync
    blr                         /* return - 3 contains lock failure bits */

/* Instruction cache unlock a single line */
/* 3 must contain the instruction address */
asm_prefix(l1icache_line_unlock):
    icblc       0, 0, 3       /* lock line in L1  */
    msync
    blr


/*****************************************************************************
    MMU Manipulation
*****************************************************************************/
.global asm_prefix(booke_tlb_write)
.global asm_prefix(booke_tlb_write5)
.global asm_prefix(booke_tlb_read)
.global asm_prefix(booke_tlb_read5)
.global asm_prefix(booke_tlb1_invalidate)

/* TLB entry configuration */
/* Used by C routine MMU_TlbEntryWrite, mmu.c.
   User must pass 3 (MAS0), 4 (MAS1), 5 (MAS2), and 6 (MAS3) values. */
asm_prefix(booke_tlb_write):
    /* configure masX registers values */
    mtspr       MAS0, 3
    isync
    mtspr       MAS1, 4
    isync
    mtspr       MAS2, 5
    isync
    mtspr       MAS3, 6
    tlbwe
    isync
    blr

/* Used by C routine MMU_TlbEntryWrite, mmu.c.
   User must pass 3 (MAS0), 4 (MAS1), 5 (MAS2), and 6 (MAS3) 7 (MAS7) values. */
asm_prefix(booke_tlb_write5):
    /* configure masX registers values */
    mtspr       MAS0, 3
    isync
    mtspr       MAS1, 4
    isync
    mtspr       MAS2, 5
    isync
    mtspr       MAS3, 6
    isync
    mtspr       MAS7, 7
    tlbwe
    isync
    blr

/* TLB entry read */
/* Used by C routine MMU_TlbEntryWrite, mmu.c.
   User must pass 3 (by address) 4 (MAS0) and 5 (MAS2).
   Routine uses 3 to return an array of the required TLB entry
   parameters as defined at the masX registers. */
asm_prefix(booke_tlb_read):
    /* set MAS0 and MAS2 to define the specified TLB entry */
    mtspr       MAS0, 4
    isync
    mtspr       MAS2, 5
    isync
    tlbre
    /* store in the address speified by 3 the TLB parameters */
    mfspr       4, MAS0
    isync
    stw         4, 0(3)
    mfspr       4, MAS1
    isync
    stw         4, 4(3)
    mfspr       4, MAS2
    isync
    stw         4, 8(3)
    mfspr       4, MAS3
    isync
    stw         4, 12(3)
    isync
    blr

asm_prefix(booke_tlb_read5):
    /* set MAS0 and MAS2 to define the specified TLB entry */
    mtspr       MAS0, 4
    isync
    mtspr       MAS2, 5
    isync
    tlbre
    /* store in the address speified by 3 the TLB parameters */
    mfspr       4, MAS0
    isync
    stw         4, 0(3)
    mfspr       4, MAS1
    isync
    stw         4, 4(3)
    mfspr       4, MAS2
    isync
    stw         4, 8(3)
    mfspr       4, MAS3
    isync
    stw         4, 12(3)
    mfspr       4, MAS7
    isync
    stw         4, 16(3)
    isync
    blr

/* TLB1 Invalidate */
/* Invalidates all entries in TLB1 table. */
asm_prefix(booke_tlb1_invalidate):
    li          3, 0x0c
    tlbivax     0,3
    isync
    blr


/*****************************************************************************
    Miscellaneous
*****************************************************************************/
.global asm_prefix(get_dec)
.global asm_prefix(get_tcr)
.global asm_prefix(get_hid0)
.global asm_prefix(get_cpu_id)

/* Get DEC */
/* void GetDec(void) */
asm_prefix(get_dec):
    mfdec       3
    blr

/* Get TCR */
/* void GetTcr(void) */
asm_prefix(get_tcr):
    mfspr       3, TCR
    blr

/* Get HID0 */
/* void GetHid0(void) */
asm_prefix(get_hid0):
    mfspr       3, HID0
    blr

/* Get CPU Id */
/* uint32_t GetCpuId(void) */
asm_prefix(get_cpu_id):
    mfpir       3
    blr


/* Enable broadcast */
.global    asm_prefix(booke_address_bus_streaming_enable)
asm_prefix(booke_address_bus_streaming_enable):
    mfspr       3,HID1
    ori         3, 3, HID1_ASTME
    msync
    mtspr       HID1,3
    isync
    blr

/* Enable address bus streaming */
.global    asm_prefix(booke_address_broadcast_enable)
asm_prefix(booke_address_broadcast_enable):
    mfspr       3,HID1
    ori         3, 3, HID1_ABE
    msync
    mtspr       HID1,3
    isync
    blr

/* Disable broadcast */
.global    asm_prefix(booke_address_broadcast_disable)
asm_prefix(booke_address_broadcast_disable):
    mfspr       3, HID1
    addis       4, 0, not_HID1_ABE@h
    ori         4, 4, not_HID1_ABE@l
    and         3, 3, 4
    msync
    mtspr       HID1,3
    isync
    blr

/* Check if tasklets are supported by the BOOKE interrupt handler */
.global asm_prefix(booke_is_tasklet_supported)
asm_prefix(booke_is_tasklet_supported):
#ifndef DISABLE_TASKLETS
    li          3, 1
#else  /* DISABLE_TASKLETS */
    li          3, 0
#endif /* DISABLE_TASKLETS */
    blr

.global asm_prefix(booke_test_and_set)
asm_prefix(booke_test_and_set):
    li      4, 1            /* prepare non-zero value */
tas_loop:
    lwarx   5, 0, 3         /* load and reserve */
    cmpwi   5, 0            /* check loaded value */
    bne-    tas_end         /* not equal to 0 - already set */
    stwcx.  4, 0, 3         /* try to store non-zero */
    bne-    tas_loop        /* lost reservation */
    li      3, 1            /* return new (non-zero) value */
    blr
tas_end:
    li      3, 0            /* return previous (zero) value */
    blr



/*****************************************************************************
    SPRS Manipulation

    The SPRs functions where build by a macro definitions based on the SPRs
    table in the user manual.
*****************************************************************************/

/* Book E Special-Purpose Registers (by SPR Abbreviation) */

.global    asm_prefix(booke_get_spr_CSRR0)
asm_prefix(booke_get_spr_CSRR0):
    mfspr   3,CSRR0         /* 58 */    /* Critical save/restore register 0 */
    blr
.global    asm_prefix(booke_set_spr_CSRR0)
asm_prefix(booke_set_spr_CSRR0):
    mtspr   CSRR0,3         /* 58 */    /* Critical save/restore register 0 */
    blr

.global    asm_prefix(booke_get_spr_CSRR1)
asm_prefix(booke_get_spr_CSRR1):
    mfspr   3,CSRR1         /* 59 */    /* Critical save/restore register 1 */
    blr
.global    asm_prefix(booke_set_spr_CSRR1)
asm_prefix(booke_set_spr_CSRR1):
    mtspr   CSRR1,3         /* 59 */    /* Critical save/restore register 1 */
    blr

.global    asm_prefix(booke_get_spr_CTR)
asm_prefix(booke_get_spr_CTR):
    mfspr   3,CTR           /* 9 */     /* Count register */
    blr
.global    asm_prefix(booke_set_spr_CTR)
asm_prefix(booke_set_spr_CTR):
    mtspr   CTR,3           /* 9 */     /* Count register */
    blr

.global    asm_prefix(booke_get_spr_DAC1)
asm_prefix(booke_get_spr_DAC1):
    mfspr   3,DAC1          /* 316 */   /* Data address compare 1 */
    blr
.global    asm_prefix(booke_set_spr_DAC1)
asm_prefix(booke_set_spr_DAC1):
    mtspr   DAC1,3          /* 316 */   /* Data address compare 1 */
    blr

.global    asm_prefix(booke_get_spr_DAC2)
asm_prefix(booke_get_spr_DAC2):
    mfspr   3,DAC2          /* 317 */   /* Data address compare 2 */
    blr
.global    asm_prefix(booke_set_spr_DAC2)
asm_prefix(booke_set_spr_DAC2):
    mtspr   DAC2,3          /* 317 */   /* Data address compare 2 */
    blr

.global    asm_prefix(booke_get_spr_DBCR0)
asm_prefix(booke_get_spr_DBCR0):
    mfspr   3,DBCR0         /* 308 */   /* Debug control register 0 1 */
    blr
.global    asm_prefix(booke_set_spr_DBCR0)
asm_prefix(booke_set_spr_DBCR0):
    mtspr   DBCR0,3         /* 308 */   /* Debug control register 0 1 */
    blr

.global    asm_prefix(booke_get_spr_DBCR1)
asm_prefix(booke_get_spr_DBCR1):
    mfspr   3,DBCR1         /* 309 */   /* Debug control register 1 1 */
    blr
.global    asm_prefix(booke_set_spr_DBCR1)
asm_prefix(booke_set_spr_DBCR1):
    mtspr   DBCR1,3         /* 309 */   /* Debug control register 1 1 */
    blr

.global    asm_prefix(booke_get_spr_DBCR2)
asm_prefix(booke_get_spr_DBCR2):
    mfspr   3,DBCR2         /* 310 */   /* Debug control register 2 1 */
    blr
.global    asm_prefix(booke_set_spr_DBCR2)
asm_prefix(booke_set_spr_DBCR2):
    mtspr   DBCR2,3         /* 310 */   /* Debug control register 2 1 */
    blr

.global    asm_prefix(booke_get_spr_DBSR)
asm_prefix(booke_get_spr_DBSR):
    mfspr   3,DBSR          /* 304 */   /* Debug status register */
    blr
.global    asm_prefix(booke_set_spr_DBSR)
asm_prefix(booke_set_spr_DBSR):
    mtspr   DBSR,3          /* 304 */   /* Debug status register */
    blr

.global    asm_prefix(booke_get_spr_DEAR)
asm_prefix(booke_get_spr_DEAR):
    mfspr   3,DEAR          /* 61 */    /* Data exception address register */
    blr
.global    asm_prefix(booke_set_spr_DEAR)
asm_prefix(booke_set_spr_DEAR):
    mtspr   DEAR,3          /* 61 */    /* Data exception address register */
    blr

.global    asm_prefix(booke_get_spr_DEC)
asm_prefix(booke_get_spr_DEC):
    mfspr   3,DEC           /* 22 */    /* Decrementer */
    blr
.global    asm_prefix(booke_set_spr_DEC)
asm_prefix(booke_set_spr_DEC):
    mtspr   DEC,3           /* 22 */    /* Decrementer */
    blr

.global    asm_prefix(booke_get_spr_DECAR)
asm_prefix(booke_get_spr_DECAR):
    mfspr   3,DECAR         /* 54 */    /* Decrementer auto-reload */
    blr
.global    asm_prefix(booke_set_spr_DECAR)
asm_prefix(booke_set_spr_DECAR):
    mtspr   DECAR,3         /* 54 */    /* Decrementer auto-reload */
    blr

.global    asm_prefix(booke_get_spr_ESR)
asm_prefix(booke_get_spr_ESR):
    mfspr   3,ESR           /* 62 */    /* Exception syndrome register */
    blr
.global    asm_prefix(booke_set_spr_ESR)
asm_prefix(booke_set_spr_ESR):
    mtspr   ESR,3           /* 62 */    /* Exception syndrome register */
    blr

.global    asm_prefix(booke_get_spr_IAC1)
asm_prefix(booke_get_spr_IAC1):
    mfspr   3,IAC1          /* 312 */   /* Instruction address compare 1 */
    blr
.global    asm_prefix(booke_set_spr_IAC1)
asm_prefix(booke_set_spr_IAC1):
    mtspr   IAC1,3          /* 312 */   /* Instruction address compare 1 */
    blr

.global    asm_prefix(booke_get_spr_IAC2)
asm_prefix(booke_get_spr_IAC2):
    mfspr   3,IAC2          /* 313 */   /* Instruction address compare 2 */
    blr
.global    asm_prefix(booke_set_spr_IAC2)
asm_prefix(booke_set_spr_IAC2):
    mtspr   IAC2,3          /* 313 */   /* Instruction address compare 2 */
    blr

.global    asm_prefix(booke_get_spr_IVPR)
asm_prefix(booke_get_spr_IVPR):
    mfspr   3,IVPR          /* 63 */    /* Interrupt vector */
    blr
.global    asm_prefix(booke_set_spr_IVPR)
asm_prefix(booke_set_spr_IVPR):
    mtspr   IVPR,3          /* 63 */    /* Interrupt vector */
    blr

.global    asm_prefix(booke_get_spr_LR)
asm_prefix(booke_get_spr_LR):
    mfspr   3,LR            /* 8 */     /* Link register */
    blr
.global    asm_prefix(booke_set_spr_LR)
asm_prefix(booke_set_spr_LR):
    mtspr   LR,3            /* 8 */     /* Link register */
    blr

.global    asm_prefix(booke_get_spr_PID)
asm_prefix(booke_get_spr_PID):
    mfspr   3,PID           /* 48 */    /* Process ID register 3 */
    blr
.global    asm_prefix(booke_set_spr_PID)
asm_prefix(booke_set_spr_PID):
    mtspr   PID,3           /* 48 */    /* Process ID register 3 */
    blr

.global    asm_prefix(booke_get_spr_PIR)
asm_prefix(booke_get_spr_PIR):
    mfspr   3,PIR           /* 286 */   /* Processor ID register */
    blr
.global    asm_prefix(booke_set_spr_PIR)
asm_prefix(booke_set_spr_PIR):
    mtspr   PIR,3           /* 286 */   /* Processor ID register */
    blr

.global    asm_prefix(booke_get_spr_PVR)
asm_prefix(booke_get_spr_PVR):
    mfspr   3,PVR           /* 287 */   /* Processor version register */
    blr
.global    asm_prefix(booke_set_spr_PVR)
asm_prefix(booke_set_spr_PVR):
    mtspr   PVR,3           /* 287 */   /* Processor version register */
    blr

.global    asm_prefix(booke_get_spr_SPRG0)
asm_prefix(booke_get_spr_SPRG0):
    mfspr   3,SPRG0         /* 272 */   /* SPR general 0 */
    blr
.global    asm_prefix(booke_set_spr_SPRG0)
asm_prefix(booke_set_spr_SPRG0):
    mtspr   SPRG0,3         /* 272 */   /* SPR general 0 */
    blr

.global    asm_prefix(booke_get_spr_SPRG1)
asm_prefix(booke_get_spr_SPRG1):
    mfspr   3,SPRG1         /* 273 */   /* SPR general 1 */
    blr
.global    asm_prefix(booke_set_spr_SPRG1)
asm_prefix(booke_set_spr_SPRG1):
    mtspr   SPRG1,3         /* 273 */   /* SPR general 1 */
    blr

.global    asm_prefix(booke_get_spr_SPRG2)
asm_prefix(booke_get_spr_SPRG2):
    mfspr   3,SPRG2         /* 274 */   /* SPR general 2 */
    blr
.global    asm_prefix(booke_set_spr_SPRG2)
asm_prefix(booke_set_spr_SPRG2):
    mtspr   SPRG2,3         /* 274 */   /* SPR general 2 */
    blr

.global    asm_prefix(booke_get_spr_SPRG3)
asm_prefix(booke_get_spr_SPRG3):
    mfspr   3,SPRG3         /* 275 */   /* SPR general 3 */
    blr
.global    asm_prefix(booke_set_spr_SPRG3)
asm_prefix(booke_set_spr_SPRG3):
    mtspr   SPRG3,3         /* 275 */   /* SPR general 3 */
    blr

.global    asm_prefix(booke_get_spr_SPRG4)
asm_prefix(booke_get_spr_SPRG4):
    mfspr   3,SPRG4         /* 276 */   /* SPR general 4 */
    blr
.global    asm_prefix(booke_set_spr_SPRG4)
asm_prefix(booke_set_spr_SPRG4):
    mtspr   SPRG4,3         /* 276 */   /* SPR general 4 */
    blr

.global    asm_prefix(booke_get_spr_SPRG5)
asm_prefix(booke_get_spr_SPRG5):
    mfspr   3,SPRG5         /* 277 */   /* SPR general 5 */
    blr
.global    asm_prefix(booke_set_spr_SPRG5)
asm_prefix(booke_set_spr_SPRG5):
    mtspr   SPRG5,3         /* 277 */   /* SPR general 5 */
    blr

.global    asm_prefix(booke_get_spr_SPRG6)
asm_prefix(booke_get_spr_SPRG6):
    mfspr   3,SPRG6         /* 278 */   /* SPR general 6 */
    blr
.global    asm_prefix(booke_set_spr_SPRG6)
asm_prefix(booke_set_spr_SPRG6):
    mtspr   SPRG6,3         /* 278 */   /* SPR general 6 */
    blr

.global    asm_prefix(booke_get_spr_SPRG7)
asm_prefix(booke_get_spr_SPRG7):
    mfspr   3,SPRG7         /* 279 */   /* SPR general 7 */
    blr
.global    asm_prefix(booke_set_spr_SPRG7)
asm_prefix(booke_set_spr_SPRG7):
    mtspr   SPRG7,3         /* 279 */   /* SPR general 7 */
    blr

.global    asm_prefix(booke_get_spr_SRR0)
asm_prefix(booke_get_spr_SRR0):
    mfspr   3,SRR0          /* 26 */    /* Save/restore register 0 */
    blr
.global    asm_prefix(booke_set_spr_SRR0)
asm_prefix(booke_set_spr_SRR0):
    mtspr   SRR0,3          /* 26 */    /* Save/restore register 0 */
    blr

.global    asm_prefix(booke_get_spr_SRR1)
asm_prefix(booke_get_spr_SRR1):
    mfspr   3,SRR1          /* 27 */    /* Save/restore register 1 */
    blr
.global    asm_prefix(booke_set_spr_SRR1)
asm_prefix(booke_set_spr_SRR1):
    mtspr   SRR1,3          /* 27 */    /* Save/restore register 1 */
    blr

.global    asm_prefix(booke_get_spr_TBL)
asm_prefix(booke_get_spr_TBL):
    mfspr   3,TBL           /* 284 */   /* Time base lower */
    blr
.global    asm_prefix(booke_set_spr_TBL)
asm_prefix(booke_set_spr_TBL):
    mtspr   TBL,3           /* 284 */   /* Time base lower */
    blr

.global    asm_prefix(booke_get_spr_TBU)
asm_prefix(booke_get_spr_TBU):
    mfspr   3,TBU           /* 285 */   /* Time base upper */
    blr
.global    asm_prefix(booke_set_spr_TBU)
asm_prefix(booke_set_spr_TBU):
    mtspr   TBU,3           /* 285 */   /* Time base upper */
    blr

.global    asm_prefix(booke_get_spr_TCR)
asm_prefix(booke_get_spr_TCR):
    mfspr   3,TCR           /* 340 */   /* Timer control register */
    blr
.global    asm_prefix(booke_set_spr_TCR)
asm_prefix(booke_set_spr_TCR):
    mtspr   TCR,3           /* 340 */   /* Timer control register */
    blr

.global    asm_prefix(booke_get_spr_TSR)
asm_prefix(booke_get_spr_TSR):
    mfspr   3,TSR           /* 336 */   /* Timer status register */
    blr
.global    asm_prefix(booke_set_spr_TSR)
asm_prefix(booke_set_spr_TSR):
    mtspr   TSR,3           /* 336 */   /* Timer status register */
    blr

.global    asm_prefix(booke_get_spr_USPRG0)
asm_prefix(booke_get_spr_USPRG0):
    mfspr   3,USPRG0        /* 256 */   /* User SPR general 06 */
    blr
.global    asm_prefix(booke_set_spr_USPRG0)
asm_prefix(booke_set_spr_USPRG0):
    mtspr   USPRG0,3        /* 256 */   /* User SPR general 06 */
    blr

.global    asm_prefix(booke_get_spr_XER)
asm_prefix(booke_get_spr_XER):
    mfspr   3,XER           /* 1 */     /* Integer exception register */
    blr
.global    asm_prefix(booke_set_spr_XER)
asm_prefix(booke_set_spr_XER):
    mtspr   XER,3           /* 1 */     /* Integer exception register */
    blr


/* Implementation-Specific SPRs (by SPR Abbreviation) */

.global    asm_prefix(booke_get_spr_BBEAR)
asm_prefix(booke_get_spr_BBEAR):
    mfspr   3,BBEAR         /* 513 */   /* Branch buffer entry address register 1 */
    blr
.global    asm_prefix(booke_set_spr_BBEAR)
asm_prefix(booke_set_spr_BBEAR):
    mtspr   BBEAR,3         /* 513 */   /* Branch buffer entry address register 1 */
    blr

.global    asm_prefix(booke_get_spr_BBTAR)
asm_prefix(booke_get_spr_BBTAR):
    mfspr   3,BBTAR         /* 514 */   /* Branch buffer target address register 1 */
    blr
.global    asm_prefix(booke_set_spr_BBTAR)
asm_prefix(booke_set_spr_BBTAR):
    mtspr   BBTAR,3         /* 514 */   /* Branch buffer target address register 1 */
    blr

.global    asm_prefix(booke_get_spr_BUCSR)
asm_prefix(booke_get_spr_BUCSR):
    mfspr   3,BUCSR         /* 1013 */  /* Branch unit control and status register 1 */
    blr
.global    asm_prefix(booke_set_spr_BUCSR)
asm_prefix(booke_set_spr_BUCSR):
    mtspr   BUCSR,3         /* 1013 */  /* Branch unit control and status register 1 */
    blr

.global    asm_prefix(booke_get_spr_HID0)
asm_prefix(booke_get_spr_HID0):
    mfspr   3,HID0          /* 1008 */  /* Hardware implementation dependent reg 0 1 */
    blr
.global    asm_prefix(booke_set_spr_HID0)
asm_prefix(booke_set_spr_HID0):
    isync
    mtspr   HID0,3          /* 1008 */  /* Hardware implementation dependent reg 0 1 */
    isync
    blr

.global    asm_prefix(booke_get_spr_HID1)
asm_prefix(booke_get_spr_HID1):
    mfspr   3,HID1          /* 1009 */  /* Hardware implementation dependent reg 1 1 */
    blr
.global    asm_prefix(booke_set_spr_HID1)
asm_prefix(booke_set_spr_HID1):
    msync
    mtspr   HID1,3          /* 1009 */  /* Hardware implementation dependent reg 1 1 */
    isync
    blr

.global    asm_prefix(booke_get_spr_L1CFG0)
asm_prefix(booke_get_spr_L1CFG0):
    mfspr   3,L1CFG0        /* 515 */   /* L1 cache configuration register 0 */
    blr
.global    asm_prefix(booke_set_spr_L1CFG0)
asm_prefix(booke_set_spr_L1CFG0):
    mtspr   L1CFG0,3        /* 515 */   /* L1 cache configuration register 0 */
    blr

.global    asm_prefix(booke_get_spr_L1CFG1)
asm_prefix(booke_get_spr_L1CFG1):
    mfspr   3,L1CFG1        /* 516 */   /* L1 cache configuration register 1 */
    blr
.global    asm_prefix(booke_set_spr_L1CFG1)
asm_prefix(booke_set_spr_L1CFG1):
    mtspr   L1CFG1,3        /* 516 */   /* L1 cache configuration register 1 */
    blr

.global    asm_prefix(booke_get_spr_L1CSR0)
asm_prefix(booke_get_spr_L1CSR0):
    mfspr   3,L1CSR0        /* 1010 */  /* L1 cache control and status register 0 1 */
    blr
.global    asm_prefix(booke_set_spr_L1CSR0)
asm_prefix(booke_set_spr_L1CSR0):
    mtspr   L1CSR0,3        /* 1010 */  /* L1 cache control and status register 0 1 */
    blr

.global    asm_prefix(booke_get_spr_L1CSR1)
asm_prefix(booke_get_spr_L1CSR1):
    mfspr   3,L1CSR1        /* 1011 */  /* L1 cache control and status register 1 1 */
    blr
.global    asm_prefix(booke_set_spr_L1CSR1)
asm_prefix(booke_set_spr_L1CSR1):
    mtspr   L1CSR1,3        /* 1011 */  /* L1 cache control and status register 1 1 */
    blr

.global    asm_prefix(booke_get_spr_L1CSR2)
asm_prefix(booke_get_spr_L1CSR2):
    mfspr   3,L1CSR2        /* 606 */  /* L1 cache control and status register 2 1 */
    blr
.global    asm_prefix(booke_set_spr_L1CSR2)
asm_prefix(booke_set_spr_L1CSR2):
    isync
    mtspr   L1CSR2,3        /* 606 */  /* L1 cache control and status register 2 1 */
    isync
    msync
    blr

.global    asm_prefix(booke_get_spr_MAS0)
asm_prefix(booke_get_spr_MAS0):
    mfspr   3,MAS0          /* 624 */   /* MMU assist register 0 1 */
    blr
.global    asm_prefix(booke_set_spr_MAS0)
asm_prefix(booke_set_spr_MAS0):
    mtspr   MAS0,3          /* 624 */   /* MMU assist register 0 1 */
    blr

.global    asm_prefix(booke_get_spr_MAS1)
asm_prefix(booke_get_spr_MAS1):
    mfspr   3,MAS1          /* 625 */   /* MMU assist register 1 1 */
    blr
.global    asm_prefix(booke_set_spr_MAS1)
asm_prefix(booke_set_spr_MAS1):
    mtspr   MAS1,3          /* 625 */   /* MMU assist register 1 1 */
    blr

.global    asm_prefix(booke_get_spr_MAS2)
asm_prefix(booke_get_spr_MAS2):
    mfspr   3,MAS2          /* 626 */   /* MMU assist register 2 1 */
    blr
.global    asm_prefix(booke_set_spr_MAS2)
asm_prefix(booke_set_spr_MAS2):
    mtspr   MAS2,3          /* 626 */   /* MMU assist register 2 1 */
    blr

.global    asm_prefix(booke_get_spr_MAS3)
asm_prefix(booke_get_spr_MAS3):
    mfspr   3,MAS3          /* 627 */   /* MMU assist register 3 1 */
    blr
.global    asm_prefix(booke_set_spr_MAS3)
asm_prefix(booke_set_spr_MAS3):
    mtspr   MAS3,3          /* 627 */   /* MMU assist register 3 1 */
    blr

.global    asm_prefix(booke_get_spr_MAS4)
asm_prefix(booke_get_spr_MAS4):
    mfspr   3,MAS4          /* 628 */   /* MMU assist register 4 1 */
    blr
.global    asm_prefix(booke_set_spr_MAS4)
asm_prefix(booke_set_spr_MAS4):
    mtspr   MAS4,3          /* 628 */   /* MMU assist register 4 1 */
    blr

.global    asm_prefix(booke_get_spr_MAS6)
asm_prefix(booke_get_spr_MAS6):
    mfspr   3,MAS6          /* 630 */   /* MMU assist register 6 1 */
    blr
.global    asm_prefix(booke_set_spr_MAS6)
asm_prefix(booke_set_spr_MAS6):
    mtspr   MAS6,3          /* 630 */   /* MMU assist register 6 1 */
    blr

.global    asm_prefix(booke_get_spr_MAS7)
asm_prefix(booke_get_spr_MAS7):
    mfspr   3,MAS7          /* 944 */   /* MMU assist register 7 1 */
    blr
.global    asm_prefix(booke_set_spr_MAS7)
asm_prefix(booke_set_spr_MAS7):
    mtspr   MAS7,3          /* 944 */   /* MMU assist register 7 1 */
    blr

.global    asm_prefix(booke_get_spr_MCAR)
asm_prefix(booke_get_spr_MCAR):
    mfspr   3,MCAR          /* 573 */   /* Machine check address register */
    blr
.global    asm_prefix(booke_set_spr_MCAR)
asm_prefix(booke_set_spr_MCAR):
    mtspr   MCAR,3          /* 573 */   /* Machine check address register */
    blr

.global    asm_prefix(booke_get_spr_MCSR)
asm_prefix(booke_get_spr_MCSR):
    mfspr   3,MCSR          /* 572 */   /* Machine check syndrome register */
    blr
.global    asm_prefix(booke_set_spr_MCSR)
asm_prefix(booke_set_spr_MCSR):
    mtspr   MCSR,3          /* 572 */   /* Machine check syndrome register */
    blr

.global    asm_prefix(booke_get_spr_MCSRR0)
asm_prefix(booke_get_spr_MCSRR0):
    mfspr   3,MCSRR0        /* 570 */   /* Machine check save/restore register 0 */
    blr
.global    asm_prefix(booke_set_spr_MCSRR0)
asm_prefix(booke_set_spr_MCSRR0):
    mtspr   MCSRR0,3        /* 570 */   /* Machine check save/restore register 0 */
    blr

.global    asm_prefix(booke_get_spr_MCSRR1)
asm_prefix(booke_get_spr_MCSRR1):
    mfspr   3,MCSRR1        /* 571 */   /* Machine check save/restore register 1 */
    blr
.global    asm_prefix(booke_set_spr_MCSRR1)
asm_prefix(booke_set_spr_MCSRR1):
    mtspr   MCSRR1,3        /* 571 */   /* Machine check save/restore register 1 */
    blr

.global    asm_prefix(booke_get_spr_MMUCFG)
asm_prefix(booke_get_spr_MMUCFG):
    mfspr   3,MMUCFG        /* 1015 */  /* MMU configuration register */
    blr
.global    asm_prefix(booke_set_spr_MMUCFG)
asm_prefix(booke_set_spr_MMUCFG):
    mtspr   MMUCFG,3        /* 1015 */  /* MMU configuration register */
    blr

.global    asm_prefix(booke_get_spr_MMUCSR0)
asm_prefix(booke_get_spr_MMUCSR0):
    mfspr   3,MMUCSR0       /* 1012 */  /* MMU control and status register 0 1 */
    blr
.global    asm_prefix(booke_set_spr_MMUCSR0)
asm_prefix(booke_set_spr_MMUCSR0):
    mtspr   MMUCSR0,3       /* 1012 */  /* MMU control and status register 0 1 */
    blr



/* #define PID 48 */        /* Book E refers to this as instead of PID0 */
.global    asm_prefix(booke_get_spr_PID0)
asm_prefix(booke_get_spr_PID0):
    mfspr   3,PID0          /* 48 */    /* Process ID register 0. */
    blr
.global    asm_prefix(booke_set_spr_PID0)
asm_prefix(booke_set_spr_PID0):
    mtspr   PID0,3          /* 48 */    /* Process ID register 0. */
    blr

.global    asm_prefix(booke_get_spr_PID1)
asm_prefix(booke_get_spr_PID1):
    mfspr   3,PID1          /* 633 */   /* Process ID register 1 1 */
    blr
.global    asm_prefix(booke_set_spr_PID1)
asm_prefix(booke_set_spr_PID1):
    mtspr   PID1,3          /* 633 */   /* Process ID register 1 1 */
    blr

.global    asm_prefix(booke_get_spr_PID2)
asm_prefix(booke_get_spr_PID2):
    mfspr   3,PID2          /* 634 */   /* Process ID register 2 1 */
    blr
.global    asm_prefix(booke_set_spr_PID2)
asm_prefix(booke_set_spr_PID2):
    mtspr   PID2,3          /* 634 */   /* Process ID register 2 1 */
    blr


.global    asm_prefix(booke_get_spr_TLB0CFG)
asm_prefix(booke_get_spr_TLB0CFG):
    mfspr   3,TLB0CFG       /* 688 */   /* TLB0CFG */
    blr

.global    asm_prefix(booke_get_spr_TLB1CFG)
asm_prefix(booke_get_spr_TLB1CFG):
    mfspr   3,TLB1CFG       /* 689 */   /* TLB1CFG */
    blr

.global    asm_prefix(booke_get_spr_TLB0PS)
asm_prefix(booke_get_spr_TLB0PS):
    mfspr   3,TLB0PS       /* 344 */   /* TLB0PS */
    blr

.global    asm_prefix(booke_get_spr_TLB1PS)
asm_prefix(booke_get_spr_TLB1PS):
    mfspr   3,TLB1PS       /* 345 */   /* TLB1PS */
    blr

.global    asm_prefix(booke_get_spr_SPEFSCR)
asm_prefix(booke_get_spr_SPEFSCR):
    mfspr   3,SPEFSCR       /* 512 */   /* Signal processing and embedded floating-point status and control register 1 */
    blr
.global    asm_prefix(booke_set_spr_SPEFSCR)
asm_prefix(booke_set_spr_SPEFSCR):
    mtspr   SPEFSCR,3       /* 512 */   /* Signal processing and embedded floating-point status and control register 1 */
    blr

.global    asm_prefix(booke_get_spr_SVR)
asm_prefix(booke_get_spr_SVR):
    mfspr   3,SVR           /* 1023 */  /* System version register */
    blr
.global    asm_prefix(booke_set_spr_SVR)
asm_prefix(booke_set_spr_SVR):
    mtspr   SVR,3           /* 1023 */  /* System version register */
    blr

.global    asm_prefix(booke_get_spr_HDBCR0)
asm_prefix(booke_get_spr_HDBCR0):
    mfspr   3, HDBCR0
    blr

.global    asm_prefix(booke_set_spr_HDBCR0)
asm_prefix(booke_set_spr_HDBCR0):
    mtspr	HDBCR0, 3
    blr

.global    asm_prefix(booke_get_spr_HDBCR2)
asm_prefix(booke_get_spr_HDBCR2):
    mfspr   3, HDBCR2
    blr

.global    asm_prefix(booke_set_spr_HDBCR2)
asm_prefix(booke_set_spr_HDBCR2):
    mtspr	HDBCR2, 3
    blr

.global    asm_prefix(booke_get_spr_HDBCR7)
asm_prefix(booke_get_spr_HDBCR7):
    mfspr   3, HDBCR7
    blr

.global    asm_prefix(booke_set_spr_HDBCR7)
asm_prefix(booke_set_spr_HDBCR7):
    mtspr	HDBCR7, 3
    isync
    blr

/*****************************************************************************
    Performance Monitoring
*****************************************************************************/

.global asm_prefix(booke_get_pmc0)
asm_prefix(booke_get_pmc0):
    mfpmr   3,PMC0
    blr
.global asm_prefix(booke_set_pmc0)
asm_prefix(booke_set_pmc0):
    mtpmr   PMC0,3
    blr
.global asm_prefix(booke_get_pmc1)
asm_prefix(booke_get_pmc1):
    mfpmr   3,PMC1
    blr
.global asm_prefix(booke_set_pmc1)
asm_prefix(booke_set_pmc1):
    mtpmr   PMC1,3
    blr
.global asm_prefix(booke_get_pmc2)
asm_prefix(booke_get_pmc2):
    mfpmr   3,PMC2
    blr
.global asm_prefix(booke_set_pmc2)
asm_prefix(booke_set_pmc2):
    mtpmr   PMC2,3
    blr
.global asm_prefix(booke_get_pmc3)
asm_prefix(booke_get_pmc3):
    mfpmr   3,PMC3
    blr
.global asm_prefix(booke_set_pmc3)
asm_prefix(booke_set_pmc3):
    mtpmr   PMC3,3
    blr
.global asm_prefix(booke_get_pml_ca0)
asm_prefix(booke_get_pml_ca0):
    mfpmr   3,PML_CA0
    blr
.global asm_prefix(booke_set_pml_ca0)
asm_prefix(booke_set_pml_ca0):
    mtpmr   PML_CA0,3
    blr
.global asm_prefix(booke_get_pml_ca1)
asm_prefix(booke_get_pml_ca1):
    mfpmr   3,PML_CA1
    blr
.global asm_prefix(booke_set_pml_ca1)
asm_prefix(booke_set_pml_ca1):
    mtpmr   PML_CA1,3
    blr
.global asm_prefix(booke_get_pml_ca2)
asm_prefix(booke_get_pml_ca2):
    mfpmr   3,PML_CA2
    blr
.global asm_prefix(booke_set_pml_ca2)
asm_prefix(booke_set_pml_ca2):
    mtpmr   PML_CA2,3
    blr
.global asm_prefix(booke_get_pml_ca3)
asm_prefix(booke_get_pml_ca3):
    mfpmr   3,PML_CA3
    blr
.global asm_prefix(booke_set_pml_ca3)
asm_prefix(booke_set_pml_ca3):
    mtpmr   PML_CA3,3
    blr
.global asm_prefix(booke_get_pml_cb0)
asm_prefix(booke_get_pml_cb0):
    mfpmr   3,PML_CB0
    blr
.global asm_prefix(booke_set_pml_cb0)
asm_prefix(booke_set_pml_cb0):
    mtpmr   PML_CB0,3
    blr
.global asm_prefix(booke_get_pml_cb1)
asm_prefix(booke_get_pml_cb1):
    mfpmr   3,PML_CB1
    blr
.global asm_prefix(booke_set_pml_cb1)
asm_prefix(booke_set_pml_cb1):
    mtpmr   PML_CB1,3
    blr
.global asm_prefix(booke_get_pml_cb2)
asm_prefix(booke_get_pml_cb2):
    mfpmr   3,PML_CB2
    blr
.global asm_prefix(booke_set_pml_cb2)
asm_prefix(booke_set_pml_cb2):
    mtpmr   PML_CB2,3
    blr
.global asm_prefix(booke_get_pml_cb3)
asm_prefix(booke_get_pml_cb3):
    mfpmr   3,PML_CB3
    blr
.global asm_prefix(booke_set_pml_cb3)
asm_prefix(booke_set_pml_cb3):
    mtpmr   PML_CB3,3
    blr
.global asm_prefix(booke_get_pmgc0)
asm_prefix(booke_get_pmgc0):
    mfpmr   3,PMGC0
    blr
.global asm_prefix(booke_set_pmgc0)
asm_prefix(booke_set_pmgc0):
    mtpmr   PMGC0,3
    blr


/*****************************************************************************
    Test functions
*****************************************************************************/
.global asm_prefix(booke_test_SC)
asm_prefix(booke_test_SC):
    sc
    blr


.global asm_prefix(booke_get_spr_ILLEGAL)
asm_prefix(booke_get_spr_ILLEGAL):
    mfspr   3,ILLEGAL       /* 512 */   /* Signal processing and embedded floating-point status and control register 1 */
    blr

.global asm_prefix(booke_set_spr_ILLEGAL)
asm_prefix(booke_set_spr_ILLEGAL):
    mtspr   ILLEGAL,3       /* 512 */   /* Signal processing and embedded floating-point status and control register 1 */
    blr

/* E200-AIOP special regs */
.global asm_prefix(booke_get_CTSCSR0)
asm_prefix(booke_get_CTSCSR0):
    mfdcr   3,CTSCSR0
    blr
.global asm_prefix(booke_set_CTSCSR0)
asm_prefix(booke_set_CTSCSR0):
    mtdcr   CTSCSR0,3
    blr
.global asm_prefix(booke_get_CTSTWS)
asm_prefix(booke_get_CTSTWS):
    mfdcr   3,CTSTWS
    blr
.global asm_prefix(booke_set_CTSTWS)
asm_prefix(booke_set_CTSTWS):
    mtdcr   CTSTWS,3
    blr
.global asm_prefix(booke_get_TASKSCR0)
asm_prefix(booke_get_TASKSCR0):
    mfdcr   3,TASKSCR0
    blr
