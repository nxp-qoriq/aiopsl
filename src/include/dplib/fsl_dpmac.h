/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dpmac.h
 *  @brief   Data Path MAC Interface API
 */
#ifndef __FSL_DPMAC_H
#define __FSL_DPMAC_H

#ifdef MC
struct dpmac;
#else
struct dpmac {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Must be set by the user
	 */
	int auth; /*!< authentication ID */
};
#endif


/**
 * @brief	Structure representing DPMAC adjust link parameters
 */
struct dpmac_adjust_link_cfg {
	uint64_t rate;
	/*!< Rate */
	int full_duplex;
	/*!< Duplex mode */
};


/**
 * @brief	Structure representing DPMAC MDIO read parameters
 */
struct dpmac_mdio_read_cfg {
	uint8_t	phy_addr;
	/*!< MDIO device address */
	uint8_t		reg;
	/*!< Address of the register within the Clause 45 PHY device from which
	 * data is to be read. */
	uint16_t	*data;
	/*!< Data read from MDIO */
};


/**
 * @brief	Structure representing DPMAC MDIO write parameters
 */
struct dpmac_mdio_write_cfg {
	uint8_t	phy_addr;
	/*!< MDIO device address */
	uint8_t		reg;
	/*!< Address of the register within the Clause 45 PHY device to which
	 * data is to be written. */
	uint16_t	data;
	/*!< Data read from MDIO */
};


/**
 * @brief	Perform MDIO read transaction
 *
 * @param[in]	dpmac	DPMAC object handle
 * @param[in]	cfg	Structure with MDIO transaction parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmac_mdio_read(struct dpmac *dpmac, struct dpmac_mdio_read_cfg *cfg);


/**
 * @brief	Perform MDIO write transaction
 *
 * @param[in]	dpmac	DPMAC object handle
 * @param[in]	cfg	Structure with MDIO transaction parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmac_mdio_write(struct dpmac *dpmac, struct dpmac_mdio_write_cfg *cfg);


/**
 * @brief	Adjusts the Ethernet link with new speed/duplex setup.
 *
 * @param[in]	dpmac		DPMAC object handle
 * @param[in]	cfg		Structure with speed/duplex mode parameters
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dpmac_adjust_link(struct dpmac *dpmac, struct dpmac_adjust_link_cfg *cfg);

#endif /* __FSL_DPMAC_H */
