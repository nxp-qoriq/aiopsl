#include "time.h"
#include "aiop_common.h"
#include "fsl_endian.h"
#include "fsl_sys.h"

__HOT_CODE uint64_t _gettime(void)
{
	uint32_t TSCRU1, TSCRU2, TSCRL;
	uint64_t temp_val;
	struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *)
			                        	       sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);

	TSCRU1 = LOAD_LE32_TO_CPU(&aiop_regs->cmgw_regs.tscru);
	TSCRL = LOAD_LE32_TO_CPU(&aiop_regs->cmgw_regs.tscrl);
	if ((TSCRU2=LOAD_LE32_TO_CPU(&aiop_regs->cmgw_regs.tscru)) > TSCRU1 )
		TSCRL = 0;
	else if(TSCRU2 < TSCRU1) /*something wrong while reading*/
		return -EACCES;

	temp_val = (uint64_t)(TSCRU2) << 32;
	temp_val |= (TSCRL); /*temp_val holds time in nanoseconds*/
	return (temp_val / 1000); /*convert to microseconds*/
}
