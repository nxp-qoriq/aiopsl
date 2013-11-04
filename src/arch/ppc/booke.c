/*

 @File          booke.c

 @Description   Upper Level ionterfaces to the ASM functions

 @Cautions

Author: Donio Ron


*//***************************************************************************/
#define __ERR_MODULE__  MODULE_UNKNOWN

#include "common/types.h"
#include "common/errors.h"
#include "common/irq.h"
#include "arch/cores/fsl_core_booke_regs.h"
#include "arch/cores/fsl_core_booke.h"
#include "booke.h"

#ifdef CORE_E6500
#include "clc_ext.h"
#endif /* CORE_E6500 */


#ifdef CORE_E500V2
extern void save_core_regs(uint32_t bptr_addr);
extern void booke_recover_deep_sleep_mode();
void load_core_regs_org();

void booke_set_doze_mode(void)
{
    uint32_t tmp_reg;

    /* Disable Decrementer interrupt */
    tmp_reg = booke_get_spr_TCR();
    tmp_reg &= ~TCR_DIE;
    booke_set_spr_TCR(tmp_reg);
}

void booke_set_nap_mode(void)
{
    uint32_t tmp_reg;

    /* Disable Decrementer interrupt */
    tmp_reg = booke_get_spr_TCR();
    tmp_reg &= ~TCR_DIE;
    booke_set_spr_TCR(tmp_reg);

    /* set the core in NAP mode */
    tmp_reg = booke_get_spr_HID0();
    tmp_reg |= HID0_NAP;
    booke_set_spr_HID0(tmp_reg);

    /* The L1 caches do not respond to snoops in nap mode,
    the L1 cache must be flushed before entering nap mode. */
    l1dcache_flush();
    l1icache_flush();

    msr_enable_we();
}

void booke_set_sleep_mode(void)
{

    uint32_t tmp_reg;

    /* Disable Decrementer interrupt */
    tmp_reg = booke_get_spr_TCR();
    tmp_reg &= ~TCR_DIE;
    booke_set_spr_TCR(tmp_reg);

    /* The L1 caches do not respond to snoops in sleep mode,
    the L1 cache must be flushed before entering nap mode. */
    l1dcache_flush();
    l1icache_flush();
}

int booke_set_deep_sleep_mode(uint32_t bptr_address)
{

    uint32_t    tmp_reg;

    save_core_regs(bptr_address);

    msr_disable_ce();
    msr_disable_me();

    /* The L1 caches do not respond to snoops in Deep Sleep mode,
    the L1 cache must be flushed before entering nap mode. */
    booke_icache_disable();
    booke_dcache_disable();

    booke_icache_enable();

    /* update Boot Page Translation Register */
    tmp_reg = (uint32_t)booke_recover_deep_sleep_mode;
    if((tmp_reg & 0xfff) != 0xffc)
    {
        RETURN_ERROR(MAJOR, E_INVALID_ADDRESS, ("booke_set_deep_sleep_mode alignment"));
    }
    tmp_reg >>= 12;
    tmp_reg |= 0x80000000;
    WRITE_UINT32(*(uint32_t*)bptr_address,tmp_reg);

    /* disable TCR */
    tmp_reg = booke_get_spr_TCR();
    tmp_reg &= ~TCR_DIE;
    booke_set_spr_TCR(tmp_reg);

    tmp_reg = booke_get_spr_TSR();
    tmp_reg |= TSR_DIS;
    booke_set_spr_TSR(tmp_reg);

    return E_OK;
}

void booke_set_jog_mode(void)
{
    uint32_t tmp_reg;

    tmp_reg = booke_get_spr_TCR();
    tmp_reg &= ~TCR_DIE;
    booke_set_spr_TCR(tmp_reg);

    l1dcache_flush();
    l1icache_flush();

    msr_disable_ee();
}


void booke_recover_jog_mode(void)
{
    uint32_t tmp_reg;

    tmp_reg = booke_get_spr_TCR();
    tmp_reg |= TCR_DIE;
    booke_set_spr_TCR(tmp_reg);

    msr_enable_ee();
}

void booke_recover_doze_mode(void)
{
    uint32_t tmp_reg;

    tmp_reg = booke_get_spr_TCR();
    tmp_reg |= TCR_DIE;
    booke_set_spr_TCR(tmp_reg);
}

void booke_recover_nap_mode(void)
{
    uint32_t tmp_reg;

    msr_disable_we();

    tmp_reg = booke_get_spr_HID0();
    tmp_reg &= ~HID0_NAP;
    booke_set_spr_HID0(tmp_reg);

    tmp_reg = booke_get_spr_TCR();
    tmp_reg |= TCR_DIE;
    booke_set_spr_TCR(tmp_reg);
}

void booke_recover_sleep_mode(void)
{
    uint32_t tmp_reg;

    tmp_reg = booke_get_spr_TCR();
    tmp_reg |= TCR_DIE;
    booke_set_spr_TCR(tmp_reg);
}
#endif /* CORE_E500V2 */

/*----------------------------------*/
/*  Is Instruction Cache Enabled    */
/*----------------------------------*/
int booke_is_icache_enabled(void)
{
    return !!l1icache_is_enabled();
}

/*----------------------------------*/
/*  Is Data Cache Enabled           */
/*----------------------------------*/
int  booke_is_dcache_enabled(void)
{
    return !!l1dcache_is_enabled();
}

/*----------------------------------*/
/*  Data Cache Enable               */
/*----------------------------------*/
void booke_dcache_enable(void)
{
#if defined(CORE_E500MC) || defined(CORE_E6500)
    l1dcache_invalidate_and_clear();
#else
    l1dcache_invalidate();
#endif
    l1dcache_enable();
}

/*----------------------------------*/
/*  Instruction Cache Enable        */
/*----------------------------------*/
void booke_icache_enable(void)
{
#if defined(CORE_E500MC) || defined(CORE_E6500)
    l1icache_invalidate_and_clear();
#else
    l1icache_invalidate();
#endif
    l1icache_enable();
}

/*----------------------------------*/
/*  Data Cache Disable              */
/*----------------------------------*/
void booke_dcache_disable(void)
{
   uint32_t       int_flags;

   int_flags = core_local_irq_save();
   l1dcache_flush();
   l1dcache_disable();
   core_local_irq_restore(int_flags);
}

/*----------------------------------*/
/*  Instruction Cache Disable       */
/*----------------------------------*/
void booke_icache_disable(void)
{
    uint32_t       int_flags;

    int_flags = core_local_irq_save();
    l1icache_flush();
    l1icache_disable();
    core_local_irq_restore(int_flags);
}

/*----------------------------------*/
/*  Data Cache Flush                */
/*----------------------------------*/
void booke_dcache_flush(void)
{
    l1dcache_flush();
}

/*----------------------------------*/
/*  Instruction Cache Flush         */
/*----------------------------------*/
void booke_icache_flush(void)
{
    l1icache_flush();
}

/*----------------------------------*/
/*  Data Cache Set Stash Id         */
/*----------------------------------*/
void booke_dcache_set_stash_id(uint8_t stash_id)
{
    uint32_t    reg = (booke_get_spr_L1CSR2() & not_L1CSR2_STASHID);

    reg |= stash_id;

    booke_set_spr_L1CSR2(reg);
}

/*----------------------------------*/
/*  Data Cache Set Stash Id         */
/*----------------------------------*/
uint8_t booke_dcache_get_stash_id(void)
{
    uint32_t   reg = booke_get_spr_L1CSR2 ();

    reg &= L1CSR2_STASHID;

    return (uint8_t)reg;
}

/*****************************************************************************/
void booke_enable_time_base(void)
{
    uint32_t t_hid0;

    t_hid0 = booke_get_spr_HID0();
    t_hid0 |= HID0_TBEN;
    booke_set_spr_HID0(t_hid0);
}

/*****************************************************************************/
void booke_disable_time_base(void)
{
    uint32_t hid0;

    hid0 = booke_get_spr_HID0();
    hid0 &= ~(HID0_TBEN);
    booke_set_spr_HID0(hid0);
}

/*****************************************************************************/
uint64_t booke_get_time_base_time(void)
{
    uint64_t time;
    uint32_t timel, timeh1, timeh2;

    do
    {
        timeh1 = booke_get_spr_TBU();
        timel = booke_get_spr_TBL();
        timeh2 = booke_get_spr_TBU();
    } while (timeh1 != timeh2); /* Verify that no carry took place between
                                   reads of high and low*/

    time = ((uint64_t)timeh1 << 32) | timel;

    return time;
}

