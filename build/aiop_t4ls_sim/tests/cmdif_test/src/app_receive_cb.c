#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "common/fsl_aiop_cmdif.h"
#include "fsl_cdma.h"
#include "io.h"
#include "cmdif_client.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"

extern void client_open_cmd(struct cmdif_desc *client, void *sync_done); 
extern void client_close_cmd(struct cmdif_desc *client); 
extern void client_sync_cmd(struct cmdif_desc *client); 
extern void client_async_cmd(struct cmdif_desc *client); 
extern void client_no_resp_cmd(struct cmdif_desc *client); 
extern void cmdif_srv_isr();

__SHRAM int received_fd = 0;
uint64_t sync_done;

/**< DDR, server accesses it with CDMA, 4 byte is enough */
__SHRAM struct cmdif_desc client_desc;
/**< Client descriptor that should be passed to open */
struct cmdif_dev client_dev;

#define     INVALID_AUTH_ID  1001

void app_receive_cb (void);
void app_receive_cb (void) 
{    
	uint32_t auth_id = 0;
	int test_id = received_fd % 13;
	sync_done = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	
	cdma_read(&auth_id, sync_done, 4);
	auth_id &= 0x0000FFFF;
	
	fsl_os_print("\n Starting test number %d\n", test_id);
	switch (test_id) {
	case 0:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_open_cmd(&client_desc, &sync_done); 
		break;
	case 1:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_sync_cmd(&client_desc); 
		break;
	case 2:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_async_cmd(&client_desc); 
		break;
	case 3:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_no_resp_cmd(&client_desc); 
		break;
	case 4:
		fsl_os_print("EXPECTED RESULT: FAILED \n");
		((struct cmdif_dev *)client_desc.dev)->auth_id = INVALID_AUTH_ID;
		client_sync_cmd(&client_desc); 		
		break;
	case 5:
		fsl_os_print("EXPECTED RESULT: FAILED \n");
		((struct cmdif_dev *)client_desc.dev)->auth_id = INVALID_AUTH_ID;
		client_async_cmd(&client_desc);
		break;
	case 6:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_sync_cmd(&client_desc); 
		break;
	case 7:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_async_cmd(&client_desc); 
		break;
	case 8:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_no_resp_cmd(&client_desc); 
		break;		
	case 9:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_close_cmd(&client_desc);
		break;	
	case 10:
		fsl_os_print("EXPECTED RESULT: FAILED \n");
		fsl_os_print("client_close_cmd should fail\n");
		client_close_cmd(&client_desc);
		break;	
	case 11:
		fsl_os_print("EXPECTED RESULT: FAILED\n");
		client_no_resp_cmd(&client_desc); 
		break;	
	case 12:
		fsl_os_print("EXPECTED RESULT: FAILED\n");
		client_sync_cmd(&client_desc); 
		break;
	default:
		break;
	}
	received_fd++;
	cmdif_srv_isr();
}
