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

/*!
 *  @file    fsl_cmdif_flib_c.h
 *  @brief   Cmdif AIOP<->GPP FLIB header file for client
 */

#ifndef __FSL_CMDIF_FLIB_C_H
#define __FSL_CMDIF_FLIB_C_H

#include <cmdif.h>
#include <fsl_cmdif_client.h>
#include <fsl_cmdif_flib_fd.h>


/*!
 * @Group	cmdif_flib_g  Command Interface - FLIB
 *
 * @brief	API to be used for FD based command interface implementation
 *
 * This is external API that is used to implement the final API as defined at
 * fsl_cmdif_client.h and fsl_cmdif_server.h. For client and server external use
 * only the API from fsl_cmdif_client.h and fsl_cmdif_server.h.
 *
 * @{
 */


/**
 *
 * @brief	Build open command.
 *
 * Should be used for building frame descriptor for open command.
 *
 * @param[in]	cidesc      - Command interface descriptor
 * @param[in]	m_name      - Module name, up to 8 characters
 * @param[in]	instance_id - Instance id which will be passed to open_cb_t
 * @param[in]	v_data	    - Core virtual address of the buffer to be used
 * 		by command interface.
 * 		The core is going to access the buffer through this address.
 * 		This address should be accessible by Server and Client.
 * 		Must be 8 byte aligned.
 * @param[in]	p_data	    - Physical address or SMMU virtual address of the
 * 		v_data buffer to be set inside the fd of command.
 * @param[in]	size        - Size of the v_data buffer. If the size if not
		enough cmdif_open() will return -ENOMEM.
 * @param[out]	fd          - Frame descriptor relevant fields for cmdif
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int cmdif_open_cmd(struct cmdif_desc *cidesc,
		const char *m_name,
		uint8_t instance_id,
		uint8_t *v_data,
		uint64_t p_data,
		uint32_t size,
		struct cmdif_fd *fd);
/**
 *
 * @brief	Synchronous/Blocking mode done indication.
 *
 * Should be used for implementation of cmdif_send() in synchronous mode.
 *
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	'0' if the command is not finished yet;
 * 		not '0' if it has finished.
 */
int cmdif_sync_ready(struct cmdif_desc *cidesc);

/**
 *
 * @brief	Synchronous command done.
 *
 * Should be used for implementation of cmdif_send() in synchronous mode.
 * Should the last call before return inside from cmdif_send().
 *
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	Error code as returned by AIOP server
 */
int cmdif_sync_cmd_done(struct cmdif_desc *cidesc);

/**
 *
 * @brief	Open command done.
 *
 * Should be used for implementation of cmdif_open().
 * Should the last call before return inside from cmdif_open().
 *
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	Error code as returned by AIOP server
 */
int cmdif_open_done(struct cmdif_desc *cidesc);


/**
 *
 * @brief	Build close command.
 *
 * Should be used for building frame descriptor for close command.
 *
 * @param[in]	cidesc      - Command interface descriptor
 * @param[out]	fd          - Frame descriptor relevant fields for cmdif
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int cmdif_close_cmd(struct cmdif_desc *cidesc, struct cmdif_fd *fd);

/**
 *
 * @brief	Close command done.
 *
 * Should be used for implementation of cmdif_close().
 * Should the last call before return inside from cmdif_close().
 *
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	Error code as returned by AIOP server
 */
int cmdif_close_done(struct cmdif_desc *cidesc);

/**
 *
 * @brief	Synchronous/Blocking mode indication.
 *
 * Should be used for implementation of cmdif_send() in synchronous mode.
 *
 * @param[in]	cmd_id - Command id that was sent
 *
 * @returns	'0' if command is asynchronous;
 * 		not '0' id command is synchronous.
 *
 */
int cmdif_is_sync_cmd(uint16_t cmd_id);

/**
 *
 * @brief	Build command.
 *
 * Should be used for building frame descriptor for command.
 *
 * @param[in]	cidesc - Command interface descriptor
 * @param[in]	cmd_id - Command id that was sent
 * @param[in]	size   - Size of data
 * @param[in]	data   - Physical address or SMMU virtual address of the
 * 		command buffer to be set inside the fd of the command.
 * @param[in]	async_cb    - Callback to be called on response of
 *		asynchronous command.
 * @param[in]	async_ctx   - Context to be received with asynchronous
 * 		command response inside async_cb()
 * @param[out]	fd     - Frame descriptor relevant fields for cmdif
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int cmdif_cmd(struct cmdif_desc *cidesc,
	uint16_t cmd_id,
	uint32_t size,
	uint64_t data,
	cmdif_cb_t *async_cb,
	void *async_ctx,
	struct cmdif_fd *fd);

/**
 *
 * @brief	Call asynchronous callback of the received frame descriptor
 *
 * @param[in]	fd - Pointer to received frame descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int cmdif_async_cb(struct cmdif_fd *fd);

/** @} *//* end of cmdif_flib_g group */

#endif /* __FSL_CMDIF_FLIB_H */
