/**************************************************************************//*
 @File          fsl_dprc_cmd.h

 @Description   defines dprc portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPBP_CMD_H
#define _FSL_DPBP_CMD_H

/* cmd IDs */
#define DPBP_CMDID_INIT					0x103
#define DPBP_CMDID_DONE					0x104
#define DPBP_CMDID_ENABLE				0x105
#define DPBP_CMDID_DISABLE				0x106
#define DPBP_CMDID_GET_ATTR				0x107
#define DPBP_CMDID_SET_IRQ				0x108
#define DPBP_CMDID_GET_IRQ				0x109
#define DPBP_CMDID_SET_IRQ_ENABLE			0x10A
#define DPBP_CMDID_GET_IRQ_ENABLE			0x10B
#define DPBP_CMDID_SET_IRQ_MASK				0x10C
#define DPBP_CMDID_GET_IRQ_MASK				0x10D
#define DPBP_CMDID_GET_IRQ_STATUS			0x10E
#define DPBP_CMDID_CLEAR_IRQ_STATUS			0x10F

/* cmd sizes */
#define DPBP_CMDSZ_INIT					(8 * 3)
#define DPBP_CMDSZ_DONE					0
#define DPBP_CMDSZ_ENABLE				0
#define DPBP_CMDSZ_DISABLE				0
#define DPBP_CMDSZ_GET_ATTR				(8 * 3)
#define DPBP_CMDSZ_SET_IRQ				(8 * 3)
#define DPBP_CMDSZ_GET_IRQ				(8 * 2)
#define DPBP_CMDSZ_SET_IRQ_ENABLE			8
#define DPBP_CMDSZ_GET_IRQ_ENABLE			8
#define DPBP_CMDSZ_SET_IRQ_MASK				8
#define DPBP_CMDSZ_GET_IRQ_MASK				8
#define DPBP_CMDSZ_GET_IRQ_STATUS			8
#define DPBP_CMDSZ_CLEAR_IRQ_STATUS			8

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_INIT(_OP) \
	_OP(0,  0,	16,	uint16_t,		cfg->buffer_size)

/*	param, offset, width,	type,			arg_name */
#define DPBP_RSP_GET_ATTRIBUTES(_OP) \
	_OP(0,  0,	16,	uint16_t,		attr->buffer_size) \
	_OP(0,  16,	16,	uint16_t,		attr->bpid)

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_SET_IRQ(_OP) \
	_OP(0,  0,	8,	uint8_t,		irq_index)\
	_OP(0,  32,	32,	uint32_t,		irq_val)\
	_OP(0,  0,	64,	uint64_t,		irq_paddr)

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPBP_RSP_GET_IRQ(_OP) \
	_OP(0,	0,	32,	uint32_t,		irq_val) \
	_OP(1,	0,	64,	uint64_t,		irq_paddr)

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_SET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPBP_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)
	
/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_SET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPBP_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) 

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPBP_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \

/*	param, offset, width,	type,			arg_name */
#define DPBP_CMD_CLEAR_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

#endif /* _FSL_DPBP_CMD_H */
