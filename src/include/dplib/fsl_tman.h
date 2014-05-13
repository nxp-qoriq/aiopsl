/**************************************************************************//**
@File          fsl_tman.h

@Description   This file contains the AIOP SW TMAN API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __FSL_TMAN_H
#define __FSL_TMAN_H

#include "common/types.h"
#include "common/errors.h"


/**************************************************************************//**
 @Group		ACCEL ACCEL (Accelerator APIs)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		TMAN TMAN

@Description	AIOP TMAN functions macros and definitions

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TMAN Timer expiration task arguments Type definition

*//***************************************************************************/

typedef uint64_t tman_arg_8B_t;
typedef uint16_t tman_arg_2B_t;

/**************************************************************************//**
@Description	User callback function, called after ipr_delete_instance
		function has finished deleting the instance and release all its
		recourses. The user provides this function and the IPR process
		invokes it.

@Param[in]	arg - Argument of the callback function.

 *//***************************************************************************/
typedef void /*__noreturn*/ (*tman_cb_t) (
					tman_arg_8B_t arg1,
					tman_arg_2B_t arg2);

/**************************************************************************//**
@Group		TMANReturnStatus TMAN functions return status

@Description	AIOP TMAN functions return status
@{
*//***************************************************************************/

/**************************************************************************//**
 @enum tman_tmi_status

 @Description	AIOP TMAN create TMI command status codes.

 @{
*//***************************************************************************/
enum tman_tmi_create_status {
	/** Success. */
	TMAN_TMI_CREATE_SUCCESS = E_OK,
	/** All TMIs are used. A TMI must be deleted before a new one can
		be created. */
	TMAN_TMIID_DEPLETION_ERR = 0x814000FF
};

/* @} end of enum tman_tmi_status */

/**************************************************************************//**
 @enum tman_timer_create_status

 @Description	AIOP TMAN create timer command status codes.

 @{
*//***************************************************************************/
enum tman_timer_create_status {
	/** Success. */
	TMAN_TMR_CREATE_SUCCESS = E_OK,
	/** illegal Timer Create Fields Description[DURATION] */
	TMAN_ILLEGAL_DURATION_VAL_ERR = 0x81400010,
	/** A non active TMI was provided as an input */
	TMAN_TMI_NOT_ACTIVE_ERR = 0x81C00010,
	/** No more available timers in the TMI */
	TMAN_TMR_DEPLETION_ERR = 0x81C00020
};

/* @} end of enum tman_timer_create_status */

/**************************************************************************//**
 @enum tman_timer_delete_status

 @Description	AIOP TMAN delete timer command status codes.

 @{
*//***************************************************************************/
enum tman_timer_delete_status {
	/** Success. */
	TMAN_DEL_TMR_DELETE_SUCCESS = E_OK,
	/** A non active timer was provided as an input */
	TMAN_DEL_TMR_NOT_ACTIVE_ERR = 0x81400050,
	/** The one shot timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_DEL_CCP_WAIT_ERR = 0x81400051,
	/** The periodic timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_DEL_PERIODIC_CCP_WAIT_ERR = 0x81400055,
	/** A delete command was already issued for this timer and the TMAN is
	 * in the process of deleting the timer. The timer will elapse in the
	 * future. */
	TMAN_DEL_TMR_DEL_ISSUED_ERR = 0x81400056,
	/** A delete command was already issued. The timer has already elapsed
	 * for the last time and it is pending a completion confirmation
	 * (done by calling the tman_timer_completion_confirmation function) */
	TMAN_DEL_TMR_DEL_ISSUED_CONF_ERR = 0x81400057,
	/** Timer is not deleted due to permanent error */
	TMAN_DEL_PERMANENT_ERR = 0x81C00010
};

/* @} end of enum tman_timer_delete_status */

