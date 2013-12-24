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

int aiop_app_init(void);
void aiop_app_free(void);
int aiop_pton_init(void);
void aiop_pton_free(void);
int aiop_ntop_init(void);
void aiop_ntop_free(void);


static int init_nic(int portal_id)
{
	struct dpni_cfg			cfg;
	struct dpni_init_params	params;
	fsl_handle_t			dpni;
	int 					err;

	/* obtain default configuration of the NIC */
	dpni_defconfig(&cfg);

	dpni = dpni_open(UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
																   (uint32_t)portal_id,
																   E_MAPPED_MEM_TYPE_MC_PORTAL)),
								 10);

	memset(&params, 0, sizeof(params));
	params.type = DPNI_TYPE_NIC;
	err = dpni_init(dpni, &cfg, &params);
	if (err)
		return err;

	return 0;
}


int aiop_app_init(void)
{
    int err = 0;

    fsl_os_print("AIOP test: NIC\n");

    err = init_nic(2);

    return err;
}

void aiop_app_free(void)
{
    /* TODO - complete!*/
}

int aiop_pton_init(void)
{
    int err = 0;
    char dst[MAX_IPV6_STR_LEN];
    char src[MAX_IPV4_STR_LEN] = "192.0.2.33";
    char str6[MAX_IPV6_STR_LEN] = "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334";
    uint32_t dst4 = 0;
    
    fsl_os_print("PTON4 test init:\n");
    
    err = inet_pton(AF_INET, "192.0.2.33", &dst[0]);
    if ((dst[0] == 192) && (dst[1] == 0) && (dst[2] == 2) && (dst[3] == 33) && (err > 0))
        fsl_os_print("PTON %u.%u.%u.%u --> PASSED \n", dst[0], dst[1], dst[2], dst[3]);
    else
        fsl_os_print("PTON %u.%u.%u.%u --> FAILED \n", dst[0], dst[1], dst[2], dst[3]);

    err = inet_pton(AF_INET, &src[0], &dst[0]);
    if ((dst[0] == 192) && (dst[1] == 0) && (dst[2] == 2) && (dst[3] == 33) && (err > 0))
        fsl_os_print("PTON %s == %u.%u.%u.%u --> PASSED \n", src, dst[0], dst[1], dst[2], dst[3]);
    else
        fsl_os_print("PTON %u.%u.%u.%u --> FAILED \n", dst[0], dst[1], dst[2], dst[3]);
    

    src[4] = '1'; src[8] = '1'; src[12] = '1';
    err = inet_pton(AF_INET, &src[0], &dst[0]);
    if ((dst[0] == 192) && (dst[1] == 1) && (dst[2] == 2) && (dst[3] == 13) && (err > 0))
        fsl_os_print("PTON %s == %u.%u.%u.%u --> PASSED \n", src, dst[0], dst[1], dst[2], dst[3]);
    else
        fsl_os_print("PTON %s != %u.%u.%u.%u --> FAILED \n", src, dst[0], dst[1], dst[2], dst[3]);

    
    err = inet_pton(AF_INET6, &str6[0], &dst[0]);
    if ((((unsigned short *)dst)[0] == 0x2001) && (((unsigned short *)dst)[7] == 0x7334) && 
        (((unsigned short *)dst)[1] == 0x1db8) && (((unsigned short *)dst)[2] == 0x85a3) && (err > 0))
        fsl_os_print("PTON %s  --> PASSED \n", str6);
    else
        fsl_os_print("PTON %s --> FAILED \n", str6);

    strcpy(str6,"0:0:0:f56:2:1:0:0");
    err = inet_pton(AF_INET6, &str6[0], &dst[0]);
    if ((((unsigned short *)dst)[0] == 0) && (((unsigned short *)dst)[7] == 0) && (((unsigned short *)dst)[5] == 1) &&  
        (((unsigned short *)dst)[1] == 0) && (((unsigned short *)dst)[2] == 0xf56) && (err > 0))
        fsl_os_print("PTON %s  --> PASSED \n", str6);
    else
        fsl_os_print("PTON %s --> FAILED \n", str6);
    
    dst4 = 0x01050709;
    inet_ntop(AF_INET, &dst4, &src[0], MAX_IPV4_STR_LEN);
    if ((src[0]== '1') && (src[2] == '5') && (src[4] == '7') && (src[6] == '9'))
        fsl_os_print("NTOP4 0x%lx == %s ---> PASSED \n", (uint32_t)dst4, src);
    else
        fsl_os_print("NTOP4 0x%lx != %s ---> FAILED \n", (uint32_t)dst4, src);

    return 0;
}

void aiop_pton_free(void)
{
    fsl_os_print("PTON test free:\n");        
}

int aiop_ntop_init(void)
{
	char dst[MAX_IPV6_STR_LEN];
	uint32_t ip_addr = 0x01020304;
	uint16_t ip6_addr[8] = {0x2001,0x1db8,0x85a3,0xffff,0xffff,0x8a2e,0x1370,0x7334};

	fsl_os_print("NTOP test init:\n");

	if (inet_ntop(AF_INET, &ip_addr, &dst[0], sizeof(dst)) != NULL)
	{
		fsl_os_print("NTOP 0x%x == %s\n",ip_addr, dst);
	}

	ip_addr = 0xfff55314;
	if ((inet_ntop(AF_INET, &ip_addr, &dst[0], sizeof(dst)) != NULL) && (strncmp(dst, "255.245.83.20", 13) == 0))
	{	   
		fsl_os_print("NTOP 0x%x == %s ---> PASSED \n",ip_addr, dst);
	}
	else fsl_os_print("NTOP 0x%x != %s ---> FAILED \n",ip_addr, dst);
	    
	if ((inet_ntop(AF_INET6, ip6_addr, &dst[0], sizeof(dst)) != NULL) && (strncmp(dst, "2001:1db8:85a3:ffff:ffff:8a2e:1370:7334", 39) == 0))
	{
		fsl_os_print("NTOP %x:%x:%x:%x:%x:%x:%x:%x: == %s ---> PASSED\n",
				ip6_addr[0],ip6_addr[1],ip6_addr[2],ip6_addr[3],ip6_addr[4],ip6_addr[5],ip6_addr[6],ip6_addr[7],dst);
	}
	else fsl_os_print("NTOP %x:%x:%x:%x:%x:%x:%x:%x: != %s ---> FAILED\n",
	                  ip6_addr[0],ip6_addr[1],ip6_addr[2],ip6_addr[3],ip6_addr[4],ip6_addr[5],ip6_addr[6],ip6_addr[7],dst);
	
	ip6_addr[0] = 0; ip6_addr[3] = 0;
    if ((inet_ntop(AF_INET6, ip6_addr, &dst[0], sizeof(dst)) != NULL) && (strncmp(dst, "0:1db8:85a3:0:ffff:8a2e:1370:7334", 25) == 0))
    {
        fsl_os_print("NTOP %x:%x:%x:%x:%x:%x:%x:%x: == %s ---> PASSED\n",
                ip6_addr[0],ip6_addr[1],ip6_addr[2],ip6_addr[3],ip6_addr[4],ip6_addr[5],ip6_addr[6],ip6_addr[7],dst);
    }
    else fsl_os_print("NTOP %x:%x:%x:%x:%x:%x:%x:%x: != %s ---> FAILED\n",
                      ip6_addr[0],ip6_addr[1],ip6_addr[2],ip6_addr[3],ip6_addr[4],ip6_addr[5],ip6_addr[6],ip6_addr[7],dst);


	return 0;
}

void aiop_ntop_free(void)
{
    fsl_os_print("NTOP test free:\n");        
}
