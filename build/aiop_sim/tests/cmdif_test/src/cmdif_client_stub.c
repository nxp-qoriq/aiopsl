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
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_io.h"
#include "fsl_fdma.h"
#include "fsl_dbg.h"
#include "fsl_cdma.h"
#include "cmdif_client.h"
#include "fsl_cmdif_flib_c.h"

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

static int cmdif_cb(void *async_ctx,
             int err,
             uint16_t cmd_id,
             uint32_t size,
             uint64_t data)
{

	fsl_os_print("PASSED Async CB: err = %d cmd = 0x%x size = 0x%x data = 0x%x async_ctx = 0x%x\n",
	             err, cmd_id, size, (uint32_t)data, async_ctx);
	return 0;
}

void client_open_cmd(struct cmdif_desc *client, void *sync_done)
{
	const char *module = "ABCABC";
	uint64_t   p_data  = fsl_os_virt_to_phys(sync_done);
	struct     cmdif_fd fd;
	int        err = 0;
	uint8_t    *v_ptr = NULL;

	err = cmdif_open_cmd(client,
	                     module,
	                     3,
	                     cmdif_cb,
	                     NULL,
	                     sync_done,
	                     p_data,
	                     (sizeof(struct cmdif_dev) +
	                	     sizeof(union cmdif_data)),
	                     &fd);
	if (err) {
		fsl_os_print("FAILED client_open_cmd\n");
		return;
	}
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, fd.u_flc.flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, fd.u_frc.frc);
	if ((fd.u_addr.d_addr != NULL) && (fd.d_size > 0)) {
		v_ptr = fsl_os_phys_to_virt(fd.u_addr.d_addr);
		cp_data_to_prc(v_ptr, (int)fd.d_size);
	}
	LDPAA_FD_SET_ADDR(HWC_FD_ADDRESS, fd.u_addr.d_addr);
	LDPAA_FD_SET_LENGTH(HWC_FD_ADDRESS, fd.d_size);
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
	err = cmdif_async_cb(&fd);
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
