/*
 *
 * @File          booke_irq.c
 *
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

#include "common/types.h"
#include "common/errors.h"
#include "common/dbg.h"

#include "arch/cores/fsl_core_booke_regs.h"
#include "ppc/include/fsl_core_booke.h"
#include "booke.h"


/*-----------------------------------------------------
*
* Local definitions from vector to index xx00->xx
*------------------------------------------------------*/
#define CRITICAL_INTR_INDEX             ((CRITICAL_INTR&0xff00)>>8)
#define MACHINE_CHECK_INTR_INDEX        ((MACHINE_CHECK_INTR&0xff00)>>8)
#define DATA_STORAGE_INTR_INDEX         ((DATA_STORAGE_INTR&0xff00)>>8)
#define INSTRUCTION_STORAGE_INTR_INDEX  ((INSTRUCTION_STORAGE_INTR&0xff00)>>8)
#define EXTERNAL_INTR_INDEX             ((EXTERNAL_INTR&0xff00)>>8)
#define ALIGNMENT_INTR_INDEX            ((ALIGNMENT_INTR&0xff00)>>8)
#define PROGRAM_INTR_INDEX              ((PROGRAM_INTR&0xff00)>>8)
#define FLOATING_POINT_INTR_INDEX       ((FLOATING_POINT_INTR&0xff00)>>8)
#define SYSTEM_CALL_INTR_INDEX          ((SYSTEM_CALL_INTR&0xff00)>>8)
#define AUXILIARY_INTR_INDEX            ((AUXILIARY_INTR&0xff00)>>8)
#define DECREMENTER_INTR_INDEX          ((DECREMENTER_INTR&0xff00)>>8)
#define FIXED_INTERVAL_INTR_INDEX       ((FIXED_INTERVAL_INTR&0xff00)>>8)
#define WATCHDOG_TIMER_INTR_INDEX       ((WATCHDOG_TIMER_INTR&0xff00)>>8)
#define D_TLB_ERROR_INTR_INDEX          ((D_TLB_ERROR_INTR&0xff00)>>8)
#define I_TLB_ERROR_INTR_INDEX          ((I_TLB_ERROR_INTR&0xff00)>>8)
#define DEBUG_INTR_INDEX                ((DEBUG_INTR&0xff00)>>8)
#define SPE_APU_INTR_INDEX              ((SPE_APU_INTR&0xff00)>>8)
#define SPE_FLT_DATA_INTR_INDEX         ((SPE_FLT_DATA_INTR&0xff00)>>8)
#define SPE_FLT_ROUND_INTR_INDEX        ((SPE_FLT_ROUND_INTR&0xff00)>>8)
#define PERF_MONITOR_INTR_INDEX         ((PERF_MONITOR_INTR&0xff00)>>8)

/******************************************************
*   Structure:      t_E500IntrTable
*
*   Description:    Hold all E500 callback functions
*******************************************************/
typedef struct {
    void            (* f_isr)(fsl_handle_t);
                                /**< interrupt handler */
    fsl_handle_t    src_handle;  /**< Parameter to pass to interrupt handler */
    int             active;     /* True if f_Isr active */
} irq_src_tbl_t;


void booke_init_interrupt_vector(void);

static irq_src_tbl_t core_intr_table[LAST_INTR_INDEX] = {{0}};


/*---------------------------------------------------------*/
/*  Install A PPC Interrupt vector routine                 */
/*---------------------------------------------------------*/
int booke_set_intr(int ppc_intr_src, void (* isr)(fsl_handle_t handle), fsl_handle_t handle )
{
    int index = (ppc_intr_src & 0xff00) >> 8; /* Numbered from 0x100-->0x1900 */

    core_intr_table[index].f_isr =  isr;
    core_intr_table[index].src_handle = handle;
    core_intr_table[index].active = 1;

    return (E_OK);
}

