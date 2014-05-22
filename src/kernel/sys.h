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

typedef struct t_system {
	/* Memory management variables */
	fsl_handle_t                mem_mng;
	int                         heap_partition_id;
	uintptr_t                   heap_addr;
	list_t                      virt_mem_list;
	struct spinlock             virt_mem_lock;
	struct spinlock             mem_part_mng_lock;
	struct spinlock             mem_mng_lock;

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
	int                  init_fail_count; //TODO what is this ??, noone reads this !!

	/*TODO check if need to be removed: list_t*/
	/*list_t                      forced_objects_list;*/

	/* Platform operations */
	t_platform_ops              platform_ops;
} t_system;


void fill_system_parameters(struct platform_param *platform_param);

/* Internal system routines */
int     sys_init_memory_management(void);
int     sys_free_memory_management(void);
int     sys_init_multi_processing(void);
void    sys_free_multi_processing(void);

void    sys_register_debugger_console(void);


#endif /* __SYS_H */
