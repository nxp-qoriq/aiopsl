#ifndef _FSL_DPLIB_SYS_H
#define _FSL_DPLIB_SYS_H

#include "common/types.h"
#include "common/errors.h"
#include "common/io.h"


#if (!defined(DECLARE_UINT_CODEC))

#define MAKE_UMASK64(_width)	(uint64_t)(((uint64_t)1 << (_width)) - 1)

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
