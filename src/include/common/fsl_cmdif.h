/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_cmdif.h

 @Description   TODO
 *//***************************************************************************/

#ifndef __FSL_CMDIF_H
#define __FSL_CMDIF_H

/**************************************************************************//**
 @Group         cmdif_g  Command Interface API

 @Description   TODO

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Description   Command interface descriptor.

 *//***************************************************************************/
struct cmdif_desc {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
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

enum cmdif_module {
	CMDIF_MOD_DPRC,
	CMDIF_MOD_DPNI,
	CMDIF_MOD_DPIO,
	CMDIF_MOD_DPSP,
	CMDIF_MOD_DPSW,
};

#define CMDIF_PRI_LOW		0	/**< Low Priority */
#define CMDIF_PRI_HIGH		1	/**< High Priority */

/**************************************************************************//**
 @Group         cmdif_server_g  Command Interface - Server API

 @Description   TODO

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Description   TODO
 *//***************************************************************************/
struct cmdif_module_ops {
	int (*open_cb)(void *dev);
	/*!< The server calls this function when  an 'open' command is received. */

	int (*close_cb)(void *dev);
	/*!< The server calls this function when  a 'close' command is received. */

	int (*ctrl_cb)(void *dev,
	               uint16_t cmd,
	               uint16_t size,
	               uint8_t *cmd_data);
	/*!< The server calls this function when any control command is received. */
};

/**************************************************************************//**
 @Function      cmdif_register_module

 @Description   registration of a module to the command portal.

 Each module needs to register to the command interface by
 supplying the following:

 @Param[in]     module_id  	module ID
 @Param[in]     ops     	A structure with 3 callbacks decscribed below for open,
 close and control

 @Return        0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_register_module(enum cmdif_module module,
                          struct cmdif_module_ops *ops);

/**************************************************************************//**
 @Function      cmdif_close_dev

 @Description   TODO

 @Param[in]     dev         A handle of the device.
 *//***************************************************************************/
int cmdif_close_dev(void *dev);

/** @} *//* end of cmdif_server_g group */

/**************************************************************************//**
 @Group         cmdif_client_g  Command Interface - Client API

 @Description   TODO

 @{
 *//***************************************************************************/

int cmdif_open(struct cmdif_desc *cidesc,
               enum cmdif_module module,
               int instance_id);

int cmdif_close(struct cmdif_desc *cidesc);

int cmdif_send(struct cmdif_desc *cidesc,
               uint16_t cmd_id,
               int size,
               int priority,
               uint8_t *cmd_data);

#if 1
// TODO - think if needed
int cmdif_get_cmd_data(struct cmdif_desc *cidesc, uint8_t **cmd_data);
#endif

/** @} *//* end of cmdif_client_g group */
/** @} *//* end of cmdif_g group */

#endif /* __FSL_CMDIF_H */
