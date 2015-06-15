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
 @File          fsl_core_booke_regs.h

 @Description   BOOKE external definitions prototypes
                This file is not included by the BOOKE
                source file as it is an assembly file. It is used
                only for prototypes exposure, for inclusion
                by user and other modules.
*//***************************************************************************/

#ifndef __FSL_CORE_BOOKE_REGS_H
#define __FSL_CORE_BOOKE_REGS_H


/**************************************************************************//**
 @Group         booke_id BOOKE Application Programming Interface

 @Description   BOOKE API functions, definitions and enums

 @{
*//***************************************************************************/

/*
 *
 * Fields in special registers
 * ---------------------------
 */

/* HID0 bits */
#define HID0_EMCP               0x80000000
#define HID0_EMCP_SHIFTED       0x8000
#define HID0_DOZE               0x00800000
#define HID0_NAP                0x00400000
#define HID0_SLEEP              0x00200000
#define HID0_TBEN               0x00004000
#define HID0_SEL_TBCLK          0x00002000
#define HID0_EN_MAS7_UPDATE     0x00000080
#define HID0_DCFA               0x00000040
#define HID0_NOPTI              0x00000001

/* HID1 bits */
#define HID1_ASTME              0x00002000
#define not_HID1_ASTME          0xffffdfff
#define HID1_ABE                0x00001000
#define not_HID1_ABE            0xffffefff


/* TSR bits */
#define TSR_ENW                 0x80000000
#define TSR_WIS                 0x40000000
#define TSR_DIS                 0x08000000
#define TSR_FIS                 0x04000000

/* TCR bits */
#define TCR_DIE                 0x04000000
#define TCR_ARE                 0x00400000

/* L1CSR0/1 bits */
#define L1CSR0_CE               0x00000001
#define not_L1CSR0_CE           ~(L1CSR0_CE)
#define L1CSR0_CFI              0x00000002
#define L1CSR0_CLFR             0x00000100

/* L1CSR1/1 bits */
#define L1CSR1_ICE              0x00000001
#define not_L1CSR1_ICE          ~(L1CSR1_ICE)
#define L1CSR1_ICFI             0x00000002
#define L1CSR1_ICLFR            0x00000100

/* L1CSR2/1 bits */
#define L1CSR2_STASHID          0x000000ff
#define not_L1CSR2_STASHID      ~(L1CSR2_STASHID)

#define L1CSR_UNABLE_TO_LOCK    0x00000400
#define L1CSR_OVER_LOCK         0x00000200
#define L1CSR_ALL_LOCK          0x00000600

/* L2CSR0/1 bits */
#define L2CSR0_L2E              0x80000000      /* cache enable */
#define not_L2CSR0_L2E          ~(L2CSR0_L2E)
#define L2CSR0_L2FI             0x00200000      /* cache flash invalidate */
#define not_L2CSR0_L2FI         ~(L2CSR0_L2FI)
#define L2CSR0_L2IO             0x00100000      /* I-cache only */
#define not_L2CSR0_L2IO         ~(L2CSR0_L2IO)
#define L2CSR0_L2DO             0x00010000      /* D-cache only */
#define not_L2CSR0_L2DO         ~(L2CSR0_L2DO)
#define L2CSR0_L2FL             0x00000800      /* L2 cache flush */
#define not_L2CSR0_L2FL         ~(L2CSR0_L2FL)
#define L2CSR0_L2LFC            0x00000400      /* L2 cache lock flash clear */
#define not_L2CSR0_L2LFC        ~(L2CSR0_L2LFC) /* L2 cache lock flash clear */

/* L2CSR1/1 bits */
#define L2CSR1_STASHID          0x000000ff
#define not_L2CSR1_STASHID      ~(L2CSR1_STASHID)

/*
 *
 * Cache Definitions
 * ------------------------
 */
#define CACHE_NWAYS         128
#define CACHE_NLINES        12
#define L1_CACHE_LINE_SIZE  32


/*
 *
 * Fields in special registers
 * ---------------------------
 */
