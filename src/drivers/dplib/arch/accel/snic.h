/**************************************************************************//**
@File          snic.h

@Description   This file contains the snic include file.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __SNIC_H
#define __SNIC_H

#include "common/types.h"
#include "dplib/fsl_ipr.h"
#include "dplib/fsl_snic_cmd.h"

/**************************************************************************//**
@Group		SNIC AIOP snic Internal

@Description	Freescale AIOP snic internal API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	SNIC_MACROS snic Macros

@Description	Freescale AIOP snic Macros

@{
*//***************************************************************************/

#define SNIC_PRPID	0
#define SNIC_HXS	0
#define SNIC_SPID	0

#define VLAN_VID_MASK	0x00000FFF
#define VLAN_PCP_MASK	0x0000E000
#define VLAN_PCP_SHIFT	13

#define MAX_SNIC_NO     16

/** @} */ /* end of SNIC_MACROS */

/**************************************************************************//**
@Group		SNIC_STRUCTS snic Structures

@Description	Freescale AIOP snic Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	AIOP snic parameters
*//***************************************************************************/

struct snic_params {
	/** IPR instance is per snic */
	ipr_instance_handle_t ipr_instance_val;
	/** snic general enable flags */
	uint32_t snic_enable_flags;
	/** IPF MTU */
	uint16_t snic_ipf_mtu;
	/** Queueing destination for the enqueue. */
	uint16_t qdid;
	/** valid */
	uint32_t valid;
};

/** @} */ /* end of SNIC_STRUCTS */

void snic_process_packet(void);
int aiop_snic_init(void);
int snic_ipf(struct snic_params *snic);
int snic_ipr(struct snic_params *snic);
int snic_add_vlan(void);
/** @} */ /* end of SNIC */


#endif /* __SNIC_H */
