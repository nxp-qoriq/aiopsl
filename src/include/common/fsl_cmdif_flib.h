#ifndef __FSL_CMDIF_FLIB_H
#define __FSL_CMDIF_FLIB_H

#include <types.h>
#include <fsl_aiop_cmdif.h>

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
	uint64_t d_addr; /*!< Data address */
};

int cmdif_open_cmd(struct cmdif_desc *cidesc,
                   const char *m_name,
                   uint8_t instance_id,
                   cmdif_cb_t async_cb,
                   void *async_ctx,
                   uint8_t *v_data,
                   uint64_t p_data,
                   uint32_t size,
                   struct cmdif_fd *fd);
int cmdif_sync_ready(struct cmdif_desc *cidesc);
int cmdif_sync_cmd_done(struct cmdif_desc *cidesc);
int cmdif_open_done(struct cmdif_desc *cidesc);
int cmdif_close_cmd(struct cmdif_desc *cidesc, struct cmdif_fd *fd);
int cmdif_close_done(struct cmdif_desc *cidesc);


#endif /* __FSL_CMDIF_FLIB_H */
