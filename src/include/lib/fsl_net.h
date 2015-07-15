/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

/**************************************************************************//**
 @File          fsl_net.h

 @Description   This file contains common and general netcomm headers definitions.
*//***************************************************************************/
#ifndef __FSL_NET_H
#define __FSL_NET_H

#include "common/types.h"


#define LAST_HDR_INDEX 0xFFFFFFFF


/*****************************************************************************/
/*                Protocol headers                                           */
/*****************************************************************************/

/**************************************************************************//**
@Description	MPLS structure.

*//***************************************************************************/
#pragma pack(push,1)
struct mplshdr {
	uint32_t label:20;		/**< Label value */
	uint32_t exp:3;			/**< Experimental use */
	uint32_t s:1;			/**< Bottom of stack */ 
	uint32_t ttl:8;			/**< Time to live */
};
#pragma pack(pop)


/**************************************************************************//**
@Description	VxLAN structure.

*//***************************************************************************/
#pragma pack(push,1)
struct vxlanhdr {
	uint8_t flags;			/**< Validation */
	uint8_t reserved_1[3];	/**< reserved 24 bits */
	uint8_t vnid[3];		/**< vxlan Network Identifier */ 
	uint8_t reserved_2;		/**< reserved 8 bits */
};
#pragma pack(pop)

/**************************************************************************//**
@Description	VLAN structure.

*//***************************************************************************/
#pragma pack(push,1)
struct vlanhdr {
	uint16_t tci;			/**< Priority, CFI and ID  */
	uint16_t ether_type;	/**< Length or Ethertype */
};
#pragma pack(pop)

/**************************************************************************//**
@Description	Ethernet structure.

*//***************************************************************************/
#pragma pack(push,1)
struct ethernethdr {
	uint8_t  dst_addr[6];		/**< MAC Destination Address */
	uint8_t  src_addr[6];		/**< MAC Source Address */
	uint16_t ether_type;		/**< Length or Ethertype */
};
#pragma pack(pop)


/**************************************************************************//**
@Description	IPv4 structure.

		Please refer to RFC 791 for more details.
*//***************************************************************************/
#pragma pack(push,1)
struct ipv4hdr {
	uint8_t  vsn_and_ihl;		/**< IP version and header length */
	uint8_t  tos;			/**< Type of service */
	uint16_t total_length;		/**< Total length */
	uint16_t id;			/**< Identificaiton */
	uint16_t flags_and_offset;	/**< IP Flags and Fragment Offset */
	uint8_t	 ttl;			/**< Time To Live */
	uint8_t	 protocol;		/**< Next Protocol */
	uint16_t hdr_cksum;		/**< Header Checksum */
	uint32_t src_addr;		/**< Source Address */
	uint32_t dst_addr;		/**< Destination Address */
};
#pragma pack(pop)

/**************************************************************************//**
@Description	IPv6 structure.

		Please refer to RFC 2460 for more details.
*//***************************************************************************/
#pragma pack(push,1)
struct ipv6hdr {
	uint32_t vsn_traffic_flow;	/**< IP ver,traffic class,flow label */
	uint16_t payload_length;
	uint8_t  next_header;
	uint8_t  hop_limit;
	uint32_t  src_addr[4];		/**< Source Address */
	uint32_t  dst_addr[4];		/**< Destination Address */
};
#pragma pack(pop)

/**************************************************************************//**
@Description	IPv6 Fragment Header.

		Please refer to RFC 2460 for more details.
*//***************************************************************************/
#pragma pack(push,1)
struct ipv6fraghdr {
	uint8_t  next_header;
	uint8_t  reserved;
	uint16_t offset_and_flags;
	uint32_t id;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	UDP structure.

