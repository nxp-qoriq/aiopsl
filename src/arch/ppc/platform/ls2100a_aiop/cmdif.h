#ifndef __CMDIF_H
#define __CMDIF_H

#include "fsl_endian.h"

#ifndef CORE_IS_BIG_ENDIAN
#error "AIOP core must be big endian\n!"
#endif

#define CPU_TO_SRV16 CPU_TO_BE16

#endif /* __CMDIF_H */
