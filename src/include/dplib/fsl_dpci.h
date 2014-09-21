/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
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
/*!
 *  @file    fsl_dpci.h
 *  @brief   Data Path Command Interface API
 *
 */
#ifndef __FSL_DPCI_H
#define __FSL_DPCI_H

/*!
 * @Group grp_dpci	Data Path Command Interface API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPCI
 *
 * @{
 */

struct fsl_mc_io;

#define DPCI_PRIO_NUM		2
#define DPCI_VFQID_NOT_VALID	(uint32_t)(-1)

/**
 *
 * @brief	Open object handle - Required after close operation
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	dpci_id - DPCI unique ID
 * @param[out]   token			Token of DPCI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpci_open(struct fsl_mc_io *mc_io, int dpci_id, uint16_t *token);

/**
 *
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed before open
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_close(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing DPCI configuration
 */
struct dpci_cfg {
	uint8_t num_of_priorities;
};

/**
 *
 * @brief	Creates object handle - Required before any operation on the
 *		object; Will allocate resources and preliminary initialization
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]	cfg - configuration parameters
 * @param[out]  token			Token of DPCI object
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	This function should be called only once during
 *		lifetime of the object
 */
int dpci_create(struct fsl_mc_io *mc_io, const struct dpci_cfg *cfg, uint16_t *token);

/**
 *
 * @brief	Free the DPCI object and all its resources.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpci_destroy(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief   DPCI destination types
 */
enum dpci_dest {
	DPCI_DEST_NONE = 0,
	/*!< unassigned destination; i.e. queues will be set in parked mode */
	DPCI_DEST_DPIO,
	/*!< queues will generate notification to the dpio's channel;
	 i.e. will be set in schedule mode and FQDAN enable */
	DPCI_DEST_DPCON
/*!< queues won't generate notification, but will be connected to this
 channel object; i.e. will be set in schedule mode and FQDAN disable */
};

/**
 * @brief	Structure representing DPCI destination parameters
 */
struct dpci_dest_cfg {
	enum dpci_dest type; /*!< destination type */
	uint16_t dpio_id;
	/*!<
	 * DPIO object id; must have a notification channel (either local or
	 * remote)
	 */
	uint16_t dpcon_id;
	/*!< DPCON object id */
	uint8_t priority;
/*!<
 * 0-1 or 0-7 (depends on the channel type) to select the priority
 * (work-queue) within the channel (not relevant for the 'NONE' case)
 */
};

/**
 *
 * @brief	Attach the DPCI to the application
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]	priority - relative number to the DPCI priorities 0-num_of
 *		priorities configured in the dpci_create() call; use 0xFF to
 *		configure all priorities identically.
 * @param[in]	dest_cfg - entry point configuration
 * @param[in]	rx_user_ctx - User context; will be received with the
 *		FD in Rx.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_rx_queue(struct fsl_mc_io *mc_io, uint16_t token,
		      uint8_t priority,
	const struct dpci_dest_cfg *dest_cfg,
	uint64_t rx_user_ctx);

/**
 *
 * @brief	Enable the DPCI, allows sending and receiving frames.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 *
 * @returns	'0' on Success; Error code otherwise.
 * @warning	This function must be called before trying to send any data on
 *		the DPCI queues
 *
 */
int dpci_enable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 *
 * @brief	Disable the DPCI, disallows sending and receiving frames.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_disable(struct fsl_mc_io *mc_io, uint16_t token);

/**
 *
 * @brief	Reset the DPCI, will return to initial state.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_reset(struct fsl_mc_io *mc_io, uint16_t token);

/**
 * @brief	Structure representing attributes of indicated priority queues
 */
struct dpci_prio_attr {
	uint32_t tx_qid;
	/*!< QID to be used for sending frames towards the peer DPCI */
	uint32_t rx_qid;
	/*!< QID to be used for receiving frames from the peer DPCI */
	uint64_t rx_user_ctx;
	/*!< User context to be received with the frames FD on the RX queue */
};

/**
 * @brief	Structure representing DPCI attributes
 */
struct dpci_attr {
	int id;
	/*!< DPCI id */
	struct {
		uint32_t major; /*!< DPCI major version*/
		uint32_t minor; /*!< DPCI minor version*/
	} version;
	/*!< DPCI version */
	uint8_t peer_attached;
	/*!< DPCI is attached to a peer DPCI */
	uint8_t peer_id;
	/*!< DPCI peer id */
	uint8_t num_of_priorities;
	/*!< number of priorities */
	struct dpci_prio_attr dpci_prio_attr[DPCI_PRIO_NUM];
	/*!< priority attributes parameters */
};

/**
 *
 * @brief	Retrieve the DPCI attributes.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_attributes(struct fsl_mc_io *mc_io, uint16_t token, struct dpci_attr *attr);

/**
 *
 * @brief	Retrieve the DPCI link state.
 *
 *		DPCI can be connected to another DPCI, together they
 *		create a 'link'. In order to use the DPCI Tx and Rx queues,
 *		both objects must be enabled.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[out]	up - link status
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 *
 */
int dpci_get_link_state(struct fsl_mc_io *mc_io, uint16_t token, int *up);

/*!
 * @name DPNI IRQ Index and Events
 */
#define DPCI_IRQ_INDEX				0
/*!< Irq index */
#define DPCI_IRQ_EVENT_LINK_CHANGED		0x00000001
/*!< irq event - Indicates that the DPCI link state changed */
/* @} */

/**
 * @brief	Sets IRQ information for the DPCI to trigger an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[out]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_irq(struct fsl_mc_io *mc_io, uint16_t token,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPCI.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_paddr and irq_val are valid);
 * @param[out]	irq_paddr	Physical address that must be written in order
 *				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 *				message-based interrupt
 * @param[in]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_irq(struct fsl_mc_io *mc_io, uint16_t token,
		 uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id);

/**
 * @brief	Sets overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_irq_enable(struct fsl_mc_io *mc_io, uint16_t token,
			uint8_t irq_index,
	uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_irq_enable(struct fsl_mc_io *mc_io, uint16_t token,
			uint8_t irq_index,
	uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_irq_mask(struct fsl_mc_io *mc_io, uint16_t token, uint8_t irq_index, uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_irq_mask(struct fsl_mc_io *mc_io, uint16_t token, uint8_t irq_index, uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpci_get_irq_status(struct fsl_mc_io *mc_io, uint16_t token, uint8_t irq_index, uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	mc_io		Pointer to opaque I/O object
 * @param[in]   token			Token of DPCI object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpci_clear_irq_status(struct fsl_mc_io *mc_io, uint16_t token,
			  uint8_t irq_index,
	uint32_t status);

/** @} */

#endif /* __FSL_DPCI_H */
