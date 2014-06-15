/**************************************************************************//**
Copyright 2013 Freescale Semiconductor, Inc.

@File          fsl_cmdif_server.h

@Description   AIOP to GPP cmdif API
 *//***************************************************************************/

#ifndef __FSL_CMDIF_SERVER_H
#define __FSL_CMDIF_SERVER_H

/**************************************************************************//**
 @Group		LIB LIB

 @Description	ARENA LIB APIs

 @{
*//***************************************************************************/

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
struct cmdif_fd;

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

int cmdif_session_open(struct cmdif_desc *cidesc,
                       const char *m_name,
                       uint8_t inst_id,
                       void *v_data,
                       uint64_t p_data,
                       uint32_t size,
                       uint16_t *auth_id);

int cmdif_session_close(struct cmdif_desc *cidesc,
                        uint16_t auth_id,
                        void *v_data,
                        uint64_t p_data,
                        uint32_t size);

int cmdif_srv_cb(struct cmdif_fd *cfd, int pr, void *send_dev);

/** @} *//* end of cmdif_server_g group */
/** @} *//* end of cmdif_g group */
/** @} *//* end of ARENA LIB APIs */

#endif /* __FSL_CMDIF_SERVER_H */