/* MSR - Machine State Register */
#define MSR_SF          0x8000000000000000 /* 64-Bit Mode */
#define MSR_UCLE        0x4000000
#define MSR_SPE         0x2000000
#define MSR_SPE_HI      0x0200 /* SPE in range of high 16 bits */
#define MSR_WE          0x40000
#define MSR_CE          0x20000
#define MSR_CE_BN       14
#define MSR_EE          0x8000
#define MSR_EE_BN       16
#define MSR_PR          0x4000
#define MSR_FP          0x2000
#define MSR_ME          0x1000
#define MSR_ME_BN       23
#define MSR_FE0         0x800
#define MSR_UBLE        0x400
#define MSR_DE          0x200
#define MSR_FE1         0x100
#define MSR_IS          0x20
#define MSR_DS          0x10
#define MSR_PMM         0x4
#define MSR_RI          0x2
#define MSR_LE          0x1

#define not_MSR_EE      ~(MSR_EE)
#define not_MSR_ME      ~(MSR_ME)
#define not_MSR_WE      ~(MSR_WE)
#define not_MSR_CE      ~(MSR_CE)
#define not_MSR_FP      ~(MSR_FP)
#define not_MSR_IS      ~(MSR_IS)
#define not_MSR_DS      ~(MSR_DS)
#define not_MSR_RI      ~(MSR_RI)
#define not_MSR_DE      ~(MSR_DE)
#define not_MSR_EECE    ~(MSR_EE | MSR_CE)
#define not_MSR_PMM     ~(MSR_PMM)
#define not_MSR_PR      ~(MSR_PR)

/* MMU registers */
/* MAS0 */
#define MAS0_TLBSEL         0x10000000
/* MAS1 */
#define MAS1_TLB_VALID      0x80000000
#define MAS1_IPROT          0x40000000
/* MAS2 */
#define MAS2_M              0x00000004

#define MAS1_TSIZE(x)   (((x) << 7) & 0x00000f80)
#define BOOKE_PAGESZ_1K         0
#define BOOKE_PAGESZ_2K         1
#define BOOKE_PAGESZ_4K         2
#define BOOKE_PAGESZ_8K         3
#define BOOKE_PAGESZ_16K        4
#define BOOKE_PAGESZ_32K        5
#define BOOKE_PAGESZ_64K        6
#define BOOKE_PAGESZ_128K       7
#define BOOKE_PAGESZ_256K       8
#define BOOKE_PAGESZ_512K       9
#define BOOKE_PAGESZ_1M         10
#define BOOKE_PAGESZ_2M         11
#define BOOKE_PAGESZ_4M         12
#define BOOKE_PAGESZ_8M         13
#define BOOKE_PAGESZ_16M        14
#define BOOKE_PAGESZ_32M        15
#define BOOKE_PAGESZ_64M        16
#define BOOKE_PAGESZ_128M       17
#define BOOKE_PAGESZ_256M       18
#define BOOKE_PAGESZ_512M       19
#define BOOKE_PAGESZ_1G         20
#define BOOKE_PAGESZ_2G         21
#define BOOKE_PAGESZ_4G         22
#define BOOKE_PAGESZ_8G         23
#define BOOKE_PAGESZ_16GB       24
#define BOOKE_PAGESZ_32GB       25
#define BOOKE_PAGESZ_64GB       26
#define BOOKE_PAGESZ_128GB      27
#define BOOKE_PAGESZ_256GB      28
#define BOOKE_PAGESZ_512GB      29
#define BOOKE_PAGESZ_1TB        30

