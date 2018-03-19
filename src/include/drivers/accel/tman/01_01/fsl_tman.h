/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File          fsl_tman.h

@Description   This file contains the AIOP SW TMAN API (01_01) 
*//***************************************************************************/

#ifndef __FSL_TMAN_H
#define __FSL_TMAN_H

#include "fsl_types.h"
#include "fsl_errors.h"


/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

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
@Group		TMANMacroes TMAN MACROS

@Description	AIOP TMAN Macros
@{
*//***************************************************************************/

/** Macro to get the number of missed expiration for periodic timers.
 * This macro may be called on the periodic timer expiration task. 
 * On REV 1 due to errata ERR009100 this macro will return the accumulated 
 * value of the missed expiration for the elapsed timer. On REV 1, in order to
 * know how many missed expiration happened between the previous expiration and
 * the current expiration the new value need to be subtracted from the old
 * value of the missed expiration. */
#define TMAN_GET_MISSED_EXPIRATION(_fd)					\
	(uint8_t)(uint32_t)({register uint8_t *__rR = 0;		\
	uint8_t err = *((uint8_t *) (((char *)_fd) + FD_BPID_OFFSET));\
	__rR = (uint8_t *) err; })

/** Macro to get the timer handle. This macro may be called on the timer
 *  expiration task or on the TMI confirmation task. */
#define TMAN_GET_TIMER_HANDLE(_fd)					\
	(uint32_t)({register uint32_t *__rR = 0;			\
	uint32_t frc = (LW_SWAP(((char *)_fd) + FD_FRC_OFFSET, 0));	\
	__rR = (uint32_t *) frc; })

/** @} end of group TMANReturnStatus */

#include "tman_inline.h"

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

/** @} end of group TMANDataStructures */

/**************************************************************************//**
@Group		TMAN_Flags TMAN Flags

@Description	AIOP TMAN Flags

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		TMANInsDeleteModeBits TMAN instance delete flags

@Description Instance delete flags.

@{
*//***************************************************************************/

	/** If set, TMI active timers should be forced into the expiration
	     queue although their expiration time was not reached yet. */
#define TMAN_INS_DELETE_MODE_FORCE_EXP 0x1012

/** @} end of group TMANInsDeleteModeBits */

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

/** @} end of group TMANTimerDeleteModeBits */


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
	/** 100 uSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_100_USEC_GRANULARITY	0x00020000
	/** 10 mSec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_10_MSEC_GRANULARITY	0x00040000
	/** 1 Sec timer ticks*/
#define TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY		0x00060000

	/** TMAN Priority. If set, the timer would be treated with higher
	     accuracy and delivered quicker to the expiration queue at the
	     relevant time tick */
#define TMAN_CREATE_TIMER_MODE_TPRI			0x00100000
	/** If set, the timer is a one-shot timer.*/
#define TMAN_CREATE_TIMER_ONE_SHOT			0x10000000

/* The following defines will be used to set the AIOP task priority.*/

	/** High priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_HIGH_PRIORITY_TASK	0x00000000
	/** Middle priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_MID_PRIORITY_TASK	0x01000000
	/** Low priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK	0x02000000

/** @} end of group TMANTimerCreateModeBits */

/** \enum e_tman_query_timer Defines the TMAN query timer state.*/
enum e_tman_query_timer {
	/** The timer is non active (in free timer list) */
	TMAN_TIMER_NON_ACTIVE = 0,
	/** The timer is in non active and also waiting for callback
	 * confirmation */
	TMAN_TIMER_NON_ACTIVE_WAIT_CONF,
	/** The timer is active */
	TMAN_TIMER_RUNNING = 4,
	/** The periodic timer is active. The timer has elapsed and is also 
	 * waiting for callback confirmation */
	TMAN_TIMER_RUNNING_WAIT_CONF,
	/** The timer is being deleted */
	TMAN_TIMER_BEING_DELETED,
	/** The periodic timer is being deleted and also waiting for callback
	 * confirmation */
	TMAN_TIMER_BEING_DELETED_WAIT_CONF
};

/** @} end of group TMAN_Flags */


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
		The size of the allocated memory should be 64*(max_num_of_timers +1)
		bytes. The allocated memory should be 64 byte aligned.\n
@Param[in]	max_num_of_timers - maximum number of timers associated
		to this instance. This number must be bigger than 4 and smaller
		than (2^22)-1.
		This variable should be 3 timers larger than the actual maximum
		number of timers needed in this TMI.
@Param[out]	tmi_id - TMAN instance ID (TMI ID).

@Return		0 on success, or negative value on error.
@Retval		ENOSPC - All TMIs are used. A TMI must be deleted before a new
		one can be created.

@Cautions	This function performs a task switch.
		Please note that the total number of instances that are
		allowed by the TMan is up to 252.
*//***************************************************************************/
int tman_create_tmi(uint64_t tmi_mem_base_addr,
			uint32_t max_num_of_timers, uint8_t *tmi_id);


/**************************************************************************//**
@Function	tman_delete_tmi

@Description	Delete a specified TMAN instance.
		This function issues a TMAN instance delete request and returns
		success in case the request was taken by the TMAN.
		Upon completion of instance deletion, a new task is created
		(confirmation task).
		This latter starts running the function represented by the
		tman_confirm_cb.
		All the timer expiration tasks that were already scheduled by
		the TMAN before this function was called should be confirmed
		using the function tman_timer_completion_confirmation.


@Param[in]	tman_confirm_cb - A callback function used for
		the task created upon completion of the delete tmi.
@Param[in]	flags - \link TMANInsDeleteModeBits TMAN instance
		delete flags \endlink
@Param[in]	tmi_id  - TMAN Instance ID. (TMI ID)
@Param[in]	conf_opaque_data1 - data to be associated with the
		confirmation task.
@Param[in]	conf_opaque_data2 - more data to be associated with the created
		task.


@Return		None.

@Cautions 	All the timers that tmi contains must be deleted before calling to 
			this function due to ERR008205 and ERR009310.
 	 	  
@Cautions	This function performs a task switch.
*//***************************************************************************/
void tman_delete_tmi(tman_cb_t tman_confirm_cb, uint32_t flags,
			uint8_t tmi_id, tman_arg_8B_t conf_opaque_data1,
			tman_arg_2B_t conf_opaque_data2);

/**************************************************************************//**
@Function	tman_create_timer

@Description	Create a TMAN timer.

@Param[in]	tmi_id  - TMAN Instance ID. (TMI ID)
@Param[in]	flags - \link TMANTimerCreateModeBits TMAN timer create
		flags \endlink
@Param[in]	duration - Timer duration time (the number of timer ticks).
		The duration must have a value larger than 10 ticks and smaller
		than 2^16-10 ticks.
@Param[in]	opaque_data1 - Data to be associated with to the created task.
@Param[in]	opaque_data2 - Data to be associated with to the created task.
@Param[in]	tman_timer_cb - A callback function used for the task created
		upon timer expiration.
@Param[out]	timer_handle - the handle of the timer for future reference.
		The handle includes the tmi ID and timer ID values.

@Return		0 on success, or negative value on error.
@Retval		ENOSPC - All timers are used. A timer must be deleted or elapse
		and confirmed before a new one can be created. This error can
		occur when the SW has missed a confirmation for a timer. 
@Retval		EBUSY - The timer was not created due to high TMAN and AIOP
		load.

@Cautions	This function performs a task switch.

*//***************************************************************************/
inline int tman_create_timer(uint8_t tmi_id, uint32_t flags,
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

@Return		0 on success, or negative value on error.
@Retval		ETIMEDOUT - The timer cannot be deleted. The timer aimed to be
		deleted expiration date is currently being processed by the
		TMAN. The timer will elapse shortly. 
		In case of periodic timer the tman_delete_timer should be
		called at the expiration routine to avoid this error. If this
		error do happen for a periodic timer than it is consider as
		a fatal error (the timer period is too short to handle the
		timer expiration callback function).
@Retval		EACCES - The timer cannot be deleted.
		For one shot timer this error should be treated as an ETIMEDOUT
		error.
		For a periodic timer this error should be treated as a fatal
		error (a delete command was already issued for this periodic
		timer). 
@Retval		ENAVAIL - The timer cannot be deleted. The timer is not an
		active one. This should be treated as a fatal error.
		On REV 2 this error will automatically generate a fatal error 
		(Errata ERR008205). 

@Cautions	This function performs a task switch. 
		In case of periodic timer the tman_delete_timer should be
		called at the expiration routine. In Rev 1, for one-shot
		timers, this function should be called after calling the
		tman_recharge_timer function for the one-shot timer (this in
		order to workaround errata ERR009310).

*//***************************************************************************/
inline int tman_delete_timer(uint32_t timer_handle, uint32_t flags);

/**************************************************************************//**
@Function	tman_recharge_timer

@Description	Re-start TMAN one-shot timer to the initial value.

@Param[in]	timer_handle - The handle of the timer to be re-started.

@Return		None

@Cautions	This function performs a task switch. This is a best effort
		function. It is not guaranteed that the timer will be recharged
		after calling this function.
		E.g. The recharge can fail if the timer already elapsed or
		going to elapse in the near future.

*//***************************************************************************/
void tman_recharge_timer(uint32_t timer_handle);

/**************************************************************************//**
@Function	tman_query_timer

@Description	This function returns the state of the specified timer.

@Param[in]	timer_handle - The handle of the timer.
@Param[out]	state - the state of the specified timer
		\ref e_tman_query_timer.

@Return		None.

@Cautions	This function performs a task switch.

*//***************************************************************************/
void tman_query_timer(uint32_t timer_handle,
			enum e_tman_query_timer *state);

/**************************************************************************//**
@Function	tman_timer_completion_confirmation

@Description	This function acknowledges that the task which was created
		upon expiration was consumed.
		This function should be invoked by any timer task.
		When a TMI is deleted this function should also be invoked in
		the tmi delete callback function.

@Param[in]	timer_handle - The handle of the timer. The handle can be
		obtained using the TMAN_GET_TIMER_HANDLE macro (this is true
		also for confirming a TMI deletion).

@Return		None.

@Cautions	None.

*//***************************************************************************/
inline void tman_timer_completion_confirmation(uint32_t timer_handle);


/**************************************************************************//**
@Function	tman_get_timestamp

@Description	This function returns the current TMAN timestamp value.

@Param[out]	timestamp - The TMAN timestamp value expressed in micro seconds.

@Return		None.

@Cautions	The TMAN Timestamp cannot be used for time of day as it is not
		synchronized to the 1588 clock. The TMAN Timestamp value is
		counted	from the time TMAN was initialized (set the INIT bit in
		the TMINIT register).

*//***************************************************************************/
inline void tman_get_timestamp(uint64_t *timestamp);

/**************************************************************************//**
@Function	tman_get_tmi_statistic

@Description	This function returns the requested statistic for the specified
		Timer Instance (TMI).

@Param[in]	tmi_id - Timer Instance ID (TMI ID)
@Param[in]	req_stat - requested statistic \ref e_tman_tmi_statistic

@Return		The counter value of the requested statistic on the
		specified TMI.

*//***************************************************************************/
inline uint32_t tman_get_tmi_statistic(uint8_t tmi_id,
				       enum tman_tmi_statistic stat);

/** @} end of group TMAN_Functions */
/** @} end of group TMAN */
/** @} */ /* end of ACCEL */


#endif /* __FSL_TMAN_H */
