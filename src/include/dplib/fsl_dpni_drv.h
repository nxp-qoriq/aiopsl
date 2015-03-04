/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		fsl_dpni_drv.h

@Description	Data Path Network Interface API
*//***************************************************************************/
#ifndef __FSL_DPNI_DRV_H
#define __FSL_DPNI_DRV_H

#include "types.h"
#include "fsl_dpni.h"
#include "fsl_ldpaa.h"
#include "dpni_drv.h"
#include "dpni_drv_rxtx_inline.h"


/**************************************************************************//**
@Group		dpni_g DPNI

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	dpni_drv_register_rx_cb

@Description	Attaches a pointer to a call back function to a NI ID.

	The callback function will be called when the NI_ID receives a frame.

@Param[in]	ni_id  - The Network Interface ID
@Param[in]	cb - Callback function for Network Interface specified flow_id

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_register_rx_cb(uint16_t        ni_id,
			rx_cb_t		    *cb);

/**************************************************************************//**
@Function	dpni_drv_unregister_rx_cb

@Description	Unregisters a NI callback function by replacing it with a
		pointer to a discard callback.
		The discard callback function will be called when the NI_ID
		receives a frame

@Param[in]	ni_id - The Network Interface ID

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_unregister_rx_cb(uint16_t		ni_id);

/**************************************************************************//**
@Function	dpni_get_receive_niid

@Description	Get ID of NI on which the default packet arrived.

@Return	NI_IDs on which the default packet arrived.
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
uint16_t dpni_get_receive_niid(void);

/**************************************************************************//**
@Function	dpni_set_send_niid

@Description	Set the NI ID on which the packet should be sent.

@Param[in]	niid - The Network Interface ID

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_set_send_niid(uint16_t niid);

/**************************************************************************//**
@Function	dpni_get_send_niid

@Description	Get ID of NI on which the default packet should be sent.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_get_send_niid(void);


/**************************************************************************//**
@Function	dpni_drv_get_primary_mac_address

@Description	Get Primary MAC address of NI.

@Param[in]	niid - The Network Interface ID

@Param[out]	mac_addr - stores primary MAC address of the supplied NI.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_drv_get_primary_mac_addr(uint16_t niid,
		uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_set_primary_mac_addr

@Description	Set Primary MAC address of NI.

@Param[in]	niid - The Network Interface ID

@Param[in]	mac_addr - primary MAC address for given NI.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_set_primary_mac_addr(uint16_t niid,
                uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_add_mac_address

@Description	Adds unicast/multicast filter MAC address.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mac_addr - MAC address to be added to NI unicast/multicast
				filter.
@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_add_mac_addr(uint16_t ni_id,
          		const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_remove_mac_address

@Description	Removes unicast/multicast filter MAC address.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mac_addr - MAC address to be removed from NI
				unicast/multicast filter.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_remove_mac_addr(uint16_t ni_id,
          		const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_set_max_frame_length

@Description	Set the maximum received frame length.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mfl - MFL length.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_set_max_frame_length(uint16_t ni_id,
                          const uint16_t mfl);

/**************************************************************************//**
@Function	dpni_drv_get_max_frame_length

@Description	Get the maximum received frame length.

@Param[in]	ni_id - The Network Interface ID

@Param[out]	*mfl - pointer to store MFL length.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_get_max_frame_length(uint16_t ni_id,
                          uint16_t *mfl);

/**************************************************************************//**
@Function	sl_prolog

@Description	Network Interface SL prolog function. It is recommended to call
		this function at the beginning of the upper layer entry-point
		function, in this way it assures that HW presentation context
		is preserved (as needed for OSM functionality and ni_id
		resolution).
		It is also recommended that user AIOP entry-point function is
		declared with __declspec(entry_point) to assure it is not
		dead-stripped by the compiler.

@Retval		0 - Success.
		It is recommended that for any error value user should discard
		the frame and terminate the task.
@Retval		EIO - Parsing Error
@Retval		ENOSPC - Parser Block Limit Exceeds.
*//***************************************************************************/
inline int sl_prolog(void);

