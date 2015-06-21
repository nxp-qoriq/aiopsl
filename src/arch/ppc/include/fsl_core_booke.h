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
 @File          fsl_core_booke.h

 @Description   BOOKE external definitions prototypes
                This file is not included by the BOOKE
                source file as it is an assembly file. It is used
                only for prototypes exposure, for inclusion
                by user and other modules.
*//***************************************************************************/

#ifndef __FSL_CORE_BOOKE_H
#define __FSL_CORE_BOOKE_H


#include "common/types.h"

#ifndef CORE_E200_Z490
#define core_dcache_enable      booke_dcache_enable
#endif /* CORE_E200_Z490 */
#define core_icache_enable      booke_icache_enable
#ifndef CORE_E200_Z490
#define core_dcache_disable     booke_dcache_disable
#endif /* CORE_E200_Z490 */
#define core_icache_disable     booke_icache_disable
#define core_get_id             booke_get_id
#define core_test_and_set       booke_test_and_set
#define core_memory_barrier     booke_memory_barrier
#define core_instruction_sync   booke_instruction_sync

#define core_local_irq_save    	booke_local_irq_save
#define core_local_irq_restore	booke_local_irq_restore


/**************************************************************************//**
 @Group         booke_id BOOKE Application Programming Interface

 @Description   BOOKE API functions, definitions and enums

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         booke_init_grp BOOKE Initialization Unit

 @Description   BOOKE initialization unit API functions, definitions and enums

 @{
*//***************************************************************************/

#ifndef CORE_E200_Z490
/**************************************************************************//**
 @Function      booke_dcache_enable

 @Description   Enables the data cache for memory pages that are
                not cache inhibited.

 @Return        None.
*//***************************************************************************/
void booke_dcache_enable(void);

/**************************************************************************//**
 @Function      booke_is_dcache_enabled

 @Description   Checks if data cache is enabled

 @Return        '1'(Enabled)/'0'(Disabled)
*//***************************************************************************/
int booke_is_dcache_enabled(void);
#endif /* CORE_E200_Z490 */

/**************************************************************************//**
 @Function      booke_is_icache_enabled

 @Description   Checks if instraction cache is enabled

 @Return        '1'(Enabled)/'0'(Disabled)
*//***************************************************************************/
int booke_is_icache_enabled(void);

/**************************************************************************//**
 @Function      booke_icache_enable

 @Description   Enables the instruction cache for memory pages that are
                not cache inhibited.

 @Return        None.
*//***************************************************************************/
void booke_icache_enable(void);

#ifndef CORE_E200_Z490
/**************************************************************************//**
 @Function      booke_dcache_disable

 @Description   Disables the data cache.

 @Return        None.
*//***************************************************************************/
void booke_dcache_disable(void);
#endif /* CORE_E200_Z490 */ 

/**************************************************************************//**
 @Function      booke_icache_disable

 @Description   Disables the instruction cache.

 @Return        None.
*//***************************************************************************/
void booke_icache_disable(void);

/**************************************************************************//**
 @Function      booke_dcache_flush

 @Description   Flushes the data cache

 @Return        None.
*//***************************************************************************/
void booke_dcache_flush(void);

/**************************************************************************//**
 @Function      booke_icache_flush

 @Description   Flushes the instruction cache.

 @Return        None.
*//***************************************************************************/
void booke_icache_flush(void);

#ifndef CORE_E200_Z490
/**************************************************************************//**
 @Function      booke_dcache_set_stash_id

 @Description   Set Stash Id for data cache

 @Param[in]     stashId     the stash id to be set.

 @Return        None.
*//***************************************************************************/
void booke_dcache_set_stash_id(uint8_t stash_id);

/**************************************************************************//**
 @Function      booke_dcache_get_stash_id

 @Description   Get Stash Id for data cache

 @Param[in]     none.

 @Return        Stash ID.
*//***************************************************************************/
uint8_t booke_dcache_get_stash_id(void);
#endif /* CORE_E200_Z490 */

/**************************************************************************//**
 @Function      booke_address_bus_streaming_enable

 @Description   Enables address bus streaming on the CCB.

                This setting, along with the ECM streaming configuration
                parameters, enables address bus streaming on the CCB.

 @Return        None.
*//***************************************************************************/
void booke_address_bus_streaming_enable(void);

/**************************************************************************//**
 @Function      booke_address_bus_streaming_disable

 @Description   Disables address bus streaming on the CCB.

 @Return        None.
*//***************************************************************************/
void booke_address_bus_streaming_disable(void);

