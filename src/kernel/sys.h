#ifndef __SYS_H
#define __SYS_H

#include "common/types.h"
#include "common/errors.h"
#include "common/list.h"
#include "common/dbg.h"
#include "kernel/smp.h"
#include "kernel/platform.h"
#include "inc/fsl_sys.h"


#define __ERR_MODULE__  MODULE_UNKNOWN

#define SYS_TILE_MASTERS_MASK (0x00000001)
#define SYS_CLUSTER_MASTER_MASK (0x00001111)
#define SYS_BOOT_SYNC_FLAG_DONE (1)

typedef struct t_system {
	/* Memory management variables */
	fsl_handle_t                mem_mng;
	int                         heap_partition_id;
	uintptr_t                   heap_addr;
	list_t                      virt_mem_list;
	uint8_t                     virt_mem_lock;
	uint8_t                     mem_part_mng_lock;
	uint8_t                     mem_mng_lock;

	/* Console variables */
	fsl_handle_t                console;
	int                         (*f_console_print)(fsl_handle_t console,
		uint8_t *p_data, uint32_t size);
	int                         (*f_console_get)(fsl_handle_t console,
		uint8_t *p_data, uint32_t size);
	char                        *p_pre_console_buf;
	uint32_t                    pre_console_buf_pos;
	uint8_t                     console_lock;

	/* Multi-Processing variables */
	int                  is_tile_master[INTG_MAX_NUM_OF_CORES];
	int                  is_cluster_master[INTG_MAX_NUM_OF_CORES];
	uint64_t             active_cores_mask;
	uint8_t              barrier_lock;
	volatile uint64_t    barrier_mask;

	/* boot synchronization variables */
	volatile uint32_t           boot_sync_flag;
	
	/* Platform operations */
	t_platform_ops              platform_ops;
} t_system;


void fill_platform_parameters(struct platform_param *platform_param);

/* Internal system routines */
int     sys_init_memory_management(void);
int     sys_free_memory_management(void);
int     sys_init_multi_processing(void);
void    sys_free_multi_processing(void);

void    sys_register_debugger_console(void);


#endif /* __SYS_H */
