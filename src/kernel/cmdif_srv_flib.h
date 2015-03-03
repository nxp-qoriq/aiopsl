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

#ifndef __CMDIF_SRV_FLIB_H
#define __CMDIF_SRV_FLIB_H

#include <fsl_cmdif_server.h>

#define CMD_ID_MASK	   0x00000000FFFF0000 /**< FLC */
#define CMD_ID_OFF	   16

#define AUTH_ID_MASK	   0x0000FFFF00000000 /**< FLC[hash] */
#define AUTH_ID_OFF	   32
#define ERROR_MASK	   0x00FF000000000000 /**< FLC[hash] */
#define ERROR_OFF	   48
#define DEV_H_MASK	   0xFF00000000000000 /**< FLC[hash] */
#define DEV_H_OFF	   56
#define INST_ID_MASK	   DEV_H_MASK         /**< FLC[hash] */
#define INST_ID_OFF	   DEV_H_OFF

#define CMD_ID_OPEN           0x8000
#define CMD_ID_CLOSE          0x4000
#define M_NUM_OF_INSTANCES    512   /**< Must be power of 2 */
#define M_NUM_OF_MODULES      64
#define M_NAME_CHARS          8     /**< Not including \0 */
#define SYNC_BUFF_RESERVED    1     /**< 1 Byte must be reserved for done bit */

#define OPEN_AUTH_ID          0xFFFF
/**< auth_id that will be sent as hash value for open commands */
#define CMDIF_SESSION_OPEN_SIZEOF (sizeof(struct cmdif_session_data))

struct cmdif_srv {
	char         (*m_name)[M_NAME_CHARS + 1];
	/**< pointer to arrays of module name per module, DDR */
	open_cb_t    **open_cb;
	/**< open(init) callbacks, one per module, DDR */
	close_cb_t   **close_cb;
	/**< close(de-init) callbacks, one per module, DDR*/
	ctrl_cb_t    **ctrl_cb;
	/**< execution callbacks one per module, SHRAM */
	void         **inst_dev;
	/**< array of instances handels(converted from the authentication ID)
	 * in the size of M_NUM_OF_INSTANCES, SHRAM */
	uint64_t     *sync_done;
	/**< array of physical addresses per instance for setting done
	 * for synchronious commands, SHRAM */
	uint8_t      *m_id;
	/**< converts auth_id to module for cb, SHRAM */
	uint16_t     inst_count;
	/**< counter for instance handlers */
};

struct cmdif_session_data {
	/** Must remain in this order because of client side */
	uint8_t  done;      /*!< Reserved for done on response */
	int8_t   err;       /*!< Reserved for done on response */
	uint16_t auth_id;
	uint32_t dev_id;    /*!< CI device id, DPCI id */
	uint8_t  inst_id;
	char     m_name[M_NAME_CHARS + 1];
};

#endif /* __CMDIF_SRV_H */
