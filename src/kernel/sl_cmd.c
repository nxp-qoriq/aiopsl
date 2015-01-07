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
#include "fsl_sl_cmd.h"
#include "fsl_errors.h"

int sl_cmd_ctrl_cb(void *dev, uint16_t cmd, uint32_t size, void *data);
int sl_cmd_open_cb(uint8_t instance_id, void **dev);
int sl_cmd_close_cb_t(void *dev);

int sl_cmd_ctrl_cb(void *dev, uint16_t cmd, uint32_t size, void *data)
{
	UNUSED(dev);
	
	switch (cmd) {
	
	case SL_CMD_NEW_BUFF:
		ASSERT_COND(size > 0);
		ASSERT_COND(data);
		break;
	default:
		break;
	}
	
	return 0;
}


int sl_cmd_open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev);
	UNUSED(instance_id);

	return 0;
}


int sl_cmd_close_cb_t(void *dev)
{
	UNUSED(dev);

	return 0;
}
