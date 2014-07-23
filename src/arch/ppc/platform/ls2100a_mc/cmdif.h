#ifndef __CMDIF_H
#define __CMDIF_H

#include <fsl_errors.h>
#include <types.h>
#include <fsl_string.h>
#include <fsl_stdlib.h>
#include <fsl_endian.h>

#ifndef __HOT_CODE
#define __HOT_CODE
#endif

#ifndef CORE_IS_BIG_ENDIAN
#error "MC core must be big endian\n!"
#endif

#define CPU_TO_SRV16(val) CPU_TO_BE16(val)
#define CPU_TO_SRV32(val) CPU_TO_BE32(val)

#define CMDIF_EPID         0     /*!< EPID to be used for setting by client */

#endif /* __CMDIF_H */
