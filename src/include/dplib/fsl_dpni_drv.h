/**************************************************************************//**
@File		fsl_dpni_drv.h

@Description	Data Path Network Interface API
*//***************************************************************************/
#ifndef __FSL_DPNI_DRV_H
#define __FSL_DPNI_DRV_H

#include "common/types.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_ldpaa.h"
#include "dpni_drv.h"


/**************************************************************************//**
@Group		dpni_g DPNI

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	dpni_drv_register_rx_cb

@Description	Attaches a pointer to a call back function to a NI ID.

	The callback function will be called when the NI_ID receives a frame.

@Param[in]	ni_id   The Network Interface ID
@Param[in]	flow_id Flow ID, it should be between 0 and
		#DPNI_DRV_MAX_NUM_FLOWS
@Param[in]	cb    Callback function for Network Interface specified flow_id
@Param[in]	arg   Argument that will be passed to callback function

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_register_rx_cb(uint16_t        ni_id,
			uint16_t        flow_id,
			rx_cb_t		    *cb,
			dpni_drv_app_arg_t arg);

/**************************************************************************//**
@Function	dpni_drv_unregister_rx_cb

@Description	Unregisters a NI callback function by replacing it with a
		pointer to a discard callback.
		The discard callback function will be called when the NI_ID
		receives a frame

@Param[in]	ni_id   The Network Interface ID
@Param[in]	flow_id Flow ID, it should be between 0 and
		#DPNI_DRV_MAX_NUM_FLOWS

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_unregister_rx_cb(uint16_t		ni_id,
                              uint16_t		flow_id);

/**************************************************************************//**
@Function	dpni_get_receive_niid

@Description	Get ID of NI on which the default packet arrived.

@Return	NI_IDs on which the default packet arrived.
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_get_receive_niid(void);

/**************************************************************************//**
@Function	dpni_set_send_niid

@Description	Set the NI ID on which the packet should be sent.

@Return	0 on success; error code, otherwise.
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_set_send_niid(uint16_t niid);

/**************************************************************************//**
@Function	dpni_get_send_niid

@Description	Get ID of NI on which the default packet should be sent.

@Return	0 on success; error code, otherwise.
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_get_send_niid(void);


/**************************************************************************//**
@Function	dpni_drv_get_primary_mac_address

@Description	Get Primary MAC address of NI.

@Return	0 on success; error code, otherwise.
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_drv_get_primary_mac_addr(uint16_t niid,
		uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);


/**************************************************************************//**
@Function	dpni_drv_add_mac_address

@Description	Adds unicast/multicast filter MAC address.

@Param[in]	ni_id	The Network Interface ID

@Param[in]	mac_addr	MAC address to be added to NI unicast/multicast
				filter.
@Return	0 on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_add_mac_addr(uint16_t ni_id,
          		const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_remove_mac_address

@Description	Removes unicast/multicast filter MAC address.

@Param[in]	ni_id	The Network Interface ID

@Param[in]	mac_addr	MAC address to be removed from NI
				unicast/multicast filter.

@Return	0 on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_remove_mac_addr(uint16_t ni_id,
          		const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_set_mfl

@Description	Set the maximum received frame length.

@Param[in]	ni_id	The Network Interface ID

@Param[in]	mfl	MFL length.

@Return	0 on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_set_mfl(uint16_t ni_id,
                          const uint16_t mfl);

/**************************************************************************//**
@Function	dpni_drv_get_mfl

@Description	Get the maximum received frame length.

@Param[in]	ni_id	The Network Interface ID

@Param[in]	*mfl	pointer to store MFL length.

@Return	0 on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_get_mfl(uint16_t ni_id,
                          uint16_t *mfl);

/**************************************************************************//**
@Function	dpni_drv_send

@Description	Network Interface send (AIOP enqueue) function.

@Param[in]	ni_id   The Network Interface ID
	Implicit: Queueing Destination Priority (qd_priority) in the TLS.

@Return	OK on success; error code, otherwise.
		For error codes refer to
		\ref FDMA_ENQUEUE_FRAME_ERRORS
		\ref DPNI_DRV_STATUS.
*//***************************************************************************/
int dpni_drv_send(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_explicit_send

@Description	Network Interface explicit send (AIOP enqueue) function.

@Param[in]	ni_id	The Network Interface ID
	Implicit: Queuing Destination Priority (qd_priority) in the TLS.

@Param[in]	fd	pointer to explicit FD. The assumption is that user
		used fdma function to create an explicit FD as
		fdma_create_frame

@Return	OK on success; error code, otherwise.
		For error codes refer to \ref FDMA_ENQUEUE_FD_ERRORS
		and \ref DPNI_DRV_STATUS.
*//***************************************************************************/
int dpni_drv_explicit_send(uint16_t ni_id, struct ldpaa_fd *fd);

/** @} */ /* end of dpni_g DPNI group */
#endif /* __FSL_DPNI_DRV_H */
