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
@File		fdma.h

@Description	This file contains the AIOP SW FDMA internal API
*//***************************************************************************/


#ifndef __FDMA_H_
#define __FDMA_H_


#include "general.h"


	/** Macro to save the FD FMT field - empty implementation.
	 * _frame_handle - working frame handle.
	 * _fd - the FD address in workspace. */
#define SET_FRAME_TYPE(_frame_handle, _fd)


/** \addtogroup FSL_AIOP_FDMA
 *  @{
 */

/**************************************************************************//**
@Group		FDMA_Internal_Definitions FDMA Internal Definitions

@Description	FDMA Internal Definitions.

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		FDMA_Commands_Definitions

@Description	FDMA Commands general definitions.

@{
*//***************************************************************************/
	/** FDMA Status Mask. */
#define FDMA_STATUS_MASK		0x000000FF
	/** FDMA Status Offset. */
#define FDMA_STATUS_ADDR		(HWC_ACC_OUT_ADDRESS + 0x3)
	/** FDMA CMD Offset. */
#define FDMA_CMD_OFFSET			0x3
	/** FDMA Segment Handle Offset. */
#define FDMA_SEG_HANDLE_OFFSET		0x3
	/** FDMA Frame Handle Offset. */
#define FDMA_FRAME_HANDLE_OFFSET	0x2
	/** FDMA Enqueue FQID mask */
#define FDMA_ENQUEUE_FQID_MASK		0x00FFFFFF
	/** FDMA Enqueue Queueing Destination mask */
#define FDMA_ENQUEUE_QD_MASK		0x0000FFFF
	/** FDMA Enqueue Queueing Destination priority mask */
#define FDMA_ENQUEUE_QD_PRIORITY_MASK	0x0F
	/** FDMA Enqueue Queueing Destination priority mask */
#define FDMA_ENQUEUE_QD_PRIORITY_OFFSET	24
	/** Enqueue ID selection:
	- 0: queueing destination(16bit)
	- 1: fqid (24bit). */
#define FDMA_EN_EIS_BIT			0x200
	/** FDMA Enqueue Queueing Destination distribution hash value mask */
#define FDMA_ENQUEUE_QD_HASH_MASK	0x0000FFFF
	/** Enqueue ID selection:
	- 0: queueing destination(16bit)
	- 1: fqid (24bit). */
#define FDMA_REPLIC_EIS_BIT		0x200
	/** FDMA Replicate command result frame handle offset */
#define FDMA_REPLIC_FRAME_HANDLE_OFFSET (HWC_ACC_OUT_ADDRESS2 + 0x3)
	/** FDMA Checksum offset */
#define FDMA_CHECKSUM_OFFSET		0x2
	/** FDMA SR offset */
#define FDMA_SR_OFFSET			0x8
	/** Macro to get the Segment Reference bit from the presentation
	 * context and locate it in the FDMA command correct location */
#define PRC_GET_SR_BIT_FDMA_CMD()					\
	(((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
	 sr_nds) & PRC_SR_MASK) << (FDMA_SR_OFFSET-PRC_SR_BIT_OFFSET))
	/** FDMA Store command output flags mask. */
#define FDMA_STORE_CMD_OUT_FLAGS_MASK		0x0000FF00
	/** FDMA Store command output flags offset. */
#define FDMA_STORE_CMD_OUT_ICID_OFFSET		2

/** @} end of group FDMA_Commands_Definitions */

/**************************************************************************//**
@Group		FDMA_Int_Init_ModeBits

@Description	FDMA internal Initial Presentation flags.

@{
*//***************************************************************************/

	/** No PTA Segment.
		- 0: present PTA.
		- 1: do not present PTA. */
#define FDMA_INIT_NPS_BIT		0x00000800
	/** No ASA Segment.
		- 0: present ASA.
		- 1: do not present ASA. */
#define FDMA_INIT_NAS_BIT		0x00000400

/** @} end of group FDMA_Int_Init_ModeBits */

/**************************************************************************//**
@Group		FDMA_Replace_Int_Flags

@Description	FDMA internal Replace Segment flags.

@{
*//***************************************************************************/

	/** Trim Anchor Mode. Applicable only to data segment replace commands.
	 * If set: Anchored mode. The segment's start and end boundaries remain
	 * anchored with respect to the working frame's start and end. The 
	 * subset of the open segment specified by LEADING_OFFSET and
	 * REPLACE_TARGET_SIZE is replaced with SEGMENT_SIZE bytes of the
	 * replacement data.
	 * If reset: Trimmed Mode. Trim the segment boundaries according to
	 * LEADING_OFFSET and REPLACE_TARGET_SIZE prior to performing the 
	 * replacement. */
#define FDMA_REPLACE_TAM_FLAG	0x00001000

/** @} end of group FDMA_Replace_Int_Flags */


/**************************************************************************//**
@Group		FDMA_Commands_IDs

@Description	FDMA Command IDs.

@{
*//***************************************************************************/

	/** FDMA Present segment command code */
#define FDMA_INIT_CMD			0x00000001
	/** FDMA Present segment command code */
#define FDMA_PRESENT_CMD		0x00000002
	/** FDMA Extend presentation command code */
#define FDMA_EXTEND_CMD			0x00000003
	/** FDMA Store working frame command code */
#define FDMA_STORE_CMD			0x00000010
	/** FDMA Enqueue working frame command code */
