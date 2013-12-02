#include "common/fsl_malloc.h"
#include "common/fsl_stdarg.h"
#include "common/spinlock.h"
#include "common/timer.h"
#include "common/irq.h"
#include "common/io.h"
#include "kernel/smp.h"

#include "inc/console.h"
#include "inc/mem_mng.h"


#define __ERR_MODULE__ MODULE_UNKNOWN

#define MAX_UDELAY   50000000

#define BUF_SIZE    1024


/*****************************************************************************/
void fsl_os_exit(int status)
{
    exit(status);
}

/*****************************************************************************/
void fsl_os_print(char *format, ...)
{
#ifndef EMULATOR_FINAL
    va_list args;
    char    tmp_buf[BUF_SIZE];

    va_start(args, format);
#ifdef ARENA_64BIT_ARCH
#if defined(__GNUC__)
extern void msr_enable_fp(void);
msr_enable_fp();
#endif /* defined(__GNUC__) */
#endif /* ARENA_64BIT_ARCH */
    vsnprintf (tmp_buf, BUF_SIZE, format, args);
    va_end(args);

    sys_print(tmp_buf);
#endif /* EMULATOR */
}

/*****************************************************************************/
/*                        Spinlock Service Routines                          */
/*****************************************************************************/
spinlock_t spin_lock_create(void)
{
    struct spinlock *p_spinlock = (struct spinlock *)fsl_os_malloc(sizeof(struct spinlock));
    if (!p_spinlock)
    {
        REPORT_ERROR(MAJOR, E_NO_MEMORY, ("spinlock structure"));
        return NULL;
    }

    sys_init_spinlock(p_spinlock);

    return p_spinlock;
}

void spin_lock_init(spinlock_t lock)
{
    sys_init_spinlock((struct spinlock *)lock);
}

void spin_lock_free(spinlock_t lock)
{
    if (lock)
        fsl_os_free(lock);
}

void spin_lock(spinlock_t lock)
{
    sys_lock_spinlock((struct spinlock *)lock);
}

void spin_unlock(spinlock_t lock)
{
    sys_unlock_spinlock((struct spinlock *)lock);
}

uint32_t spin_lock_irqsave(spinlock_t lock)
{
    return sys_lock_intr_spinlock((struct spinlock *)lock);
}

void spin_unlock_irqrestore(spinlock_t lock, uint32_t irq_flags)
{
    sys_unlock_intr_spinlock((struct spinlock *)lock, irq_flags);
}

#ifdef DEBUG_FSL_OS_MALLOC
/*****************************************************************************/
void * fsl_os_malloc_debug(size_t size, char *fname, int line)
{
    return sys_mem_alloc(SYS_DEFAULT_HEAP_PARTITION, size, 0, "", fname, line);
}

/*****************************************************************************/
void * fsl_os_malloc_smart_debug(size_t     size,
                           int          partition_id,
                           uint32_t     alignment,
                           char         *fname,
                           int          line)
{
    return sys_mem_alloc(partition_id, size, alignment, "", fname, line);
}

#else /* not DEBUG_FSL_OS_MALLOC */
/*****************************************************************************/
void * fsl_os_malloc(size_t size)
{
    return sys_mem_alloc(SYS_DEFAULT_HEAP_PARTITION, size, 0, "", "", 0);
}

/*****************************************************************************/
void * fsl_os_malloc_smart(size_t size, int partition_id, uint32_t alignment)
{
    return sys_mem_alloc(partition_id, size, alignment, "", "", 0);
}
#endif /* not DEBUG_FSL_OS_MALLOC */


/*****************************************************************************/
void fsl_os_free(void *p_memory)
{
    sys_mem_free(p_memory);
}

/*****************************************************************************/
void fsl_os_free_smart(void *p_memory)
{
    sys_mem_free(p_memory);
}

/*****************************************************************************/
void * fsl_os_phys_to_virt(dma_addr_t addr)
{
    return sys_phys_to_virt(addr);
}

/*****************************************************************************/
dma_addr_t fsl_os_virt_to_phys(void *addr)
{
    return sys_virt_to_phys(addr);
}


/*****************************************************************************/
/*                        Timers Service Routines                            */
/*****************************************************************************/

uint32_t fsl_os_current_time(void)
{
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return 0;
}

fsl_handle_t fsl_os_create_timer(void)
{
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return NULL;
}

void fsl_os_free_timer(fsl_handle_t tmr)
{
    UNUSED (tmr);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

int fsl_os_start_timer(fsl_handle_t   tmr,
                       uint32_t       msecs,
                       int            periodic,
                       void           (*expired_cb)(fsl_handle_t),
                       fsl_handle_t   arg)
{
    UNUSED (arg);
    UNUSED (expired_cb);
    UNUSED (msecs);
    UNUSED (periodic);
    UNUSED (tmr);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return 0;
}

void fsl_os_stop_timer(fsl_handle_t tmr)
{
    UNUSED (tmr);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

void fsl_os_mod_timer(fsl_handle_t tmr, uint32_t msecs)
{
    UNUSED (tmr);
    UNUSED (msecs);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

void fsl_os_udelay(uint32_t usecs)
{
    UNUSED (usecs);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

uint32_t fsl_os_sleep(uint32_t msecs)
{
    UNUSED (msecs);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return 0;
}
