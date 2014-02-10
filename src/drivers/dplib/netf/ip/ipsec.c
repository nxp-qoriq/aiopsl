/**************************************************************************//**
@File		ipsec.c

@Description	This file contains the AIOP IPSec implementation.
		
*//***************************************************************************/

#include "common/types.h"
#include "dplib/fsl_ipsec.h"
#include "ipsec.h"

/**************************************************************************//**
@Function	ipsec_init

*//****************************************************************************/
int32_t ipsec_init(uint16_t max_sa_no, uint8_t asa_copy) {
	// TODO
	uint16_t tmp1;
	uint8_t tmp2;

	tmp1 = max_sa_no;
	tmp2 = asa_copy;
	
	return 0;
}

/**************************************************************************//**

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */


