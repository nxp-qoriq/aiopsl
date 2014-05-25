#if 0
#ifndef _FSL_DPLIB_TESTS_H
#define _FSL_DPLIB_TESTS_H

#ifdef __uboot__
#include <malloc.h>
#include <dplib/fsl_dplib_sys.h>
#include <dplib/fsl_dprc.h>
#include <arch/fsl_core_arm.h>
#else // __uboot__
#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
//#include "stdlib.h"
#include "kernel/platform.h"
#endif // __uboot__

#define DPRC_TYPE 10

#ifdef __uboot__

#define MC_PORTALS_BASE		0x80c000000
#define MC_PORTAL_SIZE		0x10000
#define GET_MC_PORTAL_BASE(id)  (void*)(MC_PORTALS_BASE + (id * MC_PORTAL_SIZE))

#else

#define MC_PORTALS_BASE		0x80000000
#define MC_PORTAL_SIZE		0x10000
#define GET_MC_PORTAL_BASE(id)  (void*)(MC_PORTALS_BASE + (id * MC_PORTAL_SIZE))

#endif /* __uboot__ */

#endif /* _FSL_DPLIB_SYS_H */

#endif
