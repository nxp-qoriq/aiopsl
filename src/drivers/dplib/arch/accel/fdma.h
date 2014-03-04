/**************************************************************************//**
@File		fdma.h

@Description	This file contains the AIOP SW FDMA internal API
*//***************************************************************************/


#ifndef __FDMA_H_
#define __FDMA_H_


#include "general.h"
#include "dplib\fsl_fdma.h"

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
	 asapa_asaps) & PRC_SR_MASK) << (FDMA_SR_OFFSET-PRC_SR_BIT_OFFSET))
	/** FDMA Store command output flags mask. */
#define FDMA_STORE_CMD_OUT_FLAGS_MASK		0x0000FF00
	/** FDMA Store command output flags offset. */
#define FDMA_STORE_CMD_OUT_ICID_OFFSET		2
	/** Default Segment headroom size. */
#define DEFAULT_SEGMENT_HEADOOM_SIZE	128
	/** Default Segment size. */
#define DEFAULT_SEGMENT_SIZE		256

/* @} end of group FDMA_Commands_Definitions */

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

/* @} end of group FDMA_Int_Init_ModeBits */


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
	/** FDMA Copy data command code */
#define FDMA_COPY_CMD			0x00000040
	/** FDMA Acquire buffer command code */
#define FDMA_ACQUIRE_BUFFER_CMD		0x00000072
	/** FDMA Release buffer command code */
#define FDMA_RELEASE_BUFFER_CMD		0x00000073

/* @} end of group FDMA_Commands_IDs */


/**************************************************************************//**
@Group		FDMA_Commands_Args

@Description	FDMA Command Arguments macros.

@{
*//***************************************************************************/
	/** Getter for PRC frame handle for FDMA commands */
#define FDMA_GET_PRC_FRAME_HANDLE(_handles)				\
	((_handles << (16-PRC_FRAME_HANDLE_BIT_OFFSET)) &		\
	(PRC_FRAME_HANDLE_MASK << (16-PRC_FRAME_HANDLE_BIT_OFFSET)))

	/** Getter for PRC segment handle for FDMA commands */
#define FDMA_GET_PRC_SEGMENT_HANDLE(_handles)				\
	((_handles & PRC_SEGMENT_HANDLE_MASK) << 24)

	/** FDMA Initial presentation command arg1 */
#define FDMA_INIT_CMD_ARG1(_fd_addr, _flags)				\
	(uint32_t)((_fd_addr << 16) | _flags | FDMA_INIT_CMD)

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
	(((uint16_t)(uint32_t)_pta_address) & PRC_PTAPA_MASK) |		\
	(_asa_offset & PRC_ASAPO_MASK))

	/** FDMA Initial presentation explicit command arg4 */
#define FDMA_INIT_EXP_CMD_ARG4(_asa_address, _asa_size)			\
	(uint32_t)((((uint16_t)(uint32_t)_asa_address) & PRC_ASAPA_MASK) |\
	(_asa_size & PRC_ASAPS_MASK))
	/** FDMA Initial presentation explicit AMQ command arg4 */
#define FDMA_INIT_EXP_AMQ_CMD_ARG4(_bdi_icid ,_asa_address, _asa_size)	\
	(uint32_t)(((uint32_t)_bdi_icid << 16) |			\
	(((uint16_t)(uint32_t)_asa_address) & PRC_ASAPA_MASK) |		\
	(_asa_size & PRC_ASAPS_MASK))

	/** FDMA Present command arg1 */
#define FDMA_PRESENT_CMD_ARG1(_handles, _flags)				\
	(uint32_t)((FDMA_GET_PRC_FRAME_HANDLE(_handles)) |		\
	_flags | FDMA_PRESENT_CMD)

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
#define FDMA_EXTEND_CMD_ARG1(_handles)					\
	(uint32_t)((FDMA_GET_PRC_SEGMENT_HANDLE(_handles)) |		\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
	FDMA_EXTEND_CMD)

	/** FDMA Extend presentation command arg1 */
#define FDMA_EXTEND_CMD_ASA_ARG1(_handles)				\
	(uint32_t)((FDMA_ASA_SEG_HANDLE << 24) |			\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
	FDMA_EXTEND_CMD)

	/** FDMA Extend presentation command arg2 */
#define FDMA_EXTEND_CMD_ARG2(_seg_addr, _extend_size)			\
	(uint32_t)((_seg_addr << 16) | _extend_size)

	/**< FDMA Store default working frame command arg1 */
#define FDMA_STORE_DEFAULT_CMD_ARG1(_spid, _handles)			\
	(uint32_t)((_spid << 24) |					\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
	FDMA_STORE_CMD)

	/**< FDMA Store working frame command arg1 */
