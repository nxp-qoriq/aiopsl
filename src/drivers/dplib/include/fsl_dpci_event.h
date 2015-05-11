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

/**************************************************************************//**
@File		fsl_dpci_event.h

@Description	DPCI wrapper that is internally used by Service Layer
*//***************************************************************************/

#ifndef __FSL_DPCI_EVENT_H
#define __FSL_DPCI_EVENT_H


/**************************************************************************//**
@Function	dpci_event_assign

@Description	New DPCI was added to AIOP container or the state of 
		the DPCI has changed. Updates the DPCI table. 

@Param[in]	dpci_id - DPCI id of the AIOP side.

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_event_assign(uint32_t dpci_id);


/**************************************************************************//**
@Function	dpci_event_unassign

@Description	The DPCI was removed from AIOP container or disconnected.
		Updates the DPCI table. 

@Param[in]	dpci_id - DPCI id of the AIOP side.

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_event_unassign(uint32_t dpci_id);

#define DPCI_EVENT_UPDATE_ICID	0x1
#define DPCI_EVENT_UPDATE_TX	0x2
/**************************************************************************//**
@Function	dpci_event_update

@Description	Updates the entry of DPCI table with the AMQ + BDI from ADC.
  	  	Updates dpci_peer_id in the DPCI table.
  	  	To be called only inside the open command and before 
  	  	the AMQ bits are changed to AIOP AMQ bits  

@Param[in]	dpci_ind - Use mc_dpci_find() or dpci_drv_user_ctx_get().
@Param[in]	flags    - DPCI_EVENT_UPDATE_ICID, DPCI_EVENT_UPDATE_TX.

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_event_update(uint32_t dpci_ind, uint8_t flags);

#endif /* __FSL_DPCI_EVENT_H */
