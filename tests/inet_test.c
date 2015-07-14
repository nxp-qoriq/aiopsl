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

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "platform.h"
#include "fsl_inet.h"
#include "fsl_dbg.h"

#define MAX_IPV4_STR_LEN    16              /**< Maximal size of string representation for IPV4 address*/
#define MIN_IPV4_STR_LEN    8               /**< Minimal size of string representation for IPV4 address*/
#define MAX_IPV6_STR_LEN    40              /**< Maximal size of string representation for IPV6 address*/
#define MIN_IPV6_STR_LEN    16              /**< Minimal size of string representation for IPV6 address*/
#define TEST_PASSED         0
#define TEST_FAILED        -1

int pton_test(void);
int ntop_test(void);

/*************************************************************************/
#define PRINT_TEST_RESULT(PASS, MODULE, TEST) \
	do { \
		if (PASS) { \
			fsl_os_print("%s test %s PASSED\n",MODULE, TEST); \
		} else { \
			fsl_os_print("%s test %s ************* FAILED **************\n",MODULE, TEST); \
			return TEST_FAILED; \
		} \
	} while (0)

/*************************************************************************/
static int test_ntopton4(uint32_t ip_addr, const char *expected_str, size_t s)
{
	char     dst[MAX_IPV4_STR_LEN];
	uint32_t net_addr = 0;

	if (inet_ntop(AF_INET, &ip_addr, dst, s) != NULL) {
		if (strncmp(expected_str, dst, s) == 0) {
			inet_pton(AF_INET, dst, &net_addr);
			if (net_addr == ip_addr) return TEST_PASSED;
		}
	}

	return TEST_FAILED;
}
/*************************************************************************/
static int test_ntopton6(uint16_t *ip_addr, const char *expected_str, size_t s)
{
	char     dst[MAX_IPV6_STR_LEN];
	uint16_t ip6_addr[8];
	int      i;

	if (inet_ntop(AF_INET6, ip_addr, dst, s) != NULL) {
		if (strncmp(expected_str, dst, s) == 0) {
			inet_pton(AF_INET6, dst, ip6_addr);
			for (i = 0; i < 8; i++) {
				if (ip6_addr[i] != ip_addr[i]) return TEST_FAILED;
			}
			return TEST_PASSED;
		}
	}

	return TEST_FAILED;
}
/*************************************************************************/
static int test_ntop4(uint32_t ip_addr, const char *expected_str, size_t s)
{
	char dst[MAX_IPV4_STR_LEN];

	if (inet_ntop(AF_INET, &ip_addr, dst, s) != NULL) {
		if (strncmp(expected_str, dst, s) == 0) return TEST_PASSED;
	}

	return TEST_FAILED;
}

/*************************************************************************/
static int test_ntop6(uint16_t *ip_addr, const char *expected_str, size_t s)
{
	char dst[MAX_IPV6_STR_LEN];

	if (inet_ntop(AF_INET6, ip_addr, dst, s) != NULL) {
		if (strncmp(expected_str, dst, s) == 0) return TEST_PASSED;
	}

	return TEST_FAILED;
}
/*************************************************************************/
static int test_pton4(const char *src, uint32_t expected_value)
{
	int      res = 0;
	uint32_t ip_addr = 0;

	res = inet_pton(AF_INET, src, &ip_addr);
	if ((res > 0) && (ip_addr == expected_value)) return TEST_PASSED;

	return TEST_FAILED;
}

/*************************************************************************/
static int test_pton6(const char *src,  uint16_t * expected_value)
{
	int      res = 0, i = 0;
	uint16_t ip6_addr[8];

	res = inet_pton(AF_INET6, src, &ip6_addr[0]);
	if (res > 0) {
		for (i = 0; i < 8; i++) {
			ASSERT_COND(expected_value != NULL);
			if (ip6_addr[i] != expected_value[i]) return TEST_FAILED;
		}
		return TEST_PASSED;
	}

	return TEST_FAILED;
}

/*************************************************************************/
static int test_ptontop4(const char *src, uint32_t expected_value)
{
	int      res = 0;
	uint32_t ip_addr = 0;
	char     dst[MAX_IPV4_STR_LEN];

	res = inet_pton(AF_INET, src, &ip_addr);
	if ((res > 0) && (ip_addr == expected_value)) {
		if (inet_ntop(AF_INET, &ip_addr, dst, MAX_IPV4_STR_LEN) != NULL) {
			if (strncmp(src, dst, MAX_IPV4_STR_LEN) == 0) return TEST_PASSED;
		}
	}

	return TEST_FAILED;
}

