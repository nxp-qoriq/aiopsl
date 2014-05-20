
#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_spinlock.h"
#include "kernel/smp.h"

#include "sys.h"


typedef struct t_spin_table {
	uint64_t entry_addr;
	uint64_t r3;
	uint32_t rsvd1;
	uint32_t pir;
} t_spin_table;


/* Global System Object */
extern t_system sys;


/*****************************************************************************/
int sys_init_multi_processing(void)
{
    uint32_t core_id = core_get_id();

    if (sys.is_partition_master[core_id])
    {
        /* Initialize the central program barrier */
	sys.barrier_lock = 0;
        
	sys.barrier_mask = sys.partition_cores_mask;
    }
    else
    {
        /* Wait until system barrier is initialized */
        while (!sys.barrier_mask) ;
    }

/*
    if (sys.ipc_enabled && sys.is_partition_master[core_id])
        sys_init_ipc();
*/

    return E_OK;
}

/*****************************************************************************/
void sys_free_multi_processing(void)
{
/*
    if (sys.ipc_enabled && sys.is_partition_master[core_get_id()])
        sys_free_ipc();
*/
}


#ifdef SYS_SPIN_TABLE_BASE

extern char _spin_table[];  /* base address of spin table (from linker file) */
                            /*Get spin table offset by core ID*/
static uint64_t sys_get_spin_table(uint64_t spin_table_base, uint32_t core_id);

/*****************************************************************************/
void sys_kick_spinning_cores(uint64_t cores_mask,
                          dma_addr_t core_master_entry,
                          dma_addr_t core_guest_entry)
{
    uint64_t    spin_table_base = PTR_TO_UINT(SYS_SPIN_TABLE_BASE);
    uint64_t    spin_table_addr;
    uint32_t    core_id, this_core_id = core_get_id();
    int        core_master;

    for (core_id = 0; core_id < INTG_MAX_NUM_OF_CORES; core_id++)
    {
        do /*Busy waiting for boot_sync_flag*/
        {} while(sys.boot_sync_flag == SYS_BOOT_SYNC_FLAG_WAITING);

        if ((cores_mask & (1 << core_id)) && (core_id != this_core_id))
        {
            /* Set boot_sync_flag */
            sys.boot_sync_flag = SYS_BOOT_SYNC_FLAG_WAITING;

            core_master = IS_CORE_MASTER(core_id, cores_mask);

            /*Get spin Table address for particular core ID*/
            spin_table_addr = sys_get_spin_table(spin_table_base, core_id);

            ASSERT_COND (spin_table_addr);

            /* Kick CPU by writing to entry_addr field of the spin table */
            WRITE_UINT64(*(uint64_t *)UINT_TO_PTR(spin_table_addr),
                            (core_master) ? (uint64_t)core_master_entry : \
                            (uint64_t)core_guest_entry);

            core_memory_barrier();
        }
    }
}

uint64_t sys_get_spin_table(uint64_t spin_table_base, uint32_t core_id)
{
    uint32_t    i;
    t_spin_table *p_spin_table = (t_spin_table *)UINT_TO_PTR(spin_table_base);
    uint64_t 	spin_table_addr = 0;

    for (i = 0; i < INTG_MAX_NUM_OF_CORES; i++)
    {
        /*PAPR defines entry address has to be set to 0x1 & PIR to core ID*/
        if((p_spin_table->pir == core_id) && (p_spin_table->entry_addr == 0x1))
        {
            /*Spin Table address has been found!*/
            spin_table_addr = (uint64_t)PTR_TO_UINT(p_spin_table);
            break;
        }
        /*go to the next spin table*/
        p_spin_table = (t_spin_table *)(PTR_TO_UINT(p_spin_table) + CORE_CACHELINE_SIZE);
    }
    return spin_table_addr;
}

/*****************************************************************************/
int sys_kick_guest_partition(uint64_t cores_mask, dma_addr_t phys_base_addr)
{
    uint64_t    spin_table_base = PTR_TO_UINT(SYS_SPIN_TABLE_BASE);
    uint32_t    core_id, this_core_id = core_get_id();
    uint8_t     entry_size = CORE_CACHELINE_SIZE;

    for (core_id = 0; core_id < INTG_MAX_NUM_OF_CORES; core_id++)
    {

        if ((cores_mask & (1 << core_id)) && (core_id != this_core_id))
        {
            WRITE_UINT64(*(uint64_t *)UINT_TO_PTR(spin_table_base + (entry_size * core_id)),
                         (uint64_t)phys_base_addr);
            core_memory_barrier();
        }
    }
    return E_OK;
}

#else
/*****************************************************************************/
void sys_kick_spinning_cores(uint64_t cores_mask,
                          dma_addr_t core_master_entry,
                          dma_addr_t core_guest_entry)
{
    ASSERT_COND (sys.platform_ops.f_enable_cores);
    UNUSED(core_master_entry);
    UNUSED(core_guest_entry);
    sys.platform_ops.f_enable_cores(sys.platform_ops.h_platform, cores_mask);
}
#endif /* SYS_SPIN_TABLE_BASE */

/*****************************************************************************/
void sys_barrier(void)
{
#ifdef UNDER_CONSTRUCTION
    const uint64_t core_mask = (uint64_t)(1 << core_get_id());

    lock_spinlock(&(sys.barrier_lock));
    /* Mark this core's presence */
    sys.barrier_mask &= ~(core_mask);

    if (sys.barrier_mask)
    {
        unlock_spinlock(&(sys.barrier_lock));
        /* Wait until barrier is reset */
        while (!(sys.barrier_mask & core_mask)) {}
    }
    else
    {
        /* Last core to arrive - reset the barrier */
        sys.barrier_mask = sys.partition_cores_mask;
        unlock_spinlock(&(sys.barrier_lock));
    }
#endif
}

/*****************************************************************************/
int sys_is_core_active(uint32_t core_id)
{
    return (int)(((sys.partition_cores_mask & (1ULL << core_id)))?0:1);
}

/*****************************************************************************/
int sys_is_master_core(void)
{
    return sys.is_partition_master[core_get_id()];
}

int sys_is_master_thread(void)
{
    return sys.is_core_master[core_get_id()];
}

/*****************************************************************************/
uint32_t sys_get_core_secondary_threads_mask(void)
{
    return GET_CORE_SECONDARY_THREADS_MASK(core_get_id(), sys.partition_cores_mask);
}

/*****************************************************************************/
uint8_t sys_get_partition_id(void)
{
    return sys.partition_id;
}

/*****************************************************************************/
uint64_t sys_get_cores_mask(void)
{
#ifdef SYS_SMP_SUPPORT
    return sys.partition_cores_mask;
#else /* SYS_SMP_SUPPORT */
    return (uint64_t)(1 << core_get_id());
#endif /* SYS_SMP_SUPPORT */
}

/*****************************************************************************/
uint64_t sys_get_masters_mask(void)
{
    return sys.master_cores_mask;
}
