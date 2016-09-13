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
@File		fsl_general.h

@Description	This file contains AIOP SW external general definitions.

*//***************************************************************************/


#ifndef __FSL_GENERAL_H_
#define __FSL_GENERAL_H_

#include "fsl_endian.h"
#include "fsl_ldpaa.h"
#include "null.h"
/*#include "dplib/fsl_aiop_parser.h"*/

/**************************************************************************//**
 @Group		AIOP_GENERAL AIOP General

 @Description	AIOP General macros and functions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_General_Definitions AIOP SL General Definitions

 @Description	AIOP General Definitions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_Return_Status  AIOP Return Status

 @Description	AIOP function return status definitions

 @{
*//***************************************************************************/

	/** Success return status*/
#define SUCCESS 0

	/** The base ID for all the SW modules return status */
#define BASE_SW_MODULES_STATUS_ID 0x0F000000
	/** Header Manipulation module ID */
#define HM_MODULE_STATUS_ID	(BASE_SW_MODULES_STATUS_ID + 0x01000000)
/** IP Reassembly module ID */
#define IPR_MODULE_STATUS_ID	(BASE_SW_MODULES_STATUS_ID + 0x02000000)
/** IP Fragmentation module ID */
#define IPF_MODULE_STATUS_ID	(BASE_SW_MODULES_STATUS_ID + 0x03000000)
/** TCP GSO module ID */
#define TCP_GSO_MODULE_STATUS_ID (BASE_SW_MODULES_STATUS_ID + 0x04000000)
/** TCP GRO module ID */
#define TCP_GRO_MODULE_STATUS_ID (BASE_SW_MODULES_STATUS_ID + 0x05000000)
/** DPNI driver module ID */
#define DPNI_DRV_MODULE_STATUS_ID (BASE_SW_MODULES_STATUS_ID + 0x06000000)
/** CAPWAP Reassembly module ID */
#define CWAPR_MODULE_STATUS_ID 	(BASE_SW_MODULES_STATUS_ID + 0x07000000)
/** CAPWAP Fragmentation module ID */
#define CWAPF_MODULE_STATUS_ID 	(BASE_SW_MODULES_STATUS_ID + 0x08000000)
/** CAPWAP DTLS module ID */
#define CWAP_DTLS_MODULE_STATUS_ID	(BASE_SW_MODULES_STATUS_ID + 0x09000000)

/** @} */ /* end of AIOP_Return_Status */

/**************************************************************************//**
 @Group		ReadDataExt Read external data bits definitions

 @Description	Bit definitions for choosing what hardware
		engine should be used for reading data from external memory
		READ_DATA_USING_FDMA and READ_DATA_USING_CDMA
		cannot be used at the same time

 @{
 *//***************************************************************************/
	/** Fetch data using FDMA DMA Data */
#define READ_DATA_USING_FDMA		0x00000001
	/** Fetch data using CDMA with cache disabled */
#define READ_DATA_USING_CDMA		0x00000002

/** @} end of group ReadDataExt */

/**************************************************************************//**
 @Group		AIOP_HWC_Definitions AIOP HWC Definitions

 @Description	Hardware Context (HWC) Definitions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_HWC_General_Definitions  AIOP HWC General Definitions

 @Description	Hardware Context (HWC) General Definitions

 @{
*//***************************************************************************/

	/** Hardware Context size */
#define HWC_SIZE		0x100
	/** Size of the default working frame FD */
#define HWC_FD_SIZE		0x20
	/** Additional Dequeue Context size */
#define HWC_ADC_SIZE		0x10
	/** Presentation Context size */
#define HWC_PRC_SIZE		0x10
	/** Address of Storage Profile ID of the default working frame */
#define HWC_SPID_ADDRESS	0x07
	/** Address for passing parameters to accelerators */
#define HWC_ACC_IN_ADDRESS	0x20
	/** Address for passing parameters to accelerators */
#define HWC_ACC_IN_ADDRESS2	0x24
	/** Address for passing parameters to accelerators */