/*---------------------------------------------------------*/
/*  Clear A PPC Interrupt vector routine                   */
/*---------------------------------------------------------*/
int booke_clear_intr(int ppc_intr_src)
{
    int index = (ppc_intr_src & 0xff00) >> 8; /* Numbered from 0x100-->0x1900 */

    core_intr_table[index].active = 0;
    return (E_OK);
}


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

    if (core_intr_table[CRITICAL_INTR_INDEX].active)
        core_intr_table[CRITICAL_INTR_INDEX].f_isr(core_intr_table[CRITICAL_INTR_INDEX].src_handle);
    else
	    pr_debug("core %d int: CRITICAL\n", core_get_id());

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
    static int print_limit = 3;

    switch(intr_entry)
    {
    case(CRITICAL_INTR):                                /* IVOR0 100 */
        if (core_intr_table[CRITICAL_INTR_INDEX].active)
            core_intr_table[CRITICAL_INTR_INDEX].f_isr(core_intr_table[CRITICAL_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: CRITICAL\n", core_get_id());
        break;
    case(MACHINE_CHECK_INTR):                           /* IVOR1 200 */
        if (core_intr_table[MACHINE_CHECK_INTR_INDEX].active)
            core_intr_table[MACHINE_CHECK_INTR_INDEX].f_isr(core_intr_table[MACHINE_CHECK_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: MACHINE_CHECK\n", core_get_id());
        break;
    case(DATA_STORAGE_INTR):                            /* IVOR2 300 */
        if (core_intr_table[DATA_STORAGE_INTR_INDEX].active)
            core_intr_table[DATA_STORAGE_INTR_INDEX].f_isr(core_intr_table[DATA_STORAGE_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: DATA_STORAGE\n", core_get_id());
        break;
    case(INSTRUCTION_STORAGE_INTR):                     /* IVOR3 400 */
        if (core_intr_table[INSTRUCTION_STORAGE_INTR_INDEX].active)
            core_intr_table[INSTRUCTION_STORAGE_INTR_INDEX].f_isr(core_intr_table[INSTRUCTION_STORAGE_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: INSTRUCTION_STORAGE\n", core_get_id());
        break;
    case(EXTERNAL_INTR):                                /* IVOR4 500 */
        if (core_intr_table[EXTERNAL_INTR_INDEX].active)
            core_intr_table[EXTERNAL_INTR_INDEX].f_isr(core_intr_table[EXTERNAL_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: EXTERNAL\n", core_get_id());
        break;
    case(ALIGNMENT_INTR):                               /* IVOR5 600 */
        if (core_intr_table[ALIGNMENT_INTR_INDEX].active)
            core_intr_table[ALIGNMENT_INTR_INDEX].f_isr(core_intr_table[ALIGNMENT_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: ALIGNMENT\n", core_get_id());
        break;
    case(PROGRAM_INTR):                                 /* IVOR6 700 */
        if (core_intr_table[PROGRAM_INTR_INDEX].active)
            core_intr_table[PROGRAM_INTR_INDEX].f_isr(core_intr_table[PROGRAM_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: PROGRAM\n", core_get_id());
        break;
    case(SYSTEM_CALL_INTR):                             /* IVOR8 900 */
        if (core_intr_table[SYSTEM_CALL_INTR_INDEX].active)
            core_intr_table[SYSTEM_CALL_INTR_INDEX].f_isr(core_intr_table[SYSTEM_CALL_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: SYSTEM CALL\n", core_get_id());
        break;
    case(DECREMENTER_INTR):                             /* IVOR10 1000 */
        if (core_intr_table[DECREMENTER_INTR_INDEX].active)
            core_intr_table[DECREMENTER_INTR_INDEX].f_isr(core_intr_table[DECREMENTER_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: DECREMENTER\n", core_get_id());
        break;
    case(FIXED_INTERVAL_INTR):                          /* IVOR11 1100 */
        if (core_intr_table[FIXED_INTERVAL_INTR_INDEX].active)
            core_intr_table[FIXED_INTERVAL_INTR_INDEX].f_isr(core_intr_table[FIXED_INTERVAL_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: fixed-interval timer\n", core_get_id());
        break;
    case(WATCHDOG_TIMER_INTR):                          /* IVOR12 1200 */
        if (core_intr_table[WATCHDOG_TIMER_INTR_INDEX].active)
            core_intr_table[WATCHDOG_TIMER_INTR_INDEX].f_isr(core_intr_table[WATCHDOG_TIMER_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: watchdog timer\n", core_get_id());
        break;
    case(D_TLB_ERROR_INTR):                             /* IVOR13 1300 */
        if (core_intr_table[D_TLB_ERROR_INTR_INDEX].active)
            core_intr_table[D_TLB_ERROR_INTR_INDEX].f_isr(core_intr_table[D_TLB_ERROR_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: data TLB error\n", core_get_id());
        break;
    case(I_TLB_ERROR_INTR):                             /* IVOR14 1400 */
        if (core_intr_table[I_TLB_ERROR_INTR_INDEX].active)
            core_intr_table[I_TLB_ERROR_INTR_INDEX].f_isr(core_intr_table[I_TLB_ERROR_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: instruction TLB error\n", core_get_id());
        break;
    case(DEBUG_INTR):                                   /* IVOR15 1500 */
        if (core_intr_table[DEBUG_INTR_INDEX].active)
            core_intr_table[DEBUG_INTR_INDEX].f_isr(core_intr_table[DEBUG_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: debug\n", core_get_id());
        break;
    case(SPE_APU_INTR):                                 /* IVOR32 1600 */
        if (core_intr_table[SPE_APU_INTR_INDEX].active)
            core_intr_table[SPE_APU_INTR_INDEX].f_isr(core_intr_table[SPE_APU_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: SPE-APU unavailable\n", core_get_id());
        break;
    case(SPE_FLT_DATA_INTR):                            /* IVOR33 1700 */
        if (core_intr_table[SPE_FLT_DATA_INTR_INDEX].active)
            core_intr_table[SPE_FLT_DATA_INTR_INDEX].f_isr(core_intr_table[SPE_FLT_DATA_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: SPE-floating point data\n", core_get_id());
        break;
    case(SPE_FLT_ROUND_INTR):                           /* IVOR34 1800 */
        if (core_intr_table[SPE_FLT_ROUND_INTR_INDEX].active)
            core_intr_table[SPE_FLT_ROUND_INTR_INDEX].f_isr(core_intr_table[SPE_FLT_ROUND_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: SPE-floating point round\n", core_get_id());
        break;
    case(PERF_MONITOR_INTR):                            /* IVOR35 1900 */
        if (core_intr_table[PERF_MONITOR_INTR_INDEX].active)
            core_intr_table[PERF_MONITOR_INTR_INDEX].f_isr(core_intr_table[PERF_MONITOR_INTR_INDEX].src_handle);
        else if (print_limit-- > 0)
            pr_debug("core %d int: performance monitor\n", core_get_id());
        break;
    default:
	    pr_warn("e500 undefined interrupt #%x\n", intr_entry);
        break;
    }
}
