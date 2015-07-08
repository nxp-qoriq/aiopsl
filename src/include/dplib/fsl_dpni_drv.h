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

#include "fsl_ldpaa.h"
#include "fsl_net.h"
#include "fsl_dpkg.h"
#include "dpni_drv_rxtx_inline.h"
#include "fsl_ep.h"


/**************************************************************************//**
@Group		dpni_drv_g DPNI DRV

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		DPNI_DRV_ORDER_SCOPE Order Scope options

 @Description	Set initial order scope to "No order scope".

 @{
*//***************************************************************************/
#define DPNI_DRV_NO_ORDER_SCOPE 0
/** @} end of group DPNI_DRV_ORDER_SCOPE */

/**************************************************************************//**
 @Group		DPNI_DRV_LINK_OPT Link Options

 @Description	Available options to determine dpni link state.

 @{
*//***************************************************************************/
/** Enable auto-negotiation */
#define DPNI_DRV_LINK_OPT_AUTONEG		0x0000000000000001ULL
/** Enable half-duplex mode */
#define DPNI_DRV_LINK_OPT_HALF_DUPLEX		0x0000000000000002ULL
/** @} end of group DPNI_DRV_LINK_OPT */

/**************************************************************************//**
@Description	Structure representing DPNI driver link state.

*//***************************************************************************/
struct dpni_drv_link_state {
	/** Rate */
	uint64_t rate;
	/** Mask of available options; use \ref DPNI_DRV_LINK_OPT values*/
	uint64_t options;
	/** Link state; '0' for down, '1' for up */
	int up;
};


/**************************************************************************//**
@enum dpni_drv_counter

@Description	AIOP DPNI driver counter types

@{
*//***************************************************************************/

enum dpni_drv_counter {
	/** Counts ingress frames */
	DPNI_DRV_CNT_ING_FRAME = 0x0,
	/** Counts ingress bytes */
	DPNI_DRV_CNT_ING_BYTE = 0x1,
	/** Counts ingress frames dropped due to explicit 'drop' setting */
	DPNI_DRV_CNT_ING_FRAME_DROP = 0x2,
	/** Counts ingress frames discarded due to errors */
	DPNI_DRV_CNT_ING_FRAME_DISCARD = 0x3,
	/** Counts ingress multicast frames */
	DPNI_DRV_CNT_ING_MCAST_FRAME = 0x4,
	/** Counts ingress multicast bytes */
	DPNI_DRV_CNT_ING_MCAST_BYTE = 0x5,
	/** Counts ingress broadcast frames */
	DPNI_DRV_CNT_ING_BCAST_FRAME = 0x6,
	/** Counts ingress broadcast bytes */
	DPNI_DRV_CNT_ING_BCAST_BYTES = 0x7,
	/** Counts egress frames */
	DPNI_DRV_CNT_EGR_FRAME = 0x8,
	/** Counts egress bytes */
	DPNI_DRV_CNT_EGR_BYTE = 0x9,
	/** Counts egress frames discarded due to errors */
	DPNI_DRV_CNT_EGR_FRAME_DISCARD = 0xa
};
/* @} end of enum dpni_drv_counter */


/**************************************************************************//**
 @Group		DPNI_DRV_BUF_LAYOUT_OPT Buffer Layout modification options

 @Description	buffer layout modification options

 @{
*//***************************************************************************/
/** Select to modify the time-stamp setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_TIMESTAMP               0x00000001
/** Select to modify the parser-result setting; not applicable for Tx */
#define DPNI_DRV_BUF_LAYOUT_OPT_PARSER_RESULT           0x00000002
/** Select to modify the frame-status setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_FRAME_STATUS            0x00000004
/** Select to modify the private-data-size setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_PRIVATE_DATA_SIZE	0x00000008
/** Select to modify the data-alignment setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_DATA_ALIGN              0x00000010
/** Select to modify the data-head-room setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_DATA_HEAD_ROOM          0x00000020
/**!< Select to modify the data-tail-room setting */
#define DPNI_DRV_BUF_LAYOUT_OPT_DATA_TAIL_ROOM          0x00000040
/** @} end of group DPNI_DRV_BUF_LAYOUT_OPT */

/**************************************************************************//**
@Description	Structure representing DPNI buffer layout.

*//***************************************************************************/
struct dpni_drv_buf_layout {
	/** Flags representing the suggested modifications to the buffer
	 * layout; Use any combination of \ref DPNI_DRV_BUF_LAYOUT_OPT */
	uint32_t options;
	/** Pass timestamp value */
	int pass_timestamp;
	/** Pass parser results */
	int pass_parser_result;
	/** Pass frame status */
	int pass_frame_status;
	/** Size kept for private data (in bytes) */
	uint16_t private_data_size;
	/** Data alignment */
	uint16_t data_align;
	/** Data head room */
	uint16_t data_head_room;
	/** Data tail room */
	uint16_t data_tail_room;
};

