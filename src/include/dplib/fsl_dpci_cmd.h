/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//*
 @File          fsl_dpci_cmd.h

 @Description   defines dprc portal commandsF

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPCI_CMD_H
#define _FSL_DPCI_CMD_H

/* DPCI Version */
#define DPCI_VER_MAJOR				2
#define DPCI_VER_MINOR				0

/* cmd IDs */
#define MC_DPCI_CMDID_CLOSE				0x800
#define MC_DPCI_CMDID_OPEN				0x807
#define MC_DPCI_CMDID_CREATE				0x907
#define MC_DPCI_CMDID_DESTROY				0x900

#define DPCI_CMDID_ENABLE				0x002
#define DPCI_CMDID_DISABLE				0x003
#define DPCI_CMDID_GET_ATTR				0x004
#define DPCI_CMDID_RESET				0x005

#define DPCI_CMDID_SET_IRQ				0x010
#define DPCI_CMDID_GET_IRQ				0x011
#define DPCI_CMDID_SET_IRQ_ENABLE			0x012
#define DPCI_CMDID_GET_IRQ_ENABLE			0x013
#define DPCI_CMDID_SET_IRQ_MASK				0x014
#define DPCI_CMDID_GET_IRQ_MASK				0x015
#define DPCI_CMDID_GET_IRQ_STATUS			0x016
#define DPCI_CMDID_CLEAR_IRQ_STATUS			0x017

#define DPCI_CMDID_SET_RX_QUEUE				0x0e0
#define DPCI_CMDID_GET_LINK_STATE			0x0e1
/* cmd sizes */
#define MC_CMD_OPEN_SIZE				8
#define MC_CMD_CLOSE_SIZE				0
#define DPCI_CMDSZ_CREATE				8
#define DPCI_CMDSZ_DESTROY				0
#define DPCI_CMDSZ_SET_RX_QUEUE				(8 * 2)
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
	_OP(0,  0,	8,	uint8_t,	cfg->num_of_priorities);\
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

/*	param, offset, width,	type,			arg_name */
#define DPCI_RSP_GET_LINK_STATE(_OP) \
	_OP(0,	0,	1,	int,			up)

/*	param, offset, width,	type,		arg_name */
#define DPCI_RSP_GET_ATTR(_OP) \
do { \
	_OP(0,  0,	32,	int,		attr->id);\
	_OP(0,  32,	8,	uint8_t,	attr->peer_attached);\
	_OP(0,  40,	8,	uint8_t,	attr->peer_id);\
	_OP(1,  0,	8,	uint8_t,	attr->num_of_priorities);\
	_OP(2,  0,	32,	uint32_t, attr->dpci_prio_attr[0].tx_qid);\
	_OP(2,  32,	32,	uint32_t, attr->dpci_prio_attr[0].rx_qid);\
	_OP(3,  0,	32,	uint32_t, attr->dpci_prio_attr[1].tx_qid);\
	_OP(3,  32,	32,	uint32_t, attr->dpci_prio_attr[1].rx_qid);\
	_OP(4,  0,	64,	uint64_t, attr->dpci_prio_attr[0].rx_user_ctx);\
	_OP(5,  0,	64,	uint64_t, attr->dpci_prio_attr[1].rx_user_ctx);\
	_OP(6,  0,	32,	uint32_t,		attr->version.major);\
	_OP(6,  32,	32,	uint32_t,		attr->version.minor);\
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
