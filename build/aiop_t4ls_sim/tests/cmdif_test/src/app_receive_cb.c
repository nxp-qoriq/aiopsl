#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "common/fsl_aiop_cmdif.h"
#include "common/fsl_cmdif_flib.h"
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
/**< DDR, server accesses it with CDMA, 4 byte is enough */
__SHRAM struct cmdif_desc client_desc;
/**< Client descriptor that should be passed to open */
struct {
	struct cmdif_dev  client_dev;
	union  cmdif_data data;
} sync_done;
/**< Format of the data to be passed for cmdif_open(). 
 *   Part of the same buffer will be reused for sync_done */

#define     INVALID_AUTH_ID  1001

static int sync_done_read(union  cmdif_data *resp, struct cmdif_desc *client)
{
	struct cmdif_dev  *dev = (struct cmdif_dev *)client->dev;
	uint64_t sync_addr = fsl_os_virt_to_phys(dev->sync_done);
	return cdma_read(resp, sync_addr, sizeof(union cmdif_data));
}

#define SYNC_DONE_TEST \
{                                                                        \
	err = sync_done_read(&resp, &client_desc);                       \
	if (err || !resp.resp.done || resp.resp.err)                     \
		fsl_os_print("Test %d: FAILED!!!!!!!\n", (test_id - 1)); \
}

void app_receive_cb (void);
void app_receive_cb (void) 
{    
	int test_id = received_fd % 13;
	struct cmdif_dev  *dev = (struct cmdif_dev *)client_desc.dev;
	uint16_t auth_id = 0;
	union  cmdif_data resp;
	int err = 0;
	
	fsl_os_print("\n Starting test number %d\n", test_id);
	switch (test_id) {
	case 0:
		cmdif_sync_cmd_done(&client_desc);

		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_open_cmd(&client_desc, &sync_done); 
		break;
	case 1:
		cmdif_open_done(&client_desc);
		/* CDMA is required only because this client is tested on AIOP 
		 * AIOP writes data using CDMA therefore need to read with cdma */			
		sync_done_read(&resp, &client_desc);
		dev->auth_id = resp.resp.auth_id;

		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_sync_cmd(&client_desc); 
		break;
	case 2:
		SYNC_DONE_TEST;
		cmdif_sync_cmd_done(&client_desc);

		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_async_cmd(&client_desc); 
		break;
	case 3:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_no_resp_cmd(&client_desc); 
		break;
	case 4:
		fsl_os_print("EXPECTED RESULT: FAILED \n");
		auth_id = dev->auth_id;
		dev->auth_id = INVALID_AUTH_ID;
		client_sync_cmd(&client_desc); 	
		/* Get back the right auth_id */
		dev->auth_id = auth_id;
		break;
	case 5:
		SYNC_DONE_TEST;
		cmdif_sync_cmd_done(&client_desc);

		fsl_os_print("EXPECTED RESULT: FAILED \n");
		auth_id = dev->auth_id;
		dev->auth_id = INVALID_AUTH_ID;
		client_async_cmd(&client_desc);
		/* Get back the right auth_id */
		dev->auth_id = auth_id;
		break;
	case 6:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_sync_cmd(&client_desc); 
		break;
	case 7:
		SYNC_DONE_TEST;
		cmdif_sync_cmd_done(&client_desc);

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
		SYNC_DONE_TEST;
		cmdif_close_done(&client_desc);

		fsl_os_print("EXPECTED RESULT: FAILED \n");
		client_close_cmd(&client_desc);
		break;	
	case 11:
		SYNC_DONE_TEST;
		cmdif_close_done(&client_desc);
		
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