/**************************************************************************//**
 @enum tman_timer_mod_status

 @Description	AIOP TMAN increase timer duration command status codes.

 @{
*//***************************************************************************/
enum tman_timer_mod_status {
	/** Success. */
	TMAN_MOD_TMR_SUCCESS = E_OK,
	/** illegal Timer Create Fields Description[DURATION] */
	TMAN_MOD_ILLEGAL_DURATION_VAL_ERR = 0x81400010,
	/** A non active timer was provided as an input */
	TMAN_MOD_TMR_NOT_ACTIVE_ERR = 0x81400060,
	/** The one shot timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_MOD_CCP_WAIT_ERR = 0x81400061,
	/** The periodic timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_MOD_PERIODIC_CCP_WAIT_ERR = 0x81400065,
	/** A delete command was already issued for this timer and the TMAN is
	 * in the process of deleting the timer. The timer will elapse in the
	 * future. */
	TMAN_MOD_TMR_DEL_ISSUED_ERR = 0x81400066,
	/** A delete command was already issued. The timer has already elapsed
	 * for the last time and it is pending a completion confirmation
	 * (done by calling the tman_timer_completion_confirmation function) */
	TMAN_MOD_TMR_DEL_ISSUED_CONF_ERR = 0x81400067,
	/** Timer is not modified due to permanent error */
	TMAN_MOD_PERMANENT_ERR = 0x81C00010
};

/* @} end of enum tman_timer_mod_status */

/**************************************************************************//**
 @enum tman_timer_rech_status

 @Description	AIOP TMAN timer recharge command status codes.

 @{
*//***************************************************************************/
enum tman_timer_recharge_status {
	/** Success. */
	TMAN_REC_TMR_SUCCESS = E_OK,
	/** A non active timer was provided as an input */
	TMAN_REC_TMR_NOT_ACTIVE_ERR = 0x81400070,
	/** The one shot timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_REC_CCP_WAIT_ERR = 0x81400071,
	/** The periodic timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_REC_PERIODIC_CCP_WAIT_ERR = 0x81400075,
	/** A delete command was already issued for this timer and the TMAN is
	 * in the process of deleting the timer. The timer will elapse in the
	 * future. */
	TMAN_REC_TMR_DEL_ISSUED_ERR = 0x81400076,
	/** A delete command was already issued. The timer has already elapsed
	 * for the last time and it is pending a completion confirmation
	 * (done by calling the tman_timer_completion_confirmation function) */
	TMAN_REC_TMR_DEL_ISSUED_CONF_ERR = 0x81400077,
	/** Timer is not recharged due to permanent error */
	TMAN_REC_PERMANENT_ERR = 0x81C00010
};

/* @} end of enum tman_timer_rech_status */

/**************************************************************************//**
 @enum tman_timer_query_status

 @Description	AIOP TMAN timer query command status codes.

 @{
*//***************************************************************************/
enum tman_timer_query_status {
	/** Success. */
	TMAN_QUE_TMR_SUCCESS = E_OK,
	/** A non active timer was provided as an input */
	TMAN_QUE_TMR_NOT_ACTIVE_ERR = 0x81400080,
	/** The one shot timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_QUE_CCP_WAIT_ERR = 0x81400081,
	/** The periodic timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_QUE_PERIODIC_CCP_WAIT_ERR = 0x81400085,
	/** A delete command was already issued for this timer and the TMAN is
	 * in the process of deleting the timer. The timer will elapse in the
	 * future. */
	TMAN_QUE_TMR_DEL_ISSUED_ERR = 0x81400086,
	/** A delete command was already issued. The timer has already elapsed
	 * for the last time and it is pending a completion confirmation
	 * (done by calling the tman_timer_completion_confirmation function) */
	TMAN_QUE_TMR_DEL_ISSUED_CONF_ERR = 0x81400087,
	/** Timer is not queried due to permanent error */
	TMAN_QUE_PERMANENT_ERR = 0x81C00010
};

/* @} end of enum tman_timer_query_status */


/* @} end of group TMANReturnStatus */


/**************************************************************************//**
@Group		TMANMacroes TMAN MACROES

@Description	AIOP TMAN Macroes
@{
*//***************************************************************************/

/** Macro to get the number of missed expiration for periodic timers.
 * This macro may be called on the periodic timer expiration task. */
