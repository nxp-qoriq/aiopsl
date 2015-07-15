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
@File		inline_asm.h

@Description	This file contains AIOP SW internal inline assembler
		definitions.

*//***************************************************************************/


#ifndef __INLINE_ASM_H_
#define __INLINE_ASM_H_


/* Load Byte And Reserve Indexed */
#define __lbarx(_res, _disp, _base)			\
	asm ("lbarx %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Load Halfword And Reserve Indexed */
#define __lharx(_res, _disp, _base)			\
	asm ("lharx %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Load Word And Reserve Indexed */
#define __lwarx(_res, _disp, _base)			\
	asm ("lwarx %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Store Byte Conditional Indexed */
#define __stbcx(_res, _disp, _base)			\
	asm ("stbcx. %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Store HalfWord Conditional Indexed */
#define __sthcx(_res, _disp, _base)			\
	asm ("sthcx. %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Store Word Conditional Indexed */
#define __stwcx(_res, _disp, _base)			\
	asm ("stwcx. %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Store Double of Two Words 64 bit input variable and displacement*/
#define __st64dw_d(_val, _disp)				\
	asm ("e_stdw %[value], %[displ](r0)\n"		\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp)	\
		);
/* Store Double of Two Words 64 bit input variable and base*/
#define __st64dw_b(_val, _base_reg)				\
	asm ("e_stdw %[value], 0(%[base])\n"			\
		:						\
		:[value]"r" (_val), [base]"r" (_base_reg)	\
		);
/* Load Double of Two Words 64 bit input variable and displacement*/
#define __ld64dw_d(_val, _disp)				\
	asm ("e_ldw %[value], %[displ](r0)\n"		\
		:[value]"=r" (_val)			\
		:[displ]"i" (_disp)			\
		);
/* Load Double of Two Words 64 bit input variable and base*/
#define __ld64dw_b(_val, _base_reg)			\
	asm ("e_ldw %[value], 0(%[base])\n"		\
		:[value]"=r" (_val)			\
		:[base]"r" (_base_reg)			\
		);
/* Store word input variable by displacement */
#define __stw_d(_val, _disp)				\
	asm ("e_stw %[value], %[displ](r0)\n"		\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp)	\
		);
/* Store half word displacement as input variable */
#define __sthw_d(_val, _disp)				\
	asm ("e_sth %[value], %[displ](r0)\n"		\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp)	\
		);
/* Load byte displacement as input variable */
#define __lbz_d(_val, _disp)				\
	asm ("e_lbz %[value], %[displ](r0)\n"		\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp)	\
		);
/* Store word input variable */
#define __stw(_val, _disp, _base_reg)			\
	asm ("e_stw %[value], %[displ](%[base])\n"	\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp), [base]"r" (_base_reg)\
		);
/* Store byte input variable */
#define __se_stb(_val, _base_reg)			\
	asm ("se_stb %[value], 0(%[base])\n"		\
		:[base]"=m" (_base_reg)			\
		:[value]"r" (_val)			\
		);
/* Or immediate */
#define __or(_res, _arg1, _arg2)				\
	asm ("or %[result], %[argument1], %[argument2]\n"	\
		:[result]"=r" (_res)				\
		:[argument1]"r" (_arg1), [argument2]"r" (_arg2)	\
		);
/* Or immediate */
#define __e_or2is(_res, _arg1)				\
	asm ("e_or2is %[result], %[argument1]\n"	\
		:[result]"=r" (_res)			\
		:[argument1]"i" (_arg1)			\
		);





#endif /* __INLINE_ASM_H_ */
