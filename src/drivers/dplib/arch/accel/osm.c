/**************************************************************************//**
@File		osm.c

@Description	This file contains the AIOP OSM Operations API implementation.
*//***************************************************************************/

#include "fsl_osm.h"
#include "osm.h"
#include "dpni_drv.h"

__TASK uint8_t CURRENT_SCOPE_LEVEL;
__TASK uint8_t SCOPE_MODE_LEVEL1;
__TASK uint8_t SCOPE_MODE_LEVEL2;
__TASK uint8_t SCOPE_MODE_LEVEL3;
__TASK uint8_t SCOPE_MODE_LEVEL4;


int32_t osm_scope_transition_to_exclusive_with_increment_scope_id(void)
{

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_EXCL_OP,
			OSM_SCOPE_ID_STAGE_INCREMENT_MASK))
		return 1;
	else {
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL1):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL1 = 0;
			break;
			}
		case (LEVEL2):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL2 = 0;
			break;
			}
		case (LEVEL3):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL3 = 0;
			break;
			}
		case (LEVEL4):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL4 = 0;
			break;
			}
		}
		return 0;
	}
}

int32_t osm_scope_transition_to_exclusive_with_new_scope_id(
		uint32_t scope_id) {

	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
	switch (CURRENT_SCOPE_LEVEL) {
	case (LEVEL1):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL2):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL3):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL4):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	}

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_EXCL_WITH_NEW_SCOPEID_OP,
			scope_id))
		return 1;
	else {
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL1):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL1 = 0;
			break;
			}
		case (LEVEL2):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL2 = 0;
			break;
			}
		case (LEVEL3):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL3 = 0;
			break;
			}
		case (LEVEL4):
			{
			/** 0 = Exclusive mode. */
			SCOPE_MODE_LEVEL4 = 0;
			break;
			}
		}
		return 0;
	}
}

int32_t osm_scope_transition_to_concurrent_with_increment_scope_id(void)
{

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_CONCUR_OP,
		OSM_SCOPE_ID_STAGE_INCREMENT_MASK))
		return 1;
	else {
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL1):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL1 = 1;
			break;
			}
		case (LEVEL2):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL2 = 1;
			break;
			}
		case (LEVEL3):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL3 = 1;
			break;
			}
		case (LEVEL4):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL4 = 1;
			break;
			}

		}
		return 0;
	}
}

int32_t osm_scope_transition_to_concurrent_with_new_scope_id(
		uint32_t scope_id) {

	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
	switch (CURRENT_SCOPE_LEVEL) {
	case (LEVEL1):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL2):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL3):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL4):
		{
		scope_id = (scope_id & ~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 & OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	}

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_TRANSITION_TO_CONCUR_WITH_NEW_SCOPEID_OP,
			scope_id))
		return 1;
	else {
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL1):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL1 = 1;
			break;
			}
		case (LEVEL2):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL2 = 1;
			break;
			}
		case (LEVEL3):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL3 = 1;
			break;
			}
		case (LEVEL4):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL4 = 1;
			break;
			}

		}
		return 0;
	}
}

void osm_scope_relinquish_exclusivity(void)
{

	/* call OSM */
	__e_osmcmd(OSM_SCOPE_RELINQUISH_EXCL_OP, 0);
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL1):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL1 = 1;
			break;
			}
		case (LEVEL2):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL2 = 1;
			break;
			}
		case (LEVEL3):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL3 = 1;
			break;
			}
		case (LEVEL4):
			{
			/** 1 = Concurrent mode. */
			SCOPE_MODE_LEVEL4 = 1;
			break;
			}
		}
}

int32_t osm_scope_enter_to_exclusive_with_increment_scope_id(void)
{

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_SCOPE_INC_REL_PARENT_OP
			, OSM_SCOPE_ID_LEVEL_INCREMENT_MASK))
		return 1;
	else {
		CURRENT_SCOPE_LEVEL++;
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL1):
			{
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL1 = 0;
			break;
			}
		case (LEVEL2):
			{
			/** 1 = Parent: Concurrent mode. */
			SCOPE_MODE_LEVEL1 = 1;
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL2 = 0;
			break;
			}
		case (LEVEL3):
			{
			/** 1 = Parent: Concurrent mode. */
			SCOPE_MODE_LEVEL2 = 1;
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL3 = 0;
			break;
			}
		case (LEVEL4):
			{
			/** 1 = Parent: Concurrent mode. */
			SCOPE_MODE_LEVEL3 = 1;
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL4 = 0;
			break;
			}

		}
		return 0;
	}
}

int32_t osm_scope_enter_to_exclusive_with_new_scope_id(
		uint32_t child_scope_id) {

	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
	switch (CURRENT_SCOPE_LEVEL) {
	case (LEVEL0):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL1):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_2 &
		OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL2):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_3 &
		OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	case (LEVEL3):
		{
		child_scope_id = (child_scope_id &
		~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
		(OSM_SCOPE_ID_LEVEL_4 &
		OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
		break;
		}
	}

	/* call OSM */
	if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_REL_PARENT_OP, child_scope_id))
		return 1;
	else {
		CURRENT_SCOPE_LEVEL++;
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL1):
			{
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL1 = 0;
			break;
			}
		case (LEVEL2):
			{
			/** 1 = Parent: Concurrent mode. */
			SCOPE_MODE_LEVEL1 = 1;
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL2 = 0;
			break;
			}
		case (LEVEL3):
			{
			/** 1 = Parent: Concurrent mode. */
			SCOPE_MODE_LEVEL2 = 1;
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL3 = 0;
			break;
			}
		case (LEVEL4):
			{
			/** 1 = Parent: Concurrent mode. */
			SCOPE_MODE_LEVEL3 = 1;
			/** 0 = Child: Exclusive mode. */
			SCOPE_MODE_LEVEL4 = 0;
			break;
			}

		}
		return 0;
	}
}

