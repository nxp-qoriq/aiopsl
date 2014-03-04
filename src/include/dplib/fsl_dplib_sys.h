#ifndef _FSL_DPLIB_SYS_H
#define _FSL_DPLIB_SYS_H

#ifdef __linux__
#ifdef __uboot__

#define dmb()           __asm__ __volatile__ ("" : : : "memory")
#define __iormb()       dmb()
#define __iowmb()       dmb()
#define __arch_getq(a)                  (*(volatile unsigned long *)(a))
#define __arch_putq(v,a)                (*(volatile unsigned long *)(a) = (v))
#define readq(c)        ({ u64 __v = __arch_getq(c); __iormb(); __v; })
#define writeq(v,c)     ({ u64 __v = v; __iowmb(); __arch_putq(__v,c); __v; })
#include <common.h>
#include <errno.h>
#include <asm/io.h>

#else

#include <linux/errno.h>
#include <asm/io.h>

#endif // __uboot__

#ifndef ENOTSUP
#define ENOTSUP		95
#endif

#define PTR_TO_UINT(_ptr)       ((uintptr_t)(_ptr))
#define PTR_MOVE(_ptr, _offset)	(void*)((uint8_t*)(_ptr) + (_offset))

#define ioread64(_p)	    readq(_p)
#define iowrite64(_v, _p)   writeq(_v, _p)

#else /* __linux__ */

#include "common/types.h"
#include "common/errors.h"
#include "common/io.h"

#endif /* __linux__ */

#if (!defined(DECLARE_UINT_CODEC))

#define MAKE_UMASK64(_width) \
	((uint64_t)((_width) < 64 ? ((uint64_t)1 << (_width)) - 1 \
				  : -1))

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

DECLARE_UINT_CODEC(64)

#endif

#endif /* _FSL_DPLIB_SYS_H */
