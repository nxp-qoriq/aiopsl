#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "cmdif_srv.h"
#include "general.h"
#include "io.h"
#include "fsl_fdma.h"
#include "sys.h"
#include "fsl_malloc.h"
#include "dbg.h"
#include "spinlock.h"

/** This is where rx qid should reside */
#define FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx)
/** Get RX QID from dequeue context */
#define RESP_QID_GET \
	(uint16_t)(swap_uint64(FQD_CTX_GET) & 0x01FFFFFF) /* TODO use LLLDW_SWAP */
/** Blocking commands don't need response FD */
#define SEND_RESP(CMD)	\
	((!((CMD) & CMDIF_NORESP_CMD)) && ((CMD) & CMDIF_ASYNC_CMD))
/** Malloc array of structs */
#define ARR_MALLOC_SHRAM(FIELD, TYPE, NUM) \
	FIELD = fsl_os_xmalloc(sizeof(TYPE) * (NUM), MEM_PART_SH_RAM, 1)
/** Malloc array of structs */
#define ARR_MALLOC_DDR(FIELD, TYPE, NUM) \
	FIELD = fsl_os_xmalloc(sizeof(TYPE) * (NUM), \
	                       MEM_PART_1ST_DDR_NON_CACHEABLE, 1)

#define FREE_MODULE    '\0'

static int module_id_alloc(const char *module_name, struct cmdif_srv *srv)
{
	int i = 0;
	int id = -ENAVAIL;

	if ((module_name == NULL) || (module_name[0] == FREE_MODULE))
		return -1;

	lock_spinlock(&srv->lock);

	for (i = 0; i < MAX_NUM_OF_MODULES; i++) {
		if ((srv->m_name[i][0] == FREE_MODULE) && (id == -1)) {
			id = i;
		} else if (strncmp(srv->m_name[i], module_name, MAX_NAME_CHARS) == 0) {
			unlock_spinlock(&srv->lock);
			return -EEXIST;
		}
	}
	if (id > 0) {
		strcpy(srv->m_name[i], module_name);
	}

	unlock_spinlock(&srv->lock);
	return id;
}

int cmdif_register_module(const char *module_name, struct cmdif_module_ops *ops)
{

	struct cmdif_srv *srv = sys_get_handle(FSL_OS_MOD_CMDIF_SRV, 0);

	if ((module_name == NULL) || (ops == NULL))
		return -EINVAL;

	return 0;
}

int cmdif_unregister_module(const char *module_name)
{
	if (module_name == NULL)
		return -EINVAL;

	return 0;
}

static int epid_setup()
{
#ifdef MC_INTEGRATED
	/* Initialize EPID-table with discard_rx_cb for all entries (EP_PC field) */
#if 0
	/* TODO: following code can not currently compile on AIOP, need to port over  MC definitions */
	aiop_tile_regs = (struct aiop_tile_regs *)sys_get_memory_mapped_module_base(FSL_OS_MOD_AIOP,
	                                                     0,
	                                                     E_MAPPED_MEM_TYPE_GEN_REGS);
	ws_regs = &aiop_tile_regs->ws_regs;
	/* TODO: replace 1024 w/ constant */
	for (i = 0; i < 1024; i++) {
		/* Prepare to write to entry i in EPID table */
		iowrite32((uint32_t)i, ws_regs->epas; 					// TODO: change to LE
		iowrite32(PTR_TO_UINT(discard_rx_cb), ws_regs->ep_pc); 	// TODO: change to LE
	}
#else
	/* TODO: temporary code. should be removed. */
	wrks_addr = (sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW, 0, E_MAPPED_MEM_TYPE_GEN_REGS) +
	             SOC_PERIPH_OFF_AIOP_WRKS);


	/* TODO: replace 1024 w/ constant */
	for (i = 0; i < 1024; i++) {
		/* Prepare to write to entry i in EPID table - EPAS reg */
		iowrite32((uint32_t)i, UINT_TO_PTR(wrks_addr + 0x0f8)); // TODO: change to LE, replace address with #define

		iowrite32(PTR_TO_UINT(discard_rx_cb), UINT_TO_PTR(wrks_addr + 0x100)); // TODO: change to LE, replace address with #define

#if 0
		/* TODO : this is a temporary assignment for testing purposes, until MC initialization of EPID table will be operational. */
		iowrite32((uint32_t)i, UINT_TO_PTR(wrks_addr + 0x104));
#endif
	}
#endif
#endif /* MC_INTEGRATED */
	return 0;
}

