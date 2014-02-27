/**************************************************************************//**
@File		general.h

@Description	This file contains AIOP SW internal general definitions.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __GENERAL_H_
#define __GENERAL_H_


#include "dplib/fsl_ldpaa.h"
#include "null.h"
/*#include "dplib/fsl_aiop_parser.h"*/

/**************************************************************************//**
 @Group		AIOP_General_Definitions AIOP SL General Definitions

 @Description	AIOP General Definitions

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Group		AIOP_General_Macros

 @Description	AIOP General Macros

 @{
*//***************************************************************************/

	/** Null buffer pool ID */
#define BPID_NULL	0x00FF
	/** Logical false */
#define FALSE	0
	/** Logical true */
#define TRUE	1
	/**< 0 value */
#define ZERO	0
	/** 1 value */
#define ONE	1

/* Address of end of TLS section */
extern const uint16_t TLS_SECTION_END_ADDR[];

/** @} */ /* end of AIOP_GENERAL_MACROS */


/**************************************************************************//**
 @Group		AIOP_Accelerators_IDS

 @Description	AIOP Accelerators IDs

 @{
*//***************************************************************************/

/* Accelerators IDs (from AIOP Source IDs section in ArchDef) */
	/** Accelerator ID for yield command*/
#define YIELD_ACCEL_ID			0x00
	/** TMAN accelerator ID */
#define TMAN_ACCEL_ID			0x01
	/** PARSER & CLASSIFIER accelerator ID */
#define CTLU_PARSE_CLASSIFY_ACCEL_ID	0x04
	/** CTLU accelerator ID */
//#define CTLU_ACCEL_ID			0x05
	/** Frame Presentation DMA accelerator ID */
#define FPDMA_ACCEL_ID			0x0C
	/** Frame Output DMA accelerator ID */
#define FODMA_ACCEL_ID			0x0E
	/** SCDMA accelerator ID */
#define CDMA_ACCEL_ID			0x0D

/** @} */ /* end of AIOP_ACCELERATORS_IDS */


/**************************************************************************//**
 @Group		AIOP_Return_Status

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

/** @} */ /* end of AIOP_Return_Status */


/**************************************************************************//**
 @Group		AIOP_HWC_Definitions

 @Description	Hardware Context (HWC) Definitions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_HWC_General_Definitions

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
	/* Todo - set SPID address when according to new layout(once decided)*/
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
	/** Address of Additional Dequeue Context */
#define HWC_ADC_ADDRESS		0x40
	/** Address of Presentation Context */
#define HWC_PRC_ADDRESS		0x50
	/** Address of Default working frame FD */
#define HWC_FD_ADDRESS		0x60
	/** Address of parse results */
#define HWC_PARSE_RES_ADDRESS	0x80

/** @} */ /* end of AIOP_HWC_General_Definitions */

/**************************************************************************//**
 @Group		AIOP_ADC_Definitions

 @Description	Additional Dequeue Context (ADC) Definitions

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	Additional Dequeue Context (ADC) structure.

*//***************************************************************************/
struct additional_dequeue_context {

		/** Frame Queue Context as received from QMan
		 * via the AIOP DCP. */
	uint64_t fqd_ctx;
		/**
		- bits<0-7>: AIOP Channel that this FD arrived on.
		- bits<8-31>: The QMan Frame Queue ID that
		this frame arrived was dequeued from. */
	uint32_t channel_fqid;
		/**
		- bits<0>: Privilege Level.
		- bits<1-15>: The Isolation Context ID that the dequeued frame
		belongs to. */
	uint16_t pl_icid;
		/**
		- bits<1-3>: QMan Work Queue ID that this FD was dequeued from.
		- bits<5-7>: Entry Priority. Indicates the priority of the
		presented FD. */
	uint8_t wqid_pri;
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
		 * - bits<5>  : Virtual Address.
		 * - bits<6>  : FQD_CTX_FMT or Dequeue Response FQD Context
		 * Format. Indicates the format of the received FQD_CTX field.
		 * - bits<7>  : Bypass DPAA Resource Isolation
		 * .
		 * */
	uint8_t fdsrc_va_fca_bdi;
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

/** @} */ /* end of AIOP_ADC_Definitions */


/**************************************************************************//**
 @Group		AIOP_PRC_Definitions

 @Description	Presentation Context (ADC) Definitions

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	Presentation Context (PRC) structure.

*//***************************************************************************/
struct presentation_context {