/**************************************************************************//**
 @Function      booke_address_broadcast_enable

 @Description   Enables address broadcast.

                The booke broadcasts cache management instructions (dcbst, dcblc
                (CT = 1), icblc (CT = 1), dcbf, dcbi, mbar, msync, tlbsync, icbi)
                based on ABE. ABE must be set to allow management of external
                L2 caches.

 @Return        None.
*//***************************************************************************/
void booke_address_broadcast_enable(void);

/**************************************************************************//**
 @Function      booke_address_broadcast_disable

 @Description   Disables address broadcast.

                The booke broadcasts cache management instructions (dcbst, dcblc
                (CT = 1), icblc (CT = 1), dcbf, dcbi, mbar, msync, tlbsync, icbi)
                based on ABE. ABE must be set to allow management of external
                L2 caches.

 @Return        None.
*//***************************************************************************/
void booke_address_broadcast_disable(void);

/**************************************************************************//**
 @Function      booke_is_tasklet_supported

 @Description   Checks if tasklets are supported by the booke interrupt handler.

 @Retval        '1'    - Tasklets are supported.
 @Retval        '0'   - Tasklets are not supported.
*//***************************************************************************/
int booke_is_tasklet_supported(void);

void booke_enable_time_base(void);
void booke_disable_time_base(void);

uint64_t booke_get_time_base_time(void);

void booke_generic_irq_init(void);

/**************************************************************************//**
 @Function      booke_generic_isr

 @Description   This is the general booke interrupt handler.

                It is called by the main assembly interrupt handler
                when an exception occurs and no other function has been
                assigned to this exception.

 @Param         intrEntry   - (In) The exception interrupt vector entry.
*//***************************************************************************/
void booke_generic_isr(uint32_t intr_entry);

/**************************************************************************//**
 @Function      booke_critical_isr

 @Description   This is the specific critical booke interrupt handler.

                It is called by the main assembly interrupt handler
                when a critical interrupt occurs.

 @Param         intrEntry   - (In) The exception interrupt vector entry.
*//***************************************************************************/
void booke_critical_isr(uint32_t intr_entry);

/**************************************************************************//**
 @Function      booke_get_id

 @Description   Returns the core ID in the system.

 @Return        Core ID.
*//***************************************************************************/
uint32_t booke_get_id(void);

/**************************************************************************//**
 @Function      booke_test_and_set

 @Description   This routine tries to atomically test-and-set an integer
                in memory to a non-zero value.

                The memory will be set only if it is tested as zero, in which
                case the routine returns the new non-zero value; otherwise the
                routine returns zero.

 @Param[in]     p - pointer to a volatile int in memory, on which test-and-set
                    operation should be made.

 @Retval        Zero        - Operation failed - memory was already set.
 @Retval        Non-zero    - Operation succeeded - memory has been set.
*//***************************************************************************/
int booke_test_and_set(volatile int *p);

/**************************************************************************//**
 @Function      booke_memory_barrier

 @Description   This routine will cause the core to stop executing any commands
                until all previous memory read/write commands are completely out
                of the core's pipeline.

 @Return        None.
*//***************************************************************************/
static inline void booke_memory_barrier(void)
{
    __asm__ ("msync");
}

/**************************************************************************//**
 @Function      booke_instruction_sync

 @Description   This routine will cause the core to wait for previous instructions
                (including any interrupts they generate) to complete before the
                synchronization command executes, which purges all instructions
                from the processor's pipeline and refetches the next instruction.

 @Return        None.
*//***************************************************************************/
static inline void booke_instruction_sync(void)
{
    __asm__ ("isync");
}

/*****************************************************************************/
static inline uint32_t booke_local_irq_save(void)
{
    uint32_t   temp = 0 ;

    __asm__ ("mfmsr   %0" : "=r" (temp));
    __asm__ ("wrteei   0");                 /* Disable external interrupt */

    return temp;
}

/*****************************************************************************/
static inline void booke_local_irq_restore(uint32_t flags)
{
    __asm__ ("wrtee   %0" : : "r" (flags)); /* Restore external interrupt by flags */
}


/* Layer 1 Cache Manipulations
 *==============================
 * Should not be called directly by the user.
 */