int cmdif_srv_init(void)
{
	int     err = 0;
	struct  cmdif_srv *srv = NULL;

	if (sys_get_handle(FSL_OS_MOD_CMDIF_SRV, 0))
		return -ENODEV;

	srv = fsl_os_xmalloc(sizeof(struct cmdif_srv), MEM_PART_SH_RAM, 1);
        if (srv == NULL) {
		pr_err("No memory for CMDIF Server init");
		return -ENOMEM;
        }

	/* SHRAM */
	ARR_MALLOC_SHRAM(srv->instance_handle, fsl_handle_t, MAX_NUM_OF_INSTANCES);
	ARR_MALLOC_SHRAM(srv->m_id, uint8_t, MAX_NUM_OF_INSTANCES);
	ARR_MALLOC_SHRAM(srv->ctrl_cb, ctrl_cb_t *, MAX_NUM_OF_MODULES);
	/* DDR */
	ARR_MALLOC_DDR(srv->m_name, char[MAX_NAME_CHARS + 1], MAX_NUM_OF_MODULES);
	ARR_MALLOC_DDR(srv->open_cb, open_cb_t *, MAX_NUM_OF_MODULES);
	ARR_MALLOC_DDR(srv->close_cb, close_cb_t *, MAX_NUM_OF_MODULES);

	memset(srv->m_name, FREE_MODULE, sizeof(srv->m_name[0]) * MAX_NUM_OF_MODULES);
	srv->instances_counter = 0;
        err = sys_add_handle(srv, FSL_OS_MOD_CMDIF_SRV, 1, 0);

	return 0;
}

void cmdif_srv_free(void)
{
	struct cmdif_srv *srv = sys_get_handle(FSL_OS_MOD_CMDIF_SRV, 0);

        sys_remove_handle(FSL_OS_MOD_CMDIF_SRV, 0);
        /* TODO free all memories */
        fsl_os_xfree(srv->ctrl_cb);
        fsl_os_xfree(srv->open_cb);
        fsl_os_xfree(srv);
}

static uint16_t cmd_id_get()
{
	uint64_t data = 0;
	data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	return (uint16_t)((data & CMD_ID_MASK) >> CMD_ID_OFF);
}

static uint32_t cmd_size_get()
{
	return LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
}

static uint8_t * cmd_data_get()
{
	return (uint8_t *)fsl_os_phys_to_virt(LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS));
}

static int cmdif_fd_send()
{
	int err;

	/** TODO ordering !!!*/
	err = (int)fdma_store_and_enqueue_default_frame_fqid(
							RESP_QID_GET,
							FDMA_ENWF_NO_FLAGS);
	return err;
}

static int sync_cmd_done(uint16_t cmd_id, uint32_t size, uint8_t *data)
{
	if (!(cmd_id & CMDIF_ASYNC_CMD) && (size > 0)) {
		*data = 0x1;
		return 0;
	} else {
		return -EINVAL;
	}
}

#pragma push
#pragma force_active on

void cmdif_srv_isr(void)
{
	uint16_t cmd_id = 0;
	int      err    = 0;
	uint32_t size	= 0;
	uint8_t  *data	= 0;
	struct cmdif_srv *srv = sys_get_handle(FSL_OS_MOD_CMDIF_SRV, 0);

	cmd_id	= cmd_id_get();
	data	= cmd_data_get();
	size	= cmd_size_get();

	if (cmd_id & CMD_ID_OPEN) {
		/* Call open_cb */
		sync_cmd_done(cmd_id, size, data);
	} else {
		/* Call ctrl_cb */
		if (cmd_id & CMDIF_ASYNC_CMD) {
		} else {
			sync_cmd_done(cmd_id, size, data);
		}
	}

	if (SEND_RESP(cmd_id))
	{
		err = cmdif_fd_send();
	}

	fdma_terminate_task();
}

#pragma pop
