/**************************************************************************//**
Copyright 2013 Freescale Semiconductor, Inc.

@File          fsl_cmdif_server.h

@Description   AIOP to GPP cmdif API
 *//***************************************************************************/

#ifndef __FSL_CMDIF_SERVER_H
#define __FSL_CMDIF_SERVER_H

/**************************************************************************//**
@Group         cmdif_g  Command Interface API

@Description   AIOP and GPP command interface API

@{
 *//***************************************************************************/

/**************************************************************************//**
@Group         cmdif_server_g  Command Interface - Server API

@Description   API to be used and implemented by Server side only

@{
 *//***************************************************************************/
struct cmdif_desc;

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
@Param[in]	data - Data of the command - physical address.
		AIOP server will pass here address to the start of presentation
		segment - physical address is the same as virtual.
		On AIOP use fdma_modify_default_segment_data() if needed.

@Return		OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (ctrl_cb_t)(void *dev, uint16_t cmd, uint32_t size, uint64_t data);

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

Each module needs to unregister from the command interface

@Param[in]	module_name - Module name

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_unregister_module(const char *module_name);

/**************************************************************************//**
@Function	cmdif_session_open

@Description	Open session on server and notify client about it

@Param[in]	cidesc   - Already open connection descriptor towards second side
@Param[in]	m_name   - Name of the module as registered
		by cmdif_register_module()
@Param[in]	inst_id  - Instance id which will be passed to #open_cb_t
@Param[in]	size     - Size of v_data buffer
@Param[in]	v_data   - Buffer allocated by user. If not NULL this buffer
		will carry all the information of this session.
@Param[in]	p_data   - Physical address of v_data.
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
		uint64_t p_data,
		void *send_dev,
		uint16_t *auth_id);

/**************************************************************************//**
@Function	cmdif_session_close

@Description	Close session on server and notify client about it

@Param[in]	cidesc   - Already open connection descriptor towards second side
@Param[in]	size     - Size of v_data buffer
@Param[in]	auth_id  - Session id as returned by server.
@Param[in]	v_data   - Buffer allocated by user. If not NULL this buffer
		will carry all the information of this session.
@Param[in]	p_data   - Physical address of v_data.
@Param[in]	send_dev - Transport device used for server (nadk device).
		Device used for send and receive of frame descriptor.

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_session_close(struct cmdif_desc *cidesc,
			uint16_t auth_id,
			uint32_t size,
			void *v_data,
			uint64_t p_data,
			void *send_dev);

/**************************************************************************//**
@Function	cmdif_srv_cb

@Description	Server callback to be called on every frame command

@Param[in]	pr       - Priority
@Param[in]	send_dev - Device used for send and receive of frame descriptor

@Return		0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_srv_cb(int pr, void *send_dev);


/** @} *//* end of cmdif_server_g group */
/** @} *//* end of cmdif_g group */

#endif /* __FSL_CMDIF_SERVER_H */