#define FDMA_ENQUEUE_WF_CMD		0x00000011
	/** FDMA Enqueue frame command code */
#define FDMA_ENQUEUE_FRAME_CMD		0x00000012
	/** FDMA Discard frame command code */
#define FDMA_DISCARD_FRAME_CMD		0x00000013
	/** FDMA Terminate task command code */
#define FDMA_TERMINATE_TASK_CMD		0x00000014
	/** FDMA Replicate working frame command code */
#define FDMA_REPLICATE_CMD		0x00000015
	/** FDMA Trim working frame segment command code */
#define FDMA_CONCAT_CMD			0x00000016
	/** FDMA Trim working frame segment command code */
#define FDMA_SPLIT_CMD			0x00000017
	/** FDMA Trim working frame segment command code */
#define FDMA_TRIM_CMD			0x00000018
	/** FDMA Replace working frame segment command code */
#define FDMA_REPLACE_CMD		0x00000019
	/** FDMA Checksum working frame command code */
#define FDMA_CKS_CMD			0x0000001A
	/** FDMA get working frame length command code */
#define FDMA_GWFL_CMD			0x0000001B
	/** FDMA Copy data command code */
#define FDMA_COPY_CMD			0x00000040
/** FDMA DMA data command code */
#define FDMA_DMA_CMD			0x00000070
	/** FDMA Acquire buffer command code */
#define FDMA_ACQUIRE_BUFFER_CMD		0x00000072
	/** FDMA Release buffer command code */
#define FDMA_RELEASE_BUFFER_CMD		0x00000073

/** @} end of group FDMA_Commands_IDs */


/**************************************************************************//**
@Group		FDMA_Commands_Args

@Description	FDMA Command Arguments macros.

@{
*//***************************************************************************/

	/** FDMA Initial presentation command arg1 */
#define FDMA_INIT_CMD_ARG1(_fd_addr, _flags)				\
	(uint32_t)((_fd_addr << 16) |					\
	(_flags & ~FDMA_INIT_BDI_BIT) | FDMA_INIT_CMD)

	/** FDMA Initial presentation command arg2 */
#define FDMA_INIT_CMD_ARG2(_seg_address, _seg_offset)			\
	(uint32_t)((_seg_address << 16) | _seg_offset)

	/** FDMA Initial presentation command arg3 */
#define FDMA_INIT_CMD_ARG3(_present_size, _ptapa_asapo)\
	(uint32_t)((_present_size << 16) | _ptapa_asapo)

	/** FDMA Initial presentation command arg4 */
#define FDMA_INIT_CMD_ARG4(_asapa_asaps)				\
	(uint32_t)(_asapa_asaps)

	/** FDMA Initial presentation explicit command arg3 */
#define FDMA_INIT_EXP_CMD_ARG3(_present_size, _pta_address, _asa_offset)\
	(uint32_t)((_present_size << 16) |				\
	((uint16_t)(uint32_t)_pta_address) | _asa_offset)

	/** FDMA Initial presentation explicit command arg4 */
#define FDMA_INIT_EXP_CMD_ARG4(_asa_address, _asa_size)			\
	(uint32_t)(((uint16_t)(uint32_t)_asa_address) | _asa_size)
	/** FDMA Initial presentation explicit AMQ command arg4 */
#define FDMA_INIT_EXP_AMQ_CMD_ARG4(_flags, _icid , _asa_address, _asa_size)\
	(uint32_t)(((uint32_t)(((_icid & ~FDMA_ICID_CONTEXT_BDI) << 16) | \
	(_flags & FDMA_INIT_BDI_BIT))) |				\
	((uint16_t)(uint32_t)_asa_address) | _asa_size)

	/** FDMA Present command arg1 */
#define FDMA_PRESENT_CMD_ARG1(_frame_handle, _flags)			\
	(uint32_t)((_frame_handle << 16) | _flags | FDMA_PRESENT_CMD)

	/** FDMA explicit Present command arg1 */
#define FDMA_PRESENT_EXP_CMD_ARG1(_frame_handle, _flags)		\
	(uint32_t)((_frame_handle << 16) | _flags | FDMA_PRESENT_CMD)


	/** FDMA Present command arg2 */
#define FDMA_PRESENT_CMD_ARG2(_ws_address, _offset)			\
	(uint32_t)((_ws_address << 16) | _offset)

	/** FDMA Present command arg3 */
#define FDMA_PRESENT_CMD_ARG3(_present_size)				\
	(uint32_t)(_present_size << 16)

	/** FDMA Extend presentation command arg1 */
#define FDMA_EXTEND_CMD_ARG1(_frame_handle, _segment_handle)		\
	(uint32_t)((_segment_handle << 24) |				\
	(_frame_handle << 16) |	FDMA_EXTEND_CMD)

	/** FDMA Extend presentation command arg1 */
#define FDMA_EXTEND_CMD_ASA_ARG1(_frame_handle)				\
	(uint32_t)((FDMA_ASA_SEG_HANDLE << 24) |			\
	(_frame_handle << 16) | FDMA_EXTEND_CMD)

	/** FDMA Extend presentation command arg2 */
#define FDMA_EXTEND_CMD_ARG2(_seg_addr, _extend_size)			\
	(uint32_t)((_seg_addr << 16) | _extend_size)

	/**< FDMA Store default working frame command arg1 */