		Please refer to RFC 768 for more details.
*//***************************************************************************/
#pragma pack(push,1)
struct udphdr {
	uint16_t src_port;		/*!< Source port */
	uint16_t dst_port;		/*!< Destination port */
	uint16_t length;		/*!< Length */
	uint16_t checksum;		/*!< Checksum */
};
#pragma pack(pop)

/**************************************************************************//**
@Description	TCP structure.

		Please refer to RFC 793 for more details.
*//***************************************************************************/
#pragma pack(push,1)
struct tcphdr {
	uint16_t src_port;		/*!< Source port */
	uint16_t dst_port;		/*!< Destination port */
	uint32_t sequence_number;	/*!< Sequence number */
	uint32_t acknowledgment_number;	/*!< Acknowledgment number */
	uint8_t  data_offset_reserved;	/*!< Data offset, reserved fields. */
	uint8_t  flags;			/*!< TCP control bits */
	uint16_t window_size;		/*!< Window size */
	uint16_t checksum;		/*!< Checksum */
	uint16_t urgent_pointer;	/*!< Urgent pointer */
};
#pragma pack(pop)

#pragma pack(push,1)
struct arphdr
{
	uint16_t	hw_type;
	uint16_t	pro_type;
	uint8_t		hw_addr_len;
	uint8_t		pro_addr_len;
	uint16_t	operation;
	uint8_t		src_hw_addr[6];
	uint32_t	src_pro_addr;
	uint8_t		dst_hw_addr[6];
	uint32_t	dst_pro_addr;
};
#pragma pack(pop)

/*****************************************************************************/
/*                Protocol fields                                            */
/*****************************************************************************/

typedef uint16_t net_hdr_fld_t;

/****************************  PPP fields  ***********************************/
typedef uint16_t net_hdr_fld_ppp_t;

#define NET_HDR_FLD_PPP_PID                        (1)
#define NET_HDR_FLD_PPP_COMPRESSED                 (NET_HDR_FLD_PPP_PID << 1)
#define NET_HDR_FLD_PPP_ALL_FIELDS                 ((NET_HDR_FLD_PPP_PID << 2) - 1)

/**************************  PPPoE fields  ***********************************/
typedef uint16_t net_hdr_fld_pppoe_t;

#define NET_HDR_FLD_PPPOE_VER                      (1)
#define NET_HDR_FLD_PPPOE_TYPE                     (NET_HDR_FLD_PPPOE_VER << 1)
#define NET_HDR_FLD_PPPOE_CODE                     (NET_HDR_FLD_PPPOE_VER << 2)
#define NET_HDR_FLD_PPPOE_SID                      (NET_HDR_FLD_PPPOE_VER << 3)
#define NET_HDR_FLD_PPPOE_LEN                      (NET_HDR_FLD_PPPOE_VER << 4)
#define NET_HDR_FLD_PPPOE_SESSION                  (NET_HDR_FLD_PPPOE_VER << 5)
#define NET_HDR_FLD_PPPOE_PID                      (NET_HDR_FLD_PPPOE_VER << 6)
#define NET_HDR_FLD_PPPOE_ALL_FIELDS               ((NET_HDR_FLD_PPPOE_VER << 7) - 1)

/*************************  PPP-Mux fields  ***********************************/
#define NET_HDR_FLD_PPPMUX_PID                     (1)
#define NET_HDR_FLD_PPPMUX_CKSUM                   (NET_HDR_FLD_PPPMUX_PID << 1)
#define NET_HDR_FLD_PPPMUX_COMPRESSED              (NET_HDR_FLD_PPPMUX_PID << 2)
#define NET_HDR_FLD_PPPMUX_ALL_FIELDS              ((NET_HDR_FLD_PPPMUX_PID << 3) - 1)

/***********************  PPP-Mux sub-frame fields  **************************/
#define NET_HDR_FLD_PPPMUX_SUBFRAME_PFF            (1)
#define NET_HDR_FLD_PPPMUX_SUBFRAME_LXT            (NET_HDR_FLD_PPPMUX_SUBFRAME_PFF << 1)
#define NET_HDR_FLD_PPPMUX_SUBFRAME_LEN            (NET_HDR_FLD_PPPMUX_SUBFRAME_PFF << 2)
#define NET_HDR_FLD_PPPMUX_SUBFRAME_PID            (NET_HDR_FLD_PPPMUX_SUBFRAME_PFF << 3)
#define NET_HDR_FLD_PPPMUX_SUBFRAME_USE_PID        (NET_HDR_FLD_PPPMUX_SUBFRAME_PFF << 4)
#define NET_HDR_FLD_PPPMUX_SUBFRAME_ALL_FIELDS     ((NET_HDR_FLD_PPPMUX_SUBFRAME_PFF << 5) - 1)

/*************************  Ethernet fields  *********************************/
typedef uint16_t net_hdr_fld_eth_t;

#define NET_HDR_FLD_ETH_DA                         (1)
#define NET_HDR_FLD_ETH_SA                         (NET_HDR_FLD_ETH_DA << 1)
#define NET_HDR_FLD_ETH_LENGTH                     (NET_HDR_FLD_ETH_DA << 2)
#define NET_HDR_FLD_ETH_TYPE                       (NET_HDR_FLD_ETH_DA << 3)
#define NET_HDR_FLD_ETH_FINAL_CKSUM                (NET_HDR_FLD_ETH_DA << 4)
#define NET_HDR_FLD_ETH_PADDING                    (NET_HDR_FLD_ETH_DA << 5)
#define NET_HDR_FLD_ETH_ALL_FIELDS                 ((NET_HDR_FLD_ETH_DA << 6) - 1)

#define NET_HDR_FLD_ETH_ADDR_SIZE                 6

/***************************  VLAN fields  ***********************************/
typedef uint16_t net_hdr_fld_vlan_t;

#define NET_HDR_FLD_VLAN_VPRI                      (1)
#define NET_HDR_FLD_VLAN_CFI                       (NET_HDR_FLD_VLAN_VPRI << 1)
#define NET_HDR_FLD_VLAN_VID                       (NET_HDR_FLD_VLAN_VPRI << 2)
#define NET_HDR_FLD_VLAN_LENGTH                    (NET_HDR_FLD_VLAN_VPRI << 3)
#define NET_HDR_FLD_VLAN_TYPE                      (NET_HDR_FLD_VLAN_VPRI << 4)
#define NET_HDR_FLD_VLAN_ALL_FIELDS                ((NET_HDR_FLD_VLAN_VPRI << 5) - 1)

#define NET_HDR_FLD_VLAN_TCI                       (NET_HDR_FLD_VLAN_VPRI | \
                                                    NET_HDR_FLD_VLAN_CFI | \
                                                    NET_HDR_FLD_VLAN_VID)

