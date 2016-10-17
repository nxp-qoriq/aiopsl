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
 @File          fsl_gen.h

 @Description   General FSL OS Standard Definitions
*//***************************************************************************/

#ifndef __FSL_SYS_GEN_H
#define __FSL_SYS_GEN_H

#include "fsl_types.h"


#define PTR_TO_UINT(_ptr)       ((uintptr_t)(_ptr))
#define UINT_TO_PTR(_val)       ((void*)(uintptr_t)(_val))

#define PTR_MOVE(_ptr, _offset)	(void*)((uint8_t*)(_ptr) + (_offset))

#define MAKE_UINT64(_h32, _l32)	(((uint64_t)_h32 << 32) | (_l32))
#define UINT32_LO(_w64)		((uint32_t)_w64)
#define UINT32_HI(_w64)		(uint32_t)((_w64)>>32)

#define MAKE_UMASK32(_width)	(uint32_t)(((uint64_t)1 << (_width)) - 1)
#define MAKE_UMASK64(_width)	((uint64_t)((_width) < 64 ? ((uint64_t)1 << (_width)) - 1 \
                                 : (uint64_t)-1))

#define OS_MEM_RESERVED(start_addr,next_addr) volatile uint8_t reserved##start_addr [next_addr - start_addr] 
/**< Reserved uint8_t space generation; Useful for memory maps */

#define IS_SIM ((booke_get_spr_PVR() & 0x0c000000) == 0x0c000000)
/**< Indication if using HW or Simulator */

/*----------------------*/
/* Miscellaneous macros */
/*----------------------*/

#define UNUSED(_x)	((void)(_x))

#define KILOBYTE	1024UL
#define MEGABYTE	(KILOBYTE * KILOBYTE)
#define GIGABYTE	((KILOBYTE * MEGABYTE))

/* Macro for checking if a number is a power of 2 */
#define is_power_of_2(n)   (!((n) & ((n)-1)))

/* Macro for calculating log of base 2 */
//TODO: replace with static inline function, if needed
#define LOG2(_num, _log2_num)   \
    do  {                       \
        uint64_t tmp = (_num);  \
        _log2_num = 0;          \
        while (tmp > 1) {       \
            _log2_num++;        \
            tmp >>= 1;          \
        }                       \
    } while (0)

//TODO: replace with static inline function, if needed
#define NEXT_POWER_OF_2(_num, _next_pow) \
    do {                                 \
        if (is_power_of_2(_num))         \
            _next_pow = (_num);          \
        else {                           \
            uint64_t tmp = (_num);       \
            _next_pow = 1;               \
            while (tmp) {                \
                _next_pow <<= 1;         \
                tmp >>= 1;               \
            }                            \
        }                                \
    } while (0)

/* Ceiling division - not the fastest way, but safer in terms of overflow */
#define DIV_CEIL(x,y)   (((x)/(y)) + ((((((x)/(y)))*(y)) == (x)) ? 0 : 1))

/* Round up a number to be a multiple of a second number */
#define ROUND_UP(x,y)   ((((x) + (y) - 1) / (y)) * (y))

/* Timing macro for converting usec units to number of ticks.   */
/* (number of usec *  clock_Hz) / 1,000,000) - since            */
/* clk is in MHz units, no division needed.                     */
#define USEC_TO_CLK(usec,clk)       ((usec) * (clk))
#define CYCLES_TO_USEC(cycles,clk)  ((cycles) / (clk))

/* Timing macros for converting between nsec units and number of clocks. */
#define NSEC_TO_CLK(nsec,clk)       DIV_CEIL(((nsec) * (clk)), 1000)
#define CYCLES_TO_NSEC(cycles,clk)  (((cycles) * 1000) / (clk))

/* Timing macros for converting between psec units and number of clocks. */
#define PSEC_TO_CLK(psec,clk)       DIV_CEIL(((psec) * (clk)), 1000000)
#define CYCLES_TO_PSEC(cycles,clk)  (((cycles) * 1000000) / (clk))

/* Min, Max macros */
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#define MAX(a,b)    ((a) > (b) ? (a) : (b))

#define IN_RANGE(_min,_val,_max)	((_min)<=(_val) && (_val)<=(_max))

#define ABS(a)  ((a<0)?(a*-1):a)

#if !(defined(ARRAY_SIZE))
#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))
#endif /* !defined(ARRAY_SIZE) */
#define ARRAY_EL_SIZE(arr)	(sizeof((arr)[0]))

#ifndef IS_ALIGNED
#define IS_ALIGNED(n,align)     (!((uint32_t)(n) & (align - 1)))
#endif /* IS_ALIGNED */

#ifndef ALIGN_UP
#define ALIGN_UP(ADDRESS, ALIGNMENT)           \
        ((((uint32_t)(ADDRESS)) + ((uint32_t)(ALIGNMENT)) - 1) & (~(((uint32_t)(ALIGNMENT)) - 1)))
        /**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#endif /* ALIGN_UP */

#ifndef ALIGN_UP_64
#define ALIGN_UP_64(ADDRESS, ALIGNMENT)           \
        ((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1LL) & (~(((uint64_t)(ALIGNMENT)) - 1LL)))
        /**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#endif /* ALIGN_UP */

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(ADDRESS, ALIGNMENT)      \
        ((uint32_t)(ADDRESS) & (~(((uint32_t)(ALIGNMENT)) - 1)))
        /**< Align a given address to a lower aligned address - equivalent to floor(ADDRESS,ALIGNMENT) */
#endif /* ALIGN_DOWN */

#ifndef MODULU_POWER_OF_TWO
#define MODULU_POWER_OF_TWO(NUM, MOD) \
	((uint32_t)(NUM) & ((uint32_t)(MOD) - 1))
#endif

#define ILLEGAL_BASE    (~0)	//TODO - remove

/* @} */

/* The following 3 lines must be located at the top of the header file */
	/** Macros to verify a structure size does not exceed the size a user
	 * allocates for that structure.
	 * If verification fails the following error will be presented (on
	 * compile time):
	 * error: "division by 0". */
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#define ASSERT_STRUCT_SIZE(_sizeof_struct, _struct_fixed_size) \
	ASSERT_STRUCT_SIZE_(_sizeof_struct <= _struct_fixed_size)
#define ASSERT_STRUCT_SIZE_(e) \
	enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }


#define DECLARE_UINT_CODEC(w) \
static inline uint##w##_t u##w##_enc(int lsoffset, int width, uint##w##_t val) \
{ \
	return (uint##w##_t)(((uint##w##_t)val & MAKE_UMASK##w(width)) << lsoffset); \
} \
static inline uint##w##_t u##w##_dec(uint##w##_t val, int lsoffset, int width) \
{ \
	return (uint##w##_t)((val >> lsoffset) & MAKE_UMASK##w(width)); \
} \
static inline uint##w##_t u##w##_clr(uint##w##_t val, int lsoffset, int width) \
{ \
	return (uint##w##_t)(val & ~(MAKE_UMASK##w(width) << lsoffset)); \
} \
static inline uint##w##_t u##w##_iso(uint##w##_t val, int lsoffset, int width) \
{ \
	return (uint##w##_t)(val & (MAKE_UMASK##w(width) << lsoffset)); \
} \
static inline uint##w##_t u##w##_rmw(uint##w##_t val, int lsoffset, int width, uint##w##_t new_val) \
{ \
	return (uint##w##_t)(u##w##_clr(val, lsoffset, width) | u##w##_enc(lsoffset, width, new_val)); \
}

DECLARE_UINT_CODEC(32)
DECLARE_UINT_CODEC(64)

#endif /* __FSL_SYS_GEN_H */
