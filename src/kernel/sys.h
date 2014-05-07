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

#define SYS_BOOT_SYNC_FLAG_WAITING  1
#define SYS_BOOT_SYNC_FLAG_DONE     2

#define BUILD_CORE_MASK(_core_id)  \
    (((0x1 << INTG_THREADS_PER_CORE) - 1) << (((_core_id)/INTG_THREADS_PER_CORE) * INTG_THREADS_PER_CORE))

#define IS_CORE_MASTER(_core_id, _partition_cores_mask) (int)(((_core_id) == 0) ? \
    1 : !((BUILD_CORE_MASK(_core_id) & (_partition_cores_mask)) & ((0x1 <<(_core_id)) - 1)))

#define GET_CORE_SECONDARY_THREADS_MASK(_core_id, _partition_cores_mask) \
    (uint32_t)((BUILD_CORE_MASK(_core_id) & (_partition_cores_mask)) ^ (0x1 << (_core_id))) >> \
              (((_core_id)/INTG_THREADS_PER_CORE) * INTG_THREADS_PER_CORE)


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
    int                         (*f_console_print)(fsl_handle_t console, uint8_t *p_data, uint32_t size);
    int                         (*f_console_get)(fsl_handle_t console, uint8_t *p_data, uint32_t size);
    char                        *p_pre_console_buf;
    uint32_t                    pre_console_buf_pos;
    struct spinlock             console_lock;

    /* Multi-Processing variables */
    uint8_t                     partition_id;
    uint64_t                    partition_cores_mask;
    uint64_t                    master_cores_mask;
    int                         is_core_master[INTG_MAX_NUM_OF_CORES];
    int                         is_partition_master[INTG_MAX_NUM_OF_CORES];
    int                         is_master_partition_master[INTG_MAX_NUM_OF_CORES];
    struct spinlock             barrier_lock;
    volatile uint64_t           barrier_mask;
    int                         core_failure[INTG_MAX_NUM_OF_CORES];
    int                         init_fail_count;

    list_t                      forced_objects_list;

    /* boot synchronization variables */
    volatile uint32_t           boot_sync_flag;

    /* Platform operations */
    t_platform_ops              platform_ops;
} t_system;


int     fill_system_parameters(t_sys_param *sys_param);

/* Internal system routines */
int     sys_init_memory_management(void);
int     sys_free_memory_management(void);
int     sys_init_interrupt_management(void);
int     sys_free_interrupt_management(void);
int     sys_init_multi_processing(void);
void    sys_free_multi_processing(void);
void    sys_kick_spinning_cores(uint64_t    cores_mask,
                                dma_addr_t  core_master_entry,
                                dma_addr_t  core_guest_entry);

int     sys_init_cli(void);
void    sys_free_cli(void);
void    sys_run_cli(void);
void    sys_stop_cli(void);

void    sys_register_debugger_console(void);


#endif /* __SYS_H */
