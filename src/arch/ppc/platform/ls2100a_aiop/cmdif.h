#ifndef __CMDIF_H
#define __CMDIF_H

#include "fsl_endian.h"

#ifndef CORE_IS_BIG_ENDIAN
#error "AIOP core must be big endian\n!"
#endif

#define CPU_TO_SRV16 CPU_TO_LE16
#define CPU_TO_SRV32 CPU_TO_LE32

#define CPU_TO_CL16 CPU_TO_LE16
#define CPU_TO_CL32 CPU_TO_LE32

#endif /* __CMDIF_H */
