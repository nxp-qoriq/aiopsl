#include "arch/cores/fsl_core_booke_regs.h"


.extern __arena_start;

.global __arena_start_secondary

#if defined(__MWERKS__) && !defined(__GNUC__)
.section .newstart,4,"x"
#else
.section .newstart,"ax",@progbits
#endif /* defined(__MWERKS__) && ... */
.align 4

__arena_start_secondary:

1:
    /*1. Save input parameters to pass them to start later */
    mr      14, 3
    mr      15, 4
    mr      16, 5

	/* Reseting MSR register */
	li      6, 0
	lis		6, 0x9000
	ori		6, 6, 0x4030
	mfmsr	7
	and 	7, 6, 7
	mtmsr 	7
	/* Save input parameters to pass them to start later */
	mr      14, 3
	mr      15, 4
	mr      16, 5

    /* 1. Find the index of the entry we're executing in */
    bl      1f                          /* Find our address */
1:
    mflr    6                           /* Make it accessible */
    mfmsr   7
    li      4, 0
    li      8, 0
    rlwinm  4, 7, 27, 31, 31            /* Extract MSR[IS] */
    rlwinm  8, 7, 3, 0, 0               /* Extract MSR[GS] */
    mfpir   7
    slwi    7, 7, 16
    or      7, 7, 4
    mtspr   MAS6, 7
    mfspr   7, LPID
    andi.   7, 7, 0x003f
    or      7, 7, 8
    mtspr   MAS5, 7
    isync
    tlbsx   0, 6                        /* Search for our address TLB */
    isync
    mfspr   7, MAS1
    andis.  7, 7, MAS1_TLB_VALID@h

match_TLB:                              /* We had to match */
    mfspr   7, MAS0
    li      3, 0
    rlwinm  3, 7, 16, 20, 31            /* r3 = index of found entry */

    /* 2. Invalidate all entries except the entry we're executing in */
    mfspr   9, TLB1CFG
    andi.   9, 9, 0x0fff
    li      6, 0                        /* Set Entry counter to 0 */

    /* Invalidate TLB1 */
1:
    lis     7, MAS0_TLBSEL@h            /* Set MAS0(TLBSEL) = 1 */
    rlwimi  7, 6, 16, 4, 15             /* Setup MAS0 = TLBSEL | ESEL(r6) */
    mtspr   MAS0, 7
    isync
    tlbre
    isync
    mfspr   7, MAS1
    rlwinm  7, 7, 0, 2, 31              /* Clear MAS1 Valid and IPROT */
    cmpw    3, 6                        /* Check if that's running entry */
    beq     skip_inv                     /* Don't update the current execution TLB */
    mtspr   MAS1, 7
    isync
    tlbwe
    isync
skip_inv:
    addi    6, 6, 1                     /* Increment */
    cmpw    6, 9                        /* Are we done? */
    bne     1b                          /* If not, repeat */

    /* 3. Setup a temp mapping and jump to it */
    andi.   5, 3, 0x1                   /* Find an entry not used and is non-zero */
    addi    5, 5, 0x1                   /* r5 = either 1 or 2 */
    lis     7, MAS0_TLBSEL@h            /* Set MAS0(TLBSEL) = 1 */
    rlwimi  7, 3, 16, 4, 15             /* Setup MAS0 = TLBSEL | ESEL(r3) */
    mtspr   MAS0, 7
    isync
    tlbre
    isync

    /* Prepare temporary mapping for the same phy and virtual pages.
       Use different address space so be able to jump to the new mapping. */
    lis     7, MAS0_TLBSEL@h            /* Set MAS0(TLBSEL) = 1 */
    rlwimi  7, 5, 16, 4, 15             /* Setup MAS0 = TLBSEL | ESEL(r5) */
    mtspr   MAS0, 7
    mfspr   7, MAS1
    andi.    7, 7, 0x0f00               /* Leave/mask TSIZE bits only */
    xori    6, 4, 1                     /* Use other address space (AS) */
    slwi    6, 6, 12
    oris    6, 6, MAS1_TLB_VALID@h
    or      7, 7, 6                     /* Set AS + add Valid bit if not already there */
    mtspr   MAS1, 7
     /*3. Update WIMGE 00100*/
    mfspr   7, MAS2
    li      6, 0x1f            /*Set WIMGE mask*/
    andc    7, 7, 6          /* Clear WIMGE bits */
    ori     7, 7, 0x4         /*WIMGE = b00100*/
    mtspr   MAS2, 7
    isync
    tlbwe
    isync

    xori    6, 4, 1                     /* Address space to jump to */
    slwi    6, 6, 5                     /* Prepare for MSR[IS] bit */
    bl      1f                          /* Find our address */
