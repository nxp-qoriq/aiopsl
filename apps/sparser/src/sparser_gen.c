/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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

/*******************************************************************************
 *                              - CAUTION -
 *
 * This code must not be distributed till we haven't a clear understanding
 * about what internals of the Parser may be exposed to customers.
 ******************************************************************************/

#include "fsl_types.h"
#include <__mem.h>
#include "fsl_stdio.h"
#include "fsl_sparser_gen.h"
#include "fsl_sparser_drv.h"

#include "fsl_dbg.h"
/* If "fsl_dbg.h" is not included ASSERT_COND and pr_err must be redefined as
 * it follows */
#ifndef ASSERT_COND
	#define ASSERT_COND(_cond)
#endif
#ifndef pr_err
	#define pr_err(...)
#endif

/******************************************************************************/
struct sparser_bytecode_data {
	uint8_t		initialized;
	uint16_t	start_pc;
	uint8_t		*sp;
	uint16_t	sz;
	uint16_t	*sp_code;
	uint16_t	pc;
	int		words;
};

/******************************************************************************/
static struct sparser_bytecode_data	spd;

/******************************************************************************/
int sparser_begin_bytecode_wrt(uint16_t pc, uint8_t *sp, uint16_t sz)
{
	if (spd.initialized) {
		pr_err("SP bytecode already initialized\n");
		return -1;
	}
	if (pc < PARSER_MIN_PC || pc >= PARSER_MAX_PC) {
		pr_err("Invalid starting PC 0x%x (< 0x%x or >= 0x%x)\n",
		       pc, PARSER_MIN_PC, PARSER_MAX_PC);
		return -1;
	}
	if (!sz) {
		pr_err("SP size is 0\n");
		return -1;
	}
	if (sz % 4) {
		pr_err("SP size %d is not 4 multiple\n", sz);
		return -1;
	}
	memset(sp, 0, sz);
	spd.start_pc = pc;
	spd.pc = pc;
	spd.sp = sp;
	spd.sz = sz;
	spd.words = (sz / 2);
	spd.initialized = 1;
	spd.sp_code = (uint16_t *)sp;
	return 0;
}

/******************************************************************************/
void sparser_end_bytecode_wrt(void)
{
	spd.initialized = 0;
}

/******************************************************************************/
int sparser_set_label(uint8_t label)
{
	/*fsl_print("\t %s - %d : label = 0x%02x\n",
		    __func__, __LINE__, label);*/
	UNUSED(label);
	return 0;
}

/******************************************************************************/
int sparser_gen_nop(void)
{
	if (!spd.initialized) {
		pr_err("SP bytecode not initialized\n");
		return -1;
	}
	if (spd.words < 1) {
		pr_err("Exceeded SP size : d\n", spd.sz);
		return -1;
	}
	*(spd.sp_code++) = 0x0000;	/* NOP opcode */
	spd.words--;
	return 0;
}