void        l1dcache_invalidate (void);
void        l1icache_invalidate(void);
void        l1dcache_enable(void);
void        l1icache_enable(void);
void        l1dcache_disable(void);
void        l1icache_disable(void);
void        l1dcache_flush(void);
void        l1icache_flush(void);
uint32_t    l1icache_is_enabled(void);
uint32_t    l1dcache_is_enabled(void);
/*
 *
 */
uint32_t    l1dcache_line_lock(uint32_t addr);
uint32_t    l1icache_line_lock(uint32_t addr);
void        l1cache_broad_cast_enable(void);
void        l1cache_broad_cast_disable(void);

/* assembly routines prototypes for user reference only */

uint32_t    get_cr(void);
uint32_t    get_sp(void);
uint32_t    get_current(void);

void        set_msr(uint32_t);
void        set_cr(uint32_t);
void        set_sp(uint32_t);
void        set_current(uint32_t);

/* MSR Manipulations
 *===================
 */
uint32_t    get_msr(void);
void        msr_restore(uint32_t old_msr);
void        msr_restoree_eand_ce(uint32_t old_msr);
void        msr_enable_ee(void);
uint32_t    msr_disable_ee(void);
void        msr_enable_me(void);
void        msr_disable_me(void);
void        msr_enable_we(void);
void        msr_disable_we(void);
void        msr_enable_ce(void);
void        msr_disable_ce(void);
void        msr_enable_fp(void);
void        msr_disable_fp(void);
uint32_t    msr_clear_ee(void);
uint32_t    msr_cleare_eand_ce(void);
void        msr_enable_de(void);
void        msr_disable_de(void);
void        msr_enable_pmm(void);
void        msr_disable_pmm(void);
void        msr_enable_pr(void);
void        msr_disable_pr(void);

uint32_t    L1Ctl_write(uintptr_t address, uint32_t value);


uint32_t    get_dec(void);
uint32_t    get_tcr(void);
uint32_t    get_hid0(void);
uint32_t    get_cpu_id(void);

/************************************************************/
/*  The following routines are inter-modules and are not    */
/*  designed for direct application use. They are used by   */
/*  other modules in the drivers.                           */
/************************************************************/
uint32_t  get_srr0(void);
uint32_t  get_srr1(void);
uint32_t  get_lr(void);


/*
 *
 * Book E Special-Purpose Registers (by SPR Abbreviation)
 */
