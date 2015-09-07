/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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
 * @file    fsl_cmdif_bd_flib.h
 * @brief   BD ring FLIB
 *
 *
 */

#ifndef __FSL_CMDIF_BD_FLIB_H
#define __FSL_CMDIF_BD_FLIB_H

/*!
 * @Group	cmdif_bd_flib_g  CMDIF BD ring API
 *
 * @brief	API to be used for accessing CMDIF BD ring.
 *
 * @{
 */

#define CMDIF_BD_DATA_SIZE	64	/*!< In bytes */

struct cmdif_bd {
	uint64_t async_cb;
	uint64_t async_ctx;
	uint64_t cmd_addr;	/*!< Pointer to command data */
	uint64_t resp_addr;	/*!< Pointer to response data */
	uint32_t cmd_size;	/*!< Size of cmd_addr */
	uint32_t resp_size;	/*!< Size of resp_addr */
	uint32_t flags;		/*!< Interrupt, */
	uint16_t session_id;
	uint16_t cmdi_id;
	uint8_t cmd_data[CMDIF_BD_DATA_SIZE];
	/*!< 64 bytes data to be used for the command */
	uint8_t resp_data[CMDIF_BD_DATA_SIZE];
	/*!< 64 bytes data to be used for the response */
};

struct cmdif_bd_ring {
	uint32_t flags;
	uint32_t enq;
	uint32_t deq;
	uint8_t num_bds; /*!< Max number of BDs = 2^num_bds, must be 1 byte */ 
	uint64_t bd_addr;
};

int cmdif_flib_send(struct cmdif_bd_ring *bd_ring, const struct cmdif_bd *bd);
int cmdif_flib_receive(struct cmdif_bd_ring *bd_ring, struct cmdif_bd *bd);
int cmdif_flib_open_bd(uint64_t handle, const char *module_name, struct cmdif_bd *bd);
int cmdif_flib_close_bd(uint64_t handle, struct cmdif_bd *bd);

/** @} */ /* end of cmdif_bd_flib_g group */


#endif /* __FSL_CMDIF_BD_FLIB_H */
