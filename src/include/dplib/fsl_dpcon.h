/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dpcon.h
 *  @brief   Data Path Concentrator API
 */
#ifndef __FSL_DPCON_H
#define __FSL_DPCON_H

/*!
 * @Group grp_dpcon	Data Path Concentrator API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPCON
 * @{
 */

#ifdef MC
struct dpcon;
#else
struct dpcon {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Must be set by the user
	 */
	int auth; /*!< authentication ID */
};
#endif

/*!
 * @name General DPCON macros
 */
#define DPCON_ID_NOT_VALID			(-1)
/*!< Invalid id value  */
/* @} */

/**
 * @brief	structure representing DPCON configuration
 */
struct dpcon_cfg {
	uint8_t num_priorities; /*!< 1-8 */
};

/**
 * @brief	structure representing attributes parameters
 */
struct dpcon_attr {
	int id;
	/*!< DPCON id */
	uint16_t qbman_ch_id;
	/*!<
	 * Channel id to be used by dequeue operation; if equal to
	 * 'DPCON_ID_NOT_VALID', user should call this function after
	 * enabling the object.
	 */
	uint8_t num_priorities;
	/*!< Set to 1-8 */
	struct {
		uint32_t major; /*!< DPCON major version*/
		uint32_t minor; /*!< DPCON minor version*/
	} version;
	/*!< DPCON version */
};

/**
 * @brief	structure representing notification parameters
 */
struct dpcon_notification_cfg {
	uint16_t dpio_id;
	/*!<
	 * DPIO object id; must have a notification channel (either local
	 * or remote)
	 */
	uint8_t priority;
	/*!<
	 * Set to 0-1 or 0-7 (depends on the channel type) to select the
	 * priority(work-queue) within the channel of the DPIO
	 */
	uint64_t user_ctx;
	/*!< will be provided with each CDAN message */
};

/**
 * @brief	Open object handle, allocate resources and preliminary
 *		initialization - required before any operation on the object
 *
 * @param[in]	dpcon - Pointer to dpcon object
 * @param[in]	cfg - Configuration structure
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpcon_create(struct dpcon *dpcon, const struct dpcon_cfg *cfg);

/**
 * @brief	Open object handle
 *
 * @param[in]	dpcon - Pointer to dpcon object
 * @param[in]	dpcon_id - DPCON unique ID
 *
 * @returns	'0' on Success; Error code otherwise
 *
 */
int dpcon_open(struct dpcon *dpcon, int dpcon_id);

/**
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_close(struct dpcon *dpcon);

/**
 * @brief	Frees all allocated resources
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_destroy(struct dpcon *dpcon);

/**
 * @brief	Enable the dpcon.
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_enable(struct dpcon *dpcon);

/**
 * @brief	Disable the dpcon.
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_disable(struct dpcon *dpcon);

/**
 * @brief	Reset the dpcon, will return to initial state.
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_reset(struct dpcon *dpcon);

/**
 * @brief	Retrieve the object's attributes
 *
 * @param[in]	dpcon - Pointer to dpcon object
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise
 *
 * @warning	Allowed only following dpcon_enable().
 */
int dpcon_get_attributes(struct dpcon *dpcon, struct dpcon_attr *attr);

/**
 * @brief	Set the DPCON notification destination
 *
 * @param[in]	dpcon - Pointer to dpcon object
 * @param[in]	cfg - notification structure
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_set_notification(struct dpcon *dpcon,
			   struct dpcon_notification_cfg *cfg);

/**
 * @brief	Sets IRQ information for the DPCON to trigger an interrupt.
 *
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_set_irq(struct dpcon *dpcon,
		  uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPCON.
 *
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_paddr and irq_val are valid);
 * @param[out]	irq_paddr	Physical address that must be written in order
 *				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 *				message-based interrupt
 * @param[out]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_get_irq(struct dpcon *dpcon,
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
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_set_irq_enable(struct dpcon *dpcon,
			 uint8_t irq_index,
	uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_get_irq_enable(struct dpcon *dpcon,
			 uint8_t irq_index,
	uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_set_irq_mask(struct dpcon *dpcon, uint8_t irq_index, uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpcon_get_irq_mask(struct dpcon *dpcon, uint8_t irq_index, uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpcon_get_irq_status(struct dpcon *dpcon,
			 uint8_t irq_index,
	uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	dpcon		DPCON descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpcon_clear_irq_status(struct dpcon *dpcon,
			   uint8_t irq_index,
	uint32_t status);

/** @} */

#endif /* __FSL_DPCON_H */