1:
    mflr    7
    addi    7, 7, (2f - 1b)
    mtspr   SRR0, 7
    mtspr   SRR1, 6
    rfi                                 /* Jump to label 2f */

    /* 4. Invalidate mapping we started in */
2:
    lis     7, MAS0_TLBSEL@h            /* Set MAS0(TLBSEL) = 1 */
    rlwimi  7, 3, 16, 4, 15             /* Setup MAS0 = TLBSEL | ESEL(r3) */
    mtspr   MAS0, 7
    isync
    tlbre
    isync
    mfspr   6, MAS1
    rlwinm  6, 6, 0, 2, 31              /* Clear Valid and IPROT */
    mtspr   MAS1, 6
    isync
    tlbwe
    isync

    /* 5. Setup initial 1G mapping and jump to it */
    lis     7, MAS0_TLBSEL@h            /* Set MAS0(TLBSEL) = 1 */
    rlwimi  7, 5, 16, 4, 15             /* Setup MAS0 = TLBSEL | ESEL(r5) */
    mtspr   MAS0, 7
    isync
    tlbre
    isync

    li      3, 3                        /* r5 is either 1 or 2 so set up entry 3 */
    lis     7, MAS0_TLBSEL@h            /* Set MAS0(TLBSEL) = 1 */
    rlwimi  7, 3, 16, 4, 15             /* Setup MAS0 = TLBSEL | ESEL(r3) */
    mtspr   MAS0, 7
    mr      6, 4                        /* Use correct/original address space (AS) */
    slwi    6, 6, 12
    oris    6, 6, MAS1_TLB_VALID@h      /* Don't set IPROT so this mapping can be
                                           invalidated later by the SW */
    ori     6, 6, 0x0a00                /* MAS1[TSIZE] = 1G */
    mtspr   MAS1, 6
    mfspr   6, MAS2
    ori     6, 6, MAS2_M                /* Insure memory coherency (M bit) is set */
    mtspr   MAS2, 6
    isync                               /* EPN and RPN are the same */
    tlbwe
    isync

    mr      6, 4                        /* Address space to jump to */
    slwi    6, 6, 5                     /* Prepare for MSR[IS] bit */
    bl      1f                          /* Find our address */
1:
    mflr    7
    addi    7, 7, (2f - 1b)
    mtspr   SRR0, 7
    mtspr   SRR1, 6
    rfi                                 /* Jump to label 2f */

    /* 6. Invalidate the temp mapping */
2:
    lis     7, MAS0_TLBSEL@h            /* Set MAS0(TLBSEL) = 1 */
    rlwimi  7, 5, 16, 4, 15             /* Setup MAS0 = TLBSEL | ESEL(r5) */
    mtspr   MAS0, 7
    isync
    tlbre
    isync
    mfspr   6, MAS1
    rlwinm  6, 6, 0, 2, 31              /* Clear Valid and IPROT */
    mtspr   MAS1, 6
    isync
    tlbwe
    isync

    li      1, 0xff                     /* Initialize stack pointer with some value.
                                           Final stack initialization will take place in __arena_start */
    mr      3, 14                      /* Put back argc, argv and envp values */
    mr      4, 15
    mr      5, 16

    lis     6, __arena_start@ha          /* Jump to __arena_start */
    addi    6, 6, __arena_start@l
    mtlr    6
    blrl

