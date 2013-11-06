/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_cmdif.h

 @Description   TODO
 *//***************************************************************************/

#ifndef __FSL_CMDIF_H
#define __FSL_CMDIF_H

#include "common/types.h"
#include "arch/fsl_soc.h"


/**************************************************************************//**
 @Group         cmdif_g  Command Interface API

 @Description   TODO

 @{
 *//***************************************************************************/

#define CMDIF_PRI_LOW		0	/**< Low Priority */
#define CMDIF_PRI_HIGH		1	/**< High Priority */


/**************************************************************************//**
 @Description   command structure declaration
 *//***************************************************************************/
struct cmdif_cmd_desc;

/**************************************************************************//**
 @Description   command interface device structure declaration
 *//***************************************************************************/
struct cmdif_dev;

/**************************************************************************//**
 @Description   command status

 command completion status
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
	CMDIF_STATUS_BUSY = 0xa,
	CMDIF_STATUS_INVALID_OP = 0xb,
	CMDIF_STATUS_UNSUPPORTED_OP = 0xc,
	CMDIF_STATUS_INVALID_STATE = 0xd
};

/**************************************************************************//**
 @Group         cmdif_server_g  Command Interface - Server API

 @Description   TODO

 @{
 *//***************************************************************************/
/**************************************************************************//**
@Description    Open callback.

                User provides this function. Driver invokes it when it gets establish instance command.

 @Param[in]     handle      - device handle.
 @Param[in]     cmdif_cmd   - pointer to the init command, should include all that nneded for initialization od a module:
                              icid, ports,etc..
 @Return        Handle to instance object, or NULL for Failure.
 *//***************************************************************************/
typedef int (open_cb_t) (fsl_handle_t    dev);

/**************************************************************************//**
 @Description   De-init callback.

                User provides this function. Driver invokes it when it gets
                terminate instance command.

 @Param[in]     dev         A handle of the device.

 @Return        OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (close_cb_t) (fsl_handle_t dev);

/**************************************************************************//**
 @Description   Control callback.

                User provides this function. Driver invokes it for all runtime commands

 @Param[in]     dev         A handle of the device.
 @Param[in]     cmd         TODO
 @Param[in]     size        TODO
 @Param[in]     desc        A pointer of the command

 @Return        OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (ctrl_cb_t) (fsl_handle_t           dev,
					     uint16_t               cmd,
					     uint16_t               size,
					     struct cmdif_cmd_desc  *desc);

/**************************************************************************//**
 @Description   TODO
*//***************************************************************************/
struct cmdif_module_ops {
     open_cb_t  *open_cb;
     close_cb_t *close_cb;
     ctrl_cb_t  *ctrl_cb;
}; 


/**************************************************************************//**
 @Function      cmdif_register_module

 @Description   registration of a module to the command portal.

 Each module needs to register to the command interface by
 supplying the following:

 @Param[in]     module  module ID
 @Param[in]     ops     A structure with 3 callbacks decscribed below for open,
 close and control

 @Return        0 on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_register_module(enum fsl_os_module module,
                          struct cmdif_module_ops *ops);

/**************************************************************************//**
 @Function      cmdif_cmd_done

 @Description   updating the status for the GPP in the command portal

 Each module, upon finishing the ctrl_cb, needs to update the return
 status in the command portal using this routine.

 @Param[in]     cmd     cmd pointer that was provided to the ctrl_cb .
 @Param[in]     status  completion status
 *//***************************************************************************/
void cmdif_cmd_done(struct cmdif_cmd_desc *cmd, enum cmdif_status status);

/** @} *//* end of cmdif_server_g group */

/**************************************************************************//**
 @Group         cmdif_client_g  Command Interface - Client API

 @Description   TODO

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Function      cmdif_open

 @Description   TODO

 @Param[in]     regs    TODO
 @Param[in]     mod     TODO
 @Param[in]     mod_id  TODO
 @Param[in]     icid    TODO

 @Return        A handle to the device on success (positive value);
 NULL otherwise.
 *//***************************************************************************/
struct cmdif_dev *cmdif_open(void *regs,
                             enum fsl_os_module mod,
                             uint16_t mod_id,
                             uint16_t icid);

/**************************************************************************//**
 @Function      cmdif_close

 @Description   TODO

 @Param[in]     dev     A handle as it was returened from cmdif_open().

 @Return        OK on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_close(struct cmdif_dev *dev);

/**************************************************************************//**
 @Function      cmdif_send

 @Description   TODO

 @Param[in]     dev     A handle as it was returened from cmdif_open().
 @Param[in]     cmd     TODO
 @Param[in]     desc    TODO

 @Return        OK on success; error code, otherwise.
 *//***************************************************************************/
int cmdif_send(struct cmdif_dev *dev,
               uint16_t cmd,
               int size,
               int priority,
               struct cmdif_cmd_desc *desc);

struct cmdif_cmd_desc *cmdif_get_desc(struct cmdif_dev *dev);

/** @} *//* end of cmdif_client_g group */
/** @} *//* end of cmdif_g group */


#endif /* __FSL_CMDIF_H */
