/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dpbp.h
 *  @brief   Data Path Buffer Pool API
 */
#ifndef __FSL_DPBP_H
#define __FSL_DPBP_H

/*!
 * @Group grp_dpbp	Data Path Buffer Pool API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPBP
 * @{
 */

#ifdef MC
struct dpbp;
#else
#include <fsl_cmdif.h>
struct dpbp {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Must be set by the user
	 */
	int auth; /*!< authentication ID */
};
#endif

/*!
 * @name General DPBP macros
 */
#define DPBP_BPID_NOT_VALID			(-1)
/*!< Invalid BPID  */
/* @} */

/**
 * @brief	structure representing DPBP configuration
 */
struct dpbp_cfg {
	int tmp; /* place holder!!!! */
/* TODO - need to add depletion parameters
 * (either here or in separate routine) */
};

/**
 * @brief	structure representing attr parameter
 */
struct dpbp_attr {
	int id; /*!< DPBP id*/
	uint16_t bpid; /*!< buffer pool id;
	in case equal to DPBP_BPID_NOT_VALID the bpid isn't valid
	and must not be used; Only after 'enable' bpid will be valid; */
	struct {
		uint32_t major; /*!< DPBP major version*/
		uint32_t minor; /*!< DPBP minor version*/
	} version; /*!< DPBP version */
};

/**
 * @brief	Open object handle, allocate resources and preliminary initialization -
 *		required before any operation on the object
 *
 * @param[in]	dpbp - Pointer to dpbp object
 * @param[in]	cfg - Configuration structure
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
 */
int dpbp_create(struct dpbp *dpbp, const struct dpbp_cfg *cfg);

/**
 * @brief	Open object handle
 *
 * @param[in]	dpbp - Pointer to dpbp object
 * @param[in]	dpbp_id - DPBP unique ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int dpbp_open(struct dpbp *dpbp, int dpbp_id);

/**
 * @brief	Closes the object handle, no further operations on the object
 *		are allowed
 *
 * @param[in]	dpbp - DPBP handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_close(struct dpbp *dpbp);

/**
 * @brief	Frees all allocated resources
 *
 * @param[in]	dpbp - DPBP handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_destroy(struct dpbp *dpbp);

/**
 * @brief	Enable the dpbp
 *
 * @param[in]	dpbp - DPBP handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_enable(struct dpbp *dpbp);

/**
 * @brief	Disable the dpbp
 *
 * @param[in]	dpbp - DPBP handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_disable(struct dpbp *dpbp);

/**
 * @brief	Reset the dpbp, will return to initial state.
 *
 * @param[in]	dpbp - Pointer to dpio object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_reset(struct dpbp *dpbp);

/**
 * @brief	Retrieves the object's attributes.
 *
 * @param[in]	dpbp - DPBP handle
 * @param[out]	attr - Object's attributes
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Allowed only following dpbp_enable().
 */
int dpbp_get_attributes(struct dpbp *dpbp, struct dpbp_attr *attr);

/**
 * @brief	Sets IRQ information for the DPBP to trigger an interrupt.
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]	irq_index	Identifies the interrupt index to configure.
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 *				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 * @param[in]	irq_virt_id		irq virtual id - used in case that 
 * 				the user that set the irq is not the one who get the interrupt.
 * 				To support virtualized systems. 
 * 				
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_set_irq(struct dpbp *dpbp,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int irq_virt_id);

/**
 * @brief	Gets IRQ information from the DPBP.
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]  type		Interrupt type: 0 represents message interrupt
 *				type (both irq_paddr and irq_val are valid);
 * @param[out]	irq_paddr	Physical address that must be written in order
 *				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 *				message-based interrupt
 * @param[out]	irq_virt_id		irq virtual id - used in case that 
 * 				the user that set the irq is not the one who get the interrupt.
 * 				To support virtualized systems. 
 * 				
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_get_irq(struct dpbp *dpbp,
	uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *irq_virt_id);

/**
 * @brief	Sets overall interrupt state.
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt.
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_set_irq_enable(struct dpbp *dpbp,
	uint8_t irq_index,
	uint8_t enable_state);

/**
 * @brief	Gets overall interrupt state
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	enable_state	interrupt state - enable = 1, disable = 0.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_get_irq_enable(struct dpbp *dpbp,
	uint8_t irq_index,
	uint8_t *enable_state);

/**
 * @brief	Sets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[in]	mask		event mask to trigger interrupt.
 *				each bit:
 *					0 = ignore event
 *					1 = consider event for asserting irq
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_set_irq_mask(struct dpbp *dpbp,
	uint8_t irq_index,
	uint32_t mask);

/**
 * @brief	Gets interrupt mask.
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	mask		event mask to trigger interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_get_irq_mask(struct dpbp *dpbp,
	uint8_t irq_index,
	uint32_t *mask);

/**
 * @brief	Gets the current status of any pending interrupts.
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		interrupts status - one bit per cause
 *					0 = no interrupt pending
 *					1 = interrupt pending
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpbp_get_irq_status(struct dpbp *dpbp,
	uint8_t irq_index,
	uint32_t *status);

/**
 * @brief	Clears a pending interrupt's status
 *
 * @param[in]	dpbp		DPBP descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * @param[out]	status		bits to clear (W1C) - one bit per cause
 *					0 = don't change
 *					1 = clear status bit
 *
 * @returns	'0' on Success; Error code otherwise.
 * */
int dpbp_clear_irq_status(struct dpbp *dpbp,
	uint8_t irq_index,
	uint32_t status);

/** @} */

#endif /* __FSL_DPBP_H */
