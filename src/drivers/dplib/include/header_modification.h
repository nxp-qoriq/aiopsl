/**************************************************************************//**
@File		header_modification.h

@Description	This file contains the AIOP Header Modification internal API

		Copyright 2013 Freescale Semiconductor, Inc.

*//***************************************************************************/

#ifndef __HEADER_MODIFICATION_H
#define __HEADER_MODIFICATION_H

#define VLAN_VID_MASK		0x00000FFF
#define VLAN_PCP_MASK		0x0000E000
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

/** Returns a non-zero value in case TCP or UDP are found */
#define PARSER_IS_L4_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & (PARSER_ATT_UDP_MASK | PARSER_ATT_TCP_MASK))
/** Returns a non-zero value in case Inner IP is found (IPv4 or IPv6)
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_TUNNEL_IP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & (PARSER_ATT_IPV4_N_MASK |\
	PARSER_ATT_IPV6_N_MASK))


#define PARSER_CLEAR_RUNNING_SUM() \
(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->gross_running_sum) = 0


/**************************************************************************//**
@Function	cksum_update_uint32

@Description	Update the IPv4/UDP/TCP CS aafter updating 4 consecutive
		bytes in the IP header.

@Param[in]	cs_ptr - Pointer to the IPv4 CheckSum.

@Param[in]	old_val - first argument.

@Param[in]	new_val - second argument.

@Return		None.

@Cautions	The
*//***************************************************************************/
inline void cksum_update_uint32(register uint16_t *cs_ptr,
		register uint32_t old_val,
		register uint32_t new_val)
{
	register temp1;
	register temp2;
	asm{
		se_lhz	temp1, 0(cs_ptr)	/* Load CS */
		nor	new_val, new_val, new_val/* One's complement of the new
						value. Pipeline optimization */
		addc	temp1, temp1, old_val	/* Adding old value to CS and
						generating carry */
		adde	temp1, new_val, temp1	/* Adding new value and carry of
						the previous addition to CS */
		addze	temp1, temp1		/* Add carry of the previous
						addition to CS */
		e_rlwinm temp2, temp1, 16, 0, 31/* Replace two half words
						location */
		se_add	temp2, temp1		/* Adding two half words and
						first half word carry */
		se_srwi	temp2, 16		/* Isolate only the high 2B of
						the previous addition */
		se_sth	temp2, 0(cs_ptr)	/* Store CS */
	}
}



inline uint16_t cksum_accumulative_update_uint32(register uint16_t cksum,
		register uint32_t old_val,
		register uint32_t new_val)
{
	register temp2;
	asm{
		nor	new_val, new_val, new_val/* One's complement of the new
						value. Pipeline optimization */
		addc	cksum, cksum, old_val	/* Adding old value to CS and
						generating carry */
		adde	cksum, new_val, cksum	/* Adding new value and carry of
						the previous addition to CS */
		addze	cksum, cksum		/* Add carry of the previous
						addition to CS */
		e_rlwinm  temp2, cksum, 16, 0, 31/* Replace two half words
						location */
		se_add	temp2, cksum		/* Adding two half words and
						first half word carry */
		se_srwi	temp2, 16		/* Isolate only the high 2B of
						the previous addition */
	}
	return (uint16_t) temp2;
}


#endif /* __HEADER_MODIFICATION_H */