/*****************************************************************************/
uint32_t booke_get_id(void)
{
    uint32_t cpu_id = 0;

#ifdef E500v2_CORE_SIMULATION
    return 0;
#endif /* E500v2_CORE_SIMULATION */

    cpu_id = get_cpu_id();

#if defined(CORE_E500MC) || defined(CORE_E5500)
//    cpu_id >>= 5;
#elif defined(CORE_E6500)

#endif /* defined(CORE_E500MC) || ... */

    ASSERT_COND(cpu_id < INTG_MAX_NUM_OF_CORES);

    return cpu_id;
}

#if defined(CORE_E500MC) || defined(CORE_E5500)
/*****************************************************************************/
/*                    e500mc/e5500 L2 cache                                  */
/*****************************************************************************/
int booke_is_l2icache_enabled(void)
{
    return (int)!!(e500mc_get_spr_L2CSR0() & (L2CSR0_L2E | L2CSR0_L2IO));
}

int booke_is_l2dcache_enabled(void)
{
    return (int)!!(e500mc_get_spr_L2CSR0() & (L2CSR0_L2E | L2CSR0_L2DO));
}

void booke_l2cache_enable(booke_l2cache_mode mode)
{

    uint32_t    reg = (e500mc_get_spr_L2CSR0() & (not_L2CSR0_L2IO & not_L2CSR0_L2DO));

    /*  On boot prior to enabling the L2 cache, the processor should set this bit to clear
        any lock state bits that may be randomly set out of reset. This differs from the
        behavior defined in EREF.
        */

    reg |= L2CSR0_L2FI | L2CSR0_L2LFC;

    e500mc_set_spr_L2CSR0(reg);

    while (e500mc_get_spr_L2CSR0() != reg) ;

    reg &= not_L2CSR0_L2FI & not_L2CSR0_L2LFC;
    /* Cache enable */
    reg |= L2CSR0_L2E;

    switch (mode)
    {
        case (E_L2_CACHE_MODE_DATA_ONLY):
            reg |= L2CSR0_L2DO;
            break;
        case (E_L2_CACHE_MODE_INST_ONLY):
            reg |= L2CSR0_L2IO;
            break;
        default:
            break;
    }

    e500mc_set_spr_L2CSR0(reg);

    while (e500mc_get_spr_L2CSR0() != reg) ;
}

void booke_l2cache_disable(void)
{

    uint32_t    int_flags;
    uint32_t    reg = (e500mc_get_spr_L2CSR0() & not_L2CSR0_L2E);

    int_flags = core_local_irq_save();
    booke_l2cache_flush();
    e500mc_set_spr_L2CSR0(reg);

    while (e500mc_get_spr_L2CSR0() != reg) ;
    core_local_irq_restore(int_flags);

}

void booke_l2cache_flush(void)
{
    uint32_t    reg = e500mc_get_spr_L2CSR0();

    core_memory_barrier();
    e500mc_set_spr_L2CSR0(reg | L2CSR0_L2FL);
    core_memory_barrier();

    while (e500mc_get_spr_L2CSR0() != reg) ;
}

void booke_l2cache_set_stash_id(uint8_t stash_id)
{
    uint32_t    reg = (e500mc_get_spr_L2CSR1() & not_L2CSR1_STASHID);

    reg |= stash_id;

    e500mc_set_spr_L2CSR1(reg);
}
#endif /* defined(CORE_E500MC) || defined(CORE_E5500) */


#ifdef CORE_E6500
dma_addr_t e6500_get_ccsr_base(void)
{
    dma_addr_t   base;
    uint32_t        shifted_base;

    shifted_base = booke_get_spr_SCCSRBAR();
    base = (dma_addr_t)shifted_base << 24;
    return base;
}
#endif /* CORE_E6500 */


#ifdef CORE_E6500
/*****************************************************************************/
/*                       Cluster-Level-Cache                                 */
/*****************************************************************************/

typedef struct t_clc {
    uintptr_t   cluster_base;
} t_clc;


int clc_is_enabled(uintptr_t cluster_base)
{
    uint32_t    reg = GET_UINT32(*(uint32_t*)cluster_base);
    return !!(L2CSR0_L2E & reg);
}

fsl_handle_t clc_config_and_init(uintptr_t cluster_base)
{
    t_clc   *p_clc = (t_clc *)fsl_os_malloc(sizeof(t_clc));
    if (!p_clc)
    {
        REPORT_ERROR(MINOR, E_NO_MEMORY, ("cluster-level-cache obj!"));
        return NULL;
    }
    memset(p_clc, 0, sizeof(t_clc));

    p_clc->cluster_base = cluster_base;

    return p_clc;
}

