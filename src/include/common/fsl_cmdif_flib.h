#ifndef __FSL_CMDIF_FLIB_H
#define __FSL_CMDIF_FLIB_H

#include <types.h>
#include <fsl_cmdif_client.h>

/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_cmdif_flib.h
 *  @brief   Cmdif AIOP<->GPP FLIB header file
 */

/*!
 * The following are the FD fields that are used by CMDIF
 * cmdif_fd.fls, cmdif_fd.frc, cmdif_fd.d_addr, cmdif_fd.d_size
 * should be copied into real FD */
struct cmdif_fd {
	/*! FD[FLC] Frame descriptor relevant fields as should be set
	 * by cmdif client side when sending commands to AIOP server */
	union {
		uint64_t flc;
		struct {
			uint8_t dev_h;     /*!< 7 high bits of cmdif_desc.dev */
			uint8_t err;       /*!< Reserved for error on response*/
			uint16_t auth_id;  /*!< Authentication id */
			uint16_t cmid;     /*!< Command id */
			uint16_t epid;     /*!< Reserved fog EPID */
		} cmd;
		struct {
			uint8_t inst_id;    /*!< Module instance id*/
			uint8_t reserved0;
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t epid;      /*!< Reserved fog EPID */
		} open;
		/*!< Open command is always synchronous */
		struct {
			uint8_t reserved[2];
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t epid;      /*!< Reserved fog EPID */
		} close;
		/*!< Close command is always synchronous*/
		uint32_t word[2];
	} u_flc;

	/*! FD[FRC] Frame descriptor relevant fields as should be set
	 * by cmdif client side when sending commands to AIOP server */
	union  {
		uint32_t frc;
		struct {
			uint32_t dev_l;   /*!< 32 low bit of cmdif_desc.dev */
		} cmd;
	} u_frc;

	uint32_t d_size; /*!< Data length */
	union {
		uint64_t d_addr; /*!< Data address */
		uint32_t word[2];
	}u_addr;
};

/**
 *
 * @brief	Build open command.
 * 
 * Should be used for building frame descriptor for open command.
 *   
 * @param[in]	cidesc      - Command interface descriptor
 * @param[in]	m_name      - Module name, up to 8 charachters
 * @param[in]	instance_id - Instance id which will be passed to open_cb_t
 * @param[in]	async_cb    - Callback to be called on response of
 *		asynchronous command.
 * @param[in]	async_ctx   - Context to be received with asynchronous
 * 		command response inside async_cb().		
 * @param[in]	v_data	    - Virtual address of the buffer to be used
 * 		by command interface.
 * 		This address should be accessible by Server and Client.
 * @param[in]	p_data	    - Physical address of the v_data buffer.
 * @param[in]	size        - Size of the v_data buffer. If the size if not
		enough cmdif_open() will return -ENOMEM.
 * @param[out]	fd          - Frame descriptor relevant fields for cmdif
 * 
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int cmdif_open_cmd(struct cmdif_desc *cidesc,
                   const char *m_name,
                   uint8_t instance_id,
                   cmdif_cb_t async_cb,
                   void *async_ctx,
                   uint8_t *v_data,
                   uint64_t p_data,
                   uint32_t size,
                   struct cmdif_fd *fd);
/**
 *
 * @brief	Synchronious/Blocking mode done indication.
 * 
 * Should be used for implementation of cmdif_send() in synchronious mode.  
 *
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	'0' if the command is not finished yet; 
 *               not '0' if it has finished.
 */
int cmdif_sync_ready(struct cmdif_desc *cidesc);

/**
 *
 * @brief	Synchronious command done.
 * 
 * Should be used for implementation of cmdif_send() in synchronious mode.
 * Should the last call before return inside from cmdif_send().  
 *
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	Error code as returned by AIOP server 
 */
int cmdif_sync_cmd_done(struct cmdif_desc *cidesc);

/**
 *
 * @brief	Open command done.
 * 
 * Should be used for implementation of cmdif_open().
 * Should the last call before return inside from cmdif_open().  
 *
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	Error code as returned by AIOP server 
 */
int cmdif_open_done(struct cmdif_desc *cidesc);


/**
 *
 * @brief	Build close command.
 * 
 * Should be used for building frame descriptor for close command.
 *   
 * @param[in]	cidesc      - Command interface descriptor
 * @param[out]	fd          - Frame descriptor relevant fields for cmdif
 * 
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int cmdif_close_cmd(struct cmdif_desc *cidesc, struct cmdif_fd *fd);

/**
 *
 * @brief	Close command done.
 * 
 * Should be used for implementation of cmdif_close().  
 * Should the last call before return inside from cmdif_close().
 *   
 * @param[in]	cidesc      - Command interface descriptor
 *
 * @returns	Error code as returned by AIOP server 
 */
int cmdif_close_done(struct cmdif_desc *cidesc);

/**
 *
 * @brief	Synchronious/Blocking mode indication.
 * 
 * Should be used for implementation of cmdif_send() in synchronious mode.  
 *
 * @param[in]	cmd_id - Command id that was sent
 *
 * @returns	'0' if command is asynchronious; 
 *               not '0' id command is synchronious.
 *
 */
int cmdif_is_sync_cmd(uint16_t cmd_id);

/**
 *
 * @brief	Build command.
 * 
 * Should be used for building frame descriptor for command.
 *   
 * @param[in]	cidesc - Command interface descriptor
 * @param[in]	cmd_id - Command id that was sent
 * @param[in]	size   - Size of data
 * @param[in]	data   - Physical address to data
 * @param[out]	fd     - Frame descriptor relevant fields for cmdif
 * 
 * @returns	'0' if command is asynchronious; 
 *               not '0' id command is synchronious.
 *
 */
int cmdif_cmd(struct cmdif_desc *cidesc,
              uint16_t cmd_id,
              uint32_t size,
              uint64_t data,
              struct cmdif_fd *fd);

/**
 *
 * @brief	Call asynchronious callback of the received frame descriptor
 *
 * @param[in]	fd - Pointer to received frame descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 */
int cmdif_async_cb(struct cmdif_fd *fd);

#endif /* __FSL_CMDIF_FLIB_H */
