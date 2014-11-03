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
 *  @file    cmdif_client.h
 *  @brief   Cmdif client AIOP<->GPP internal header file
 */

#ifndef __CMDIF_CLIENT_H
#define __CMDIF_CLIENT_H

#include <fsl_cmdif_client.h>

/* Common settings for Server and Client */
#define CMD_ID_OPEN        0x8000
#define CMD_ID_CLOSE       0x4000
#define OPEN_AUTH_ID       0xFFFF
#define M_NAME_CHARS       8     /*!< Not including \0 */
#define CMDIF_OPEN_SIZEOF (sizeof(struct cmdif_dev) + sizeof(union cmdif_data))

#define CMDIF_DEV_SET(FD, PTR) \
	do { \
		(FD)->u_flc.cmd.dev_h = \
		(uint8_t)((((uint64_t)(PTR)) & 0xFF00000000) >> 32); \
		(FD)->u_frc.cmd.dev_l = ((uint32_t)(PTR)); \
	} while(0)

#define CMDIF_DEV_GET(FD) \
	((struct cmdif_dev *)((uint64_t)(((uint64_t)((FD)->u_frc.cmd.dev_l)) \
		| (((uint64_t)((FD)->u_flc.cmd.dev_h)) << 32))))

#define CMDIF_ASYNC_ADDR_GET(DATA, SIZE) \
		((uint64_t)(DATA) + (SIZE))

#define CMDIF_DEV_RESERVED_BYTES 12

#define CMDIF_CMD_FD_SET(FD, DEV, DATA, SIZE, CMD) \
	do { \
		(FD)->u_addr.d_addr     = DATA; \
		(FD)->d_size            = (SIZE); \
		(FD)->u_flc.flc         = 0; \
		(FD)->u_flc.cmd.auth_id = (DEV)->auth_id; \
		(FD)->u_flc.cmd.cmid    = CPU_TO_SRV16(CMD); \
		(FD)->u_flc.cmd.epid    = CPU_TO_BE16(CMDIF_EPID); \
		CMDIF_DEV_SET((FD), (DEV)); \
		(FD)->u_flc.flc = CPU_TO_BE64((FD)->u_flc.flc); \
	} while (0)

struct cmdif_async {
	uint64_t async_cb; /*!< Pointer to asynchronous callback */
	uint64_t async_ctx;/*!< Pointer to asynchronous context */
};

struct cmdif_dev {
	uint64_t   p_sync_done;
	/*!< Physical address of sync_done */
	void       *sync_done;
	/*!< 4 bytes to be used for synchronous commands */
	uint16_t   auth_id;
	/*!< Authentication ID to be used for session with server*/
	uint8_t    reserved[CMDIF_DEV_RESERVED_BYTES];
};

/*! FD[ADDR] content of the buffer to be sent with open command
 * when sending to AIOP server*/
union cmdif_data {
	struct {
		uint8_t done;        /*!< Reserved for done on response */
		char m_name[M_NAME_CHARS]; /*!< Module name that was registered */
	}send;
	struct {
		uint8_t  done;      /*!< Reserved for done on response */
		int8_t   err;       /*!< Reserved for done on response */
		uint16_t auth_id;   /*!< New authentication id */
	}resp;
};

#endif /* __CMDIF_CLIENT_H */
