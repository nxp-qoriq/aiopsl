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
#include "fsl_dpni_drv.h"
#include "fsl_dprc_drv.h"
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
*//***************************************************************************/

/* TODO - move to soc files */
#define SOC_MAX_NUM_OF_DPNI           64
#define DPNI_NOT_IN_USE               0xFFFF
#define DPNI_DRV_FAST_MEMORY    MEM_PART_PEB
#define DPNI_DRV_DDR_MEMORY     MEM_PART_DP_DDR
#define DPNI_DRV_NUM_USED_BPIDS   BPIDS_USED_FOR_POOLS_IN_DPNI
#define DPNI_DRV_PEB_BPID_IDX         0
#define DPNI_DRV_DDR_BPID_IDX         1
#define SP_MASK_BMT_AND_RSV       0xC000FFFF
#define SP_MASK_BPID              0x3FFF
#define ORDER_MODE_CLEAR_BIT      0xFEFFFFFF /*clear the bit for exclusive / concurrent mode*/
#define ORDER_MODE_BIT_MASK       0x01000000
#define ORDER_MODE_NO_ORDER_SCOPE 0xEEFCFFF8 /*clear src, ep, sel, osrm*/
#define DPNI_DRV_CONCURRENT_MODE      0
#define DPNI_DRV_EXCLUSIVE_MODE       1
#define PARAMS_IOVA_BUFF_SIZE         256
#define PARAMS_IOVA_ALIGNMENT         8
#define DPNI_DRV_PTA_SIZE             64

/* Default DPNI requirements values */
#define DPNI_DRV_DHR_DEF              96 /* Data Head Room */
#define DPNI_DRV_DTR_DEF              0  /* Data Tail Room */
#define DPNI_DRV_PTA_DEF              0  /* Pass Thru Annotation - Private Data Size */

/* Supported options for initializing presentation fields*/
#define DPNI_DRV_SUPPORTED_INIT_PRESENTATION_OPTIONS    \
		DPNI_DRV_INIT_PRESENTATION_OPT_PTA   |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_ASAPA |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_ASAPO |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_ASAPS |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_SPA   |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_SPS   |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_SPO   |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_SR    |  \
		DPNI_DRV_INIT_PRESENTATION_OPT_NDS

/* Initial Presentation settings, masks, shift */
#define FDPA_MASK                 0xFFE00000
#define FDPA_SHIFT                21
#define ADPCA_MASK                0x0000FFE0
#define ADPCA_SHIFT               5
#define PTAPA_MASK                0x0000FFC0
#define PTAPA_SHIFT               6
#define ASAPS_MASK                0x000F0000
#define ASAPS_SHIFT               16
#define ASAPA_MASK                0x0000FFC0
#define ASAPA_SHIFT               6
#define ASAPO_MASK                0x0000000F
#define SPA_MASK                  0x0000FFFF
#define SPS_MASK                  0xFFFF0000
#define SPS_SHIFT                 16
#define SR_MASK                   0x80000000
#define SR_SHIFT                  31
#define NDS_MASK                  0x02000000
#define NDS_SHIFT                 25
#define SPO_MASK                  0x0000FFFF

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
@Function	dpni_drv_probe

@Description	Function to probe the DPNI object.

@Param[in]	dprc - Pointer to resource container

@Param[in]	mc_niid - MC DP Network Interface ID

@Return	OK on success; error code, otherwise.
		For error posix refer to
		\ref error_g
*//***************************************************************************/
int dpni_drv_probe(struct mc_dprc *dprc,
                   uint16_t mc_niid);
/**************************************************************************//**
@Function	discard_rx_cb

@Description	Default call back function to discard frame and terminate task.

@Return	None.
*//***************************************************************************/
void discard_rx_cb(void);

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
@Function	dpni_drv_is_dpni_exist

@Description	Returns TRUE if mc_niid already exist or FALSE otherwise.

@Param[in]	mc_niid - DPNI unique ID

@Return		-1 if not exist
		index in nis table if exist
*//***************************************************************************/
int dpni_drv_is_dpni_exist(uint16_t mc_niid);

/**************************************************************************//**
@Function	dpni_drv_valid_dpnis

@Description	Turn on a bit that represent an index of existing dpni in nis
		table.

@Param[out]	valid_dpnis - register to represent indexes for existing valid
				dpnis.

*//***************************************************************************/
void dpni_drv_valid_dpnis(uint64_t *valid_dpnis);

/** @} */ /* end of DPNI_DRV_STATUS group */
#endif /* __DPNI_DRV_H */