#define HWC_ACC_IN_ADDRESS3	0x28
	/** Address for passing parameters to accelerators */
#define HWC_ACC_IN_ADDRESS4	0x2C
	/** Address for reading results from accelerators (1st register) */
#define HWC_ACC_OUT_ADDRESS	0x30
	/** Address for reading results from accelerators (2nd register) */
#define HWC_ACC_OUT_ADDRESS2	0x34
	/** Address for reading reserved 1 from hardware accelerator context*/
#define HWC_ACC_RESERVED1	0x38
	/** Address for reading reserved 2 from hardware accelerator context*/
#define HWC_ACC_RESERVED2	0x3C
	/** Address of Additional Dequeue Context */
#define HWC_ADC_ADDRESS		0x40
	/** Address of Presentation Context */
#define HWC_PRC_ADDRESS		0x50
	/** Address of Default working frame FD */
#define HWC_FD_ADDRESS		0x60
	/** Address of parse results */
#define HWC_PARSE_RES_ADDRESS	0x80

	/** Getter for the Storage Profile ID of the default working frame */
#define GET_DEFAULT_SPID() 	((uint8_t)(*((uint8_t *)HWC_SPID_ADDRESS)))
	/** Setter for the Storage Profile ID of the default working frame.
	 * _spid - new spid value. */
#define SET_DEFAULT_SPID(_spid) (*((uint8_t *)HWC_SPID_ADDRESS)) = _spid;

/** @} */ /* end of AIOP_HWC_General_Definitions */

/**************************************************************************//**
 @Group		AIOP_ADC_Definitions  AIOP ADC Definitions

 @Description	Additional Dequeue Context (ADC) Definitions

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	Frame Queue Context structure

		8 byte opaque Frame Queue Context delivered from the QMAN in the
		dequeue responses from this FQ.
		At DCP portals, if FF = 0 this carries the return or output FQID
		for an accelerator, and possibly one or two extra sets of access
		management qualifiers (AMQ).
		Optionally, if FF = 1, this field may carry other data specific
		to the accelerator.

*//***************************************************************************/
struct frame_queue_context {
		/** 
		 * - bits<0>: VA_3. Valid if AUC = 3.
		 * - bits<1>: VA_2. Valid if AUC is non-zero.
		 * - bits<6-7>: AUC - AMQ usage control:
		 * 	00 = AMQ_2 and AMQ_3 are not valid. 
		 * 	     AMQ_1 is used for all needed authorization (input,
		 * 	     output, and control).
		 *	01 = AMQ_2 is valid. AMQ_1 used for input and control, 
		 *	     AMQ_2 used for output.
		 *	10 = AMQ_2 is valid. AMQ_1 used for input, AMQ_2 used 
		 *	     for control and output.
		 *	11 = AMQ_2 and AMQ_3 are valid. AMQ_1 used for input, 
		 *	AMQ_2 used for control, AMQ_3 used for output.
		 * - bits<8-31>: fqid - Return/Output FQID for an accelerator.
		 * */
	volatile uint32_t fqid_auc_va_2_3;
		/** 
		 * - bits<0>: PL_3. Valid if AUC = 3.
		 * - bits<1-15>: ICID_3. Valid if AUC = 3.
		 * */
	volatile uint16_t pl_3_icid_3;
		/** 
		 * - bits<0>: PL_2. Valid if AUC is non-zero.
		 * - bits<1-15>: ICID_2. Valid if AUC is non-zero.
		 * */
	volatile uint16_t pl_2_icid_2;
};

/**************************************************************************//**
@Description	Additional Dequeue Context (ADC) structure.

*//***************************************************************************/
struct additional_dequeue_context {