uint32_t booke_get_spr_CSRR0(void);           /* [58]         Critical save/restore register 0 */
void booke_set_spr_CSRR0(uint32_t newvalue);  /* [58]         Critical save/restore register 0 */
uint32_t booke_get_spr_CSRR1(void);           /* [59]         Critical save/restore register 1 */
void booke_set_spr_CSRR1(uint32_t newvalue);  /* [59]         Critical save/restore register 1 */
uint32_t booke_get_spr_CTR(void);             /* [9]          Count register */
void booke_set_spr_CTR(uint32_t newvalue);    /* [9]          Count register */
uint32_t booke_get_spr_DAC1(void);            /* [316]        Data address compare 1 */
void booke_set_spr_DAC1(uint32_t newvalue);   /* [316]        Data address compare 1 */
uint32_t booke_get_spr_DAC2(void);            /* [317]        Data address compare 2 */
void booke_set_spr_DAC2(uint32_t newvalue);   /* [317]        Data address compare 2 */
uint32_t booke_get_spr_DBCR0(void);           /* [308]        Debug control register 0 1 */
void booke_set_spr_DBCR0(uint32_t newvalue);  /* [308]        Debug control register 0 1 */
uint32_t booke_get_spr_DBCR1(void);           /* [309]        Debug control register 1 1 */
void booke_set_spr_DBCR1(uint32_t newvalue);  /* [309]        Debug control register 1 1 */
uint32_t booke_get_spr_DBCR2(void);           /* [310]        Debug control register 2 1 */
void booke_set_spr_DBCR2(uint32_t newvalue);  /* [310]        Debug control register 2 1 */
uint32_t booke_get_spr_DBSR(void);            /* [304]        Debug status register */
void booke_set_spr_DBSR(uint32_t newvalue);   /* [304]        Debug status register */
uint32_t booke_get_spr_DEAR(void);            /* [61]         Data exception address register */
void booke_set_spr_DEAR(uint32_t newvalue);   /* [61]         Data exception address register */
uint32_t booke_get_spr_DEC(void);             /* [22]         Decrementer */
void booke_set_spr_DEC(uint32_t newvalue);    /* [22]         Decrementer */
uint32_t booke_get_spr_DECAR(void);           /* [54]         Decrementer auto-reload */
void booke_set_spr_DECAR(uint32_t newvalue);  /* [54]         Decrementer auto-reload */
uint32_t booke_get_spr_ESR(void);             /* [62]         Exception syndrome register */
void booke_set_spr_ESR(uint32_t newvalue);    /* [62]         Exception syndrome register */
uint32_t booke_get_spr_IAC1(void);            /* [312]        Instruction address compare 1 */
void booke_set_spr_IAC1(uint32_t newvalue);   /* [312]        Instruction address compare 1 */
uint32_t booke_get_spr_IAC2(void);            /* [313]        Instruction address compare 2 */
void booke_set_spr_IAC2(uint32_t newvalue);   /* [313]        Instruction address compare 2 */
uint32_t booke_get_spr_IVPR(void);            /* [63]         Interrupt vector */
void booke_set_spr_IVPR(uint32_t newvalue);   /* [63]         Interrupt vector */
uint32_t booke_get_spr_LR(void);              /* [8]          Link register */
void booke_set_spr_LR(uint32_t newvalue);     /* [8]          Link register */
uint32_t booke_get_spr_PID(void);             /* [48]         Process ID register 3 */
void booke_set_spr_PID(uint32_t newvalue);    /* [48]         Process ID register 3 */
uint32_t booke_get_spr_PIR(void);             /* [286]        Processor ID register */
void booke_set_spr_PIR(uint32_t newvalue);    /* [286]        Processor ID register */
uint32_t booke_get_spr_PVR(void);             /* [287]        Processor version register */
void booke_set_spr_PVR(uint32_t newvalue);    /* [287]        Processor version register */
uint32_t booke_get_spr_SPRG0(void);           /* [272]        SPR general 0 */
void booke_set_spr_SPRG0(uint32_t newvalue);  /* [272]        SPR general 0 */
uint32_t booke_get_spr_SPRG1(void);           /* [273]        SPR general 1 */
void booke_set_spr_SPRG1(uint32_t newvalue);  /* [273]        SPR general 1 */
uint32_t booke_get_spr_SPRG2(void);           /* [274]        SPR general 2 */
void booke_set_spr_SPRG2(uint32_t newvalue);  /* [274]        SPR general 2 */
uint32_t booke_get_spr_SPRG3(void);           /* [275]        SPR general 3 */
void booke_set_spr_SPRG3(uint32_t newvalue);  /* [275]        SPR general 3 */
uint32_t booke_get_spr_SPRG4(void);           /* [276]        SPR general 4 */
void booke_set_spr_SPRG4(uint32_t newvalue);  /* [276]        SPR general 4 */
uint32_t booke_get_spr_SPRG5(void);           /* [277]        SPR general 5 */
void booke_set_spr_SPRG5(uint32_t newvalue);  /* [277]        SPR general 5 */
uint32_t booke_get_spr_SPRG6(void);           /* [278]        SPR general 6 */
void booke_set_spr_SPRG6(uint32_t newvalue);  /* [278]        SPR general 6 */
uint32_t booke_get_spr_SPRG7(void);           /* [279]        SPR general 7 */
void booke_set_spr_SPRG7(uint32_t newvalue);  /* [279]        SPR general 7 */
uint32_t booke_get_spr_SRR0(void);            /* [26]         Save/restore register 0 */
void booke_set_spr_SRR0(uint32_t newvalue);   /* [26]         Save/restore register 0 */
uint32_t booke_get_spr_SRR1(void);            /* [27]         Save/restore register 1 */
void booke_set_spr_SRR1(uint32_t newvalue);   /* [27]         Save/restore register 1 */
uint32_t booke_get_spr_TBL(void);             /* [284]        Time base lower */
void booke_set_spr_TBL(uint32_t newvalue);    /* [284]        Time base lower */
uint32_t booke_get_spr_TBU(void);             /* [285]        Time base upper */
void booke_set_spr_TBU(uint32_t newvalue);    /* [285]        Time base upper */
uint32_t booke_get_spr_TCR(void);             /* [340]        Timer control register */
void booke_set_spr_TCR(uint32_t newvalue);    /* [340]        Timer control register */
uint32_t booke_get_spr_TSR(void);             /* [336]        Timer status register */
void booke_set_spr_TSR(uint32_t newvalue);    /* [336]        Timer status register */
uint32_t booke_get_spr_USPRG0(void);          /* [256]        User SPR general 06 */
void booke_set_spr_USPRG0(uint32_t newvalue); /* [256]        User SPR general 06 */
uint32_t booke_get_spr_XER(void);             /* [1]          Integer exception register */
void booke_set_spr_XER(uint32_t newvalue);    /* [1]          Integer exception register */


