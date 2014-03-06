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
	struct cmdif_desc cidesc;
};
#endif

/**
 * @brief	structure representing DPBP configuration
 */
struct dpbp_cfg {
	uint16_t buffer_size; /*!< Buffer size of the pool */
/* TODO - need to add depletion parameters
 * (either here or in separate routine) */
};

/**
 * @brief	structure representing attr parameter
 */
struct dpbp_attr {
	uint16_t buffer_size; /*!< Buffer size of the pool */
	uint16_t bpid; /*!< buffer pool id*/
};

/**
 * @brief	Open object handle - required before any operation on the object
 *
 * @param[in]	dpbp - Pointer to dpio object
 * @param[in]	dpbp_id - DPBP unique ID
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Required before any operation on the object
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
 * @brief	Will allocate resources and preliminary initialization
 *
 * @param[in]	dpbp - DPBP handle
 * @param[in]	cfg - Configuration structure
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_init(struct dpbp *dpbp, const struct dpbp_cfg *cfg);

/**
 * @brief	Frees all allocated resources
 *
 * @param[in]	dpbp - DPBP handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_done(struct dpbp *dpbp);

/**
 * @brief	Enables the IO, will allow sending and receiving frames.
 *
 * @param[in]	dpbp - DPBP handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_enable(struct dpbp *dpbp);

/**
 * @brief	Disables the IO, will disallow sending and receiving frames.
 *
 * @param[in]	dpbp - DPBP handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_disable(struct dpbp *dpbp);

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
 * @brief	Sets IRQ information for the dpbp object
 *		(required for QBMan to signal events).
 *
 * @param[in]	dpbp - DPBP handle
 * @param[in]	irq_index - identifies the specific IRQ to configure
 * @param[in]	irq_paddr - physical IRQ address that must be written to
 *		signal the interrupt
 * @param[in]	irq_val - IRQ value to write into the IRQ address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpbp_set_irq(struct dpbp *dpbp,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val);

int dpbp_get_irq(struct dpbp *dpbp,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val);

int dpbp_set_irq_enable(struct dpbp *dpbp,
                          uint8_t irq_index,
                          uint8_t enable_state);

int dpbp_get_irq_enable(struct dpbp *dpbp,
                          uint8_t irq_index,
                          uint8_t *enable_state);

int dpbp_set_irq_mask(struct dpbp *dpbp,
                        uint8_t irq_index,
                        uint32_t mask);

int dpbp_get_irq_mask(struct dpbp *dpbp,
                        uint8_t irq_index,
                        uint32_t *mask);

int dpbp_get_irq_status(struct dpbp *dpbp,
                         uint8_t irq_index,
                         uint32_t *status);

int dpbp_clear_irq_status(struct dpbp *dpbp,
                            uint8_t irq_index,
                            uint32_t status);

/** @} */

#endif /* __FSL_DPBP_H */
