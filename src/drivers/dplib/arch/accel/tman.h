/**************************************************************************//**
@File          tman.h

@Description   This file contains the AIOP SW TMAN definitions

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_TMAN_H
#define __AIOP_TMAN_H

/*! \enum e_tman_cmd_type Defines the TMAN CMDTYPE field.*/
enum e_tman_cmd_type {
	TMAN_CMDTYPE_TMI_CREATE = 0x1000,
	TMAN_CMDTYPE_TMI_DELETE = 0x1011,
	TMAN_CMDTYPE_TMI_DELETE_FORCE = 0x1012,
	TMAN_CMDTYPE_TMI_QUERY = 0x1023,
	TMAN_CMDTYPE_TIMER_CREATE = 0x2010,
	TMAN_CMDTYPE_TIMER_DELETE = 0x2001,
	TMAN_CMDTYPE_TIMER_DELETE_FORCE,
	TMAN_CMDTYPE_TIMER_DELETE_FORCE_WAIT,
	TMAN_CMDTYPE_TIMER_MODIFY,
	TMAN_CMDTYPE_TIMER_RECHARGE,
	TMAN_CMDTYPE_TIMER_QUERY
};

#define TMAN_QUERY_MAX_NT_MASK	0x00FFFFFF
#define TMAN_STATUS_MASK	0xF8000000
/** TMAN Peripheral base address */
#define TMAN_BASE_ADDRESS	0x02020000
/** TMCBCC- TMan Callback Completion Confirmation */
#define TMAN_TMCBCC_ADDRESS	(TMAN_BASE_ADDRESS+0x014)
/** TMTSTMP- TMan TMAN Timestamp register address */
#define TMAN_TMTSTMP_ADDRESS	(TMAN_BASE_ADDRESS+0x020)
/** TMan Dedicated EPID */
#define EPID_TIMER_EVENT_IDX	1
/** Offset to USER_OPAQUE1 in FD */
#define FD_OPAQUE1_OFFSET	0x1E

/**************************************************************************//**
@Description	TMI input extension params
*//***************************************************************************/
struct tman_tmi_input_extention {
		/** data to be associated with the confirmation task. */
	uint64_t	opaque_data1;
		/** Hash value for the confirmation task ScopeID */
	uint32_t	hash;
		/** data to be associated with the confirmation task.
		and EPID */
	uint32_t	opaque_data2_epid;
};


/**************************************************************************//**
@Function	tman_timer_callback

@Description	Callback function, called for every timer expiration and for
		TMI delete completion confirmation.
		This function will call the user function the argument
		specified in the tman_delete_tmi and tman_create_timer
		functions.

@Param		None.

@Return		None.
@Cautions	This is a none return function.

*//***************************************************************************/
void tman_timer_callback(void);

#endif /* __AIOP_TMAN_H */