void clc_enable(fsl_handle_t h_clc)
{
    t_clc       *p_clc = (t_clc *)h_clc;
    uint32_t    reg;

    SANITY_CHECK_RETURN(p_clc, ENODEV);
    reg = GET_UINT32(*(uint32_t*)p_clc->cluster_base);

    /*  On boot prior to enabling the L2 cache, the processor should set this bit to clear
        any lock state bits that may be randomly set out of reset. This differs from the
        behavior defined in EREF. */
    reg |= L2CSR0_L2FI | L2CSR0_L2LFC;

    WRITE_UINT32(*(uint32_t*)p_clc->cluster_base, reg);
    core_memory_barrier();

    reg = (GET_UINT32(*(uint32_t*)p_clc->cluster_base) & (not_L2CSR0_L2FI & not_L2CSR0_L2LFC));
    while (GET_UINT32(*(uint32_t*)p_clc->cluster_base) != reg) ;

    core_instruction_sync();
    __asm__("sync");
    core_instruction_sync();

    /* Cache enable */
    reg |= L2CSR0_L2E;
    WRITE_UINT32(*(uint32_t*)p_clc->cluster_base, reg);
    core_memory_barrier();
    while (GET_UINT32(*(uint32_t*)p_clc->cluster_base) != reg) ;
}

void clc_disable(fsl_handle_t h_clc)
{
    t_clc       *p_clc = (t_clc *)h_clc;
    uint32_t    reg;
    uint32_t    int_flags;

    SANITY_CHECK_RETURN(p_clc, ENODEV);
    reg = GET_UINT32(*(uint32_t*)p_clc->cluster_base);

    int_flags = core_local_irq_save();
    clc_flush(p_clc);
    WRITE_UINT32(*(uint32_t*)p_clc->cluster_base, reg);
    core_memory_barrier();
    while (GET_UINT32(*(uint32_t*)p_clc->cluster_base) != reg) ;
    core_local_irq_restore(int_flags);
}

void clc_flush(fsl_handle_t h_clc)
{
    t_clc       *p_clc = (t_clc *)h_clc;
    uint32_t    reg;
    uint32_t    L2PARN [8];
    uint8_t     i;

    SANITY_CHECK_RETURN(p_clc, ENODEV);
    reg = GET_UINT32(*(uint32_t*)p_clc->cluster_base);

    for (i = 0; i < 8; i++)
    {
        L2PARN[i] =  GET_UINT32(*(uint32_t*)(p_clc->cluster_base+0x208+i*16));
        WRITE_UINT32(*(uint32_t*)(p_clc->cluster_base+0x208 + i*16),0);
    }

    core_memory_barrier();

    for (i = 0; i < 8; i++)
        while (GET_UINT32(*(uint32_t*)(p_clc->cluster_base+0x208 + 16*i)) != 0);

    reg |= L2CSR0_L2FL;
    WRITE_UINT32(*(uint32_t*)p_clc->cluster_base, reg);
    core_memory_barrier();

    core_instruction_sync();
    __asm__("sync");
    core_instruction_sync();

    reg = (GET_UINT32(*(uint32_t*)p_clc->cluster_base) & not_L2CSR0_L2FL);

    while (GET_UINT32(*(uint32_t*)p_clc->cluster_base) != reg) ;

    for (i = 0; i < 8; i++)
        WRITE_UINT32(*(uint32_t*)(p_clc->cluster_base+0x208+16*i),L2PARN[i]);

    core_memory_barrier();

    for (i = 0; i < 8; i++)
        while (GET_UINT32(*(uint32_t*)(p_clc->cluster_base+0x208+16*i)) != L2PARN[i]);
}

void clc_set_stash_id(fsl_handle_t h_clc, uint8_t stash_id)
{
    t_clc       *p_clc = (t_clc *)h_clc;
    uint32_t    reg;

    SANITY_CHECK_RETURN(p_clc, ENODEV);
    reg = GET_UINT32(*(uint32_t*)p_clc->cluster_base);

    reg |= stash_id;

    WRITE_UINT32(*(uint32_t*)(p_clc->cluster_base+0x004), reg);
    while (GET_UINT32(*(uint32_t*)(p_clc->cluster_base+0x004)) != reg) ;
}
#endif /* CORE_E6500 */
