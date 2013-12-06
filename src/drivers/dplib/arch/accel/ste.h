/**************************************************************************//**
@File		ste.h

@Description	This file contains the AIOP statistics API.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_LIB_STATISTICS_H
#define __AIOP_LIB_STATISTICS_H

#define STE_CTR_CMD_MEM_ADDR		0x0010000
#define STE_CTR_8B_SET_CMD_MEM_ADDR	0x40

#define STE_CTR_SET_CMD_TYPE		0x1181
#define STE_CTR_GET_CMD_TYPE		0x0981
#define STE_CTR_SET_WRITE_DATA_ACCESS	0x8000
#define STE_CTR_SET_READ_DATA_ACCESS	0x4000

#define STE_CTR_UPDATE_CTR_8_BYTE	0x10000000
#define STE_CTR_UPDATE_ACC_8_BYTE	0x20000000

#define STE_OSM_REQ_TYPE		0x4


/*! \enum e_ste_cmd_type defines the statistics engine CMDTYPE field.*/
enum e_ste_cmd_type {
	STE_CMDTYPE_SET = 0x1,
	STE_CMDTYPE_SYNC = 0x7,
	STE_CMDTYPE_ADD = 0x8,
	STE_CMDTYPE_SUB = 0x9,
	STE_CMDTYPE_INC_ADD = 0xC,
	STE_CMDTYPE_INC_SUB = 0xD,
	STE_CMDTYPE_DEC_ADD = 0xE,
	STE_CMDTYPE_DEC_SUB = 0xF
};


/* @} end of group STE_functions */
/* @} end of group STATS */

#endif /* __AIOP_LIB_STATISTICS_H */
