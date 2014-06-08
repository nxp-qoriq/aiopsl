#ifndef __CMDIF_SRV_AIOP_H
#define __CMDIF_SRV_AIOP_H

#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "cmdif_srv.h"

struct cmdif_srv_aiop {
	struct cmdif_srv *srv;
	/**< Common Server fields */
	uint8_t lock;
	/**< cmdif spinlock used for module id allocation */
};

void cmdif_srv_isr(void);

#endif /* __CMDIF_SRV_H */