#define FDMA_STORE_DEFAULT_CMD_ARG1(_spid, _frame_handle)		\
	(uint32_t)((_spid << 24) | (_frame_handle << 16) | FDMA_STORE_CMD)

	/**< FDMA Store working frame command arg1 */
#define FDMA_STORE_CMD_ARG1(_spid, _handle)				\
	(uint32_t)((_spid << 24) | (_handle << 16) | FDMA_STORE_CMD)

	/** FDMA Enqueue working frame command arg1 */
#define FDMA_ENQUEUE_WF_ARG1(_spid, _frame_handle, _flags)		\
	(uint32_t)((_spid << 24) | 					\
	(_frame_handle << 16) |						\
	_flags | FDMA_ENQUEUE_WF_CMD)

	/** FDMA Enqueue working frame command arg1 */
#define FDMA_ENQUEUE_WF_EXP_ARG1(_spid, _frame_handle, _flags)		\
	(uint32_t)((_spid << 24) |					\
	(_frame_handle << 16) | _flags | FDMA_ENQUEUE_WF_CMD)

	/** FDMA Enqueue working frame command arg2 */
#define FDMA_ENQUEUE_WF_QD_ARG2(_pri, _qd)				\
	(uint32_t)((_pri << FDMA_ENQUEUE_QD_PRIORITY_OFFSET) | _qd)

	/** FDMA Enqueue working frame command arg3 */
#define FDMA_ENQUEUE_WF_QD_ARG3(_hash_value)				\
	(uint32_t)(_hash_value)

	/** FDMA Enqueue frame command arg1 */
#define FDMA_ENQUEUE_FRAME_ARG1(_flags)					\
	(uint32_t)(((HWC_FD_ADDRESS << 16) |				\
	(_flags & ~FDMA_ENF_BDI_BIT) | FDMA_ENQUEUE_FRAME_CMD))

	/** FDMA Enqueue frame explicit command arg1 */
#define FDMA_ENQUEUE_FRAME_EXP_ARG1(_flags, _fd)			\
	(uint32_t)((((uint32_t)_fd << 16) |				\
	(_flags & ~FDMA_ENF_BDI_BIT) | FDMA_ENQUEUE_FRAME_CMD))

	/** FDMA Enqueue frame command arg3 */
#define FDMA_ENQUEUE_FRAME_ARG3(_flags, _icid)				\
	(uint32_t)(((_icid << 16) & ~FDMA_ENF_BDI_BIT) |		\
	(_flags & FDMA_ENF_BDI_BIT))

	/** FDMA Enqueue frame command arg3 */
#define FDMA_ENQUEUE_FRAME_QD_ARG3(_flags, _icid, _hash)		\
	(uint32_t)(((_icid << 16) & ~FDMA_ENF_BDI_BIT) |		\
	(_flags & FDMA_ENF_BDI_BIT) | _hash)

	/** FDMA Discard working frame command arg1 */
#define FDMA_DISCARD_ARG1_WF(_frame_handle, _flags)			\
	(uint32_t)((_frame_handle << 16) | _flags | FDMA_DISCARD_FRAME_CMD)

	/** FDMA Discard frame command arg1 */
#define FDMA_DISCARD_ARG1_FRAME(_frame, _flags)				\
	(uint32_t)((_frame << 16) | 					\
	(_flags & ~FDMA_DIS_BDI_BIT) | FDMA_DISCARD_FRAME_CMD)
	/** FDMA Discard frame command arg3 */
#define FDMA_DISCARD_ARG3_FRAME(_flags, _icid)				\
	(uint32_t)(((_icid << 16) & ~FDMA_DIS_BDI_BIT) |		\
	(_flags & FDMA_DIS_BDI_BIT))

	/** FDMA Terminate task command arg1 */
#define FDMA_TERM_TASK_CMD_ARG1()					\
	(uint32_t)(FDMA_TERMINATE_TASK_CMD)

	/** FDMA Replicate frames command command arg1 */
#define FDMA_REPLIC_CMD_ARG1(_spid, _handle, _flags)			\
	(uint32_t)((_spid << 24) | (_handle << 16) | _flags |		\
			FDMA_REPLICATE_CMD)
	/** FDMA Replicate frames by fqid command command arg3 */
#define FDMA_REPLIC_CMD_ARG3_FQID(_fd_addr)				\
	(uint32_t)((_fd_addr << 16))
	/** FDMA Replicate frames by qd command command arg2 */
#define FDMA_REPLIC_CMD_ARG2_QD(_pri, _qd)				\
	(uint32_t)((_pri << FDMA_ENQUEUE_QD_PRIORITY_OFFSET) | _qd)
	/** FDMA Replicate frames by qd command command arg3 */
#define FDMA_REPLIC_CMD_ARG3_QD(_fd_addr, _hash)			\
	(uint32_t)((_fd_addr << 16) | _hash)

	/** FDMA Concatenate frames command command arg1 */
#define FDMA_CONCAT_CMD_ARG1(_spid, _handle, _flags)			\
	(uint32_t)((_spid << 24) | (_handle << 16) | _flags |		\
	FDMA_CONCAT_CMD)

	/** FDMA Concatenate frames command command arg2 */
#define FDMA_CONCAT_CMD_ARG2(_frame2, _frame1)				\
	(uint32_t)((_frame2 << 16) | _frame1)

	/** FDMA Split frame command command arg1 */
#define FDMA_SPLIT_CMD_ARG1(_spid, _handle, _flags)			\
	(uint32_t)((_spid << 24) | (_handle << 16) | _flags | FDMA_SPLIT_CMD)

	/** FDMA Split frame command command arg2 */
