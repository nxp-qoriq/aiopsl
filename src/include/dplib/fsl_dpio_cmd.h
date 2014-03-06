/**************************************************************************//*
 @File          fsl_dpio_cmd.h

 @Description   defines dprc portal commandsF

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPIO_CMD_H
#define _FSL_DPIO_CMD_H

/* cmd IDs */
#define DPIO_CMDID_INIT					0x112
#define DPIO_CMDID_DONE					0x113
#define DPIO_CMDID_ENABLE				0x114
#define DPIO_CMDID_DISABLE				0x115
#define DPIO_CMDID_GET_ATTR				0x116
#define DPIO_CMDID_SET_IRQ				0x117
#define DPIO_CMDID_GET_IRQ				0x118
#define DPIO_CMDID_SET_IRQ_ENABLE			0x119
#define DPIO_CMDID_GET_IRQ_ENABLE			0x11A
#define DPIO_CMDID_SET_IRQ_MASK				0x11B
#define DPIO_CMDID_GET_IRQ_MASK				0x11C
#define DPIO_CMDID_GET_IRQ_STATUS			0x11D
#define DPIO_CMDID_CLEAR_IRQ_STATUS			0x11E

/* cmd sizes */
#define DPIO_CMDSZ_INIT					8
#define DPIO_CMDSZ_DONE					0
#define DPIO_CMDSZ_ENABLE				0
#define DPIO_CMDSZ_DISABLE				0
#define DPIO_CMDSZ_GET_ATTR				(8 * 3)
#define DPIO_CMDSZ_SET_IRQ				(8 * 2)
#define DPIO_CMDSZ_GET_IRQ				(8 * 2)
#define DPIO_CMDSZ_SET_IRQ_ENABLE			8
#define DPIO_CMDSZ_GET_IRQ_ENABLE			8
#define DPIO_CMDSZ_SET_IRQ_MASK				8
#define DPIO_CMDSZ_GET_IRQ_MASK				8
#define DPIO_CMDSZ_GET_IRQ_STATUS			8
#define DPIO_CMDSZ_CLEAR_IRQ_STATUS			8

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_INIT(_OP) \
/*	_OP(0,  0,	16,	int,		cfg->dpio_id) TODO - remove?*/\
	_OP(0,  16,	2,	enum dpio_channel_mode,	cfg->channel_mode)\
	_OP(0,  32,	8,	uint8_t,		cfg->num_priorities)\

/*	param, offset, width,	type,		arg_name */
#define DPIO_RSP_GET_ATTR(_OP) \
	_OP(0,  16,	16,	uint16_t,	attr->qbman_portal_id)\
	_OP(0,  32,	8,	uint8_t,	attr->num_priorities)\
	_OP(1,  0,	64,	dma_addr_t,	attr->qbman_portal_ce_paddr)\
	_OP(2,  0,	64,	dma_addr_t,	attr->qbman_portal_ci_paddr)\

/*	param, offset, width,	type,		arg_name */
#define DPIO_CMD_SET_IRQ(_OP) \
	_OP(0,  0,	8,	uint8_t,	irq_index)\
	_OP(0,  32,	32,	uint32_t,	irq_val)\
	_OP(0,  0,	64,	uint64_t,	irq_paddr)\

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ(_OP) \
	_OP(0,	0,	32,	uint32_t,		irq_val) \
	_OP(1,	0,	64,	uint64_t,		irq_paddr)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_SET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)
	
/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_SET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) 

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPIO_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \

/*	param, offset, width,	type,			arg_name */
#define DPIO_CMD_CLEAR_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

#endif /* _FSL_DPIO_CMD_H */
