/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_cmdif.h

 @Description   TODO
 *//***************************************************************************/

#ifndef __FSL_CMDIF_H
#define __FSL_CMDIF_H

#include "common/types.h"

/**************************************************************************//**
 @Group         cmdif_g  Command Interface API

 @Description   TODO

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Description   Module types.
*//***************************************************************************/
struct cmdif_desc {
	void *regs;
	void *dev;
	void *lock;
	void (*lock_cb)(void *lock);
	void (*unlock_cb)(void *lock);
};

struct cmdif_cmd_data;

enum cmdif_module {
	CMDIF_MOD_DPRC,
	CMDIF_MOD_DPNI,
	CMDIF_MOD_DPIO,
	CMDIF_MOD_DPSP,
	CMDIF_MOD_DPSW,
};

#define CMDIF_PRI_LOW		0	/**< Low Priority */
#define CMDIF_PRI_HIGH		1	/**< High Priority */

#if 1
/**************************************************************************//**
 @Group         cmdif_server_g  Command Interface - Server API

 @Description   TODO

 @{
 *//***************************************************************************/

enum cmdif_status {
	CMDIF_STATUS_OK = 0x0, /**< passed */
	CMDIF_STATUS_READY = 0x1, /**< Ready to be processed */
	CMDIF_STATUS_AUTH_ERR = 0x3, /**< Authentication error */
	CMDIF_STATUS_NO_PRIVILEGE = 0x4,
	CMDIF_STATUS_DMA_ERR = 0x5,
	CMDIF_STATUS_CONFIG_ERR = 0x6,
	CMDIF_STATUS_TIMEOUT = 0x7,
	CMDIF_STATUS_NO_RESOURCE = 0x8,
	CMDIF_STATUS_NO_MEMORY = 0x9,
	CMDIF_STATUS_BUSY = 0xA,
	CMDIF_STATUS_UNSUPPORTED_OP = 0xB,
	CMDIF_STATUS_INVALID_STATE = 0xC
};

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
int cmdif_register_module(enum cmdif_module module, struct cmdif_module_ops *ops);

/**************************************************************************//**
 @Function      cmdif_cmd_done

 @Description   updating the status for the GPP in the command portal

 Each module, upon finishing the ctrl_cb, needs to update the return
 status in the command portal using this routine.

 @Param[in]     cmd     cmd pointer that was provided to the ctrl_cb .
 @Param[in]     status  completion status
 *//***************************************************************************/
void cmdif_cmd_done(struct cmdif_cmd_data *cmd, enum cmdif_status status);

/**************************************************************************//**
 @Function      cmdif_close_dev

 @Description   TODO

 @Param[in]     dev         A handle of the device.
 *//***************************************************************************/
int cmdif_close_dev(void *dev);

/** @} *//* end of cmdif_server_g group */
#endif

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
// think if needed
int cmdif_get_cmd_data(struct cmdif_desc *cidesc,
                       uint8_t **cmd_data);
#endif

/** @} *//* end of cmdif_client_g group */
/** @} *//* end of cmdif_g group */

#endif /* __FSL_CMDIF_H */
