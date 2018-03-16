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

/**************************************************************************//*
 @File          drv.h

 @Description   DPNI driver structure and internal functions.
*//***************************************************************************/

#ifndef __DRV_H
#define __DRV_H

#include "fsl_types.h"
#include "fsl_dpni_drv.h"
#include "fsl_net.h"
#include "fsl_dprc.h"

#define DPNI_DRV_FLG_PARSE		0x80
#define DPNI_DRV_FLG_PARSER_DIS		0x40
#define DPNI_DRV_FLG_MTU_ENABLE		0x20
#define DPNI_DRV_FLG_SCANNED		0x01


#pragma pack(push, 1)
struct dpni_drv_params {
	/** starting HXS */
	uint16_t            starting_hxs;
	/** Ingress Parse Profile ID */
	uint8_t             prpid;
	/** Egress Parse Profile ID */
	uint8_t             prpid_egress;
	/** \ref DPNI_DRV_DEFINES */
	uint8_t             flags;
	/** Storage profile ID */
	uint8_t             spid;
	/** Storage profile ID to use DDR pool*/
	uint8_t             spid_ddr;
	/** epid table index */
	uint16_t            epid_idx;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct dpni_drv_tx_params {
	/** Queuing destination for the enqueue. */
	uint16_t            qdid;

};
#pragma pack(pop)

#pragma pack(push, 1)
struct dpni_drv_fs_entry {
	uint16_t etype;
	uint8_t pos;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct dpni_drv_fs_params {
	uint8_t size;
	struct dpni_drv_fs_entry table[DPNI_FS_TABLE_SIZE];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct dpni_drv {
	struct dpni_drv_params dpni_drv_params_var;
	struct dpni_drv_tx_params dpni_drv_tx_params_var;
	/** network interface ID assigned by MC -
	*  known outside AIOP */
	uint16_t            dpni_id;
	/** MAC address of this NI */
	uint8_t 	    mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	/* lock for multi-core support */
	struct dpni_drv_fs_params fs;
};
#pragma pack(pop)


#endif /* __DRV_H */
