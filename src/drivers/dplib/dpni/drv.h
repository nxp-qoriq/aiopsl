/**************************************************************************//*
 @File          dvr.h

 @Description   TODO

 @Cautions      None.
*//***************************************************************************/

#ifndef __DRV_H
#define __DRV_H

#include "common/types.h"
#include "dplib/dpni_drv.h"


#define DPNI_DRV_FLG_ENABLED	0x80
#define DPNI_DRV_FLG_PARSE	0x40
#define DPNI_DRV_FLG_MTU_DIS	0x20
#define DPNI_DRV_FLG_PARSER_DIS	0x01


struct dpni_drv {
	/** network interface ID */
	uint16_t            id;
	/** Storage profile ID */
	uint8_t             spid;
	uint8_t             res0[1];
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
	/** error mask for the \ref receive_cb() function FD
	* error check 0 - continue; 1 - discard */
	uint8_t             fd_err_mask;
	uint8_t             res[1];
	fsl_handle_t        dpni;

	/** call back application function */
	rx_cb_t             *rx_cbs[DPNI_DRV_MAX_NUM_FLOWS];
	uint32_t            res1;
	/** call back application argument */
	dpni_drv_app_arg_t  args[DPNI_DRV_MAX_NUM_FLOWS];
};


void receive_cb (void);


#endif /* __DRV_H */
