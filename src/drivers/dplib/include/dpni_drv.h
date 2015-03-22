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
@File		dpni_drv.h

@Description	Data Path Network Interface API
*//***************************************************************************/
#ifndef __DPNI_DRV_H
#define __DPNI_DRV_H

#include "types.h"
#include "fsl_dpni.h"
#include "fsl_ldpaa.h"
#include "fsl_platform.h"

/**************************************************************************//**
@Group		grp_dplib_aiop	DPLIB

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/
/**************************************************************************//**
@Group		grp_dpni_aiop	DPNI (AIOP Data Path Network Interface API)

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/

/* TODO - move to soc files */
#define SOC_MAX_NUM_OF_DPNI		64

#define DPNI_DRV_FAST_MEMORY    MEM_PART_PEB
#define DPNI_DRV_DDR_MEMORY     MEM_PART_DP_DDR
#define DPNI_DRV_NUM_USED_BPIDS   BPIDS_USED_FOR_POOLS_IN_DPNI
#define DPNI_DRV_PEB_BPID_IDX         0
#define DPNI_DRV_DDR_BPID_IDX         1
#define SP_MASK_BMT_AND_RSV       0xC000FFFF
#define SP_MASK_BPID              0x3FFF
#define ORDER_MODE_CLEAR_BIT      0xFEFFFFFF /*clear the bit for exclusive / concurrent mode*/
#define ORDER_MODE_BIT_MASK       0x01000000
#define DPNI_DRV_CONCURRENT_MODE      0
#define DPNI_DRV_EXCLUSIVE_MODE       1
#define PARAMS_IOVA_BUFF_SIZE         256
#define PARAMS_IOVA_ALIGNMENT         8

/**************************************************************************//**
@Group	DPNI_DRV_STATUS
@{
*//***************************************************************************/
/** MTU was crossed for DPNI driver send function */
#define	DPNI_DRV_MTU_ERR	(DPNI_DRV_MODULE_STATUS_ID | 0x1)
/* @} */

typedef uint64_t	dpni_drv_app_arg_t;

/* TODO: need to define stats */
struct dpni_stats {
	int num_pkts;
};

struct dpni_early_init_request{
	uint16_t head_room_sum; 
	uint16_t tail_room_sum; 
	uint16_t private_data_size_sum;
	
	uint16_t count; /* Count how many requests have been registered */
};

/**************************************************************************//**
@Description   Information for every bpid
*//***************************************************************************/
struct aiop_psram_entry {
    uint32_t    aiop_specific;
    /**< IP-Specific Storage Profile Information (e.g., ICIDs, cache controls, etc. */
    uint32_t    reserved0;

    uint32_t    frame_format_low;
    /**< Frame Format and Data Placement Controls */
    uint32_t    frame_format_high;
    /**< Frame Format and Data Placement Controls */

    uint32_t    bp1;
    /**< Buffer Pool 1 Attributes and Controls */
    uint32_t    bp2;
    /**< Buffer Pool 2 Attributes and Controls */
    uint32_t    reserved1;
    uint32_t    reserved2;
};

/**************************************************************************//**
@Description	Application Receive callback

		User provides this function. Driver invokes it when it gets a
		frame received on this interface.


@Return	OK on success; error code, otherwise.
*//***************************************************************************/
typedef void /*__noreturn*/ (rx_cb_t) (void);

/**************************************************************************//**
@Function	discard_rx_cb

@Description	Default call back function to discard frame and terminate task.

@Return	None.
*//***************************************************************************/
void discard_rx_cb(void);

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
@Function	dpni_drv_get_attrib

@Description	Returns the value of the NI_ID attribute specified with the
	attrib argument. See the dpni_ni_attrib enum for the list of supported
	attributes.

@Param[in]	ni_id   The Network Interface ID
@Param[in]	attrib  The NI attribute to be returned

@Return        The value of the attribute, negative value in case of an error
*//***************************************************************************/
int dpni_drv_get_attrib(uint16_t ni_id, int attrib);

/**************************************************************************//**
@Function	dpni_drv_get_stats

@Description	Retrieve NI statistics

@Param[in]	ni_id - Network Interface ID
@Param[out]	stats - Statics

@Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_get_stats(uint16_t	ni_id, struct dpni_stats *stats);

/**************************************************************************//**
@Function	dpni_drv_reset_stats

@Description	Reset NI statistics.

@Param[in]	ni_id - Network Interface ID

@Return	'0' on Success; Error code otherwise.
*//***************************************************************************/
int dpni_drv_reset_stats(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_register_discard_rx_cb

@Description	register a default receive callback functions.
		The default callback function will be called when a frame is
		received at a NI_ID that does not have a registered callback.
		Packet arrives on an NI, there is registered handler. AIOP SL
		invokes the registered callback.  wing logic is implemented:
		Packet arrives on an NI, there is no registered handler.
		AIOP SL checks if
		there is a default handler and invokes the registered callback
		for the default handler.  Packet arrives on an NI, there is no
		registered handler. Also, there is no registered default
		handler. AIOP SL drops the packet.

@Param[in]	dpio    TODO
@Param[in]	dpsp    TODO
@Param[in]	cb      TODO
@Param[in]	arg     TODO

@Return	OK on success; error code, otherwise.
*//***************************************************************************/
int dpni_drv_register_discard_rx_cb(
		fsl_handle_t		dpio,
		fsl_handle_t		dpsp,
		rx_cb_t			*cb,
		dpni_drv_app_arg_t	arg);


/**************************************************************************//**
@Function	dpni_get_num_of_ni

@Description	Returns the number of NI_IDs in the system.  Called by the AIOP
		applications to learn the maximum number of available network
		interfaces.

@Return	Number of NI_IDs in the system
*//***************************************************************************/
int dpni_get_num_of_ni(void);

/**************************************************************************//**
@Function	dpni_drv_get_ordering_mode

@Description	Returns the configuration in epid table for ordering mode.
@Param[in]	ni_id - Network Interface ID

@Return	Ordering mode for given NI
		0 - Concurrent
		1 - Exclusive
*//***************************************************************************/
int dpni_drv_get_ordering_mode(uint16_t ni_id);

/**************************************************************************//**
@Function	dpni_drv_set_rx_buffer_layout

@Description	Function to change SP’s attributes (specify how many headroom)

@Param[in]	ni_id   The AIOP Network Interface ID

@Param[in]	layout  Structure representing DPNI buffer layout

@warning	Allowed only when DPNI is disabled

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpni_drv_set_rx_buffer_layout(uint16_t ni_id, const struct dpni_buffer_layout *layout);

/**************************************************************************//**
@Function	dpni_register_requirements

@Description	register a request for DPNI requirement.

@Param[in]	head_room           Requested head room.
@Param[in]	tail_room           Requested tail room.
@Param[in]	private_data_size   Requested private data size.

@Return		0        - on success,
		-ENAVAIL - resource not available or not found,
		-ENOMEM  - not enough memory.
 *//***************************************************************************/
int dpni_register_requirements(uint16_t head_room, uint16_t tail_room, uint16_t private_data_size);

/** @} */ /* end of DPNI_DRV_STATUS group */
#endif /* __DPNI_DRV_H */
