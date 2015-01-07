/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "dplib/dpni_drv.h"


int aiop_app_init(void);
void aiop_app_free(void);


static void /*__noreturn*/ rflct_rx_cb (dpni_drv_app_arg_t arg)
{
	/* TODO - add here:
	 * 1. print frame content */
	/* TODO - add here swap on headers:
	 * swap_eth_hdr((uint32_t *)p_data);
	 * swap_ip_hdr((uint32_t *)(PTR_TO_UINT(p_data)+26));
	 */
	dpni_drv_send((uint16_t)arg);
}


int aiop_app_init(void)
{
    int i, err = 0;

    fsl_os_print("AIOP test: NIC\n");

    for (i=0; i<dpni_get_num_of_ni(); i++)
	    err = dpni_drv_register_rx_cb(i, 0, NULL, NULL, rflct_rx_cb, i);

    return err;
}

void aiop_app_free(void)
{
    /* TODO - complete!*/
}
