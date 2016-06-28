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
@File          aiop_verification_CWAPR.h

@Description   This file contains the AIOP CWAPR SW Verification Structures
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_CWAPR_H_
#define __AIOP_VERIFICATION_CWAPR_H_

#include "fsl_ldpaa.h"
#include "fsl_net.h"
#include "fsl_cwapr.h"
#include "cwapr.h"


/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_CWAPR_FM_Verification

 @Description	AIOP CWAPR Verification structures definitions.

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


/*! \enum e_cwapr_verif_cmd_type defines the CWAPR CMDTYPE field.*/
enum e_cwapr_verif_cmd_type {
	CWAPR_CMDTYPE_VERIF_INIT = 0,
	CWAPR_CMDTYPE_CREATE_INSTANCE,
	CWAPR_CMDTYPE_DELETE_INSTANCE,
	CWAPR_CMDTYPE_REASSEMBLE,
	CWAPR_CMDTYPE_MODIFY_REASS_FRM_SIZE,
	CWAPR_CMDTYPE_MODIFY_TO_VALUE,
	CWAPR_CMDTYPE_GET_REASS_FRM_CNTR
};

/* CWAPR Commands Structure identifiers */

#define CWAPR_VERIF_INIT_CMD_STR	((CWAPR_MODULE << 16)  | \
		(uint32_t)CWAPR_CMDTYPE_VERIF_INIT)

#define CWAPR_CREATE_INSTANCE_CMD_STR	((CWAPR_MODULE << 16)  | \
		(uint32_t)CWAPR_CMDTYPE_CREATE_INSTANCE)

#define CWAPR_DELETE_INSTANCE_CMD_STR	((CWAPR_MODULE << 16)  | \
		(uint32_t)CWAPR_CMDTYPE_DELETE_INSTANCE)

#define CWAPR_REASSEMBLE_CMD_STR	((CWAPR_MODULE << 16)  | \
		(uint32_t)CWAPR_CMDTYPE_REASSEMBLE)

#define CWAPR_MODIFY_REASS_FRM_SIZE_CMD_STR ((CWAPR_MODULE << 16)  | \
		(uint32_t)CWAPR_CMDTYPE_MODIFY_REASS_FRM_SIZE)

#define CWAPR_MODIFY_TO_VALUE_CMD_STR	((CWAPR_MODULE << 16)  | \
		(uint32_t)CWAPR_CMDTYPE_MODIFY_TO_VALUE)

#define CWAPR_GET_REASS_FRM_CNTR_CMD_STR	((CWAPR_MODULE << 16)  | \
		(uint32_t)CWAPR_CMDTYPE_GET_REASS_FRM_CNTR)

/**************************************************************************//**
@Description	CWAPR verification init Command structure.

		This command calls the cwapr_init which inits the CWAPR. This command
		must be called only once in the beggining of a test	that includes CWAPR.
		As apposed to init parser that should be called for each new task, this
		command is called only once in the test.
*//***************************************************************************/
struct cwapr_init_verif_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	max_buffers;
	uint32_t	flags;
	uint8_t		tmi_id;
	uint8_t		res[3];
};

/**************************************************************************//**
@Description	CWAPR create instance Command structure.

		Includes information needed for CWAPR Command verification.
*//***************************************************************************/
struct cwapr_create_instance_command {
	uint32_t		opcode;
		/**< Command structure identifier. */
	int32_t			status;
	uint32_t		instance_index;
	struct cwapr_params	cwapr_params;
#ifdef CLOSE_MODEL
	cwapr_instance_handle_t	cwapr_instance;
	uint8_t			cwapr_instance_ref;
	uint8_t			res[3];
#endif

};

/**************************************************************************//**
@Description	CWAPR delete instance Command structure.

		Includes information needed for CWAPR Command verification.
*//***************************************************************************/
struct cwapr_delete_instance_command {
	uint32_t			opcode;
	/**< Command structure identifier. */
	int32_t				status;
	cwapr_del_cb_t		*confirm_delete_cb;
	cwapr_del_arg_t		delete_arg;
	uint32_t			instance_index;
#ifdef CLOSE_MODEL
	cwapr_instance_handle_t		cwapr_instance;
	uint8_t				cwapr_instance_ref;
	uint8_t				res[3];
#endif	
};

/**************************************************************************//**
@Description	CWAPR Reassembly Command structure.

		Includes information needed for CWAPR Command verification.
*//***************************************************************************/
struct cwapr_reassemble_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	instance_index;
	uint64_t	tunnel_id;
		/** Returned Value: parse results. */
	struct parse_result pr;

#ifdef CLOSE_MODEL
	cwapr_instance_handle_t	cwapr_instance;
	uint8_t			cwapr_instance_ref;
	uint8_t			res[3];
#endif
};

/**************************************************************************//**
@Description	CWAPR max reassembled frame size Command structure.

		Includes information needed for CWAPR Command verification.
*//***************************************************************************/
struct cwapr_modify_max_reass_frm_size_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	max_reass_frm_size;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	cwapr_instance_handle_t			cwapr_instance;
	uint8_t					cwapr_instance_ref;
	uint8_t					res[5];
#else
	uint16_t				res;
#endif	
};

/**************************************************************************//**
@Description	CWAPR modify TO value Command structure.

		Includes information needed for CWAPR Command verification.
*//***************************************************************************/
struct cwapr_modify_timeout_value_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	reasm_timeout_value;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	cwapr_instance_handle_t			cwapr_instance;
	uint8_t					cwapr_instance_ref;
	uint8_t					res[5];
#else
	uint16_t				res;
#endif	
};

/**************************************************************************//**
@Description	CWAPR get reassembled frame counter Command structure.

		Includes information needed for CWAPR Command verification.
*//***************************************************************************/
struct cwapr_get_reass_frm_cntr_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	flags;
	uint32_t	reass_frm_cntr;
	uint32_t	instance_index;
#if CLOSED_MODEL	
	cwapr_instance_handle_t			cwapr_instance;
	uint8_t					cwapr_instance_ref;
	uint8_t					res[7];
#endif		
};

/**************************************************************************//**
@Description	CWAPR FDMA Enqueue Working Frame Command structure.

		Includes information needed for FDMA Enqueue Working Frame
		command verification.

*//***************************************************************************/
struct cwapr_fdma_enqueue_wf_command {
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

uint16_t aiop_verification_cwapr(uint32_t asa_seg_addr);
void cwapr_delete_instance_cb_verif(uint64_t arg);
void cwapr_timeout_cb_verif(uint64_t arg, uint32_t flags);

/** @} */ /* end of AIOP_CWAPR_FM_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_CWAPR_H_ */
