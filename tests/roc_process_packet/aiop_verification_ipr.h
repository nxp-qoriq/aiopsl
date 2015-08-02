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
@File          aiop_verification_IPR.h

@Description   This file contains the AIOP IPR SW Verification Structures
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_IPR_H_
#define __AIOP_VERIFICATION_IPR_H_

#include "fsl_ldpaa.h"
#include "fsl_net.h"
#include "fsl_ipr.h"
#include "ipr.h"


/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_IPR_FM_Verification

 @Description	AIOP IPR Verification structures definitions.

 @{
*//***************************************************************************/

//#define IPR_VERIF_ACCEL_ID	0xFD
	/**< HM accelerator ID For verification purposes*/
#define PARSER_INIT_BPID	1
	/**< A BPID to use for the parser init.
	 * This BPID needs to be initialized in BMAN in order for the
	 * HM tests to run correctly. The HM tests requires only one buffer. */
#define PARSER_INIT_BUFF_SIZE	128
	/**< A buffer size that corresponds to the PARSER_INIT_BPID to use for
	 * the parser init.
	 * This BPID needs to be initialized in BMAN in order for the
	 * HM tests to run correctly */


/*! \enum e_ipr_verif_cmd_type defines the IPR CMDTYPE field.*/
enum e_ipr_verif_cmd_type {
	IPR_CMDTYPE_VERIF_INIT = 0,
	IPR_CMDTYPE_CREATE_INSTANCE,
	IPR_CMDTYPE_DELETE_INSTANCE,
	IPR_CMDTYPE_REASSEMBLE,
	IPR_CMDTYPE_MODIFY_REASS_FRM_SIZE,
	IPR_CMDTYPE_MODIFY_MIN_FRAG_SIZE_IPV4,
	IPR_CMDTYPE_MODIFY_MIN_FRAG_SIZE_IPV6,
	IPR_CMDTYPE_MODIFY_TO_VALUE_IPV4,
	IPR_CMDTYPE_MODIFY_TO_VALUE_IPV6,
	IPR_CMDTYPE_GET_REASS_FRM_CNTR
};

/* IPR Commands Structure identifiers */

#define IPR_VERIF_INIT_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_VERIF_INIT)

#define IPR_CREATE_INSTANCE_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_CREATE_INSTANCE)

#define IPR_DELETE_INSTANCE_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_DELETE_INSTANCE)

#define IPR_REASSEMBLE_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_REASSEMBLE)

#define IPR_MODIFY_REASS_FRM_SIZE_CMD_STR ((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_REASS_FRM_SIZE)

#define IPR_MODIFY_MODIFY_MIN_FRAG_SIZE_IPV4_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_MIN_FRAG_SIZE_IPV4)

#define IPR_MODIFY_MODIFY_MIN_FRAG_SIZE_IPV6_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_MIN_FRAG_SIZE_IPV6)

#define IPR_MODIFY_TO_VALUE_IPV4_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_TO_VALUE_IPV4)

#define IPR_MODIFY_TO_VALUE_IPV6_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_TO_VALUE_IPV6)

#define IPR_GET_REASS_FRM_CNTR_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_GET_REASS_FRM_CNTR)

/**************************************************************************//**
@Description	IPR verification init Command structure.

		This command calls the ipr_init which inits the IPR. This command
		must be called only once in the beggining of a test	that includes IPR.
		As apposed to init parser that should be called for each new task, this
		command is called only once in the test.
*//***************************************************************************/
struct ipr_init_verif_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	max_buffers;
	uint32_t	flags;
	uint8_t		tmi_id;
	uint8_t		res[3];
};

/**************************************************************************//**
@Description	IPR create instance Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_create_instance_command {
	uint32_t		opcode;
		/**< Command structure identifier. */
	int32_t			status;
	uint32_t		instance_index;
	struct ipr_params	ipr_params;
#ifdef CLOSE_MODEL
	ipr_instance_handle_t	ipr_instance;
	uint8_t			ipr_instance_ref;
	uint8_t			res[3];
#endif

};

