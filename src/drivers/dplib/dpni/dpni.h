/**************************************************************************//*
 @File          dpni.h

 @Description   DPNI FLib internal file

 @Cautions      None.
 *//***************************************************************************/

#ifndef _DPNI_H
#define _DPNI_H

#include "fsl_endian.h"
#include "common/fsl_cmdif.h"
#include "dplib/fsl_dpni.h"

struct cmdif_cmd_data {
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};

static __inline__ uint64_t u64_read_field(uint64_t reg, int start_bit, int size)
{
	if (size >= 64)
		return reg;
	return (reg >> start_bit) & ((0x0000000000000001LL << size) - 1);
}

static __inline__ uint64_t u64_write_field(uint64_t reg,
                                           int start_bit,
                                           int size,
                                           uint64_t val)
{
	if (size >= 64)
		reg = val;
	else
		reg |= (uint64_t)(
		        (val & ((0x0000000000000001LL << size) - 1))
		        << start_bit);
	return reg;
}

#define GPP_CMD_READ_PARAM(_ptr, _id) \
	swap_uint64(((struct cmdif_cmd_data *)(_ptr))->param##_id)

#define GPP_CMD_WRITE_PARAM(_ptr, _id, _val) \
	((_ptr)->param##_id = swap_uint64(_val))

#endif /* _DPNI_H */
