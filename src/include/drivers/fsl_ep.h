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
@File		fsl_ep.h

@Description	Entry Point driver API
*//***************************************************************************/

#ifndef __FSL_EP_H
#define __FSL_EP_H

/**************************************************************************//**
@Group		ep_g EP

@Description	Entry Point lookup table

@{
*//***************************************************************************/


/**************************************************************************//**
 @Group		EP_INIT_PRESENTATION_OPT Initial Presentation Options

 @Description	initial presentation modification options

 @{
*//***************************************************************************/
/** Select to modify the PTA Presentation address */
#define EP_INIT_PRESENTATION_OPT_PTA             0x0004
/** Select to modify the ASA presentation address */
#define EP_INIT_PRESENTATION_OPT_ASAPA           0x0008
/** Select to modify the ASA presentation offset */
#define EP_INIT_PRESENTATION_OPT_ASAPO           0x0010
/** Select to modify the ASA presentation size */
#define EP_INIT_PRESENTATION_OPT_ASAPS           0x0020
/** Select to modify the segment presentation address */
#define EP_INIT_PRESENTATION_OPT_SPA             0x0040
/** Select to modify the segment presentation size */
#define EP_INIT_PRESENTATION_OPT_SPS             0x0080
/** Select to modify the segment presentation offset */
#define EP_INIT_PRESENTATION_OPT_SPO             0x0100
/** Select to modify the segment reference bit */
#define EP_INIT_PRESENTATION_OPT_SR              0x0200
/** Select to modify no data segment bit */
#define EP_INIT_PRESENTATION_OPT_NDS             0x0400
/** @} end of group EP_INIT_PRESENTATION_OPT */

/**************************************************************************//**
@Group		EP_INIT_PRESENTATION Initial Presentation

@Description	Structure representing initial presentation settings.

@{
*//***************************************************************************/
struct ep_init_presentation {
	/** Flags representing the requested modifications to initial
	 * presentation;
	 * Use any combination of \ref EP_INIT_PRESENTATION_OPT */
	uint16_t options;
	/** FD Presentation Address - updating this
	 * value is not supported*/
	uint16_t fdpa;
	/** Additional Dequeue and Presentation Context Address - updating this
	 * value is not supported*/
	uint16_t adpca;
	/** PTA Presentation Address */
	uint16_t ptapa;
	/** ASA Presentation Address */
	uint16_t asapa;
	/** Offset within ASA to begin presenting from */
	uint8_t  asapo;
	/** ASA Presentation Size */
	uint8_t  asaps;
	/** Segment Presentation Address */
	uint16_t spa;
	/** Segment Presentation Size */
	uint16_t sps;
	/** Segment Presentation Offset within frame to begin presenting */
	uint16_t spo;
	/** Segment Reference bit - 0: start, 1: end. (Reference within the
	 * frame to present from) */
	uint8_t  sr;
	/**  No Data Segment bit - 1: to not allocate data segment  */
	uint8_t  nds;
};
/** @} end of group EP_INIT_PRESENTATION */

/** @} end of group ep_g */
#endif /* __FSL_EP_H */
