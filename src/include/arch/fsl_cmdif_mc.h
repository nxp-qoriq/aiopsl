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

#ifndef __FSL_MC_CMDIF_H
#define __FSL_MC_CMDIF_H

#define MC_CMD_NUM_OF_PARAMS	7

struct mc_cmd_data {
	uint64_t params[MC_CMD_NUM_OF_PARAMS];
};

struct mc_portal {
	uint64_t header;
	struct mc_cmd_data data;
};

enum mc_cmd_status {
	MC_CMD_STATUS_OK = 0x0, /*!< Completed successfully */
	MC_CMD_STATUS_READY = 0x1, /*!< Ready to be processed */
	MC_CMD_STATUS_AUTH_ERR = 0x3, /*!< Authentication error */
	MC_CMD_STATUS_NO_PRIVILEGE = 0x4, /*!< No privilege */
	MC_CMD_STATUS_DMA_ERR = 0x5, /*!< DMA or I/O error */
	MC_CMD_STATUS_CONFIG_ERR = 0x6, /*!< Configuration error */
	MC_CMD_STATUS_TIMEOUT = 0x7, /*!< Operation timed out */
	MC_CMD_STATUS_NO_RESOURCE = 0x8, /*!< No resources */
	MC_CMD_STATUS_NO_MEMORY = 0x9, /*!< No memory available */
	MC_CMD_STATUS_BUSY = 0xA, /*!< Device is busy */
	MC_CMD_STATUS_UNSUPPORTED_OP = 0xB, /*!< Unsupported operation */
	MC_CMD_STATUS_INVALID_STATE = 0xC /*!< Invalid state */
};

#define MC_CMD_HDR_CMDID_O	52	/* Command ID field offset */
#define MC_CMD_HDR_CMDID_S	12	/* Command ID field size */
#define MC_CMD_HDR_AUTHID_O	38	/* Authentication ID field offset */
#define MC_CMD_HDR_AUTHID_S	10	/* Authentication ID field size */
#define MC_CMD_HDR_SIZE_O	31	/* Size field offset */
#define MC_CMD_HDR_SIZE_S	6	/* Size field size */
#define MC_CMD_HDR_STATUS_O	16	/* Status field offset */
#define MC_CMD_HDR_STATUS_S	8	/* Status field size*/
#define MC_CMD_HDR_PRI_O	15	/* Priority field offset */
#define MC_CMD_HDR_PRI_S	1	/* Priority field size */

#define MC_CMD_HDR_READ_STATUS(_hdr) \
	((enum mc_cmd_status)u64_dec((_hdr), \
		MC_CMD_HDR_STATUS_O, MC_CMD_HDR_STATUS_S))

#define MC_CMD_HDR_READ_AUTHID(_hdr) \
	((uint16_t)u64_dec((_hdr), MC_CMD_HDR_AUTHID_O, MC_CMD_HDR_AUTHID_S))

#define MC_CMD_PRI_LOW		0 /*!< Low Priority command indication */
#define MC_CMD_PRI_HIGH		1 /*!< High Priority command indication */

static inline void mc_cmd_write(struct mc_portal *portal,
				uint16_t cmd_id,
	uint16_t auth_id,
	uint8_t size,
	int pri,
	struct mc_cmd_data *cmd_data)
{
	uint64_t hdr;
	int i;

	hdr = u64_enc(MC_CMD_HDR_CMDID_O, MC_CMD_HDR_CMDID_S, cmd_id);
	hdr |= u64_enc(MC_CMD_HDR_AUTHID_O, MC_CMD_HDR_AUTHID_S, auth_id);
	hdr |= u64_enc(MC_CMD_HDR_SIZE_O, MC_CMD_HDR_SIZE_S, size);
	hdr |= u64_enc(MC_CMD_HDR_PRI_O, MC_CMD_HDR_PRI_S, pri);
	hdr |= u64_enc(MC_CMD_HDR_STATUS_O, MC_CMD_HDR_STATUS_S,
			MC_CMD_STATUS_READY);

	if (cmd_data)
		/* copy command parameters into the portal */
		for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
			iowrite64(cmd_data->params[i],
				  &(portal->data.params[i]));
	else
		/* zero all parameters */
		for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
			iowrite64(0, &(portal->data.params[i]));

	/* submit the command by writing the header */
	iowrite64(hdr, &portal->header);
}

static inline enum mc_cmd_status mc_cmd_read_status(struct mc_portal *portal)
{
	uint64_t hdr = ioread64(&(portal->header));

	return MC_CMD_HDR_READ_STATUS(hdr);
}

static inline uint16_t mc_cmd_read_auth_id(struct mc_portal *portal)
{
	uint64_t hdr = ioread64(&(portal->header));

	return MC_CMD_HDR_READ_AUTHID(hdr);
}

static inline void mc_cmd_read_response(struct mc_portal *portal,
					struct mc_cmd_data *resp)
{
	int i;

	if (resp)
		for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
			resp->params[i] = ioread64(&(portal->data.params[i]));
}

#endif /* __FSL_MC_CMDIF_H */
