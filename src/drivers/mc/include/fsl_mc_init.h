#ifndef __FSL_MC_INIT_H
#define __FSL_MC_INIT_H

#include "dplib/fsl_dpci.h"


struct dpci_obj {
	struct dpci_attr *attr;
	struct dpci      *dpci;
	int    count;
};

#endif /*__FSL_MC_INIT_H */
