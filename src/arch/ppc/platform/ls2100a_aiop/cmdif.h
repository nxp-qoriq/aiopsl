#ifndef __CMDIF_H
#define __CMDIF_H

#include "fsl_endian.h"

#ifndef CORE_IS_BIG_ENDIAN
#error "AIOP core must be big endian\n!"
#endif

#define CPU_TO_SRV16(val) CPU_TO_LE16(val)
#define CPU_TO_SRV32(val) CPU_TO_LE32(val)

#define CPU_TO_CL16(val) CPU_TO_LE16(val)
#define CPU_TO_CL32(val) CPU_TO_LE32(val)

#endif /* __CMDIF_H */
