/*
 * Copyright 2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause or GPL-2.0+
 */

#ifndef __RTA_COMPAT_H__
#define __RTA_COMPAT_H__

#include <stdint.h>
#include <errno.h>

#ifdef __GLIBC__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <byteswap.h>
/* FSL's Embedded Warrior C Library; assume AIOP or MC environment */
#elif defined(__EWL__) && (defined(AIOP) || defined(MC))
#include "common/fsl_string.h"
#include "common/fsl_stdlib.h"
#include "common/fsl_stdio.h"
#if defined(AIOP)
#include "dplib/fsl_cdma.h"
#endif
#include "fsl_dbg.h"
#include "fsl_endian.h"
#if _EWL_C99
#include <stdbool.h>
#else
#if !__option(c99)
typedef unsigned char			_Bool;
#endif
#define bool				_Bool
#define true				1
#define false				0
#define __bool_true_false_are_defined	1
#endif /* _EWL_C99 */
#else
#error Environment not supported!
#endif

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif

#ifndef __maybe_unused
#define __maybe_unused __attribute__((unused))
#endif

#if defined(__GLIBC__) && !defined(pr_debug)
#if !defined(SUPPRESS_PRINTS) && defined(RTA_DEBUG)
#define pr_debug(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif
#endif /* pr_debug */

#if defined(__GLIBC__) && !defined(pr_err)
#if !defined(SUPPRESS_PRINTS)
#define pr_err(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define pr_err(fmt, ...)
#endif
#endif /* pr_err */

#if defined(__GLIBC__) && !defined(pr_warning)
#if !defined(SUPPRESS_PRINTS)
#define pr_warning(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define pr_warning(fmt, ...)
#endif
#endif /* pr_warning */

#if defined(__GLIBC__) && !defined(pr_warn)
#define pr_warn	pr_warning
#endif /* pr_warn */

/**
 * ARRAY_SIZE - returns the number of elements in an array
 * @x: array
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN
#define ALIGN(x, a) (((x) + ((__typeof__(x))(a) - 1)) & \
			~((__typeof__(x))(a) - 1))
#endif

#ifndef BIT
#define BIT(nr)		(1UL << (nr))
#endif

#ifndef upper_32_bits
/**
 * upper_32_bits - return bits 32-63 of a number
 * @n: the number we're accessing
 */
#define upper_32_bits(n) ((uint32_t)(((n) >> 16) >> 16))
#endif

#ifndef lower_32_bits
/**
 * lower_32_bits - return bits 0-31 of a number
 * @n: the number we're accessing
 */
#define lower_32_bits(n) ((uint32_t)(n))
#endif

/* Use Linux naming convention */
#ifdef __GLIBC__
#define swab16(x) bswap_16(x)
#define swab32(x) bswap_32(x)
#define swab64(x) bswap_64(x)
#if !defined(cpu_to_be32) && !defined(cpu_to_le32)
#ifdef __BIG_ENDIAN
#define cpu_to_be32(x)	(x)
#define cpu_to_le32(x)	swab32(x)
#elif defined(__LITTLE_ENDIAN)
#define cpu_to_be32(x)	swab32(x)
#define cpu_to_le32(x)	(x)
#else
#error Endianness not set in environment!
#endif /* __BIG_ENDIAN */
#endif /* !defined(cpu_to_be32) && !defined(cpu_to_le32) */
#elif defined(__EWL__) && (defined(AIOP) || defined(MC))
#define swab16(x) swap_uint16(x)
#define swab32(x) swap_uint32(x)
#define swab64(x) swap_uint64(x)
#define cpu_to_be32(x)	CPU_TO_BE32(x)
#define cpu_to_le32(x)	CPU_TO_LE32(x)
#ifdef CORE_IS_BIG_ENDIAN
#define __BIG_ENDIAN
#elif defined(CORE_IS_LITTLE_ENDIAN)
#define __LITTLE_ENDIAN
#else
#error Endianness not set in environment!
#endif /* CORE_IS_BIG_ENDIAN */
#endif

#endif /* __RTA_COMPAT_H__ */
