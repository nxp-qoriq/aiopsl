#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"

int single_cluster_test();

int single_cluster_test()
{
	int err = 0;

#ifndef ARENA_TEST
	/* Skip init because ARENA test has full boot flow */
#endif

	/* Check that all n=<4 cores get here use CORE_ID */

	/* Check smp.c API */

	/* Check fsl_spinlock.h API */

	return err;
}