/*
*
* Implementation-Specific SPRs (by SPR Abbreviation)
*
*/

uint32_t booke_get_spr_BBEAR(void);           /* [513]        Branch buffer entry address register 1 */
void booke_set_spr_BBEAR(uint32_t newvalue);  /* [513]        Branch buffer entry address register 1 */
uint32_t booke_get_spr_BBTAR(void);           /* [514]        Branch buffer target address register 1 */
void booke_set_spr_BBTAR(uint32_t newvalue);  /* [514]        Branch buffer target address register 1 */
uint32_t booke_get_spr_BUCSR(void);           /* [1013]       Branch unit control and status register 1 */
void booke_set_spr_BUCSR(uint32_t newvalue);  /* [1013]       Branch unit control and status register 1 */
uint32_t booke_get_spr_HID0(void);            /* [1008]       Hardware implementation dependent reg 0 1 */
void booke_set_spr_HID0(uint32_t newvalue);   /* [1008]       Hardware implementation dependent reg 0 1 */
uint32_t booke_get_spr_HID1(void);            /* [1009]       Hardware implementation dependent reg 1 1 */
void booke_set_spr_HID1(uint32_t newvalue);   /* [1009]       Hardware implementation dependent reg 1 1 */
uint32_t booke_get_spr_L1CFG0(void);          /* [515]        L1 cache configuration register 0 */
void booke_set_spr_L1CFG0(uint32_t newvalue); /* [515]        L1 cache configuration register 0 */
uint32_t booke_get_spr_L1CFG1(void);          /* [516]        L1 cache configuration register 1 */
void booke_set_spr_L1CFG1(uint32_t newvalue); /* [516]        L1 cache configuration register 1 */
uint32_t booke_get_spr_L1CSR0(void);          /* [1010]       L1 cache control and status register 0 1 */
void booke_set_spr_L1CSR0(uint32_t newvalue); /* [1010]       L1 cache control and status register 0 1 */
uint32_t booke_get_spr_L1CSR1(void);          /* [1011]       L1 cache control and status register 1 1 */
void booke_set_spr_L1CSR1(uint32_t newvalue); /* [1011]       L1 cache control and status register 1 1 */
uint32_t booke_get_spr_L1CSR2(void);          /* [606]        L1 cache control and status register 2 1 */
void booke_set_spr_L1CSR2(uint32_t newvalue); /* [606]        L1 cache control and status register 2 1 */

uint32_t booke_get_spr_MAS0(void);            /* [624]        MMU assist register 0 1 */
void booke_set_spr_MAS0(uint32_t newvalue);   /* [624]        MMU assist register 0 1 */
uint32_t booke_get_spr_MAS1(void);            /* [625]        MMU assist register 1 1 */
void booke_set_spr_MAS1(uint32_t newvalue);   /* [625]        MMU assist register 1 1 */
uint32_t booke_get_spr_MAS2(void);            /* [626]        MMU assist register 2 1 */
void booke_set_spr_MAS2(uint32_t newvalue);   /* [626]        MMU assist register 2 1 */
uint32_t booke_get_spr_MAS3(void);            /* [627]        MMU assist register 3 1 */
void booke_set_spr_MAS3(uint32_t newvalue);   /* [627]        MMU assist register 3 1 */
uint32_t booke_get_spr_MAS4(void);            /* [628]        MMU assist register 4 1 */
void booke_set_spr_MAS4(uint32_t newvalue);   /* [628]        MMU assist register 4 1 */
uint32_t booke_get_spr_MAS6(void);            /* [630]        MMU assist register 6 1 */
void booke_set_spr_MAS6(uint32_t newvalue);   /* [630]        MMU assist register 6 1 */
uint32_t booke_get_spr_MAS7(void);            /* [944]        MMU assist register 7 1 */
void booke_set_spr_MAS7(uint32_t newvalue);   /* [944]        MMU assist register 7 1 */
uint32_t booke_get_spr_MCAR(void);            /* [573]        Machine check address register */
void booke_set_spr_MCAR(uint32_t newvalue);   /* [573]        Machine check address register */
uint32_t booke_get_spr_MCSR(void);            /* [572]        Machine check syndrome register */
void booke_set_spr_MCSR(uint32_t newvalue);   /* [572]        Machine check syndrome register */
uint32_t booke_get_spr_MCSRR0(void);          /* [570]        Machine check save/restore register 0 */
void booke_set_spr_MCSRR0(uint32_t newvalue); /* [570]        Machine check save/restore register 0 */
uint32_t booke_get_spr_MCSRR1(void);          /* [571]        Machine check save/restore register 1 */
void booke_set_spr_MCSRR1(uint32_t newvalue); /* [571]        Machine check save/restore register 1 */
uint32_t booke_get_spr_MMUCFG(void);          /* [1015]       MMU configuration register */
void booke_set_spr_MMUCFG(uint32_t newvalue); /* [1015]       MMU configuration register */
uint32_t booke_get_spr_MMUCSR0(void);         /* [1012]       MMU control and status register 0 1 */
void booke_set_spr_MMUCSR0(uint32_t newvalue);/* [1012]       MMU control and status register 0 1 */

