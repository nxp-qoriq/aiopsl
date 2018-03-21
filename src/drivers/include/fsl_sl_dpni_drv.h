/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		fsl_sl_dpni_drv.h

@Description	Data Path Network Interface API
*//***************************************************************************/
#ifndef __FSL_SL_DPNI_DRV_H
#define __FSL_SL_DPNI_DRV_H

#include "fsl_types.h"
#include "fsl_dpni.h"
#include "fsl_ldpaa.h"
#include "fsl_malloc.h"
#include "fsl_dpni_drv.h"
#include "fsl_sl_dprc_drv.h"
#include "dpni_drv_rxtx_inline.h"

/**************************************************************************//**
@Group		grp_dplib_aiop	DPLIB

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{
*//***************************************************************************/
/**************************************************************************//**
@Group		grp_dpni_aiop	DPNI (AIOP Data Path Network Interface API)

@Description	Contains initialization APIs and runtime control APIs for DPNI

@{

/**************************************************************************//**
@Group	DPNI_DRV_STATUS
@{
*//***************************************************************************/
/** MTU was crossed for DPNI driver send function */
#define	DPNI_DRV_MTU_ERR	(DPNI_DRV_MODULE_STATUS_ID | 0x1)
/* @} */

struct dpni_early_init_request{
	uint16_t head_room_sum;
	uint16_t tail_room_sum;
	uint16_t private_data_size_sum;
	uint32_t frame_anno;
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
@Function	dpni_drv_unprobe

@Description	Function to unprobe the DPNI object.

@Param[in]	aiop_niid - AIOP Network Interface ID

*//***************************************************************************/
void dpni_drv_unprobe(uint16_t aiop_niid);

/**************************************************************************//**
@Function	dpni_drv_probe

@Description	Function to probe the DPNI object.

@Param[in]	dprc - Pointer to resource container

@Param[in]	mc_niid - MC DP Network Interface ID

@Param[in]	label - label of the DPNI object

@Return	aiop_niid (aiop_niid >= 0)on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_probe(struct mc_dprc *dprc, uint16_t mc_niid, uint16_t *niid,
		   char *label);
/**************************************************************************//**
@Function	discard_rx_cb

@Description	Default call back function to discard frame and terminate task.

@Return	None.
*//***************************************************************************/
void discard_rx_cb(void);

/**************************************************************************//**
@Function	dpni_drv_is_dpni_exist

@Description	Returns TRUE if mc_niid already exist or FALSE otherwise.

@Param[in]	mc_niid - DPNI unique ID

@Return		-1 if not exist
		index in nis table if exist
*//***************************************************************************/
int dpni_drv_is_dpni_exist(uint16_t mc_niid);

/**************************************************************************//**
@Function	dpni_drv_update_obj

@Description	Update NIS table regarding given mc dpni id and mark if this
		object exist so the sync function wont remove it.

@Param[in]	dprc - A pointer for AIOP received container.

@Param[in]	mc_niid - DPNI id of an object in the container.

@Param[in]	label - label of the DPNI object.

@Return		0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_update_obj(struct mc_dprc *dprc, uint16_t mc_niid, char *label);

/**************************************************************************//**
@Function	dpni_drv_handle_removed_objects

@Description	Update NIS table and remove the NI's which were not found
		during the scan.

@Return		0 on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
void dpni_drv_handle_removed_objects(void);

/**************************************************************************//**
@Function	dpni_drv_get_ordering_mode

@Description	Function to enable / disable the IRQ for given NI.

@Param[in]	ni_id - Network Interface ID

@Param[in]	en - Interrupt state: - enable = 1, disable = 0

*//***************************************************************************/
int dpni_drv_set_irq_enable(uint16_t ni_id, uint8_t en);

/**************************************************************************//**
@Function	dpni_drv_get_ingress_parse_profile_id

@Description	Get the ID of the "ingress" Parse Profile configured on a DPNI.
		All AIOP belonging DPNIs share the same "ingress" Parse Profile.

@Param[in]	ni_id : The AIOP Network Interface ID.

@Param[out]	prpid : On return contains the ID of the "ingress" Parse
		Profile.

@Return		None
*//***************************************************************************/
void dpni_drv_get_ingress_parse_profile_id(uint16_t ni_id, uint8_t *prpid);

/**************************************************************************//**
@Function	dpni_drv_get_egress_parse_profile_id

@Description	Get the ID of the "egress" Parse Profile configured on a DPNI.
		All AIOP belonging DPNIs share the same "egress" Parse Profile.

@Param[in]	ni_id : The AIOP Network Interface ID.

@Param[out]	prpid : On return contains the ID of the "egress" Parse
		Profile.

@Return		None
*//***************************************************************************/
void dpni_drv_get_egress_parse_profile_id(uint16_t ni_id, uint8_t *prpid);

/** @} */ /* end of DPNI_DRV_STATUS group */
#endif /* __FSL_SL_DPNI_DRV_H */