#define TMAN_GET_MISSED_EXPIRATION(_fd)					\
	(uint8_t)(uint32_t)({register uint8_t *__rR = 0;		\
	uint8_t err = *((uint8_t *) (((char *)_fd) + FD_BPID_OFFSET));\
	__rR = (uint8_t *) err; })

/** Macro to get the timer handle. This macro may be called on the timer
 *  expiration task or on the TMI confirmation task. */
#define TMAN_GET_TIMER_HANDLE(_fd)					\
	(uint32_t)({register uint32_t *__rR = 0;			\
	uint32_t frc = (LW_SWAP(((char *)_fd) + FD_FRC_OFFSET));	\
	__rR = (uint32_t *) frc; })

/* @} end of group TMANReturnStatus */


/**************************************************************************//**
@Group		TMANDataStructures TMAN Data Structures

@Description	AIOP TMAN Data Structures
@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TMI Params Configuration
*//***************************************************************************/
struct tman_tmi_params {
		/** Reserved for compliance with HW format.
		    User should not access this field. */
	uint32_t	reserved0;
		/** Maximum number of timers associated with this instance */
	uint32_t	max_num_of_timers;
		/** Address to the memory used for the timers
		    associated with this instance */
	uint64_t	tmi_mem_base_addr;
};

/* @} end of group TMANDataStructures */

/**************************************************************************//**
@Group		TMAN_Modes TMAN Modes

@Description	AIOP TMAN Modes

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		TMANInsDeleteModeBits TMAN instance delete flags

@Description Instance delete flags.

@{
*//***************************************************************************/

	/** If set, TMI active timers will be deleted without creating new
	     expiration tasks. */
#define TMAN_INS_DELETE_MODE_WO_EXPIRATION 0x1011
	/** If set, TMI active timers should be forced into the expiration
	     queue although their expiration time was not reached yet. */
#define TMAN_INS_DELETE_MODE_FORCE_EXP 0x1012

/* @} end of group TMANInsDeleteModeBits */

/**************************************************************************//**
@Group		TMANTimerDeleteModeBits TMAN timer delete flags

@Description Timer delete flags.

@{
*//***************************************************************************/

	/** If set, the timer will be deleted without creating any expiration
	     task. */
#define TMAN_TIMER_DELETE_MODE_WO_EXPIRATION 0x2001
	/** If set, the timer will be forced into the expiration
	     queue although its expiration time was not reached yet. */
#define TMAN_TIMER_DELETE_MODE_FORCE_EXP 0x2002
	/** If set, the timer will be deleted after its next expiration.
	     Timer Id will be returned to free pool after callback
	     completion confirmation. */
#define TMAN_TIMER_DELETE_MODE_WAIT_EXP 0x2003

/* @} end of group TMANTimerDeleteModeBits */


/**************************************************************************//**
@Group		TMANTimerCreateModeBits TMAN timer create flags

@Description Timer create flags.


| 0 - 2  | 3  | 4-5 |    6 - 7    | 8-10 | 11 | 12 | 13- 14- 15| 0 - 15  |
|--------|----|-----|-------------|------|----|----|-----------|---------|
|        |Type|     |AIOP_priority|      |TPRI|    |Granularity|         |

@{
*//***************************************************************************/

/* The following defines will be used to set the TMAN timer tick size.*/

	/** 1 uSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_USEC_GRANULARITY		0x00000000
	/** 10 uSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_10_USEC_GRANULARITY	0x00010000
	/** 100 uSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_100_USEC_GRANULARITY	0x00020000
	/** 1 mSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_MSEC_GRANULARITY		0x00030000
	/** 10 mSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_10_MSEC_GRANULARITY	0x00040000
	/** 100 mSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_100_MSEC_GRANULARITY	0x00050000
	/** 1 Sec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY		0x00060000

	/** TMAN Priority. If set, the timer would be treated with higher
	     accuracy and delivered quicker to the expiration queue at the
	     relevant time tick */
#define TMAN_CREATE_TIMER_MODE_TPRI			0x00100000
	/** If set, the timer is a one-shot timer.*/
