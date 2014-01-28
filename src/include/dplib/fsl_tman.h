/**************************************************************************//**
@File          fsl_tman.h

@Description   This file contains the AIOP SW TMAN API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __FSL_TMAN_H
#define __FSL_TMAN_H

#include "common/types.h"


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
@Group		TMANReturnStatus TMAN functions return status

@Description	AIOP TMAN functions return status
@{
*//***************************************************************************/


/** No Error found */
#define TMAN_SUCCESS				SUCCESS

/* @} end of group TMANReturnStatus */

/**************************************************************************//**
@Group		TMANDataStructures TMAN Data Structurs

@Description	AIOP TMAN Data Structurs
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


| 0 - 7  | 8  | 9 |   10 - 11   | 19 | 16- 17- 18| 0 - 15  |
|--------|----|---|-------------|----|-----------|---------|
|        |Type|   |AIOP_priority|TPRI|Granularity|         |

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
#define TMAN_CREATE_TIMER_MODE_TPRI			0x00080000
	/** If set, the timer is a one-shot timer.*/
#define TMAN_CREATE_TIMER_ONE_SHOT			0x00800000

/* The following defines will be used to set the AIOP task priority.*/

	/** Low priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK	0x00000000
	/** Middle priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_MID_PRIORITY_TASK	0x00100000
	/** High priority AIOP task*/
#define TMAN_CREATE_TIMER_MODE_HIGH_PRIORITY_TASK	0x00200000

/* @} end of group TMANTimerCreateModeBits */

/*! \enum e_tman_query_timer Defines the TMAN query timer state.*/
enum e_tman_query_timer {
	/** Timer is non active (in tmi free list) */
	TMAN_TIMER_NON_ACTIVE = 0,
	/** Timer is in expiration queue */
	TMAN_TIMER_EXPIRATION_QUEUE,
	/** Timer has not expired */
	TMAN_TIMER_RUNNING,
	/** Timer is waiting for confirmation */
	TMAN_TIMER_PENDING_CONF
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
		ICID, PL and BDI.

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


@Param[in]	confirmation_epid - EPID used as the index to the
		Entry Point Mapping table to extract the starting PC for
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
int32_t tman_delete_tmi(uint8_t confirmation_epid, uint32_t flags,
			uint8_t tmi_id, uint64_t conf_opaque_data1,
			uint16_t conf_opaque_data2);

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
@Param[in]	epid - EPID used as the index to the Entry Point Mapping table
		to extract the starting PC for the task created upon timer
		expiration.
@Param[in]	scope_id - The ordering scope for the created upon timer
		expiration.
@Param[out]	timer_handle - the handle of the timer for future reference.
		The handle includes the tmi ID and timer ID values.

@Return		Success or Failure(instance_id is not valid or no available
		timer)

@Cautions	This function performs a task switch.

*//***************************************************************************/
int32_t tman_create_timer(uint8_t tmi_id, uint32_t flags,
			uint16_t duration, uint64_t opaque_data1,
			uint16_t opaque_data2, uint8_t epid, uint32_t scope_id,
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

@Description	This function returns the current TAMN timestamp value.

@Param[out]	timestamp - The TMAN timestamp value.

@Return		None.

@Cautions	The TMAN Timestamp do not give the current time as it is not
		synchronized to the 1588 clock.

*//***************************************************************************/
void tman_get_timestamp(uint64_t *timestamp);

/* @} end of group TMAN_Functions */
/* @} end of group TMAN */
/** @} */ /* end of ACCEL */


#endif /* __FSL_TMAN_H */