#define FDMA_SPLIT_CMD_ARG2(_seg_address, _seg_offset)			\
	(uint32_t)((_seg_address << 16) | _seg_offset)

	/** FDMA Split frame command command arg3 */
#define FDMA_SPLIT_CMD_ARG3(_present_size)				\
	(uint32_t)(_present_size << 16)

	/** FDMA Split frame command command arg4 */
#define FDMA_SPLIT_CMD_ARG4(_fd_addr, _split_size_sf)			\
	(uint32_t)((_fd_addr << 16) | _split_size_sf)

	/** FDMA Trim working frame segment command arg1 */
#define FDMA_TRIM_CMD_ARG1(_frame_handle, _seg_handle)			\
	(uint32_t)((_seg_handle << 24) | (_frame_handle << 16) | FDMA_TRIM_CMD)

	/** FDMA Trim working frame segment command arg2 */
#define FDMA_TRIM_CMD_ARG2(_offset, _size)				\
	(uint32_t)((_offset << 16) | _size)

	/** FDMA Replace working frame segment command arg1 */
#define FDMA_REPLACE_CMD_ARG1(_frame_handle, _seg_handle, _flags)	\
	(uint32_t)((_seg_handle << 24) | (_frame_handle << 16) |	\
	(_flags) | FDMA_REPLACE_CMD)

/** FDMA Replace working frame segment command arg1 */
#define FDMA_MODIFY_CMD_ARG1(_frame_handle, _seg_handle, _flags)	\
	(uint32_t)((_seg_handle << 24) | (_frame_handle << 16) |	\
	(_flags) | FDMA_REPLACE_CMD)

/** FDMA explicit Replace working frame segment command arg1 */
#define FDMA_REPLACE_EXP_CMD_ARG1(_seg_handle, _frame_handle, _flags)	\
	(uint32_t)((_seg_handle << 24) | (_frame_handle << 16) |	\
	(_flags) | FDMA_REPLACE_CMD)

/** FDMA Delete segment command arg1 */
#define FDMA_DELETE_CMD_ARG1(_seg_handle, _frame_handle, _flags)	\
	(uint32_t)((_seg_handle << 24) |		\
	(_frame_handle << 16) |				\
	(_flags) | FDMA_REPLACE_CMD)

	/** FDMA Replace working frame segment command arg2 */
#define FDMA_REPLACE_CMD_ARG2(_offset, _replace_target_size)		\
	(uint32_t)((_offset << 16) | _replace_target_size)

	/** FDMA Replace working frame segment command arg3 */
#define FDMA_REPLACE_CMD_ARG3(_ws_address, _size)			\
	(uint32_t)((((uint32_t)_ws_address) << 16) | _size)

	/** FDMA Replace working frame segment command arg4 */
#define FDMA_REPLACE_CMD_ARG4(_ws_address, _size)			\
	(uint32_t)((((uint32_t)_ws_address) << 16) | _size)

	/** FDMA Close working frame segment command arg1 */
#define FDMA_CLOSE_SEG_CMD_ARG1(_frame_handle, _seg_handle)		\
	(uint32_t)((_seg_handle << 24) | (_frame_handle << 16) |	\
	(FDMA_REPLACE_SA_CLOSE_BIT) | FDMA_REPLACE_CMD)

	/** FDMA Replace working frame segment command arg1 */
#define FDMA_REPLACE_PTA_ASA_CMD_ARG1(_seg_handle, _frame_handle, _flags)\
	(uint32_t)((_seg_handle << 24) |				\
	(_frame_handle << 16) | (_flags) | FDMA_REPLACE_CMD)

	/** FDMA Checksum working frame command arg1 */
#define FDMA_CKS_CMD_ARG1(_frame_handle)				\
	(uint32_t)((((uint32_t)_frame_handle) << 16) | FDMA_CKS_CMD)
	/**< FDMA Checksum  working frame segment command arg2 */
#define FDMA_CKS_CMD_ARG2(_offset, _size)				\
	(uint32_t)((_offset << 16) | _size)

#define FDMA_GET_FRAME_LENGTH_CMD_ARG1(_frame_handle)			\
	(uint32_t)((((uint32_t)_frame_handle) << 16) | FDMA_GWFL_CMD)

	/** FDMA Copy data command arg1 */
#define FDMA_COPY_CMD_ARG1(_copy_size, _flags)				\
	(uint32_t)((((uint32_t)_copy_size) << 16) | _flags | FDMA_COPY_CMD)

	/** FDMA DMA data command arg1 */
#define FDMA_DMA_CMD_ARG1(_icid, _flags)				\
	(uint32_t)(((((uint32_t)_icid) & 0x7FFF) << 16) | _flags | FDMA_DMA_CMD)
/** FDMA DMA data command arg2 */
#define FDMA_DMA_CMD_ARG2(_copy_size, _loc_address)			\
	(uint32_t)(((((uint32_t)_copy_size) & 0xFFF) << 20) |		\
		((uint32_t)_loc_address & 0xFFFFF))

	/** FDMA Acquire buffer command arg1 */
#define FDMA_ACQUIRE_CMD_ARG1(_icid, _flags)				\
	(uint32_t)((((uint32_t)_icid) << 16) | _flags | FDMA_ACQUIRE_BUFFER_CMD)
	/** FDMA Acquire buffer command arg2 */
