/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          dpni_drv.h

 @Description   TODO
*//***************************************************************************/
#ifndef __FSL_DPNI_DRV_H
#define __FSL_DPNI_DRV_H

#include "common/types.h"


/**************************************************************************//**
 @Group         ldpaa_g  LDPAA API

 @Description   TODO

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         dpni_drv_g  Network Interface API

 @Description   TODO

 @{
*//***************************************************************************/

/* TODO - move to soc files */
#define SOC_MAX_NUM_OF_DPNI		128

#define DPNI_DRV_MAX_NUM_FLOWS		8


typedef uint64_t	dpni_drv_app_arg_t;

/**************************************************************************//**
@Description    Application Receive callback

                User provides this function. Driver invokes it when it gets a
                frame received on this interface.

 @Param[in]     arg     argument for application callback.
 *//***************************************************************************/
typedef void /*__noreturn*/ (rx_cb_t) (dpni_drv_app_arg_t arg);


/**************************************************************************//**
 @Function      dpni_drv_enable

 @Description   TODO

 @Param[in]     ni_id   The Network Interface ID

 @Return        OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_enable (uint16_t ni_id);

/**************************************************************************//**
 @Function      dpni_drv_disable

 @Description   TODO

 @Param[in]     ni_id   The Network Interface ID

 @Return        OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_disable (uint16_t ni_id);

/**************************************************************************//**
 @Function      dpni_drv_is_up

 @Description   TODO

 @Param[in]     ni_id   The Network Interface ID

 @Return        '1' if up; '0' otherwise.
*//***************************************************************************/
int dpni_drv_is_up (uint16_t ni_id);

/**************************************************************************//**
 @Function      dpni_drv_enable

 @Description   TODO

 @Param[in]     ni_id   The Network Interface ID
 @Param[in]     cb      TODO
 @Param[in]     arg     TODO

 @Return        OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_register_rx_cb (uint16_t     	ni_id,
                             uint16_t     	flow_id,
                             rx_cb_t      	*cb,
                             dpni_drv_app_arg_t arg);

/**************************************************************************//**
@Group	DPNI_DRV_STATUS  DP network interface Statuses

@Description \ref dpni_drv_send() possible return values

@{
*//***************************************************************************/
	/** MTU was crossed for DPNI driver send function */
#define	DPNI_DRV_MTU_ERR	(DPNI_DRV_MODULE_STATUS_ID | 0x1)
	/** NI is not enabled in DPNI driver send function */
#define	DPNI_DRV_NI_DIS		(DPNI_DRV_MODULE_STATUS_ID | 0x2)
/** @} */

/**************************************************************************//**
 @Function      dpni_drv_send

 @Description   @Description	Network Interface send (AIOP enqueue) function.

 @Param[in]     ni_id   The Network Interface ID
 @Param[imp]    Queueing Destination Priority (qd_priority) in the TLS.

 @Return        OK on success; error code, otherwise.
		For error codes refer to \ref FDMA_ENQUEUE_FRAME_ERRORS
		and \ref DPNI_DRV_STATUS.
*//***************************************************************************/
int dpni_drv_send (uint16_t ni_id);

/**************************************************************************//**
 @Function     dpni_get_num_of_ni

 @Description   Returns the number of NI_ISs in the system.  Called by the AIOP
                applications to learn the maximum number of available network
	       	interfaces.

 @Param[in]    none

 @Return       Number of NI_IDs in the system
*//***************************************************************************/
int dpni_get_num_of_ni (void);

/** @} */ /* end of dpni_drv_g group */
/** @} */ /* end of ldpaa_g group */


#endif /* __FSL_DPNI_DRV_H */
