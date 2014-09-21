/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
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
 @File          fsl_dpni_cmd.h

 @Description   defines dpni portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPNI_CMD_H
#define _FSL_DPNI_CMD_H

#define MC_CMD_EXTRACT_DATA_PARAMS		25
struct extract_data {
	uint64_t params[MC_CMD_EXTRACT_DATA_PARAMS];
};

/* DPNI Version */
#define DPNI_VER_MAJOR				1
#define DPNI_VER_MINOR				1

/* cmd IDs */
#define DPNI_CMDID_CLOSE			0x800
#define DPNI_CMDID_OPEN				0x801
#define DPNI_CMDID_CREATE			0x901
#define DPNI_CMDID_DESTROY			0x900

#define DPNI_CMDID_GET_L3_CHKSUM_VALIDATION	0x120
#define DPNI_CMDID_SET_L3_CHKSUM_VALIDATION	0x121
#define DPNI_CMDID_GET_L4_CHKSUM_VALIDATION	0x122
#define DPNI_CMDID_SET_L4_CHKSUM_VALIDATION	0x123
#define DPNI_CMDID_SET_POOLS			0x127

#define DPNI_CMDID_SET_TX_TC			0x129
#define DPNI_CMDID_SET_RX_TC			0x12A
#define DPNI_CMDID_SET_TX_FLOW			0x12B
#define DPNI_CMDID_GET_TX_FLOW			0x12C
#define DPNI_CMDID_RESET			0x12D
#define DPNI_CMDID_GET_ATTR			0x12E
#define DPNI_CMDID_GET_QDID			0x12F
#define DPNI_CMDID_ENABLE			0x130
#define DPNI_CMDID_DISABLE			0x131
#define DPNI_CMDID_GET_COUNTER			0x132
#define DPNI_CMDID_SET_COUNTER			0x133
#define DPNI_CMDID_GET_LINK_STATE		0x134
#define DPNI_CMDID_SET_MFL			0x135
#define DPNI_CMDID_SET_MTU			0x136
#define DPNI_CMDID_SET_MCAST_PROMISC		0x137
#define DPNI_CMDID_GET_MCAST_PROMISC		0x138
#define DPNI_CMDID_SET_UNICAST_PROMISC		0x167
#define DPNI_CMDID_GET_UNICAST_PROMISC		0x168
#define DPNI_CMDID_SET_PRIM_MAC			0x139
#define DPNI_CMDID_ADD_MAC_ADDR			0x13A
#define DPNI_CMDID_REMOVE_MAC_ADDR		0x13B
#define DPNI_CMDID_CLR_MAC_TBL			0x13C
#define DPNI_CMDID_ADD_VLAN_ID			0x13D
#define DPNI_CMDID_REMOVE_VLAN_ID		0x13E
#define DPNI_CMDID_CLR_VLAN_TBL			0x13F
#define DPNI_CMDID_SET_QOS_TBL			0x140
#define DPNI_CMDID_DELETE_QOS_TBL		0x141
#define DPNI_CMDID_ADD_QOS_ENT			0x142
#define DPNI_CMDID_REMOVE_QOS_ENT		0x143
#define DPNI_CMDID_CLR_QOS_TBL			0x144
#define DPNI_CMDID_ADD_FS_ENT			0x148
#define DPNI_CMDID_REMOVE_FS_ENT		0x149
#define DPNI_CMDID_CLR_FS_ENT			0x14A
#define DPNI_CMDID_SET_IRQ			0x14B

#define DPNI_CMDID_SET_VLAN_FILTERS		0x14E
#define DPNI_CMDID_SET_TX_CONF_BUFFER_LAYOUT	0x14F