/**************************************************************************//**
@Description	Structure representing DPNI ls_checksum.

*//***************************************************************************/
struct dpni_drv_tx_checksum {
	/* '1' to enable L3 checksum generation; '0' disable;*/
	uint16_t l3_checksum_gen;
	/* '1' to enable L4 checksum generation; '0' disable;*/
	uint16_t l4_checksum_gen;
};

/**************************************************************************//**
@Description	Application Receive callback

		User provides this function. Driver invokes it when it gets a
		frame received on this interface.


@Return	OK on success; error code, otherwise.
*//***************************************************************************/
typedef void /*__noreturn*/ (rx_cb_t) (void);

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
@Function	dpni_drv_enable

@Description	Enable a NI_ID referenced by ni_id. Allows sending and
		receiving frames.

@Param[in]	ni_id   The Network Interface ID

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_enable(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_disable

@Description	Disables a NI_ID referenced by ni_id. Disallows sending and
		receiving frames

@Param[in]	ni_id	The Network Interface ID

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_disable(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_get_receive_niid

@Description	Get ID of NI on which the default packet arrived.

@Return	NI_IDs on which the default packet arrived.
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
inline uint16_t dpni_get_receive_niid(void);

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

@Param[in]	ni_id - The Network Interface ID

@Param[out]	mac_addr - stores primary MAC address of the supplied NI.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
/* TODO : replace by macros/inline funcs */
int dpni_drv_get_primary_mac_addr(uint16_t ni_id,
		uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE]);

/**************************************************************************//**
@Function	dpni_drv_set_primary_mac_addr

@Description	Set Primary MAC address of NI.

@Param[in]	ni_id - The Network Interface ID

@Param[in]	mac_addr - primary MAC address for given NI.

@Return	0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_set_primary_mac_addr(uint16_t ni_id,
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
@Function	sl_tman_expiration_task_prolog

@Description	Network Interface SL tman expiration task prolog function.
		This function initialize into WS the ICID as taken from the
		SPID and clear starting HXS and PRPID.
		It should be called from the beginning of user's timer
		expiration call-back function to assure that fdma create and
		fdma store functions will work properly.

@param[in]	spid - storage profile id.

*//***************************************************************************/
inline void sl_tman_expiration_task_prolog(uint16_t spid);

/**************************************************************************//**
@Function	dpni_drv_send

@Description	Network Interface send (AIOP store and enqueue) function.
	Store and enqueue the default Working Frame.

@Param[in]		ni_id - The Network Interface ID
	Implicit: Queuing Destination Priority (qd_priority) in the TLS.

@Retval			0 - Success.
	It is recommended that for any error value user should discard
	the frame and terminate the task.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN. It is
	recommended calling fdma_discard_fd() afterwards and then terminate task.
@Retval		ENOMEM - Failed due to buffer pool depletion. It is recommended
	calling fdma_discard_default_frame() afterwards and then terminate task.
@Cautions      The frame to be enqueued must be open (presented)
	when calling this function
*//***************************************************************************/
inline int dpni_drv_send(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_explicit_send

@Description	Network Interface explicit send (AIOP enqueue) function.
	Enqueue the explicit closed frame.

@Param[in]		ni_id - The Network Interface ID
	Implicit: Queuing Destination Priority (qd_priority) in the TLS.

@Param[in]		fd - pointer to the explicit FD.

@Retval			0 - Success.
	It is recommended that for any error value user should discard
	the frame and terminate the task.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN. It is
	recommended calling fdma_discard_fd() afterwards and then terminate task.

@Cautions	The frame to be enqueued must be closed (stored) when calling
	this function

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
@Function	dpni_drv_get_num_of_nis

@Description	Returns the number of NI_IDs in the system.  Called by the AIOP
		applications to learn the maximum number of available network
		interfaces.

@Return	Number of NI_IDs in the system
*//***************************************************************************/
int dpni_drv_get_num_of_nis(void);

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
		To disable order scope refer to \ref DPNI_DRV_ORDER_SCOPE

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

@Deprecated	The function is deprecated and will be removed in the next release.
		The function: "dpni_drv_get_connected_ni()" should be used instead.
*//***************************************************************************/
int dpni_drv_get_connected_aiop_ni_id(const uint16_t dpni_id, uint16_t *aiop_niid, int *state);

/**************************************************************************//**
@Function	dpni_drv_get_connected_ni

@Description	Function to receive the connected AIOP NI ID for OBJ ID and type.

@Param[in]	id  object ID to find connection.

@Param[in]	type  The type of the give object ID ("dpni", dpmac").

@Param[out]	aiop_niid  Connected NI ID.

@Param[out]	state  link state on success: 1 - link is up, 0 - link is down;

@Return 0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_connected_ni(const int id, const char type[16], uint16_t *aiop_niid, int *state);

/**************************************************************************//**
@Function	dpni_drv_get_connected_dpni_id

@Description	Function to receive the connected DPNI ID.

@Param[in]	aiop_niid   The AIOP Network Interface ID

@Param[out]	dpni_id   Connected DPNI ID to the given NI ID

@Param[out]	state   link state on success: 1 - link is up, 0 - link is down;

@Return 0 on success;
	error code, otherwise. For error posix refer to \ref error_g

@Deprecated	The function is deprecated and will be removed in the next release.
		The function: "dpni_drv_get_connected_obj()" should be used instead.
*//***************************************************************************/
int dpni_drv_get_connected_dpni_id(const uint16_t aiop_niid, uint16_t *dpni_id, int *state);

/**************************************************************************//**
@Function	dpni_drv_get_connected_obj

@Description	Function to receive the connected OBJ ID and type.

@Param[in]	aiop_niid  The AIOP Network Interface ID

@Param[out]	id  Connected object ID to the given NI ID

@Param[out]	type  The type of the connected object for given NI.

@Param[out]	state  link state on success: 1 - link is up, 0 - link is down;

@Return 0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_connected_obj(const uint16_t aiop_niid, int *id, char type[16], int *state);
/**************************************************************************//**
@Function	dpni_drv_set_rx_buffer_layout

@Description	Function to change SP’s attributes (specify how many headroom)

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	layout  Structure representing DPNI buffer layout

@warning	Allowed only when DPNI is disabled

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_buffer_layout(uint16_t ni_id, const struct dpni_drv_buf_layout *layout);

/**************************************************************************//**
@Function	dpni_drv_get_rx_buffer_layout

@Description	Function to receive SP’s attributes for RX buffer.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[out]	layout  Structure representing DPNI buffer layout

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_rx_buffer_layout(uint16_t ni_id, struct dpni_drv_buf_layout *layout);

/**************************************************************************//**
@Function	dpni_drv_register_rx_buffer_layout_requirements

@Description	register a request for DPNI requirement.

@Param[in]	head_room           Requested head room.
@Param[in]	tail_room           Requested tail room.
@Param[in]	private_data_size   Requested private data size.

@Return		0        - on success,
		-ENAVAIL - resource not available or not found,
		-ENOMEM  - not enough memory.
 *//***************************************************************************/
int dpni_drv_register_rx_buffer_layout_requirements(uint16_t head_room, uint16_t tail_room, uint16_t private_data_size);

/**************************************************************************//**
@Function	dpni_drv_get_counter

@Description	Function to receive DPNI counter.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	counter Type of DPNI counter.

@Param[out]	value   Counter value for the requested type.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_counter(uint16_t ni_id, enum dpni_drv_counter counter, uint64_t *value);

/**************************************************************************//**
@Function	dpni_drv_reset_counter

@Description	Function to reset DPNI counter.

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	counter Type of DPNI counter.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_reset_counter(uint16_t ni_id, enum dpni_drv_counter counter);

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
int dpni_drv_get_link_state(uint16_t ni_id, struct dpni_drv_link_state *state);

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

/**************************************************************************//**
@Function	dpni_drv_get_initial_presentation

@Description	Function to get initial presentation settings from EPID table
		for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[out]	init_presentation Get initial presentation parameters
 	 	 \ref EP_INIT_PRESENTATION

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_initial_presentation(
	uint16_t ni_id,
	struct ep_init_presentation* const init_presentation);

/**************************************************************************//**
@Function	dpni_drv_set_initial_presentation

@Description	Function to set initial presentation settings in EPID table for
		given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	init_presentation Set initial presentation parameters for given
		options and parameters \ref EP_INIT_PRESENTATION

@Cautions	1) Data Segment, PTA Segment, ASA Segment must not reside
		   outside the bounds of the
		   presentation area. i.e. They must not fall within the HWC,
		   TLS or Stack areas.
		2) There should not be any overlap among the Segment, PTA & ASA.
		3) Minimum presented segment size must be configured.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_initial_presentation(
	uint16_t ni_id,
	const struct ep_init_presentation* const init_presentation);

/**************************************************************************//**
@Function	dpni_drv_set_tx_checksum

@Description	Function to enable/disable l3/l4 check-sum for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[in]	tx_checksum representing checksums to be enabled/disabled.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_tx_checksum(uint16_t ni_id,
                             const struct dpni_drv_tx_checksum * const tx_checksum);

/**************************************************************************//**
@Function	dpni_drv_get_tx_checksum

@Description	Function to get status of l3/l4 check-sum for given NI.

@Param[in]	ni_id The AIOP Network Interface ID.

@Param[out]	tx_checksum return the status for l3/l4 checksums.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_get_tx_checksum(uint16_t ni_id,
                            struct dpni_drv_tx_checksum * const tx_checksum);
/** @} */ /* end of dpni_drv_g DPNI DRV group */
#endif /* __FSL_DPNI_DRV_H */
