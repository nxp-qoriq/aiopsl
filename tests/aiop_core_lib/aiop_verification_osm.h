/**************************************************************************//**
@File		aiop_verification_osm.h

@Description	This file contains the AIOP OSM SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_OSM_H_
#define __AIOP_VERIFICATION_OSM_H_

//#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_osm.h"
#include "general.h"
#include "osm.h"

/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_OSM_SRs_Verification

 @Description	AIOP OSM Verification structures definitions.

 @{
*//***************************************************************************/

/** \enum osm_verif_cmd_type defines the OSM verification CMDTYPE
 * field. */
enum osm_verif_cmd_type {
	OSM_SCOPE_TRANS_TO_EX_INC_SCOPE_ID_CMDTYPE = 0,
	OSM_SCOPE_TRANS_TO_EX_NEW_SCOPE_ID_CMDTYPE,
	OSM_SCOPE_TRANS_TO_CON_INC_SCOPE_ID_CMDTYPE,
	OSM_SCOPE_TRANS_TO_CON_NEW_SCOPE_ID_CMDTYPE,	
	OSM_SCOPE_RELINQUISH_EX_CMDTYPE,
	OSM_SCOPE_ENTER_TO_EX_INC_SCOPE_ID_CMDTYPE,
	OSM_SCOPE_ENTER_TO_EX_NEW_SCOPE_ID_CMDTYPE,
	OSM_SCOPE_ENTER_CMDTYPE,
	OSM_SCOPE_EXIT_CMDTYPE,
	OSM_GET_SCOPE_CMDTYPE,
	OSM_INIT_CMDTYPE
};

#define OSM_SCOPE_TRANS_TO_EX_INC_SCOPE_ID_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_TRANS_TO_EX_INC_SCOPE_ID_CMDTYPE)

#define OSM_SCOPE_TRANS_TO_EX_NEW_SCOPE_ID_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_TRANS_TO_EX_NEW_SCOPE_ID_CMDTYPE)

#define OSM_SCOPE_TRANS_TO_CON_INC_SCOPE_ID_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_TRANS_TO_CON_INC_SCOPE_ID_CMDTYPE)

#define OSM_SCOPE_TRANS_TO_CON_NEW_SCOPE_ID_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_TRANS_TO_CON_NEW_SCOPE_ID_CMDTYPE)

#define OSM_SCOPE_RELINQUISH_EX_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_RELINQUISH_EX_CMDTYPE)

#define OSM_SCOPE_ENTER_TO_EX_INC_SCOPE_ID_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_ENTER_TO_EX_INC_SCOPE_ID_CMDTYPE)

#define OSM_SCOPE_ENTER_TO_EX_NEW_SCOPE_ID_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_ENTER_TO_EX_NEW_SCOPE_ID_CMDTYPE)

#define OSM_SCOPE_ENTER_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_ENTER_CMDTYPE)

#define OSM_SCOPE_EXIT_STR  ((OSM_MODULE << 16) | \
		OSM_SCOPE_EXIT_CMDTYPE)

#define OSM_GET_SCOPE_STR  ((OSM_MODULE << 16) | \
		OSM_GET_SCOPE_CMDTYPE)

#define OSM_INIT_STR  ((OSM_MODULE << 16) | \
		OSM_INIT_CMDTYPE)

/** \addtogroup AIOP_Service_Routines_Verification
 *  @{
 */


/**************************************************************************//**
 @Group		AIOP_OSM_SRs_Verification

 @Description	AIOP OSM Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	OSM Scope Initial Command structure.
		
		This command read TASK_ID from TASKCSR0, then write it to ORTAR 
		for getting OSM information registers.
*//***************************************************************************/
struct osm_initial_verif_command {
	uint32_t	opcode;
	uint8_t 	pad[4];
};

/**************************************************************************//**
@Description	OSM Scope Transition to Exclusive Command structure.
		
		This command increment the stage of the original scope_id (according
		to the \ref OSM_SCOPE_ID_STAGE_INCREMENT_MASK definition).
*//***************************************************************************/
struct osm_scope_tran_to_ex_inc_scope_id_verif_command {
	uint32_t	opcode;
	int32_t		status;
};

/**************************************************************************//**
@Description	OSM Scope Transition to Exclusive Command structure.
		
		This command associated with the new specified scope_id.

*//***************************************************************************/
struct osm_scope_tran_to_ex_new_scope_id_verif_command {
	uint32_t	opcode;
	uint32_t 	scope_id;
	int32_t		status;
	uint8_t 	pad[4];
};

/**************************************************************************//**
@Description	OSM Scope Transition to Concurrent Command structure.
		
		This command increment the stage of the original scope_id (according
		to the \ref OSM_SCOPE_ID_STAGE_INCREMENT_MASK definition).
*//***************************************************************************/
struct osm_scope_tran_to_con_inc_scope_id_verif_command {
	uint32_t	opcode;
	int32_t		status;
};

/**************************************************************************//**
@Description	OSM Scope Transition to Concurrent Command structure.
		
		This command associated with the new specified scope_id.
		
*//***************************************************************************/
struct osm_scope_tran_to_con_new_scope_id_verif_command {
	uint32_t	opcode;
	uint32_t 	scope_id;
	int32_t		status;
	uint8_t 	pad[4];
};

/**************************************************************************//**
@Description	OSM Scope Relinquish Exclusive Command structure.
		
		This command Proceed from exclusive (XX) to concurrent (XC) stage of 
		the same ordering scope (relinquish exclusivity).
		
*//***************************************************************************/
struct osm_scope_relinquish_ex_verif_command {
	uint32_t	opcode;
	uint8_t 	pad[4];
};

/**************************************************************************//**
@Description	OSM Scope Enter to Exclusive Command structure.
		
		This command increment the stage of the original scope_id (according
		to the \ref OSM_SCOPE_ID_STAGE_INCREMENT_MASK definition).
		
*//***************************************************************************/
struct osm_scope_enter_to_ex_inc_scope_id_verif_command {
	uint32_t	opcode;
	int32_t		status;
};

/**************************************************************************//**
@Description	OSM Scope Enter to Exclusive Command structure.
		
		This command associated with the new specified scope_id.

		
*//***************************************************************************/
struct osm_scope_enter_to_ex_new_scope_id_verif_command {
	uint32_t	opcode;
	uint32_t 	child_scope_id;
	int32_t		status;
	uint8_t 	pad[4];
};


/**************************************************************************//**
@Description	OSM Scope Enter Command structure.
		
		This command enter the next level of ordering scope in the hierarchy
		("child") to either exclusive (XX) or concurrent (XC) stage.
		
*//***************************************************************************/
struct osm_scope_enter_verif_command {
	uint32_t	opcode;
	uint32_t 	scope_enter_flags;
	uint32_t 	child_scope_id;
	int32_t		status;
};

/**************************************************************************//**
@Description	OSM Scope Exit Command structure.
		
		This command exit the current ordering scope and return to its "parent"
		ordering scope.
		
*//***************************************************************************/
struct osm_scope_exit_verif_command {
	uint32_t	opcode;
	uint8_t 	pad[4];
};

/**************************************************************************//**
@Description	OSM get Scope Command structure.
		
		This command returns the current scope status.
		
*//***************************************************************************/
struct osm_get_scope_verif_command {
	uint32_t	opcode;
	uint32_t 	scope_status;
};


/** @} */ /* end of AIOP_OSM_SRs_Verification */

/** @}*/ /* end of AIOP_Service_Routines_Verification */


uint16_t aiop_verification_osm(uint32_t asa_seg_addr);


#endif /* __AIOP_VERIFICATION_OSM_H_ */