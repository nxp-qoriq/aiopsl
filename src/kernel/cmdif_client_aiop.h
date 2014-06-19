
/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    cmdif_client.h
 *  @brief   Cmdif client AIOP<->GPP internal header file
 */

#ifndef __CMDIF_CLIENT_AIOP_H
#define __CMDIF_CLIENT_AIOP_H

#include "cmdif_client.h"
#include "dplib/fsl_dpci.h"

#define CMDIF_MN_SESSIONS	20 /**< Maximal number of sessions */
#define CMDIF_NUM_PR  		2

struct cmdif_reg {
	struct dpci *dpci_dev;
	/**< Open AIOP dpci device */
	struct dpci_attr *attr;
};

/* To be allocated on DDR */
struct cmdif_cl {
	struct {
		char m_name[M_NAME_CHARS + 1];
		/**< Module application name */
		struct cmdif_reg *regs;
		/**< Send device, to be placed as cidesc.reg */
		struct cmdif_dev *dev;
		/**< To be placed as cidesc.dev */
		uint8_t ins_id;
		/**< Instanse id that was used for open */
	} gpp[CMDIF_MN_SESSIONS];
	
	uint8_t count;
	/**< Count the number of sessions */
};


#endif /* __CMDIF_CLIENT_H */
