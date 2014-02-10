/**************************************************************************//**
@File		aiop_verification_data.h

@Description	This file contains the AIOP SW Verification Data
		needed for the test

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_DATA_H_
#define __AIOP_VERIFICATION_DATA_H_

#define __VERIF_GLOBAL __declspec(section ".verif_data")
#define __VERIF_TLS __declspec(section ".verif_tdata")

#pragma section RW ".verif_data" ".verif_bss"
#pragma section RW ".verif_tdata" ".verif_tbss"



#endif /* __AIOP_VERIFICATION_DATA_H_ */
