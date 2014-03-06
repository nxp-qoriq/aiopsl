/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dpio.h
 *  @brief   Data Path I/O Portal API
 */
#ifndef __FSL_DPIO_H
#define __FSL_DPIO_H

/*!
 * @Group grp_dpbp	Data Path Buffer I/O Portal API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPIO
 * @{
 */

#ifdef MC
struct dpio;
#else
#include <fsl_cmdif.h>
struct dpio {
	struct cmdif_desc cidesc;
};
#endif

/**
 * @brief	DPIO notification channel mode
 */
enum dpio_channel_mode {
	DPIO_NO_CHANNEL = 0, /*!< No notification channel support */
	DPIO_LOCAL_CHANNEL,
/*!< notifications associated with this dpio will be received by the
 * dedicated channel of this dpio */
#if 0
DPIO_REMOTE_CHANNEL,
/*!< notifications associated with this dpio will be forward to a
 * notification channel of a remote dpio */
#endif
};

/**
 * @brief	structure representing DPIO configuration
 */
struct dpio_cfg {
	enum dpio_channel_mode channel_mode;
	/*!< select the channel mode */
	uint8_t num_priorities;
/*!< 1-8; relevant only for 'DPIO_LOCAL_CHANNEL' */
#if 0
int dpio_id;/*!< remote DPIO object id */
#endif
};

/**
 * @brief	structure representing attributes parameters
 */
struct dpio_attr {
	dma_addr_t qbman_portal_ce_paddr;
	/*!< physical address of the sw-portal cache-enabled area */
	dma_addr_t qbman_portal_ci_paddr;
	/*!< physical address of the sw-portal cache-inhibited area */
	uint16_t qbman_portal_id;
	/*!< sw-portal-id */
	uint8_t num_priorities;
/*!< 1-8; relevant only for 'DPIO_LOCAL_CHANNEL' */
/* TODO - irq??? */
};

/**
 * @brief	Open object handle - required before any operation on the object
 *
 * @param[in]	dpio - Pointer to dpio object
 * @param[in]	dpio_id - DPIO unique ID
 *
 * @returns     '0' on Success; Error code otherwise
 *
 * @warning	Required before any operation on the object
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
 * @brief	Will allocate resources and preliminary initialization
 *
 * @param[in]	dpio - Pointer to dpio object
 * @param[in]	cfg - Configuration structure
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpio_init(struct dpio *dpio, const struct dpio_cfg *cfg);

/**
 * @brief	Frees all allocated resources
 *
 * @param[in]	dpio - Pointer to dpio object
 *
 * @returns	'0' on Success; Error code otherwise
 */
int dpio_done(struct dpio *dpio);

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
 * @brief	Sets IRQ information for the dpio object
 *		(required for QBMan to signal events).
 *
 * @param[in]	dpio - Pointer to dpio object
 * @param[in]	irq_index - identifies the specific IRQ to configure
 * @param[in]	irq_paddr - physical IRQ address that must be written to
 *		signal the interrupt
 * @param[in]	irq_val - IRQ value to write into the IRQ address
 *
 * @returns      '0' on Success; Error code otherwise.
 */
int dpio_set_irq(struct dpio *dpio,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val);

int dpio_get_irq(struct dpio *dpio,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val);

int dpio_set_irq_enable(struct dpio *dpio,
                          uint8_t irq_index,
                          uint8_t enable_state);

int dpio_get_irq_enable(struct dpio *dpio,
                          uint8_t irq_index,
                          uint8_t *enable_state);

int dpio_set_irq_mask(struct dpio *dpio,
                        uint8_t irq_index,
                        uint32_t mask);

int dpio_get_irq_mask(struct dpio *dpio,
                        uint8_t irq_index,
                        uint32_t *mask);

int dpio_get_irq_status(struct dpio *dpio,
                         uint8_t irq_index,
                         uint32_t *status);

int dpio_clear_irq_status(struct dpio *dpio,
                            uint8_t irq_index,
                            uint32_t status);


/** @} */

#endif /* __FSL_DPIO_H */
