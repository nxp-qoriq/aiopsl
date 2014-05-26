/**************************************************************************//*
 @File          fsl_dpcon_cmd.h

 @Description   defines dprc portal commands

 @Cautions      None.
 *//***************************************************************************/
#if 0
#ifndef _FSL_DPCON_CMD_H
#define _FSL_DPCON_CMD_H

/* cmd IDs */
#define DPCON_CMDID_INIT				0x170
#define DPCON_CMDID_DONE				0x171
#define DPCON_CMDID_ENABLE				0x172
#define DPCON_CMDID_DISABLE				0x173
#define DPCON_CMDID_GET_ATTR				0x174
#define DPCON_CMDID_SET_IRQ				0x175
#define DPCON_CMDID_GET_IRQ				0x109
#define DPCON_CMDID_SET_IRQ_ENABLE			0x10A
#define DPCON_CMDID_GET_IRQ_ENABLE			0x10B
#define DPCON_CMDID_SET_IRQ_MASK				0x10C
#define DPCON_CMDID_GET_IRQ_MASK				0x10D
#define DPCON_CMDID_GET_IRQ_STATUS			0x10E
#define DPCON_CMDID_CLEAR_IRQ_STATUS			0x10F

/* cmd sizes */
#define DPCON_CMDSZ_INIT				8
#define DPCON_CMDSZ_DONE				0
#define DPCON_CMDSZ_ENABLE				0
#define DPCON_CMDSZ_DISABLE				0
#define DPCON_CMDSZ_GET_ATTR				8
#define DPCON_CMDSZ_SET_IRQ				(8 * 2)
#define DPCON_CMDSZ_GET_IRQ				(8 * 2)
#define DPCON_CMDSZ_SET_IRQ_ENABLE			8
#define DPCON_CMDSZ_GET_IRQ_ENABLE			8
#define DPCON_CMDSZ_SET_IRQ_MASK				8
#define DPCON_CMDSZ_GET_IRQ_MASK				8
#define DPCON_CMDSZ_GET_IRQ_STATUS			8
#define DPCON_CMDSZ_CLEAR_IRQ_STATUS			8

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_INIT(_OP) \
	_OP(0,  0,	8,	uint8_t,		cfg->num_priorities)\

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_ATTR(_OP) \
	_OP(0,  0,	8,	uint8_t,		attr->num_priorities)\
	_OP(0,  32,	16,	uint16_t,		attr->qbman_ch_id)\

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_SET_IRQ(_OP) \
	_OP(0,  0,	8,	uint8_t,		irq_index)\
	_OP(0,  32,	32,	uint32_t,		irq_val)\
	_OP(0,  0,	64,	uint64_t,		irq_paddr)\

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_IRQ(_OP) \
	_OP(0,  32,	32,	uint32_t,		irq_val)\
	_OP(0,  0,	64,	uint64_t,		irq_paddr)\

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_SET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_SET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_CLEAR_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status) \
	_OP(0,	32,	8,	uint8_t,		irq_index)
#endif /* _FSL_DPCON_CMD_H */
#endif
