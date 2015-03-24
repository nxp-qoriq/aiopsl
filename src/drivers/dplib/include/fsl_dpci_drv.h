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

#ifndef __FSL_DPCI_DRV_H
#define __FSL_DPCI_DRV_H

/*
 * New DPCI was added or the state of the DPCI has changed
 * The dpci_id must belong to AIOP side
 */
int dpci_drv_added(uint32_t dpci_id);

/*
 * The DPCI was removed from AIOP container
 * The dpci_id must belong to AIOP side
 */
int dpci_drv_removed(uint32_t dpci_id);

/*
 * The DPCI user context and AMQ bits are updated
 * This function is to be called only inside the open command and before
 * the AMQ bits had been changed to AIOP AMQ bits
 * For dpci_ind Use mc_dpci_find() or dpci_drv_user_ctx_get()
 */
int dpci_drv_update(uint32_t dpci_ind);

/*
 * The dpci_id can be either AIOP dpci id or the peer id
 * tx_fqid - should be array for 2 fqids
 */
int dpci_drv_tx_get(uint32_t dpci_id, uint32_t *tx_fqid);

/*
 * Read the DPCI index and FQID from user context in ADC
 */
void dpci_drv_user_ctx_get(uint32_t *dpci_ind, uint32_t *fqid);

/*
 * Read icid and amq of specific DPCI table entry
 */
void dpci_drv_icid_get(uint32_t dpci_ind, uint16_t *icid, uint16_t *amq_bdi);



#endif
