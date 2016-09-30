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
@File		fsl_cwap_dtls.h

@Description	This file contains the AIOP CAPWAP DTLS Functional Module API.
*//***************************************************************************/

#ifndef __FSL_CWAP_DTLS_H
#define __FSL_CWAP_DTLS_H

#include "desc/common.h"
#include "desc/tls.h"

/**************************************************************************//**
@Group		NETF NETF (Network Libraries)

@Description	AIOP Accelerator APIs

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_CWAP_DTLS CAPWAP DTLS

@Description	AIOP CAPWAP DTLS Functional Module API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_CWAP_DTLS_MACROS CAPWAP DTLS Macros

@Description	CAPWAP DTLS Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	CAPWAP DTLS handles type definition

*//***************************************************************************/
typedef uint64_t cwap_dtls_sa_handle_t;
typedef uint64_t cwap_dtls_instance_handle_t;

/**************************************************************************//**
@Group	FSL_CWAP_DTLS_MACROS_SA_FLAGS CAPWAP DTLS SA flags

@Description	CAPWAP DTLS SA flags

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	CAPWAP DTLS SA flags

		Use for flags member of \ref cwap_dtls_sa_descriptor_params
		structure.
*//***************************************************************************/

/**
 * Enable DTLS pad check (default = no check).
 * Valid for DTLS decapsulation only.
 * Verifies that padding is correct (as mandated by the TLS RFCs), i.e. "each
 * uint8 in the padding data vector MUST be filled with the padding length
 * value".
 * Caution: the presentation segment size must be large enough to accommodate
 * the entire DTLS padding / trailer. Otherwise an error will be produced.
 * Caution: enabling this option has a performance degradation impact.
 */
#define CWAP_DTLS_FLG_PAD_CHECK		0x00000001

/**
 * Reuse the input frame buffer.
 * If this bit is set, the input frame buffer is used for the output frame.
 * Otherwise a new buffer is allocated and the input buffer is released.
 */
#define CWAP_DTLS_FLG_BUFFER_REUSE	0x00000002

/** @} */ /* end of FSL_CWAP_DTLS_MACROS_SA_FLAGS */

/**************************************************************************//**
@Group		FSL_CWAP_DTLS_MACROS_RET_CODE CAPWAP DTLS return codes

@Description	CAPWAP DTLS return codes.
		Use SUCCESS in case of no error.
@{
*//***************************************************************************/

/** Buffer pool depletion */
#define CWAP_DTLS_BUFFER_POOL_DEPLETION	(CWAP_DTLS_MODULE_STATUS_ID + 0x0040)

/** Sequence Number overflow */
#define CWAP_DTLS_SEQ_NUM_OVERFLOW	(CWAP_DTLS_MODULE_STATUS_ID + 0x0100)

/** Anti-replay check: Late packet */
#define CWAP_DTLS_AR_LATE_PACKET	(CWAP_DTLS_MODULE_STATUS_ID + 0x0200)

/** Anti-replay check: Replay packet */
#define CWAP_DTLS_AR_REPLAY_PACKET	(CWAP_DTLS_MODULE_STATUS_ID + 0x0400)

/** ICV check failed */
#define CWAP_DTLS_ICV_CHK_FAIL		(CWAP_DTLS_MODULE_STATUS_ID + 0x0800)

#define CWAP_DTLS_INTERNAL_ERR		(CWAP_DTLS_MODULE_STATUS_ID + 0x1000)

/** DTLS pad checking error; valid only for decapsulation */
#define CWAP_DTLS_PAD_CHECK_ERR		(CWAP_DTLS_MODULE_STATUS_ID + 0x2000)

/**
 * Decryption validity error
 * The frame after decryption is invalid due to checksum or other header error
 */
#define CWAP_DTLS_DECR_VALIDITY_ERR	(CWAP_DTLS_MODULE_STATUS_ID + 0x4000)

/** General encryption error */
#define CWAP_DTLS_GEN_ENCR_ERR		(CWAP_DTLS_MODULE_STATUS_ID + 0x8000)

/** General decryption error */
#define CWAP_DTLS_GEN_DECR_ERR		(CWAP_DTLS_MODULE_STATUS_ID + 0x10000)

/** @} */ /* end of group FSL_CWAP_DTLS_MACROS_RET_CODE */
/** @} */ /* end of FSL_CWAP_DTLS_MACROS */

