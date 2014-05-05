#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "io.h"
#include "fsl_fdma.h"
#include "sys.h"
#include "dbg.h"
#include "fsl_cdma.h"
#include "errors.h"
#include "cmdif_client.h"
#include "fsl_cmdif_flib.h"

/* The purpose of this code is to modify the defaults FD in WS 
 * in order to test Server.
 * This means that cmd data and size are already set. In real world NADK API
 * will set it for us. 
 * This code also define the communication between client and Server through FD */


/** These functions define how GPP should setup the FD 
 * It does not include data_addr and data_lenght because it's setup by NADK API
 * This code is used to modify the defaults FD inside WS */
void client_open_cmd(struct cmdif_desc *client, void *sync_done); 
void client_close_cmd(struct cmdif_desc *client); 
void client_sync_cmd(struct cmdif_desc *client); 
void client_async_cmd(struct cmdif_desc *client); 
void client_no_resp_cmd(struct cmdif_desc *client); 

static void cp_data_to_prc(uint8_t *data, int size)
{
	uint8_t * addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();

	memcpy(addr, data, (size_t)size);
}

void client_open_cmd(struct cmdif_desc *client, void *sync_done) 
{
	const char *module = "ABCABC";	
	uint64_t   p_data  = fsl_os_virt_to_phys(sync_done);
	struct     cmdif_fd fd;
	int        err = 0;
	uint8_t    *v_ptr = NULL;
	
	err = cmdif_open_cmd(client, module, 3, NULL, NULL, 
	                     sync_done, p_data, 26, &fd);
	
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, fd.u_flc.flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, fd.u_frc.frc);
	if ((fd.d_addr != NULL) && (fd.d_size > 0)) {
		v_ptr = fsl_os_phys_to_virt(fd.d_addr);
		cp_data_to_prc(v_ptr, (int)fd.d_size);
	}
	LDPAA_FD_SET_ADDR(HWC_FD_ADDRESS, fd.d_addr);
}

void client_close_cmd(struct cmdif_desc *client) 
{
	struct cmdif_fd fd;
	int    err = 0;

	err = cmdif_close_cmd(client, &fd);
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, fd.u_flc.flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, fd.u_frc.frc);
	
	cmdif_close_done(client);
}

void client_sync_cmd(struct cmdif_desc *client) 
{
	uint16_t cmd_id = 0xCC; /* Any number */
	struct   cmdif_fd fd;
	int      err = 0;

	err = cmdif_cmd(client, cmd_id, 0, NULL, &fd);
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, fd.u_flc.flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, fd.u_frc.frc);
}

void client_async_cmd(struct cmdif_desc *client) 
{
	uint16_t cmd_id = CMDIF_ASYNC_CMD | 0xA;
	struct   cmdif_fd fd;
	int      err = 0;

	err = cmdif_cmd(client, cmd_id, 0, NULL, &fd);
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, fd.u_flc.flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, fd.u_frc.frc);
}

void client_no_resp_cmd(struct cmdif_desc *client) 
{
	uint16_t cmd_id = CMDIF_NORESP_CMD | 0x2;
	struct   cmdif_fd fd;
	int      err = 0;

	err = cmdif_cmd(client, cmd_id, 0, NULL, &fd);
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, fd.u_flc.flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, fd.u_frc.frc);
}
