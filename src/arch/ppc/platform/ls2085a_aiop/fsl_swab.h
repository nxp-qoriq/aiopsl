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
 @Group		SWAB

 @Description	General macros and functions - used for SWAp Bytes
 	 	 The following macros are used to speed up byte reverse
 	 	 functionality in store and load.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		Endian_Macros

 @Description	Endian Swap Macros

 @{
*//***************************************************************************/
#ifndef __FSL_SWAB_H_
#define __FSL_SWAB_H_
#include "common/types.h"
#include "inc/fsl_core.h"

/**
 * reverses bytes of each halfword in unsigned int expression to return an
 * unsigned int
 * _val - variable to be swapped. */
#define SWAP_WH(_val) ((uint32_t) __byterevh(uint32_t) expr))


/** swap 4 bytes with endian swap of half word.
 * _val - variable to be swapped. */
#define SWAP_W(_val) ((uint32_t) __byterevw((uint32_t) _val))

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


/** @} */ /* end of Endian_Macros */

/** return 2 bytes with endian swap.
 * _val - 16 bit value to be swaped. */

static uint16_t swap_uint16(uint16_t val)
{
	return LH_SWAP(0, &val );
}

 /** return 4 bytes with endian swap.
  * _val - 32 bit value to be swaped. */
static uint32_t swap_uint32(uint32_t val)
{
	return LW_SWAP(0, &val );
}

/** return 8 bytes with endian swap.
 * _val - 64 bit value to be swaped. */

static uint64_t swap_uint64(uint64_t val)
{
	return LLLDW_SWAP(0, &val );
}


/*Defines to use intrinsics with store and swap*/
#ifdef CORE_IS_BIG_ENDIAN

#define STORE_CPU_TO_LE16(val, addr)	STH_SWAP(val, 0, addr)
#define STORE_CPU_TO_LE32(val, addr)	STW_SWAP(val, 0, addr)
#define STORE_CPU_TO_LE64(val, addr)	LLSTDW_SWAP(val, 0, addr)
#define STORE_CPU_TO_LE16_WT(val, addr)	({uint16_t sval = swap_uint16(val); STORE_CPU_TO_BE16_WT(sval, addr);})
#define STORE_CPU_TO_LE32_WT(val, addr)	({uint32_t sval = swap_uint32(val); STORE_CPU_TO_BE32_WT(sval, addr);})
#define STORE_CPU_TO_LE64_WT(val, addr)	({uint64_t sval = swap_uint64(val); STORE_CPU_TO_BE64_WT(sval, addr);})

#define STORE_CPU_TO_BE16(val, addr)	({ *addr = val; })
#define STORE_CPU_TO_BE32(val, addr)	({ *addr = val; })
#define STORE_CPU_TO_BE64(val, addr)	({ *addr = val; })
#define STORE_CPU_TO_BE16_WT(val, addr)	({register uint16_t rval = val; register volatile uint16_t *raddr = addr; asm{sthwtx rval, 0, raddr} })
#define STORE_CPU_TO_BE32_WT(val, addr)	({register uint32_t rval = val; register volatile uint32_t *raddr = addr; asm{stwwtx rval, 0, raddr} })
#define STORE_CPU_TO_BE64_WT(val, addr)	({  register uint64_t rval = val; \
	                                        register volatile uint32_t *raddr = (uint32_t*)addr; \
	                                        asm{stwwtx rval@hiword, 0, raddr} \
	                                        raddr++; \
	                                        asm{stwwtx rval@loword, 0, raddr} \
	                                    })

#define LOAD_LE16_TO_CPU(addr)		LH_SWAP(0, addr)
#define LOAD_LE32_TO_CPU(addr)		LW_SWAP(0, addr)
#define LOAD_LE64_TO_CPU(addr)		LLLDW_SWAP(0, addr)

#define LOAD_BE16_TO_CPU(addr)		((uint16_t)(*addr))
#define LOAD_BE32_TO_CPU(addr)		((uint32_t)(*addr))
#define LOAD_BE64_TO_CPU(addr)		((uint64_t)(*addr))


#else  /* CORE_IS_LITTLE_ENDIAN */

#define STORE_CPU_TO_LE16(val, addr)	({ *addr = val; })
#define STORE_CPU_TO_LE32(val, addr)	({ *addr = val; })
#define STORE_CPU_TO_LE64(val, addr)	({ *addr = val; })

#define STORE_CPU_TO_BE16(val, addr)	STH_SWAP(val, 0, addr)
#define STORE_CPU_TO_BE32(val, addr)	STW_SWAP(val, 0, addr)
#define STORE_CPU_TO_BE64(val, addr)	LLSTDW_SWAP(val, 0, addr)

#define LOAD_LE16_TO_CPU(addr)		((uint16_t)(*addr))
#define LOAD_LE32_TO_CPU(addr)		((uint32_t)(*addr))
#define LOAD_LE64_TO_CPU(addr)		((uint64_t)(*addr))

#define LOAD_BE16_TO_CPU(addr)		LH_SWAP(0, addr)
#define LOAD_BE32_TO_CPU(addr)		LW_SWAP(0, addr)
#define LOAD_BE64_TO_CPU(addr)		LLLDW_SWAP(0, addr)

#endif /* CORE_IS_LITTLE_ENDIAN */


#endif
/** @} */ /* end of Endian */
