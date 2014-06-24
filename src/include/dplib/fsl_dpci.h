/* Copyright 2014 Freescale Semiconductor, Inc. */
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

#ifdef MC
struct dpci;
#else
struct dpci {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Must be set by the user
	 */
	int auth; /*!< authentication ID */
};
#endif

#define DPCI_PRIO_NUM		2
#define DPCI_VFQID_NOT_VALID	(uint32_t)(-1)

/**
 *
 * @brief	Open object handle - Required after close operation
 *
 * @param[in]	dpci - Pointer to dpci object
 * @param[in]	dpci_id - DPCI unique ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpci_open(struct dpci *dpci, int dpci_id);

/**
 *
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed before open
 *
 * @param[in]	dpci - Pointer to dpci object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_close(struct dpci *dpci);

/**
 * @brief	Structure representing DPCI configuration
 */
struct dpci_cfg {
	uint8_t num_of_Priorities;
};

/**
 *
 * @brief	Creates object handle - Required before any operation on the
 *		object; Will allocate resources and preliminary initialization
 *
 * @param[in]	dpci - Pointer to dpci object
 * @param[in]	cfg - configuration parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	This function should be called only once during
 *		lifetime of the object
 */
int dpci_create(struct dpci *dpci, const struct dpci_cfg *cfg);

/**
 *
 * @brief	Free the DPCI object and all its resources.
 *
 * @param[in]	dpci - Pointer to dpci object
 *
 * @returns	'0' on Success; error code otherwise.
 */
int dpci_destroy(struct dpci *dpci);

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
 * @param[in]	dpci - Pointer to dpci object
 * @param[in]	priority - priority of requests to AIOP and of the returning
 *		queue (rx queue); use 0xFF to configure all priorities
 *		identically.
 * @param[in]	dest_cfg - attach configuration
 * @param[in]	rx_user_ctx - User context; will be received with the
 *		FD in Rx.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_rx_queue(struct dpci *dpci,
		      uint8_t priority,
	const struct dpci_dest_cfg *dest_cfg,
	uint64_t rx_user_ctx);

/**
 *
 * @brief	Dettach the DPCI to the application
 *
 * @param[in]	dpci - Pointer to dpci object
 * @param[in]	priority - priority to dettach
 *
 * @returns	'0' on Success; Error code otherwise.
 */

int dpci_free_rx_queue(struct dpci *dpci, uint8_t priority);

/**
 *
 * @brief	Enable the DCPI, will allow sending and receiving frames.
 *
 * @param[in]	dpci - Pointer to dpci object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_enable(struct dpci *dpci);

/**
 *
 * @brief	Disable the DCPI, will disallow sending and receiving frames.
 *
 * @param[in]	dpci - Pointer to dpci object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_disable(struct dpci *dpci);

/**
 *
 * @brief	Reset the DCPI, will return to initial state.
 *
 * @param[in]	dpci - Pointer to dpci object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_reset(struct dpci *dpci);

/**
 * @brief	Structure representing priority attributes parameters
 */
struct dpci_prio_attr {
	uint32_t tx_qid;
	/*!< QID to be used for sending frames towards the AIOP */
	uint32_t rx_qid;
	/*!< QID to be used for reciving frames from the AIOP */
	uint64_t rx_user_ctx;
	/*!< User context to be received with the frames FD on the RX queue */
};

/**
 * @brief	Structure representing attributes parameters
 */
struct dpci_attr {
	int id;
	/*!< DPCI id */
	int enable;
	/*!< enable */
	uint8_t peer_attached;
	/*!< DPCI is attached to a peer DPCI*/
	uint8_t peer_id;
	/*!< DPCI peer id */
	uint8_t num_of_priorities;
	/*!< number of priorities */
	struct dpci_prio_attr dpci_prio_attr[DPCI_PRIO_NUM];
	/*!< priority attributes parameters */
	struct {
		uint32_t major; /*!< DPCI major version*/
		uint32_t minor; /*!< DPCI minor version*/
	} version;
	/*!< DPCI version */
};

/**
 *
 * @brief	Retrieve a single priority attributes.
 *
 * @param[in]	dpci - Pointer to dpci object
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_attributes(struct dpci *dpci, struct dpci_attr *attr);

/**
 *
 * @brief	Retrieve the DPCI state of the link to other DPCI .
 *
 * @param[in]	dpci - Pointer to dpci object
 * @param[out]	up - link status
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_link_state(struct dpci *dpci, int *up);

/*!
 * @name DPNI IRQ Index and Events
 */
#define DPCI_IRQ_INDEX				0
/*!< Irq index */
#define DPCI_IRQ_EVENT_LINK_CHANGED		0x00000001
/*!< irq event - Indicates that the link state changed */
/* @} */

/**
 * @brief	Sets IRQ information for the DPCI to trigger an interrupt.
 *
 * @param[in]	dpci		DPCI descriptor object
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[out]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_irq(struct dpci *dpci,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPCI.
 *
 * @param[in]	dpci		DPCI descriptor object
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
int dpci_get_irq(struct dpci *dpci,
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
 * @param[in]	dpci		DPCI descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_irq_enable(struct dpci *dpci,
			uint8_t irq_index,
	uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	dpci		DPCI descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_irq_enable(struct dpci *dpci,
			uint8_t irq_index,
	uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpci		DPCI descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_set_irq_mask(struct dpci *dpci, uint8_t irq_index, uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpci		DPCI descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpci_get_irq_mask(struct dpci *dpci, uint8_t irq_index, uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	dpci		DPCI descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpci_get_irq_status(struct dpci *dpci, uint8_t irq_index, uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	dpci		DPCI descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpci_clear_irq_status(struct dpci *dpci,
			  uint8_t irq_index,
	uint32_t status);

/** @} */

#endif /* __FSL_DPCI_H */
