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
@File		fsl_dpci_event.h

@Description	DPCI wrapper that is internally used by Service Layer
*//***************************************************************************/

#ifndef __FSL_DPCI_EVENT_H
#define __FSL_DPCI_EVENT_H

/**************************************************************************//**
@Function	dpci_event_update_obj

@Description	Add new DPCI entry if dpci_id is new. 
		Mark the DPCI entry as visited.   

@Param[in]	dpci_id - DPCI id of the AIOP side.

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_event_update_obj(uint32_t dpci_id);

/**************************************************************************//**
@Function	dpci_event_handle_removed_objects

@Description	Delete all the DPCI entries that haven't been scanned. 

 *//***************************************************************************/
void dpci_event_handle_removed_objects();

/**************************************************************************//**
@Function	dpci_event_link_change

@Description	Updates the entry of DPCI table with the dpci_peer_id and the 
		tx queues.
		Updates dpci_peer_id in the DPCI table.
		To be called only inside the open command and before 
		the AMQ bits are changed to AIOP AMQ bits  

@Param[in]	dpci_ind - Use dpci_mng_find() or dpci_mng_user_ctx_get().

@Return		0      - on success, POSIX error code otherwise
 *//***************************************************************************/
int dpci_event_link_change(uint32_t dpci_id);

#endif /* __FSL_DPCI_EVENT_H */
