/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <cmdif.h>
#include <bd_ring/fsl_cmdif_bd_flib.h>
#include <bd_ring/fsl_cmdif_client.h>
#include <bd_ring/fsl_cmdif_client_flib.h>

static void bd_ring_get(void *device, int pr, struct cmdif_bd_ring **bd_ring)
{
	bd_ring = NULL; /* TODO */
}

int cmdif_open(void *device, const char *m_name, void *handle)
{
	struct cmdif_bd bd;
	struct cmdif_bd *bd_in_ring;
	int err;
	struct cmdif_bd_ring *bd_ring;
	uint16_t session_id;
	
	err = cmdif_flib_open_bd(handle, m_name, &bd);

	/* Get bd_ring from device */
	bd_ring_get(device, CMDIF_PRI_LOW, &bd_ring);

	err = cmdif_flib_send(bd_ring, &bd, &bd_in_ring);
	if (err)
		return err;

	err = cmdif_flib_open_done(bd_in_ring, &session_id);
	while(err) {
		err = cmdif_flib_open_done(bd_in_ring, &session_id);
	}

	/* Update handle with session_id and etc. */
	cmdif_flib_handle_init(handle, session_id, device);
}

#if 0
int cmdif_sync_send(void *handle, uint16_t cmd_id, uint32_t size,
                    uint64_t data, int priority, uint32_t flags,
                    uint32_t response_size, uint64_t response_data)
{
	
}
#endif