#define DPNI_CMDID_GET_TX_DATA_OFFSET		0x150
#define DPNI_CMDID_GET_PRIM_MAC			0x151
#define DPNI_CMDID_GET_MFL			0x152
#define DPNI_CMDID_GET_MTU			0x153
#define DPNI_CMDID_GET_RX_BUFFER_LAYOUT		0x154
#define DPNI_CMDID_SET_RX_BUFFER_LAYOUT		0x155
#define DPNI_CMDID_GET_IRQ			0x156
#define DPNI_CMDID_SET_IRQ_ENABLE		0x157
#define DPNI_CMDID_GET_IRQ_ENABLE		0x158
#define DPNI_CMDID_SET_IRQ_MASK			0x159
#define DPNI_CMDID_GET_IRQ_MASK			0x15A
#define DPNI_CMDID_GET_IRQ_STATUS		0x15B
#define DPNI_CMDID_CLEAR_IRQ_STATUS		0x15C
#define DPNI_CMDID_GET_TX_BUFFER_LAYOUT		0x15D
#define DPNI_CMDID_SET_TX_BUFFER_LAYOUT		0x15E
#define DPNI_CMDID_GET_TX_CONF_BUFFER_LAYOUT	0x15F

#define DPNI_CMDID_GET_SPID			0x160
#define DPNI_CMDID_SET_RX_FLOW			0x161
#define DPNI_CMDID_GET_RX_FLOW			0x162