		/** Entry point opaque parameter value. */
	uint32_t param;
		/** Segment presentation address. */
	uint16_t seg_address;
		/** Segment actual size. */
	uint16_t seg_length;
		/**
		- bits<0-9>  : Pass Through Annotation(PTA) presentation
		address value.
		- bits<10> : No PTA segment (NPS).
		- bits<11> : No ASA segment (NAS).
		- bits<12-15>: Acceleration Specific Annotation(ASA)
		presentation offset value. */
	uint16_t  ptapa_asapo;
		/**
		- bits<0-9>  : Accelerator Specific Annotation (ASA)
		presentation address value.
		- bits<10>   : Entry Point Segment Reference (SR) bit.
		- bits<11>   : No Data segment (NDS).
		- bits<12-15>: Acceleration Specific Annotation (ASA)
		presentation size value. */
	uint16_t  asapa_asaps;
		/**
		- bits<0>  : OSM Entry Point Source value.
		- bits<1>  : OSM Entry Point Execution Phase value.
		- bits<2-3>: OSM Entry Point Select value.
		- bits<5-7>: OSM Entry Point Order Scope Range Mask value. */
	uint8_t osrc_oep_osel_osrm;
		/**
		- bits<0-3>  : Working Frame Handle.
		- bits<4-7>  : Open Segment Handle. */
	uint8_t handles;
		/** Segment Presentation Offset value. */
	uint16_t seg_offset;
};

/* Presentation Context (PRC) Macros */
	/** PTA presentation address mask.*/
#define PRC_PTAPA_MASK		0xFFC0
	/** ASA presentation offset mask */
#define PRC_ASAPO_MASK		0x000F
	/** ASA presentation address mask */
#define PRC_ASAPA_MASK		0xFFC0
	/** Segment Reference (SR) bit mask */
#define PRC_SR_MASK		0x0020
	/** No Data Segment (NDS) bit mask */
#define PRC_NDS_MASK		0x0010
#if NAS_NPS_ENABLE
	/** No PTA Segment (NPS) bit mask */
#define PRC_NPS_MASK		0x0020
	/** No ASA Segment (NAS) bit mask */
#define PRC_NAS_MASK		0x0010
#endif /*NAS_NPS_ENABLE*/
	/** ASA presentation size mask */
#define PRC_ASAPS_MASK		0x000F
	/** OSM Entry point source value mask*/
#define PRC_OSRC_MASK		0x80
	/** OSM Entry Point Execution Phase value mask */
#define PRC_OEP_MASK		0x40
	/** OSM Entry Point Select value mask */
#define PRC_OSEL_MASK		0x30
	/** OSM Entry Point Order Scope Range value mask */
#define PRC_OSRM_MASK		0x07
	/** Frame handle mask */
#define PRC_FRAME_HANDLE_MASK	0xF0
	/** Segment handle mask */
#define PRC_SEGMENT_HANDLE_MASK	0x0F
	/** PRC Frame handle offset */
#define PRC_FRAME_HANDLE_BIT_OFFSET	0x4
	/** Segment Reference bit offset */
#define PRC_SR_BIT_OFFSET	0x5
	/** No Data Segment bit offset */
#define PRC_NDS_BIT_OFFSET	0x4
#if NAS_NPS_ENABLE
	/** No PTA Segment bit offset */
#define PRC_NPS_BIT_OFFSET	0x5
	/** No ASA Segment bit offset */
#define PRC_NAS_BIT_OFFSET	0x4
#endif /*NAS_NPS_ENABLE*/
	/** OSM Entry Point source value offset */
#define PRC_OSRC_BIT_OFFSET	0x7
	/** OSM Entry Point Execution Phase value offset */
#define PRC_OEP_BIT_OFFSET	0x6
	/** OSM Entry Point Select value offset */
#define PRC_OSEL_BIT_OFFSET	0x4
	/** PTA size */
#define PRC_PTA_SIZE		0x40
	/** PTA address when PTA is not loaded/not intended to be loaded to the
	 * working frame */
#define PRC_PTA_NOT_LOADED_ADDRESS	0xFFC0
	/** ASA address offset */
#define PRC_ASA_ADDRESS_OFFSET	0x6
	/** PTA address offset */
#define PRC_PTA_ADDRESS_OFFSET	0x6

/**************************************************************************//**
 @Group		AIOP_PRC_Getters

 @Description	Presentation Context (ADC) Getters

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
	/** Macro to get the default frame PTA address in workspace from the
	 * presentation context */
#define PRC_GET_PTA_ADDRESS()						\
	((uint16_t)((((struct presentation_context *)HWC_PRC_ADDRESS)->\
		ptapa_asapo) & PRC_PTAPA_MASK))
	/** Macro to get the default frame ASA offset from the presentation
	 * context */
#define PRC_GET_ASA_OFFSET()						\
	((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo) & PRC_ASAPO_MASK)
	/** Macro to get the default frame ASA address in workspace from the
	 * presentation context */
#define PRC_GET_ASA_ADDRESS()						\
	((uint16_t)((((struct presentation_context *)HWC_PRC_ADDRESS)->\
		asapa_asaps) & PRC_ASAPA_MASK))
	/** Macro to get the Segment Reference bit from the presentation
	 * context */
#define PRC_GET_SR_BIT()						\
	(((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps) & PRC_SR_MASK) >> PRC_SR_BIT_OFFSET)
	/** Macro to get the No-Data-Segment bit from the presentation
	 * context */
#define PRC_GET_NDS_BIT()						\
	(((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps) & PRC_NDS_MASK) >> PRC_NDS_BIT_OFFSET)
#if NAS_NPS_ENABLE
	/** Macro to get the No-ASA-Segment bit from the presentation
	 * context */
#define PRC_GET_NAS_BIT()						\
	(((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo) & PRC_NAS_MASK) >> PRC_NAS_BIT_OFFSET)
	/** Macro to get the No-PTA-Segment bit from the presentation
	 * context */
#define PRC_GET_NPS_BIT()						\
	(((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		ptapa_asapo) & PRC_NPS_MASK) >> PRC_NPS_BIT_OFFSET)
#endif /*NAS_NPS_ENABLE*/
	/** Macro to get the default frame ASA size in workspace from the
	 * presentation context */
#define PRC_GET_ASA_SIZE()						\
	((uint16_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->	\
		asapa_asaps) & PRC_ASAPS_MASK)
	/** Macro to get the default frame handle + default segment handles
	 * from the presentation context */
#define PRC_GET_HANDLES()						\
	((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->handles))
	/** Macro to get the default frame handle from the presentation
	 * context */
#define PRC_GET_FRAME_HANDLE()						\
	((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->handles) \
		>> PRC_FRAME_HANDLE_BIT_OFFSET)
	/** Macro to get the default segment handle from the presentation
	 * context */
#define PRC_GET_SEGMENT_HANDLE()					\
	((uint8_t)(((struct presentation_context *)HWC_PRC_ADDRESS)->handles) \
		& PRC_SEGMENT_HANDLE_MASK)
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
 @Group		AIOP_DEFAULT_TASK_Params

