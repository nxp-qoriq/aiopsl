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
@File		fsl_dpci_drv.h

@Description	Data Path Command Interface API
*//***************************************************************************/
#ifndef __FSL_DPCI_DRV_H
#define __FSL_DPCI_DRV_H

#include "fsl_ep.h"

/**************************************************************************//**
@Group		dpci_drv_g DPCI DRV

@Description	Contains the API for DPCI devices which are used by command
 	 	interface

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		DPCI_DRV_INIT_PR_Flags  DPCI Entry Point flags

@Description	Flags to get or set intial presentation of the client or server

@{
*//***************************************************************************/

#define DPCI_DRV_EP_SERVER	0
/**< Read or update server entry point */
#define DPCI_DRV_EP_CLIENT	1
/**< Read or update client asynchronous responses entry point */

/** @} end of group DPCI_DRV_INIT_PR_Flags */

/**************************************************************************//**
@Function	dpci_drv_enable

@Description	Enable the DPCI.

If the peer DPCI is already enabled then it will result in link up.

@Param[in]	dpci_id - DPCI id of the AIOP side.

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_drv_enable(uint32_t dpci_id);

/**************************************************************************//**
@Function	dpci_drv_disable

@Description	Disable the DPCI.

It will result in link down.

@Param[in]	dpci_id - DPCI id of the AIOP side.

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_drv_disable(uint32_t dpci_id);

/**************************************************************************//**
@Function	dpci_drv_linkup

@Description	Get linkup status.

@Param[in]	dpci_id - DPCI id of the AIOP side.
@Param[out]	up - 1 if the link is up or 0 otherwise.

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_drv_linkup(uint32_t dpci_id, int *up);

/**************************************************************************//**
@Function	dpci_drv_get_initial_presentation

@Description	Function to get initial presentation settings from EPID table.

@Param[in]	flags \link DPCI_DRV_INIT_PR_Flags
		DPCI Entry Point flags \endlink

@Param[out]	init_presentation Get initial presentation parameters
 	 	 \ref EP_INIT_PRESENTATION

@Return		0 on success;
		error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpci_drv_get_initial_presentation(uint8_t flags,
	struct ep_init_presentation* const init_presentation);

/**************************************************************************//**
@Function	dpni_drv_set_initial_presentation

@Description	Function to set initial presentation settings in EPID table.

@Param[in]	flags \link DPCI_DRV_INIT_PR_Flags
		DPCI Entry Point flags \endlink

@Param[in]	init_presentation Set initial presentation parameters for given
		options and parameters \ref EP_INIT_PRESENTATION

@Cautions	1) Data Segment, PTA Segment, ASA Segment must not reside
		   outside the bounds of the
		   presentation area. i.e. They must not fall within the HWC,
		   TLS or Stack areas.
		2) There should not be any overlap among the Segment, PTA & ASA.
		3) Minimum presented segment size must be configured.

@Return		0 on success;
		error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int dpci_drv_set_initial_presentation(uint8_t flags,
	const struct ep_init_presentation* const init_presentation);

/**************************************************************************//**
@Function	dpci_drv_set_concurrent

@Description	Function to set the initial ordering mode to concurrent for
		for either command interface server or client

@Param[in]	flags \link DPCI_DRV_INIT_PR_Flags
		DPCI Entry Point flags \endlink

@Cautions       This method should be called in boot mode only.

@Return	'0' on Success;
*//***************************************************************************/
int dpci_drv_set_concurrent(uint8_t flags);


/**************************************************************************//**
@Function	dpci_drv_set_exclusive

@Description	Function to set the initial ordering mode to exclusive for
		for either command interface server or client

@Param[in]	flags \link DPCI_DRV_INIT_PR_Flags
		DPCI Entry Point flags \endlink

@Cautions       This method should be called in boot mode only.

@Return	'0' on Success;
*//***************************************************************************/
int dpci_drv_set_exclusive(uint8_t flags);

/**************************************************************************//**
@Function	dpci_drv_set_order_mode_none

@Description	Function to set the initial ordering mode to none for
		for either command interface server or client

@Param[in]	flags \link DPCI_DRV_INIT_PR_Flags
		DPCI Entry Point flags \endlink

@Cautions       This method should be called in boot mode only.

@Return	'0' on Success;
*//***************************************************************************/
int dpci_drv_set_order_mode_none(uint8_t flags);

/** @} */ /* end of dpci_drv_g */

#endif /* __FSL_DPCI_DRV_H */