		/** Frame Queue Context as received from QMan
		 * via the AIOP DCP. */
	volatile struct frame_queue_context fqd_ctx;
		/**
		- bits<0-7>: AIOP Channel that this FD arrived on.
		- bits<8-31>: The QMan Frame Queue ID that
		this frame arrived was dequeued from. */
	volatile uint32_t channel_fqid;
		/**
		- bits<0>: Privilege Level.
		- bits<1-15>: The Isolation Context ID that the dequeued frame
		belongs to. */
	volatile uint16_t pl_icid;
		/**
		- bits<1-3>: QMan Work Queue ID that this FD was dequeued from.
		- bits<5-7>: Entry Priority. Indicates the priority of the
		presented FD. */
	volatile uint8_t wqid_pri;
		/**
		 * - bits<1-3>: FD Source. Coded value indicating the source of
		 * the presented FD:\n
		 *	- 0 QMan\n
		 *	- 1 TMan\n
		 *	- 2 CMGW Host Command\n
		 *	- 3 CSCN Message\n
		 *	- 4 BPSCN Message\n
		 *	- 5-7 Reserved.
		 *	.
		 * - bits<5> : VA- Virtual Address (configured in Frame Queue).
		 * - bits<6> : FCF- FQD_CTX_FMT or Dequeue Response FQD Context.
		 * Format. Indicates the format of the received FQD_CTX field.
		 * - bits<7>  : BDI- Bypass DPAA Resource Isolation.
		 * .
		 * */
	volatile uint8_t fdsrc_va_fca_bdi;
};

/* Additional Dequeue Context (ADC) Masks */
	/** AIOP Channel mask */
#define ADC_CHANNEL_MASK	0xFF000000
	/** QMan Frame Queue ID mask */
#define ADC_FQID_MASK		0x00FFFFFF
	/** Privilege Level mask */
#define ADC_PL_MASK		0x8000
	/** Isolation Context ID mask */
#define ADC_ICID_MASK		0x7FFF
	/** QMan Work Queue ID mask */
#define ADC_WQID_MASK		0x70
	/** Entry Priority mask */
#define ADC_PRI_MASK		0x07
	/** FD source mask */
#define ADC_FDSRC_MASK		0x70
	/** Virtual Address mask */
#define ADC_VA_MASK		0x04
	/** FQD_CTX format mask */
#define ADC_FCF_MASK		0x02
	/** Bypass DPAA Resource Isolation mask */
#define ADC_BDI_MASK		0x01

/* Additional Dequeue Context (ADC) Offsets */
	/** ADC fqd_ctx offset */
#define ADC_FQD_CTX_OFFSET	0x0
	/** ADC channel_fqid offset */
#define ADC_CHANNEL_FQID_OFFSET	0x8
	/** ADC pl_icid offset */
#define ADC_PL_ICID_OFFSET	0xC
	/** ADC wqid_pri offset */
#define ADC_WQID_PRI_OFFSET	0xE
	/** ADC fdsrc_va_fca_bdi offset */
#define ADC_FDSRC_VA_FCA_BDI_OFFSET	0xF


/**************************************************************************//**
 @Group		AIOP_ADC_Getters  AIOP ADC Getters

 @Description	Additional Dequeue Context (ADC) Getters

 @{
*//***************************************************************************/

/** Macro to get ICID field */
#define ADC_GET_ICID()							\
	(LH_SWAP_MASK(0, (HWC_ADC_ADDRESS + ADC_PL_ICID_OFFSET),	\
	       ADC_ICID_MASK))

/** @} */ /* end of AIOP_ADC_Getters */

/** @} */ /* end of AIOP_ADC_Definitions */


/**************************************************************************//**
 @Group		AIOP_PRC_Definitions  AIOP PRC Definitions

 @Description	Presentation Context (PRC) Definitions

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	Presentation Context (PRC) structure.

*//***************************************************************************/
struct presentation_context {

