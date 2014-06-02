/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_cmdif.h

 @Description   TODO
 *//***************************************************************************/

#ifndef __FSL_CMDIF_H
#define __FSL_CMDIF_H

/*!
 * @Group grp_cmdif	Command Interface API
 *
 * @brief	Contains API for Command Interface
 * @{
 */

/**
 * @brief	Command interface descriptor
 */
struct cmdif_desc {
	void *regs;
	/*!<
	 * Pointer to command interface registers (virtual address);
	 * Must be set by the user
	 */
	void *dev;
	/*!<
	 * Opaque handle for the use of the command interface;
	 * user should not modify it.
	 */
	void *lock;
	/*!<
	 * Optional lock object to be used with the lock/unlock callbacks;
	 * user must zero it if not needed.
	 */
	void (*lock_cb)(void *lock);
	/*!<
	 * Callback for locking the command interface (multi-user scenario);
	 * user must zero it if not needed.
	 */
	void (*unlock_cb)(void *lock);
	/*!<
	 * Callback for unlocking the command interface (multi-user scenario);
	 * user must zero it if not needed.
	 */
};

enum cmdif_module {
	CMDIF_MOD_DPRC,
	CMDIF_MOD_DPNI,
	CMDIF_MOD_DPMAC,
	CMDIF_MOD_DPIO,
	CMDIF_MOD_DPBP,
	CMDIF_MOD_DPSW,
	CMDIF_MOD_DPDMUX,
	CMDIF_MOD_DPCON,
	CMDIF_MOD_DPCI,
	CMDIF_MOD_DPSECI
};

#define CMDIF_PRI_LOW		0 /*!< Low Priority command indication */
#define CMDIF_PRI_HIGH		1 /*!< High Priority command indication */

/*
 * @Group grp_cmdif	Command Interface Client API
 *
 * @brief	Contains API for Command Interface client side
 * @{
 */

int cmdif_open(struct cmdif_desc *cidesc,
	enum cmdif_module module,
	int size,
	uint8_t *cmd_data,
	uint32_t options);

int cmdif_close(struct cmdif_desc *cidesc);

int cmdif_send(struct cmdif_desc *cidesc,
	       uint16_t cmd_id,
	       int size,
	       int priority,
	       uint8_t *cmd_data);

#if 0
/* TODO - think if needed */
int cmdif_get_cmd_data(struct cmdif_desc *cidesc, uint8_t **cmd_data);
#endif

/** @} *//* end of grp_cmdif group */
/** @} *//* end of grp_cmdif */

#endif /* __FSL_CMDIF_H */
