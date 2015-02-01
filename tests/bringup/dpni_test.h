/**
@File          dpni_test.h

@Description   Internal header file for dpni test
*//***************************************************************************/
#ifndef __DPNI_TEST_H
#define __DPNI_TEST_H

#include "common/types.h"
#include "fsl_mc_init.h"
int test_dpni_drv_probe(struct mc_dprc *dprc,
        uint16_t mc_niid,
        uint16_t aiop_niid);


#endif /* __DPNI_TEST_H */

