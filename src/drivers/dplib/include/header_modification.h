/**************************************************************************//**
@File		header_modification.h

@Description	This file contains the AIOP Header Modification internal API

		Copyright 2013 Freescale Semiconductor, Inc.

*//***************************************************************************/

#ifndef __HEADER_MODIFICATION_H
#define __HEADER_MODIFICATION_H

#define VLAN_VID_MASK		0x00000FFF
#define VLAN_PCP_MASK		0x0000E000
#define VLAN_PCP_SHIFT		13
#define NO_OFFSET_FOUND		0xFF
#define IPV6_ECN_MASK		0xFFCFFFFF
#define IPV4_ECN_MASK		0xFC
#define MIN_REPRESENT_SIZE	128
#define TCP_DATA_OFFSET_SHIFT	2
#define TCP_NO_OPTION_SIZE	20
#define IPV6_ADDR_SIZE		32
#define IPV4_PROTOCOL_ID	0x04
#define IPV6_PROTOCOL_ID	0x29


#define IPV4_DSCP_MASK		0x03
#define IPV4_DF_MASK		0xbfff
#define IPV6_DSCP_MASK		0xF03FFFFF
#define ETYPE_IPV4		0x0800
#define ETYPE_IPV6		0x86DD
#define MPLS_LABEL_MASK		0x00001fff
#define MPLS_LABEL_IPV4		0x00000000
#define MPLS_LABEL_IPV6		0x00002000
#define IPV6_TC_MASK		0xf00fffff
#define IPV6_FLOW_MASK		0xfff00000


#define PARSER_CLEAR_RUNNING_SUM() \
(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->gross_running_sum) = 0


#endif /* __HEADER_MODIFICATION_H */