		/** Entry point opaque parameter value. */
	volatile uint32_t param;
		/** Segment presentation address. */
	volatile uint16_t seg_address;
		/** Segment actual size. */
	volatile uint16_t seg_length;
		/** Segment Presentation Offset value. */
	volatile uint16_t seg_offset;
		/**
		- bits<7>   : No Data segment (NDS).
		- bits<6>   : Entry Point Segment Reference (SR) bit. */
	volatile uint8_t  sr_nds;
		/** Segment Handle. */
	volatile uint8_t seg_handle;
		/**
		- bits<0>  : OSM Entry Point Source value.
		- bits<1>  : OSM Entry Point Execution Phase value.
		- bits<2-3>: OSM Entry Point Select value.
		- bits<5-7>: OSM Entry Point Order Scope Range Mask value. */
	volatile uint8_t osrc_oep_osel_osrm;
		/** Working Frame Handle. */
	volatile uint8_t frame_handle;
		/**
		- bits<15>: Initial Scope Value */
	volatile uint16_t  isv;
};

/* Presentation Context (PRC) Macros */
#define PRC_SR_MASK		0x02
	/** No Data Segment (NDS) bit mask.
	 * If set - do not present Data segment.
	 * Otherwise - present data segment */
#define PRC_NDS_MASK		0x01
	/** OSM Entry point source value mask*/
#define PRC_OSRC_MASK		0x80
	/** OSM Entry Point Execution Phase value mask */
#define PRC_OEP_MASK		0x40
	/** OSM Entry Point Select value mask */
#define PRC_OSEL_MASK		0x30
	/** OSM Entry Point Order Scope Range value mask */
#define PRC_OSRM_MASK		0x07
	/** PRC Frame handle offset */
#define PRC_SR_BIT_OFFSET	0x1
	/** No Data Segment bit offset */
#define PRC_NDS_BIT_OFFSET	0x0
	/** OSM Entry Point source value offset */
#define PRC_OSRC_BIT_OFFSET	0x7
	/** OSM Entry Point Execution Phase value offset */
#define PRC_OEP_BIT_OFFSET	0x6
	/** OSM Entry Point Select value offset */
#define PRC_OSEL_BIT_OFFSET	0x4

	/** PTA address when PTA is not loaded/not intended to be loaded to the
	 * working frame */
#define PTA_NOT_LOADED_ADDRESS	0xFFC0

/**************************************************************************//**
 @Group		AIOP_PRC_Getters  AIOP PRC Getters

 @Description	Presentation Context (PRC) Getters

 @{
*//***************************************************************************/
	/** Macro to get the task entry point parameter value from the
	 * presentation context */
#define PRC_GET_PARAMETER()						\
	((uint32_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->param))
	/** Macro to get the default segment address from the presentation
	 * context */
#define PRC_GET_SEGMENT_ADDRESS()					\
	((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		seg_address))
	/** Macro to get the default segment length from the presentation
	 * context */
#define PRC_GET_SEGMENT_LENGTH()					\
	((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		seg_length))
	/** Macro to get the default segment offset from the presentation
	 * context */
#define PRC_GET_SEGMENT_OFFSET()					\
	((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		seg_offset))
	/** Macro to get the Segment Reference bit from the presentation
	 * context */
#define PRC_GET_SR_BIT()						\
	(((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		sr_nds) & PRC_SR_MASK) >> PRC_SR_BIT_OFFSET)
	/** Macro to get the No-Data-Segment bit from the presentation
	 * context */
#define PRC_GET_NDS_BIT()						\
	(((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		sr_nds) & PRC_NDS_MASK) >> PRC_NDS_BIT_OFFSET)
	/** Macro to get the default frame handle from the presentation
	 * context */
#define PRC_GET_FRAME_HANDLE()						\
	((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		frame_handle))
	/** Macro to get the default segment handle from the presentation
	 * context */
#define PRC_GET_SEGMENT_HANDLE()					\
	((uint8_t)							\
		(((struct presentation_context *)HWC_PRC_ADDRESS)->seg_handle))
	/** Macro to get the OSM Entry Point Source value from the presentation
	 * context */
#define PRC_GET_OSM_SOURCE_VALUE()					\
	((((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)	\
		->osrc_oep_osel_osrm)) & PRC_OSRC_MASK) ? 1 : 0)
	/** Macro to get the OSM Entry Point Execution Phase value from the
	 * presentation context */
