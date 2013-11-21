/**************************************************************************//**
@File		dpni_drv.c

@Description	This file contains the AIOP INIT Functions
                and Network Interface functions.
*//***************************************************************************/

#include "dpni_drv.h"
#include "fsl_dpni_drv.h"

struct dpni_drv *dpni_drv_address;

extern __TASK uint8_t CURRENT_SCOPE_LEVEL;
extern __TASK uint8_t SCOPE_MODE_LEVEL1;
extern __TASK uint8_t SCOPE_MODE_LEVEL2;
extern __TASK uint8_t SCOPE_MODE_LEVEL3;
extern __TASK uint8_t SCOPE_MODE_LEVEL4;


void osm_task_init(void) {
	CURRENT_SCOPE_LEVEL = ((uint8_t)PRC_GET_OSM_SOURCE_VALUE());
		/**< 	0- No order scope specified.\n
			1- Scope was specified for level 1 of hierarchy */
	SCOPE_MODE_LEVEL1 = ((uint8_t)PRC_GET_OSM_EXECUTION_PHASE_VALUE());
		/**< 	0 = Exclusive mode.\n
			1 = Concurrent mode. */
	SCOPE_MODE_LEVEL2 = 0x00;  	
		/**< Exclusive (default) Mode in level 2 of hierarchy */
	SCOPE_MODE_LEVEL3 = 0x00;
		/**< Exclusive (default) Mode in level 3 of hierarchy */
	SCOPE_MODE_LEVEL4 = 0x00;
		/**< Exclusive (default) Mode in level 4 of hierarchy  */
}

void receive_cb(void)
{
	
}

int32_t dpni_drv_send(uint16_t ni_id, uint8_t qd_priority)
{
	struct dpni_drv *send_dpni_drv; 
	
	send_dpni_drv = dpni_drv_address + ni_id; /* calculate pointer to the send 
	 	 	 	 	 	 	 	 	 	 	   * NI structure                */
	
	return ((int32_t)qd_priority);
}

