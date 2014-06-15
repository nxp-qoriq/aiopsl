
/* Copyright 2013 Freescale Semiconductor, Inc. */
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
#define CMDIF_EPID         0

struct cmdif_dev {
	uint64_t   p_sync_done;
	/*!< Physical address of sync_done */
	cmdif_cb_t *async_cb;
	/*!<  Asynchronous commands callback */
	void       *async_ctx;
	/*!< Asynchronous commands context */
	void       *sync_done;
	/*!< 4 bytes to be used for synchronous commands */
	uint16_t   auth_id;
	/*!< Authentication ID to be used for session with server*/
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