/************************  IP (generic) fields  ******************************/
typedef uint16_t net_hdr_fld_ip_t;

#define NET_HDR_FLD_IP_VER                         (1)
#define NET_HDR_FLD_IP_DSCP                        (NET_HDR_FLD_IP_VER << 2)
#define NET_HDR_FLD_IP_ECN                         (NET_HDR_FLD_IP_VER << 3)
#define NET_HDR_FLD_IP_PROTO                       (NET_HDR_FLD_IP_VER << 4)
#define NET_HDR_FLD_IP_SRC                         (NET_HDR_FLD_IP_VER << 5)
#define NET_HDR_FLD_IP_DST                         (NET_HDR_FLD_IP_VER << 6)
#define NET_HDR_FLD_IP_TOS_TC                      (NET_HDR_FLD_IP_VER << 7)
#define NET_HDR_FLD_IP_ID                          (NET_HDR_FLD_IP_VER << 8)
#define NET_HDR_FLD_IP_ALL_FIELDS                  ((NET_HDR_FLD_IPv4_VER << 9) - 1)

#define NET_HDR_FLD_IP_PROTO_SIZE                  1

/*****************************  IPv4 fields  *********************************/
typedef uint16_t net_hdr_fld_ipv4_t;

#define NET_HDR_FLD_IPv4_VER                       (1)
#define NET_HDR_FLD_IPv4_HDR_LEN                   (NET_HDR_FLD_IPv4_VER << 1)
#define NET_HDR_FLD_IPv4_TOS                       (NET_HDR_FLD_IPv4_VER << 2)
#define NET_HDR_FLD_IPv4_TOTAL_LEN                 (NET_HDR_FLD_IPv4_VER << 3)
#define NET_HDR_FLD_IPv4_ID                        (NET_HDR_FLD_IPv4_VER << 4)
#define NET_HDR_FLD_IPv4_FLAG_D                    (NET_HDR_FLD_IPv4_VER << 5)
#define NET_HDR_FLD_IPv4_FLAG_M                    (NET_HDR_FLD_IPv4_VER << 6)
#define NET_HDR_FLD_IPv4_OFFSET                    (NET_HDR_FLD_IPv4_VER << 7)
#define NET_HDR_FLD_IPv4_TTL                       (NET_HDR_FLD_IPv4_VER << 8)
#define NET_HDR_FLD_IPv4_PROTO                     (NET_HDR_FLD_IPv4_VER << 9)
#define NET_HDR_FLD_IPv4_CKSUM                     (NET_HDR_FLD_IPv4_VER << 10)
#define NET_HDR_FLD_IPv4_SRC_IP                    (NET_HDR_FLD_IPv4_VER << 11)
#define NET_HDR_FLD_IPv4_DST_IP                    (NET_HDR_FLD_IPv4_VER << 12)
#define NET_HDR_FLD_IPv4_OPTS                      (NET_HDR_FLD_IPv4_VER << 13)
#define NET_HDR_FLD_IPv4_OPTS_COUNT                (NET_HDR_FLD_IPv4_VER << 14)
#define NET_HDR_FLD_IPv4_ALL_FIELDS                ((NET_HDR_FLD_IPv4_VER << 15) - 1)

#define NET_HDR_FLD_IPv4_ADDR_SIZE                 4
#define NET_HDR_FLD_IPv4_PROTO_SIZE                1

/*****************************  IPv6 fields  *********************************/
typedef uint16_t net_hdr_fld_ipv6_t;

#define NET_HDR_FLD_IPv6_VER                       (1)
#define NET_HDR_FLD_IPv6_TC                        (NET_HDR_FLD_IPv6_VER << 1)
#define NET_HDR_FLD_IPv6_SRC_IP                    (NET_HDR_FLD_IPv6_VER << 2)
#define NET_HDR_FLD_IPv6_DST_IP                    (NET_HDR_FLD_IPv6_VER << 3)
#define NET_HDR_FLD_IPv6_NEXT_HDR                  (NET_HDR_FLD_IPv6_VER << 4)
#define NET_HDR_FLD_IPv6_FL                        (NET_HDR_FLD_IPv6_VER << 5)
#define NET_HDR_FLD_IPv6_HOP_LIMIT                 (NET_HDR_FLD_IPv6_VER << 6)
#define NET_HDR_FLD_IPv6_ID 			   (NET_HDR_FLD_IPv6_VER << 7)
#define NET_HDR_FLD_IPv6_ALL_FIELDS                ((NET_HDR_FLD_IPv6_VER << 8) - 1)

#define NET_HDR_FLD_IPv6_ADDR_SIZE                 16
#define NET_HDR_FLD_IPv6_NEXT_HDR_SIZE             1

/*****************************  ICMP fields  *********************************/
#define NET_HDR_FLD_ICMP_TYPE                      (1)
#define NET_HDR_FLD_ICMP_CODE                      (NET_HDR_FLD_ICMP_TYPE << 1)
#define NET_HDR_FLD_ICMP_CKSUM                     (NET_HDR_FLD_ICMP_TYPE << 2)
#define NET_HDR_FLD_ICMP_ID                        (NET_HDR_FLD_ICMP_TYPE << 3)
#define NET_HDR_FLD_ICMP_SQ_NUM                    (NET_HDR_FLD_ICMP_TYPE << 4)
#define NET_HDR_FLD_ICMP_ALL_FIELDS                ((NET_HDR_FLD_ICMP_TYPE << 5) - 1)