#define TMAN_CREATE_TIMER_ONE_SHOT			0x10000000

/* The following defines will be used to set the AIOP task priority.*/

	/** Low priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK	0x00000000
	/** Middle priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_MID_PRIORITY_TASK	0x01000000
	/** High priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_HIGH_PRIORITY_TASK	0x02000000

/* @} end of group TMANTimerCreateModeBits */

/*! \enum e_tman_query_timer Defines the TMAN query timer state.*/
enum e_tman_query_timer {
	/** The timer is non active (in free timer list) */
	TMAN_TIMER_NON_ACTIVE = 0,
	/** The timer is in non active and waiting for callback confirmation */
	TMAN_TIMER_NON_ACTIVE_WAIT_CONF,
	/** The timer is active */
	TMAN_TIMER_RUNNING = 4,
	/** The timer is active. The timer has elapsed and waiting for callback
	 *  confirmation */
	TMAN_TIMER_RUNNING_WAIT_CONF,
	/** The timer is being deleted */
	TMAN_TIMER_BEING_DELETED,
	/** The timer is being deleted and waiting for callback confirmation */
	TMAN_TIMER_BEING_DELETED_WAIT_CONF
};

/* @} end of group TMAN_Modes */


/**************************************************************************//**
@Group		TMAN_Functions TMAN functions

@Description	TMAN functions

@{
*//***************************************************************************/


/**************************************************************************//**
@Function	tman_create_tmi

@Description	Creates an TMAN instance.
		Implicit input parameters:
		ICID, VA, PL and BDI.

@Param[in]	tmi_mem_base_addr - address to memory used for the timers
		associated with this instance.
		The size of the allocated memory should be 64*max_num_of_timers
		bytes. The allocated memory should be 64 byte aligned.\n
@Param[in]	max_num_of_timers - maximum number of timers associated
		to this instance.
@Param[out]	tmi_id - TMAN instance ID (TMI ID).

@Return		Success or Failure (There is no available TMI ID).

@Cautions	This function performs a task switch.
		Please note that the total number of instances that are
		allowed by the TMan is up to 252.
*//***************************************************************************/
int32_t tman_create_tmi(uint64_t tmi_mem_base_addr,
			uint32_t max_num_of_timers, uint8_t *tmi_id);


/**************************************************************************//**
@Function	tman_delete_tmi

@Description	Delete a specified TMAN instance.
		This function issues a TMAN instance delete request and returns
		success in case the request was taken by the TMAN.
		Upon completion of instance deletion, a new task is created
		(confirmation task).
		This latter starts running the function represented by the
		confirmation_epid.


@Param[in]	tman_confirm_cb - A callback function used for
		the task created upon completion of the delete tmi.
@Param[in]	flags - \link TMANInsDeleteModeBits TMAN instance
		delete flags \endlink
@Param[in]	tmi_id  - TMAN Instance ID. (TMI ID)
@Param[in]	conf_opaque_data1 - data to be associated with the
		confirmation task.
@Param[in]	conf_opaque_data2 - more data to be associated with the created
		task.


@Return		Success or Failure(in case instance_id is not valid)

@Cautions	This function performs a task switch.
*//***************************************************************************/
int32_t tman_delete_tmi(tman_cb_t tman_confirm_cb, uint32_t flags,
			uint8_t tmi_id, tman_arg_8B_t conf_opaque_data1,
			tman_arg_2B_t conf_opaque_data2);

/**************************************************************************//**
@Function	tman_query_tmi

@Description	Returns all the parameters associated with the specified
		instance.

@Param[in]	tmi_id  - TMAN Instance ID. (TMI ID)
@Param[in,out]	output_ptr  - pointer to a \link tman_tmi_params
		TMI params structure \endlink
		where the instance's parameters will be returned.

@Return		Success or Failure(in case instance_id is not valid)

@Cautions	This function performs a task switch.

*//***************************************************************************/
int32_t tman_query_tmi(uint8_t tmi_id,
			struct tman_tmi_params *output_ptr);