#define PRC_GET_OSM_EXECUTION_PHASE_VALUE()				\
	((((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)	\
		->osrc_oep_osel_osrm)) & PRC_OEP_MASK) ? 1 : 0)
	/** Macro to get the OSM Entry Point Select value from the
	 * presentation context */
#define PRC_GET_OSM_SELECT_VALUE()					\
	((((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)	\
		->osrc_oep_osel_osrm)) & PRC_OSEL_MASK) >> PRC_OSEL_BIT_OFFSET)
	/** Macro to get the OSM Entry Point Order Scope Range Mask value from
	 * the presentation context */
#define PRC_GET_OSM_ORDER_SCOPE_RANGE_MASK_VALUE()			\
	(((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)	\
		->osrc_oep_osel_osrm)) & PRC_OSRM_MASK)
	/** Macro to get the Initial Scope Value */
#define PRC_GET_ISV_VALUE()						\
	((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->isv))

/** @} */ /* end of AIOP_PRC_Getters */


/** @} */ /* end of AIOP_PRC_Definitions */

/**************************************************************************
@Description	Hardware Context (HWC) structure.

*************************************************************************
struct hardware_context {

	uint8_t reserved0[32];
		*< Reserved by the Layerscape Datapath architecture for
		 * future use.
	uint32_t accel_hw_context_param[4];
		*< Accelerator command parameter words.
	uint32_t accel_hw_context_result[2];
		*< Accelerator command result words.
	uint32_t reserved1[2];
		*< Accelerator command reserved words.
	struct additional_dequeue_context adc;
		*< Additional Dequeue Context (ADC) structure.
	struct presentation_context prc;
		*< Presentation Context (PRC) structure
	struct ldpaa_fd	fd;
		*< Frame Descriptor structure
	//struct parse_result prs;
		*< Parser Result structure
};*/

/** @} */ /* end of AIOP_HWC_Definitions */

/**************************************************************************//**
 @Group		AIOP_DEFAULT_TASK_Params  AIOP Default Task Params

 @Description	AIOP Default Task Parameters

 @{
*//***************************************************************************/
/**************************************************************************//**
@Description	Default Task Parameters
*//***************************************************************************/

struct aiop_default_task_params {
	/** NI ID the packet should be sent on */
	uint16_t send_niid;
	/** Task default Starting HXS for Parser */
	uint16_t parser_starting_hxs;
	/** Task default Parser Profile ID */
	uint8_t parser_profile_id;
	/** Queueing Destination Priority */
	uint8_t qd_priority;
	/** current scope level */
	uint8_t current_scope_level;
	/** scope mode level [0-3] */
	uint8_t scope_mode_level_arr[4];
};
/** @} */ /* end of AIOP_DEFAULT_TASK_Params */

/* Workaround for ENGR346926 */
/*
#define __stqw(arg1, arg2, arg3, arg4, displ, base)	\
	__stdw(arg1, arg2, displ, base);    		\
	__stdw(arg3, arg4, displ+8, base);
*/


/** @} */ /* end of AIOP_General_Definitions */

/**************************************************************************//**
@Function	fsl_read_external_data

@Description	This service routine will read the data from an external
		address in System memory using DMA to fetch in workspace memory

@Param[in]	ws_addr - A pointer to the source/target location in Workspace
		Memory for DMA Data. Workspace address is
		limited to 16 bits.
@Param[in]	ext_address - System memory source address, for DMA, where
		data reside. External address can only be from PEB,
		DP-DDR or System DDR as memory partitions
@Param[in]	copy_size - Number of bytes to copy (limited to 12 bits in case
		FDMA is used as engine)
@Param[in]	flags - Please refer to \link ReadDataExt command flags
		\endlink.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF in case
		CDMA is used as engine
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//****************************************************************************/
inline void fsl_read_external_data(void *ws_addr, uint64_t ext_address,
				   uint16_t copy_size, uint32_t flags);

/** @} */ /* end of AIOP_GENERAL */

#endif /* __FSL_GENERAL_H_ */