#define NET_HDR_FLD_ICMP_CODE_SIZE                 1
#define NET_HDR_FLD_ICMP_TYPE_SIZE                 1

/*****************************  IGMP fields  *********************************/
#define NET_HDR_FLD_IGMP_VERSION                   (1)
#define NET_HDR_FLD_IGMP_TYPE                      (NET_HDR_FLD_IGMP_VERSION << 1)
#define NET_HDR_FLD_IGMP_CKSUM                     (NET_HDR_FLD_IGMP_VERSION << 2)
#define NET_HDR_FLD_IGMP_DATA                      (NET_HDR_FLD_IGMP_VERSION << 3)
#define NET_HDR_FLD_IGMP_ALL_FIELDS                ((NET_HDR_FLD_IGMP_VERSION << 4) - 1)

/*****************************  TCP fields  **********************************/
typedef uint16_t net_hdr_fld_tcp_t;

#define NET_HDR_FLD_TCP_PORT_SRC                   (1)
#define NET_HDR_FLD_TCP_PORT_DST                   (NET_HDR_FLD_TCP_PORT_SRC << 1)
#define NET_HDR_FLD_TCP_SEQ                        (NET_HDR_FLD_TCP_PORT_SRC << 2)
#define NET_HDR_FLD_TCP_ACK                        (NET_HDR_FLD_TCP_PORT_SRC << 3)
#define NET_HDR_FLD_TCP_OFFSET                     (NET_HDR_FLD_TCP_PORT_SRC << 4)
#define NET_HDR_FLD_TCP_FLAGS                      (NET_HDR_FLD_TCP_PORT_SRC << 5)
#define NET_HDR_FLD_TCP_WINDOW                     (NET_HDR_FLD_TCP_PORT_SRC << 6)
#define NET_HDR_FLD_TCP_CKSUM                      (NET_HDR_FLD_TCP_PORT_SRC << 7)
#define NET_HDR_FLD_TCP_URGPTR                     (NET_HDR_FLD_TCP_PORT_SRC << 8)
#define NET_HDR_FLD_TCP_OPTS                       (NET_HDR_FLD_TCP_PORT_SRC << 9)
#define NET_HDR_FLD_TCP_OPTS_COUNT                 (NET_HDR_FLD_TCP_PORT_SRC << 10)
#define NET_HDR_FLD_TCP_ALL_FIELDS                 ((NET_HDR_FLD_TCP_PORT_SRC << 11) - 1)

#define NET_HDR_FLD_TCP_PORT_SIZE                  2

#define NET_HDR_FLD_TCP_FLAGS_FIN                  0x01
#define NET_HDR_FLD_TCP_FLAGS_SYN                  0x02
#define NET_HDR_FLD_TCP_FLAGS_RST                  0x04
#define NET_HDR_FLD_TCP_FLAGS_PSH                  0x08
#define NET_HDR_FLD_TCP_FLAGS_ACK                  0x10
#define NET_HDR_FLD_TCP_FLAGS_URG                  0x20
#define NET_HDR_FLD_TCP_FLAGS_ECE                  0x40
#define NET_HDR_FLD_TCP_FLAGS_CWR                  0x80

/* TCP data_offset field offset value */
#define NET_HDR_FLD_TCP_DATA_OFFSET_OFFSET	4
/* TCP data_offset field byte unit shift value (the data offset field specifies
 * the size of the TCP header in 32but words)*/
#define NET_HDR_FLD_TCP_DATA_OFFSET_SHIFT_VALUE	2
/* TCP data_offset field mask value */
#define NET_HDR_FLD_TCP_DATA_OFFSET_MASK        0xf0


/*****************************  UDP fields  *********************************/
typedef uint16_t net_hdr_fld_udp_t;

#define NET_HDR_FLD_UDP_PORT_SRC                   (1)
#define NET_HDR_FLD_UDP_PORT_DST                   (NET_HDR_FLD_UDP_PORT_SRC << 1)
#define NET_HDR_FLD_UDP_LEN                        (NET_HDR_FLD_UDP_PORT_SRC << 2)
#define NET_HDR_FLD_UDP_CKSUM                      (NET_HDR_FLD_UDP_PORT_SRC << 3)
#define NET_HDR_FLD_UDP_ALL_FIELDS                 ((NET_HDR_FLD_UDP_PORT_SRC << 4) - 1)

#define NET_HDR_FLD_UDP_PORT_SIZE                  2

/***************************  UDP-lite fields  *******************************/
typedef uint16_t net_hdr_fld_udp_lite_t;

#define NET_HDR_FLD_UDP_LITE_PORT_SRC              (1)
#define NET_HDR_FLD_UDP_LITE_PORT_DST              (NET_HDR_FLD_UDP_LITE_PORT_SRC << 1)
#define NET_HDR_FLD_UDP_LITE_ALL_FIELDS            ((NET_HDR_FLD_UDP_LITE_PORT_SRC << 2) - 1)