#define DPNI_CMDID_SET_IPR          	 	0x163
#define DPNI_CMDID_SET_IPF          	 	0x164
#define DPNI_CMDID_SET_VLAN_INSERTION      	0x165
#define DPNI_CMDID_SET_VLAN_REMOVAL         0x166
#define DPNI_CMDID_SET_RX_ERR_QUEUE 		0x167
#define DPNI_CMDID_GET_RX_ERR_QUEUE 		0x168
#define DPNI_CMDID_SET_TX_CONF_ERR_QUEUE 		0x169
#define DPNI_CMDID_GET_TX_CONF_ERR_QUEUE 		0x16a

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_OPEN(cmd, dpni_id) \
	MC_CMD_OP(cmd, 	 0,  	0,	32,	int,	dpni_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_CREATE(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,	8,  uint8_t,  cfg->adv.max_tcs); \
	MC_CMD_OP(cmd, 0, 8,	8,  uint8_t,  cfg->adv.max_senders); \
	MC_CMD_OP(cmd, 0, 16,	8,  uint8_t,  cfg->mac_addr[5]); \
	MC_CMD_OP(cmd, 0, 24,	8,  uint8_t,  cfg->mac_addr[4]); \
	MC_CMD_OP(cmd, 0, 32,	8,  uint8_t,  cfg->mac_addr[3]); \
	MC_CMD_OP(cmd, 0, 40,	8,  uint8_t,  cfg->mac_addr[2]); \
	MC_CMD_OP(cmd, 0, 48,	8,  uint8_t,  cfg->mac_addr[1]); \
	MC_CMD_OP(cmd, 0, 56,	8,  uint8_t,  cfg->mac_addr[0]); \
	MC_CMD_OP(cmd, 1, 0,	64, uint64_t, cfg->adv.options); \
	MC_CMD_OP(cmd, 2, 0,	8,  uint8_t,  cfg->adv.max_unicast_filters); \
	MC_CMD_OP(cmd, 2, 8,	8,  uint8_t,  cfg->adv.max_multicast_filters); \
	MC_CMD_OP(cmd, 2, 16,	8,  uint8_t,  cfg->adv.max_vlan_filters); \
	MC_CMD_OP(cmd, 2, 24,	8,  uint8_t,  cfg->adv.max_qos_entries); \
	MC_CMD_OP(cmd, 2, 32,	8,  uint8_t,  cfg->adv.max_qos_key_size); \
	MC_CMD_OP(cmd, 2, 48,	8,  uint8_t,  cfg->adv.max_dist_key_size); \
	MC_CMD_OP(cmd, 2, 56,	8,  enum net_prot, cfg->adv.start_hdr); \
	MC_CMD_OP(cmd, 3, 0,	8,  uint8_t,  cfg->adv.max_dist_per_tc[0]); \
	MC_CMD_OP(cmd, 3, 8,	8,  uint8_t,  cfg->adv.max_dist_per_tc[1]); \
	MC_CMD_OP(cmd, 3, 16,	8,  uint8_t,  cfg->adv.max_dist_per_tc[2]); \
	MC_CMD_OP(cmd, 3, 24,	8,  uint8_t,  cfg->adv.max_dist_per_tc[3]); \
	MC_CMD_OP(cmd, 3, 32,	8,  uint8_t,  cfg->adv.max_dist_per_tc[4]); \
	MC_CMD_OP(cmd, 3, 40,	8,  uint8_t,  cfg->adv.max_dist_per_tc[5]); \
	MC_CMD_OP(cmd, 3, 48,	8,  uint8_t,  cfg->adv.max_dist_per_tc[6]); \
	MC_CMD_OP(cmd, 3, 56,	8,  uint8_t,  cfg->adv.max_dist_per_tc[7]); \
	MC_CMD_OP(cmd, 4, 0,	16, uint16_t, \
	          	  	    cfg->adv.ipr_cfg.max_reass_frm_size); \
	MC_CMD_OP(cmd, 4, 16,	16, uint16_t, \
	          	  	    cfg->adv.ipr_cfg.min_frag_size_ipv4); \
	MC_CMD_OP(cmd, 4, 32,	16, uint16_t, \
	          	  	    cfg->adv.ipr_cfg.min_frag_size_ipv6); \
	MC_CMD_OP(cmd, 5, 0,	16, uint16_t, \
	          	  	  cfg->adv.ipr_cfg.max_open_frames_ipv4); \
	MC_CMD_OP(cmd, 5, 16,	16, uint16_t, \
	          	  	  cfg->adv.ipr_cfg.max_open_frames_ipv6); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_IRQ(cmd, irq_index, irq_paddr, irq_val, user_irq_id) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  irq_index); \
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, irq_val); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, irq_paddr); \
	MC_CMD_OP(cmd, 2, 0,  32, int,	     user_irq_id); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_ATTR(cmd, attr) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, int,	    attr->id);\
	MC_RSP_OP(cmd, 0, 32, 8,  uint8_t,  attr->max_tcs); \
	MC_RSP_OP(cmd, 0, 40, 8,  uint8_t,  attr->max_senders); \
	MC_RSP_OP(cmd, 0, 48, 8,  enum net_prot, attr->start_hdr); \
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t, attr->options); \
	MC_RSP_OP(cmd, 2, 0,  8,  uint8_t,  attr->max_unicast_filters); \
	MC_RSP_OP(cmd, 2, 8,  8,  uint8_t,  attr->max_multicast_filters);\
	MC_RSP_OP(cmd, 2, 16, 8,  uint8_t,  attr->max_vlan_filters); \
	MC_RSP_OP(cmd, 2, 24, 8,  uint8_t,  attr->max_qos_entries); \
	MC_RSP_OP(cmd, 2, 32, 8,  uint8_t,  attr->max_qos_key_size); \
	MC_RSP_OP(cmd, 2, 40, 8,  uint8_t,  attr->max_dist_key_size); \
	MC_RSP_OP(cmd, 3, 0,  8,  uint8_t,  attr->max_dist_per_tc[0]); \
	MC_RSP_OP(cmd, 3, 8,  8,  uint8_t,  attr->max_dist_per_tc[1]); \
	MC_RSP_OP(cmd, 3, 16, 8,  uint8_t,  attr->max_dist_per_tc[2]); \
	MC_RSP_OP(cmd, 3, 24, 8,  uint8_t,  attr->max_dist_per_tc[3]); \
	MC_RSP_OP(cmd, 3, 32, 8,  uint8_t,  attr->max_dist_per_tc[4]); \
	MC_RSP_OP(cmd, 3, 40, 8,  uint8_t,  attr->max_dist_per_tc[5]); \
	MC_RSP_OP(cmd, 3, 48, 8,  uint8_t,  attr->max_dist_per_tc[6]); \
	MC_RSP_OP(cmd, 3, 56, 8,  uint8_t,  attr->max_dist_per_tc[7]); \
	MC_RSP_OP(cmd, 4, 0,  32, uint32_t, attr->version.major);\
	MC_RSP_OP(cmd, 4, 32, 32, uint32_t, attr->version.minor);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_RX_BUFFER_LAYOUT(cmd, layout) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, layout->private_data_size); \
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, layout->data_align); \
	MC_RSP_OP(cmd, 0, 32, 32, uint32_t, layout->options); \
	MC_RSP_OP(cmd, 1, 0,  1,  int,	    layout->pass_timestamp); \
	MC_RSP_OP(cmd, 1, 1,  1,  int,	    layout->pass_parser_result); \
	MC_RSP_OP(cmd, 1, 2,  1,  int,	    layout->pass_frame_status); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_RX_BUFFER_LAYOUT(cmd, layout) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, layout->private_data_size); \
	MC_CMD_OP(cmd, 0, 16, 16, uint16_t, layout->data_align); \
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, layout->options); \
	MC_CMD_OP(cmd, 1, 0,  1,  int,	    layout->pass_timestamp); \
	MC_CMD_OP(cmd, 1, 1,  1,  int,	    layout->pass_parser_result); \
	MC_CMD_OP(cmd, 1, 2,  1,  int,	    layout->pass_frame_status); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_TX_BUFFER_LAYOUT(cmd, layout) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, layout->private_data_size); \
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, layout->data_align); \
	MC_RSP_OP(cmd, 0, 32, 32, uint32_t, layout->options); \
	MC_RSP_OP(cmd, 1, 0,  1,  int,      layout->pass_timestamp); \
	MC_RSP_OP(cmd, 1, 1,  1,  int,	    layout->pass_parser_result); \
	MC_RSP_OP(cmd, 1, 2,  1,  int,	    layout->pass_frame_status); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_TX_BUFFER_LAYOUT(cmd, layout) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, layout->private_data_size); \
	MC_CMD_OP(cmd, 0, 16, 16, uint16_t, layout->data_align); \
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, layout->options); \
	MC_CMD_OP(cmd, 1, 0,  1,  int,	    layout->pass_timestamp); \
	MC_CMD_OP(cmd, 1, 1,  1,  int,	    layout->pass_parser_result); \
	MC_CMD_OP(cmd, 1, 2,  1,  int,	    layout->pass_frame_status); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_TX_CONF_BUFFER_LAYOUT(cmd, layout) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, layout->private_data_size); \
	MC_RSP_OP(cmd, 0, 16, 16, uint16_t, layout->data_align); \
	MC_RSP_OP(cmd, 0, 32, 32, uint32_t, layout->options); \
	MC_RSP_OP(cmd, 1, 0,  1,  int,      layout->pass_timestamp); \
	MC_RSP_OP(cmd, 1, 1,  1,  int,	    layout->pass_parser_result); \
	MC_RSP_OP(cmd, 1, 2,  1,  int,	    layout->pass_frame_status); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_TX_CONF_BUFFER_LAYOUT(cmd, layout) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, layout->private_data_size); \
	MC_CMD_OP(cmd, 0, 16, 16, uint16_t, layout->data_align); \
	MC_CMD_OP(cmd, 0, 32, 32, uint32_t, layout->options); \
	MC_CMD_OP(cmd, 1, 0,  1,  int, 	    layout->pass_timestamp); \
	MC_CMD_OP(cmd, 1, 1,  1,  int,	    layout->pass_parser_result); \
	MC_CMD_OP(cmd, 1, 2,  1,  int,	    layout->pass_frame_status); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_QDID(cmd, qdid) \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, qdid)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_SPID(cmd, spid) \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, spid)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_TX_DATA_OFFSET(cmd, data_offset) \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, data_offset)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_COUNTER(cmd, counter) \
	MC_CMD_OP(cmd, 0, 0,  16, enum dpni_counter, counter)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_COUNTER(cmd, value) \
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t, value)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_LINK_STATE(cmd, up) \
	MC_RSP_OP(cmd, 0, 0,  1,  int,	    up)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_COUNTER(cmd, counter, value) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, enum dpni_counter, counter); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, value); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_MFL(cmd, mfl) \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, mfl)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_MFL(cmd, mfl) \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, mfl)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_MTU(cmd, mtu) \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, mtu)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_MTU(cmd, mtu) \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, mtu)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_MULTICAST_PROMISC(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,      en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_MULTICAST_PROMISC(cmd, en) \
	MC_RSP_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_UNICAST_PROMISC(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,      en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_UNICAST_PROMISC(cmd, en) \
	MC_RSP_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_PRIMARY_MAC_ADDR(cmd, addr) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  addr[5]); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  addr[4]); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  addr[3]); \
	MC_CMD_OP(cmd, 0, 40, 8,  uint8_t,  addr[2]); \
	MC_CMD_OP(cmd, 0, 48, 8,  uint8_t,  addr[1]); \
	MC_CMD_OP(cmd, 0, 56, 8,  uint8_t,  addr[0]); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_PRIMARY_MAC_ADDR(cmd, addr) \
