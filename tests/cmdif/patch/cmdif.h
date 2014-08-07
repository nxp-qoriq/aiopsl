#ifndef __CMDIF_H
#define __CMDIF_H

#include <fsl_errors.h>
#include <types.h>
#include <fsl_string.h>
#include <fsl_stdlib.h>
#include <fsl_endian.h>

#ifndef CORE_IS_BIG_ENDIAN
#error "AIOP core must be big endian\n!"
#endif

#define CMDIF_TEST_WITH_MC_SRV

#ifdef CMDIF_TEST_WITH_MC_SRV
#define CPU_TO_SRV16(val) CPU_TO_BE16(val)
#define CPU_TO_SRV32(val) CPU_TO_BE32(val)

#else
#define CPU_TO_SRV16(val) CPU_TO_LE16(val)
#define CPU_TO_SRV32(val) CPU_TO_LE32(val)
#endif

#define CMDIF_EPID         2    /*!< EPID to be used for setting by client */

#endif /* __CMDIF_H */