#define NET_HDR_FLD_UDP_LITE_PORT_SIZE             2

/***************************  UDP-encap-ESP fields  **************************/
typedef uint16_t net_hdr_fld_udp_encap_esp_t;

#define NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SRC         (1)
#define NET_HDR_FLD_UDP_ENCAP_ESP_PORT_DST         (NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SRC << 1)
#define NET_HDR_FLD_UDP_ENCAP_ESP_LEN              (NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SRC << 2)
#define NET_HDR_FLD_UDP_ENCAP_ESP_CKSUM            (NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SRC << 3)
#define NET_HDR_FLD_UDP_ENCAP_ESP_SPI              (NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SRC << 4)
#define NET_HDR_FLD_UDP_ENCAP_ESP_SEQUENCE_NUM     (NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SRC << 5)
#define NET_HDR_FLD_UDP_ENCAP_ESP_ALL_FIELDS       ((NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SRC << 6) - 1)

#define NET_HDR_FLD_UDP_ENCAP_ESP_PORT_SIZE        2
#define NET_HDR_FLD_UDP_ENCAP_ESP_SPI_SIZE         4

/*****************************  SCTP fields  *********************************/
typedef uint16_t net_hdr_fld_sctp_t;

#define NET_HDR_FLD_SCTP_PORT_SRC                  (1)
#define NET_HDR_FLD_SCTP_PORT_DST                  (NET_HDR_FLD_SCTP_PORT_SRC << 1)
#define NET_HDR_FLD_SCTP_VER_TAG                   (NET_HDR_FLD_SCTP_PORT_SRC << 2)
#define NET_HDR_FLD_SCTP_CKSUM                     (NET_HDR_FLD_SCTP_PORT_SRC << 3)
#define NET_HDR_FLD_SCTP_ALL_FIELDS                ((NET_HDR_FLD_SCTP_PORT_SRC << 4) - 1)

#define NET_HDR_FLD_SCTP_PORT_SIZE                 2

/*****************************  DCCP fields  *********************************/
typedef uint16_t net_hdr_fld_dccp_t;

#define NET_HDR_FLD_DCCP_PORT_SRC                  (1)
#define NET_HDR_FLD_DCCP_PORT_DST                  (NET_HDR_FLD_DCCP_PORT_SRC << 1)
#define NET_HDR_FLD_DCCP_ALL_FIELDS                ((NET_HDR_FLD_DCCP_PORT_SRC << 2) - 1)

#define NET_HDR_FLD_DCCP_PORT_SIZE                 2

/*****************************  IPHC fields  *********************************/
#define NET_HDR_FLD_IPHC_CID                       (1)
#define NET_HDR_FLD_IPHC_CID_TYPE                  (NET_HDR_FLD_IPHC_CID << 1)
#define NET_HDR_FLD_IPHC_HCINDEX                   (NET_HDR_FLD_IPHC_CID << 2)
#define NET_HDR_FLD_IPHC_GEN                       (NET_HDR_FLD_IPHC_CID << 3)
#define NET_HDR_FLD_IPHC_D_BIT                     (NET_HDR_FLD_IPHC_CID << 4)
#define NET_HDR_FLD_IPHC_ALL_FIELDS                ((NET_HDR_FLD_IPHC_CID << 5) - 1)

/*****************************  SCTP fields  *********************************/
#define NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE           (1)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_FLAGS          (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 1)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_LENGTH         (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 2)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_TSN            (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 3)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_STREAM_ID      (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 4)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_STREAM_SQN     (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 5)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_PAYLOAD_PID    (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 6)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_UNORDERED      (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 7)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_BEGGINING      (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 8)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_END            (NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 9)
#define NET_HDR_FLD_SCTP_CHUNK_DATA_ALL_FIELDS     ((NET_HDR_FLD_SCTP_CHUNK_DATA_TYPE << 10) - 1)

/***************************  L2TPv2 fields  *********************************/
#define NET_HDR_FLD_L2TPv2_TYPE_BIT                (1)
#define NET_HDR_FLD_L2TPv2_LENGTH_BIT              (NET_HDR_FLD_L2TPv2_TYPE_BIT << 1)
#define NET_HDR_FLD_L2TPv2_SEQUENCE_BIT            (NET_HDR_FLD_L2TPv2_TYPE_BIT << 2)
#define NET_HDR_FLD_L2TPv2_OFFSET_BIT              (NET_HDR_FLD_L2TPv2_TYPE_BIT << 3)
#define NET_HDR_FLD_L2TPv2_PRIORITY_BIT            (NET_HDR_FLD_L2TPv2_TYPE_BIT << 4)
#define NET_HDR_FLD_L2TPv2_VERSION                 (NET_HDR_FLD_L2TPv2_TYPE_BIT << 5)
#define NET_HDR_FLD_L2TPv2_LEN                     (NET_HDR_FLD_L2TPv2_TYPE_BIT << 6)
#define NET_HDR_FLD_L2TPv2_TUNNEL_ID               (NET_HDR_FLD_L2TPv2_TYPE_BIT << 7)
#define NET_HDR_FLD_L2TPv2_SESSION_ID              (NET_HDR_FLD_L2TPv2_TYPE_BIT << 8)
#define NET_HDR_FLD_L2TPv2_NS                      (NET_HDR_FLD_L2TPv2_TYPE_BIT << 9)
#define NET_HDR_FLD_L2TPv2_NR                      (NET_HDR_FLD_L2TPv2_TYPE_BIT << 10)
#define NET_HDR_FLD_L2TPv2_OFFSET_SIZE             (NET_HDR_FLD_L2TPv2_TYPE_BIT << 11)
#define NET_HDR_FLD_L2TPv2_FIRST_BYTE              (NET_HDR_FLD_L2TPv2_TYPE_BIT << 12)
#define NET_HDR_FLD_L2TPv2_ALL_FIELDS              ((NET_HDR_FLD_L2TPv2_TYPE_BIT << 13) - 1)

