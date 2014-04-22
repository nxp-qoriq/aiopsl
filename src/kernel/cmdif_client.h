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

#endif /* __CMDIF_CLIENT_H */
