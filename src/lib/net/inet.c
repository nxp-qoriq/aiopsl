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

/**************************************************************************//*!
@File         inet.c

@Description   AIOP Service Layer Network Utilities implementation

@Cautions      None.

*//***************************************************************************/

#include "fsl_inet.h"
#include <cstring>

#define IPV4_ADDR_SIZE  4            /**< IPV4 address size in bytes */
#define IPV6_ADDR_SIZE  16           /**< IPV6 address size in bytes */
#define MAX_IPV4_STR_LEN    16       /**< Maximal size of string representation
					for IPV4 address*/
#define MIN_IPV4_STR_LEN    8        /**< Minimal size of string representation
					for IPV4 address*/
#define MAX_IPV6_STR_LEN    40       /**< Maximal size of string representation
					for IPV6 address*/
#define MIN_IPV6_STR_LEN    16       /**< Minimal size of string representation
					for IPV6 address */

#define MAX_IPV4_ADDR_DELIMITERS 3   /**< The number of . in IPV4 address */
#define MAX_IPV6_ADDR_DELIMITERS 7   /**< The number of : in IPV6 address */

/***************************************************************************/
static int pton4(const char *src, void *dst)
{

	/* returns
	1 on success (network address was successfully converted).
	0 is returned if src does not contain a character string representing
	a valid network address in the specified address family.
	 */
#if 0
	/* TODO need to define real ntohs()*/
#endif

	uint8_t  tmp[IPV4_ADDR_SIZE];
	char     ch = '\0';
	uint32_t num = 0;
	char     digit0 = '0', digit9 = '9';
	int      i = 0, del_cnt = 0;

	while (((ch = src[i]) != '\0') && (i < MAX_IPV4_STR_LEN)) {
		if ((ch <= digit9) && (ch >= digit0)) {
			/* seeing digit */
			num = (uint32_t)(ch - digit0) + (num * 10);
		} else if ((ch == '.') && (num < 0x100)) {
			/* delimiter, network is in big endian */
			tmp[del_cnt] = (uint8_t)num;
			num = 0;
			if (++del_cnt > MAX_IPV4_ADDR_DELIMITERS)
				return 0;
		} else
			return 0; /* wrong format */

		++i;
	}

	if (ch == '\0')
		tmp[del_cnt] = (unsigned char)num;
	else
		return 0; /* wrong format */

	memcpy(dst, tmp, IPV4_ADDR_SIZE);
	return 1;
}

/***************************************************************************/
static int pton6(const char *src, void *dst)
{
	/* returns
	1 on success (network address was successfully converted).
	0 is returned if src does not contain a character string representing
	a valid network address in the specified address family.
	 */
#if 0
	/* TODO add Groups of zeroes support for example - ::1
	 * define real htons() for ARM support
	 * add Dotted-quad notation support for example ::ffff:192.0.2.128
	 */
#endif
	const char digits_l[] = "0123456789abcdef";
	const char digits_u[] = "0123456789ABCDEF";
	uint16_t   tmp[IPV6_ADDR_SIZE >> 1]; /* >> 1 because of
					unsigned short type */
	int        ch = '\0';
	uint32_t   num = 0;
	char       *dig_ptr = NULL;
	int        i = 0, del_cnt = 0;

	while (((ch = src[i]) != '\0') && (i < MAX_IPV6_STR_LEN)) {
		dig_ptr = strchr(digits_l, ch);
		if (dig_ptr != NULL) {
			/* seeing digit */
			num = (uint32_t)(dig_ptr - digits_l) + (num << 4);
		} else {
			dig_ptr = strchr(digits_u, ch);
			if (dig_ptr != NULL) {
				/* seeing digit */
				num = (uint32_t)(dig_ptr - digits_u) +
					(num << 4);
			} else if ((ch == ':') && (num < 0x10000)) {
				/* delimiter, network is in big endian
				 * but host might be little endian */
				tmp[del_cnt] = htons((unsigned short)num);
				num = 0;
				if (++del_cnt > MAX_IPV6_ADDR_DELIMITERS)
					return 0;
			} else
				return 0; /* wrong format */
		}
		i++;
	}

	if (ch == '\0')
		tmp[del_cnt] = htons((unsigned short)num);
	else
		return 0; /* wrong format */

	memcpy(dst, tmp, IPV6_ADDR_SIZE);
	return 1;
}
/***************************************************************************/
int inet_pton(int af, const char *src, void *dst)
{
	int res = -1;

	switch (af) {
	case AF_INET:
		res = pton4(src, dst);
		break;
	case AF_INET6:
		res = pton6(src, dst);
		break;
	default:
		res = -1; /* af does not contain a valid address family */
		break;
	}
	return res;
}

/***************************************************************************/
const char *inet_ntop(int af, const void *src, char *dst, size_t size)
{
	int len = 0;

	switch (af) {
	case AF_INET:
	{
		char tmp[MAX_IPV4_STR_LEN];
		const char * __src = (const char *)src;

		if ((size < MIN_IPV4_STR_LEN) || (dst == NULL))
			return NULL;
		/* network is in big endian */
		len = snprintf(tmp, sizeof(tmp), "%u.%u.%u.%u", __src[0],
			       __src[1], __src[2], __src[3]);
		if ((len >= sizeof(tmp)) || (size <= len))
			return NULL;
		memcpy(dst, tmp, (uint32_t)len+1);
		return dst;
	}
	break;
	case AF_INET6:
	{
		char tmp[MAX_IPV6_STR_LEN];
		const uint16_t * __src = (const uint16_t *)src;

		if ((size < MIN_IPV6_STR_LEN) || (dst == NULL))
			return NULL;
		/* network is in big endian but host might be little endian */
		len = snprintf(tmp, sizeof(tmp), "%x:%x:%x:%x:%x:%x:%x:%x",
			       ntohs(__src[0]), ntohs(__src[1]),
			       ntohs(__src[2]), ntohs(__src[3]),
			       ntohs(__src[4]), ntohs(__src[5]),
			       ntohs(__src[6]), ntohs(__src[7]));
		if ((len >= sizeof(tmp)) || (size <= len))
			return NULL;
		memcpy(dst, tmp, (uint32_t)len + 1);
		return dst;
	}
	break;
	default:
		break;
	}
	return NULL;
}

/***************************************************************************/
uint16_t htons(uint16_t hostshort)
{
	return hostshort;
}

/***************************************************************************/
uint16_t ntohs(uint16_t netshort)
{
	return netshort;
}

/***************************************************************************/
uint32_t htonl(uint32_t hostlong)
{
	return hostlong;
}

/***************************************************************************/
uint32_t ntohl(uint32_t netlong)
{
	return netlong;
}