/*************************************************************************/
static int test_ptontop6(const char *src,  uint16_t * expected_value)
{
	int      res = 0, i = 0;
	uint16_t ip6_addr[8];
	char     dst[MAX_IPV6_STR_LEN];

	res = inet_pton(AF_INET6, src, &ip6_addr[0]);
	if (res > 0) {
		for (i = 0; i < 8; i++) {
			ASSERT_COND(expected_value != NULL);
			if (ip6_addr[i] != expected_value[i]) return TEST_FAILED;
		}
		if (inet_ntop(AF_INET6, ip6_addr, dst, MAX_IPV6_STR_LEN) != NULL) {
			if (strncmp(src, dst, MAX_IPV6_STR_LEN) == 0) return TEST_PASSED;
		}
	}

	return TEST_FAILED;
}
/*************************************************************************/
int pton_test(void)
{
	int err = 0;
	uint16_t ip6_addr[8] = {0x2001,0x1db8,0x85a3,0xffff,0xffff,0x8a2e,0x1370,0x7334};

	err = test_pton4("192.0.2.33", 0xc0000221);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON4", "192.0.2.33");
	err = test_ptontop4("192.0.2.33", 0xc0000221);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON4", "192.0.2.33");

	err = test_pton4("192.192.192.255", 0xc0c0c0ff);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON4", "192.192.192.255");
	err = test_ptontop4("192.192.192.255", 0xc0c0c0ff);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON4", "192.192.192.255");

	err = test_pton4("...", 0);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON4", "...");
	err = test_ptontop4("0.0.0.0", 0);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON4", "0.0.0.0");

	err = test_pton6("2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", &ip6_addr[0]);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON6", "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");
	err = test_ptontop6("2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", &ip6_addr[0]);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON6", "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");

	ip6_addr[1] = 0xdb8; ip6_addr[2] = 0x5a3;
	err = test_pton6("2001:db8:05a3:ffff:ffff:8a2e:1370:7334", &ip6_addr[0]);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON6", "2001:db8:05a3:ffff:ffff:8a2e:1370:7334");

	memset(ip6_addr, 0, 16);
	err = test_pton6(":::::::", &ip6_addr[0]);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON6", ":::::::");
	err = test_ptontop6("0:0:0:0:0:0:0:0", &ip6_addr[0]);
	PRINT_TEST_RESULT(err == TEST_PASSED, "PTON6", "0:0:0:0:0:0:0:0");

	/* TEST errors */
	err = test_pton4("1920.2.33", 0);
	PRINT_TEST_RESULT(err != TEST_PASSED, "PTON6", "1920.2.33");

	err = test_pton4(".2.33", 0);
	PRINT_TEST_RESULT(err != TEST_PASSED, "PTON6", ".2.33");

	err = test_pton4(".a.33", 0);
	PRINT_TEST_RESULT(err != TEST_PASSED, "PTON4", ".A.33");

	err = test_pton6("20011db8:85a3:ffff:ffff:8a2e:1370:7334", NULL);
	PRINT_TEST_RESULT(err != TEST_PASSED, "PTON6", "20011db8:85a3:ffff:ffff:8a2e:1370:7334");

	err = test_pton6("2001:1db8:85a3:ffff:ffff.8a2e:1370:7334", NULL);
	PRINT_TEST_RESULT(err != TEST_PASSED, "PTON6", "2001:1db8:85a3:ffff.ffff:8a2e:1370:7334");

	err = test_pton6(":2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", NULL);
	PRINT_TEST_RESULT(err != TEST_PASSED, "PTON6", ":2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");

	return 0;
}

/*************************************************************************/
int ntop_test(void)
{
	int err;
	uint16_t ip6_addr[8] = {0x2001,0x1db8,0x85a3,0xffff,0xffff,0x8a2e,0x1370,0x7334};

	err = test_ntop4(0x01020304, "1.2.3.4", MAX_IPV4_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP4", "1.2.3.4");
	err = test_ntopton4(0x01020304, "1.2.3.4", MAX_IPV4_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP4", "1.2.3.4");

	err = test_ntop4(0xffffffff, "255.255.255.255", MAX_IPV4_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP4", "255.255.255.255");
	err = test_ntopton4(0xffffffff, "255.255.255.255", MAX_IPV4_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP4", "255.255.255.255");

	err = test_ntop4(0xfff55314, "255.245.83.20", MAX_IPV4_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP4", "255.245.83.20");
	err = test_ntopton4(0xfff55314, "255.245.83.20", MAX_IPV4_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP4", "255.245.83.20");

	err = test_ntop6(ip6_addr, "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP6", "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");
	err = test_ntopton6(ip6_addr, "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP6", "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");

	ip6_addr[0] = 0; ip6_addr[3] = 0;
	err = test_ntop6(ip6_addr, "0:1db8:85a3:0:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP6", "0:1db8:85a3:0:ffff:8a2e:1370:7334");

	ip6_addr[1] = 0;
	err = test_ntop6(ip6_addr, "0:0:85a3:0:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP6", "0:0:85a3:0:ffff:8a2e:1370:7334");
	err = test_ntopton6(ip6_addr, "0:0:85a3:0:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
	PRINT_TEST_RESULT(err == TEST_PASSED, "NTOP6", "0:0:85a3:0:ffff:8a2e:1370:7334");

	/* TEST errors */
	err = test_ntop6(ip6_addr, "0:0:85a3:0:ffff:8a2e:1370:7334", 10); // size is too small
	PRINT_TEST_RESULT(err != TEST_PASSED, "NTOP6", "0:0:85a3:0:ffff:8a2e:1370:7334");

	err = test_ntop4(0x01020304, "1.2.3.4", 2); // size is too small
	PRINT_TEST_RESULT(err != TEST_PASSED, "NTOP4", "1.2.3.4 size 2");

	return 0;
}

