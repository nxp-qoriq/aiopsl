#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "general.h"
#include "sys.h"
#include "dbg.h"
#include "errors.h"
#include "cmdif_client.h"

int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int priority,
		uint64_t data)
{
	UNUSED(cidesc);
	UNUSED(cmd_id);
	UNUSED(size);
	UNUSED(priority);
	UNUSED(data);

	return -ENOTSUP;
}
