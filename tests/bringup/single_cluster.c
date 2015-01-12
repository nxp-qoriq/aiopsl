#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "bringup_tests.h"
#include "fsl_soc.h"

#if (TEST_SINGLE_CLUSTER == ON)

extern struct aiop_init_info g_init_data;

int single_cluster_test()
{
	int err = 0;

	/* Check that all n=<4 cores get here use CORE_ID */
	
	/* Check smp.c API */

	/* Check fsl_spinlock.h API */

	return err;
}

#endif /* (TEST_SINGLE_CLUSTER == ON) */