/***************************  L2TPv3 fields  *********************************/
#define NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT           (1)
#define NET_HDR_FLD_L2TPv3_CTRL_LENGTH_BIT         (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 1)
#define NET_HDR_FLD_L2TPv3_CTRL_SEQUENCE_BIT       (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 2)
#define NET_HDR_FLD_L2TPv3_CTRL_VERSION            (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 3)
#define NET_HDR_FLD_L2TPv3_CTRL_LENGTH             (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 4)
#define NET_HDR_FLD_L2TPv3_CTRL_CONTROL            (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 5)
#define NET_HDR_FLD_L2TPv3_CTRL_SENT               (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 6)
#define NET_HDR_FLD_L2TPv3_CTRL_RECV               (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 7)
#define NET_HDR_FLD_L2TPv3_CTRL_FIRST_BYTE         (NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 8)
#define NET_HDR_FLD_L2TPv3_CTRL_ALL_FIELDS         ((NET_HDR_FLD_L2TPv3_CTRL_TYPE_BIT << 9) - 1)

#define NET_HDR_FLD_L2TPv3_SESS_TYPE_BIT           (1)
#define NET_HDR_FLD_L2TPv3_SESS_VERSION            (NET_HDR_FLD_L2TPv3_SESS_TYPE_BIT << 1)
#define NET_HDR_FLD_L2TPv3_SESS_ID                 (NET_HDR_FLD_L2TPv3_SESS_TYPE_BIT << 2)
#define NET_HDR_FLD_L2TPv3_SESS_COOKIE             (NET_HDR_FLD_L2TPv3_SESS_TYPE_BIT << 3)
#define NET_HDR_FLD_L2TPv3_SESS_ALL_FIELDS         ((NET_HDR_FLD_L2TPv3_SESS_TYPE_BIT << 4) - 1)

typedef uint16_t net_hdr_fld_llc_t;

#define NET_HDR_FLD_LLC_DSAP                       (1)
#define NET_HDR_FLD_LLC_SSAP                       (NET_HDR_FLD_LLC_DSAP << 1)
#define NET_HDR_FLD_LLC_CTRL                       (NET_HDR_FLD_LLC_DSAP << 2)
#define NET_HDR_FLD_LLC_ALL_FIELDS                 ((NET_HDR_FLD_LLC_DSAP << 3) - 1)

#define NET_HDR_FLD_NLPID_NLPID                    (1)
#define NET_HDR_FLD_NLPID_ALL_FIELDS               ((NET_HDR_FLD_NLPID_NLPID << 1) - 1)

typedef uint16_t net_hdr_fld_snap_t;

#define NET_HDR_FLD_SNAP_OUI                       (1)
#define NET_HDR_FLD_SNAP_PID                       (NET_HDR_FLD_SNAP_OUI << 1)
#define NET_HDR_FLD_SNAP_ALL_FIELDS                ((NET_HDR_FLD_SNAP_OUI << 2) - 1)

typedef uint16_t net_hdr_fld_llc_snap_t;

#define NET_HDR_FLD_LLC_SNAP_TYPE                  (1)
#define NET_HDR_FLD_LLC_SNAP_ALL_FIELDS            ((NET_HDR_FLD_LLC_SNAP_TYPE << 1) - 1)

#define NET_HDR_FLD_ARP_HTYPE                      (1)
#define NET_HDR_FLD_ARP_PTYPE                      (NET_HDR_FLD_ARP_HTYPE << 1)
#define NET_HDR_FLD_ARP_HLEN                       (NET_HDR_FLD_ARP_HTYPE << 2)
#define NET_HDR_FLD_ARP_PLEN                       (NET_HDR_FLD_ARP_HTYPE << 3)
#define NET_HDR_FLD_ARP_OPER                       (NET_HDR_FLD_ARP_HTYPE << 4)
#define NET_HDR_FLD_ARP_SHA                        (NET_HDR_FLD_ARP_HTYPE << 5)
#define NET_HDR_FLD_ARP_SPA                        (NET_HDR_FLD_ARP_HTYPE << 6)
#define NET_HDR_FLD_ARP_THA                        (NET_HDR_FLD_ARP_HTYPE << 7)
#define NET_HDR_FLD_ARP_TPA                        (NET_HDR_FLD_ARP_HTYPE << 8)
#define NET_HDR_FLD_ARP_ALL_FIELDS                 ((NET_HDR_FLD_ARP_HTYPE << 9) - 1)