/**************************************************************************//**
@Group		FSL_CWAP_DTLS_STRUCTS CAPWAP DTLS Structures

@Description	CAPWAP DTLS Structures

@{
*//***************************************************************************/

/**************************************************************************//**
 @Description   CAPWAP DTLS Descriptor Parameters
*//***************************************************************************/
struct cwap_dtls_sa_descriptor_params {
	/** CAPWAP DTLS SA flags */
	uint32_t flags;
	/** DTLS direction, cipher suite  */
	struct protcmd protcmd;
	/** DTLS protocol data block */
	union {
		struct tls_block_pdb cbc;
		struct tls_gcm_pdb gcm;
	} pdb;
	/**
	 * cipher algorithm information; alginfo.algtype must be filled
	 * even though protcmd.protinfo indicates the DTLS cipher suite
	 */
	struct alginfo cipherdata;
	/**
	 * authentication algorithm information; alginfo.algtype must be filled
	 * even though protcmd.protinfo indicates the DTLS cipher suite
	 */
	struct alginfo authdata;
	/** Storage Profile ID of the CAPWAP DTLS output frame */
	uint16_t spid;
};

/** @} */ /* end of FSL_CWAP_DTLS_STRUCTS */

/**************************************************************************//**
@Group		FSL_CWAP_DTLS_Functions CAPWAP DTLS Functions

@Description	AIOP CAPWAP DTLS Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	cwap_dtls_drv_init

@Description	Function fills the CWAP/DTLS dedicated BP, if that is requested
		in the application. It is called from the AIOP initialization
		task, in the global initialization stage.
		Note : The application requests the dedicated BP creation by
		setting the "CWAP_DTLS_BUFFER_ALLOCATE_ENABLE" application
		configuration flag.

@Return		0 - success
		-ENOMEM - not enough memory
		-other error code - otherwise
*//****************************************************************************/
int cwap_dtls_drv_init(void);

/**************************************************************************//**
@Function	cwap_dtls_early_init

@Description	This function should be called at the AIOP "early init" stage,
		for declaring the amount of instances and SAs which are going to
		be used throughout the application lifetime.

@Param[in]	total_instance_num - the maximum total number of CAPWAP DTLS
		instances that may be used.
		This is the maximum number of instances that can be valid at a
		given time.
@Param[in]	total_committed_sa_num - the total sum of all committed SAs of
		all CAPWAP DTLS instances that may be used.
		total_committed_sa_num =
			SUM(instance[1 .. total_instance_num].committed_sa_num)
@Param[in]	total_max_sa_num - the total sum of all maximum SAs number of
		all CAPWAP DTLS instances that may be used.
		total_max_sa_num =
			SUM(instance[1 .. total_instance_num].max_sa_num)

@Return		0 - success
		-ENAVAIL - resource not available or not found,
		-ENOMEM  - not enough memory for requested memory partition

*//****************************************************************************/
int cwap_dtls_early_init(uint32_t total_instance_num,
			 uint32_t total_committed_sa_num,
			 uint32_t total_max_sa_num);

/**************************************************************************//**
@Function	cwap_dtls_create_instance

@Description	This function creates an instance for a CAPWAP DTLS application.
		It should be called once when the application goes up.
		All SAs belonging to this instance should be called with the
		returned instance handle.

@Param[in]	committed_sa_num - committed number of SAs for this instance.
		Resources for this number of SAs are preallocated, thus
		\ref cwap_dtls_add_sa_descriptor() cannot fail on depletion.
@Param[in]	max_sa_num - maximum number of SAs to be used by this instance.
		Resources for additional SAs are allocated at run time on, thus
		\ref cwap_dtls_add_sa_descriptor() may fail on depletion.
@Param[out]	instance_handle - instance handle

@Return		0 - success
		-ENOMEM - not enough memory for partition
		-ENOSPC	- unable to allocate due to depletion

*//****************************************************************************/
int cwap_dtls_create_instance(uint32_t committed_sa_num, uint32_t max_sa_num,
			      cwap_dtls_instance_handle_t *instance_handle);

/**************************************************************************//**
@Function	cwap_dtls_delete_instance

@Description	This function deletes an instance of a CAPWAP DTLS application.
		It should be called once, only after all SA descriptors
		belonging to this instance were deleted.

@Param[out]	instance_handle - instance handle

@Return		0 - success
		-ENAVAIL - instance does not exist
		-EPERM - trying to delete an instance before deleting all SAs

*//****************************************************************************/
int cwap_dtls_delete_instance(cwap_dtls_instance_handle_t instance_handle);