#define FDMA_ACQUIRE_CMD_ARG2(_dst, _bpid)				\
	(uint32_t)((((uint32_t)_dst) << 16) | _bpid)

	/** FDMA Release buffer command arg1 */
#define FDMA_RELEASE_CMD_ARG1(_icid, _flags)				\
	(uint32_t)((((uint32_t)_icid) << 16) | _flags | FDMA_RELEASE_BUFFER_CMD)

/** @} end of group FDMA_Commands_Args */

/** @}*/ /* end of group FDMA_Internal_Definitions */

/** @}*/ /* end of group FSL_AIOP_FDMA */


/** \addtogroup AIOP_PRC_Definitions
 *  @{
 */

/**************************************************************************//**
 @Group		AIOP_PRC_Setters

 @Description	Presentation Context (ADC) Setters

 @{
*//***************************************************************************/
	/** Macro to set the task entry point parameter value from the
	 * presentation context */
#define PRC_SET_PARAMETER(_val)						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->param =	\
		(uint32_t)_val)
	/** Macro to set the default segment address from the presentation
	 * context */
#define PRC_SET_SEGMENT_ADDRESS(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->seg_address =\
		(uint16_t)_val)
	/** Macro to set the default segment length from the presentation
	 * context */
#define PRC_SET_SEGMENT_LENGTH(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->seg_length = \
		(uint16_t)_val)
	/** Macro to set the default segment offset from the presentation
	 * context */
#define PRC_SET_SEGMENT_OFFSET(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->seg_offset = \
		(uint16_t)_val)
	/** Macro to set Segment reference bit in workspace from the
	 * presentation context */
#define PRC_SET_SR_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->sr_nds =\
		((((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		sr_nds & ~PRC_SR_MASK) | PRC_SR_MASK))
	/** Macro to reset Segment reference bit in workspace from the
	 * presentation context */
#define PRC_RESET_SR_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->sr_nds =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		sr_nds & ~PRC_SR_MASK))
	/** Macro to set No-Data-Segment bit in workspace from the
	 * presentation context */
#define PRC_SET_NDS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->sr_nds =\
		((((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		sr_nds & ~PRC_NDS_MASK) | PRC_NDS_MASK))
	/** Macro to reset No-Data-Segment bit in workspace from the
	 * presentation context */
#define PRC_RESET_NDS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->sr_nds =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		sr_nds & ~PRC_NDS_MASK))
	/** Macro to set the default frame handle from the presentation
	 * context */
#define PRC_SET_FRAME_HANDLE(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->frame_handle = _val)
	/** Macro to set the default segment handle from the presentation
	 * context */
#define PRC_SET_SEGMENT_HANDLE(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->seg_handle = _val)
	/** Macro to set the OSM Entry Point Source value from the presentation
	 * context */
#define PRC_SET_OSM_SOURCE_VALUE(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->osrc_oep_osel_osrm =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		osrc_oep_osel_osrm & ~PRC_OSRC_MASK) |			\
		(((uint8_t)_val << PRC_OSRC_BIT_OFFSET) & PRC_OSRC_MASK))
	/** Macro to set the OSM Entry Point Execution Phase value from the
	 * presentation context */
#define PRC_SET_OSM_EXECUTION_PHASE_VALUE(_val)				\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->osrc_oep_osel_osrm =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		osrc_oep_osel_osrm & ~PRC_OEP_MASK) |			\
		(((uint8_t)_val << PRC_OEP_BIT_OFFSET) & PRC_OEP_MASK))
	/** Macro to set the OSM Entry Point Select value from the
	 * presentation context */
#define PRC_SET_OSM_SELECT_VALUE(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->osrc_oep_osel_osrm =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		osrc_oep_osel_osrm & ~PRC_OSEL_MASK) |			\
		(((uint8_t)_val << PRC_OSEL_BIT_OFFSET) & PRC_OSEL_MASK))
	/** Macro to set the OSM Entry Point Order Scope Range Mask value from
	 * the presentation context */
#define PRC_SET_OSM_ORDER_SCOPE_RANGE_MASK_VALUE(_val)			\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->osrc_oep_osel_osrm =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		osrc_oep_osel_osrm & ~PRC_OSRM_MASK) |			\
		((uint8_t)_val & PRC_OSRM_MASK))
	/** Macro to set Initial Scope Value bit in workspace from the
	 * presentation context */