uint32_t booke_get_spr_TLB0CFG(void);         /* [688]        TLB0CFG register 0. */
uint32_t booke_get_spr_TLB1CFG(void);         /* [689]        TLB1CFG register 1. */

uint32_t booke_get_spr_TLB0PS(void);          /* [344]        TLB0PS register 0. */
uint32_t booke_get_spr_TLB1PS(void);          /* [345]        TLB1PS register 1. */

uint32_t booke_get_spr_PID0(void);            /* [48]         Process ID register 0. */
void booke_set_spr_PID0(uint32_t newvalue);   /* [48]         Process ID register 0. */
uint32_t booke_get_spr_PID1(void);            /* [633]        Process ID register 1 1 */
void booke_set_spr_PID1(uint32_t newvalue);   /* [633]        Process ID register 1 1 */
uint32_t booke_get_spr_PID2(void);            /* [634]        Process ID register 2 1 */
void booke_set_spr_PID2(uint32_t newvalue);   /* [634]        Process ID register 2 1 */
uint32_t booke_get_spr_SPEFSCR(void);         /* [512]        Signal processing and embedded floating-point status and control register 1 */
void booke_set_spr_SPEFSCR(uint32_t newvalue);/* [512]        Signal processing and embedded floating-point status and control register 1 */
uint32_t booke_get_spr_SVR(void);             /* [1023]       System version register */
void booke_set_spr_SVR(uint32_t newvalue);    /* [1023]       System version register */
uint32_t booke_get_spr_ILLEGAL(void);         /* [999]        System version register */
void booke_set_spr_ILLEGAL(uint32_t newvalue);/* [999]        System version register */

uint32_t booke_get_spr_HDBCR0(void);
void booke_set_spr_HDBCR0(uint32_t newvalue);
uint32_t booke_get_spr_HDBCR2(void);
void booke_set_spr_HDBCR2(uint32_t newvalue);
uint32_t booke_get_spr_HDBCR7(void);
void booke_set_spr_HDBCR7(uint32_t newvalue);

#ifdef CORE_E200
/* E200-AIOP special regs */
// Number of tasks as they defined by CTSCSR register.
#define CTSCSR_ENABLE 		(0x80000000)
#define CTSCSR_1_TASKS 		(0x00000000)  
#define CTSCSR_2_TASKS 		(0x01000000)
#define CTSCSR_4_TASKS 		(0x02000000)
#define CTSCSR_8_TASKS 		(0x03000000)
#define CTSCSR_16_TASKS 	(0x04000000)
#define CTSCSR_TASKS_MASK 	(0x0f000000)

uint32_t booke_get_CTSCSR0(void);           /* [464]  CTS gen control and status reg 0. */
void booke_set_CTSCSR0(uint32_t newvalue);  /* [464]  CTS gen control and status reg 0. */
uint32_t booke_get_CTSTWS(void);            /* [467]  CTS Task Watchdog Status Register. */
void booke_set_CTSTWS(uint32_t newvalue);   /* [467]  CTS Task Watchdog Status Register. */
uint32_t booke_get_TASKSCR0(void);          /* [476]  Task Control and Status Register 0 */

#endif /* CORE_E200 */

/** @} */ /* end of booke_init_grp group */
/** @} */ /* end of booke_grp group */


#endif /* __FSL_CORE_BOOKE_H */