int32_t osm_scope_enter(
		uint32_t scope_enter_flags,
		uint32_t child_scope_id) {

	switch (scope_enter_flags) {

	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_CONC_OP,
			child_scope_id))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				break;
				}
			case (LEVEL2):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				break;
				}
			case (LEVEL3):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				break;
				}
			case (LEVEL4):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL4 = 1;
				break;
				}

			}
			return 0;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT):
	{
		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_CONC_SCOPE_INC_OP,
				OSM_SCOPE_ID_LEVEL_INCREMENT_MASK))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				break;
				}
			case (LEVEL2):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				break;
				}
			case (LEVEL3):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				break;
				}
			case (LEVEL4):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL4 = 1;
				break;
				}

			}
			return 0;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_CONC_REL_PARENT_OP,
			child_scope_id))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				break;
				}
			case (LEVEL2):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				break;
				}
			case (LEVEL3):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				break;
				}
			case (LEVEL4):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL4 = 1;
				break;
				}

			}
			return 0;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_CONCURENT |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
		if (__e_osmcmd_(
			OSM_SCOPE_ENTER_CONC_SCOPE_INC_REL_PARENT_OP
				, OSM_SCOPE_ID_LEVEL_INCREMENT_MASK))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				break;
				}
			case (LEVEL2):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				break;
				}
			case (LEVEL3):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				break;
				}
			case (LEVEL4):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				/** 1 = Child: Concurrent mode. */
				SCOPE_MODE_LEVEL4 = 1;
				break;
				}

			}
			return 0;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(
			OSM_SCOPE_ENTER_EXCL_OP, child_scope_id))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL1 = 0;
				break;
				}
			case (LEVEL2):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL2 = 0;
				break;
				}
			case (LEVEL3):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL3 = 0;
				break;
				}
			case (LEVEL4):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL4 = 0;
				break;
				}

			}
			return 0;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT):
	{
		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_SCOPE_INC_OP,
				OSM_SCOPE_ID_LEVEL_INCREMENT_MASK))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL1 = 0;
				break;
				}
			case (LEVEL2):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL2 = 0;
				break;
				}
			case (LEVEL3):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL3 = 0;
				break;
				}
			case (LEVEL4):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL4 = 0;
				break;
				}

			}
			return 0;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
	/* update the SCOPE_ID_LEVEL_INCREMENT field in the new scope_id */
		switch (CURRENT_SCOPE_LEVEL) {
		case (LEVEL0):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL1):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_2 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL2):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_3 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		case (LEVEL3):
			{
			child_scope_id = (child_scope_id &
			~OSM_SCOPE_ID_LEVEL_INCREMENT_MASK) |
			(OSM_SCOPE_ID_LEVEL_4 &
			OSM_SCOPE_ID_LEVEL_INCREMENT_MASK);
			break;
			}
		}

		/* call OSM */
		if (__e_osmcmd_(OSM_SCOPE_ENTER_EXCL_REL_PARENT_OP,
				child_scope_id))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL1 = 0;
				break;
				}
			case (LEVEL2):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL2 = 0;
				break;
				}
			case (LEVEL3):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL3 = 0;
				break;
				}
			case (LEVEL4):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL4 = 0;
				break;
				}

				}
			return 0;
		}
		break;
	}
	case (OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE |
			OSM_SCOPE_ENTER_CHILD_SCOPE_INCREMENT |
			OSM_SCOPE_ENTER_RELINQUISH_PARENT_EXCLUSIVITY):
	{
		/* call OSM */
		if (__e_osmcmd_(
			OSM_SCOPE_ENTER_EXCL_SCOPE_INC_REL_PARENT_OP
				, OSM_SCOPE_ID_LEVEL_INCREMENT_MASK))
			return 1;
		else {
			CURRENT_SCOPE_LEVEL++;
			switch (CURRENT_SCOPE_LEVEL) {
			case (LEVEL1):
				{
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL1 = 0;
				break;
				}
			case (LEVEL2):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL1 = 1;
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL2 = 0;
				break;
				}
			case (LEVEL3):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL2 = 1;
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL3 = 0;
				break;
				}
			case (LEVEL4):
				{
				/** 1 = Parent: Concurrent mode. */
				SCOPE_MODE_LEVEL3 = 1;
				/** 0 = Child: Exclusive mode. */
				SCOPE_MODE_LEVEL4 = 0;
				break;
				}

				}
			return 0;
		}
		break;
	}
	default:
		return 1;
		break;
	}
}

void osm_scope_exit(void)
{
	/* call OSM */
	__e_osmcmd(OSM_SCOPE_EXIT_OP, 0);
		if (CURRENT_SCOPE_LEVEL != 0)
			CURRENT_SCOPE_LEVEL--;
}

void osm_get_scope(struct scope_status_params *scope_status)
{
	scope_status->scope_level = CURRENT_SCOPE_LEVEL;
	switch (CURRENT_SCOPE_LEVEL) {
	case (LEVEL1):
		{
		scope_status->scope_mode = SCOPE_MODE_LEVEL1;
		break;
		}
	case (LEVEL2):
		{
		scope_status->scope_mode = SCOPE_MODE_LEVEL2;
		break;
		}
	case (LEVEL3):
		{
		scope_status->scope_mode = SCOPE_MODE_LEVEL3;
		break;
		}
	case (LEVEL4):
		{
		scope_status->scope_mode = SCOPE_MODE_LEVEL4;
		break;
		}
	}
}
