/**************************************************************************//**
@File		fsl_dpni_drv.h

@Description	This file contains the AIOP SW network interface API.

*//***************************************************************************/


#ifndef __FSL_DPNI_DRV_H_
#define __FSL_DPNI_DRV_H_


#include "general.h"


/**************************************************************************//**
 @Group		FSL_DPNI_DRV

 @Description	AIOP FSL Network Interface API

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Group		FSL_DPNI_DRV_FUNCTIONS

 @Description	AIOP FSL Network Interface API Functions.

 @{
*//***************************************************************************/

/*************************************************************************//**
@Function	dpni_drv_send

@Description	Network Interface send (AIOP enqueue) function.

@Param[in]	Network Interface ID for send function.

@Param[in]	Queueing Destination Priority.
         
@Return		Status, please refer to \ref FDMA_ENQUEUE_FRAME_ERRORS.

@Cautions	None
*//***************************************************************************/
int32_t dpni_drv_send(uint16_t ni_id, uint8_t qd_priority);

/** @} */ /* end of FSL_DPNI_DRV_FUNCTIONS */


/** @} */ /* end of FSL_DPNI_DRV */




#endif /* __FSL_DPNI_DRV_H_ */