do { \
	MC_RSP_OP(cmd, 0, 16, 8,  uint8_t,  addr[5]); \
	MC_RSP_OP(cmd, 0, 24, 8,  uint8_t,  addr[4]); \
	MC_RSP_OP(cmd, 0, 32, 8,  uint8_t,  addr[3]); \
	MC_RSP_OP(cmd, 0, 40, 8,  uint8_t,  addr[2]); \
	MC_RSP_OP(cmd, 0, 48, 8,  uint8_t,  addr[1]); \
	MC_RSP_OP(cmd, 0, 56, 8,  uint8_t,  addr[0]); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_ADD_MAC_ADDR(cmd, addr) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  addr[5]); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  addr[4]); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  addr[3]); \
	MC_CMD_OP(cmd, 0, 40, 8,  uint8_t,  addr[2]); \
	MC_CMD_OP(cmd, 0, 48, 8,  uint8_t,  addr[1]); \
	MC_CMD_OP(cmd, 0, 56, 8,  uint8_t,  addr[0]); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_REMOVE_MAC_ADDR(cmd, addr) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  addr[5]); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  addr[4]); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  addr[3]); \
	MC_CMD_OP(cmd, 0, 40, 8,  uint8_t,  addr[2]); \
	MC_CMD_OP(cmd, 0, 48, 8,  uint8_t,  addr[1]); \
	MC_CMD_OP(cmd, 0, 56, 8,  uint8_t,  addr[0]); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_ADD_VLAN_ID(cmd, vlan_id) \
	MC_CMD_OP(cmd, 0, 32, 16, uint16_t, vlan_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_REMOVE_VLAN_ID(cmd, vlan_id) \
	MC_CMD_OP(cmd, 0, 32, 16, uint16_t, vlan_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_POOLS(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  cfg->num_dpbp); \
	MC_CMD_OP(cmd, 0, 16, 16, uint16_t, cfg->pools[0].dpbp_id); \
	MC_CMD_OP(cmd, 0, 32, 16, uint16_t, cfg->pools[0].buffer_size);\
	MC_CMD_OP(cmd, 1, 0,  16, uint16_t, cfg->pools[1].dpbp_id); \
	MC_CMD_OP(cmd, 1, 16, 16, uint16_t, cfg->pools[1].buffer_size);\
	MC_CMD_OP(cmd, 1, 32, 16, uint16_t, cfg->pools[2].dpbp_id); \
	MC_CMD_OP(cmd, 1, 48, 16, uint16_t, cfg->pools[2].buffer_size);\
	MC_CMD_OP(cmd, 2, 0,  16, uint16_t, cfg->pools[3].dpbp_id); \
	MC_CMD_OP(cmd, 2, 16, 16, uint16_t, cfg->pools[3].buffer_size);\
	MC_CMD_OP(cmd, 2, 32, 16, uint16_t, cfg->pools[4].dpbp_id); \
	MC_CMD_OP(cmd, 2, 48, 16, uint16_t, cfg->pools[4].buffer_size);\
	MC_CMD_OP(cmd, 3, 0,  16, uint16_t, cfg->pools[5].dpbp_id); \
	MC_CMD_OP(cmd, 3, 16, 16, uint16_t, cfg->pools[5].buffer_size);\
	MC_CMD_OP(cmd, 3, 32, 16, uint16_t, cfg->pools[6].dpbp_id); \
	MC_CMD_OP(cmd, 3, 48, 16, uint16_t, cfg->pools[6].buffer_size);\
	MC_CMD_OP(cmd, 4, 0,  16, uint16_t, cfg->pools[7].dpbp_id); \
	MC_CMD_OP(cmd, 4, 16, 16, uint16_t, cfg->pools[7].buffer_size);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_TX_TC(cmd, tc_id, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->depth_limit); \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_RX_TC(cmd, tc_id, cfg, ext_paddr) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->dist_size); \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id); \
	MC_CMD_OP(cmd, 0, 24, 4,  enum dpni_dist_mode, cfg->dist_mode); \
	MC_CMD_OP(cmd, 0, 24, 4,  enum dpni_dist_mode, cfg->dist_mode); \
	MC_CMD_OP(cmd, 0, 28, 4,  enum dpni_fs_miss_action, cfg->fs_cfg.miss_action); \
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, cfg->fs_cfg.default_flow_id); \
	MC_CMD_OP(cmd, 6, 0,  64, uint64_t, ext_paddr); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_TX_FLOW(cmd, flow_id, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id);\
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  cfg->dest_cfg.priority);\
	MC_CMD_OP(cmd, 0, 24, 1,  int,	    cfg->only_error_frames);\
	MC_CMD_OP(cmd, 0, 25, 2,  enum dpni_dest, cfg->dest_cfg.type);\
	MC_CMD_OP(cmd, 0, 27, 1,  int,	    cfg->l3_chksum_gen);\
	MC_CMD_OP(cmd, 0, 28, 1,  int,	    cfg->l4_chksum_gen);\
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, flow_id);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx);\
	MC_CMD_OP(cmd, 2, 0,  32, uint32_t, cfg->options);\
	MC_CMD_OP(cmd, 2, 32, 32, int,	    cfg->tx_conf_err);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_SET_TX_FLOW(cmd, flow_id) \
	MC_RSP_OP(cmd, 0, 48, 16, uint16_t, flow_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_TX_FLOW(cmd, flow_id) \
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, flow_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_TX_FLOW(cmd, cfg, fqid) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id);\
	MC_RSP_OP(cmd, 0, 16, 8,  uint8_t,  cfg->dest_cfg.priority);\
	MC_RSP_OP(cmd, 0, 24, 1,  int,	    cfg->only_error_frames);\
	MC_RSP_OP(cmd, 0, 25, 2,  enum dpni_dest, cfg->dest_cfg.type);\
	MC_RSP_OP(cmd, 0, 27, 1,  int,	    cfg->l3_chksum_gen);\
	MC_RSP_OP(cmd, 0, 28, 1,  int,	    cfg->l4_chksum_gen);\
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx);\
	MC_RSP_OP(cmd, 2, 0,  32, uint32_t, cfg->options);\
	MC_RSP_OP(cmd, 2, 32, 32, int,	    cfg->tx_conf_err);\
	MC_RSP_OP(cmd, 3, 0,  32, uint32_t, fqid);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_RX_FLOW(cmd, tc_id, flow_id, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id); \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id); \
	MC_CMD_OP(cmd, 0, 24, 16, uint8_t,  cfg->dest_cfg.priority);\
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, flow_id); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx); \
	MC_CMD_OP(cmd, 2, 0,  32, uint32_t, cfg->options); \
	MC_CMD_OP(cmd, 2, 32, 2,  enum dpni_dest, cfg->dest_cfg.type);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_RX_FLOW(cmd, tc_id, flow_id) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id); \
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, flow_id); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_RX_FLOW(cmd, cfg, fqid) \
do { \
	MC_RSP_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id); \
	MC_RSP_OP(cmd, 0, 24, 16, uint8_t,  cfg->dest_cfg.priority);\
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx); \
	MC_RSP_OP(cmd, 2, 0,  32, uint32_t, cfg->options); \
	MC_RSP_OP(cmd, 2, 32, 2,  enum dpni_dest, cfg->dest_cfg.type); \
	MC_RSP_OP(cmd, 3, 0,  32, uint32_t, fqid); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_QOS_TABLE(cmd, cfg, ext_paddr) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, int,	    cfg->drop_frame); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  cfg->default_tc); \
	MC_CMD_OP(cmd, 6, 0,  64, uint64_t, ext_paddr); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_ADD_QOS_ENTRY(cmd, cfg, tc_id, key_paddr, mask_paddr) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->size); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, key_paddr); \
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, mask_paddr); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_REMOVE_QOS_ENTRY(cmd, cfg, key_paddr, mask_paddr) \
do { \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->size); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, key_paddr); \
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, mask_paddr); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_ADD_FS_ENTRY(cmd, tc_id, cfg, flow_id, key_paddr, \
                              	      	      	      	      mask_paddr) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id); \
	MC_CMD_OP(cmd, 0, 48, 16, uint16_t, flow_id); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->size); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, key_paddr); \
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, mask_paddr); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_REMOVE_FS_ENTRY(cmd, tc_id, cfg, key_paddr, mask_paddr) \
do { \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->size); \
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, key_paddr); \
	MC_CMD_OP(cmd, 2, 0,  64, uint64_t, mask_paddr); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_CLEAR_FS_ENTRIES(cmd, tc_id) \
	MC_CMD_OP(cmd, 0, 16, 8,  uint8_t,  tc_id)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_IRQ(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_IRQ(cmd, type, irq_paddr, irq_val, user_irq_id) \
do { \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t, irq_val); \
	MC_RSP_OP(cmd, 1, 0,  64, uint64_t, irq_paddr); \
	MC_RSP_OP(cmd, 2, 0,  32, int,      user_irq_id); \
	MC_RSP_OP(cmd, 2, 32, 32, int,	    type); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_IRQ_ENABLE(cmd, irq_index, enable_state) \
do { \
	MC_CMD_OP(cmd, 0, 0,  8,  uint8_t,  enable_state); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_IRQ_ENABLE(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_IRQ_ENABLE(cmd, enable_state) \
	MC_RSP_OP(cmd, 0, 0,  8,  uint8_t,  enable_state)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_IRQ_MASK(cmd, irq_index, mask) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, mask); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_IRQ_MASK(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_IRQ_MASK(cmd, mask) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t,  mask)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_GET_IRQ_STATUS(cmd, irq_index) \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_IRQ_STATUS(cmd, status) \
	MC_RSP_OP(cmd, 0, 0,  32, uint32_t,  status)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_CLEAR_IRQ_STATUS(cmd, irq_index, status) \
do { \
	MC_CMD_OP(cmd, 0, 0,  32, uint32_t, status); \
	MC_CMD_OP(cmd, 0, 32, 8,  uint8_t,  irq_index);\
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_VLAN_FILTERS(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_L3_CHKSUM_VALIDATION(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_L3_CHKSUM_VALIDATION(cmd, en) \
	MC_RSP_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_L4_CHKSUM_VALIDATION(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_L4_CHKSUM_VALIDATION(cmd, en) \
	MC_RSP_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_IPR(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_IPF(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_VLAN_INSERTION(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_VLAN_REMOVAL(cmd, en) \
	MC_CMD_OP(cmd, 0, 0,  1,  int,	    en)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_RX_ERR_QUEUE(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->dest_cfg.priority);\
	MC_CMD_OP(cmd, 0, 32, 2,  enum dpni_dest, cfg->dest_cfg.type);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx); \
	MC_CMD_OP(cmd, 2, 0,  32, uint32_t, cfg->options); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_RX_ERR_QUEUE(cmd, cfg, fqid) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->dest_cfg.priority);\
	MC_CMD_OP(cmd, 0, 32, 2,  enum dpni_dest, cfg->dest_cfg.type);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx); \
	MC_CMD_OP(cmd, 2, 0,  32, uint32_t, cfg->options); \
	MC_CMD_OP(cmd, 2, 32,  32, uint32_t, fqid); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_CMD_SET_TX_CONF_ERR_QUEUE(cmd, cfg) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->dest_cfg.priority);\
	MC_CMD_OP(cmd, 0, 32, 2,  enum dpni_dest, cfg->dest_cfg.type);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx); \
	MC_CMD_OP(cmd, 2, 0,  32, uint32_t, cfg->options); \
} while (0)

/*                cmd, param, offset, width, type, arg_name */
#define DPNI_RSP_GET_TX_CONF_ERR_QUEUE(cmd, cfg, fqid) \
do { \
	MC_CMD_OP(cmd, 0, 0,  16, uint16_t, cfg->dest_cfg.id); \
	MC_CMD_OP(cmd, 0, 24, 8,  uint8_t,  cfg->dest_cfg.priority);\
	MC_CMD_OP(cmd, 0, 32, 2,  enum dpni_dest, cfg->dest_cfg.type);\
	MC_CMD_OP(cmd, 1, 0,  64, uint64_t, cfg->user_ctx); \
	MC_CMD_OP(cmd, 2, 0,  32, uint32_t, cfg->options); \
	MC_CMD_OP(cmd, 2, 32,  32, uint32_t, fqid); \
} while (0)

#endif /* _FSL_DPNI_CMD_H */
