/**************************************************************************//*
 @File          fsl_dpci_cmd.h

 @Description   defines dprc portal commandsF

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPCI_CMD_H
#define _FSL_DPCI_CMD_H

/* cmd IDs */
#define MC_CMDID_CLOSE					0x800
#define MC_DPCI_CMDID_OPEN				0x807
#define MC_DPCI_CMDID_CREATE				0x907

#define DPCI_CMDID_DESTROY				0x182
#define DPCI_CMDID_SET_RX_QUEUE				0x184
#define DPCI_CMDID_FREE_RX_QUEUE			0x185
#define DPCI_CMDID_ENABLE				0x186
#define DPCI_CMDID_DISABLE				0x187
#define DPCI_CMDID_RESET				0x188
#define DPCI_CMDID_GET_ATTR				0x189
#define DPCI_CMDID_SET_IRQ				0x18a
#define DPCI_CMDID_GET_IRQ				0x18b
#define DPCI_CMDID_SET_IRQ_ENABLE			0x18c
#define DPCI_CMDID_GET_IRQ_ENABLE			0x18d
#define DPCI_CMDID_SET_IRQ_MASK				0x18e
#define DPCI_CMDID_GET_IRQ_MASK				0x18f
#define DPCI_CMDID_GET_IRQ_STATUS			0x190
#define DPCI_CMDID_CLEAR_IRQ_STATUS			0x191
#define DPCI_CMDID_GET_LINK_STATE			0x192

/* cmd sizes */
#define MC_CMD_OPEN_SIZE				8
#define MC_CMD_CLOSE_SIZE				0
#define DPCI_CMDSZ_CREATE				8
#define DPCI_CMDSZ_DESTROY				0
#define DPCI_CMDSZ_SET_RX_QUEUE				(8 * 2)
#define DPCI_CMDSZ_FREE_RX_QUEUE			8
#define DPCI_CMDSZ_ENABLE				0
#define DPCI_CMDSZ_DISABLE				0
#define DPCI_CMDSZ_RESET				0
#define DPCI_CMDSZ_GET_ATTR				(8 * 6)
#define DPCI_CMDSZ_SET_IRQ				(8 * 3)
#define DPCI_CMDSZ_GET_IRQ				(8 * 3)
#define DPCI_CMDSZ_SET_IRQ_ENABLE			8
#define DPCI_CMDSZ_GET_IRQ_ENABLE			8
#define DPCI_CMDSZ_SET_IRQ_MASK				8
#define DPCI_CMDSZ_GET_IRQ_MASK				8
#define DPCI_CMDSZ_GET_IRQ_STATUS			8
#define DPCI_CMDSZ_CLEAR_IRQ_STATUS			8
#define DPCI_CMDSZ_GET_LINK_STATE			8
/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_OPEN(_OP) \
	_OP(0,  0,	32,	int,			dpci_id)

/*	param, offset, width,	type,		arg_name */
#define DPCI_CMD_CREATE(_OP) \
do { \
	_OP(0,  0,	8,	uint8_t,	cfg->num_of_Priorities);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPCI_CMD_SET_RX_QUEUE(_OP) \
do { \
	_OP(0,  0,	16,	uint16_t,	dest_cfg->dpio_id);\
	_OP(0,  16,	16,	uint16_t,	dest_cfg->dpcon_id);\
	_OP(0,  32,	8,	uint8_t,	dest_cfg->priority);\
	_OP(0,  40,	8,	uint8_t,	priority);\
	_OP(0,  48,	4,	enum dpci_dest,	dest_cfg->type);\
	_OP(1,  0,	64,	uint64_t,	rx_user_ctx);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPCI_CMD_FREE_RX_QUEUE(_OP) \
	_OP(0,  0,	8,	uint8_t,	priority)

/*	param, offset, width,	type,			arg_name */
#define DPCI_RSP_GET_LINK_STATE(_OP) \
	_OP(0,	0,	1,	int,			up)

/*	param, offset, width,	type,		arg_name */
#define DPCI_RSP_GET_ATTR(_OP) \
do { \
	_OP(0,  0,	32,	int,		attr->id);\
	_OP(0,  32,	8,	uint8_t,	attr->attach_link);\
	_OP(0,  40,	8,	uint8_t,	attr->peer_id);\
	_OP(1,  0,	8,	uint8_t,	attr->num_of_priorities);\
	_OP(1,  9,	1,	int,		attr->enable);\
	_OP(2,  0,	16,	uint16_t, attr->dpci_prio_attr[0].tx_qid);\
	_OP(2,  16,	16,	uint16_t, attr->dpci_prio_attr[0].rx_qid);\
	_OP(2,  32,	16,	uint16_t, attr->dpci_prio_attr[1].tx_qid);\
	_OP(2,  48,	16,	uint16_t, attr->dpci_prio_attr[1].rx_qid);\
	_OP(3,  0,	64,	uint64_t, attr->dpci_prio_attr[0].rx_user_ctx);\
	_OP(4,  0,	64,	uint64_t, attr->dpci_prio_attr[1].rx_user_ctx);\
	_OP(5,  0,	32,	uint32_t,		attr->version.major);\
	_OP(5,  32,	32,	uint32_t,		attr->version.minor);\
} while (0)

/*	param, offset, width,	type,		arg_name */
#define DPCI_CMD_SET_IRQ(_OP) \
do { \
	_OP(0,  0,	8,	uint8_t,	irq_index);\
	_OP(0,  32,	32,	uint32_t,	irq_val);\
	_OP(1,  0,	64,	uint64_t,	irq_paddr);\
	_OP(2,  0,	32,	int,		user_irq_id); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_GET_IRQ(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCI_RSP_GET_IRQ(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		irq_val); \
	_OP(1,	0,	64,	uint64_t,		irq_paddr);\
	_OP(2,  0,	32,	int,			user_irq_id); \
	_OP(2,	32,	32,	int,			type); \
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_SET_IRQ_ENABLE(_OP) \
do { \
	_OP(0,	0,	8,	uint8_t,		enable_state); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_GET_IRQ_ENABLE(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCI_RSP_GET_IRQ_ENABLE(_OP) \
	_OP(0,	0,	8,	uint8_t,		enable_state)

/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_SET_IRQ_MASK(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		mask); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_GET_IRQ_MASK(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCI_RSP_GET_IRQ_MASK(_OP) \
	_OP(0,	0,	32,	uint32_t,		mask)

/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_GET_IRQ_STATUS(_OP) \
	_OP(0,	32,	8,	uint8_t,		irq_index)

/*	param, offset, width,	type,			arg_name */
#define DPCI_RSP_GET_IRQ_STATUS(_OP) \
	_OP(0,	0,	32,	uint32_t,		status)

/*	param, offset, width,	type,			arg_name */
#define DPCI_CMD_CLEAR_IRQ_STATUS(_OP) \
do { \
	_OP(0,	0,	32,	uint32_t,		status); \
	_OP(0,	32,	8,	uint8_t,		irq_index);\
} while (0)

#endif /* _FSL_DPCI_CMD_H */
