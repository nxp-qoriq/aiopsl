#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "kernel/platform.h"
#include "dplib/fsl_dpni.h"
#include "net/inet.h"

#define MAX_IPV4_STR_LEN    16              /**< Maximal size of string representation for IPV4 address*/
#define MIN_IPV4_STR_LEN    8               /**< Minimal size of string representation for IPV4 address*/
#define MAX_IPV6_STR_LEN    40              /**< Maximal size of string representation for IPV6 address*/
#define MIN_IPV6_STR_LEN    16              /**< Minimal size of string representation for IPV6 address*/
#define TEST_PASSED         1
#define TEST_FAILED         0

int aiop_pton_init(void);
void aiop_pton_free(void);
int aiop_ntop_init(void);
void aiop_ntop_free(void);

/*************************************************************************/
static int test_ntop4(uint32_t ip_addr, const char *expected_str, size_t s)
{
    char dst[MAX_IPV4_STR_LEN];
    
    if (inet_ntop(AF_INET, &ip_addr, dst, s) != NULL)
    {
        if (strncmp(expected_str, dst, s) == 0) return TEST_PASSED;
    }    
    return TEST_FAILED;
}

/*************************************************************************/
static int test_ntop6(uint16_t *ip_addr, const char *expected_str, size_t s)
{
    char dst[MAX_IPV6_STR_LEN];
    
    if (inet_ntop(AF_INET6, ip_addr, dst, s) != NULL)
    {
        if (strncmp(expected_str, dst, s) == 0) return TEST_PASSED;
    }    
    return TEST_FAILED;
}
/*************************************************************************/
static int test_pton4(const char *src, uint32_t expected_value)
{
    int res = 0;
    uint32_t ip_addr = 0;
    
    res = inet_pton(AF_INET, src, &ip_addr);
    if ((res > 0) && (ip_addr == expected_value)) return TEST_PASSED;
    
    return TEST_FAILED;
}

/*************************************************************************/
static int test_pton6(const char *src,  uint16_t * expected_value)
{
    int res = 0, i = 0;
    uint16_t ip6_addr[8];
    
    res = inet_pton(AF_INET6, src, &ip6_addr[0]);
    if (res > 0)  
    {
        for (i = 0; i < 8; i++)
        {
            if (ip6_addr[i] != expected_value[i]) return TEST_FAILED;
        }
        return TEST_PASSED;
    }
    
    return TEST_FAILED;
}

/*************************************************************************/
int aiop_pton_init(void)
{
    int err = 0;
    uint16_t ip6_addr[8] = {0x2001,0x1db8,0x85a3,0xffff,0xffff,0x8a2e,0x1370,0x7334};
    
    err = test_pton4("192.0.2.33", 0xc0000221);
    if (err != TEST_PASSED) fsl_os_print("PTON4 test %s FAILED:\n", "192.0.2.33");
    
    err = test_pton4("192.192.192.255", 0xc0c0c0ff);
    if (err != TEST_PASSED) fsl_os_print("PTON4 test %s FAILED:\n", "192.192.192.255");

    err = test_pton4("...", 0);
    if (err != TEST_PASSED) fsl_os_print("PTON4 test %s FAILED:\n", "...");

    err = test_pton6("2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", &ip6_addr[0]);
    if (err != TEST_PASSED) fsl_os_print("PTON6 test %s FAILED:\n", "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");

    ip6_addr[1] = 0xdb8; ip6_addr[2] = 0x5a3;
    err = test_pton6("2001:db8:05a3:ffff:ffff:8a2e:1370:7334", &ip6_addr[0]);
    if (err != TEST_PASSED) fsl_os_print("PTON6 test %s FAILED:\n", "2001:db8:05a3:ffff:ffff:8a2e:1370:7334");

    memset(ip6_addr, 0, 16);    
    err = test_pton6(":::::::", &ip6_addr[0]);
    if (err != TEST_PASSED) fsl_os_print("PTON6 test %s FAILED:\n", ":::::::");

    
    // TEST errors
    err = test_pton4("1920.2.33", 0);
    if (err == TEST_PASSED) fsl_os_print("PTON4 test %s FAILED:\n", "1920.2.33");
    
    err = test_pton4(".2.33", 0);
    if (err == TEST_PASSED) fsl_os_print("PTON4 test %s FAILED:\n", ".2.33");
    
    err = test_pton4(".a.33", 0);
    if (err == TEST_PASSED) fsl_os_print("PTON4 test %s FAILED:\n", ".a.33");

    err = test_pton6("20011db8:85a3:ffff:ffff:8a2e:1370:7334", NULL);
    if (err == TEST_PASSED) fsl_os_print("PTON6 test %s FAILED:\n", "20011db8:85a3:ffff:ffff:8a2e:1370:7334");
    
    err = test_pton6("2001:1db8:85a3:ffff:ffff.8a2e:1370:7334", NULL);
    if (err == TEST_PASSED) fsl_os_print("PTON6 test %s FAILED:\n", "2001:1db8:85a3:ffff.ffff:8a2e:1370:7334");
    
    err = test_pton6(":2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", NULL);
    if (err == TEST_PASSED) fsl_os_print("PTON6 test %s FAILED:\n", ":2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");

    return 0;
}

/*************************************************************************/
void aiop_pton_free(void)
{
    return;
}

/*************************************************************************/
int aiop_ntop_init(void)
{
    int err;
	uint16_t ip6_addr[8] = {0x2001,0x1db8,0x85a3,0xffff,0xffff,0x8a2e,0x1370,0x7334};

	err = test_ntop4(0x01020304, "1.2.3.4", MAX_IPV4_STR_LEN);
    if (err != TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "1.2.3.4");
    
    err = test_ntop4(0xffffffff, "255.255.255.255", MAX_IPV4_STR_LEN);
    if (err != TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "255.255.255.255");
    
    err = test_ntop4(0xfff55314, "255.245.83.20", MAX_IPV4_STR_LEN);
    if (err != TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "255.245.83.20");

    err = test_ntop6(ip6_addr, "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
    if (err != TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334");
	    	
	ip6_addr[0] = 0; ip6_addr[3] = 0;
    err = test_ntop6(ip6_addr, "0:1db8:85a3:0:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
    if (err != TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "0:1db8:85a3:0:ffff:8a2e:1370:7334");

    ip6_addr[1] = 0;
    err = test_ntop6(ip6_addr, "0:0:85a3:0:ffff:8a2e:1370:7334", MAX_IPV6_STR_LEN);
    if (err != TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "0:0:85a3:0:ffff:8a2e:1370:7334");

    // TEST errors
    err = test_ntop6(ip6_addr, "0:0:85a3:0:ffff:8a2e:1370:7334", 10); // size is too small
    if (err == TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "0:0:85a3:0:ffff:8a2e:1370:7334");

    err = test_ntop4(0x01020304, "1.2.3.4", 2); // size is too small
    if (err == TEST_PASSED) fsl_os_print("NTOP4 test %s FAILED:\n", "1.2.3.4");

	return 0;
}

/*************************************************************************/
void aiop_ntop_free(void)
{
    return;
}
