/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dpio.h
 *  @brief   Data Path I/O Portal API
 */
#ifndef __FSL_DPIO_H
#define __FSL_DPIO_H

/*!
 * @Group grp_dpio	Data Path I/O Portal API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPIO
 * @{
 */

#ifdef MC
struct dpio;
#else
struct dpio {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Must be set by the user
	 */
	int auth; /*!< authentication ID */
};
#endif

/**
 * @brief	DPIO notification channel mode
 */
enum dpio_channel_mode {
	DPIO_NO_CHANNEL = 0,
	/*!< No notification channel support */
	DPIO_LOCAL_CHANNEL,
	/*!<
	 * Notifications associated with this dpio will be received by the
	 * dedicated channel of this dpio
	 */
};

/**
 * @brief	structure representing DPIO configuration
 */
struct dpio_cfg {
	enum dpio_channel_mode channel_mode;
	/*!< select the channel mode */
	uint8_t num_priorities;
/*!< 1-8; relevant only for 'DPIO_LOCAL_CHANNEL' */
};

/**
 * @brief	structure representing attributes parameters
 */
struct dpio_attr {
	int id;
	/*!< DPIO id */
	uint64_t qbman_portal_ce_paddr;
	/*!< physical address of the sw-portal cache-enabled area */
	uint64_t qbman_portal_ci_paddr;
	/*!< physical address of the sw-portal cache-inhibited area */
	uint16_t qbman_portal_id;
	/*!< sw-portal-id */
	enum dpio_channel_mode channel_mode; /*!< channel mode */
	uint8_t num_priorities;
	/*!< 1-8; relevant only for 'DPIO_LOCAL_CHANNEL' */
	struct {
		uint32_t major; /*!< DPIO major version*/
		uint32_t minor; /*!< DPIO minor version*/
	} version;
	/*!< DPIO version */
};

/**
 * @brief	Open object handle, allocate resources and preliminary
 *		initialization - required before any operation on the object
 *
 * @param[in]	dpio - Pointer to dpio object
 * @param[in]	cfg - Configuration structure
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpio_create(struct dpio *dpio, const struct dpio_cfg *cfg);

/**
 * @brief	Open object handle
 *
 * @param[in]	dpio - Pointer to dpio object
 * @param[in]	dpio_id - DPIO unique ID
 *
 * @returns     '0' on Success; Error code otherwise
 *
 */
int dpio_open(struct dpio *dpio, int dpio_id);

/**
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed
 *
 * @param[in]	dpio - Pointer to dpio object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpio_close(struct dpio *dpio);

/**
 * @brief	Frees all allocated resources
 *
 * @param[in]	dpio - Pointer to dpio object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpio_destroy(struct dpio *dpio);

/**
 * @brief	Enable the IO, will allow sending and receiving frames.
 *
 * @param[in]	dpio - Pointer to dpio object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpio_enable(struct dpio *dpio);

/**
 * @brief	Disable the IO, will disallow sending and receiving frames.
 *
 * @param[in]	dpio - Pointer to dpio object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpio_disable(struct dpio *dpio);

/**
 * @brief	Reset the IO, will return to initial state.
 *
 * @param[in]	dpio - Pointer to dpio object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpio_reset(struct dpio *dpio);

/**
 * @brief	Retrieve the object's attributes
 *
 * @param[in]	dpio - Pointer to dpio object
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise
 *
 * @warning	Allowed only following dpio_enable().
 */
int dpio_get_attributes(struct dpio *dpio, struct dpio_attr *attr);

/**
 * @brief	Sets IRQ information for the DPIO to trigger an interrupt.
 *
 * @param[in]	dpio		DPIO descriptor object
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[in]	user_irq_id	A user defined number associated with this IRQ;
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpio_set_irq(struct dpio *dpio,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id);

/**
 * @brief	Gets IRQ information from the DPIO.
 *
 * @param[in]	dpio		DPIO descriptor object
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
int dpio_get_irq(struct dpio *dpio,
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
 * @param[in]	dpio		DPIO descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpio_set_irq_enable(struct dpio *dpio,
			uint8_t irq_index,
	uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	dpio		DPIO descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpio_get_irq_enable(struct dpio *dpio,
			uint8_t irq_index,
	uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpio		DPIO descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpio_set_irq_mask(struct dpio *dpio, uint8_t irq_index, uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpio		DPIO descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpio_get_irq_mask(struct dpio *dpio, uint8_t irq_index, uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	dpio		DPIO descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpio_get_irq_status(struct dpio *dpio, uint8_t irq_index, uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	dpio		DPIO descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpio_clear_irq_status(struct dpio *dpio,
			  uint8_t irq_index,
	uint32_t status);

/** @} */

#endif /* __FSL_DPIO_H */
