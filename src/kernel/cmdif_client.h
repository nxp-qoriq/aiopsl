#ifndef __CMDIF_CLIENT_H
#define __CMDIF_CLIENT_H

/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    cmdif_client.h
 *  @brief   Cmdif client AIOP<->GPP internal header file
 */

/**
 * @brief    Structure to be used to access struct cmdif_desc.dev
 */

/* Common settings for Server and Client */
#define CMD_ID_OPEN        0x8000
#define CMD_ID_CLOSE       0x4000
#define OPEN_AUTH_ID       0xFFFF 
#define M_NAME_CHARS       8     /*!< Not including \0 */

struct cmdif_dev {
	cmdif_cb_t *async_cb;   
	/*!<  Asynchronous commands callback */
	void       *async_ctx;  
	/*!< Asynchronous commands context */
	void       *sync_done;
	/*!< 4 bytes to be used for synchronous commands*/
	uint16_t   auth_id;     
	/*!< Authentication ID to be used for session with server*/	
};

/*! FD[FLC] Frame descriptor relevant fields as should be set 
 * by cmdif client side when sending commands to AIOP server */
union cmdif_flc {
	uint64_t flc;
	struct {
		uint8_t dev_h;     /*!< 7 high bits of cmdif_desc.dev */
		uint8_t err;       /*!< Reserved for error on response */
		uint16_t auth_id;  /*!< Authentication id */
		uint16_t cmid;     /*!< Command id */
		uint16_t reserved; /*!< Reserved fog EPID */
	} cmd;
	struct {
		uint8_t inst_id;    /*!< Module instance id*/
		uint8_t reserved0;
		uint16_t auth_id;   /*!< Authentication id */
		uint16_t cmid;      /*!< Command id */
		uint16_t reserved1; /*!< Reserved fog EPID */
	} open;
	/*!< Open command is always synchronous */
	struct {
		uint8_t reserved[2];
		uint16_t auth_id;   /*!< Authentication id */
		uint16_t cmid;      /*!< Command id */
		uint16_t reserved1; /*!< Reserved fog EPID */
	} close;
	/*!< Close command is always synchronous*/
};

/*! FD[FRC] Frame descriptor relevant fields as should be set 
 * by cmdif client side when sending commands to AIOP server */
union cmdif_frc {
	uint32_t frc;	
	struct {
		uint32_t dev_l;   /*!< 32 low bit of cmdif_desc.dev */
	} cmd;
};

/*! FD[ADDR] content of the buffer to be sent with open command 
 * when sending to AIOP server*/
union cmdif_data{
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
