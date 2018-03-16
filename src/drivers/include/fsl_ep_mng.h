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
@File		fsl_ep_mng.h

@Description	Entry Point driver internal API
*//***************************************************************************/

#ifndef __FSL_EP_MNG_H
#define __FSL_EP_MNG_H

/**************************************************************************//**
 @Description   Take write mutex for EP entry
 *//***************************************************************************/
#define EP_MNG_MUTEX_W_TAKE \
do { \
cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK); \
} while(0)

/**************************************************************************//**
 @Description   Take read mutex for EP entry
 *//***************************************************************************/
#define EP_MNG_MUTEX_R_TAKE \
do { \
cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_READ_LOCK); \
} while(0)

/**************************************************************************//**
 @Description   Release mutex for EP entry
 *//***************************************************************************/
#define EP_MNG_MUTEX_RELEASE \
do { \
cdma_mutex_lock_release((uint64_t)&wrks_addr->epas); \
} while(0)

/**************************************************************************//**
@Function	ep_mng_get_initial_presentation

@Description	Function to get initial presentation settings from EPID table
		for given NI.

@Param[in]	epid The entry point index

@Param[out]	init_presentation Get initial presentation parameters
 	 	 \ref ep_initial_presentation

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int ep_mng_get_initial_presentation(
	uint16_t epid,
	struct ep_init_presentation* const init_presentation);

/**************************************************************************//**
@Function	ep_mng_set_initial_presentation

@Description	Function to set initial presentation settings in EPID table for
		given NI.

@Param[in]	epid The entry point index

@Param[in]	init_presentation Set initial presentation parameters for given
		options and parameters \ref ep_initial_presentation

@Cautions	1) Data Segment, PTA Segment, ASA Segment must not reside
		   outside the bounds of the
		   presentation area. i.e. They must not fall within the HWC,
		   TLS or Stack areas.
		2) There should not be any overlap among the Segment, PTA & ASA.
		3) Minimum presented segment size must be configured.

@Return	0 on success;
	error code, otherwise. For error posix refer to \ref error_g
*//***************************************************************************/
int ep_mng_set_initial_presentation(
	uint16_t epid,
	const struct ep_init_presentation* const init_presentation);

#endif
