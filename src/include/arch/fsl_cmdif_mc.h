#ifndef __FSL_CMDIF_MC_H
#define __FSL_CMDIF_MC_H

#include "common/types.h"
#include "common/endian.h"


struct cmdif_cmd_data {
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};

struct mc_portal_regs {
	volatile uint64_t header;
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};


/**************************************************************************//**
 @Collection    Command portal Header
 @{
 *//***************************************************************************/
#define CMDIF_MC_CMDID_OFFSET	52	/**< Command offset */
#define CMDIF_MC_CMDID_SIZE	12	/**< command size */
#define CMDIF_MC_AUTHID_OFFSET	38	/**< Authentication offset */
#define CMDIF_MC_AUTHID_SIZE	10	/**< Authentication size */
#define CMDIF_MC_SIZE_OFFSET	31	/**< Size offset */
#define CMDIF_MC_SIZE_SIZE	6	/**< Size of size */
#define CMDIF_MC_STATUS_OFFSET	16	/**< Status offset */
#define CMDIF_MC_STATUS_SIZE	8	/**< Status size*/
#define CMDIF_MC_PRI_OFFSET	12	/**< Priority offset */
#define CMDIF_MC_PRI_SIZE	1	/**< Priority size */
/* @} */

/**************************************************************************//**
 @Collection    Read/Write command portal macros
 @{
 *//***************************************************************************/
#define CMDIF_MC_READ_PARAM(_ptr, _id)	        ((_ptr)->param##_id)
#define CMDIF_MC_WRITE_PARAM(_ptr, _id, _val)   ((_ptr)->param##_id = (_val))

#define GPP_CMD_READ_PARAM(_ptr, _id)	        swap_uint64((_ptr)->param##_id)
#define GPP_CMD_WRITE_PARAM(_ptr, _id, _val)    ((_ptr)->param##_id = swap_uint64(_val))
/* @} */


#endif /* __FSL_CMDIF_MC_H */
