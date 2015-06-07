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
@File		fsl_fdma.h

@Description	This file contains the AIOP SW FDMA API

*//***************************************************************************/

#ifndef __FSL_FDMA_H
#define __FSL_FDMA_H

#include "types.h"
#include "fsl_errors.h"
#include "fsl_ldpaa.h"


/* extern uint8_t HWC_PRC_ADDR[]; */


/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		FSL_AIOP_FDMA FDMA

 @Description	FSL AIOP FDMA macros and functions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		FDMA_General_Definitions FDMA General Definitions

 @Description	FDMA General Definitions

 @{
*//***************************************************************************/
	/** SW annotation segment handle */
#define FDMA_PTA_SEG_HANDLE	0xF
	/** HW annotation segment handle */
#define FDMA_ASA_SEG_HANDLE	0xE
	/** Default Segment headroom size. */
#define DEFAULT_SEGMENT_HEADROOM_SIZE	128
	/** Default Segment size. */
#define DEFAULT_SEGMENT_SIZE		256

/** @} end of group FDMA_General_Definitions */


/**************************************************************************//**
 @Group		FDMA_Enumerations FDMA Enumerations

 @Description	FDMA Enumerations

 @{
*//***************************************************************************/

/**************************************************************************//**
 @enum fdma_st_options

 @Description	AIOP FDMA segment type options.

 @{
*//***************************************************************************/
enum fdma_st_options {
		/** Data Segment. */
	FDMA_ST_DATA_SEGMENT_BIT =	0x00000000,
		/** PTA Segment. */
	FDMA_ST_PTA_SEGMENT_BIT =	0x00000400,
		/** ASA Segment. */
	FDMA_ST_ASA_SEGMENT_BIT =	0x00000800
};

/* @} end of enum fdma_st_options */

/**************************************************************************//**
 @enum fdma_cfa_options

 @Description	AIOP FDMA copy frame annotations options.

 @{
*//***************************************************************************/
enum fdma_cfa_options {
		/** Do not copy annotations. */
	FDMA_CFA_NO_COPY_BIT =	0x00000000,
		/** Copy ASA. */
	FDMA_CFA_ASA_COPY_BIT =	0x00002000,
		/** Copy PTA. */
	FDMA_CFA_PTA_COPY_BIT =	0x00004000,
		/** Copy ASA + PTA. */
	FDMA_CFA_COPY_BIT =	0x00006000
};

/* @} end of enum fdma_cfa_options */

/**************************************************************************//**
 @enum fdma_split_psa_options

 @Description	AIOP FDMA Split command Post split action.

 @{
*//***************************************************************************/
enum fdma_split_psa_options {
		/** Do not present segment from the split frame,
		 * keep split working frame open. */
	FDMA_SPLIT_PSA_NO_PRESENT_BIT =	0x00000000,
		/** Present segment from the split frame,
		 * keep split working frame open. */
	FDMA_SPLIT_PSA_PRESENT_BIT =	0x00000400
#ifdef REV2
		/** Do not present, store and close split working frame. */
	FDMA_SPLIT_PSA_CLOSE_FRAME_BIT = 0x00000800
#endif /* REV2 */
};

/* @} end of enum fdma_split_psa_options */

/**************************************************************************//**
 @enum fdma_enqueue_tc_options

 @Description	AIOP FDMA Enqueue Terminate Control options.

 @{
*//***************************************************************************/
enum fdma_enqueue_tc_options {
		/** Return after enqueue	*/
	FDMA_EN_TC_RET_BITS =	0x0000,
		/** Terminate: this command will trigger the Terminate task
		 * command right after the enqueue. If the enqueue failed, the
		 * frame will be discarded.	*/
	FDMA_EN_TC_TERM_BITS = 0x0400
#ifdef REV2
		/** Conditional Terminate : trigger the Terminate task command
		 * only if the enqueue succeeded. If the enqueue failed, the
		 * frame handle is not released and the command returns with an
		 * error code.	*/
	FDMA_EN_TC_CONDTERM_BITS =	0x0800
#endif /* REV2 */
};

/* @} end of enum fdma_enqueue_tc_options */

/**************************************************************************//**
 @enum fdma_replace_sa_options

 @Description	AIOP FDMA Replace Segment Action options.

		When this command is invoked on annotation segments (PTA or ASA)
		they always remain open.
 @{
*//***************************************************************************/
enum fdma_replace_sa_options {
#ifdef REV2
		/** Keep the segment open */
	FDMA_REPLACE_SA_OPEN_BIT =	0x0000,
#endif /* REV2 */
		/** Re-present the segment in workspace */
	FDMA_REPLACE_SA_REPRESENT_BIT =	0x0100,
		/** Close the replaced segment to free the workspace memory
		 * associated with the segment.
		 * This option is not relevant for replacing ASA/PTA. If it as
		 * chosen for replacing ASA/PTA it will be ignored. */
	FDMA_REPLACE_SA_CLOSE_BIT =	0x0200
};

/* @} end of enum fdma_replace_sa_options */

/**************************************************************************//**
 @enum fdma_dma_data_access_options

 @Description	AIOP FDMA DMA Data Access command options.

 @{
*//***************************************************************************/
enum fdma_dma_data_access_options {
		/** system memory read to workspace write */
	FDMA_DMA_DA_SYS_TO_WS_BIT =	0x00000000,
		/** workspace read to system memory write */
	FDMA_DMA_DA_WS_TO_SYS_BIT =	0x00000100,
		/** system memory read to AIOP Shared Memory write */
	FDMA_DMA_DA_SYS_TO_SRAM_BIT =	0x00000200,
		/** AIOP Shared Memory read to system memory write */
	FDMA_DMA_DA_SRAM_TO_SYS_BIT =	0x00000300
};

/* @} end of enum fdma_dma_data_access_options */

/**************************************************************************//**
 @enum fdma_pta_size_type

 @Description	PTA size type.

 @{
*//***************************************************************************/
enum fdma_pta_size_type {
		/** The frame has no PTA	*/
	PTA_SIZE_NO_PTA = 0x0,
		/** Only the first 32B of the PTA are valid and were
		 * presented. */
	PTA_SIZE_PTV1 = 0x1,
		/** Only the second 32B of the PTA are valid and were
		 * presented. */
	PTA_SIZE_PTV2 = 0x2,
		/** 64B of the PTA are valid and were presented. */
	PTA_SIZE_PTV1_2 = 0x3
};

/* @} end of enum fdma_pta_size_type */

/** @} end of group FDMA_Enumerations */


/**************************************************************************//**
 @Group		FDMA_Commands_Flags FDMA Commands Flags

 @Description	FDMA Commands Flags

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		FDMA_Present_Frame_Flags  FDMA Present Frame Flags

@Description	FDMA Frame Presentation flags

@{
*//***************************************************************************/
	/** Default command configuration. */
#define FDMA_INIT_NO_FLAGS	0x00000000
	/** No Data Segment.
	 * If set - do not present Data segment.
	 * Otherwise - present Data segment. */
#define FDMA_INIT_NDS_BIT	0x00000200
	/** Reference within the frame to present from.
	 * If set - end of the frame.
	 * Otherwise - start of the frame. */
#define FDMA_INIT_SR_BIT	0x00000100
	/** AMQ attributes (PL, VA, BDI, ICID) Source.
	 * If set - supplied AMQ attributes are used.
	 * If reset - task default AMQ attributes (From Additional Dequeue
	 * Context) are used. */
#define FDMA_INIT_AS_BIT	0x00001000
	/** Virtual Address. Frame AMQ attribute.
	 * Used only in case \ref FDMA_INIT_AS_BIT is set. */
#define FDMA_INIT_VA_BIT	0x00004000
	/** Privilege Level. Frame AMQ attribute.
	 * Used only in case \ref FDMA_INIT_AS_BIT is set. */
#define FDMA_INIT_PL_BIT	0x00008000
	/** Bypass Datapath Isolation. Frame AMQ attribute.
	 * Used only in case \ref FDMA_INIT_AS_BIT is set. */
#define FDMA_INIT_BDI_BIT	0x80000000

/** @} end of group FDMA_Present_Frame_Flags */

/**************************************************************************//**
@Group		FDMA_PRES_Flags  FDMA PRES Flags

@Description	FDMA Present segment flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_PRES_NO_FLAGS	0x00000000
	/** Reference within the frame to present from (This field is ignored
	 * when presenting PTA or ASA segments).
	 * If set - end of the frame. Otherwise - start of the frame. */
#define FDMA_PRES_SR_BIT	0x100

/** @} end of group FDMA_PRES_Flags */

/**************************************************************************//**
@Group		FDMA_EXT_Flags  FDMA EXT Flags

@Description	FDMA Extend segment flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_EXT_NO_FLAGS	0x00000000
	/** The type of segment to present. Only one option may be choose from
	 * \ref fdma_st_options. */
#define FDMA_EXT_ST_BIT	fdma_st_options

/** @} end of group FDMA_EXT_Flags */

/**************************************************************************//**
@Group		FDMA_ENWF_Flags  FDMA ENWF Flags

@Description	FDMA Enqueue working frame flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_ENWF_NO_FLAGS	0x00000000
	/** Terminate Control options. Only one option may be choose from
	 * \ref fdma_enqueue_tc_options. */
#define FDMA_ENWF_TC		fdma_enqueue_tc_options
	/** Enqueue Priority source.
	 * Relevant for Queuing Destination Selection.
	 * If set - use QD_PRI from h/w context (this is the value found in the
	 * WQID field from ADC). Otherwise - use QD_PRI provided with DMA
	 * Command. */
#define FDMA_ENWF_PS_BIT	0x00001000

/** @} end of group FDMA_ENWF_Flags */

/**************************************************************************//**
@Group		FDMA_ENF_Flags  FDMA ENF Flags

@Description	FDMA Enqueue frame flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_ENF_NO_FLAGS	0x00000000
	/** Terminate Control options. Only one option may be choose from
	 * \ref fdma_enqueue_tc_options. */
#define FDMA_ENF_TC		fdma_enqueue_tc_options
	/** Enqueue Priority source.
	 * Relevant for Queuing Destination Selection.
	 * If set - use QD_PRI from h/w context (this is the value found in the
	 * WQID field from ADC). Otherwise - use QD_PRI provided with DMA
	 * Command. */
#define FDMA_ENF_PS_BIT		0x00001000
	/** Bypass DPAA resource Isolation.
	 * If set - Isolation is not enabled for this command (the FQID ID
	 * specified is a real (not virtual) pool ID). Otherwise - Isolation is
	 * enabled for this command (the FQID ID specified is virtual within the
	 * specified ICID). */
#define FDMA_ENF_BDI_BIT	0x80000000

/** @} end of group FDMA_ENF_Flags */


/**************************************************************************//**
@Group		FDMA_Discard_WF_Flags  FDMA Discard WF Flags

@Description	FDMA Discard working frame flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_DIS_NO_FLAGS	0x00000000
#ifdef REV2
	/** Terminate Control.
	 * If set - Trigger the Terminate task command right after the discard.
	 * Otherwise - Return after discard.*/
#define FDMA_DIS_WF_TC_BIT	0x00000100
	/** Frame Source: Discard working frame (using frame handle).*/
#define FDMA_DIS_FS_HANDLE_BIT	0x0000
	/** Frame Source: Discard Frame (using frame FD).*/
#define FDMA_DIS_FS_FD_BIT	0x0200

#endif /* REV2 */


/** @} end of group FDMA_Discard_WF_Flags */


/**************************************************************************//**
@Group		FDMA_Replicate_Flags  FDMA Replicate Flags

@Description	FDMA Replicate Working Frame flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_REPLIC_NO_FLAGS	0x00000000
#ifdef REV2
	/** Enqueue the replicated frame to the provided Queueing Destination.
	 * Release destination frame handle is implicit when enqueueing.
	 * If set - replicate and enqueue. Otherwise - replicate only. */
#define FDMA_REPLIC_ENQ_BIT	0x00000400
	/** The source frame resources are released after the replication.
	 * Release source frame handle is implicit when discarding.
	 * If set - discard source frame and release frame handle.
	 * Otherwise - keep source frame. */
#define FDMA_REPLIC_DSF_BIT	0x00000800
	/** Enqueue Priority source.
	 * Relevant for Queuing Destination Selection.
	 * If set - use QD_PRI from h/w context.
	 * Otherwise - use QD_PRI provided with DMA Command. */
#define FDMA_REPLIC_PS_BIT	0x00001000
#endif /* REV2 */
	/** AIOP FDMA copy frame annotations options. Only one option may be
	 * choose from \ref fdma_cfa_options. */
#define FDMA_REPLICATE_CFA	fdma_cfa_options

/** @} end of group FDMA_Replicate_Flags */


/**************************************************************************//**
@Group		FDMA_Concatenate_Flags  FDMA Concatenate Flags

@Description	FDMA Concatenate Frames flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_CONCAT_NO_FLAGS	0x00000000
	/** Set SF bit on SGE of frame 2. Note, this will force the
	 * usage of SG format on the concatenated frame.
	 * If set - Set SF bit. Otherwise - Do not set SF bit. */
#define FDMA_CONCAT_SF_BIT	0x00000100
	/** Post Concatenate Action.
	 * If set - close resulting working frame 1 using provided storage
	 * profile, update FD1.
	 * Otherwise - keep resulting working frame 1 open. */
#define FDMA_CONCAT_PCA_BIT	0x00000800

/** @} end of group FDMA_Concatenate_Flags */

/**************************************************************************//**
@Group		FDMA_Split_Flags  FDMA Split Flags

@Description	FDMA Split Frame flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_SPLIT_NO_FLAGS	0x00000000
	/** AIOP FDMA copy frame annotations options. Only one option may be
	 * choose from \ref fdma_cfa_options. */
#define FDMA_SPLIT_CFA		fdma_cfa_options
	/** AIOP FDMA Post split action options. Only one option may be
	 * choose from \ref fdma_split_psa_options. */
#define FDMA_SPLIT_PSA		fdma_split_psa_options
	/** Frame split mode.
	 * If set - Split is performed based on Scatter Gather Entry SF bit.
	 * Otherwise - Split is performed at the split_size_sf parameter value
	 * of the split command. */
#define FDMA_SPLIT_SM_BIT	0x00000200
	/** Reference within the frame to present from.
	 * If set - end of the frame. Otherwise - start of the frame. */
#define FDMA_SPLIT_SR_BIT	0x00000100

/** @} end of group FDMA_Split_Flags */

/**************************************************************************//**
@Group		FDMA_Replace_Flags  FDMA Replace Flags

@Description	FDMA Replace working frame segment flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_REPLACE_NO_FLAGS	0x00000000
	/** Segment action options. Only one option may be
	 * choose from \ref fdma_replace_sa_options. */
#define FDMA_REPLACE_SA		fdma_replace_sa_options

/** @} end of group FDMA_Replace_Flags */

/**************************************************************************//**
@Group		FDMA_Copy_Flags  FDMA Copy Flags

@Description	FDMA Copy data flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_COPY_NO_FLAGS	0x00000000
	/** Source Memory:.
	 * If set - Copy source memory address is in the AIOP Shared
	 * Memory.
	 * Otherwise - Copy source memory address is in the workspace. */
#define FDMA_COPY_SM_BIT	0x00000100
	/** Destination Memory:.
	 * If set - Copy destination memory address is in the AIOP Shared
	 * Memory.
	 * Otherwise - Copy destination memory address is in the workspace. */
#define FDMA_COPY_DM_BIT	0x00000200

/** @} end of group FDMA_Copy_Flags */

/**************************************************************************//**
@Group		FDMA_ACQUIRE_BUFFER_Flags  FDMA ACQUIRE BUFFER Flags

@Description	FDMA Acquire buffer flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_ACQUIRE_NO_FLAGS	0x00000000
	/** Bypass DPAA resource Isolation:
	 * If reset - Isolation is enabled for this command. The pool ID
	 * specified is virtual within the specified ICID.
	 * If set - Isolation is not enabled for this command. The pool ID
	 * specified is a real (not virtual) pool ID. */
#define FDMA_ACQUIRE_BDI_BIT	0x80000000

/** @} end of group FDMA_ACQUIRE_BUFFER_Flags */

/**************************************************************************//**
@Group		FDMA_RELEASE_BUFFER_Flags  FDMA RELEASE BUFFER Flags

@Description	FDMA Release buffer flags

@{
*//***************************************************************************/

	/** Default command configuration. */
#define FDMA_RELEASE_NO_FLAGS	0x00000000
	/** Bypass DPAA resource Isolation:
	 * If reset - Isolation is enabled for this command. The pool ID
	 * specified is virtual within the specified ICID.
	 * If set - Isolation is not enabled for this command. The pool ID
	 * specified is a real (not virtual) pool ID. */
#define FDMA_RELEASE_BDI_BIT	0x80000000

/** @} end of group FDMA_RELEASE_BUFFER_Flags */

/**************************************************************************//**
@Group		FDMA_ISOLATION_ATTRIBUTES_Flags  FDMA ISOLATION ATTRIBUTES Flags

@Description	Isolation context flags

@{
*//***************************************************************************/

	/** Virtual Address of the Stored frame flag. */
#define FDMA_ICID_CONTEXT_VA	0x0001
	/** Effective Virtual Address of the Stored frame flag. */
#define FDMA_ICID_CONTEXT_eVA	0x0001
	/** Privilege Level of the Stored frame flag. */
#define FDMA_ICID_CONTEXT_PL	0x0004
	/** BDI of the Stored frame flag. */
#define FDMA_ICID_CONTEXT_BDI	0x8000

/** @} end of group FDMA_ISOLATION_ATTRIBUTES_Flags */


/** @} end of group FDMA_Commands_Flags */


/**************************************************************************//**
 @Group		FDMA_Structures FDMA Structures

 @Description	FDMA Structures

 @{
*//***************************************************************************/

/*
*************************************************************************
@Description	Working Frame structure.

		Includes a presented frame information.

*************************************************************************
struct working_frame {
		* A pointer to Frame descriptor in workspace.
		 * The FD address in workspace must be aligned to 32 bytes.
	struct ldpaa_fd *fd;
		* Handle to the HW working frame
	uint8_t frame_handle;
};


*************************************************************************
@Description	Segment structure.

		Includes a presented segment information.

*************************************************************************
struct segment {
		* A pointer to the Working Frame holding the segment
	struct working_frame *wf;
		* A pointer to the address within the workspace of the
			 * presented segment.
	void *ws_address;
		* The number of segments bytes presented
	uint16_t seg_length;
		* Handle to the presented segment.
	uint8_t	 seg_handle;

		* Segment flags \ref FDMA_PRES_Flags
	uint8_t	 flags;
		* Segment offset in the frame relative to the
		 * \ref FDMA_PRES_SR_BIT flag
	uint16_t seg_offset;
};
*/

/**************************************************************************//**
@Description	FDMA access management qualifier (AMQs) structure.

*//***************************************************************************/
struct fdma_amq {
		/** \link FDMA_ISOLATION_ATTRIBUTES_Flags icid context flags
		 * \endlink */
	uint16_t flags;
		/**
		 * bits<1-15> : ICID of the Stored frame. */
	uint16_t icid;
};

/**************************************************************************//**
@Description	Frame presentation parameters structure.

*//***************************************************************************/
struct fdma_present_frame_params {
		/** \link FDMA_Present_Frame_Flags initial frame presentation
		 * flags \endlink */
	uint32_t flags;
		/** A pointer to the location in workspace for the presented
		 * frame segment. */
	void *seg_dst;
		/** A pointer to a 64B-aligned location in the workspace to
		 * store the 64B PTA field. Set \ref PRC_PTA_NOT_LOADED_ADDRESS
		 * for no PTA presentation. */
	void *pta_dst;
		/** A pointer to a 64B-aligned location in the workspace to
		 * store the ASA. */
	void *asa_dst;
		/** A pointer to the location in workspace of the FD that is to
		* be presented.
		* The FD address in workspace must be aligned to 32 bytes.*/
	struct ldpaa_fd *fd_src;
		/** location within the presented frame to start presenting
		 * the segment from. */
	uint16_t seg_offset;
		/** Number of frame bytes to present and create an open
		 * segment for. */
	uint16_t present_size;
		/** Bits<1-15> : Isolation Context ID. Frame AMQ attribute.
		* Used only in case \ref FDMA_INIT_AS_BIT is set. */
	uint16_t icid;
		/** The first ASA 64B quantity to present. */
	uint8_t asa_offset;
		/** Number (maximum) of 64B ASA quantities to present. */
	uint8_t asa_size;
		/** Returned parameter:
		* The number of bytes actually presented (the segment actual
		* size). */
	uint16_t seg_length;
		/** Returned parameter:
		 * The handle of the working frame. */
	uint8_t frame_handle;
		/** Returned parameter:
		 * The handle of the presented segment. */
	uint8_t seg_handle;
};

/**************************************************************************//**
@Description	Segment Presentation parameters structure.

*//***************************************************************************/
struct fdma_present_segment_params {
		/** \link FDMA_PRES_Flags Present segment flags \endlink */
	uint32_t flags;
		/** A pointer to the location in workspace for the presented
		 * frame segment. */
	void	 *ws_dst;
		/** Location within the presented frame to start presenting
		 * from. Must be within the bound of the frame. Relative to
		\ref FDMA_PRES_SR_BIT flag. */
	uint16_t offset;
		/** Number of frame bytes to present (any value including 0).*/
	uint16_t present_size;
		/** Returned parameter:
		 * The number of bytes actually presented (the segment actual
		 * size). */
	uint16_t seg_length;
		/** Returned parameter:
		 * The handle of the presented segment. */
	uint8_t  seg_handle;
		/** working frame from which to open a segment. */
	uint8_t	 frame_handle;
};

/**************************************************************************//**
@Description	Queueing Destination Enqueue parameters structure.

*//***************************************************************************/
struct fdma_queueing_destination_params {
		/** Queueing destination for the enqueue. */
	uint16_t qd;
		/** Distribution hash value passed to QMan for distribution
		 * purpose on the enqueue. */
	uint16_t qdbin;
		/** Queueing Destination Priority. */
	uint8_t	 qd_priority;
};

/**************************************************************************//**
@Description	Concatenate frames parameters structure.

*//***************************************************************************/
struct fdma_concatenate_frames_params {
		/** \link FDMA_Concatenate_Flags concatenate frames
		 * flags \endlink */
	uint32_t  flags;
		/** Returned parameter:
		 * AMQ attributes (the VA is the effective VA) */
	struct fdma_amq amq;
		/** The handle of working frame 1. */
	uint16_t frame1;
		/** The handle of working frame 2. */
	uint16_t frame2;
		/** Storage Profile used to store frame data if additional
		 * buffers are required when optionally closing the concatenated
		 *  working frame (\ref FDMA_CONCAT_PCA_BIT is set) */
	uint8_t  spid;
		/** Trim a number of bytes from the beginning of frame 2 before
		 * the concatenation. A size of zero disables the trim. */
	uint8_t  trim;
};

/**************************************************************************//**
@Description	Split frame parameters structure.

*//***************************************************************************/
struct fdma_split_frame_params {
		/** \link FDMA_Split_Flags split frames flags
		 * \endlink */
	uint32_t flags;
		/** A pointer to the location in workspace for the split FD. */
	struct ldpaa_fd *fd_dst;
		/** A pointer to the location in workspace for the presented
		 * split frame segment. */
	void *seg_dst;
		/** location within the presented split frame to start
		 * presenting the segment from. */
	uint16_t seg_offset;
		/** Number of frame bytes to present and create an open segment
		 * for (in the split frame). */
	uint16_t present_size;
		/** SM bit = 0: Split size, number of bytes to split from the
		 * head of the input frame and move to the output frame.
		 * SM bit = 1: not used. */
	uint16_t split_size_sf;
		/** Returned parameter:
		 * The number of bytes actually presented from the split frame
		 * (the segment actual size). */
	uint16_t seg_length;
		/** The handle of the source working frame. */
	uint8_t  source_frame_handle;
		/** Returned parameter:
		 * A pointer to the handle of the split working frame. */
	uint8_t  split_frame_handle;
		/** Returned parameter:
		 * A pointer to the handle of the presented segment (in the
		 * split frame). */
	uint8_t  seg_handle;
		/** Storage Profile used to store frame data if additional
		 * buffers are required when optionally closing the split
		 * working frame */
	uint8_t  spid;
};

/**************************************************************************//**
@Description	Insert Segment data parameters structure.

*//***************************************************************************/
struct fdma_insert_segment_data_params {
		/** a pointer to the workspace location from which the inserted
		 * segment data starts. */
	void	 *from_ws_src;
		/** A pointer to the location in workspace for the represented
		 * frame segment (relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT
		 *  flag is set). */
	void	 *ws_dst_rs;
		/** \link FDMA_Replace_Flags replace working frame segment
		 * flags \endlink */
	uint32_t flags;
		/** Offset from the previously presented segment representing
		 * where to insert the data.
		 * Must be within the presented segment size. */
	uint16_t to_offset;
		/** Size of the data being inserted to the segment. */
	uint16_t insert_size;
		/** Number of frame bytes to represent in the segment. Must be
		 * greater than 0.
		 * Relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set.*/
	uint16_t size_rs;
		/** Returned parameter:
		 * The number of bytes actually presented (the segment actual
		 * size).
		 * Relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set*/
	uint16_t seg_length_rs;
		/** Working frame handle to which the data is being inserted.*/
	uint8_t	 frame_handle;
		/** Data segment handle (related to the working frame handle)
		 * to which the data is being inserted. */
	uint8_t  seg_handle;
};

/**************************************************************************//**
@Description	Delete Segment data parameters structure.

*//***************************************************************************/
struct fdma_delete_segment_data_params {
		/** A pointer to the location in workspace for the represented
		 * frame segment (relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT
		 *  flag is set). */
	void	 *ws_dst_rs;
		/** \ref FDMA_Replace_Flags replace working frame segment
		 * flags. */
	uint32_t flags;
		/** Offset from the previously presented segment representing
		 * from where to delete data.
		 * Must be within the presented segment size. */
	uint16_t to_offset;
		/** Size of the data being deleted from the segment. */
	uint16_t delete_target_size;
		/** Number of frame bytes to represent in the segment. Must be
		 * greater than 0.
		 * Relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set.*/
	uint16_t size_rs;
		/** Returned parameter:
		 * The number of bytes actually presented (the segment actual
		 * size).
		 * Relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set.*/
	uint16_t seg_length_rs;
		/** Working frame handle from which the data is being
		 * deleted.*/
	uint8_t	 frame_handle;
		/** Data segment handle (related to the working frame handle)
		 * from which the data is being deleted. */
	uint8_t  seg_handle;
};

/** @} end of group FDMA_Structures */

/**************************************************************************//**
 @Group		FDMA_STATUS FDMA Status

 @Description	FDMA Statuses.

 @{
*//***************************************************************************/

	/** Unable to fulfill specified Data segment presentation size.
	 * This is an indication that the requested data segment presentation 
	 * size could not be fulfill since the frame size (starting from the 
	 * segment offset) is smaller than the requested presentation size. */
#define FDMA_STATUS_UNABLE_PRES_DATA_SEG	0x00000008
	/** Unable to fulfill specified ASA segment presentation size.
	 * This is an indication that the requested ASA segment presentation 
	 * size could not be fulfill since the ASA size (starting from the 
	 * ASA segment offset) is smaller than the requested presentation 
	 * size. */
#define FDMA_STATUS_UNABLE_PRES_ASA_SEG		0x0000000A

/** @} end of group FDMA_STATUS */


/**************************************************************************//**
 @Group		FDMA_Functions FDMA Functions

 @Description	FDMA Functions

 @{
*//***************************************************************************/


/* FPDMA (Frame Presentation DMA) Commands from AIOP Software */

/**************************************************************************//**
@Function	fdma_present_default_frame

@Description	Initial presentation of a default working frame into the task
		workspace.

		Implicit input parameters in Task Defaults: segment address,
		segment offset, segment size, PTA address (\ref
		PRC_PTA_NOT_LOADED_ADDRESS for no presentation), ASA address,
		ASA size, ASA offset, Segment Reference bit, fd address in
		workspace (\ref HWC_FD_ADDRESS), AMQ attributes (PL, VA, BDI,
		ICID).

		This command can also be used to initiate construction of a
		frame from scratch (without a presented frame). In this case
		the fd address parameter must point to a null FD (all 0x0) in
		the workspace, and an empty segment must be allocated (of size
		0). Due to PDM TKT254401 this option can work only on the 
		default frame.

		Implicitly updated values in Task Defaults:  frame handle,
		segment handle.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (not relevant if the 
		NDS bit in the presentation context is set, or if the Data size
		in the presentation context is 0).
		This return value is caused since the requested presentation 
		exceeded frame data end.
@Retval		::FDMA_STATUS_UNABLE_PRES_ASA_SEG - Unable to fulfill 
		specified ASA segment presentation size (not relevant if the ASA
		size in the presentation context is 0).
		This return value is caused since the requested presentation 
		exceeded frame ASA end.
@Retval		EIO - Received frame with non-zero FD[err] field. In such a case 
		the returned frame handle is valid, but no presentations 
		occurred.

@Cautions	In case the presented segment will be used by 
		PARSER/CTLU/KEYGEN, it should be presented in a 16 byte aligned 
		workspace address (due to TKT254635).
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline int fdma_present_default_frame(void);

/**************************************************************************//**
@Function	fdma_present_frame

@Description	Initial presentation of the frame into the task
		workspace.

		This command can also be used to initiate construction of a
		frame from scratch (without a presented frame). In this case
		the fd address parameter must point to a null FD (all 0x0) in
		the workspace, and an empty segment must be allocated (of size
		0). Due to PDM TKT254401 this option can work only on the 
		default frame.

		In case the fd destination parameter points to the default FD
		address, the service routine will update Task defaults variables
		according to command parameters.

@Param[in,out]	params - A pointer to the Initial frame presentation command
		parameters \ref fdma_present_frame_params.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (not relevant if the 
		NDS bit flag in the function parameters is set, or if the Data 
		size in the function parameters is 0).
		This return value is caused since the requested presentation 
		exceeded frame data end.
@Retval		::FDMA_STATUS_UNABLE_PRES_ASA_SEG - Unable to fulfill 
		specified ASA segment presentation size (not relevant if the ASA
		size in the function parameters is 0).
		This return value is caused since the requested presentation 
		exceeded frame ASA end.
@Retval		EIO - Received frame with non-zero FD[err] field. In such a case 
		the returned frame handle is valid, but no presentations 
		occurred.

@Cautions	In case the presented segment will be used by 
		PARSER/CTLU/KEYGEN, it should be presented in a 16 byte aligned 
		workspace address (due to TKT254635).
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_present_frame(
		struct fdma_present_frame_params *params);

/**************************************************************************//**
@Function	fdma_present_default_frame_without_segments

@Description	Initial presentation of a default frame into the task workspace
		without any segments (Data, ASA, PTA).

		Implicit input parameters in Task Defaults: AMQ attributes (PL,
		VA, BDI, ICID), FD address.

		Implicitly updated values in Task Defaults: frame handle, NDS
		bit, ASA size (0), PTA address(\ref PRC_PTA_NOT_LOADED_ADDRESS).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EIO - Received frame with non-zero FD[err] field. In such a case 
		the returned frame handle is valid, but no presentations 
		occurred.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_present_default_frame_without_segments(void);

/**************************************************************************//**
@Function	fdma_present_frame_without_segments

@Description	Initial presentation of a frame into the task workspace without
		any segments (Data, ASA, PTA).

		Implicit input parameters in Task Defaults: AMQ attributes (PL,
		VA, BDI, ICID).

		Implicitly updated values in Task Defaults in case the FD points
		to the default FD location: frame handle, NDS bit, ASA size (0),
		PTA address (\ref PRC_PTA_NOT_LOADED_ADDRESS).

@Param[in]	fd - A pointer to the workspace location of the Frame Descriptor
		to present.
@Param[in]	flags - \link FDMA_Present_Frame_Flags Present segment flags.
		\endlink
@Param[in]	icid - Bits<1-15> : Isolation Context ID. Frame AMQ attribute.
		Used only in case \ref FDMA_INIT_AS_BIT is set.
@Param[out]	frame_handle - A handle to the opened working frame.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EIO - Received frame with non-zero FD[err] field. In such a case 
		the returned frame handle is valid, but no presentations 
		occurred.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_present_frame_without_segments(
		struct ldpaa_fd *fd,
		uint32_t flags,
		uint16_t icid,
		uint8_t *frame_handle);

/**************************************************************************//**
@Function	fdma_present_default_frame_segment

@Description	Open a default segment of the default working frame and copy the
		segment data into the specified location in the workspace.

		Implicit input parameters in Task Defaults: frame handle.

		Implicitly updated values in Task Defaults: segment length,
		segment handle, segment address, segment offset.

@Param[in]	flags - \link FDMA_PRES_Flags Present segment flags. \endlink
@Param[in]	ws_dst - A pointer to the location in workspace for the
		presented frame segment.
@Param[in]	offset - Location within the presented frame to start presenting
		from. Must be within the bound of the frame. Relative to
		\ref FDMA_PRES_SR_BIT flag.
@Param[in]	present_size - Number of frame bytes to present (any value
		including 0).

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (not relevant if the 
		present_size in the function parameters is 0).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@Cautions	In case the presented segment will be used by 
		PARSER/CTLU/KEYGEN, it should be presented in a 16 byte aligned 
		workspace address (due to TKT254635).
@Cautions	This command may be invoked only for a default Data segments.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline int fdma_present_default_frame_segment(
		uint32_t flags,
		void	 *ws_dst,
		uint16_t offset,
		uint16_t present_size);

/**************************************************************************//**
@Function	fdma_present_frame_segment

@Description	Open a segment of a working frame and copy the
		segment data into the specified location in the workspace.

@Param[in]	params - A pointer to the Present frame segment command
		parameters \ref fdma_present_segment_params.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (not relevant if the 
		present_size in the function parameters is 0).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@Cautions	In case the presented segment will be used by 
		PARSER/CTLU/KEYGEN, it should be presented in a 16 byte aligned 
		workspace address (due to TKT254635).
@Cautions	This command may be invoked only for Data segments.
@Cautions	This command may not be invoked on the default Data segments.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_present_frame_segment(
		struct fdma_present_segment_params *params);

/**************************************************************************//**
@Function	fdma_read_default_frame_asa

@Description	Read the Accelerator Specific Annotation (ASA) section
		associated with the default working frame from external memory
		into a specified location in the workspace.

		Implicit input parameters in Task Defaults: frame handle.

		Implicitly updated values in Task Defaults: ASA segment address,
		ASA segment length (the number of bytes actually presented given
		in 64B units), ASA segment offset.

@Param[in]	ws_dst - A pointer to the location in workspace for 
		the presented ASA segment. (The address is specified in 64bytes
		units due to a HW bug). 
@Param[in]	offset - Location within the ASA to start presenting from.
		Must be within the bound of the frame. Specified in 64B units.
		Relative to \ref FDMA_PRES_SR_BIT flag.
@Param[in]	present_size - Number of frame bytes to present (Must be greater
		than 0). Contains the number of 64B quantities to present
		because the Frame ASAL field is specified in 64B units.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_ASA_SEG - Unable to fulfill 
		specified ASA segment presentation size (not relevant if the 
		present_size in the function parameters is 0).
		This return value is caused since the requested presentation 
		exceeded frame ASA end.

@remark		The ASA segment handle value is fixed \ref FDMA_ASA_SEG_HANDLE.

@Cautions	The HW must have previously opened the frame with an
		automatic initial presentation or initial presentation command.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_read_default_frame_asa(
		void	 *ws_dst,
		uint16_t offset,
		uint16_t present_size);

/**************************************************************************//**
@Function	fdma_read_default_frame_pta

@Description	Read the Pass Through Annotation (PTA) section associated with
		the default working frame from external memory into a specified
		location in the workspace.

		Implicit input parameters in Task Defaults: frame handle.

		Implicitly updated values in Task Defaults: PTA segment address.

@Param[in]	ws_dst - A pointer to the location in workspace for
		the presented PTA segment. (The address is specified in 64bytes
		units due to a HW bug).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EIO - Unable to present PTA segment (no PTA segment in the 
		working frame).

@remark		The PTA segment handle value is fixed \ref FDMA_PTA_SEG_HANDLE.
@remark		The length of the read PTA can be read directly from the FD.

@Cautions	The HW must have previously opened the frame with an
		automatic initial presentation or initial presentation command.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_read_default_frame_pta(void *ws_dst);

/**************************************************************************//**
@Function	fdma_extend_default_segment_presentation

@Description	Extend an existing presentation default segment with additional
		data.

		This command can present frame data, and frame accelerator
		specific annotation data (ASA).

		Implicitly updated values in Task Defaults: segment (Data or
		ASA) length. For ASA segment, specifies the the count (in 64B 
		units) of the deepest presented unit of ASA.

@Param[in]	extend_size - Number of additional bytes to present (0 results
		in no operation). For ASA segment, specifies the number of 
		additional 64B quantities to present from the ASA.
@Param[in]	ws_dst - A pointer to the location within the workspace
		to present the additional frame segment data.
@Param[in]	flags - \link FDMA_EXT_Flags Extend segment mode
		bits. \endlink

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment extend size.
		This return value is caused since the requested presentation 
		exceeded frame data end.
@Retval		::FDMA_STATUS_UNABLE_PRES_ASA_SEG - Unable to fulfill 
		specified ASA segment extend size.
		This return value is caused since the requested presentation 
		exceeded frame ASA end.

@remark		The extended data to be presented does not have to be
		sequential relative to the current presented segment.
@remark		After this command the extended default segment (Data/ASA) 
		values in the presentation context relevant to this segment 
		will not be valid.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_extend_default_segment_presentation(
		uint16_t extend_size,
		void	 *ws_dst,
		uint32_t flags);


/* FDMA (Frame Output DMA) Commands from AIOP Software */

/**************************************************************************//**
@Function	fdma_store_default_frame_data

@Description	Write out modified default Working Frame to the backing storage
		in system memory described by the Frame Descriptor and close the
		working frame.

		Existing FD buffers are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.
		If the original structure can not be modified, then a new
		structure will be assembled using the default frame storage
		profile ID.

		Implicit input parameters in Task Defaults: frame handle,
		spid (storage profile ID).

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
inline int fdma_store_default_frame_data(void);

/**************************************************************************//**
@Function	fdma_store_frame_data

@Description	Write out modified Working Frame to the backing storage
		in system memory described by the Frame Descriptor and close the
		working frame.

		Existing FD buffers are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.
		If the original structure can not be modified, then a new
		structure will be assembled using the frame storage
		profile ID parameter.

@Param[in]	frame_handle - Handle to the frame to be closed.
@Param[in]	spid - storage profile ID used to store frame data if additional
		buffers are required.
@Param[out]	amq - AMQ attributes \ref fdma_amq.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@remark		FD is updated.
@remark		If some segments of the Working Frame are not closed, they
		will be closed and the segment handles will be implicitly
		released.
@remark		Release frame handle is implicit in this function.

@Cautions	All modified segments (which are to be stored) must be replaced
		(by a replace command) before storing a frame.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_store_frame_data(
		uint8_t frame_handle,
		uint8_t spid,
		struct fdma_amq *amq);

/**************************************************************************//**
@Function	fdma_store_and_enqueue_default_frame_fqid

@Description	Enqueue the default Working Frame to a given destination
		according to a frame queue id.

		After completion, the Enqueue Working Frame command can
		terminate the task or return.

		If the Working Frame to be enqueued is modified, the Enqueue
		Frame command performs a Store Frame Data command on the
		Working Frame.

		If the Working Frame to be enqueued is modified, existing
		buffers as described by the FD are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.

		If the original structure can not be modified, then a new
		structure will be assembled using the default frame storage
		profile ID.

		Implicit input parameters in Task Defaults: frame handle, spid
		(storage profile ID).

@Param[in]	fqid - frame queue ID for the enqueue.
@Param[in]	flags - \link FDMA_ENWF_Flags enqueue working frame mode
		bits. \endlink

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@remark		If some segments of the Working Frame are not closed, they
		will be closed and the segment handles will be implicitly
		released.
@remark		Release frame handle is implicit in this function.

@Cautions	Function may not return.
@Cautions	All modified segments (which are to be stored) must be
		replaced (by a replace command) before storing a frame.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_store_and_enqueue_default_frame_fqid(
		uint32_t fqid,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_store_and_enqueue_frame_fqid

@Description	Enqueue a Working Frame to a given destination according to a
		frame queue id.

		After completion, the Enqueue Working Frame command can
		terminate the task or return.

		If the Working Frame to be enqueued is modified, the Enqueue
		Frame command performs a Store Frame Data command on the
		Working Frame.

		If the Working Frame to be enqueued is modified, existing
		buffers as described by the FD are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.

		If the original structure can not be modified, then a new
		structure will be assembled using the provided storage
		profile ID.

@Param[in]	frame_handle - working frame handle to enqueue.
@Param[in]	flags - \link FDMA_ENWF_Flags enqueue working frame mode
		bits. \endlink
@Param[in]	fqid - frame queue ID for the enqueue.
@Param[in]	spid - Storage Profile ID used to store frame data.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@remark		If some segments of the Working Frame are not closed, they
		will be closed and the segment handles will be implicitly
		released.
@remark		Release frame handle is implicit in this function.

@Cautions	Function may not return.
@Cautions	All modified segments (which are to be stored) must be
		replaced (by a replace command) before storing a frame.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_store_and_enqueue_frame_fqid(
		uint8_t  frame_handle,
		uint32_t flags,
		uint32_t fqid,
		uint8_t  spid);

/**************************************************************************//**
@Function	fdma_store_and_enqueue_default_frame_qd

@Description	Enqueue the default Working Frame to a given destination
		according to a queueing destination.

		After completion, the Enqueue Working Frame command can
		terminate the task or return.

		If the Working Frame to be enqueued is modified, the Enqueue
		Frame command performs a Store Frame Data command on the
		Working Frame.

		If the Working Frame to be enqueued is modified, existing
		buffers as described by the FD are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.

		If the original structure can not be modified, then a new
		structure will be assembled using the default frame storage
		profile ID.

		Implicit input parameters in Task Defaults: frame handle, spid
		(storage profile ID).

@Param[in]	qdp - Pointer to the queueing destination parameters \ref
		fdma_queueing_destination_params.
@Param[in]	flags - \link FDMA_ENWF_Flags enqueue working frame mode
		bits. \endlink

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@remark		If some segments of the Working Frame are not closed, they
		will be closed and the segment handles will be implicitly
		released.
@remark		Release frame handle is implicit in this function.

@Cautions	Function may not return.
@Cautions	All modified segments (which are to be stored) must be
		replaced (by a replace command) before storing a frame.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline int fdma_store_and_enqueue_default_frame_qd(
		struct fdma_queueing_destination_params *qdp,
		uint32_t	flags);

/**************************************************************************//**
@Function	fdma_store_and_enqueue_frame_qd

@Description	Enqueue a Working Frame to a given destination according to a
		queueing destination.

		After completion, the Enqueue Working Frame command can
		terminate the task or return.

		If the Working Frame to be enqueued is modified, the Enqueue
		Frame command performs a Store Frame Data command on the
		Working Frame.

		If the Working Frame to be enqueued is modified, existing
		buffers as described by the FD are used to store data.

		If the modified frame no longer fits in the original structure,
		new buffers can be added using the provided storage profile.

		If the original structure can not be modified, then a new
		structure will be assembled using the provided storage
		profile ID.

		Implicit input parameters in Task Defaults: frame handle, spid
		(storage profile ID).

@Param[in]	frame_handle - working frame handle to enqueue.
@Param[in]	flags - \link FDMA_ENWF_Flags enqueue working frame mode
		bits. \endlink
@Param[in]	qdp - Pointer to the queueing destination parameters \ref
		fdma_queueing_destination_params.
@Param[in]	spid - Storage Profile ID used to store frame data.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@remark		If some segments of the Working Frame are not closed, they
		will be closed and the segment handles will be implicitly
		released.
@remark		Release frame handle is implicit in this function.

@Cautions	Function may not return.
@Cautions	All modified segments (which are to be stored) must be
		replaced (by a replace command) before storing a frame.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_store_and_enqueue_frame_qd(
		uint8_t  frame_handle,
		uint32_t flags,
		struct fdma_queueing_destination_params *qdp,
		uint8_t spid);

/**************************************************************************//**
@Function	fdma_enqueue_default_fd_fqid

@Description	Enqueue the default FD (which is not presented) to a given
		destination according to a frame queue id.

		After completion, the Enqueue Frame command can
		terminate the task or return.

		Implicit input parameters in Task Defaults: fd address.

@Param[in]	icid - ICID of the FD to enqueue.
@Param[in]	flags - \link FDMA_ENF_Flags enqueue frame flags.
		\endlink
@Param[in]	fqid - frame queue ID for the enqueue.


@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.

@Cautions	The frame associated with the FD must not be presented (closed).
@Cautions	Function may not return.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_enqueue_default_fd_fqid(
		uint16_t icid,
		uint32_t flags,
		uint32_t fqid);

/**************************************************************************//**
@Function	fdma_enqueue_fd_fqid

@Description	Enqueue a Frame Descriptor (which is not presented) to a given
		destination according to a frame queue id.

		After completion, the Enqueue Frame command can
		terminate the task or return.

@Param[in]	fd - Pointer to the location in workspace of the Frame
		Descriptor to be enqueued \ref ldpaa_fd.
@Param[in]	flags - \link FDMA_ENF_Flags enqueue frame flags.
		\endlink
@Param[in]	fqid - frame queue ID for the enqueue.
@Param[in]	icid - ICID of the FD to enqueue.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.

@Cautions	The frame associated with the FD must not be presented (closed).
@Cautions	Function may not return.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_enqueue_fd_fqid(
		struct ldpaa_fd *fd,
		uint32_t flags,
		uint32_t fqid,
		uint16_t icid);

/**************************************************************************//**
@Function	fdma_enqueue_default_fd_qd

@Description	Enqueue the default FD (which is not presented) to a given
		destination according to a queueing destination.

		After completion, the Enqueue Frame command can
		terminate the task or return.

		Implicit input parameters in Task Defaults: fd address.

@Param[in]	icid - ICID of the FD to enqueue.
@Param[in]	flags - \link FDMA_ENF_Flags enqueue frame flags.
		\endlink
@Param[in]	enqueue_params - Pointer to the queueing destination parameters
 	 	 \ref fdma_queueing_destination_params.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.

@Cautions	The frame associated with the FD must not be presented (closed).
@Cautions	Function may not return.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_enqueue_default_fd_qd(
		uint16_t icid,
		uint32_t flags,
		struct fdma_queueing_destination_params *enqueue_params);

/**************************************************************************//**
@Function	fdma_enqueue_fd_qd

@Description	Enqueue a Frame Descriptor (which is not presented) to a given
		destination according to a queueing destination.

		After completion, the Enqueue Frame command can
		terminate the task or return.

@Param[in]	fd - Pointer to the location in workspace of the Frame
		Descriptor to be enqueued \ref ldpaa_fd.
@Param[in]	flags - \link FDMA_ENF_Flags enqueue frame flags.
		\endlink
@Param[in]	enqueue_params - Pointer to the queueing destination parameters
		\ref fdma_queueing_destination_params.
@Param[in]	icid - ICID of the FD to enqueue.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.

@Cautions	The frame associated with the FD must not be presented (closed).
@Cautions	Function may not return.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_enqueue_fd_qd(
		struct ldpaa_fd *fd,
		uint32_t flags,
		struct fdma_queueing_destination_params *enqueue_params,
		uint16_t icid);

/**************************************************************************//**
@Function	fdma_discard_default_frame

@Description	Release the resources associated with the default working
		frame.

		Implicit input parameters in Task Defaults: frame handle.

@Param[in]	flags - \link FDMA_Discard_WF_Flags discard frame flags.
		\endlink

@Return		None.

@remark		Release frame handle and release segment handle(s) are implicit
		in this function.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline void fdma_discard_default_frame(uint32_t flags);

/**************************************************************************//**
@Function	fdma_discard_frame

@Description	Release the resources associated with a working frame.

@Param[in]	frame - Frame handle to be discarded.
@Param[in]	flags - \link FDMA_Discard_WF_Flags discard working frame
		frame flags. \endlink

@Return		None.

@remark		Release frame handle and release segment handle(s) are implicit
		in this function.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_discard_frame(uint16_t frame, uint32_t flags);

/**************************************************************************//**
@Function	fdma_discard_fd

@Description	Release the resources associated with a frame
		descriptor.

		Implicit input parameters in Task Defaults: AMQ attributes (PL,
		VA, BDI, ICID).

		Implicitly updated values in Task Defaults in case the FD points
		to the default FD location: frame handle, NDS bit, ASA size (0),
		PTA address (\ref PRC_PTA_NOT_LOADED_ADDRESS).

@Param[in]	fd - A pointer to the location in the workspace of the FD to be
		discarded \ref ldpaa_fd.
@Param[in]	flags - \link FDMA_Discard_WF_Flags discard working frame
		frame flags. \endlink

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EIO - Received frame with non-zero FD[err] field.

@Cautions	The frame associated with the FD must not be presented (closed).
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline int fdma_discard_fd(struct ldpaa_fd *fd, uint32_t flags);

/**************************************************************************//**
@Function	fdma_force_discard_frame

@Description	Force frame discard. This function first zero FD.err field and
		than discard the frame.
		(A frame with FD.err != 0 cannot be discarded).

		Implicit input parameters in Task Defaults: AMQ attributes (PL,
		VA, BDI, ICID).

		Implicitly updated values: FD.err is zeroed.

@Param[in]	fd - A pointer to the location in the workspace of the FD to be
		discarded \ref ldpaa_fd.

@Return		None.

@Cautions	The frame associated with the FD must not be presented (closed).
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_force_discard_fd(struct ldpaa_fd *fd);

/**************************************************************************//**
@Function	fdma_terminate_task

@Description	End all processing on the associated task and notify the Order
		Scope Manager of the task termination.

@Return		None.

@remark		Release frame handle(s) and release segment handle(s) are
		implicit in this function.

@Cautions	Application software must store (in software managed context)
		or discard the input frame before calling Terminate task
		command to avoid buffer leak.
@Cautions	Function does not return.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline void fdma_terminate_task(void);

/**************************************************************************//**
@Function	fdma_replicate_frame_fqid

@Description	Make a copy of the working frame and optionally enqueue the
		replicated frame (the copy) according to a frame queue id.

		The source Working Frame may be modified (FDMA internal memory
		is updated).

		The source frame is replicated based on its last known state by
		the FDMA.
		
		The replicated working frame DD, DROPP and eVA are inherited 
		from the source frame, its ICID, BDI, PL and SL are taken from 
		the Storage Profile.

@Param[in]	frame_handle1 - Handle of the source frame.
@Param[in]	spid - Storage Profile used to store frame data of the
		destination frame if enqueue is selected, also
		used to determine ICID and memory attributes (BDI/PL/ICID) of 
		the replicated frame. The eVA of the replicated frame is taken 
		from the source working frame.
@Param[in]	fqid - frame queue ID for the enqueue.
@Param[in]	fd_dst - A pointer to the location within the workspace of the
		destination FD.
@Param[in]	flags - \link FDMA_Replicate_Flags replicate working frame
		flags \endlink.
@Param[out]	frame_handle2 - Handle of the replicated frame (when no enqueue
		selected).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@Cautions	Due to TKT258499 this service routine can be called only in 
		order to replicate non-empty frames.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_replicate_frame_fqid(
		uint8_t	frame_handle1,
		uint8_t	spid,
		uint32_t fqid,
		void *fd_dst,
		uint32_t flags,
		uint8_t *frame_handle2
		);

/**************************************************************************//**
@Function	fdma_replicate_frame_qd

@Description	Make a copy of the working frame and optionally enqueue the
		replicated frame (the copy) according to a queueing destination.

		The source Working Frame may be modified (FDMA internal memory
		is updated).

		The source frame is replicated based on its last known state by
		the FDMA.
		
		The replicated working frame DD, DROPP and eVA are inherited 
		from the source frame, its ICID, BDI, PL and SL are taken from 
		the Storage Profile.

@Param[in]	frame_handle1 - Handle of the source frame.
@Param[in]	spid - Storage Profile used to store frame data of the
		destination frame if enqueue is selected, also
		used to determine ICID and memory attributes (BDI/PL/ICID) of 
		the replicated frame. The eVA of the replicated frame is taken 
		from the source working frame.
@Param[in]	enqueue_params - Pointer to the queueing destination parameters
		\ref fdma_queueing_destination_params.
@Param[in]	fd_dst - A pointer to the location within the workspace of the
		destination FD.
@Param[in]	flags - \link FDMA_Replicate_Flags replicate working frame
		flags \endlink.
@Param[out]	frame_handle2 - Handle of the replicated frame (when no enqueue
		was selected).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EBUSY - Enqueue failed due to congestion in QMAN.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@Cautions	Due to TKT258499 this service routine can be called only in 
		order to replicate non-empty frames.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_replicate_frame_qd(
		uint8_t	frame_handle1,
		uint8_t	spid,
		struct fdma_queueing_destination_params *enqueue_params,
		void *fd_dst,
		uint32_t flags,
		uint8_t *frame_handle2
		);

/**************************************************************************//**
@Function	fdma_concatenate_frames

@Description	Join two frames {frame1 , frame2} and return a new 
		concatenated frame.

		The two frames may be modified but all the segments must be
		closed.

		The command also support the option to trim a number of
		bytes from the beginning of the 2nd frame before it is
		concatenated.

		The frames must be in the same ICID.

@Param[in,out]	params - A pointer to the Concatenate frames command parameters
		\ref fdma_concatenate_frames_params.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		ENOMEM - Failed due to buffer pool depletion (relevant only if
		\ref FDMA_CONCAT_PCA_BIT flag is set).

@remark		Frame annotation of the first frame becomes the frame annotation
		of the concatenated frame.
@remark		Release of frame handle 2 is implicit in this function.

@Cautions	Both frames must be opened once calling this command.
@Cautions	In case frame1 handle parameter is the default frame handle,
		the default frame length variable in the Task defaults will not
		be valid after the service routine.
@Cautions	In case frame2 handle parameter is the default frame handle,
		all Task default variables will not be valid after the service
		routine.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_concatenate_frames(
		struct fdma_concatenate_frames_params *params);

/**************************************************************************//**
@Function	fdma_split_frame

@Description	Split a Working Frame into two frames and return an updated
		Working Frame along with a split frame.

		The source Working Frame may be modified (FDMA internal memory
		is updated) but all the segments must be Closed.

		All the frame open segments will be implicitly closed, and
		their segment handles will be released.

		In case the fd destination parameter points to the default FD
		address, the service routine will update the Task
		defaults variables according to command parameters.

		In case \ref FDMA_SPLIT_SM_BIT flag is not set, the service
		routine updates the split frame fd.length field.

@Param[in,out]	params - A pointer to the Split frame command parameters \ref
		fdma_split_frame_params.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		ENOMEM - Failed due to buffer pool depletion (relevant only if
		closing split frame) (relevant for Rev 2).
@Retval		EINVAL - Last split is not possible.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size
		(relevant if \ref FDMA_SPLIT_PSA_PRESENT_BIT flag is set).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@remark		The first fd is updated to reflect the remainder of the
		input fd (the second part of the split frame).
@remark		The second fd represent the split portion of the frame (the
		first part of the split frame). This FD is updated (in place). 
		Most of the FD is cloned from the FD1. The ADDR, LENGTH, BPID, 
		OFFSET, FMT are not updated. The split working frame DD, DROPP, 
		eVA, ICID, BDI, PL and SL are inherited from the source frame.
@remark		Frame annotation of the first frame is preserved.
@remark		If split size is >= frame size then an error will be returned.

@Cautions	In case the presented segment will be used by 
		PARSER/CTLU/KEYGEN, it should be presented in a 16 byte aligned 
		workspace address (due to TKT254635).
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
@Cautions	Due to ticket TKT240996 the following FDMA functions should be 
		called before and after the split command:
		1. Store/Close source frame
		2. Present/Open source frame
		3. Split frame command with PSA flag: 
		FDMA_SPLIT_PSA_NO_PRESENT_BIT
		4. Store/Close new split frame
		5. Present/Open new split frame
*//***************************************************************************/
int fdma_split_frame(
		struct fdma_split_frame_params *params);

/**************************************************************************//**
@Function	fdma_trim_default_segment_presentation

@Description	Re-size down a previously presented default Data segment
		(head/tail trim).

		Updates the presented data identified with the segment (Does
		not do a replace command). The change will only take place in
		the FDMA side. After this command the FDMA will associate the
		segment with a subset of the original segment of 'size' bytes
		starting from a specified offset in the original segment (the
		new size and offset are the Service Routine parameters).

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length.

@Param[in]	offset - Offset from the previously presented segment
		representing the new start of the segment (head trim).
@Param[in]	size - New Data segment size in bytes.

@Return		None.

@remark		Example: Trim segment to 20 bytes at offset 10.
		The default Data segment represents a 100 bytes at offset 0 in
		the frame (0-99) and the user want to the segment to represent
		only 20 bytes starting at offset 10 (relative to the presented
		segment).
		Parameters:
			- offset - 10 (relative to the presented segment)
			- size - 20

@Cautions	This command may be invoked only for Data segments.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_trim_default_segment_presentation(
		uint16_t offset, uint16_t size);


/**************************************************************************//**
@Function	fdma_modify_default_segment_data

@Description	Modifies data in the default Data segment of the default
		Working Frame (in the FDMA).

		This Service Routine updates the FDMA that certain data in the
		presented segment was modified. The updated data is located in
		the same place the old data was located at in the segment
		presentation in workspace.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	offset - The offset from the previously presented segment
		representing the start point of the modification.
		Must be within the presented segment size.
@Param[in]	size - The Working Frame modified size.

@Return		None.

@remark		Example: Modify 14 bytes. The default Data segment represents a
		100 bytes at offset 0 in the frame (0-99) and the user has
		updated bytes 11-24 (14 bytes) at their original location in the
		segment presentation in workspace.
		Parameters:
			- offset - 11 (relative to the presented segment)
			- size - 14

@Cautions	As part of a workaround to ticket TKT237377 this command closes
		and reopens the segment. (meaning that all segment modifications
		done in workspace but not included in the range of this command
		are lost).
@Cautions	This command may be invoked only on the default Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline void fdma_modify_default_segment_data(
		uint16_t offset,
		uint16_t size);

/**************************************************************************//**
@Function	fdma_modify_default_segment_full_data

@Description	Modifies full data in the default Data segment of the default
		Working Frame (in the FDMA).

		This Service Routine updates the FDMA that all the data in the
		presented segment was modified. The updated data is located in
		the same place the old data was located at in the segment
		presentation in workspace.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address, segment offset, segment size.

@Return		None.

@Cautions	This command may be invoked only on the default Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline void fdma_modify_default_segment_full_data();

#ifdef REV2
/*
@Function	fdma_modify_segment_data

@Description	Modifies data in the Data segment in a Working Frame (in the
		FDMA).

		This Service Routine updates the FDMA that certain data in the
		presented segment was modified. The updated data is located in
		the same place the old data was located at in the segment
		presentation in workspace.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	frame_handle - Working frame handle in which the data is being
		modified.
@Param[in]	seg_handle - Data segment handle (related to the working frame
		handle) in which the data is being modified.
@Param[in]	offset - The offset from the previously presented segment
		representing the start point of the modification.
		Must be within the presented segment size.
@Param[in]	size - The Working Frame modified size.
@Param[in]	from_ws_src - a pointer to the workspace location from which
		the segment data starts.

@Return		None.

@remark		Example: Modify 14 bytes. The default Data segment represents a
		100 bytes at offset 0 in the frame (0-99) and the user has
		updated bytes 11-24 (14 bytes) at their original location in the
		segment presentation in workspace.
		Parameters:
			- offset - 11 (relative to the presented segment)
			- size - 14

@Cautions	This command may be invoked only on the default Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*/
void fdma_modify_segment_data(
		uint8_t frame_handle,
		uint8_t seg_handle,
		uint16_t offset,
		uint16_t size,
		void	 *from_ws_src);
#endif /*REV2*/

/**************************************************************************//**
@Function	fdma_insert_default_segment_data

@Description	Insert new data to the default Working Frame (in the FDMA)
		through the default Data segment .

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length,
		segment address.

@Param[in]	to_offset - Offset from the previously presented segment
		representing where to insert the data.
		Must be within the presented segment size.
@Param[in]	from_ws_src - a pointer to the workspace location from which
		the inserted segment data starts.
@Param[in]	insert_size - Size of the data being inserted to the segment.
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (relevant if
		 \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@remark		This is basically a replace command with
		to_size = 0 (0 bytes are replaced, 'size' bytes are inserted).
@remark		Example: Insert 2 bytes - The default Data
		segment represents a 100 bytes at offset 0 in the frame (0-99),
		and the user want to insert 2 bytes after the 24th byte in the
		segment.
		Parameters:
			- to_offset - 25 (relative to the presented segment)
			- from_ws_address - <workspace address of the 2 bytes>
			- insert_size - 2

@remark		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set, the
		pointer to the workspace start address of the represented
		segment and the number of frame bytes to represent remains the 
		same due to .

@Cautions	As part of a workaround to ticket TKT237377 this command closes
		and reopens the segment. (meaning that all segment modifications
		done in workspace but not included in the range of this command
		are lost).
@Cautions	In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set, The
		Service Routine checks whether there is enough headroom in the
		Workspace before the default segment address to present the
		inserted data (the headroom should be large enough to contain
		the inserted data size).
		In case the headroom is large enough, all the original segment +
		inserted data will be presented, and the segment size will be
		increased by the inserted size.
		In case there is not enough headroom for the inserted size, the
		segment representation will overwrite the old segment
		presentation in workspace. The segment size will remain the
		same.
@Cautions	This command may be invoked only on the default Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline int fdma_insert_default_segment_data(
		uint16_t to_offset,
		void	 *from_ws_src,
		uint16_t insert_size,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_insert_segment_data

@Description	Insert new data to a Working Frame (in the FDMA) through a Data
		segment.

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

@Param[in]	params - A pointer to the insert segment data command
		parameters \ref fdma_insert_segment_data_params.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (relevant if 
		\ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@remark		This is basically a replace command with
		to_size = 0 (0 bytes are replaced, 'size' bytes are inserted).
@remark		Example: Insert 2 bytes - The default Data
		segment represents a 100 bytes at offset 0 in the frame (0-99),
		and the user want to insert 2 bytes after the 24th byte in the
		segment.
		Parameters:
			- to_offset - 25 (relative to the presented segment)
			- from_ws_address - <workspace address of the 2 bytes>
			- insert_size - 2

@Cautions	This command may be invoked only on the Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
@Cautions	Due to Ticket TKT237377 this function must be called with SA 
		flag \ref FDMA_REPLACE_SA_CLOSE_BIT.
*//***************************************************************************/
int fdma_insert_segment_data(
		struct fdma_insert_segment_data_params *params);

/**************************************************************************//**
@Function	fdma_delete_default_segment_data

@Description	Delete data from the default Working Frame (in the FDMA) through
		the default Data segment.

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length,
		segment address.

@Param[in]	to_offset - Offset from the previously presented segment
		representing from where to delete data.
		Must be within the presented segment size.
@Param[in]	delete_target_size - Size of the data being deleted from the
		segment.
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (relevant if 
		\ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@remark		This is basically a replace command with
		to_size = delete_target_size, ws_address = irrelevant (0),
		size = 0 (replacing 'delete_target_size' bytes with 0 bytes =
		deletion).
@remark		Example: Delete 10 bytes. The default Data segment represents
		a 100 bytes at offset 0 in the frame (0-99), and the user want
		to delete 10 bytes after the 24th byte.
		Parameters:
			- to_offset - 25 (relative to the presented segment)
			- delete_target_size - 10

@remark		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set the
		pointer to the workspace start address of the represented
		segment and the number of frame bytes to represent remains the 
		same.

@Cautions	As part of a workaround to ticket TKT237377 this command closes
		and reopens the segment. (meaning that all segment modifications
		done in workspace but not included in the range of this command
		are lost).
@Cautions	This command may be invoked only on the default Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline int fdma_delete_default_segment_data(
		uint16_t to_offset,
		uint16_t delete_target_size,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_delete_segment_data

@Description	Delete data from a Working Frame (in the FDMA) through a Data
		segment.

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

@Param[in]	params - A pointer to the delete segment data command
		parameters \ref fdma_delete_segment_data_params.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (relevant if 
		\ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@remark		This is basically a replace command with
		to_size = delete_target_size, ws_address = irrelevant (0),
		size = 0 (replacing 'delete_target_size' bytes with 0 bytes =
		deletion).
@remark		Example: Delete 10 bytes. The default Data segment represents
		a 100 bytes at offset 0 in the frame (0-99), and the user want
		to delete 10 bytes after the 24th byte.
		Parameters:
			- to_offset - 25 (relative to the presented segment)
			- delete_target_size - 10

@Cautions	This command may be invoked only on Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
@Cautions	Due to Ticket TKT237377 this function must be called with SA 
		flag \ref FDMA_REPLACE_SA_CLOSE_BIT.
*//***************************************************************************/
int fdma_delete_segment_data(
		struct fdma_delete_segment_data_params *params);

/**************************************************************************//**
@Function	fdma_close_default_frame_segment

@Description	Closes the default segment in the default frame.
		Free the workspace memory associated with the segment.
		All segment modifications which were not written to the working
		frame will be lost.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

@Return		None.

@Cautions	This command may be invoked only for Data segments.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline void fdma_close_default_segment(void);

/**************************************************************************//**
@Function	fdma_close_segment

@Description	Closes a segment in the specified frame.
		Free the workspace memory associated with the segment.
		All segment modifications which were not written to the working
		frame will be lost.

@Param[in]	frame_handle - working frame from which to close the segment
@Param[in]	seg_handle - The handle of the segment to be closed.

@Return		None.

@Cautions	This command may be invoked only for Data segments.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
void fdma_close_segment(uint8_t frame_handle, uint8_t seg_handle);

/**************************************************************************//**
@Function	fdma_replace_default_frame_asa_segment_data

@Description	Replace modified data in the Accelerator Specific Annotation
		(ASA) segment associated with the default Working Frame (in the
		FDMA).

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the ASA segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		ASA segment handle.

		Implicitly updated values in Task Defaults:
		ASA segment address, ASA segment length.

		Implicitly updated values in FD - ASA length.

@Param[in]	to_offset - Offset from the beginning of the ASA data
		representing the start point of the replacement specified in
		64B quantities.
@Param[in]	to_size - The number of 64B quantities that are to be replaced
		within the ASA segment.
@Param[in]	from_ws_src - A pointer to the workspace location from which
		the replacement segment data starts.
@Param[in]	from_size - The number of 64B units that will replace the
		specified portion of the ASA segment.
@Param[in]	ws_dst_rs - A pointer to the location in workspace 
		for the represented frame segment (relevant if \ref
		FDMA_REPLACE_SA_REPRESENT_BIT) flag is set).
@Param[in]	size_rs - Number of frame bytes to represent in 64B portions.
		Must be greater than 0.
		Relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set.
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink
		In REV1 \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is not supported
		due to ERR008620.

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_ASA_SEG - Unable to fulfill 
		specified ASA segment presentation size (relevant if 
		\ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
		This return value is caused since the requested presentation 
		exceeded frame ASA end.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
@Cautions	Due to Ticket TKT237377 this function must be called with SA 
		flag \ref FDMA_REPLACE_SA_CLOSE_BIT.
*//***************************************************************************/
int fdma_replace_default_asa_segment_data(
		uint16_t to_offset,
		uint16_t to_size,
		void	 *from_ws_src,
		uint16_t from_size,
		void	 *ws_dst_rs,
		uint16_t size_rs,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_replace_default_pta_segment_data

@Description	Replace modified data in the Pass-Through Annotation (PTA)
		segment associated with the default Working Frame (in the FDMA).

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the PTA segment data between the
		Task Workspace and the FDMA.

		The entire PTA annotation (32 or 64 bytes) is replaced when this
		command is invoked.

		Usage examples:
		- Passing GRO information to GPP.

		Implicit input parameters in Task Defaults: frame handle,
		PTA segment handle, PTA segment address.

		Implicitly updated values in Task Defaults: PTA segment address.

		Implicitly updated values in FD - PTA length.

@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink
		In REV1 \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is not supported
		due to ERR008620.
@Param[in]	from_ws_src - a pointer to the workspace location from which
		the replacement segment data starts.
@Param[in]	ws_dst_rs - A pointer to the location in workspace 
		for the represented frame segment (relevant if \ref
		FDMA_REPLACE_SA_REPRESENT_BIT flag is set). In case of
		representing the PTA, always represent the full PTA (64 bytes).
@Param[in]	size_type - Replacing segment size type of the PTA
		(\ref fdma_pta_size_type).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		EIO - Unable to present PTA segment (no PTA segment in working
		frame)(relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is
		set).

@remark		The length of the represented PTA can be read directly from the
		FD.

@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int fdma_replace_default_pta_segment_data(
		uint32_t flags,
		void	 *from_ws_src,
		void	 *ws_dst_rs,
		enum fdma_pta_size_type size_type);

/**************************************************************************//**
@Function	fdma_calculate_default_frame_checksum

@Description	Computes the gross checksum of the default Working
		Frame.

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
inline void fdma_calculate_default_frame_checksum(
		uint16_t offset,
		uint16_t size,
		uint16_t *checksum);

/**************************************************************************//**
@Function	get_default_amq_attributes

@Description	Getter for AMQ (ICID, PL, VA, BDI) default attributes. The 
		default AMQ attributes are located in the Additional Dequeue 
		Context.

@Param[in]	amq - Returned Additional Dequeue Context AMQ attributes.

@Return		None.
*//***************************************************************************/
void get_default_amq_attributes(
	struct fdma_amq *amq);

/**************************************************************************//**
@Function	set_default_amq_attributes

@Description	setter for AMQ (ICID, PL, VA, BDI) default attributes. The 
		default AMQ attributes are located in the Additional Dequeue 
		Context.

@Param[in]	amq - AMQ attributes to set in the Additional Dequeue Context.

@Return		None.
*//***************************************************************************/
void set_default_amq_attributes(
	struct fdma_amq *amq);

#include "fdma_inline.h"

/**************************************************************************//**
@Function	fdma_replace_default_segment_data

@Description	Replace modified data in the default Data segment in the default
		Working Frame (in the FDMA).

		This Service Routine can replace any data size in the segment
		with any new data size. the new data will be taken from
		from_ws_src pointer parameter.

		In case \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set this
		Service Routine synchronizes the segment data between the
		Task Workspace and the FDMA.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle.

		Implicitly updated values in task defaults: segment length,
		segment address.

@Param[in]	to_offset - The offset from the previously presented segment
		representing the start point of the replacement.
		Must be within the presented segment size.
@Param[in]	to_size - The Working Frame replaced size.
@Param[in]	from_ws_src - a pointer to the workspace location from which
		the replacement segment data starts.
@Param[in]	from_size - The replacing segment size.
@Param[in]	ws_dst_rs - A pointer to the location in workspace for the
		represented frame segment (relevant if
		\ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
@Param[in]	size_rs - Number of frame bytes to represent in the segment.
		Must be greater than 0.
		Relevant if \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
@Param[in]	flags - \link FDMA_Replace_Flags replace working frame
		segment flags. \endlink

@Return		0 or positive value on success. Negative value on error.

@Retval		0 - Success.
@Retval		::FDMA_STATUS_UNABLE_PRES_DATA_SEG - Unable to fulfill 
		specified data segment presentation size (relevant if
		 \ref FDMA_REPLACE_SA_REPRESENT_BIT flag is set).
		This return value is caused since the requested presentation 
		exceeded frame data end.

@remark		Example: Modify 14 bytes + insert 2 bytes. The default Data
		segment represents a 100 bytes at offset 0 in the frame (0-99)
		and the user want to replace bytes 11-24 (14 bytes) with new 16
		bytes (14 updated + additional 2).
		Parameters:
			- to_offset - 11 (relative to the presented segment)
			- to_size - 14
			- from_ws_address - <workspace address of the 16 bytes>
			- from_size - 16

@Cautions	As part of a workaround to ticket TKT237377 this command closes
		and reopens the segment. (meaning that all segment modifications
		done in workspace but not included in the range of this command
		are lost).
@Cautions	This command may be invoked only on the default Data segment.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline int fdma_replace_default_segment_data(
		uint16_t to_offset,
		uint16_t to_size,
		void	 *from_ws_src,
		uint16_t from_size,
		void	 *ws_dst_rs,
		uint16_t size_rs,
		uint32_t flags);

/**************************************************************************//**
@Function	fdma_copy_data

@Description	Copy a workspace/Shared memory data to another location within
		the workspace/Shared memory.

@Param[in]	copy_size - Number of bytes to copy from source to destination.
@Param[in]	flags - Please refer to
		\link FDMA_Copy_Flags Copy command flags \endlink.
@Param[in]	src - A pointer to the location in the workspace/AIOP Shared
		memory of the source data (limited to 20 bits). 
		(SRAM address is relative to SRAM start.
		Workspace address is relative to Workspace start).
@Param[in]	dst - A pointer to the location in the workspace/AIOP Shared
		memory to store the copied data (limited to 20 bits).
		(SRAM address is relative to SRAM start.
		Workspace address is relative to Workspace start).

@Return		None.

@remark		SRAM to SRAM copy of size 0xffc0 is not supported for REV1.

@Cautions	If source and destination regions overlap then this is a
		destructive copy.
@Cautions	This function may result in a fatal error.
@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
inline void fdma_copy_data(
		uint16_t copy_size,
		uint32_t flags,
		void *src,
		void *dst);

/** @} */ /* end of FDMA_Functions */
/** @} */ /* end of FSL_AIOP_FDMA */
/** @} */ /* end of ACCEL */


#endif /* __FSL_FDMA_H */
