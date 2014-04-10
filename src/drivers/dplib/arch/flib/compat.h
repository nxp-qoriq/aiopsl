/* Copyright 2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_COMPAT_H__
#define __RTA_COMPAT_H__

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __GLIBC__
#include <byteswap.h>
/* FSL's Embedded Warrior C Library; assume AIOP or MC environment */
#elif defined(__EWL__) && defined(AIOP)
#include "general.h"
#elif defined(__EWL__) && defined(MC)
#include "common/endian.h"
#else
#error Environment not supported!
#endif

#ifndef pr_debug
#ifdef RTA_DEBUG
#define pr_debug(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif
#endif

/**
 * ARRAY_SIZE(x) - Returns the number of elements in an array
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN
#define ALIGN(x, a) (((x) + ((__typeof__(x))(a) - 1)) & \
			~((__typeof__(x))(a) - 1))
#endif

/* Use Linux naming convention */
#ifdef __GLIBC__
# define swab16 bswap_16
# define swab32 bswap_32
# define swab64 bswap_64
#elif defined(__EWL__) && defined(AIOP)
# define swab16 LH_SWAP
# define swab32 LW_SWAP
# define swab64 LDW_SWAP
#elif defined(__EWL__) && defined(MC)
# define swab16 SWAP_UINT16
# define swab32 SWAP_UINT32
# define swab64 SWAP_UINT64
#endif

#endif /* __RTA_COMPAT_H__ */
