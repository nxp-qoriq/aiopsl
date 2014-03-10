/**************************************************************************//**
@File          aiop_verification_IPR.h

@Description   This file contains the AIOP IPR SW Verification Structures
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_IPR_H_
#define __AIOP_VERIFICATION_IPR_H_

#include "dplib/fsl_ldpaa.h"
#include "net/fsl_net.h"
#include "dplib/fsl_ipr.h"
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
	IPR_CMDTYPE_MODIFY_MIN_FRAG_SIZE,
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

#define IPR_MODIFY_MODIFY_MIN_FRAG_SIZE_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_MIN_FRAG_SIZE)

#define IPR_MODIFY_TO_VALUE_IPV4_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_TO_VALUE_IPV4)

#define IPR_MODIFY_TO_VALUE_IPV6_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_MODIFY_TO_VALUE_IPV6)

#define IPR_GET_REASS_FRM_CNTR_CMD_STR	((IPR_MODULE << 16)  | \
		(uint32_t)IPR_CMDTYPE_GET_REASS_FRM_CNTR)

#pragma pack(push, 1)


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
	uint32_t				opcode;
		/**< Command structure identifier. */
	int32_t			status;
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
#ifdef CLOSE_MODEL
struct ipr_delete_instance_command {
	uint32_t			opcode;
	/**< Command structure identifier. */
	int32_t				status;
	ipr_instance_handle_t		ipr_instance;
	ipr_del_cb_t			*confirm_delete_cb;
	ipr_del_arg_t			delete_arg;
	uint8_t				ipr_instance_ref;
	uint8_t				res[3];
};
#endif

/**************************************************************************//**
@Description	IPR Reassembly Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_reassemble_command {
	uint32_t				opcode;
		/**< Command structure identifier. */
	int32_t					status;
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
#ifdef CLOSE_MODEL
struct ipr_modify_max_reass_frm_size_command {
	uint32_t				opcode;
		/**< Command structure identifier. */
	int32_t					status;
	ipr_instance_handle_t			ipr_instance;
	uint16_t				max_reass_frm_size;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
};

/**************************************************************************//**
@Description	IPR modify min fragment size Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_modify_min_frag_size_command {
	uint32_t				opcode;
		/**< Command structure identifier. */
	int32_t					status;
	ipr_instance_handle_t	ipr_instance;
	uint16_t				min_frag_size;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
};

/**************************************************************************//**
@Description	IPR modify IPv4 TO value Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_modify_timeout_value_ipv4_command {
	uint32_t				opcode;
		/**< Command structure identifier. */
	int32_t					status;
	ipr_instance_handle_t	ipr_instance;
	uint16_t				reasm_timeout_value_ipv4;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
};

/**************************************************************************//**
@Description	IPR modify IPv6 TO value Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_modify_timeout_value_ipv6_command {
	uint32_t				opcode;
		/**< Command structure identifier. */
	int32_t					status;
	ipr_instance_handle_t			ipr_instance;
	uint16_t				reasm_timeout_value_ipv6;
	uint8_t					ipr_instance_ref;
	uint8_t					res[5];
};

/**************************************************************************//**
@Description	IPR get reassembled frame counter Command structure.

		Includes information needed for IPR Command verification.
*//***************************************************************************/
struct ipr_get_reass_frm_cntr_command {
	uint32_t				opcode;
		/**< Command structure identifier. */
	int32_t					status;
	ipr_instance_handle_t	ipr_instance;
	uint32_t				flags;
	uint32_t				*reass_frm_cntr;
	uint8_t					ipr_instance_ref;
	uint8_t					res[7];
};

#endif
#pragma pack(pop)

uint16_t aiop_verification_ipr(uint32_t asa_seg_addr);

void ipr_verif_update_frame(uint16_t iteration);


/** @} */ /* end of AIOP_IPR_FM_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_IPR_H_ */
