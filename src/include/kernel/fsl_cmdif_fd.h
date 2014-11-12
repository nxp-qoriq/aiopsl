/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 *  @file    fsl_cmdif_fd.h
 *  @brief   Cmdif AIOP<->GPP FLIB header file describing frame descriptor
 */

#ifndef __FSL_CMDIF_FD_H
#define __FSL_CMDIF_FD_H


#include <types.h>

/*!
 * @Group	cmdif_g  Command Interface API
 *
 * @brief	AIOP and GPP command interface API
 *
 * @{
 */


/*!
 * @Group	cmdif_flib_g  Command Interface - FLIB API
 *
 * @brief	API to be used for FD based command interface implementation
 *
 * This is external API that is used to implement the final API as defined at
 * fsl_cmdif_client.h and fsl_cmdif_server.h. For client and server external use
 * only the API from fsl_cmdif_client.h and fsl_cmdif_server.h.
 *
 * @{
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
		/*!< Full FLC field */
		struct {
			uint8_t dev_h;     /*!< 7 high bits of cmdif_desc.dev */
			uint8_t err;       /*!< Reserved for error on response*/
			uint16_t auth_id;  /*!< Authentication id */
			uint16_t cmid;     /*!< Command id */
			uint16_t epid;     /*!< Reserved fog EPID */
		} cmd;
		/*!< FLC field for command after the session is open */
		struct {
			uint8_t inst_id;    /*!< Module instance id*/
			uint8_t reserved0;
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t epid;      /*!< Reserved fog EPID */
		} open;
		/*!< FLC field for open command */
		struct {
			uint8_t reserved[2];
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t epid;      /*!< Reserved fog EPID */
		} close;
		/*!< FLC field for close command */
		uint32_t word[2];
	} u_flc;

	/*! FD[FRC] Frame descriptor relevant fields as should be set
	 * by cmdif client side when sending commands to AIOP server */
	union  {
		uint32_t frc;
		/*!< Full FRC field */
		struct {
			uint32_t dev_l;   /*!< 32 low bit of cmdif_desc.dev */
		} cmd;
		/*!< FRC field for command after the session is open */
	} u_frc;

	uint32_t d_size; /*!< Data length */

	/*! FD[ADDR] Frame descriptor relevant field as should be set
	 * by cmdif client side when sending commands to AIOP server */
	union {
		uint64_t d_addr; /*!< Data address */
		uint32_t word[2];
	}u_addr;
};

/** @} *//* end of cmdif_flib_g group */
/** @} *//* end of cmdif_g group */

#endif /* __FSL_CMDIF_FD_H */