#define PRC_SET_ISV_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->isv =\
		((((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		isv & ~PRC_ISV_MASK) | PRC_ISV_MASK))
	/** Macro to reset Initial Scope Value bit in workspace from the
	 * presentation context */
#define PRC_RESET_ISV_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->isv =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		isv & ~PRC_ISV_MASK))
	/** Dummy Macro for IPSEC */
#define PRC_SET_ASA_SIZE(_val)

/** @} */ /* end of AIOP_PRC_Setters */

/** @}*/ /* end of group AIOP_PRC_Definitions */

/** \addtogroup FDMA_Commands_Flags
 *  @{
 */

/**************************************************************************//**
@Group		FDMA_CONCAT_FLAGS

@Description	FDMA Concatenate flags

@{
*//***************************************************************************/
	/** FD1 PL+VA flags mask.
	 * Relevant only if \ref FDMA_CONCAT_FS1_BIT is set */
#define FDMA_CONCAT_FS1_PL_VA_MASK   (FDMA_CONCAT_AMQ_PL1 | FDMA_CONCAT_AMQ_VA1) 
	/** FD2 PL+VA flags mask.
	 * Relevant only if \ref FDMA_CONCAT_FS2_BIT is set*/
#define FDMA_CONCAT_FS2_PL_VA_MASK (FDMA_CONCAT_AMQ_PL2 | FDMA_CONCAT_AMQ_VA2)
	/** FD1 BDI flag.
	 * Relevant only if 
	 * \ref FDMA_CONCAT_FS1_BIT or \ref FDMA_CONCAT_FS2_BIT are set */
#define FDMA_CONCAT_BDI_BIT		0x00008000


/** @} end of group FDMA_DISCARD_FLAGS */

/**************************************************************************//**
@Group		FDMA_DISCARD_FLAGS

@Description	FDMA Discard flags

@{
*//***************************************************************************/
/** Frame Source: Discard working frame (using frame handle).*/
#define FDMA_DIS_FS_HANDLE_BIT	0x0000
	/** Frame Source: Discard Frame (using frame FD).*/
#define FDMA_DIS_FS_FD_BIT	0x0200

/** @} end of group FDMA_DISCARD_FLAGS */

/**************************************************************************//**
@Group		FDMA_DMA_Flags

@Description	FDMA DMA data flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_DMA_NO_FLAGS	0x00000000
	/** DMA Data Access options. */
#define FDMA_DMA_DA		fdma_dma_data_access_options
	/** Effective Virtual Address. Frame AMQ attribute.
	 * The DMA uses this memory attribute to make the access. */
#define FDMA_DMA_eVA_BIT		0x00002000
	/** Bypass the Memory Translation. Frame AMQ attribute.
	 * The DMA uses this memory attribute to make the access. */
#define FDMA_DMA_BMT_BIT	0x00004000
	/** Privilege Level. Frame AMQ attribute.
	 * The DMA uses this memory attribute to make the access. */
#define FDMA_DMA_PL_BIT		0x00008000

/** @} end of group FDMA_DMA_Flags */

/** @} end of group FDMA_Commands_Flags */

/** \addtogroup FDMA_Enumerations
 *  @{
 */

/**************************************************************************//**
 @enum fdma_hw_errors

 @Description	AIOP FDMA Hardware Error codes.

 @{
*//***************************************************************************/
enum fdma_hw_errors {
		/** Success. */
	FDMA_SUCCESS = 0,
		/** Enqueue failed due to QMan enqueue rejection. */
	FDMA_ENQUEUE_FAILED_ERR = 0x1,
		/** Failed due to buffer pool depletion. */
	FDMA_BUFFER_POOL_DEPLETION_ERR = 0x2,
		/** Unable to split frame, split size is larger than frame
		 * size or no SF bit found. */
	FDMA_UNABLE_TO_SPLIT_ERR = 0x4,
		/** Unable to trim frame to concatenate, trim size is larger
		 * than frame size. */
	FDMA_UNABLE_TO_TRIM_ERR = 0x6,
		/** Frame Store failed, single buffer frame full and Storage
		 * Profile FF is set to 10. */
	FDMA_FRAME_STORE_ERR = 0x7,
		/** Unable to fulfill specified segment presentation size. */
	FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR = 0x8,
		/** ASAPO value beyond ASAL in received FD. No ASA presentation
		 * possible. */
	FDMA_ASA_OFFSET_BEYOND_ASA_LENGTH_ERR = 0x9,
		/** Unable to fulfill specified ASAPS on an initial frame
		 * presentation. Only ASAL amount of annotation was presented.*/
	FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR = 0xA,
		/** Unable to present the PTA segment because no PTA segment is
		 * present in the working frame.*/
	FDMA_UNABLE_TO_PRESENT_PTA_ERR = 0xB,
		/** Unable to perform required processing due to received
		 * FD[FMT]=0x3 (the reserved value). */
	FDMA_UNABLE_TO_EXECUTE_DUE_TO_RESERVED_FMT_ERR = 0xD,
		/** Received non-zero FD[ERR] field from Work Scheduler. */
	FDMA_FD_ERR = 0xE,
		/** Frame Handle depletion (max of 6). */
	FDMA_FRAME_HANDLE_DEPLETION_ERR = 0x80,
		/** Invalid Frame Handle. */
	FDMA_INVALID_FRAME_HANDLE_ERR = 0x81,
		/** Segment Handle depletion (max of 8). */
	FDMA_SEGMENT_HANDLE_DEPLETION_ERR = 0x82,
		/** Invalid Segment Handle. */
	FDMA_INVALID_SEGMENT_HANDLE_ERR = 0x83,
		/** Invalid DMA command arguments. */
	FDMA_INVALID_DMA_COMMAND_ARGS_ERR = 0x86,
		/** Invalid DMA command. */
	FDMA_INVALID_DMA_COMMAND_ERR = 0x87,
		/** Internal memory ECC uncorrected ECC error. */
	FDMA_INTERNAL_MEMORY_ECC_ERR = 0xA0,
		/** Workspace memory read Error. */
	FDMA_WORKSPACE_MEMORY_READ_ERR = 0xA1,
		/** Workspace memory write Error. */
	FDMA_WORKSPACE_MEMORY_WRITE_ERR = 0xA2,
		/** System memory read error (permission or ECC). */
	FDMA_SYSTEM_MEMORY_READ_ERR = 0xA3,
		/** System memory write error (permission or ECC). */
	FDMA_SYSTEM_MEMORY_WRITE_ERR = 0xA4,
		/** QMan enqueue error (access violation). */
	FDMA_QMAN_ENQUEUE_ERR = 0xA5,
		/** Frame structural error (invalid S/G bits settings, hop
		 * limit). */
	FDMA_FRAME_STRUCTURAL_ERR = 0xA6,
		/** FDMA Internal error, SRU depletion. */
	FDMA_INTERNAL_ERR = 0xA7,
		/** Storage Profile ICID does not match frame ICID and Storage
		 * Profile BS=1 error. */
	FDMA_SPID_ICID_ERR = 0xA8,
		/** Shared SRAM memory read Error. */
	FDMA_SRAM_MEMORY_READ_ERR = 0xA9,
		/** Profile SRAM memory read Error. */
	FDMA_PROFILE_SRAM_MEMORY_READ_ERR = 0xAA,
		/** Frames to concatenate ICIDs does not match error. */
	FDMA_CONCATENATE_ICID_NOT_MATCH_ERR = 0xAB,
		/** Shared SRAM memory write Error. */
	FDMA_SHARED_SRAM_MEMORY_WRITE_ERROR = 0xAC

};

/* @} end of enum fdma_hw_errors */

/**************************************************************************//**
 @enum fdma_sw_errors

 @Description	AIOP FDMA Software Error codes.

 @{
*//***************************************************************************/
enum fdma_sw_errors {
		/** The segment handle does not represent a Data segment. */
	/*FDMA_NO_DATA_SEGMENT_HANDLE = 0x80000070,*/
		/** Invalid PTA address (\ref PRC_PTA_NOT_LOADED_ADDRESS). */
	FDMA_INVALID_PTA_ADDRESS = 0x71
};

/* @} end of enum fdma_sw_errors */

/**************************************************************************//**
 @enum fdma_functions

 @Description	AIOP FDMA Functions enumertion.

 @{
*//***************************************************************************/
enum fdma_function_identifier {
	FDMA_PRESENT_DEFAULT_FRAME = 0,
	FDMA_PRESENT_FRAME,
	FDMA_PRESENT_DEFAULT_FRAME_WITHOUT_SEGMENTS,
	FDMA_PRESENT_FRAME_WITHOUT_SEGMENTS,
	FDMA_PRESENT_DEFAULT_FRAME_SEGMENT,
	FDMA_PRESENT_DEFAULT_FRAME_DEFAULT_SEGMENT,
	FDMA_PRESENT_FRAME_SEGMENT,
	FDMA_READ_DEFAULT_FRAME_ASA,
	FDMA_READ_DEFAULT_FRAME_PTA,
	FDMA_EXTEND_DEFAULT_SEGMENT_PRESENTATION,
	FDMA_STORE_DEFAULT_FRAME_DATA,
	FDMA_STORE_FRAME_DATA,
	FDMA_STORE_AND_ENQUEUE_DEFAULT_FRAME_FQID,
	FDMA_STORE_AND_ENQUEUE_FRAME_FQID,
	FDMA_STORE_AND_ENQUEUE_DEFAULT_FRAME_QD,
	FDMA_STORE_AND_ENQUEUE_FRAME_QD,
	FDMA_ENQUEUE_DEFAULT_FD_FQID,
	FDMA_ENQUEUE_FD_FQID,
	FDMA_ENQUEUE_DEFAULT_FD_QD,
	FDMA_ENQUEUE_FD_QD,
	FDMA_DISCARD_DEFAULT_FRAME,
	FDMA_DISCARD_FRAME,
	FDMA_DISCARD_FD,
	FDMA_FORCE_DISCARD_FD,
	FDMA_TERMINATE_TASK,
	FDMA_REPLICATE_FRAME_FQID,
	FDMA_REPLICATE_FRAME_QD,
	FDMA_CONCATENATE_FRAMES,
	FDMA_SPLIT_FRAME,
	FDMA_TRIM_DEFAULT_SEGMENT_PRESENTATION,
	FDMA_MODIFY_DEFAULT_SEGMENT_DATA,
	FDMA_MODIFY_DEFAULT_SEGMENT_FULL_DATA,
	FDMA_MODIFY_SEGMENT_DATA,
	FDMA_REPLACE_DEFAULT_SEGMENT_DATA,
	FDMA_INSERT_DEFAULT_SEGMENT_DATA,
	FDMA_INSERT_SEGMENT_DATA,
	FDMA_DELETE_DEFAULT_SEGMENT_DATA,
	FDMA_DELETE_SEGMENT_DATA,
	FDMA_CLOSE_DEFAULT_SEGMENT,
	FDMA_CLOSE_SEGMENT,
	FDMA_REPLACE_DEFAULT_ASA_SEGMENT_DATA,
	FDMA_REPLACE_DEFAULT_PTA_SEGMENT_DATA,
	FDMA_CALCULATE_DEFAULT_FRAME_CHECKSUM,
	FDMA_GET_FRAME_LENGTH,
	FDMA_COPY_DATA,
	FDMA_DMA_DATA,
	FDMA_ACQUIRE_BUFFER,
	FDMA_RELEASE_BUFFER
};

/** @}*/ /* end of group FDMA_Enumerations */


/**************************************************************************//**
@Function	fdma_present_default_frame_default_segment

@Description	Open a segment of the default working frame and copy the
		segment data into the specified location in the workspace.

		Implicit input parameters in Task Defaults: frame handle,
		segment length, segment address, segment offset.

		Implicitly updated values in Task Defaults: segment length,
		segment handle.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (not relevant if the 
		present_size in the function parameters is 0).
		This error is caused since the requested presentation exceeded
		frame data end.

@Cautions	In case the presented segment will be used by 
		PARSER/CTLU/KEYGEN, it should be presented in a 16 byte aligned 
		workspace address (due to HW limitations).
@Cautions	This command may be invoked only for Data segments.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_present_default_frame_default_segment();

/**************************************************************************//**
@Function	fdma_dma_data

@Description	Provide direct access to any system memory data. Transfer system
		memory data to/from the task workspace/AIOP shared memory.

@Param[in]	copy_size - Number of bytes to copy (limited to 12 bits).
@Param[in]	icid - Memory Access ICID. The DMA uses the provided Isolation
		Context to make the access.
@Param[in]	loc_addr - A pointer to the source/target location in Workspace
		or AIOP Shared Memory for DMA data. Workspace address is
		limited to 16 bits. AIOP Shared Memory address is limited to 20
		bits.
@Param[in]	sys_addr - System memory source/target address for DMA data.
@Param[in]	flags - Please refer to \link FDMA_DMA_Flags DMA command flags
		\endlink.

@Return		None.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_dma_data(
		uint16_t copy_size,
		uint16_t icid,
		void *loc_addr,
		uint64_t sys_addr,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_acquire_buffer

@Description	Provides direct access to the BMan in order to acquire a BMan
		buffer in a software managed way.

@Param[in]	icid - Buffer Pool ICID.
@Param[in]	flags - Please refer to
		\link FDMA_ACQUIRE_BUFFER_Flags command flags \endlink.
@Param[in]	bpid - Buffer pool ID used for the Acquire Buffer.
@Param[out]	dst - A pointer to the location in the workspace where to return
		the acquired 64 bit buffer address.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@Cautions	This command is not intended to be used in a normal datapath,
		but more of a get out of jail card where access to BMan buffers
		is required when operating on a frame while not using the
		provided FDMA working frame commands.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_acquire_buffer(
		uint16_t icid,
		uint32_t flags,
		uint16_t bpid,
		void *dst);

/**************************************************************************//**
@Function	fdma_release_buffer

@Description	Provides direct access to the BMan in order to release a BMan
		buffer in a software managed way.

@Param[in]	icid - Buffer Pool ICID.
@Param[in]	flags - Please refer to
		\link FDMA_RELEASE_BUFFER_Flags command flags \endlink.
@Param[in]	bpid - Buffer pool ID used for the Release Buffer.
@Param[out]	addr - Buffer address to be released.

@Return		None.

@Cautions	This command is not intended to be used in a normal datapath,
		but more of a get out of jail card where access to BMan buffers
		is required when operating on a frame while not using the
		provided FDMA working frame commands.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_release_buffer(
		uint16_t icid,
		uint32_t flags,
		uint16_t bpid,
		uint64_t addr);

/**************************************************************************//**
@Function	fdma_discard_fd_wrp

@Description	Wrapper to the function fdma_discard_fd.
		See description of the function fdma_discard_fd.

@Param[in]	fd - A pointer to the location in the workspace of the FD to be
		discarded \ref ldpaa_fd.
@Param[in]	icid - ICID of the FD to discard.
@Param[in]	flags - \link FDMA_Discard_WF_Flags discard working frame
		frame flags. \endlink

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EIO - Received frame with non-zero FD[err] field.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_discard_fd_wrp(struct ldpaa_fd *fd, uint16_t icid, uint32_t flags);

/**************************************************************************//**
@Function	fdma_calculate_default_frame_checksum_wrp

@Description	Wrapper of function fdma_calculate_default_frame_checksum.
		See description of function
		fdma_calculate_default_frame_checksum.

@Param[in]	offset - Number of bytes offset in the frame from which to start
		calculation of checksum.
@Param[in]	size - Number of bytes to do calculation of checksum.
		Use 0xffff to calculate checksum until the end of the frame.
@Param[out]	checksum - Ones complement sum over the specified range of the
		working frame.

@Return		None.

@Cautions	The h/w must have previously opened the frame with an
		initial presentation or initial presentation command.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_calculate_default_frame_checksum_wrp(
		uint16_t offset,
		uint16_t size,
		uint16_t *checksum);

/**************************************************************************//**
@Function	fdma_store_default_frame_data_wrp

@Description	Wrapper to function fdma_store_default_frame_data.
		See description of the function
		fdma_store_default_frame_data_wrp.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@remark		FD is updated.
@remark		If some segments of the Working Frame are not closed, they will
		be closed and the segment handles will be implicitly released.
@remark		Release frame handle is implicit in this function.

@Cautions	All modified segments (which are to be stored) must be replaced
		(by a replace command) before storing a frame.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_store_default_frame_data_wrp(void);


/**************************************************************************//**
@Function	fdma_exception_handler

@Description	Handler for the error status returned from the FDMA API
		functions.

@Param[in]	file_path - The path of the file in which the error occurred.
@Param[in]	func_id - The function in which the error occurred.
@Param[in]	line - The line in which the error occurred.
@Param[in]	status - Status to be handled be this function.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void fdma_exception_handler(enum fdma_function_identifier func_id,
			     uint32_t line,
			     int32_t status);


#endif /* __FDMA_H_ */
