/**************************************************************************//**
@File		checksum.h

@Description	This file contains checksum utilities

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __CHECKSUM_H
#define __CHECKSUM_H

/**************************************************************************//**
@Group	CKSUM Checksum (Internal)

@Description	AIOP Checksum utilities

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		CKSUM_Functions Checksum Functions

@Description	AIOP Checksum Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	cksum_ones_complement_sum16

@Description	Calculates a 1's complement sum of two 16 bit arguments.

@Param[in]	arg1 - first argument.

@Param[in]	arg2 - second argument.

@Return		1's complement sum of the two 16 bit arguments.

@Cautions	None
*//***************************************************************************/
inline uint16_t cksum_ones_complement_sum16(uint16_t arg1, uint16_t arg2)
{
	int32_t tmp = arg1 + arg2;
	return (uint16_t)(tmp + (tmp >> 16));
}



/**************************************************************************//**
@Function	cksum_update_uint32

@Description	Updates the IPv4/UDP/TCP CS after updating 4 consecutive
		bytes in the IPv4/UDP/TCP header. The update is being done
		directly to the workspace memory.

@Param[in]	cs_ptr - Pointer to the IPv4/UDP/TCP CheckSum.

@Param[in]	old_val - The original value of the 4 bytes changed in the
		header.

@Param[in]	new_val - The new value of the 4 bytes changed in the header.

@Return		None.

@Cautions	Replace of the segment is not performed in this SR. In order
		to update the frame in external memory there is a need to use
		FDMA SRs.
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


/**************************************************************************//**
@Function	cksum_accumulative_update_uint32

@Description	Updates the IPv4/UDP/TCP CS after updating 4 consecutive
		bytes in the IPv4/UDP/TCP header. The update is being done
		to the cksum parameter.

@Param[in]	cksum - The initial checksum value to be updated.

@Param[in]	old_val - The original value of the 4 bytes changed in the
		header.

@Param[in]	new_val - The new value of the 4 bytes changed in the header.

@Return		The updated cksum value.

@Cautions	None.
*//***************************************************************************/
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

/** @} */ /* end of CKSUM_Functions */

/** @} */ /* end of CKSUM */

#endif /* __CHECKSUM_H */
