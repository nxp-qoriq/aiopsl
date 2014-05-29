/**************************************************************************//*
 @File          fsl_dpcon_cmd.h

 @Description   defines dprc portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPCON_CMD_H
#define _FSL_DPCON_CMD_H

/* cmd IDs */
#define MC_CMDID_CLOSE					0x800
#define MC_DPCON_CMDID_OPEN				0x808
#define MC_DPCON_CMDID_CREATE				0x908

#define DPCON_CMDID_DESTROY				0x171
#define DPCON_CMDID_ENABLE				0x172
#define DPCON_CMDID_DISABLE				0x173
#define DPCON_CMDID_GET_ATTR				0x174
#define DPCON_CMDID_SET_NOTIFICATION			0x175
#define DPCON_CMDID_SET_IRQ				0x176
#define DPCON_CMDID_GET_IRQ				0x177
#define DPCON_CMDID_SET_IRQ_ENABLE			0x178
#define DPCON_CMDID_GET_IRQ_ENABLE			0x179
#define DPCON_CMDID_SET_IRQ_MASK			0x17A
#define DPCON_CMDID_GET_IRQ_MASK			0x17B
#define DPCON_CMDID_GET_IRQ_STATUS			0x17C
#define DPCON_CMDID_CLEAR_IRQ_STATUS			0x17E
#define DPCON_CMDID_RESET				0x17F

/* cmd sizes */
#define MC_CMD_OPEN_SIZE				8
#define MC_CMD_CLOSE_SIZE				0
#define DPCON_CMDSZ_CREATE				8
#define DPCON_CMDSZ_DESTROY				0
#define DPCON_CMDSZ_ENABLE				0
#define DPCON_CMDSZ_DISABLE				0
#define DPCON_CMDSZ_GET_ATTR				(8 * 2)
#define DPCON_CMDSZ_SET_NOTIFICATION			(8 * 2)
#define DPCON_CMDSZ_SET_IRQ				(8 * 3)
#define DPCON_CMDSZ_GET_IRQ				(8 * 3)
#define DPCON_CMDSZ_SET_IRQ_ENABLE			8
#define DPCON_CMDSZ_GET_IRQ_ENABLE			8
#define DPCON_CMDSZ_SET_IRQ_MASK			8
#define DPCON_CMDSZ_GET_IRQ_MASK			8
#define DPCON_CMDSZ_GET_IRQ_STATUS			8
#define DPCON_CMDSZ_CLEAR_IRQ_STATUS			8
#define DPCON_CMDSZ_RESET				0

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_OPEN(_OP) \
	_OP(0,  0,	32,	int,			dpcon_id)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_CREATE(_OP) \
	_OP(0,  0,	8,	uint8_t,		cfg->num_priorities)

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_ATTR(_OP) \
do { \
	_OP(0,  0,	32,	int,			attr->id);\
	_OP(0,  32,	16,	uint16_t,		attr->qbman_ch_id);\
	_OP(0,  48,	8,	uint8_t,		attr->num_priorities);\
	_OP(1,  0,	32,	uint32_t,		attr->version.major);\
	_OP(1,  32,	32,	uint32_t,		attr->version.minor);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_SET_NOTIFICATION(_OP) \
do { \
	_OP(0,  0,	16,	uint16_t,		cfg->dpio_id);\
	_OP(0,  16,	8,	uint8_t,		cfg->priority);\
	_OP(1,  0,	64,	uint64_t,		cfg->user_ctx);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_SET_IRQ(_OP) \
do { \
	_OP(0,  0,	8,	uint8_t,		irq_index);\
	_OP(0,  32,	32,	uint32_t,		irq_val);\
	_OP(1,  0,	64,	uint64_t,		irq_paddr);\
	_OP(2,  0,	32,	int,			user_irq_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_IRQ(_OP) \
do { \
	_OP(0,  0,	32,	uint32_t,		irq_val);\
	_OP(1,  0,	64,	uint64_t,		irq_paddr);\
	_OP(2,  0,	32,	int,			user_irq_id); \
	_OP(2,  32,	32,	int,			type);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_SET_IRQ_ENABLE(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		enable_state); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCON_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_SET_IRQ_MASK(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		mask); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

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
	_OP(0,	0,	32,	uint32_t,		status)

/*	param, offset, width,	type,			arg_name */
#define DPCON_CMD_CLEAR_IRQ_STATUS(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		status); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

#endif /* _FSL_DPCON_CMD_H */
