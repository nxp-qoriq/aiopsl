#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "general.h"
#include "sys.h"
#include "dbg.h"
#include "errors.h"
#include "cmdif_client_aiop.h"

static int session_get(const char *module_name, 
                       uint8_t ins_id, 
                       uint32_t dpci_id, 
                       struct cmdif_desc *cidesc)
{
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);

}

int cmdif_open(struct cmdif_desc *cidesc,
		const char *module_name,
		uint8_t ins_id,
		cmdif_cb_t async_cb,
		void *async_ctx,
		uint8_t *v_data,
		uint64_t p_data,
		uint32_t size)
{
	struct cmdif_dev *dev = NULL;
	int    err = 0;
	
	if ((v_data == NULL) || (p_data == NULL) || (size > 0))
		return -EINVAL; /* Buffer allocated by GPP */
	
	err = session_get(module_name, ins_id, (uint32_t)cidesc->regs, cidesc);
}

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