/**************************************************************************//**
@Function	dpni_drv_send

@Description	Network Interface send (AIOP enqueue) function.

@Param[in]	ni_id - The Network Interface ID
	Implicit: Queuing Destination Priority (qd_priority) in the TLS.

@Retval		0 - Success.
		It is recommended that for any error value user should discard
		the frame and terminate the task.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
@Retval		ENOMEM - Failed due to buffer pool depletion.
*//***************************************************************************/
inline int dpni_drv_send(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_explicit_send

@Description	Network Interface explicit send (AIOP enqueue) function.

@Param[in]	ni_id - The Network Interface ID
	Implicit: Queuing Destination Priority (qd_priority) in the TLS.

@Param[in]	fd - pointer to explicit FD. The assumption is that user
		used fdma function to create an explicit FD as
		fdma_create_frame

@Retval		0 - Success.
		It is recommended that for any error value user should discard
		the frame and terminate the task.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
*//***************************************************************************/
int dpni_drv_explicit_send(uint16_t ni_id, struct ldpaa_fd *fd);

/**************************************************************************//**
@Function	dpni_drv_set_multicast_promisc

@brief		Enable/Disable multicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[in]	en - '1' for enabling/'0' for disabling

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_set_multicast_promisc(uint16_t ni_id, int en);

/**************************************************************************//**
@Function	dpni_drv_get_multicast_promisc

@brief		Get multicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[out]	en - '1' for enabled/'0' for disabled

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_get_multicast_promisc(uint16_t ni_id, int *en);

/**************************************************************************//**
@Function	dpni_drv_set_unicast_promisc

@brief		Enable/Disable unicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[in]	en - '1' for enabling/'0' for disabling

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_set_unicast_promisc(uint16_t ni_id, int en);

/**************************************************************************//**
@Function	dpni_drv_get_unicast_promisc

@brief		Get unicast promiscuous mode

@param[in]	ni_id - The Network Interface ID
@param[out]	en - '1' for enabled/'0' for disabled

@returns	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_get_unicast_promisc(uint16_t ni_id, int *en);

/**************************************************************************//**
@Function	dpni_drv_get_spid

@Description	Function to receive PEB storage profile ID for specified NI.

@Param[in]	ni_id   The Network Interface ID
@Param[out]	spid - storage profile to use PEB buffer pool(for now using 1 byte).

@Return	'0' on Success;
*//***************************************************************************/
int dpni_drv_get_spid(uint16_t ni_id, uint16_t *spid);

/**************************************************************************//**
@Function	dpni_drv_get_spid_ddr

@Description	Function to receive DDR storage profile ID for specified NI.

@Param[in]	ni_id   The Network Interface ID
@Param[out]	spid_ddr - storage profile to use DDR buffer pool
		(for now using 1 byte).

@Return	'0' on Success;
*//***************************************************************************/
int dpni_drv_get_spid_ddr(uint16_t ni_id, uint16_t *spid_ddr);

/**************************************************************************//**
@Function	dpni_drv_set_concurrent

@Description	Function to set the initial ordering mode to concurrent for the given NI.

@Param[in]	ni_id   The Network Interface ID

@Cautions       This method should be called in boot mode only.

@Return	'0' on Success;
*//***************************************************************************/
int dpni_drv_set_concurrent(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_exclusive

@Description	Function to set the initial ordering mode to exclusive for the given NI.

@Param[in]	ni_id   The Network Interface ID

@Cautions       This method should be called in boot mode only.

@Return	'0' on Success;

*//***************************************************************************/
int dpni_drv_set_exclusive(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_order_scope

@Description	Function to set order scope source for the specified NI.

@Param[in]	ni_id   The Network Interface ID

@Param[in]	key_cfg   A structure for defining a full Key Generation
 		profile (rule)
@Cautions	This method should be called in boot mode only.


@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_set_order_scope(uint16_t ni_id, struct dpkg_profile_cfg *key_cfg);

/**************************************************************************//**
@Function	dpni_drv_get_connected_aiop_ni_id

@Description	Function to receive the connected NI ID.

@Param[in]	dpni_id   The Network Interface ID

@Param[out]	aiop_niid   Connected Network Interface ID to the given NI ID

@Param[out]	state   link state on success: 1 - link is up, 0 - link is down;

@Return 0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_connected_aiop_ni_id(const uint16_t dpni_id, uint16_t *aiop_niid, int *state);

/**************************************************************************//**
@Function	dpni_drv_get_connected_dpni_id

@Description	Function to receive the connected DPNI ID.

@Param[in]	aiop_niid   The AIOP Network Interface ID

@Param[out]	dpni_id   Connected DPNI ID to the given NI ID

@Param[out]	state   link state on success: 1 - link is up, 0 - link is down;

@Return 0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_connected_dpni_id(const uint16_t aiop_niid, uint16_t *dpni_id, int *state);

/**************************************************************************//**
@Function	dpni_drv_get_rx_buffer_layout

@Description	Function to receive SP’s attributes for RX buffer.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[out]	layout  Structure representing DPNI buffer layout

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_rx_buffer_layout(uint16_t ni_id, struct dpni_buffer_layout *layout);

/**************************************************************************//**
@Function	dpni_drv_get_counter

@Description	Function to receive DPNI counter.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	counter Type of DPNI counter.

@Param[out]	value   Counter value for the requested type.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_counter(uint16_t ni_id, enum dpni_counter counter, uint64_t *value);

/**************************************************************************//**
@Function	dpni_drv_get_dpni_id

@Description	Function to receive DPNI ID, known outside to AIOP.

@Param[in]	ni_id   The AIOP Network Interface ID.

@Param[out]	dpni_id DPNI ID known outside to AIOP.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_dpni_id(uint16_t ni_id, uint16_t *dpni_id);

/**************************************************************************//**
@Function	dpni_drv_get_ni_id

@Description	Function to receive AIOP internal NI ID.

@Param[in]	dpni_id DPNI ID known outside to AIOP.

@Param[out]	ni_id The AIOP Network Interface ID.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_ni_id(uint16_t dpni_id, uint16_t *ni_id);

/**************************************************************************//**
@Function	dpni_drv_get_link_state

@Description	Function to receive DPNI link state for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[out]	state Returned link state.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_link_state(uint16_t ni_id, struct dpni_link_state *state);

/**************************************************************************//**
@Function	dpni_drv_clear_mac_filters

@Description	Function to clear DPNI unicast or multicast addresses.
		The primary MAC address is not cleared by this operation.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	unicast Set to '1' to clear unicast addresses.

@Param[in]	multicast Set to '1' to clear multicast addresses.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_clear_mac_filters(uint16_t ni_id, uint8_t unicast, uint8_t multicast);

/**************************************************************************//**
@Function	dpni_drv_clear_vlan_filters

@Description	Function to clear VLAN filters for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_clear_vlan_filters(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_vlan_filters

@Description	Function to set VLAN filters for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	en Set to '1' to enable; '0' to disable.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_vlan_filters(uint16_t ni_id, int en);

/**************************************************************************//**
@Function	dpni_drv_add_vlan_id

@Description	Function to add VLAN filters for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	vlan_id VLAN ID to add to given NI.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_add_vlan_id(uint16_t ni_id, uint16_t vlan_id);

/**************************************************************************//**
@Function	dpni_drv_remove_vlan_id

@Description	Function to remove VLAN filters in given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	vlan_id VLAN ID to remove in given NI.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_remove_vlan_id(uint16_t ni_id, uint16_t vlan_id);

/** @} */ /* end of dpni_g DPNI group */
#endif /* __FSL_DPNI_DRV_H */
