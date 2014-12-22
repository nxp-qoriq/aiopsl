/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
@File          fsl_cmdif_client.h

@Description   AIOP to GPP cmdif API
 *//***************************************************************************/

#ifndef __FSL_CMDIF_CLIENT_H
#define __FSL_CMDIF_CLIENT_H

/**************************************************************************//**
@Group		cmdif_client_g  Command Interface - Client

@Description	API to be used and implemented by Client side only

@{
 *//***************************************************************************/

/**************************************************************************//**
@Group		CMDIF_SEND_ATTRIBUTES Send Attributes

@Description	The attributes to be used with cmdif_send()

@{
*//***************************************************************************/
#define CMDIF_PRI_LOW		0	/**< Low Priority */
#define CMDIF_PRI_HIGH		1	/**< High Priority */

#define CMDIF_ASYNC_CMD		0x2000
/**< Bit to be used for cmd_id to identify asynchronous commands */
#define CMDIF_NORESP_CMD	0x1000
/**< Bit to be used for commands that don't need response */

/** @} end of group CMDIF_SEND_ATTRIBUTES */


#define CMDIF_OPEN_SIZE		64  /**< cmdif_open() default size */

/**************************************************************************//**
@Description   Command interface descriptor.
*//***************************************************************************/
struct cmdif_desc {
	void *regs;
	/*!<
	 * Pointer to transport layer device for sending commands;
	 * On GPP the user should pass NADK device
	 * On AIOP the user should pass dpci_id as known by GPP SW context
	 * Must be set by the user
	 */
	void *dev;
	/*!<
	 * Opaque handle for the use of the command interface;
	 * user should not modify it.
	 */
};

/**************************************************************************//**
@Description	Command callback

User provides this function. Driver invokes it for all asynchronous commands
that had been sent through cidesc.

@Param[in]	async_ctx   User context that was setup during cmdif_open()
@Param[in]	err         Error as returned by server
@Param[in]	cmd_id      Id of command
@Param[in]	size        Size of the data.
		On the AIOP side use PRC_GET_SEGMENT_LENGTH() to determine the
		size of presented data.
@Param[in]	data        Data of the command.
		On the AIOP side it is the pointer to segment presentation
		address; use fdma_modify_default_segment_data() if needed.
		On GPP side it should be virtual address that belongs
		to current SW context.
@Return		OK on success; error code, otherwise.
@Cautions	Please make sure to modify only size bytes of the data.
		Automatic expansion of the buffer is not available.
 *//***************************************************************************/
typedef int (cmdif_cb_t)(void *async_ctx,
			int err,
			uint16_t cmd_id,
			uint32_t size,
			void *data);

/**************************************************************************//**
@Function	cmdif_open

@Description	Open command interface device for the specified module

@Param[in]	cidesc		Command interface descriptor, cmdif device will
		be returned inside this descriptor.
		Sharing of the same cidesc by multiple threads requires locks 
		outside CMDIF API, as an alternative each thread can open it's 
		own session by calling cmdif_open(). 
		Only cidesc.regs must be set by user see struct cmdif_desc.
@Param[in]	module_name	Module name, up to 8 characters.
@Param[in]	instance_id	Instance id which will be passed to #open_cb_t
@Param[in]	data		Buffer to be used by command interface.
		This address should be accessible by Server and Client.
		This buffer can be freed only after cmdif_close().
		On AIOP, set data as NULL. 
		On GPP it must be from Write-Back Cacheable and 
		Outer Shareable memory. 
@Param[in]	size		Size of the data buffer. If the size is not
		enough cmdif_open() will return -ENOMEM. On AIOP, set it to 0.
		By default, set it to #CMDIF_OPEN_SIZE bytes.

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_open(struct cmdif_desc *cidesc,
		const char *module_name,
		uint8_t instance_id,
		void *data,
		uint32_t size);

/**************************************************************************//**
@Function	cmdif_close

@Description	Close this command interface device and free this instance entry
		on the Server.

It's not yet supported by the AIOP client.

@Param[in]	cidesc   Command interface descriptor which was setup by
		cmdif_open().

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_close(struct cmdif_desc *cidesc);

/**************************************************************************//**
@Function	cmdif_send

@Description	Send command to the module device that was created during
		cmdif_open().

		This function may be activated in synchronous and asynchronous
		mode, see \ref CMDIF_SEND_ATTRIBUTES.
		Note, AIOP client supports only asynchronous commands see
		\ref CMDIF_ASYNC_CMD.

@Param[in]	cidesc     Command interface descriptor which was setup by
		cmdif_open().
@Param[in]	cmd_id     Id which represent command on the module that was
		registered on Server; Application may use bits 11-0.
		See \ref CMDIF_SEND_ATTRIBUTES.
@Param[in]	size       Size of the data including extra 16 bytes for 
		\ref cmdif_cb_t in case of \ref CMDIF_ASYNC_CMD.
@Param[in]	priority   High or low priority queue.
		See \ref CMDIF_SEND_ATTRIBUTES.
@Param[in]	data       Data of the command or buffer allocated by user which
		will be used inside command.
		This address should be accessible by Server and Client.
		It should be virtual address that belongs to current SW context.
		In case of asynchronous command last 16 bytes must be reserved 
		for cmdif usage.
		On GPP it must be from Write-Back Cacheable and 
		Outer Shareable memory. 		
@Param[in]	async_cb	Callback to be called on response of
		asynchronous command.
@Param[in]	async_ctx	Context to be received with asynchronous
		command response inside async_cb().

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int priority,
		uint64_t data,
		cmdif_cb_t *async_cb,
		void *async_ctx);

/**************************************************************************//**
@Function	cmdif_resp_read

@Description	Check the response queue for new responses,
		de-queue and activate the callback function for each response

This function is not blocking; if nothing was found it will return error code.
Note, this functionality is not relevant for AIOP client.

@Param[in]	cidesc   Command interface descriptor which was setup by
		cmdif_open().
@Param[in]	priority  High or low priority queue to be checked.

@Return		0 on success; error code, if no responses have been found.
 *//***************************************************************************/
int cmdif_resp_read(struct cmdif_desc *cidesc, int priority);


/** @} *//* end of cmdif_client_g group */

#endif /* __FSL_CMDIF_CLIENT_H */