/**************************************************************************//**
@Function	tman_create_timer

@Description	Create a TMAN timer.

@Param[in]	tmi_id  - TMAN Instance ID. (TMI ID)
@Param[in]	flags - \link TMANTimerCreateModeBits TMAN timer create
		flags \endlink
@Param[in]	duration - Timer duration time (the number of timer ticks).
@Param[in]	opaque_data1 - Data to be associated with to the created task.
@Param[in]	opaque_data2 - Data to be associated with to the created task.
@Param[in]	tman_timer_cb - A callback function used for the task created
		upon timer expiration.
@Param[out]	timer_handle - the handle of the timer for future reference.
		The handle includes the tmi ID and timer ID values.

@Return		Success or Failure(instance_id is not valid or no available
		timer)

@Cautions	This function performs a task switch.

*//***************************************************************************/
int32_t tman_create_timer(uint8_t tmi_id, uint32_t flags,
			uint16_t duration, tman_arg_8B_t opaque_data1,
			tman_arg_2B_t opaque_data2, tman_cb_t tman_timer_cb,
			uint32_t *timer_handle);

/**************************************************************************//**
@Function	tman_delete_timer

@Description	Delete a TMAN timer.
		This function issues a TMAN timer delete request.

@Param[in]	timer_handle - The handle of the timer to be deleted.
@Param[in]	flags - \link TMANTimerDeleteModeBits TMAN timer
		delete flags \endlink.

@Return		Success or Failure(in case the one-shot timer already expired).

@Cautions	This function performs a task switch.

*//***************************************************************************/
int32_t tman_delete_timer(uint32_t timer_handle, uint32_t flags);

/**************************************************************************//**
@Function	tman_increase_timer_duration

@Description	Increase TMAN timer duration time.

@Param[in]	timer_handle - The handle of the timer to be modified.
@Param[in]	duration - new timer duration time (the number of timer ticks).

@Return		Success or Failure(tmi/timer is not existing or
		expired one-shot timer).

@Cautions	The new duration can only be a bigger number than the original
		one.
		The granularity value for the timer remains the same.
		In case where a granularity change is also needed,
		the timer should be deleted and re-created with the new
		granularity factor.

@Cautions	This function performs a task switch.

*//***************************************************************************/
int32_t tman_increase_timer_duration(uint32_t timer_handle, uint16_t duration);

/**************************************************************************//**
@Function	tman_recharge_timer

@Description	Re-start TMAN timer to the initial value.

@Param[in]	timer_handle - The handle of the timer to be re-started.

@Return		Success or Failure(tmi/timer is not existing).

@Cautions	This function performs a task switch.

*//***************************************************************************/
int32_t tman_recharge_timer(uint32_t timer_handle);

/**************************************************************************//**
@Function	tman_query_timer

@Description	This function returns the state of the specified timer.

@Param[in]	timer_handle - The handle of the timer.
@Param[out]	state - the state of the specified timer
		\ref e_tman_query_timer.

@Return		Success or Failure(tmi/timer is not existing).

@Cautions	This function performs a task switch.

*//***************************************************************************/
int32_t tman_query_timer(uint32_t timer_handle,
			enum e_tman_query_timer *state);

/**************************************************************************//**
@Function	tman_timer_completion_confirmation

@Description	This function acknowledges that the task which was created
		upon expiration was consumed.
		This function should be invoked by any timer task.

@Param[in]	timer_handle - The handle of the timer.

@Return		None.

@Cautions	None.

*//***************************************************************************/
void tman_timer_completion_confirmation(uint32_t timer_handle);


/**************************************************************************//**
@Function	tman_get_timestamp

@Description	This function returns the current TMAN timestamp value.

@Param[out]	timestamp - The TMAN timestamp value expressed in micro seconds.

@Return		None.

@Cautions	The TMAN Timestamp cannot be used for time of day as it is not
		synchronized to the 1588 clock.

*//***************************************************************************/
void tman_get_timestamp(uint64_t *timestamp);

/* @} end of group TMAN_Functions */
/* @} end of group TMAN */
/** @} */ /* end of ACCEL */


#endif /* __FSL_TMAN_H */
