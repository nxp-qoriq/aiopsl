/**************************************************************************//*
 @File          fsl_dpmac_cmd.h

 @Description   defines dpmac portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPMAC_CMD_H
#define _FSL_DPMAC_CMD_H


/* cmd IDs */
#define DPMAC_CMDID_MDIO_READ			0x170 
#define DPMAC_CMDID_MDIO_WRITE			0x171
#define DPMAC_CMDID_ADJUST_LINK			0x172
#define DPMAC_CMDID_GET_LINK_STATE		0x173
#define DPMAC_CMDID_GET_LINK_SPEED		0x174

/* cmd sizes */
#define DPMAC_CMDSZ_MDIO_READ			8 
#define DPMAC_CMDSZ_MDIO_WRITE			8
#define DPMAC_CMDSZ_ADJUST_LINK			(8 * 2)
#define DPMAC_CMDSZ_GET_LINK_STATE		0
#define DPMAC_CMDSZ_GET_LINK_SPEED		0

/* 	param, offset, width, 	type, 			arg_name */
#define DPMAC_CMD_MDIO_READ(_OP) \
	_OP(0, 	0, 	8, 	uint8_t, 		cfg->phy_addr) \
	_OP(0,  8, 	8, 	uint8_t,		cfg->reg) \
	
/* 	param, offset, width, 	type, 			arg_name */
#define DPMAC_RSP_MDIO_READ(_OP) \
	_OP(0,  16, 	16, 	uint16_t,		*(cfg->data) )\

/* 	param, offset, width, 	type, 			arg_name */
#define DPMAC_CMD_MDIO_WRITE(_OP) \
	_OP(0, 	0, 	8, 	uint8_t, 		cfg->phy_addr) \
	_OP(0,  8, 	8, 	uint8_t,		cfg->reg) \
	_OP(0,  16, 	16, 	uint16_t,		cfg->data) \
	
/* 	param, offset, width, 	type, 			arg_name */
#define DPMAC_CMD_ADJUST_LINK(_OP) \
	_OP(0,  8, 	1, 	int,			cfg->full_duplex) \
	_OP(1, 	0, 	64, 	uint64_t, 		cfg->rate) \


#endif /* _FSL_DPMAC_CMD_H */
