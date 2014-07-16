#ifndef __CMDIF_H
#define __CMDIF_H

#include <errno.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <nadk_types.h>
#include <nadk_byteorder.h>

#ifndef __HOT_CODE
#define __HOT_CODE
#endif

#define CPU_TO_SRV16(val) rte_bswap16(val)
#define CPU_TO_SRV32(val) rte_bswap32(val)
#define CPU_TO_BE64(val)  rte_bswap64(val)
#define CPU_TO_BE16(val)  rte_bswap16(val)


#endif /* __CMDIF_H */
