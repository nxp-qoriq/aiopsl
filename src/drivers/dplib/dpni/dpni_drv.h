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
#include "fsl_sl_dpni_drv.h"

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



/** @} */ /* end of DPNI_DRV_STATUS group */
#endif /* __DPNI_DRV_H */