/**************************************************************************//**
@Function	cwap_dtls_add_sa_descriptor

@Description	This function configures the internal SA management structures:
		the CAPWAP DTLS flow context, the SEC Shared Descriptor etc.
		Implicit input: BPID in the SRAM (internal usage).

@Param[in]	params - pointer to descriptor parameters
@Param[in]	instance_handle - CAPWAP DTLS instance handle achieved with
		cwap_dtls_create_instance()
@Param[out]	sa_handle - CAPWAP DTLS SA handle

@Return		0 - success
		-ENOSPC	- unable to allocate resources due to memory depletion
		-EPERM - trying to allocate more than maximum SAs for instance
		-ENAVAIL - unable to create SA descriptor

*//****************************************************************************/
int cwap_dtls_add_sa_descriptor(struct cwap_dtls_sa_descriptor_params *params,
				cwap_dtls_instance_handle_t instance_handle,
				cwap_dtls_sa_handle_t *sa_handle);

/**************************************************************************//**
@Function	cwap_dtls_del_sa_descriptor

@Description	This function performs SA resource deallocation.
		Implicit input: BPID in the SRAM (internal usage).

@Param[in]	sa_handle - CAPWAP DTLS SA handle

@Return		0 - success
		-ENAVAIL - SA/Instance not found
		-EPERM - trying to delete SA descriptor from empty instance

*//****************************************************************************/
int cwap_dtls_del_sa_descriptor(cwap_dtls_sa_handle_t sa_handle);

/**************************************************************************//**
@Function	cwap_dtls_get_ar_info

@Description	This function returns anti-replay related information:
		- DTLS sequence number and epoch
		- Anti-replay bitmap (scorecard) (if applicable)

@Param[in]	sa_handle - CAPWAP DTLS SA handle
@Param[out]	sequence_number - 64-bit value consisting of DTLS epoch (upper
		16 bits) and sequence number (lower 48 bits).
@Param[out]	anti_replay_bitmap - Anti-replay bitmap (4 words):
		* For 1-entry ARS, only the first word is valid
		* For 2-entry ARS, only the first two words are valid
		* For 4-entry ARS, all four words are valid

@Cautions	anti_replay_bitmap is relevant for inbound (decapsulation) only,
		and should be ignored for outbound (encapsulation).

*//****************************************************************************/
void cwap_dtls_get_ar_info(cwap_dtls_sa_handle_t sa_handle,
			   uint64_t *sequence_number,
			   uint32_t anti_replay_bitmap[4]);

/**************************************************************************//**
@Function	cwap_dtls_frame_decrypt

@Description	This function performs DTLS decryption, according to RFC4347 for
		DTLS 1.0, RFC6347 for DTLS 1.2.
		The function also updates the decrypted frame parser result.
		Default frame is used as input frame.

@Param[in]	sa_handle - CAPWAP DTLS SA handle

@Return		Status -\n
		\link FSL_CWAP_DTLS_MACROS_RET_CODE CAPWAP DTLS return status
		\endlink \n

@Cautions	User should note the following:
		 - In this function the task yields.
		TODO: add more details here once implementation is complete.
*//****************************************************************************/
int cwap_dtls_frame_decrypt(cwap_dtls_sa_handle_t sa_handle);

/**************************************************************************//**
@Function	cwap_dtls_frame_encrypt

@Description	This function performs DTLS encryption, according to RFC4347 for
		DTLS 1.0, RFC6347 for DTLS 1.2.
		The function also updates the encrypted frame parser result.
		Default frame is used as input frame.

@Param[in]	sa_handle - CAPWAP DTLS SA handle

@Return		Status -\n
		\link FSL_CWAP_DTLS_MACROS_RET_CODE CAPWAP DTLS return status
		\endlink \n

@Cautions	User should note the following:
		- In this function the task yields.
		TODO: add more details here once implementation is complete.
*//****************************************************************************/
int cwap_dtls_frame_encrypt(cwap_dtls_sa_handle_t sa_handle);

/** @} */ /* end of FSL_CWAP_DTLS_Functions */
/** @} */ /* end of FSL_CWAP_DTLS */
/** @} */ /* end of NETF */

#endif /* __FSL_CWAP_DTLS_H */
