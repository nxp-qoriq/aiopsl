/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File          fsl_cwapr.h

@Description   This file contains the AIOP SW CAPWAP Reassembly API

*//***************************************************************************/

#ifndef __FSL_CWAPR_H
#define __FSL_CWAPR_H

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		FSL_CWAPR CWAPR

@Description	AIOP CAPWAP reassembly functions macros and definitions

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	CWAPR_MACROS CWAPR Macros

@Description	CWAPR Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	CWAPR Instance handle Type definition

 *//***************************************************************************/
typedef uint64_t cwapr_instance_handle_t;
typedef uint64_t cwapr_del_arg_t;
typedef uint64_t cwapr_timeout_arg_t;

/**************************************************************************//**
@Description	User callback function, called after cwapr_delete_instance
		function has finished deleting the instance and release all its
		recourses. The user provides this function and the CWAPR process
		invokes it.

@Param[in]	arg - Argument of the callback function.

 *//***************************************************************************/
typedef void /*__noreturn*/ (cwapr_del_cb_t) (cwapr_del_arg_t arg);

/**************************************************************************//**
@Description	User callback function, called after time out expired.
		The user provides this function and the CWAPR process invokes it.

		In case the first fragment (frag offset=0) was received, the
		first fragment will be set as the default frame.
		Otherwise, another fragment will be set as the default frame.
		Default frame will be returned with no open segment.

@Param[in]	arg - Argument of the callback function.
@Param[in]	flags - \link FSL_CWAPRTOCallbackFlags CWAPR Time Out Callback flags
		\endlink,

@Cautions	No task default parameters beside frame_handle will be valid
		(e.g parse result).

 *//***************************************************************************/
typedef void /*__noreturn*/ (cwapr_timeout_cb_t) (cwapr_timeout_arg_t arg,
						uint32_t flags);

/** @} */ /* end of group CWAPR_MACROS */

/**************************************************************************//**
@Group		CWAPR_STRUCTS CWAPR Data Structures

@Description	AIOP CAPWAP reassembly Data Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	CWAPR Parameters
*//***************************************************************************/
struct cwapr_params {
	/** \link FSL_CWAPRInsFlags CAPWAP reassembly flags \endlink */
	uint32_t  flags;
		/** maximum number of frames which can be reassembled concurrently */
	uint32_t  max_open_frames;
		/** maximum reassembled frame size */
	uint16_t  max_reass_frm_size;	
		/** timeout value expressed in units of 10 ms */
	uint16_t  timeout_value;
		/** callback function invoked upon timeout occurrence */
	cwapr_timeout_cb_t *timeout_cb;
		/** argument for timeout callback function */
	cwapr_timeout_arg_t cb_timeout_arg;
		/** if extended_stat flag is set in
		\link FSL_CWAPRInsFlags CAPWAP reassembly flags \endlink,
		this is the address to the statistics data structure
		\ref extended_stats_cntrs. This structure should be allocated
		in DDR.\n
		If the extended_stat flag is reset, this parameter is invalid*/
	uint64_t  extended_stats_addr;
		/** TMAN instance ID to be used for timers creation.
		 * This instance may use up to max_open_frames+1 timers */
	uint8_t	  tmi_id;

};

/**************************************************************************//**
@Description	CWAPR Statistics counters
*//***************************************************************************/

struct cwapr_stats_cntrs {
		/** Counts the number of valid CAPWAP fragments handled */
	uint32_t	valid_frags_cntr;
		/** Counts the number of malformed CAPWAP fragments handled */
		/* TODO: define malformed fragments */
	uint32_t	malformed_frags_cntr;
		/** Counts the number of times the re-assembly process can't
		     start since the number of open CAPWAP reassembled frames
		     has exceeded the max_open_frames */
	uint32_t	open_reass_frms_exceed_cntr;
		/** Counts the number of times that a successful reassembled frame
             length exceeds max_reass_frm_size value*/
	uint32_t 	exceed_max_reass_frm_size;
		/** Counts the number of times the re-assembly process came up
		     against more than 64 fragments per CAPWAP frame */
	uint32_t	more_than_64_frags_cntr;
		/** Counts the number of times the re-assembly process of
		     a CAPWAP frame stopped due to Time Out occurrence */
	uint32_t	time_out_cntr;
};

/** @} */ /* end of group CWAPR_STRUCTS */

