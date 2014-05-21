/**************************************************************************//**
@File          snic.h

@Description   This file contains the snic include file.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __SNIC_H
#define __SNIC_H

#include "common/types.h"

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


/**************************************************************************//**
@Group	SNIC_EN_FLAGS snic enable flags
@{
*//***************************************************************************/
	/** snic enable flags */
#define SNIC_IPR_EN			0x0001
#define SNIC_VLAN_REMOVE_EN		0x0002
#define SNIC_VLAN_ADD_EN		0x0004
#define SNIC_IPF_EN			0x0008

/** @} */ /* end of SNIC_EN_FLAGS */

enum  snic_cmds{
	SNIC_SET_MTU = 0,
	SNIC_IPR_CREATE_INSTANCE,
	SNIC_ENABLE_FLAGS,
	SNIC_SET_QDID,
	SNIC_GET_EPID_PC
};

#define TODO_CMDIF_MOD_SNIC 16 
/** @} */ /* end of SNIC_MACROS */

/**************************************************************************//**
@Group		SNIC_STRUCTS snic Structures

@Description	Freescale AIOP snic Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	AIOP snic parameters
*//***************************************************************************/
#pragma pack(push, 1)
struct snic_params {
	/** snic general enable flags*/
	uint32_t snic_enable_flags;
	/** IPF MTU */
	uint16_t snic_ipf_mtu;
	/** Queueing destination for the enqueue. */
	uint16_t qdid;
};
#pragma pack(pop)

/** @} */ /* end of SNIC_STRUCTS */

void snic_process_packet(void);
int snic_open_cb(void *dev);
int snic_close_cb(void *dev);
int snic_ctrl_cb(void *dev, uint16_t cmd, uint16_t size, uint8_t *data);
int32_t aiop_snic_init(void);
/** @} */ /* end of SNIC */


#endif /* __SNIC_H */
