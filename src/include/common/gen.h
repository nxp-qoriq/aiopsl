 /**************************************************************************//**
 @File          gen.h

 @Description   General ARENA Standard Definitions
*//***************************************************************************/

#ifndef __FSL_ARENA_GEN_H
#define __FSL_ARENA_GEN_H

#include "common/types.h"


#define PTR_TO_UINT(_ptr)           ((uintptr_t)(_ptr))
#define UINT_TO_PTR(_val)           ((void*)(uintptr_t)(_val))

#define PTR_MOVE(_ptr, _offset)     (void*)((uint8_t*)(_ptr) + (_offset))


#define MAKE_UINT64(high32, low32)      (((uint64_t)high32 << 32) | (low32))


/*----------------------*/
/* Miscellaneous macros */
/*----------------------*/

#define UNUSED(_x)	((void)(_x))

#define KILOBYTE            0x400UL                 /* 1024 */
#define MEGABYTE            (KILOBYTE * KILOBYTE)   /* 1024*1024 */
#define GIGABYTE            ((uint64_t)(KILOBYTE * MEGABYTE))   /* 1024*1024*1024 */
#define TERABYTE            ((uint64_t)(KILOBYTE * GIGABYTE))   /* 1024*1024*1024*1024 */


/* Macro for checking if a number is a power of 2 */
#define POWER_OF_2(n)   (!((n) & ((n)-1)))

/* Macro for calculating log of base 2 */
//TODO: replace with static inline function, if needed
#define LOG2(num, log2_num)     \
    do                          \
    {                           \
        uint64_t tmp = (num);   \
        log2_num = 0;           \
        while (tmp > 1)         \
        {                       \
            log2_num++;         \
            tmp >>= 1;          \
        }                       \
    } while (0)

//TODO: replace with static inline function, if needed
#define NEXT_POWER_OF_2(_num, _next_pow)\
do                                      \
{                                       \
    if (POWER_OF_2(_num))               \
        _next_pow = (_num);             \
    else                                \
    {                                   \
        uint64_t tmp = (_num);          \
        _next_pow = 1;                  \
        while (tmp)                     \
        {                               \
            _next_pow <<= 1;            \
            tmp >>= 1;                  \
        }                               \
    }                                   \
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
#define IN_RANGE(min,val,max) ((min)<=(val) && (val)<=(max))

#define ABS(a)  ((a<0)?(a*-1):a)

#if !(defined(ARRAY_SIZE))
#define ARRAY_SIZE(arr)   (sizeof(arr) / sizeof((arr)[0]))
#endif /* !defined(ARRAY_SIZE) */

#ifndef IS_ALIGNED
#define IS_ALIGNED(n,align)     (!((uint32_t)(n) & (align - 1)))
#endif /* IS_ALIGNED */

#define ILLEGAL_BASE    (~0)

#define ARENA_MASTER_PART_ID      (0)
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


static __inline__ uint64_t u64_read_field(uint64_t reg, int start_bit, int size)
{
    if (size >= 64)
        return reg;
    return (reg >> start_bit) & ((0x0000000000000001LL << size)-1);
}

static __inline__ void u64_write_field(uint64_t reg, int start_bit, int size, uint64_t val)
{
    if (size >= 64)
        reg = val;
    else
        reg |= (uint64_t)(val & ((0x0000000000000001LL << size) - 1) << start_bit);
}


#endif /* __FSL_ARENA_GEN_H */
