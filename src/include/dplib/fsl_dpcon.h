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
#include <fsl_cmdif.h>
struct dpcon {
	struct cmdif_desc cidesc; /*!< Descriptor for command portal */
};
#endif

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
	uint16_t qbman_ch_id; /*!< channel id to be used by dequeue operation*/
	uint8_t num_priorities; /*!< 1-8 */
};

/**
 * @brief	structure representing notification parameters
 */
struct dpcon_notification_cfg {
	uint16_t dpio_id; /*!< DPIO object id; must have a notification channel
	 (either local or remote) */
	uint8_t priority; /*!<  0-1 or 0-7 (depends on the channel type) to
	 select the priority(work-queue) within the channel */
	uint64_t user_ctx; /*!< will be provided with each CDAN message */
};

/**
 * @brief	Open object handle - required before any operation on the object
 *
 * @param[in]	dpcon - Pointer to dpcon object
 * @param[in]	dpcon_id - DPCON unique ID
 *
 * @returns	'0' on Success; Error code otherwise
 *
 * @warning	Required before any operation on the object
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
 * @brief	Will allocate resources and preliminary initialization
 *
 * @param[in]	dpcon - Pointer to dpcon object
 * @param[in]	cfg - Configuration structure
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_init(struct dpcon *dpcon, const struct dpcon_cfg *cfg);

/**
 * @brief	Frees all allocated resources
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_done(struct dpcon *dpcon);

/**
 * @brief	Enable the dpcon.
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_enable(struct dpcon *dpcon);

/**
 * @brief	Disable the dpcon, will disallow sending and receiving frames.
 *
 * @param[in]	dpcon - Pointer to dpcon object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpcon_disable(struct dpcon *dpcon);

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
 * @brief	Sets IRQ information for the dpcon object
 *		(required for QBMan to signal events).
 *
 * @param[in]	dpcon - Pointer to dpcon object
 * @param[in]	irq_index - identifies the specific IRQ to configure
 * @param[in]	irq_paddr - physical IRQ address that must be written to
 *		signal the interrupt
 * @param[in]	irq_val - IRQ value to write into the IRQ address
 *
 * @returns      '0' on Success; Error code otherwise.
 */
int dpcon_set_irq(struct dpcon *dpcon,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val);

int dpcon_get_irq(struct dpcon *dpcon,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val);

int dpcon_set_irq_enable(struct dpcon *dpcon,
                          uint8_t irq_index,
                          uint8_t enable_state);

int dpcon_get_irq_enable(struct dpcon *dpcon,
                          uint8_t irq_index,
                          uint8_t *enable_state);

int dpcon_set_irq_mask(struct dpcon *dpcon,
                        uint8_t irq_index,
                        uint32_t mask);

int dpcon_get_irq_mask(struct dpcon *dpcon,
                        uint8_t irq_index,
                        uint32_t *mask);

int dpcon_get_irq_status(struct dpcon *dpcon,
                         uint8_t irq_index,
                         uint32_t *status);

int dpcon_clear_irq_status(struct dpcon *dpcon,
                            uint8_t irq_index,
                            uint32_t status);


/** @} */

#endif /* __FSL_DPCON_H */
