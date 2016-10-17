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

#include <fsl_mc_sys.h>
#include <fsl_mc_cmd.h>
#include <kernel/fsl_spinlock.h>
#include "fsl_cdma.h"

static int mc_status_to_error(enum mc_cmd_status status)
{
	switch (status) {
	case MC_CMD_STATUS_OK:
		return 0;
	case MC_CMD_STATUS_AUTH_ERR:
		return -EACCES; /* Token error */
	case MC_CMD_STATUS_NO_PRIVILEGE:
		return -EPERM; /* Permission denied */
	case MC_CMD_STATUS_DMA_ERR:
		return -EIO; /* Input/Output error */
	case MC_CMD_STATUS_CONFIG_ERR:
		return -EINVAL; /* Device not configured */
	case MC_CMD_STATUS_TIMEOUT:
		return -ETIMEDOUT; /* Operation timed out */
	case MC_CMD_STATUS_NO_RESOURCE:
		return -ENAVAIL; /* Resource temporarily unavailable */
	case MC_CMD_STATUS_NO_MEMORY:
		return -ENOMEM; /* Cannot allocate memory */
	case MC_CMD_STATUS_BUSY:
		return -EBUSY; /* Device busy */
	case MC_CMD_STATUS_UNSUPPORTED_OP:
		return -ENOTSUP; /* Operation not supported by device */
	case MC_CMD_STATUS_INVALID_STATE:
		return -ENODEV; /* Invalid device state */
	default:
		break;
	}

	/* Not expected to reach here */
	return -EINVAL;
}

int mc_send_command(struct fsl_mc_io *mc_io, struct mc_command *cmd)
{
	//struct mc_io *mc_portal = (struct mc_portal *)mc_io;
	enum mc_cmd_status status;

	if (!mc_io)
		return -EACCES;

	/* --- Call lock function here in case portal is shared --- */
	cdma_mutex_lock_take((uint64_t)mc_io->regs, CDMA_MUTEX_WRITE_LOCK);

	mc_write_command(mc_io->regs, cmd);

	/* Spin until status changes */
	do {
		status = MC_CMD_HDR_READ_STATUS(ioread64(mc_io->regs));

		/* --- Call wait function here to prevent blocking ---
		 * Change the loop condition accordingly to exit on timeout.
		 */
	} while (status == MC_CMD_STATUS_READY);

	/* Read the response back into the command buffer */
	mc_read_response(mc_io->regs, cmd);
#if 0
	/* The authentication id is read in create() or open() commands,
	 * to setup the control session.
	 */
	if (0 == mc_portal->auth)
		mc_portal->auth = (int)mc_cmd_read_auth_id(mc_portal->regs);
#endif

	/* --- Call unlock function here in case portal is shared --- */
	cdma_mutex_lock_release((uint64_t)mc_io->regs);

	return mc_status_to_error(status);
}