 @Description	AIOP Default Task Parameters

 @{
*//***************************************************************************/
/**************************************************************************//**
@Description	Default Task Parameters
*//***************************************************************************/

struct aiop_default_task_params {
	/** Task default Starting HXS for Parser */
	uint16_t parser_starting_hxs;
	/** Task default Parser Profile ID */
	uint8_t parser_profile_id;
	/** Queueing Destination Priority */
	uint8_t qd_priority;
};
/** @} */ /* end of AIOP_DEFAULT_TASK_Params */


/**************************************************************************//**
 @Group		AIOP_Inline_Asm

 @Description	Inline Assembler Macros

 @{
*//***************************************************************************/


/* Load Byte And Reserve Indexed */
#define __lbarx(_res, _disp, _base)			\
	asm ("lbarx %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Load Halfword And Reserve Indexed */
#define __lharx(_res, _disp, _base)			\
	asm ("lharx %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Load Word And Reserve Indexed */
#define __lwarx(_res, _disp, _base)			\
	asm ("lwarx %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Store Byte Conditional Indexed */
#define __stbcx(_res, _disp, _base)			\
	asm ("stbcx. %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Store HalfWord Conditional Indexed */
#define __sthcx(_res, _disp, _base)			\
	asm ("sthcx. %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);
/* Store Word Conditional Indexed */
#define __stwcx(_res, _disp, _base)			\
	asm ("stwcx. %[result], %[displ], %[base]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp),[base]"r"(_base)	\
		);


/* Load Halfword Byte-Reverse Indexed */
#define __lhbrx(_res, _disp)				\
	asm ("lhbrx %[result], r0, %[displ]\n"	\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp)			\
		);
/* Load Word Byte-reversed */
#define __lwbrx(_res, _disp)				\
	asm ("lwbrx %[result], r0, %[displ]\n"		\
		:[result]"=r"(_res)			\
		:[displ]"r"(_disp)			\
		);
/* Load DoubleWord into Two Words with Byte-Reversal of Words */
#define __e_ldwbrw_d(_val, _disp)			\
	asm  ("e_ldwbrw %[value], %[displ](r0)\n"	\
		:[value]"=r" (_val)			\
		:[displ]"i" (_disp)			\
			);

/* Store Halfword Byte-Reverse Indexed */
#define __sthbrx(_val, _disp)				\
	asm ("sthbrx %[value], r0, %[displ]\n"	\
		:					\
		:[value]"r"(_val), [displ]"r"(_disp)	\
		);
/* Store Word Byte-reversed */
#define __stwbrx(_val, _disp)				\
	asm ("stwbrx %[value],r0, %[displ]\n"		\
		:					\
		:[value]"r"(_val), [displ]"r"(_disp)	\
		);
/* Store Double of Two Words with Byte-Reversal of Words */
#define __e_stdwbrw(_val, _disp)			\
	asm ("e_stdwbrw %[value], r0, %[displ]\n"	\
		:					\
		:[value]"r"(_val), [displ]"r"(_disp)	\
		);
/* Store Double of Two Words 64 bit input variable and displacement*/
#define __st64dw_d(_val, _disp)				\
	asm ("e_stdw %[value], %[displ](r0)\n"		\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp)	\
		);
/* Store Double of Two Words 64 bit input variable and base*/
#define __st64dw_b(_val, _base_reg)				\
	asm ("e_stdw %[value], 0(%[base])\n"			\
		:						\
		:[value]"r" (_val), [base]"r" (_base_reg)	\
		);
/* Load Double of Two Words 64 bit input variable and displacement*/
#define __ld64dw_d(_val, _disp)				\
	asm ("e_ldw %[value], %[displ](r0)\n"		\
		:[value]"=r" (_val)			\
		:[displ]"i" (_disp)			\
		);
/* Load Double of Two Words 64 bit input variable and base*/
#define __ld64dw_b(_val, _base_reg)			\
	asm ("e_ldw %[value], 0(%[base])\n"		\
		:[value]"=r" (_val)			\
		:[base]"r" (_base_reg)			\
		);
/* Store word input variable */
#define __stw_d(_val, _disp)				\
	asm ("e_stw %[value], %[displ](r0)\n"		\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp)	\
		);
#define __stw(_val, _disp, _base_reg)			\
	asm ("e_stw %[value], %[displ](%[base])\n"	\
		:					\
		:[value]"r" (_val), [displ]"i" (_disp), [base]"r" (_base_reg)\
		);
/* Store byte input variable */
#define __se_stb(_val, _base_reg)			\
	asm ("se_stb %[value], 0(%[base])\n"		\
		:[base]"=m" (_base_reg)			\
		:[value]"r" (_val)			\
		);
/* Or immediate */
#define __or(_res, _arg1, _arg2)				\
	asm ("or %[result], %[argument1], %[argument2]\n"	\
		:[result]"=r" (_res)				\
		:[argument1]"r" (_arg1), [argument2]"r" (_arg2)	\
		);
/* Or immediate */
#define __e_or2is(_res, _arg1)				\
	asm ("e_or2is %[result], %[argument1]\n"	\
		:[result]"=r" (_res)			\
		:[argument1]"i" (_arg1)			\
		);
/* Rotate left and merge */
#define __e_rlwimi(_res, _arg, _shift, _mask1, _mask2)			\
	asm ("e_rlwimi %[result], %[argu], %[sh], %[maskb], %[maske]\n"	\
		:[result]"+r"(_res)					\
		:[argu]"r"(_arg), [sh]"i"(_shift), [maskb]"i"(_mask1),	\
		 [maske]"i"(_mask2)					\
		);
/* Rotate left and mask */
#define __e_rlwinm(_res, _arg, _shift, _mask1, _mask2)			\
	asm ("e_rlwinm %[result], %[argu], %[sh], %[maskb], %[maske]\n"	\
		:[result]"=r"(_res)					\
		:[argu]"r"(_arg), [sh]"i"(_shift), [maskb]"i"(_mask1),	\
		 [maske]"i"(_mask2)					\
		);


/* Todo - Note to Hw/Compiler team:  * swap intrinsics can be used here */

#define LH_SWAP(_addr)						\
	(uint16_t)(uint32_t)({register uint16_t *__rR = 0;	\
	uint16_t temp;						\
	__lhbrx(temp, _addr);					\
	__rR = (uint16_t *) temp; })

#define LW_SWAP(_addr)						\
	(uint32_t)({register uint32_t *__rR = 0;		\
	uint32_t temp;						\
	__lwbrx(temp, _addr);					\
	__rR = (uint32_t *) temp; })

#define LDW_SWAP(_addr)						\
	(uint64_t)({register uint64_t *__rR = 0;		\
	uint32_t temp1, temp2;					\
	__ldwbrw(temp1, temp2, _addr, 0);			\
	__rR = (uint64_t *)					\
		((((uint64_t)temp1) << 32) | (uint64_t)temp2); })

#define LH_SWAP_MASK(_addr, _mask)				\
	(uint16_t)(uint32_t)({register uint16_t *__rR = 0;	\
	uint16_t temp;						\
	__lhbrx(temp, _addr);					\
	temp &= _mask;						\
	__rR = (uint16_t *) temp; })

#define LW_SWAP_MASK(_addr, _mask)				\
	(uint32_t)({register uint32_t *__rR = 0;		\
	uint32_t temp;						\
	__lwbrx(temp, _addr);					\
	temp &= _mask;						\
	__rR = (uint32_t *) temp; })

#define LW_SWAP_MASK_SHIFT(_addr, _mask, _shift)		\
	(uint32_t)({register uint32_t *__rR = 0;		\
	uint32_t temp;						\
	__lwbrx(temp, _addr);					\
	temp &= _mask;						\
	temp >>= _shift;					\
	__rR = (uint32_t *) temp; })

#define STH_SWAP(_val, _addr)					\
	__sthbrx(_val, _addr);

#define STW_SWAP(_val, _addr)					\
	__stwbrx(_val, _addr);

#define STQ_SWAP(_val, _addr)					\
	__stwbrx(_val, _addr);

/** @} */ /* end of AIOP_Inline_Asm */

/**************************************************************************//**
 @Group		AIOP_General_Protocols

 @Description	AIOP general protocols definitions and structures.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		FSL_General_Protocols_Macros General Protocols Macros

@Description	Freescale AIOP General Protocols Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		AIOP_General_Protocols_IP_Definitions Internet Protocol (IP) \
		 definitions
@{
*//***************************************************************************/
#define IPV4_HDR_LENGTH		      20   /**< IPv4 header length */
#define IPV4_HDR_ADD_LENGTH	      8    /**< IPv4 header address length */

#define IPV4_EOOL_OPTION_TYPE	      0	   /*!< End of Option List */
#define IPV4_NOP_OPTION_TYPE	      1	   /*!< No Operation */
#define IPV4_RECORD_ROUTE_OPTION_TYPE 7	   /*!< Record Route */
#define IPV4_TIMESTAMP_OPTION_TYPE    68   /*!< Internet Timestamp */
#define IPV4_SCURITY_OPTION_TYPE      130  /*!< Security option */
#define IPV4_LSRR_OPTION_TYPE	      131  /*!< Loose Source & Record Route */
#define IPV4_STREAM_IDENTIFIER_TYPE   136  /*!< Stream Identifier */
#define IPV4_SSRR_OPTION_TYPE	      137  /*!< Strict Source & Record Route */

#define IPV6_HDR_LENGTH		      40   /*!< IPv6 header length */
#define IPV6_HDR_ADD_LENGTH	      32   /*!< IPv4 header address length */

/** @} */ /* end of AIOP_General_Protocols_IP_Definitions */

/**************************************************************************//**
@Group		AIOP_General_Protocols_IPV4_HDR_Masks IPv4 Header Masks
@{
*//***************************************************************************/
#define IPV4_HDR_VERSION_MASK	  0xF0   /*!< IPv4 Version mask */
#define IPV4_HDR_IHL_MASK	  0x0F   /*!< IPv4 IHL mask */
#define IPV4_HDR_X_FLAG_MASK	  0x8000 /*!< IPv4 evil bit mask */
#define IPV4_HDR_D_FLAG_MASK	  0x4000 /*!< IPv4 don't fragment bit mask */
#define IPV4_HDR_M_FLAG_MASK	  0x2000 /*!< IPv4 more fragments bit mask */
#define IPV4_HDR_FRAG_OFFSET_MASK 0x1FFF /*!< IPv4 fragment offset mask */

/** @} */ /* end of AIOP_General_Protocols_IPV4_HDR_Masks */

/**************************************************************************//**
@Group		AIOP_General_Protocols_IPV4_HDR_Offsets IPv4 Header Offsets
@{
*//***************************************************************************/
#define IPV4_HDR_VERSION_OFFSET     4 /*!< IPv4 header Version field offset*/
#define IPV4_HDR_IHL_OFFSET	    0 /*!< IPv4 header IHL field offset*/
#define IPV4_HDR_X_FLAG_OFFSET      13/*!< IPv4 evil bit offset */
#define IPV4_HDR_D_FLAG_OFFSET      14/*!< IPv4 don't fragment bit offset */
#define IPV4_HDR_M_FLAG_OFFSET      15/*!< IPv4 more fragments bit offset */
#define IPV4_HDR_FRAG_OFFSET_OFFSET 0 /*!< IPv4 fragment offset field offset */

/** @} */ /* end of AIOP_General_Protocols_IPV4_HDR_Offsets */

/**************************************************************************//**
@Group		AIOP_General_Protocols_TCP_Definitions Transmission Control
		Protocol (TCP) Definitions
@{
*//***************************************************************************/
#define TCP_HDR_LENGTH		      20   /**< TCP header length */
#define TCP_PROTOCOL		      6   /**< TCP Protocol number */

/** @} */ /* end of AIOP_General_Protocols_TCP_Definitions */

/** @} */ /* end of FSL_General_Protocols_Macros */
/** @} */ /* end of AIOP_General_Protocols */
/** @} */ /* end of AIOP_General_Definitions */


#endif /* __GENERAL_H_ */
