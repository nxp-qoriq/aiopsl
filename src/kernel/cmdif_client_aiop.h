
/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    cmdif_client.h
 *  @brief   Cmdif client AIOP<->GPP internal header file
 */

#ifndef __CMDIF_CLIENT_AIOP_H
#define __CMDIF_CLIENT_AIOP_H

#include "cmdif_client.h"


/* To be allocated on DDR */
struct cmdif_cl {
	struct {
		char *m_name[M_NAME_CHARS + 1];
		/**< pointer to arrays of module name per module, DDR */
		uint16_t auth_id;
		/**< Authentication ID to be used for session with server*/
		uint32_t dpci_id;
		/**< DPCI id as known by GPP side */
	} gpp[10];
	struct {
		uint32_t dpci_id;
		/**< DPCI id as known by GPP side */
		void *attr;		
	} dpci[10];
};


#endif /* __CMDIF_CLIENT_H */
