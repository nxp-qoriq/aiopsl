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

/**************************************************************************//**
@File		fsl_sparser_gen.h

@Description	Soft Parser byte-code generation API
*//***************************************************************************/
#ifndef __FSL_SPARSER_GEN_H
#define __FSL_SPARSER_GEN_H

#include "fsl_dbg.h"

/**************************************************************************//**
@Group		sparser_gen_g SPARSER GEN

@Description	Contains API functions and wrapper macros used to generate the
		byte code array of a Soft Parser.
@{
*//***************************************************************************/

/**************************************************************************//**
@Description	 enum sparser_jmp_flags - Jump destination post-fix flags

*//***************************************************************************/
enum sparser_jmp_flags {
	/* GOSUB flag */
	GF = 0x8000,
	/* Relative address flag */
	LF = 0x4000,
	/* Advance flag */
	AF = 0x2000,
	/* SP "label" flag */
	SF = 0x0800
};

/**************************************************************************//**
 @Group		SPARSER_LOC_LABELS Soft Parser Local Labels

 @Description	Defines the value of the first "local" label.

 @{
*//***************************************************************************/
/* Local Labels Base Address */
#define SPARSER_LOC_LABEL_BASE	0x0800

/**************************************************************************//**
@Description	 enum sparser_local_labels - 16 Local Labels

*//***************************************************************************/
enum sparser_local_labels {
	/* Local Label 1 */
	SP_LABEL_1 = SPARSER_LOC_LABEL_BASE,
	SP_LABEL_2,
	SP_LABEL_3,
	SP_LABEL_4,
	SP_LABEL_5,
	SP_LABEL_6,
	SP_LABEL_7,
	SP_LABEL_8,
	SP_LABEL_9,
	SP_LABEL_10,
	SP_LABEL_11,
	SP_LABEL_12,
	SP_LABEL_13,
	SP_LABEL_14,
	SP_LABEL_15,
	/* Local Label 16 */
	SP_LABEL_16
};

/** @} end of group SPARSER_LOC_LABELS */

/**************************************************************************//**
@Function	sparser_begin_bytecode_wrt

@Description	Initializes a Soft Parser byte-code writing. Mandatory call.

@Param[in]	pc : Starting Program Counter of the Soft Parser.

@Param[in]	sp : Soft Parser byte-code array. Must be aligned on a four
		bytes boundary.

@Param[in]	sz : Soft Parser byte-code array size, in bytes. Must be a
		multiple of four.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_begin_bytecode_wrt(uint16_t pc, uint8_t *sp, uint16_t sz);

#define SPARSER_BEGIN(_pc, _sp, _sz)					\
	do {								\
		int	err;						\
									\
		err = sparser_begin_bytecode_wrt(_pc, _sp, _sz);	\
		ASSERT_COND(!err);					\
	} while (0)

/**************************************************************************//**
@Function	sparser_end_bytecode_wrt

@Description	Finalizes a Soft Parser byte-code writing. Mandatory call.

@Return		None

*//***************************************************************************/
void sparser_end_bytecode_wrt(void);

#define SPARSER_END	sparser_end_bytecode_wrt()

/******************************************************************************/
int sparser_set_label(uint8_t label);
#define SP_LABEL(_a)	sparser_set_label(_a)

/**************************************************************************//**
@Function	sparser_gen_nop

@Description	Generates NOP opcode.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_gen_nop(void);

#define NOP								\
	do {								\
		int	err;						\
									\
		err = sparser_gen_nop();				\
		ASSERT_COND(!err);					\
	} while (0)

/** @} */ /* end of sparser_gen_g SPARSER GEN group */
#endif		/* __FSL_SPARSER_GEN_H */
