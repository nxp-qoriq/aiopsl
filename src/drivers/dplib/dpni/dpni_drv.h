/**************************************************************************//**
@File		dpni_drv.h

@Description	This file contains the AIOP SW network interface
		definitions/functions for task initialization.

*//***************************************************************************/


#ifndef __DPNI_DRV_H_
#define __DPNI_DRV_H_


#include "general.h"


/**************************************************************************//**
 @Group		DPNI_DRV

 @Description	AIOP Network Interface

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		DPNI_DRV_DEFINES	 

 @Description	AIOP Network Interface Defines.

 @{
*//***************************************************************************/
typedef uint64_t	dpni_drv_app_arg_t;

/**************************************************************************//**
@Description    Application Receive callback

                User provides this function. Driver invokes it when it gets a
                frame received on this interface.

 @Param[in]     arg     portal number.

 @Return        None.
 *//***************************************************************************/
typedef void (rx_cb_t) (dpni_drv_app_arg_t arg);

#define DPNI_DRV_FLG_ENABLED    0x80
#define DPNI_DRV_FLG_PARSE      0x40

/** @} */ /* end of  DPNI_DRV_DEFINES */

/**************************************************************************//**
 @Group		AIOP_DPNI_DRV_FUNCTIONS		

 @Description	AIOP Network Interface functions.

 @{
*//***************************************************************************/

/*************************************************************************//**
@Function	osm_task_init

@Description	Initialization of the OSM parameters.

@Return		None.

@Cautions	None
*//***************************************************************************/
void osm_task_init(void);

/*************************************************************************//**
@Function	receive_cb

@Description	Network Interface receive callback function that is called
            from EPID table (HW). This function perform the following:
            1. Call parser_profile_create and parse_result_generate.
            2. Check frame error.
            
            Implicit: 
            ni_id: in the presentation context[param] field: Entry Point
            Opaque Parameter value.

@Return		None.

@Cautions	None
*//***************************************************************************/
void receive_cb(void);

/** @} */ /* end of AIOP_DPNI_DRV_FUNCTIONS */

/**************************************************************************//**
@Group		DPNI_DRV_STRUCTURES

@Description	AIOP DPNI DRV Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Network Interface structure.
*//***************************************************************************/
struct dpni_drv {
	/** network interface ID */
    uint8_t        id;
    /** \ref DPNI_DRV_DEFINES */
    uint8_t        flags;
    /** Storage profile ID */
    /* Need to store it in HW context */
    uint8_t        spid;
    /** MTU value needed for the \ref dpni_drv_send() function */
    uint16_t       mtu;
    /* used for MTU check failure */
    /** error FQID needed for the MTU check */
    uint32_t       err_fqid;
    /** error mask for the \ref receive_cb() function FD error check 
    * 0 - continue; 1 - discard          */
    uint8_t        fd_err_mask;
	/** Parse Profile ID */
	uint8_t prpid;
	/** parser starting offset from presented segment */
	uint8_t starting_offset;
	/** starting HXS */
	uint16_t starting_hxs;
	/** Queueing destination for the enqueue. */
	uint16_t qdid;
	/** call back application function */
	rx_cb_t         *rx_cbs[8];
    /** call back application argument */
	dpni_drv_app_arg_t    args[8];
};

/** @} */ /* end of DPNI_DRV_STRUCTURES */


/** @} */ /* end of DPNI_DRV */




#endif /* __DPNI_DRV_H_ */
