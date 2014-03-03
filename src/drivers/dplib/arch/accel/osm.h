/**************************************************************************//**
@File		osm.h

@Description	This file contains the AIOP SW OSM internal API
*//***************************************************************************/


#ifndef __OSM_H_
#define __OSM_H_

#include "general.h"

extern __TASK uint8_t CURRENT_SCOPE_LEVEL;
extern __TASK uint8_t SCOPE_MODE_LEVEL1;
extern __TASK uint8_t SCOPE_MODE_LEVEL2;
extern __TASK uint8_t SCOPE_MODE_LEVEL3;
extern __TASK uint8_t SCOPE_MODE_LEVEL4;


#define LEVEL0		0x00000000  /** Null ScopeID */
#define LEVEL1		0x00000001  /** Level 1 hierarchy scope level */
#define LEVEL2		0x00000002  /** Level 2 hierarchy scope level */
#define LEVEL3		0x00000003  /** Level 3 hierarchy scope level */
#define LEVEL4		0x00000004  /** Level 4 hierarchy scope level */

/** Level field in scope_id independently of the
	OSM_SCOPE_ID_LEVEL_INCREMENT_MASK definition */
	/** Level 1 in scope_id */
#define OSM_SCOPE_ID_LEVEL_1		0x00000000
	/** Level 2 in scope_id */
#define OSM_SCOPE_ID_LEVEL_2		0x55555555
	/** Level 3 in scope_id */
#define OSM_SCOPE_ID_LEVEL_3		0xaaaaaaaa
	/** Level 4 in scope_id */
#define OSM_SCOPE_ID_LEVEL_4		0xffffffff


/* OSM Commands OP */

	/** OSM Scope Transition to Exclusive Opcode */
#define OSM_SCOPE_TRANSITION_TO_EXCL_OP				0x0d
	/** OSM Scope Transition to Exclusive and Modify Scope_id Opcode */
#define OSM_SCOPE_TRANSITION_TO_EXCL_WITH_NEW_SCOPEID_OP	0x09
	/** OSM Scope Transition to Concurrent Opcode */
#define OSM_SCOPE_TRANSITION_TO_CONCUR_OP			0x0c
	/** OSM Scope Transition to Concurrent and Modify Scope_id Opcode */
#define OSM_SCOPE_TRANSITION_TO_CONCUR_WITH_NEW_SCOPEID_OP	0x08
	/** OSM Scope Relinquish Exclusivity Opcode */
#define OSM_SCOPE_RELINQUISH_EXCL_OP				0x05
	/** OSM Scope Exit Opcode */
#define OSM_SCOPE_EXIT_OP					0x02

/* OSM SCOPE_ENTER Commands OP */

	/** OSM Scope Enter: relinquish_exclusivity_parent=false, scope_id=N,
	 * request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_OP				0x10
	/** OSM Scope Enter: relinquish_exclusivity_parent=false,
	 * scope_id=INCR, request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_SCOPE_INC_OP		0x14
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=N, request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_REL_PARENT_OP		0x12
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=INCR, request_exclusivity=false */
#define OSM_SCOPE_ENTER_CONC_SCOPE_INC_REL_PARENT_OP	0x16
	/** OSM Scope Enter: relinquish_exclusivity_parent=false, scope_id=N,
	 * request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_OP				0x11
	/** OSM Scope Enter: relinquish_exclusivity_parent=false,
	 * scope_id=INCR, request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_SCOPE_INC_OP		0x15
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=N, request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_REL_PARENT_OP		0x13
	/** OSM Scope Enter: relinquish_exclusivity_parent=true,
	 * scope_id=INCR, request_exclusivity=true */
#define OSM_SCOPE_ENTER_EXCL_SCOPE_INC_REL_PARENT_OP	0x17

inline void osm_task_init(void)
{
	/**<	0 = No order scope specified.\n
			1 = Scope was specified for level 1 of hierarchy */
	CURRENT_SCOPE_LEVEL = ((uint8_t)PRC_GET_OSM_SOURCE_VALUE());
	/**<	0 = Concurrent mode.\n
			1 = Exclusive mode. */
	SCOPE_MODE_LEVEL1 = ((uint8_t)PRC_GET_OSM_EXECUTION_PHASE_VALUE());
	/**<	Exclusive (default) Mode in level 2 of hierarchy */
	SCOPE_MODE_LEVEL2 = 0x00;
	/**<	Exclusive (default) Mode in level 3 of hierarchy */
	SCOPE_MODE_LEVEL3 = 0x00;
	/**<	Exclusive (default) Mode in level 4 of hierarchy */
	SCOPE_MODE_LEVEL4 = 0x00;
}


#endif /* __OSM_H_ */