#define FDMA_STORE_CMD_ARG1(_spid, _handle)				\
	(uint32_t)((_spid << 24) | (_handle << 16) | FDMA_STORE_CMD)

	/** FDMA Enqueue working frame command arg1 */
#define FDMA_ENQUEUE_WF_ARG1(_spid, _handles, _flags)			\
	(uint32_t)((_spid << 24) |					\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
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
#define FDMA_DISCARD_ARG1_WF(_handles, _flags)				\
	(uint32_t)((FDMA_GET_PRC_FRAME_HANDLE(_handles)) |		\
	_flags | FDMA_DISCARD_FRAME_CMD)

	/** FDMA Discard frame command arg1 */
#define FDMA_DISCARD_ARG1_FRAME(_frame, _flags)			\
	(uint32_t)((_frame << 16) | _flags | FDMA_DISCARD_FRAME_CMD)

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
#define FDMA_TRIM_CMD_ARG1(_handles)					\
	(uint32_t)((FDMA_GET_PRC_SEGMENT_HANDLE(_handles)) |		\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
	FDMA_TRIM_CMD)

	/** FDMA Trim working frame segment command arg2 */
#define FDMA_TRIM_CMD_ARG2(_offset, _size)				\
	(uint32_t)((_offset << 16) | _size)

	/** FDMA Replace working frame segment command arg1 */
#define FDMA_REPLACE_CMD_ARG1(_handles, _flags)				\
	(uint32_t)((FDMA_GET_PRC_SEGMENT_HANDLE(_handles)) |		\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
	(_flags) | FDMA_REPLACE_CMD)

/** FDMA explicit Replace working frame segment command arg1 */
#define FDMA_REPLACE_EXP_CMD_ARG1(_seg_handle, _frame_handle, _flags)	\
	(uint32_t)((_seg_handle << 24) | (_frame_handle << 16) |	\
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
#define FDMA_CLOSE_SEG_CMD_ARG1(_handles, _seg_handle)			\
	(uint32_t)((_seg_handle << 24) |				\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
	(FDMA_REPLACE_SA_CLOSE_BIT) | FDMA_REPLACE_CMD)

	/** FDMA Replace working frame segment command arg1 */
#define FDMA_REPLACE_PTA_ASA_CMD_ARG1(_seg_handle, _handles, _flags)	\
	(uint32_t)((_seg_handle << 24) |				\
	(FDMA_GET_PRC_FRAME_HANDLE(_handles)) |				\
	(_flags) | FDMA_REPLACE_CMD)

	/** FDMA Checksum working frame command arg1 */
#define FDMA_CKS_CMD_ARG1(_frame_handle)				\
	(uint32_t)((((uint32_t)_frame_handle) << 16) | FDMA_CKS_CMD)
	/**< FDMA Checksum  working frame segment command arg2 */
#define FDMA_CKS_CMD_ARG2(_offset, _size)				\
	(uint32_t)((_offset << 16) | _size)

	/** FDMA Copy data command arg1 */
#define FDMA_COPY_CMD_ARG1(_copy_size, _flags)				\
	(uint32_t)((((uint32_t)_copy_size) << 16) | _flags | FDMA_COPY_CMD)

	/** FDMA Acquire buffer command arg1 */
#define FDMA_ACQUIRE_CMD_ARG1(_icid, _flags)				\
	(uint32_t)((((uint32_t)_icid) << 16) | _flags | FDMA_ACQUIRE_BUFFER_CMD)
	/** FDMA Acquire buffer command arg2 */
#define FDMA_ACQUIRE_CMD_ARG2(_dst, _bpid)				\
	(uint32_t)((((uint32_t)_dst) << 16) | _bpid)

	/** FDMA Release buffer command arg1 */
#define FDMA_RELEASE_CMD_ARG1(_icid, _flags)				\
	(uint32_t)((((uint32_t)_icid) << 16) | _flags | FDMA_RELEASE_BUFFER_CMD)

/* @} end of group FDMA_Commands_Args */

	/** Getter for AMQ (ICID, PL, VA, BDI) default attributes */
/* Todo - enable inline when inline works correctly+move definition to .h file*/
/*inline*/ void get_default_amq_attributes(
		struct fdma_isolation_attributes *amq);

	/** Setter for AMQ (ICID, PL, VA, BDI) default attributes */
/* Todo - enable inline when inline works correctly+move definition to .h file*/
/*inline*/ void set_default_amq_attributes(
		struct fdma_isolation_attributes *amq);


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
	/** Macro to set the default frame PTA address in workspace from the
	 * presentation context */
#define PRC_SET_PTA_ADDRESS(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->ptapa_asapo =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo & ~PRC_PTAPA_MASK) |			\
		((uint16_t)_val & PRC_PTAPA_MASK))
	/** Macro to set the default frame ASA offset from the presentation
	 * context */
#define PRC_SET_ASA_OFFSET(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->ptapa_asapo =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo & ~PRC_ASAPO_MASK) |			\
		((uint16_t)_val & PRC_ASAPO_MASK))
	/** Macro to set the default frame ASA address in workspace from the
	 * presentation context */
