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

/**************************************************************************//**
@File          fsl_cmdif_server.h

@Description   AIOP to GPP cmdif API
 *//***************************************************************************/

#ifndef __FSL_CMDIF_SERVER_H
#define __FSL_CMDIF_SERVER_H

/**************************************************************************//**
@Group         cmdif_server_g  Command Interface - Server

@Description   API to be used and implemented by Server side only

@{
 *//***************************************************************************/

#define CMDIF_SESSION_OPEN_SIZE		64
/**< cmdif_session_open() default size */

struct cmdif_desc;

/**************************************************************************//**
@Description	Open callback

User provides this function.
Server invokes it when it gets open instance command.

@Param[in]	instance_id - Instance id to be specified by client
		on cmdif_open().
@Param[out]	dev         - device handle.

@Return		Handle to instance object, or NULL for Failure.
 *//***************************************************************************/
typedef int (open_cb_t)(uint8_t instance_id, void **dev);

/**************************************************************************//**
@Description	De-init callback

User provides this function.
Driver invokes it when it gets close instance command.

@Param[in]	dev - A handle of the device.

@Return		OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (close_cb_t)(void *dev);

/**************************************************************************//**
@Description	Control callback

User provides this function. Driver invokes it for all runtime commands

@Param[in]	dev -  A handle of the device which was returned after
		module open callback
@Param[in]	cmd -  Id of command
@Param[in]	size - Size of the data.
		On the AIOP side use PRC_GET_SEGMENT_LENGTH() to determine the
		size of presented data.
@Param[in]	data - Data of the command.
		AIOP server will pass here address to the start of presentation
		segment - physical address is the same as virtual.
		On AIOP use fdma_modify_default_segment_data() if needed.
		On GPP, it should be virtual address that belongs
		to current SW context.
@Return		OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (ctrl_cb_t)(void *dev, uint16_t cmd, uint32_t size, void *data);

/**************************************************************************//**
@Description	Function pointers to be supplied during module registration
 *//***************************************************************************/
struct cmdif_module_ops {
	open_cb_t  *open_cb;
	/**< Open callback to be activated after client calls cmdif_open() */
	close_cb_t *close_cb;
	/**< Close callback to be activated after client calls cmdif_close() */
	ctrl_cb_t  *ctrl_cb;
	/**< Control callback to be activated on each command */
};

/**************************************************************************//**
@Function	cmdif_register_module

@Description	Registration of a module to the server.

For AIOP, use this API during AIOP boot.

Each module needs to register to the command interface by
supplying the following:

@Param[in]	module_name - Module name, it should be a valid string of
		up to 8 characters.
@Param[in]	ops -         A structure with 3 callbacks described above
		for open, close and control
@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_register_module(const char *module_name,
			struct cmdif_module_ops *ops);

/**************************************************************************//**
@Function	cmdif_unregister_module

@Description	Cancel the registration of a module on the server
		and free the module id acquired during registration

For AIOP, use this API during AIOP boot.

@Param[in]	module_name - Module name, up to 8 characters.

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_unregister_module(const char *module_name);

/**************************************************************************//**
@Function	cmdif_session_open

@Description	Open session on server and notify client about it.

This functionality is relevant only for GPP.

@Param[in]	cidesc   - Already open connection descriptor towards the
		second side
@Param[in]	m_name   - Name of the module as registered
		by cmdif_register_module()
@Param[in]	inst_id  - Instance id which will be passed to #open_cb_t
@Param[in]	size     - Size of v_data buffer.
		By default, set it to #CMDIF_SESSION_OPEN_SIZE bytes.
@Param[in]	v_data   - Buffer allocated by user. If not NULL this buffer
		will carry all the information of this session.
		If not NULL, the buffer can be freed after cmdif_session_close().
@Param[in]	send_dev - Transport device to be used for server (nadk device).
		Device used for send and receive of frame descriptor.
@Param[out]	auth_id  - Session id as returned by server.

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_session_open(struct cmdif_desc *cidesc,
		const char *m_name,
		uint8_t inst_id,
		uint32_t size,
		void *v_data,
		void *send_dev,
		uint16_t *auth_id);

/**************************************************************************//**
@Function	cmdif_session_close

@Description	Close session on server and notify client about it.

This functionality is relevant only for GPP but it's not yet supported
by the GPP server.

@Param[in]	cidesc   - Already open connection descriptor towards second side
@Param[in]	size     - Size of v_data buffer
@Param[in]	auth_id  - Session id as returned by server.
@Param[in]	v_data   - Buffer allocated by user. If not NULL this buffer
		will carry all the information of this session.
@Param[in]	send_dev - Transport device used for server (nadk device).
		Device used for send and receive of frame descriptor.

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_session_close(struct cmdif_desc *cidesc,
			uint16_t auth_id,
			uint32_t size,
			void *v_data,
			void *send_dev);

/**************************************************************************//**
@Function	cmdif_srv_cb

@Description	Server callback to be called on every frame command

This functionality is relevant only for GPP.

@Param[in]	pr       - Priority
@Param[in]	send_dev - Device used for send and receive of frame descriptor

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_srv_cb(int pr, void *send_dev);


/** @} *//* end of cmdif_server_g group */

#endif /* __FSL_CMDIF_SERVER_H */
