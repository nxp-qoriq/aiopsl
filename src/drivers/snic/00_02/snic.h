/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File          snic.h

@Description   This file contains the snic include file.

*//***************************************************************************/

#ifndef __SNIC_H
#define __SNIC_H

#ifdef ENABLE_SNIC

#include "fsl_types.h"
#include "fsl_ipr.h"
#include "fsl_ipsec.h"
#include "ipsec.h"
#include "fsl_keygen.h"
#include "fsl_snic_cmd.h"
#include "fsl_tman.h"

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

#define SNIC_MEM_ALIGN 64

#define SNIC_PRPID	0
#define SNIC_HXS	0
#define SNIC_SPID	0

#define VLAN_VID_MASK	0x00000FFF
#define VLAN_PCP_MASK	0x0000E000
#define VLAN_PCP_SHIFT	13

#define MAX_SNIC_NO     16
/* todo need to set SNIC_MAX_NO_OF_TIMERS bigger than max number of open 
 * reassembly frames and IPsec SAs in all snics */
#define SNIC_MAX_NO_OF_TIMERS   100
/* maximum open reassembly frames per nic */
#define MAX_OPEN_IPR_FRAMES     100
/* Maximum number of IPsec SAs per nic */
#define MAX_SA_NO               100

/* ipsec_flags */
#define SNIC_IPSEC_IPV4_ENABLE  0x01
#define SNIC_IPSEC_IPV6_ENABLE  0x02

/* ASA size and location */
#define SNIC_ASA_SIZE		2 /* Number of 64B blocks */
#define SNIC_ASA_LOCATION	0x300

/* ASA fields */
#define FASWO1 0x50
#define FASWO2 0x54

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
#ifdef ENABLE_SNIC_IPR
	/** IPR instance is per snic */
	ipr_instance_handle_t ipr_instance_val;
#endif

#ifdef ENABLE_SNIC_IPSEC
	/** IPsec instance is per snic */
	ipsec_instance_handle_t ipsec_instance_val;
#endif

	/** snic general enable flags */
	uint32_t snic_enable_flags;
	/** IPF MTU */
	uint16_t snic_ipf_mtu;
	/** Queueing destination for the enqueue. */
	uint16_t qdid;
	/** valid */
	uint32_t valid;
	/** Storage profile ID */
	uint8_t spid;

#ifdef ENABLE_SNIC_IPSEC
	uint8_t ipsec_ipv4_key_size;
	uint8_t ipsec_table_id;
	uint8_t ipsec_ipv6_key_size;
	uint8_t dec_ipsec_ipv4_key_id;
	uint8_t dec_ipsec_ipv4_table_id;
	uint8_t dec_ipsec_ipv6_table_id;
	uint8_t dec_ipsec_ipv6_key_id;
	uint8_t ipsec_flags;
#endif

	/** Address of the TCP GRO internal context */
	uint64_t tcp_gro_ctx;
};

/** @} */ /* end of SNIC_STRUCTS */

void snic_process_packet(void);

int aiop_snic_init(void);
int aiop_snic_early_init(void);
void aiop_snic_free(void);
void snic_tman_confirm_cb(tman_arg_8B_t arg1, tman_arg_2B_t arg2);

#ifdef ENABLE_SNIC_IPF
int snic_ipf(struct snic_params *snic);
#endif

#ifdef ENABLE_SNIC_IPR
int snic_ipr(struct snic_params *snic);
void snic_ipr_timout_cb(ipr_timeout_arg_t arg, uint32_t flags);
void snic_ipr_confirm_delete_cb(ipr_del_arg_t arg);
#endif

#ifdef ENABLE_SNIC_VLAN
int snic_add_vlan(void);
#endif

#ifdef ENABLE_SNIC_IPSEC
int snic_create_table_key_id(uint8_t fec_no, uint8_t fec_array[8], 
				uint8_t key_size,
				uint32_t committed_sa_num, uint32_t max_sa_num,
				uint8_t *key_id,
				uint16_t *table_id);
int snic_ipsec_decrypt(struct snic_params *snic);
int snic_ipsec_encrypt(struct snic_params *snic);
int snic_ipsec_create_instance(struct snic_cmd_data *cmd_data);
int snic_ipsec_add_sa(struct snic_cmd_data *cmd_data);
int snic_ipsec_del_sa(struct snic_cmd_data *cmd_data);
int snic_ipsec_del_instance(struct snic_cmd_data *cmd_data);
int snic_ipsec_sa_get_stats(struct snic_cmd_data *cmd_data);
#endif
/** @} */ /* end of SNIC */

#endif	/* ENABLE_SNIC */
#endif /* __SNIC_H */
