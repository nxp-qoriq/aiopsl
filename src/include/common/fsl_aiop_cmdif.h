/**************************************************************************//**
Copyright 2013 Freescale Semiconductor, Inc.

@File          fsl_aiop_cmdif.h

@Description   AIOP to GPP cmdif API
 *//***************************************************************************/

#ifndef __FSL_AIOP_CMDIF_H
#define __FSL_AIOP_CMDIF_H

/**************************************************************************//**
@Group         cmdif_g  Command Interface API

@Description   AIOP and GPP command interface API

@{
 *//***************************************************************************/

/**************************************************************************//**
@Group		CMDIF_SEND_ATTRIBUTES

@Description	The attributes to be used with cmdif_send() 

@{
*//***************************************************************************/
#define CMDIF_PRI_LOW		0	/**< Low Priority */
#define CMDIF_PRI_HIGH		1	/**< High Priority */

#define CMDIF_ASYNC_CMD		0x2000
/**< Bit to be used for cmd_id to identify asynchronous commands */
#define CMDIF_NORESP_CMD	0x1000
/**< Bit to be used for commands that don't need response */

/* @} end of group FDMA_PRESENT_FRAME_ERRORS */

/**************************************************************************//**
@Description   Command interface descriptor.
*//***************************************************************************/
struct cmdif_desc {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Or pointer to transport layer device for sending commands;
	 * Must be set by the user
	 */
	void *dev;
	/*!<
	 * Opaque handle for the use of the command interface;
	 * user should not modify it.
	 */
	void *lock;
	/*!<
	 * Optional lock object to be used with the lock/unlock callbacks;
	 * user must zero it if not needed.
	 */
	void (*lock_cb)(void *lock);
	/*!<
	 * Callback for locking the command interface (multiple users scenario);
	 * user must zero it if not needed.
	 */
	void (*unlock_cb)(void *lock);
	/*!<
	 * Callback for unlocking the command interface (multiple users scenario);
	 * user must zero it if not needed.
	 */
};

/**************************************************************************//**
@Group         cmdif_server_g  Command Interface - Server API

@Description   API to be used and implemented by Server side only

@{
 *//***************************************************************************/

/**************************************************************************//**
@Description	Open callback

User provides this function.
Driver invokes it when it gets establish instance command.

@Param[in]	instance_id - Instance id to be specified by client
		on cmdif_open().
@Param[in]	size        - Size of the data.
@Param[in]	data        - Data allocated by user.
@Param[out]	dev         - device handle.

@Return		Handle to instance object, or NULL for Failure.
 *//***************************************************************************/
typedef int (open_cb_t)(uint8_t instance_id, void **dev);

/**************************************************************************//**
@Description	De-init callback

User provides this function. Driver invokes it when it gets
terminate instance command.

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
@Param[in]	size - Size of the data
@Param[in]	data - Data of the command

@Return		OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (ctrl_cb_t)(void *dev, uint16_t cmd, uint32_t size, uint8_t *data);

/**************************************************************************//**
@Description	Function pointers to be supplied during module registration
 *//***************************************************************************/
struct cmdif_module_ops {
	open_cb_t  *open_cb;
	close_cb_t *close_cb;
	ctrl_cb_t  *ctrl_cb;
};

/**************************************************************************//**
@Function	cmdif_register_module

@Description	Registration of a module to the server.

Each module needs to register to the command interface by
supplying the following:

@Param[in]	module_name - Module name, it should be a valid string of
		up to 8 characters.
@Param[in]	ops -         A structure with 3 callbacks described above
																for open, close and control

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_register_module(const char *module_name, struct cmdif_module_ops *ops);

/**************************************************************************//**
@Function	cmdif_unregister_module

@Description	Cancel the registration of a module on the server
		and free the module id acquired during registration

Each module needs to unregister from the command interface

@Param[in]	module_name - Module name

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_unregister_module(const char *module_name);

/** @} *//* end of cmdif_server_g group */

/**************************************************************************//**
@Group		cmdif_client_g  Command Interface - Client API

@Description	API to be used and implemented by Client side only

@{
 *//***************************************************************************/

/**************************************************************************//**
@Description	Command callback

User provides this function. Driver invokes it for all asynchronous commands
that had been sent through cidesc.

@Param[in]	async_ctx - User context that was setup during cmdif_open()
@Param[in]	cmd_id    - Id of command
@Param[in]	size      - Size of the data
@Param[in]	data      - Data of the command

@Return		OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (cmdif_cb_t)(void *async_ctx,
			uint16_t cmd_id,
			uint32_t size,
			uint8_t *data);

/**************************************************************************//**
@Function	cmdif_open

@Description	Open command interface device for the specified module

@Param[in]	cidesc      - Command interface descriptor, cmdif device will
		be returned inside this descriptor.
@Param[in]	module_name - Module name
@Param[in]	instance_id - Instance id which will be passed to open callback
@Param[in]	async_cb    - Callback to be called on response of
		asynchronous command.
@Param[in]	async_ctx   - Context to be received with asynchronous command
		response inside async_cb().
@Param[in]	size     - Size of the data.
@Param[in]	data     - Data of the command or buffer allocated by user which
		will be by open_cb_t().
		This address should be accessible by Server and Client

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_open(struct cmdif_desc *cidesc,
		const char *module_name,
		uint8_t instance_id,
		cmdif_cb_t async_cb,
		void *async_ctx);

/**************************************************************************//**
@Function	cmdif_close

@Description	Close this command interface device and free this instance entry
		on the Server.

@Param[in]	cidesc - Command interface descriptor which was setup by
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
		
@Param[in]	cidesc   - Command interface descriptor which was setup by
		cmdif_open().
@Param[in]	cmd_id   - Id which represent command on the module that was
		registered on Server; Application may use bits 12-0.
		See \ref CMDIF_SEND_ATTRIBUTES.
@Param[in]	size     - Size of the data.
@Param[in]	priority - High or low priority queue to be checked.
		See \ref CMDIF_SEND_ATTRIBUTES.
@Param[in]	data     - Data of the command or buffer allocated by user which
		will be used inside command.
		This address should be accessible by Server and Client

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int priority,
		uint8_t *data);

/**************************************************************************//**
@Function	cmdif_resp_read

@Description	Check the response queue for new responses, de-queue and activate
		the callback function for each response

This function is not blocking; if nothing was found it will return error code

@Param[in]	cidesc - Command interface descriptor which was setup by
		cmdif_open().
@Param[in]	priority - High or low priority queue to be checked.

@Return		0 on success; error code, if no responses have been found.
 *//***************************************************************************/
int cmdif_resp_read(struct cmdif_desc *cidesc, int priority);


/** @} *//* end of cmdif_client_g group */
/** @} *//* end of cmdif_g group */

#endif /* __FSL_AIOP_CMDIF_H */
