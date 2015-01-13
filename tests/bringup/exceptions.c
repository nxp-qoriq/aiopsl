#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_dbg.h"
#include "inline_asm.h"
#include "fsl_io_ccsr.h"
#include "cmgw.h"
#include "aiop_common.h"

static struct aiop_cmgw_regs *cmgw_regs;

static void init()
{
	cmgw_regs = (struct aiop_cmgw_regs *)\
		(SOC_PERIPH_OFF_AIOP_TILE + SOC_PERIPH_OFF_AIOP_CMGW);
}

static uint32_t _cmgw_get_ntasks()
{
	return (ioread32_ccsr(&cmgw_regs->wscr) & CMGW_WSCR_NTASKS_MASK);
}

static inline void configure_stack_overflow_detection(void)
{
    /* DBCR2 */
    booke_set_spr_DBCR2(booke_get_spr_DBCR2() | 0x00c00000);

    /* DBCR4 */
    asm {
        mfspr   r6,DBCR4
        ori r6, r6, 0x0080 /* DAC1CFG */
        mtspr   DBCR4,r6
        isync
    }
//  booke_set_spr_DBCR4(booke_get_spr_DBCR4() | 0x00000080);
//  booke_instruction_sync();

    /* DBCR0 */
    booke_set_spr_DBCR0(booke_get_spr_DBCR0() | 0x400f0000);
    booke_instruction_sync();

    /* initiate DAC registers */
    booke_set_spr_DAC1(0x400);
    booke_set_spr_DAC2(0x8000);
}

static inline void config_runtime_stack_overflow_detection()
{
	switch(_cmgw_get_ntasks())
	{
	case 0: /* 1 Task */
		booke_set_spr_DAC2(0x8000);
		break;
	case 1: /* 2 Tasks */
		booke_set_spr_DAC2(0x4000);
		break;
	case 2: /* 4 Tasks */
		booke_set_spr_DAC2(0x2000);
		break;
	case 3: /* 8 Tasks */
		booke_set_spr_DAC2(0x1000);
		break;
	case 4: /* 16 Tasks */
		booke_set_spr_DAC2(0x800);
		break;
	default:
		//TODO complete
		break;
	}
}

__HOT_CODE static int func_in_iram()
{
	int err = 0;

	err = -1;

	return err;
}

static int stack_overflow_test()
{
	uint8_t arr[0x10000];

	arr[0x8000] = 0xdd;
	arr[0] = 0xdd;
	arr[5] = 0xdd;

	if (arr[0x8000] == 0xdd)
		return -EINVAL;
	else
		return 0;
}

static int recursion_func(int i)
{
	pr_debug("i = %d\n", i);
	recursion_func(i+1);
}


int exceptions_test();
int exceptions_test()
{
	int err = 0;
	uint8_t *iram_ptr = (uint8_t *)((void *)func_in_iram);
	uint32_t esr;
	uint32_t _res, _disp, _base;

	init();

	/* Write to IRAM */
	iram_ptr[0] = 0xff;
	iram_ptr[1] = 0xff;
	if ((iram_ptr[1] == 0xff) || (iram_ptr[1] == 0xff)) {
		pr_debug("Can write to IRAM\n");
	} else {
		pr_debug("Can't write to IRAM\n");
	}
	esr = booke_get_spr_ESR();
	pr_debug("esr = 0x%x\n", esr);

	err = func_in_iram();
	esr = booke_get_spr_ESR();
	pr_debug("esr = 0x%x\n", esr);

	/* Divide by Zero */
	err = (0x11 / err);
	esr = booke_get_spr_ESR();
	pr_debug("esr = 0x%x\n", esr);

	_res = 9;
	_disp = 3;
	_base = 0x00fe0001;
	__stwcx(_res, _disp, _base);
	esr = booke_get_spr_ESR();
	pr_debug("esr = 0x%x\n", esr);

#ifndef ARENA_TEST
	/* Stack overflow */
	configure_stack_overflow_detection();
#endif

	err = stack_overflow_test();
	esr = booke_get_spr_ESR();
	pr_debug("esr = 0x%x\n", esr);
	//if (!esr) return -EINVAL;

	config_runtime_stack_overflow_detection();
	err = stack_overflow_test();
	esr = booke_get_spr_ESR();
	pr_debug("esr = 0x%x\n", esr);

	recursion_func(1);

	return err;
}
