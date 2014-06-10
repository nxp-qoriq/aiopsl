/**************************************************************************//**
 @Group		ENDIAN

 @Description	General macros and functions

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
#include "common/fsl_core.h"

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

#define STORE_CPU_TO_BE16(val, addr)	({ *addr = val; })
#define STORE_CPU_TO_BE32(val, addr)	({ *addr = val; })
#define STORE_CPU_TO_BE64(val, addr)	({ *addr = val; })

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
