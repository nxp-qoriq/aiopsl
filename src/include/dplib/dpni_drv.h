/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          dpni_drv.h

 @Description   This file contains LDPAA Network Interfae API.
*//***************************************************************************/
#ifndef __FSL_DPNI_DRV_H
#define __FSL_DPNI_DRV_H

#include "common/types.h"
#include "net.h"


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

/**************************************************************************//**
@Description    Application Receive callback

                User provides this function. Driver invokes it when it gets a
                frame received on this interface.

 @Param[in]     arg     portal number.

 @Return        OK on success; error code, otherwise.
 *//***************************************************************************/
typedef int (rx_cb_t) (fsl_handle_t arg);


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
int dpni_drv_register_rx_cb (uint16_t     ni_id,
                                   rx_cb_t      *cb,
                                   fsl_handle_t arg);

/**************************************************************************//**
 @Function      dpni_drv_send

 @Description   TODO

 @Param[in]     ni_id   The Network Interface ID
 @Param[imp]    skb     A pointwer to the Frame structure

 @Return        OK on success; error code, otherwise.
*//***************************************************************************/    
void __noreturn__ dpni_drv_send (uint16_t ni_id);

/**************************************************************************//**
 @Function     dpni_get_num_of_ni 

 @Description   Returns the number of NI_ISs in the system.  Called by the AIOP
                applications to learn the maximum number of available network
	       	interfaces.  

 @Param[in]    none

 @Return       Number of NI_IDs in the system
*//***************************************************************************/    
int dpni_get_num_of_ni ( void );

/** @} */ /* end of dpni_drv_g group */
/** @} */ /* end of ldpaa_g group */


#endif /* __FSL_DPNI_DRV_H */