/*******************************************************
* interrupt vector
*
* IVORx is loaded with this value
* Also the handler is fixed to this address.
********************************************************/
#define CRITICAL_INTR               0x0100
#define MACHINE_CHECK_INTR          0x0200
#define DATA_STORAGE_INTR           0x0300
#define INSTRUCTION_STORAGE_INTR    0x0400
#define EXTERNAL_INTR               0x0500
#define ALIGNMENT_INTR              0x0600
#define PROGRAM_INTR                0x0700
#define DECREMENTER_INTR            0x0900
#define WATCHDOG_TIMER_INTR         0x0b00
#define SYSTEM_CALL_INTR            0x0c00
#define FIXED_INTERVAL_INTR         0x0f00
#define I_TLB_ERROR_INTR            0x1000
#define D_TLB_ERROR_INTR            0x1100
#define DEBUG_INTR                  0x1500
#define SPE_APU_INTR                0x1600
#define SPE_FLT_DATA_INTR           0x1700
#define SPE_FLT_ROUND_INTR          0x1800
#define PERF_MONITOR_INTR           0x1900
/* Next should be +1 from last Interrupt, for table build */
#define LAST_INTR_INDEX             0x1b00


/*
 *
 * Book E Special-Purpose Registers (by SPR Abbreviation)
 */
#define CSRR0       58      /* Critical save/restore register 0 */
#define CSRR1       59      /* Critical save/restore register 1 */
#define CTR         9       /* Count register */
#define DAC1        316     /* Data address compare 1 */
#define DAC2        317     /* Data address compare 2 */
#define DBCR0       308     /* Debug control register 0 1 */
#define DBCR1       309     /* Debug control register 1 1 */
#define DBCR2       310     /* Debug control register 2 1 */
#define DBSR        304     /* Debug status register */
#define DEAR        61      /* Data exception address register */
#define DEC         22      /* Decrementer */
#define DECAR       54      /* Decrementer auto-reload */
#define ESR         62      /* Exception syndrome register */
#define IAC1        312     /* Instruction address compare 1 */
#define IAC2        313     /* Instruction address compare 2 */
#define IVOR0       400     /* Critical input */
#define IVOR1       401     /* Machine check interrupt offset */
#define IVOR2       402     /* Data storage interrupt offset */
#define IVOR3       403     /* Instruction storage interrupt offset */
#define IVOR4       404     /* External input interrupt offset */
#define IVOR5       405     /* Alignment interrupt offset */
#define IVOR6       406     /* Program interrupt offset */
#define IVOR8       408     /* System call interrupt offset */
#define IVOR10      410     /* Decrementer interrupt offset */
#define IVOR11      411     /* Fixed-interval timer interrupt offset */
#define IVOR12      412     /* Watchdog timer interrupt offset */
#define IVOR13      413     /* Data TLB error interrupt offset */
#define IVOR14      414     /* Instruction TLB error interrupt offset */
#define IVOR15      415     /* Debug interrupt offset */
#define IVOR32      528     /* SPE APU unavailable interrupt offset */
#define IVOR33      529     /* Floating-point data exception interrupt offset */
#define IVOR34      530     /* Floating-point round exception interrupt offset */
#define IVOR35      531     /* Performance monitor */
#define IVPR        63      /* Interrupt vector */
#define LR          8       /* Link register */
#define PID         48      /* Process ID register */
#define PIR         286     /* Processor ID register */
#define TIR         446     /* Thread ID register */
#define TENS        438     /* Thread Enable Register*/
#define TENC        439     /* Thread Enable Clear Register*/
#define TENSR       437     /*Thread Enable Status Register*/
#define PVR         287     /* Processor version register */
#define LPID        338     /* Logical partition ID register */
#define SPRG0       272     /* SPR general 0 */
#define SPRG1       273     /* SPR general 1 */
#define SPRG2       274     /* SPR general 2 */
#define SPRG3       275     /* SPR general 3 */
#define SPRG4       276     /* SPR general 4 */
#define SPRG5       277     /* SPR general 5 */
#define SPRG6       278     /* SPR general 6 */
#define SPRG7       279     /* SPR general 7 */
#define SRR0        26      /* Save/restore register 0 */
#define SRR1        27      /* Save/restore register 1 */
#define TBL         284     /* Time base lower */
#define TBU         285     /* Time base upper */
#define TCR         340     /* Timer control register */
#define TSR         336     /* Timer status register */
#define USPRG0      256     /* User SPR general 06 */
#define XER         1       /* Integer exception register */