#define PRC_SET_ASA_ADDRESS(_val)					\
	((((struct presentation_context *)HWC_PRC_ADDRESS)->asapa_asaps =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps & ~PRC_ASAPA_MASK) |			\
		((uint16_t)_val & PRC_ASAPA_MASK))
	/** Macro to set Segment reference bit in workspace from the
	 * presentation context */
#define PRC_SET_SR_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->asapa_asaps =\
		((((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps & ~PRC_SR_MASK) | PRC_SR_MASK))
	/** Macro to reset Segment reference bit in workspace from the
	 * presentation context */
#define PRC_RESET_SR_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->asapa_asaps =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps & ~PRC_SR_MASK))
	/** Macro to set No-Data-Segment bit in workspace from the
	 * presentation context */
#define PRC_SET_NDS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->asapa_asaps =\
		((((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps & ~PRC_NDS_MASK) | PRC_NDS_MASK))
	/** Macro to reset No-Data-Segment bit in workspace from the
	 * presentation context */
#define PRC_RESET_NDS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->asapa_asaps =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps & ~PRC_NDS_MASK))
	/** Macro to set No-PTA-Segment bit in workspace from the
	 * presentation context */
#if NAS_NPS_ENABLE
#define PRC_SET_NPS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->ptapa_asapo =\
		((((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo & ~PRC_NPS_MASK) | PRC_NPS_MASK))
	/** Macro to reset No-PTA-Segment bit in workspace from the
	 * presentation context */
#define PRC_RESET_NPS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->ptapa_asapo =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo & ~PRC_NPS_MASK))
	/** Macro to set No-ASA-Segment bit in workspace from the
	 * presentation context */
#define PRC_SET_NAS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->ptapa_asapo =\
		((((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo & ~PRC_NAS_MASK) | PRC_NAS_MASK))
	/** Macro to reset No-ASA-Segment bit in workspace from the
	 * presentation context */
#define PRC_RESET_NAS_BIT()						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->ptapa_asapo =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo & ~PRC_NAS_MASK))
#endif /*NAS_NPS_ENABLE*/
	/** Macro to set the default frame ASA size in workspace from the
	 * presentation context */
#define PRC_SET_ASA_SIZE(_val)						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->asapa_asaps =\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps & ~PRC_ASAPS_MASK) |			\
		((uint16_t)_val & PRC_ASAPS_MASK))
	/** Macro to set the default frame handle + default segment handles
	 * from the presentation context */
#define PRC_SET_HANDLES(_val)						\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->handles =	\
		(uint8_t)_val)
	/** Macro to set the default frame handle from the presentation
	 * context */
#define PRC_SET_FRAME_HANDLE(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->handles =	\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		handles & ~PRC_FRAME_HANDLE_MASK) |			\
		(((uint8_t)_val << PRC_FRAME_HANDLE_BIT_OFFSET) &	\
			PRC_FRAME_HANDLE_MASK))
	/** Macro to set the default segment handle from the presentation
	 * context */
#define PRC_SET_SEGMENT_HANDLE(_val)					\
	(((struct presentation_context *)HWC_PRC_ADDRESS)->handles =	\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		handles & ~PRC_SEGMENT_HANDLE_MASK) |			\
		((uint8_t)_val & PRC_SEGMENT_HANDLE_MASK))
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


/** @} */ /* end of AIOP_PRC_Setters */

/** @}*/ /* end of group AIOP_PRC_Definitions */


/**************************************************************************//**
@Function	fdma_present_default_frame_without_segments

@Description	Initial presentation of a default frame into the task workspace
		without any segments (Data, ASA, PTA).

		Implicit input parameters in Task Defaults: AMQ attributes (PL,
		VA, BDI, ICID), FD address.

		Implicitly updated values in Task Defaults: frame handle, NDS
		bit, ASA size (0), PTA address(\ref PRC_PTA_NOT_LOADED_ADDRESS).

@Return		Status - Success or Failure (e.g. DMA error. (\ref
		FDMA_PRESENT_FRAME_ERRORS)).

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_present_default_frame_without_segments(void);

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
@Param[out]	frame_handle - A handle to the opened working frame.

@Return		Status - Success or Failure (e.g. DMA error. (\ref
		FDMA_PRESENT_FRAME_ERRORS)).

@Cautions	In this Service Routine the task yields.
*//***************************************************************************/
int32_t fdma_present_frame_without_segments(
		struct ldpaa_fd *fd,
		uint8_t *frame_handle);

#endif /* __FDMA_H_ */
