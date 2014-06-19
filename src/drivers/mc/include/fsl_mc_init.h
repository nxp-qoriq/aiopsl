#ifndef __FSL_MC_INIT_H
#define __FSL_MC_INIT_H

#include "dplib/fsl_dpci.h"

#define DPCI_OBJ_MN	20 /**< Maximal number of dpci objects */

struct dpci_obj {
	struct dpci_attr attr[DPCI_OBJ_MN];
	struct dpci      dpci[DPCI_OBJ_MN];
	int    count;
};

#endif /*__FSL_MC_INIT_H */
