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
@File          apps.h

@Description   This file contains the AIOP SL user defined setup.
*//***************************************************************************/

/**************************************************************************//**
@Group		app_init	General

@Description	Use the following macros to define the general AIOP SL setup.
@{
*//***************************************************************************/

#define APP_INIT_TASKS_PER_CORE		4	/**< Tasks per core in AIOP */
#define APP_INIT_APP_MAX_NUM		10
/**<  Maximal number of applications to be filled by build_apps_array() */

/** @} */ /* end of app_init */

/**************************************************************************//**
@Group		app_mem		Memory Allocation

@Description	Use the following macros to define the memory sizes required by 
		the sum of all AIOP applications and Service Layer.
		All the sizes refer to the sum of all static (LCF) and 
		dynamic (malloc) allocations. 
		All the sizes should be aligned to a power of 2.
@{
*//***************************************************************************/
#define APP_MEM_DP_DDR_SIZE	(128 * MEGABYTE)/**< DP DDR size */ 
#define APP_MEM_PEB_SIZE	(512 * KILOBYTE)/**< PEB size */
#define APP_MEM_SYS_DDR1_SIZE 	(0)		/**< System DDR size */

/** @} */ /* end of app_mem */

/**************************************************************************//**
@Group		app_ctlu	CTLU

@Description	Use the following macros to define the CTLU setup.
@{
*//***************************************************************************/

#define APP_CTLU_SYS_DDR_NUM_ENTRIES	2048 /**< SYS DDR number of entries */
#define APP_CTLU_DP_DDR_NUM_ENTRIES	2048 /**< DP DDR number of entries */
#define APP_CTLU_PEB_NUM_ENTRIES	2048 /**< PEB number of entries */

/** @} */ /* end of app_ctlu */

/**************************************************************************//**
@Group		app_mflu	MFLU

@Description	Use the following macros to define the MFLU setup.
@{
*//***************************************************************************/

#define APP_MFLU_SYS_DDR_NUM_ENTRIES	2048 /**< SYS DDR number of entries */
#define APP_MFLU_DP_DDR_NUM_ENTRIES	2048 /**< DP DDR number of entries */
#define APP_MFLU_PEB_NUM_ENTRIES	2048 /**< PEB number of entries */

/** @} */ /* end of app_mflu */


/**************************************************************************//**
@Group		app_tman	TMAN

@Description	Use the following macros to define the TMAN setup.
@{
*//***************************************************************************/

#define APP_TMAN_FREQUENCY		800	/**< Tman frequency */

/** @} */ /* end of app_tman */

/**************************************************************************//**
@Group		app_dpni	DPNI

@Description	Use the following macros to define the DPNI driver.
		Two AIOP buffer pools are reserved for frame data buffers of all
		AIOP DPNIs; One for frame data residing in DP-DDR and one for
		frame data residing in PEB. The buffer amount, size and 
		alignment defined below applies to both pools and to all AIOP
		DPNIs. Each DPNI uses two Storage Profile IDs (one for DP-DDR
		and one for PEB) and therefore APP_DPNI_SPID_COUNT must
		accommodate at least two SPIDs per AIOP DPNI.
@{
*//***************************************************************************/

#define APP_DPNI_NUM_BUFS_IN_POOL	50	/**< Number of buffers */
#define APP_DPNI_BUF_SIZE_IN_POOL	2048	/**< Size of buffer */
#define APP_DPNI_BUF_ALIGN_IN_POOL	64	/**< Alignment of buffer */
#define APP_DPNI_SPID_COUNT		8	/**< Max number of SPIDs */

#define APP_DPNI_SP_DEF_DHR     96 /**< Data head room */
#define APP_DPNI_SP_DEF_DTR     0  /**< Data tail room */
#define APP_DPNI_SP_DEF_PDS     0  /**< Private data size */

/** @} */ /* end of app_dpni */




