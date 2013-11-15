#ifndef __FSL_MC_H
#define __FSL_MC_H

#include "common/types.h"
#include "common/endian.h"


struct cmdif_cmd_desc {
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};


/**************************************************************************//**
 @Collection    Read/Write command portal macros
 @{
 *//***************************************************************************/
#define CMDIF_MC_READ_CMDID(_hdr)		u64_read_field((_hdr), CMDIF_MC_CMDID_OFFSET, CMDIF_MC_CMDID_SIZE)
#define CMDIF_MC_READ_AUTHID(_hdr)	u64_read_field((_hdr), CMDIF_MC_AUTHID_OFFSET, CMDIF_MC_AUTHID_SIZE)
#define CMDIF_MC_READ_SIZE(_hdr)		u64_read_field((_hdr), CMDIF_MC_SIZE_OFFSET, CMDIF_MC_SIZE_SIZE)
#define CMDIF_MC_READ_STATUS(_hdr)	u64_read_field((_hdr), CMDIF_MC_STATUS_OFFSET, CMDIF_MC_STATUS_SIZE)
#define CMDIF_MC_READ_PRI(_hdr)		u64_read_field((_hdr), CMDIF_MC_PRI_OFFSET, CMDIF_MC_PRI_SIZE)

#define CMDIF_MC_READ_HEADER(_ptr)	((_ptr)->header)

#define CMDIF_MC_READ_PARAM(_ptr, _id)	((_ptr)->param##_id)

#define CMDIF_MC_WRITE_PARAM(_ptr, _id, _val) ((_ptr)->param##_id = (_val))

#define CMDIF_MC_WRITE_HEADER(_ptr, _id, _auth, _size, _status, _pri)					\
	do { 																			\
		volatile uint64_t tmp = 0;													\
		u64_write_field(tmp, CMDIF_MC_CMDID_OFFSET, CMDIF_MC_CMDID_SIZE, (_id));		\
		u64_write_field(tmp, CMDIF_MC_AUTHID_OFFSET, CMDIF_MC_AUTHID_SIZE, (_auth));	\
		u64_write_field(tmp, CMDIF_MC_SIZE_OFFSET, CMDIF_MC_SIZE_SIZE, (_size));		\
		u64_write_field(tmp, CMDIF_MC_STATUS_OFFSET, CMDIF_MC_STATUS_SIZE, (_status));	\
		u64_write_field(tmp, CMDIF_MC_PRI_OFFSET, CMDIF_MC_PRI_SIZE, (_pri));			\
		(_ptr)->header = swap_uint64(tmp);											\
	} while (0)

/* @} */


#endif /* __FSL_MC_H */