#define NET_HDR_FLD_RFC2684_LLC                    (1)
#define NET_HDR_FLD_RFC2684_NLPID                  (NET_HDR_FLD_RFC2684_LLC << 1)
#define NET_HDR_FLD_RFC2684_OUI                    (NET_HDR_FLD_RFC2684_LLC << 2)
#define NET_HDR_FLD_RFC2684_PID                    (NET_HDR_FLD_RFC2684_LLC << 3)
#define NET_HDR_FLD_RFC2684_VPN_OUI                (NET_HDR_FLD_RFC2684_LLC << 4)
#define NET_HDR_FLD_RFC2684_VPN_IDX                (NET_HDR_FLD_RFC2684_LLC << 5)
#define NET_HDR_FLD_RFC2684_ALL_FIELDS             ((NET_HDR_FLD_RFC2684_LLC << 6) - 1)

#define NET_HDR_FLD_USER_DEFINED_SRCPORT           (1)
#define NET_HDR_FLD_USER_DEFINED_PCDID             (NET_HDR_FLD_USER_DEFINED_SRCPORT << 1)
#define NET_HDR_FLD_USER_DEFINED_ALL_FIELDS        ((NET_HDR_FLD_USER_DEFINED_SRCPORT << 2) - 1)

#define NET_HDR_FLD_PAYLOAD_BUFFER                 (1)
#define NET_HDR_FLD_PAYLOAD_SIZE                   (NET_HDR_FLD_PAYLOAD_BUFFER << 1)
#define NET_HDR_FLD_MAX_FRM_SIZE                   (NET_HDR_FLD_PAYLOAD_BUFFER << 2)
#define NET_HDR_FLD_MIN_FRM_SIZE                   (NET_HDR_FLD_PAYLOAD_BUFFER << 3)
#define NET_HDR_FLD_PAYLOAD_TYPE                   (NET_HDR_FLD_PAYLOAD_BUFFER << 4)
#define NET_HDR_FLD_FRAME_SIZE                     (NET_HDR_FLD_PAYLOAD_BUFFER << 5)
#define NET_HDR_FLD_PAYLOAD_ALL_FIELDS             ((NET_HDR_FLD_PAYLOAD_BUFFER << 6) - 1)

typedef uint16_t net_hdr_fld_gre_t;

#define NET_HDR_FLD_GRE_TYPE                       (1)
#define NET_HDR_FLD_GRE_ALL_FIELDS                 ((NET_HDR_FLD_GRE_TYPE << 1) - 1)

typedef uint16_t net_hdr_fld_minencap_t;

#define NET_HDR_FLD_MINENCAP_SRC_IP                (1)
#define NET_HDR_FLD_MINENCAP_DST_IP                (NET_HDR_FLD_MINENCAP_SRC_IP << 1)
#define NET_HDR_FLD_MINENCAP_TYPE                  (NET_HDR_FLD_MINENCAP_SRC_IP << 2)
#define NET_HDR_FLD_MINENCAP_ALL_FIELDS            ((NET_HDR_FLD_MINENCAP_SRC_IP << 3) - 1)

typedef uint16_t net_hdr_fld_ipsec_ah_t;

#define NET_HDR_FLD_IPSEC_AH_SPI                   (1)
#define NET_HDR_FLD_IPSEC_AH_NH                    (NET_HDR_FLD_IPSEC_AH_SPI << 1)
#define NET_HDR_FLD_IPSEC_AH_ALL_FIELDS            ((NET_HDR_FLD_IPSEC_AH_SPI << 2) - 1)

typedef uint16_t net_hdr_fld_ipsec_esp_t;

#define NET_HDR_FLD_IPSEC_ESP_SPI                  (1)
#define NET_HDR_FLD_IPSEC_ESP_SEQUENCE_NUM         (NET_HDR_FLD_IPSEC_ESP_SPI << 1)
#define NET_HDR_FLD_IPSEC_ESP_ALL_FIELDS           ((NET_HDR_FLD_IPSEC_ESP_SPI << 2) - 1)

#define NET_HDR_FLD_IPSEC_ESP_SPI_SIZE             4

typedef uint16_t net_hdr_fld_mpls_t;

#define NET_HDR_FLD_MPLS_LABEL_STACK               (1)
#define NET_HDR_FLD_MPLS_LABEL_STACK_ALL_FIELDS    ((NET_HDR_FLD_MPLS_LABEL_STACK << 1) - 1)

typedef uint16_t net_hdr_fld_macsec_t;

#define NET_HDR_FLD_MACSEC_SECTAG                  (1)
#define NET_HDR_FLD_MACSEC_ALL_FIELDS              ((NET_HDR_FLD_MACSEC_SECTAG << 1) - 1)

typedef uint16_t net_hdr_fld_gtp_t;

#define NET_HDR_FLD_GTP_TEID                       (1)

/*****************************************************************************/
/*                Protocol optnios                                           */
/*****************************************************************************/

typedef uint8_t net_hdr_option_t;