/*
 *
 * Implementation-Specific SPRs (by SPR Abbreviation)
 *
 */
#define BBEAR       513     /* Branch buffer entry address register 1 */
#define BBTAR       514     /* Branch buffer target address register 1 */
#define BUCSR       1013    /* Branch unit control and status register 1 */
#define HID0        1008    /* Hardware implementation dependent reg 0 1 */
#define HID1        1009    /* Hardware implementation dependent reg 1 1 */
#define L1CFG0      515     /* L1 cache configuration register 0 */
#define L1CFG1      516     /* L1 cache configuration register 1 */
#define L1CSR0      1010    /* L1 cache control and status register 0 */
#define L1CSR1      1011    /* L1 cache control and status register 1 */
#define L1CSR2      606     /* L1 cache control and status register 2 */
#define L2CFG0      519     /* L2 cache configuration register 0 */
#define L2CSR0      1017    /* L2 cache control and status register 0 */
#define L2CSR1      1018    /* L2 cache control and status register 1 */
#define MAS0        624     /* MMU assist register 0 1 */
#define MAS1        625     /* MMU assist register 1 1 */
#define MAS2        626     /* MMU assist register 2 1 */
#define MAS3        627     /* MMU assist register 3 1 */
#define MAS4        628     /* MMU assist register 4 1 */
#define MAS5        339     /* MMU assist register 5 1 */
#define MAS6        630     /* MMU assist register 6 1 */
#define MAS7        944     /* MMU assist register 7 1 */
#define MAS8        341     /* MMU assist register 8 1 */
#define MCAR        573     /* Machine check address register */
#define MCSR        572     /* Machine check syndrome register */
#define MCSRR0      570     /* Machine check save/restore register 0 */
#define MCSRR1      571     /* Machine check save/restore register 1 */
#define MMUCFG      1015    /* MMU configuration register */
#define MMUCSR0     1012    /* MMU control and status register 0 1 */
#define CTSCSR0     464     /* CTS General Control and Status Registers 0 */
#define CTSTWS      467     /* CTS Task Watchdog Status Register */

#define TLB0CFG     688     /* TLB0CFG register 0. */
#define TLB1CFG     689     /* TLB1CFG register 1. */
#define TLB0PS      344     /* TLB0PS register 0. */
#define TLB1PS      345     /* TLB1PS register 1. */
#define TASKSCR0    476     /* Task Control and Status Register 0 */
#define PID0        48      /* Process ID register 0. */
#define PID1        633     /* Process ID register 1 1 */
#define PID2        634     /* Process ID register 2 1 */
#define SPEFSCR     512     /* Signal processing and embedded floating-point status and control register 1 */
#define SVR         1023    /* System version register */
#define ILLEGAL     333     /* For test only */

/* Hardware Debug Control Registers */
#define HDBCR0      976
#define HDBCR1      977
#define HDBCR2      978
#define HDBCR3      979
#define HDBCR4      980
#define HDBCR5      981
#define HDBCR6      982
#define HDBCR7      631
#define HDBCR8      632

/*
 * PM Implementation-Specific SPRs (by SPR Abbreviation)
 *
 */
#define PMC0        16      /* Performance monitor counter 0 */
#define PMC1        17      /* Performance monitor counter 1 */
#define PMC2        18      /* Performance monitor counter 2 */
#define PMC3        19      /* Performance monitor counter 3 */
#define PML_CA0     144     /* Performance monitor local control a0 */
#define PML_CA1     145     /* Performance monitor local control a1 */
#define PML_CA2     146     /* Performance monitor local control a2 */
#define PML_CA3     147     /* Performance monitor local control a3 */
#define PML_CB0     272     /* Performance monitor local control b0 */
#define PML_CB1     273     /* Performance monitor local control b1 */
#define PML_CB2     274     /* Performance monitor local control b2 */
#define PML_CB3     274     /* Performance monitor local control b3 */
#define PMGC0       400     /* Performance monitor global control 0 */

/** @} */ /* end of booke_grp group */


#endif /* __FSL_CORE_BOOKE_REGS_H */
