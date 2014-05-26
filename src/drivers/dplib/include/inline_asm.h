/**************************************************************************//**
@File		inline_asm.h

@Description	This file contains AIOP SW internal inline assembler
		definitions.

		Copyright 2013 Freescale Semiconductor, Inc.
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
