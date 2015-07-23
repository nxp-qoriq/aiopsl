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
 *  @file    fsl_cmdif_flib_s.h
 *  @brief   Cmdif AIOP<->GPP FLIB header file for server
 */

#ifndef __FSL_CMDIF_FLIB_S_H
#define __FSL_CMDIF_FLIB_S_H

#include <fsl_types.h>
#include <fsl_cmdif_server.h>
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

#define CMD_ID_NOTIFY_OPEN    0xF000
/*!< Special command for cmdif_session_open() */
#define CMD_ID_NOTIFY_CLOSE   0xE000
/*!< Special command for cmdif_session_close() */

/**
 *
 * @brief	Allocate server handle to be used by server FLIBs.
 *
 * Should be used one time during server initialization.
 *
 * @param[in]	fast_malloc  - Malloc function for fast memory allocation that
 *		is accessed for every command.
 * @param[in]	slow_malloc  - Malloc function for slow memory allocation,
 *		this memory will be used to malloc data that is
 *		accessed only during initialization.
 *
 * @returns	Valid pointer on success; NULL otherwise.
 *
 */
void *cmdif_srv_allocate(void *(*fast_malloc)(int),
			void *(*slow_malloc)(int));

/**
 *
 * @brief	Deallocate server handle allocated by cmdif_srv_allocate().
 *
 * Should be used one time during server shutdown.
 *
 * @param[in]	srv	- Server handle allocated by cmdif_srv_allocate()
 * @param[in]	free	- Function to be used to free server allocated
 *		memory.
 * @returns	None.
 *
 */
void cmdif_srv_deallocate(void *srv, void (*free)(void *ptr));

/**
 *
 * @brief	Unregister module under server.
 *
 * Should be used to implement cmdif_unregister_module.
 * This function is not multitask protected.
 * Wrap it with locks if required.
 *
 * @param[in]	srv	- Server handle allocated by cmdif_srv_allocate()
 * @param[in]	m_name	- Module name to unregister
 *
 * @returns	0 on success, error code otherwise.
 *
 */
int cmdif_srv_unregister(void *srv, const char *m_name);

/**
 *
 * @brief	Register module under server.
 *
 * Should be used to implement cmdif_register_module.
 * This function is not multitask protected.
 * Wrap it with locks if required.
 *
 * @param[in]	srv	- Server handle allocated by cmdif_srv_allocate()
 * @param[in]	m_name	- Module name to unregister
 * @param[in]	ops	- Module callback functions
 *
 * @returns	0 on success, error code otherwise.
 *
 */
int cmdif_srv_register(void *srv, const char *m_name,
		struct cmdif_module_ops *ops);

/**
 *
 * @brief	Open session on server
 *
 * Should be used for implementation of cmdif_session_open()
 * or inside cmdif_srv_cb().
 * This API is to be used to create a session on server.
 * Session information will be placed inside v_data, this buffer can be send to
 * the other side using #CMD_ID_NOTIFY_OPEN command.
 *
 * @param[in]	srv      - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	m_name   - Name of the module that have been registered using
 * 		cmdif_srv_register()
 * @param[in]	inst_id  - Instance id which will be passed to #open_cb_t
 * @param[in]	size     - Size of v_data buffer.
 * @param[in]	dev_id   - Transport device id to be used for this session.
 * @param[out]	v_data   - Buffer allocated by user. If not NULL this buffer
 * 		will carry all the information of this session.
 * 		Must be 8 bytes aligned.
 * @param[out]	auth_id  - Session id as returned by server.
 *
 * @returns	O on success or error code otherwise.
 */
int cmdif_srv_open(void *srv,
		const char *m_name,
		uint8_t inst_id,
		uint32_t dev_id,
		uint32_t size,
		void *v_data,
		uint16_t *auth_id);

/**
 *
 * @brief	Close session on server
 *
 * Should be used for implementation of cmdif_session_close()
 * or inside cmdif_srv_cb().
 * This API is to be used to close a session on server.
 * Session information will be placed inside v_data, this buffer can be send to
 * the other side using #CMD_ID_NOTIFY_CLOSE command.
 *
 * @param[in]	srv      - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	auth_id  - Session id as returned by cmdif_srv_open().
 * @param[in]	dev_id   - Transport device id to be used for this session.
 * @param[in]	size     - Size of v_data buffer.
 * @param[out]	v_data   - Buffer allocated by user. If not NULL this buffer
 * 		will carry all the information of this session.
 *
 * @returns	O on success or error code otherwise.
 */
int cmdif_srv_close(void *srv,
		uint16_t auth_id,
		uint32_t dev_id,
		uint32_t size,
		void *v_data);

/**
 *
 * @brief	Server handle command function
 *
 * Should be called upon every command frame that have been dequeued.
 * Use it inside cmdif_srv_cb()
 *
 * @param[in]	srv       - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	cfd       - CMDIF input frame descriptor
 * @param[in]	v_addr    - Virtual address to be used for ctrl cb.
 * 		This is workaround for SMMU disable mode, set it to NULL if
 * 		cfd->u_addr.d_addr can be passed as #ctrl_cb_t data.
 * 		Otherwise set v_addr as virtual address of cfd->u_addr.d_addr.
 * @param[out]	cfd_out   - CMDIF output frame descriptor,
 *		if response is required
 * @param[out]	send_resp - Response indication. If set to 1 the response FD
 *		must be sent.
 *
 * @returns	O on success or error code otherwise.
 */
int cmdif_srv_cmd(void *srv,
		struct cmdif_fd *cfd,
		void   *v_addr,
		struct cmdif_fd *cfd_out,
		uint8_t *send_resp);

/** @} *//* end of cmdif_flib_g group */

#endif /* __FSL_CMDIF_FLIB_H */
