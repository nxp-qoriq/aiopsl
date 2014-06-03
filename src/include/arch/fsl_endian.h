/**************************************************************************//**
@File		fsl_endian.h

@Description	This file contains AIOP endian macros.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __FSL_ENDIAN_H_
#define __FSL_ENDIAN_H_
#include "common/types.h"
#include "common/fsl_core.h"


/**************************************************************************//**
 @Group		AIOP_GENERAL AIOP General

 @Description	AIOP General macros and functions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_Endian_Macros

 @Description	Endian Swap Macros

 @{
*//***************************************************************************/

/** Load 2 bytes with endian swap.
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0. */
#define LH_SWAP(_disp, _base) ((uint16_t)__lhbr((uint32_t)_disp, (void *)_base))

/** Load 4 bytes with endian swap.
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0. */
#define LW_SWAP(_disp, _base) ((uint32_t)__lwbr((uint32_t)_disp, (void *)_base))

/** Load 8 bytes with endian swap of each 4 bytes.
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0. */
#define LDW_SWAP(_disp, _base)					\
	(uint64_t)({register uint64_t __rR = 0;			\
	unsigned int temp1, temp2;				\
	__ldwbrw(&temp1, &temp2, (uint32_t)_disp, (void *)_base);\
	__rR = (uint64_t)					\
		((((uint64_t)temp1) << 32) | (uint64_t)temp2); })

/** Load 8 bytes with endian swap.
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0. */
#define LLLDW_SWAP(_disp, _base)				\
	(uint64_t)({register uint64_t __rR = 0;			\
	uint64_t temp;						\
	__llldbrw(&temp, (uint32_t)_disp, (void *)_base);	\
	__rR = (uint64_t) temp; })

/** Load 2 bytes with endian swap and mask result
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0.
 * _mask - mask value. */
#define LH_SWAP_MASK(_disp, _base, _mask)			\
	(uint16_t)(uint32_t)({register uint16_t *__rR = 0;	\
	uint16_t temp = LH_SWAP(_disp, _base);			\
	temp &= _mask;						\
	__rR = (uint16_t *) temp; })

/** Load 4 bytes with endian swap and mask result.
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0.
 * _mask - mask value. */
#define LW_SWAP_MASK(_disp, _base, _mask)			\
	(uint32_t)({register uint32_t *__rR = 0;		\
	uint32_t temp = LW_SWAP(_disp, _base);			\
	temp &= _mask;						\
	__rR = (uint32_t *) temp; })

/** Load 4 bytes with endian swap, mask and shift result.
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0.
 * _mask - mask value.
 * _shift - shift value.*/
#define LW_SWAP_MASK_SHIFT(_disp, _base, _mask, _shift)		\
	(uint32_t)({register uint32_t *__rR = 0;		\
	uint32_t temp = LW_SWAP(_disp, _base);			\
	temp &= _mask;						\
	temp >>= _shift;					\
	__rR = (uint32_t *) temp; })

/** Load 8 bytes.
 * The address loaded from memory is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0. */
#define LLLDW(_disp, _base)					\
	(uint64_t)({register uint64_t __rR = 0;			\
	uint64_t temp;						\
	__llldw(&temp, (uint32_t)_disp, (void *)_base);		\
	__rR = (uint64_t) temp; })

/** Store 2 bytes with endian swap.
 * The memory address being stored is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0.
 * _val - 16bit value to be stored.*/
#define STH_SWAP(_val, _disp, _base) 				\
	__sthbr(_val, (uint32_t)_disp, (void *)_base)

/** Store 4 bytes with endian swap.
 * The memory address being stored is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0.
 * _val - 32bit value to be stored.*/
#define STW_SWAP(_val, _disp, _base)				\
	__stwbr(_val, (uint32_t)_disp, (void *)_base)

/** Store 8 bytes with endian swap.
 * The memory address being stored is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0.
 * _val - 64bit value to be stored. */
#define LLSTDW_SWAP(_val, _disp, _base)				\
	__llstdbrw(_val, (uint32_t)_disp, (void *)_base)


/** Store 8 bytes.
 * The memory address being stored is calculated as: _displ + _base.
 * _displ - a word aligned constant value between 0-1020.
 * _base - a variable containing the base address.
 * If 'base' is a literal 0, the base address is considered as 0.
 * _val - 64bit value to be stored. */
#define LLSTDW(_val, _disp, _base)				\
	__llstdw(_val, (uint32_t)_disp, (void *)_base)


/** @} */ /* end of AIOP_Endian_Macros */

/**************************************************************************//**
 @Collection    Little-Endian Conversion Macros

                These macros convert given parameters to or from Little-Endian
                format. Use these macros when you want to read or write a specific
                Little-Endian value in memory, without a-priori knowing the CPU
                byte order.

                These macros use the byte-swap routines. For conversion of
                constants in initialization structures, you may use the CONST
                versions of these macros (see below), which are using the
                byte-swap macros instead.
 @{
*//***************************************************************************/


#ifdef CORE_IS_BIG_ENDIAN

/* write */
#define CPU_TO_LE16(val, addr)  STH_SWAP(val, 0, addr)
#define CPU_TO_LE32(val, addr)  STW_SWAP(val, 0, addr)
#define CPU_TO_LE64(val, addr)  LLSTDW_SWAP(val, 0, addr)

#define CPU_TO_BE16(val, addr)	{ *addr = val }
#define CPU_TO_BE32(val, addr)	{ *addr = val }
#define CPU_TO_BE64(val, addr)	LLSTDW(val, 0, addr) /*TODO: check if compiler is using intrinsic by default (opt 4) */

/* read */
#define LE16_TO_CPU(addr)        LH_SWAP(0, addr)
#define LE32_TO_CPU(addr)        LW_SWAP(0, addr)
#define LE64_TO_CPU(addr)        LLLDW_SWAP(0, addr)

#define BE16_TO_CPU(addr)        ((uint16_t)(*addr))
#define BE32_TO_CPU(addr)        ((uint32_t)(*addr))
#define BE64_TO_CPU(addr)        LLLDW(0, addr)

#else  /* CORE_IS_LITTLE_ENDIAN */
/* write */
#define CPU_TO_LE16(val, addr)	{ *addr = val }
#define CPU_TO_LE32(val, addr)	{ *addr = val }
#define CPU_TO_LE64(val, addr)	LLSTDW(val, 0, addr)

#define CPU_TO_BE16(val, addr)  STH_SWAP(val, 0, addr)
#define CPU_TO_BE32(val, addr)  STW_SWAP(val, 0, addr)
#define CPU_TO_BE64(val, addr)  LLSTDW_SWAP(val, 0, addr)

/* read */
#define LE16_TO_CPU(addr)        ((uint16_t)(*addr))
#define LE32_TO_CPU(addr)        ((uint32_t)(*addr))
#define LE64_TO_CPU(addr)        LLLDW(0, addr)

#define BE16_TO_CPU(addr)        LH_SWAP(0, addr)
#define BE32_TO_CPU(addr)        LW_SWAP(0, addr)
#define BE64_TO_CPU(addr)        LLLDW_SWAP(0, addr)

#endif /* CORE_IS_LITTLE_ENDIAN */


/* @} */

/** @} */ /* end of AIOP_GENERAL */

#endif /* __FSL_ENDIAN_H_ */