/* Ethernet options */
#define	NET_HDR_OPT_ETH_BROADCAST			1
#define	NET_HDR_OPT_ETH_MULTICAST			2
#define	NET_HDR_OPT_ETH_UNICAST				3
#define	NET_HDR_OPT_ETH_BPDU				4

/* VLAN options */
#define	NET_HDR_OPT_VLAN_CFI				1

/* IPv4 options */
#define	NET_HDR_OPT_IPV4_UNICAST            		1
#define	NET_HDR_OPT_IPV4_MULTICAST         		2
#define	NET_HDR_OPT_IPV4_BROADCAST			3
#define	NET_HDR_OPT_IPV4_OPTION				4
#define	NET_HDR_OPT_IPV4_FRAG				5
#define	NET_HDR_OPT_IPV4_INITIAL_FRAG			6

/* IPv6 options */
#define	NET_HDR_OPT_IPV6_UNICAST            		1
#define	NET_HDR_OPT_IPV6_MULTICAST         		2
#define	NET_HDR_OPT_IPV6_OPTION				3
#define	NET_HDR_OPT_IPV6_FRAG				4
#define	NET_HDR_OPT_IPV6_INITIAL_FRAG			5

/* General IP options (may be used for any version) */
#define	NET_HDR_OPT_IP_FRAG				1
#define	NET_HDR_OPT_IP_INITIAL_FRAG			2
#define	NET_HDR_OPT_IP_OPTION				3

/* Minenc. options */
#define	NET_HDR_OPT_MINENCAP_SRC_ADDR_PRESENT		1

/* GRE. options */
#define	NET_HDR_OPT_GRE_ROUTING_PRESENT			1

/* TCP options */
#define	NET_HDR_OPT_TCP_OPTIONS				1
#define	NET_HDR_OPT_TCP_CONTROL_HIGH_BITS		2
#define	NET_HDR_OPT_TCP_CONTROL_LOW_BITS		3

/* CAPWAP options */
#define	NET_HDR_OPT_CAPWAP_DTLS				1

enum net_prot {
	NET_PROT_NONE = 0,
	NET_PROT_PAYLOAD,
	NET_PROT_ETH,
	NET_PROT_VLAN,
	NET_PROT_IPv4,
	NET_PROT_IPv6,
	NET_PROT_IP,
	NET_PROT_TCP,
	NET_PROT_UDP,
	NET_PROT_UDP_LITE,
	NET_PROT_IPHC,
	NET_PROT_SCTP,
	NET_PROT_SCTP_CHUNK_DATA,
	NET_PROT_PPPOE,
	NET_PROT_PPP,
	NET_PROT_PPPMUX,
	NET_PROT_PPPMUX_SUBFRAME,
	NET_PROT_L2TPv2,
	NET_PROT_L2TPv3_CTRL,
	NET_PROT_L2TPv3_SESS,
	NET_PROT_LLC,
	NET_PROT_LLC_SNAP,
	NET_PROT_NLPID,
	NET_PROT_SNAP,
	NET_PROT_MPLS,
	NET_PROT_IPSEC_AH,
	NET_PROT_IPSEC_ESP,
	NET_PROT_UDP_IPSEC_ESP,
	NET_PROT_MACSEC,
	NET_PROT_GRE,
	NET_PROT_MINENCAP,
	NET_PROT_DCCP,
	NET_PROT_ICMP,
	NET_PROT_IGMP,
	NET_PROT_ARP,
	NET_PROT_CAPWAP_DATA,
	NET_PROT_CAPWAP_CTRL,
	NET_PROT_RFC2684,
	NET_PROT_ICMPV6,
	NET_PROT_FCOE,
	NET_PROT_FIP,
	NET_PROT_ISCSI,
	NET_PROT_GTP,
	NET_PROT_USER_DEFINED_L2,
	NET_PROT_USER_DEFINED_L3,
	NET_PROT_USER_DEFINED_L4,
	NET_PROT_USER_DEFINED_L5,
	NET_PROT_USER_DEFINED_SHIM1,
	NET_PROT_USER_DEFINED_SHIM2,
	NET_PROT_USER_DEFINED_SHIM3,
	NET_PROT_USER_DEFINED_SHIM4,
	NET_PROT_USER_DEFINED_SHIM5,
	NET_PROT_USER_DEFINED_SHIM6,
	NET_PROT_USER_DEFINED_SHIM7,
	NET_PROT_USER_DEFINED_SHIM8,
	NET_PROT_DUMMY_LAST
};

/*****************************************************************************/
/*                Next Protocol                                              */
/*****************************************************************************/
/* IPv4 next protocol */
#define IPV4_PROTOCOL_ID	0x04
#define IPV6_PROTOCOL_ID	0x29

/* Ethernet next protocol */
#define ETYPE_IPV4		0x0800
#define ETYPE_IPV6		0x86DD

/* MPLS next protocol */
#define MPLS_LABEL_IPV4		0x00000000
#define MPLS_LABEL_IPV6		0x00002000

/* ARP next protocol */
#define ARPHDR_ETHER_PRO_TYPE	1
#define ARPHDR_IPV4_PRO_TYPE	0x800
#define ARP_ETHERTYPE			0x0806

#endif /* __FSL_NET_H */