/**************************************************************************//**
@Group		FSL_CWAPR_Modes_And_Flags CWAPR Modes And Flags

@Description	AIOP CAPWAP reassembly Modes and Flags

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		FSL_CWAPRInsFlags CWAPR instance flags

@Description CAPWAP reassembly flags.


|      0      |      1     |               |      4-5       | 6-31 |
|-------------|------------|---------------|----------------|------|
|Extended_stat|TO_type     |               | Table Location |      |
\n

@{
*//***************************************************************************/

/** If set, Timeout limits the reassembly time from the first fragment 
 * (opening fragment). If reset, Timeout limits the interval
 *   time between two consecutive fragments.*/
#define CWAPR_MODE_TIMEOUT_TYPE			0x40000000
/** If set, extended statistics is enabled */
#define CWAPR_MODE_EXTENDED_STATS_EN		0x80000000

/** Table is located in Packet Express Buffer memory */
#define CWAPR_MODE_TABLE_LOCATION_PEB		0x08000000

/** Table is located in Data Path DDR memory */
#ifndef LS1088A_REV1
	/** Data Path DDR. Not available on LS1088 */
	/** Tables is located in Data Path DDR memory */
	#define CWAPR_MODE_TABLE_LOCATION_DP_DDR	0x04000000
#endif

/** Tables is located in System DDR memory */
#define CWAPR_MODE_TABLE_LOCATION_SYS_DDR	0x0C000000


/** @} */ /* end of group FSL_CWAPRInsModeBits */

/**************************************************************************//**
@Group		FSL_CWAPRReassReturnStatus CWAPR functions return status

@Description	CWAPR reassemble return values.

@{
*//***************************************************************************/

/** Frame was a regular frame*/
#define CWAPR_REASSEMBLY_REGULAR		0
/** Frame was correctly reassembled */
#define CWAPR_REASSEMBLY_SUCCESS		(CWAPR_MODULE_STATUS_ID + 0x1000)
/** Reassembly isn't completed yet but fragment was successfully added to the
   partially reassembled frame*/
#define CWAPR_REASSEMBLY_NOT_COMPLETED	(CWAPR_MODULE_STATUS_ID + 0x0200)
/** Fragment has been recognized as malformed (overlap, duplicate,
 *  not multiple of 8,...) and wasn't added to the partially reassembled frame*/
#define CWAPR_MALFORMED_FRAG		(CWAPR_MODULE_STATUS_ID + 0x0400)

/** @} */ /* end of group FSL_CWAPRReassReturnStatus */

/**************************************************************************//**
@Group		FSL_CWAPRTOCallbackFlags CWAPR Time Out Callback flags

@Description	CWAPR Time Out Callback flags.

@{
*//***************************************************************************/

/** If set, it indicates that the first fragment (frag offset=0) was received*/
#define CWAPR_TO_CB_FIRST_FRAG		0x01

/** @} */ /* end of group FSL_CWAPRTOCallbackFlags */
/** @} */ /* end of group FSL_CWAPR_Modes_And_Flags */

/**************************************************************************//**
@Group		FSL_CWAPR_Functions CWAPR functions

@Description	CAPWAP reassembly functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	cwapr_early_init

@Description	Reserve the context buffers for all the CWAPR instances.\n

@Param[in]	num_of_instances - Number of CWAPR instances that will be created.
@Param[in]	num_of_context_buffers - Number of context buffers to be
		allocated for all the instances.

@Return		0        - on success \n
		-ENAVAIL - resource not available or not found \n
		-ENOMEM  - not enough memory for requested memory partition \n

@Cautions	In this function, the task yields.
*//***************************************************************************/
int cwapr_early_init(uint32_t num_of_instances, uint32_t num_of_context_buffers);


/**************************************************************************//**
@Function	cwapr_create_instance

@Description	Creates a CWAPR instance.\n

@Param[in]	params - pointer to CWAPR params structure \ref cwapr_params
		The structure pointed by this pointer must be in the task's
		workspace.
@Param[out]	cwapr_instance_handle - Pointer to the CWAPR Instance handle to be used
		by the caller to invoke CAPWAP reassembly functions.
		This pointer should points within workspace.

@Return		Success

@Cautions	This function may result in a fatal error.
@Cautions	In this function, the task yields.
*//***************************************************************************/
int cwapr_create_instance(struct cwapr_params *params,
			    cwapr_instance_handle_t *cwapr_instance_handle);