/**************************************************************************//**
@Description	IPR delete instance Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_delete_instance_command {
	uint32_t			opcode;
	/**< Command structure identifier. */
	int32_t				status;
	ipr_del_cb_t			*confirm_delete_cb;
	ipr_del_arg_t			delete_arg;
	uint32_t			instance_index;
#ifdef CLOSE_MODEL
	ipr_instance_handle_t		ipr_instance;
	uint8_t				ipr_instance_ref;
	uint8_t				res[3];
#endif	
};

/**************************************************************************//**
@Description	IPR Reassembly Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_reassemble_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	instance_index;
		/** Returned Value: parse results. */
	struct parse_result pr;

#ifdef CLOSE_MODEL
	ipr_instance_handle_t	ipr_instance;
	uint8_t			ipr_instance_ref;
	uint8_t			res[3];
#endif
};

/**************************************************************************//**
@Description	IPR max reassembled frame size Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_modify_max_reass_frm_size_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	max_reass_frm_size;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	ipr_instance_handle_t			ipr_instance;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
#else
	uint16_t				res;
#endif	
};

/**************************************************************************//**
@Description	IPR modify min fragment size Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_modify_min_frag_size_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	min_frag_size;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	ipr_instance_handle_t			ipr_instance;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
#else
	uint16_t				res;
#endif	
};

/**************************************************************************//**
@Description	IPR modify IPv4 TO value Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_modify_timeout_value_ipv4_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	reasm_timeout_value_ipv4;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	ipr_instance_handle_t			ipr_instance;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
#else
	uint16_t				res;
#endif	
};

/**************************************************************************//**
@Description	IPR modify IPv6 TO value Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_modify_timeout_value_ipv6_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	reasm_timeout_value_ipv6;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	ipr_instance_handle_t			ipr_instance;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
#else
	uint16_t				res;
#endif	
};

/**************************************************************************//**
@Description	IPR get reassembled frame counter Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_get_reass_frm_cntr_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	flags;
	uint32_t	reass_frm_cntr;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	ipr_instance_handle_t			ipr_instance;
	uint8_t					ipr_instance_ref;
	uint8_t					res[7];
#endif		
};

/**************************************************************************//**
@Description	IPR FDMA Enqueue Working Frame Command structure.

		Includes information needed for FDMA Enqueue Working Frame
		command verification.

*//***************************************************************************/
struct ipr_fdma_enqueue_wf_command {
		/** FDMA Enqueue working frame command structure
		* identifier. */
	uint32_t opcode;
		/** Queueing destination for the enqueue
		 * (enqueue_id_sel = 0,16bit) or Frame Queue ID for the enqueue
		 * (enqueue_id_sel = 1,24bit).*/
	uint32_t qd_fqid;
		/** Distribution hash value passed to QMan for distribution
		 * purpose on the enqueue. */
	uint16_t qdbin;
		/** Queueing Destination Priority. */
	uint16_t icid;
	uint8_t	qd_priority;
		/** Storage profile used to store frame data if additional
		* buffers are required*/
	uint8_t	 spid;
		/** Enqueue Priority source
		* - 0: use QD_PRI provided with DMA Command
		* - 1: use QD_PRI from h/w context. This is the value
		* found in the WQID field from ADC. */
	uint8_t	PS;
		/** Terminate Control:
		* - 0: Return after enqueue.
		* - 1: Terminate: this command will trigger the Terminate task
		* command right after the enqueue. If the enqueue failed, the
		* frame will be discarded.
		* - 3: reserved */
	uint8_t	TC;
		/** Enqueue ID selection:
		* - 0 = queueing destination(16bit)
		* - 1 = fqid (24bit). */
	uint8_t	EIS;
	uint8_t	BDI;
		/** Command returned status. */
	int8_t  status;
	uint8_t res[5];
};

uint16_t aiop_verification_ipr(uint32_t asa_seg_addr);
void ipr_delete_instance_cb_verif(uint64_t arg);
void ipr_timeout_cb_verif(uint64_t arg, uint32_t flags);

/** @} */ /* end of AIOP_IPR_FM_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_IPR_H_ */
