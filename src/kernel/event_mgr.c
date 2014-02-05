/**************************************************************************//*
 @file          event_mgr.c

 @description   AIOP Event Manager 
                Implements the AIOP Event Manager logic.

 @cautions      None.
*//***************************************************************************/

/*!
 * AIOP initalization functions array. 
 * Add here pointers to initialization fuctions inplemented in an AIOP
 * appocation. The functions listed in this array will called by the AIOP
 * Service Layer at initialization time. The call order will be matching the
 * order the functiona appear in the array - from top to bottom
 */
#include "common/types.h"
#include "kernel/event_mgr.h"

#if 0
int evm_register_callback( uint8_t evm_event_id,
	uint64_t app_listen_mask,
	uint8_t priority,
	uint8_t type,  
	evm_cb_t cb)
{
    UNUSED (type);
    UNUSED (app_listen_mask);
    UNUSED (cb);
    UNUSED (evm_event_id);
    UNUSED (priority);
	/* to be done */
    return 0;
}
#endif