/**************************************************************************//**
@Function	cwapr_delete_instance

@Description	Delete a specified CWAPR instance.

		The delete request has been registered and the deletion will be
		performed gradually.

@Param[in]	cwapr_instance_handle - The CWAPR instance handle.
@Param[in]	confirm_delete_cb - The function to be used for confirmation
		after all resources associated to the instance have been
		deleted.
@Param[in]	delete_arg - Argument of the confirm callback function.

@Return		Success

@Cautions	This function may result in a fatal error.
@Cautions	In this function, the task yields.
*//***************************************************************************/
int cwapr_delete_instance(cwapr_instance_handle_t cwapr_instance_handle,
			    cwapr_del_cb_t *confirm_delete_cb,
			    cwapr_del_arg_t delete_arg);

/**************************************************************************//**
@Function	cwapr_reassemble

@Description	Perform reassembly.\n
		When called for a regular frame,no action is done.\n
		When called for a non-closing fragment, the fragment is
		inserted to the partially reassembled frame.\n
		When called for a closing fragment, reassembly is done.\n
		The reassembled frame is returned to the caller.

		The function returns with the same ordering scope mode
		it enters (exclusive or concurrent).
				
		In case of completed reassembly, the reassembled frame is
		returned as default frame and segment is presented.\n
		In case of malformed fragment, the presented fragment is
		returned.\n
		In case of reassembly not completed, no open frame is returned,
		no segment is presented.\n

		This function requires one of the four nested scope levels.
				
		It is assumed that the address of the presented segment
		is aligned to 16 bytes.
		
		Implicitly updated values in task defaults: segment length,
							    segment address,
							    segment offset


@Param[in]	cwapr_instance - The CWAPR instance handle.
@Param[in]	tunnel_id - ID used to uniquely identify a CAPWAP tunnel

@Return		Status -\n
		\link FSL_CWAPRReassReturnStatus CAPWAP Reassembly Return status
		\endlink \n
		ETIMEDOUT - Early Time out. Timeout occurred while this fragment
		is proceeded. No fragment is returned.\n
		ENOSPC - Maximum open reassembled frames has been reached.\n
		ENOTSUP - Maximum number of fragments per reassembly has been
			  reached.\n
		EIO - L4 checksum not valid.	  

@Cautions	This function may result in a fatal error.
@Cautions       As part of a workaround to ticket TKT260685 in REV1 this 
                function requires an additional nested scope levels,
                meaning that this function requires two of the four nested
                scope levels.
@Cautions	It is forbidden to call this function when the task
		isn't found in any ordering scope (null scope_id).\n
		If this function is called in concurrent mode, the scope_id is
		incremented.\n
		If this function is called while the task is currently
		in exclusive mode, the scope_id is preserved.\n
		In this function, the task yields.

*//***************************************************************************/
int cwapr_reassemble(cwapr_instance_handle_t cwapr_instance_handle, uint64_t tunnel_id);

/**************************************************************************//**
@Function	cwapr_modify_max_reass_frm_size

@Description	Update max_reass_frm_size parameter for the specified instance.

@Param[in]	cwapr_instance - The CWAPR instance handle.
@Param[in]	max_reass_frm_size - New maximum reassembled frame size.

@Return		None

@Cautions	In this function, the task yields.
*//***************************************************************************/
void cwapr_modify_max_reass_frm_size(cwapr_instance_handle_t cwapr_instance_handle,
				      uint16_t max_reass_frm_size);

/**************************************************************************//**
@Function	cwapr_modify_timeout_value

@Description	Update timeout value for the specified instance.

@Param[in]	cwapr_instance_handle - The CWAPR instance handle.
@Param[in]	timeout_value - New reassembly timeout value

@Return		None

@Cautions	In this function, the task yields.
*//***************************************************************************/
void cwapr_modify_timeout_value(cwapr_instance_handle_t cwapr_instance_handle,
				      uint16_t timeout_value);

/**************************************************************************//**
@Function	cwapr_get_reass_frm_cntr

@Description	Returns the number of reassembled frames counter value
		of the given instance.

@Param[in]	cwapr_instance_handle - The CWAPR instance handle.
@Param[out]	reass_frm_cntr - The number of reassembled frames
		for this instance.
		This pointer should points within workspace.
		
@Return		None

@Cautions	None.
*//***************************************************************************/
void cwapr_get_reass_frm_cntr(cwapr_instance_handle_t cwapr_instance_handle,
				uint32_t *reass_frm_cntr);

/** @} */ /* end of group FSL_CWAPR_Functions */
/** @} */ /* end of group FSL_CWAPR */
/** @} */ /* end of group NETF */

#endif /* __FSL_CWAPR_H */
