
/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    cmdif_client.h
 *  @brief   Cmdif client AIOP<->GPP internal header file
 */

#ifndef __CMDIF_CLIENT_AIOP_H
#define __CMDIF_CLIENT_AIOP_H

#include "cmdif_client.h"

#define CMDIF_MN_SESSIONS	50 /**< Maximal number of sessions */
#define CMDIF_NUM_PR  		2

struct cmdif_sdev {
	int id;
	/**< DPCI id as known by GPP side */
	uint8_t num_of_pr;
	/**< number of priorities */
	uint16_t fqid[CMDIF_NUM_PR];
	/**< Will be accessed by CMDIF_PRI_LOW/HIGH*/
};

/* To be allocated on DDR */
struct cmdif_cl {
	struct {
		uint64_t p_addr;
		/**< Physical address of buffer allocated by GPP */
		char m_name[M_NAME_CHARS + 1];
		/**< Module application name */
		struct cmdif_sdev *sdev;
		/**< Send device, to be placed as cidesc.reg */
		void *dpci_dev;
		/**< Open AIOP dpci device */
		uint16_t auth_id;
		/**< Authentication ID to be used for session with server*/
		uint8_t ins_id;
		/**< Instanse id that was used for open */
	} gpp[CMDIF_MN_SESSIONS]; 
};


#endif /* __CMDIF_CLIENT_H */
