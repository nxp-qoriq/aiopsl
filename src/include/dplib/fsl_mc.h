#ifndef __FSL_MC_H
#define __FSL_MC_H

#include "common/types.h"


struct mc_portal_regs {
	uint64_t header;
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};

struct cmdif_cmd_desc {
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};

#define MC_CMD_OPEN_SIZE	8
#define MC_CMD_CLOSE_SIZE	0

/**************************************************************************//**
 @Collection    Command portal Header
 @{
 *//***************************************************************************/
#define MC_CMD_CMDID_OFFSET	52	/**< Command offset */
#define MC_CMD_CMDID_SIZE	12	/**< command size */
#define MC_CMD_AUTHID_OFFSET	38	/**< Authentication offset */
#define MC_CMD_AUTHID_SIZE	10	/**< Authentication size */
#define MC_CMD_SIZE_OFFSET	32	/**< Size offset */
#define MC_CMD_SIZE_SIZE	6	/**< Size of size */
#define MC_CMD_STATUS_OFFSET	16	/**< Status offset */
#define MC_CMD_STATUS_SIZE	8	/**< Status size*/
#define MC_CMD_PRI_OFFSET	15	/**< Priority offset */
#define MC_CMD_PRI_SIZE		1	/**< Priority size */


#define MC_CMD_DPNI_OPEN	0x201
#define MC_CMD_DPSW_OPEN	0x202
#define MC_CMD_DPIO_OPEN	0x203
#define MC_CMD_DPSP_OPEN	0x204
#define MC_CMD_DPRC_OPEN	0x205

#define MC_CMD_CLOSE		0x200

/* @} */

/**************************************************************************//**
 @Collection    Read/Write command portal macros
 @{
 *//***************************************************************************/
#define MC_CMD_READ_CMDID(_hdr)		u64_read_field((_hdr), MC_CMD_CMDID_OFFSET, MC_CMD_CMDID_SIZE)
#define MC_CMD_READ_AUTHID(_hdr)	u64_read_field((_hdr), MC_CMD_AUTHID_OFFSET, MC_CMD_AUTHID_SIZE)
#define MC_CMD_READ_SIZE(_hdr)		u64_read_field((_hdr), MC_CMD_SIZE_OFFSET, MC_CMD_SIZE_SIZE)
#define MC_CMD_READ_STATUS(_hdr)	u64_read_field((_hdr), MC_CMD_STATUS_OFFSET, MC_CMD_STATUS_SIZE)
#define MC_CMD_READ_PRI(_hdr)		u64_read_field((_hdr), MC_CMD_PRI_OFFSET, MC_CMD_PRI_SIZE)

#define MC_CMD_READ_HEADER(_ptr)	((_ptr)->header)

#define MC_CMD_READ_PARAM(_ptr, _id)	((_ptr)->param##_id)

#define MC_CMD_WRITE_PARAM(_ptr, _id, _val) ((_ptr)->param##_id = (_val))

#define MC_CMD_WRITE_HEADER(_ptr, _id, _auth, _size, _status, _pri) 			\
	do { 										\
		volatile uint64_t tmp = 0;							\
		u64_write_field(tmp, MC_CMD_CMDID_OFFSET, MC_CMD_CMDID_SIZE, (_id));	\
		u64_write_field(tmp, MC_CMD_AUTHID_OFFSET, MC_CMD_AUTHID_SIZE, (_auth));	\
		u64_write_field(tmp, MC_CMD_SIZE_OFFSET, MC_CMD_SIZE_SIZE, (_size));	\
		u64_write_field(tmp, MC_CMD_STATUS_OFFSET, MC_CMD_STATUS_SIZE, (_status));	\
		u64_write_field(tmp, MC_CMD_PRI_OFFSET, MC_CMD_PRI_SIZE, (_pri));		\
		(_ptr)->header = tmp; \
	} while (0)

/* @} */


#endif /* __FSL_MC_H */
