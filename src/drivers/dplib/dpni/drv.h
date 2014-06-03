/**************************************************************************//*
 @File          drv.h

 @Description   DPNI driver structure and internal functions.
*//***************************************************************************/

#ifndef __DRV_H
#define __DRV_H

#include "common/types.h"
#include "dplib/dpni_drv.h"
#include "net/fsl_net.h"
#include "dplib/fsl_dprc.h"

int dpni_drv_probe(struct dprc *dprc, uint16_t	mc_niid, uint16_t aiop_niid,
		struct dpni_pools_cfg *pools_params);

#define DPNI_DRV_FLG_PARSE		0x80
#define DPNI_DRV_FLG_PARSER_DIS		0x40
#define DPNI_DRV_FLG_MTU_ENABLE		0x20

struct dpni_drv {
	/** network interface ID which is equal to this entry's index in the NI
	 *  table - internal to AIOP */
	uint16_t            aiop_niid;

#if 0
	/** TODO: the mc_niid field will be necessary if we decide to close the
	 *  DPNI at the end of Probe. network interface ID assigned by MC -
	 *  known outside AIOP */
	uint16_t            mc_niid;
#endif

	/** MAC address of this NI */
	uint8_t 	    mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	/** Storage profile ID */
	uint8_t             spid;
	uint8_t             res[1];
	/** Queueing destination for the enqueue. */
	uint16_t            qdid;
	/** starting HXS */
	uint16_t            starting_hxs;
	/** MTU value needed for the \ref dpni_drv_send() function */
	uint32_t            mtu;
	/** Parse Profile ID */
	uint8_t             prpid;
	/** \ref DPNI_DRV_DEFINES */
	uint8_t             flags;

	/** call back application function */
	rx_cb_t             *rx_cbs[DPNI_DRV_MAX_NUM_FLOWS];
	/** call back application argument */
	dpni_drv_app_arg_t  args[DPNI_DRV_MAX_NUM_FLOWS];
	/** connection for the command interface */
	struct dpni 		dpni;
};

void receive_cb(void);

#endif /* __DRV_H */
