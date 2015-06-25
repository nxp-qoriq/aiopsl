/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "fsl_malloc.h"
#include "fsl_spinlock.h"
#include "fsl_dbg.h"
#include "fsl_stdlib.h"
#include "mem_mng_util.h"
#include "fsl_mem_mng.h"
#include "sys.h"
#include "fsl_string.h"


#ifdef VERILOG
#if (defined(__GNUC__)) && (defined(sun))
extern int sigblock(int);
extern int sigsetmask(int);
#else
#define sigblock(i)     0
#define sigsetmask(i)
#endif /* GNUC */
#endif /* VERILOG */






/* Put all function (execution code) into  dtext_vle section,aka __COLD_CODE */
__START_COLD_CODE

static void     sys_print_mem_leak(void        *p_memory,
                                uint32_t    size,
                                char        *info,
                                char        *filename,
                                int         line);

void sys_mem_partitions_init_complete()
{
    mem_mng_mem_partitions_init_completed(&sys.mem_mng);
}

/*****************************************************************************/
 /* Implement a trivial version of conversion, return the same value as received. */
dma_addr_t sys_virt_to_phys(void *virt_addr)
{
    return (dma_addr_t)virt_addr;
}

/*****************************************************************************/
void * sys_shram_alloc(uint32_t    size,
                    uint32_t    alignment,
                    char        *info,
                    char        *filename,
                    int         line)
{
	 ASSERT_COND(size > 0);
	 return mem_mng_alloc_mem(&sys.mem_mng,
			           MEM_PART_SH_RAM,
	                   size,
	                   alignment,
	                   info,
	                   filename,
	                   line);
}
/*****************************************************************************/
void sys_shram_free(void *mem)
{
    mem_mng_free_mem(&sys.mem_mng, mem);
}
/*****************************************************************************/
 int sys_register_phys_addr_alloc_partition(int  partition_id,
        uint64_t  base_paddress,
        uint64_t   size,
         uint32_t   attributes,
         char       name[])
{
	int err_code;

	if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
	{
	        pr_err("Invalid value in sys_register_phys_addr_alloc_partition:"
	               "partition ID %d is reserved for default heap\n",
	               SYS_DEFAULT_HEAP_PARTITION);
	        return -EDOM;
	}
	err_code = mem_mng_register_phys_addr_alloc_partition(&sys.mem_mng,
	                                         partition_id,
	                                         base_paddress,
	                                         size,
	                                         attributes,
	                                         name);
	  if (err_code != 0)
	  {
	      if(-EEXIST == err_code)
                  pr_err("Resource already exists\n");
              else if(-EAGAIN == err_code)
                  pr_err("Resource is unavailable\n");
              return err_code;
	  }
	  return 0;
}

/*****************************************************************************/
 int sys_register_mem_partition(int        partition_id,
                                 uintptr_t  base_address,
                                 uint64_t   size,
                                 uint32_t   attributes,
                                 char       name[],
                                 int        enable_debug)
{
    int err_code;


    if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
    {
        pr_err("Invalid value in sys_register_mem_partition: partition ID %d"
               "is reserved for default heap",SYS_DEFAULT_HEAP_PARTITION);
        return -EDOM;
    }


    err_code = mem_mng_register_partition(&sys.mem_mng,
                                        partition_id,
                                        base_address,
                                        size,
                                        attributes,
                                        name,
                                        enable_debug);
    if (err_code != 0)
    {
        if(-EEXIST == err_code)
            pr_err("Resource already exists\n");
        else if(-EAGAIN == err_code)
            pr_err("Resource is unavailable \n");
        return err_code;
    }
    return 0;
}


/*****************************************************************************/
 int sys_unregister_mem_partition(int partition_id)
{
    t_mem_mng_partition_info   partition_info;
    uint32_t                leaks_count;
    int                 err_code;


    if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
    {
        pr_err("Invalid value in sys_unregister_mem_partition: partition ID %d"
               "is reserved for default heap",
	   SYS_DEFAULT_HEAP_PARTITION);
        return -EDOM;
    }

    leaks_count = mem_mng_check_leaks(&sys.mem_mng, partition_id, NULL);

    if (leaks_count)
    {
        /* Print memory leaks for this partition */
        mem_mng_get_partition_info(&sys.mem_mng, partition_id, &partition_info);

        pr_info("\r\n_memory leaks report - %s:\r\n", partition_info.name);
        pr_info("------------------------------------------------------------\r\n");
        mem_mng_check_leaks(&sys.mem_mng, partition_id, sys_print_mem_leak);
        pr_info("------------------------------------------------------------\r\n");
    }

    err_code = mem_mng_unregister_partition(&sys.mem_mng, partition_id);

    if (err_code != 0)
    {
        return err_code;
    }


    return 0;
}
/*****************************************************************************/
#if 0
int sys_get_mem_partition_info(int partition_id,t_mem_mng_partition_info* partition_info)
{
    int                 err_code;

    if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
    {
       pr_err("Invalid value in sys_get_mem_partition_info: partition ID %d is "
	       "reserved for default heap",SYS_DEFAULT_HEAP_PARTITION);
       return -EDOM;
    }


    err_code = mem_mng_get_partition_info(&sys.mem_mng, partition_id, partition_info);

    if (err_code != 0)
    {
        return (uint32_t)ILLEGAL_BASE;
    }

    return 0;
}
#endif
/*****************************************************************************/
int sys_get_phys_addr_alloc_partition_info(int partition_id,
                                           t_mem_mng_phys_addr_alloc_info* partition_info)
{
    int                 err_code;


    if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
    {
       pr_err("Invalid value in sys_get_phys_addr_alloc_partition_info: "
	       "partition ID %d is reserved for default heap",
	       SYS_DEFAULT_HEAP_PARTITION);
       return -EDOM;
    }

    err_code = mem_mng_get_phys_addr_alloc_info(&sys.mem_mng, partition_id, partition_info);

    if (err_code != 0)
    {
        return (uint32_t)ILLEGAL_BASE;
    }

    return 0;
}
/*****************************************************************************/
#if 0
uint64_t sys_get_mem_partition_base(int partition_id)
{
    t_mem_mng_partition_info   partition_info;
    int                 err_code;

    if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
    {
        pr_err("Invalid value in sys_get_mem_partition_base: partition ID %d is"
               "reserved for default heap",SYS_DEFAULT_HEAP_PARTITION);
        return -EDOM;
    }

    err_code = mem_mng_get_partition_info(&sys.mem_mng, partition_id, &partition_info);

    if (err_code != 0)
    {
        return (uint32_t)ILLEGAL_BASE;
    }

    return partition_info.base_address;
}


/*****************************************************************************/
uint32_t sys_get_mem_partition_attributes(int partition_id)
{
    t_mem_mng_partition_info   partition_info;
    int                 err_code;


    if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
    {
       pr_err("Invalid value in sys_get_mem_partition_attributes: partition"
	       "ID %d is reserved for default heap",
	   SYS_DEFAULT_HEAP_PARTITION);
       return 0;
    }

    err_code = mem_mng_get_partition_info(&sys.mem_mng, partition_id, &partition_info);

    if (err_code != 0)
    {
        return (uint32_t)0;
    }

    return partition_info.attributes;
}


/*****************************************************************************/
void sys_print_mem_partition_debug_info(int partition_id, int report_leaks)
{
    t_mem_mng_partition_info   partition_info;
    uint32_t                leaks_count;


    if (partition_id == SYS_DEFAULT_HEAP_PARTITION)
    {
       pr_err("Invalid value in sys_print_mem_partition_debug_info: no debug "
	       "information for default heap\n");
       return;
    }

    mem_mng_get_partition_info(&sys.mem_mng, partition_id, &partition_info);

    pr_info("\r\n_memory usage - %s:\r\n",partition_info.name);
    pr_info("------------------------------------------------------------\r\n");
    pr_info("base address:         0x%08X\r\n", partition_info.base_address);
    pr_info("total size (KB):      %10lu\r\n", (partition_info.size / 1024));
#ifdef ENABLE_DEBUG_ENTRIES
    pr_info("current usage (KB):   %10lu\r\n", (partition_info.current_usage / 1024));
    pr_info("maximum usage (KB):   %10lu\r\n", (partition_info.maximum_usage / 1024));
    pr_info("total allocations:    %10lu\r\n", partition_info.total_allocations);
    pr_info("total deallocations:  %10lu\r\n", partition_info.total_deallocations);
#endif
    pr_info("\r\n");

    if (report_leaks)
    {
        pr_info("\r\n_memory leaks report - %s:\r\n", partition_info.name);
        pr_info("------------------------------------------------------------\r\n");

        leaks_count = mem_mng_check_leaks(&sys.mem_mng, partition_id, sys_print_mem_leak);

        if (!leaks_count)
        {
            pr_info("no memory leaks were found\r\n");
        }
        pr_info("\r\n");
    }
}
#endif
/*****************************************************************************/
 int sys_init_memory_management(void)
{
    t_mem_mng_param mem_mng_param;


#ifdef AIOP
    sys.mem_mng_lock = 0;
    sys.mem_part_mng_lock = 0;
#else /* not AIOP */
    spin_lock_init(&(sys.mem_mng_lock));
    spin_lock_init(&(sys.mem_part_mng_lock));
#endif /* AIOP */


    mem_mng_param.lock = &(sys.mem_part_mng_lock);

    /* initialize boot memory manager to use MEM_PART_DP_DDR*/
#ifndef NO_DP_DDR
    boot_mem_mng_init(&sys.boot_mem_mng,MEM_PART_DP_DDR);
#else
    boot_mem_mng_init(&sys.boot_mem_mng,MEM_PART_SYSTEM_DDR);
#endif
    if(mem_mng_init(&sys.boot_mem_mng,&mem_mng_param,&sys.mem_mng) != 0)
    {
        pr_err("Resource is unavailable: memory management object\n");
        return -EAGAIN;
    }

    return 0;
}


/*****************************************************************************/
 int sys_free_memory_management(void)
{
    uint32_t leaks_count;



    leaks_count = mem_mng_check_leaks(&sys.mem_mng, MEM_MNG_EARLY_PARTITION_ID, NULL);

    if (leaks_count)
    {
        /* Print memory leaks of early allocations */
        pr_info("\r\n_memory leaks report - early allocations:\r\n");
        pr_info("------------------------------------------------------------\r\n");
        mem_mng_check_leaks(&sys.mem_mng, MEM_MNG_EARLY_PARTITION_ID, sys_print_mem_leak);
        pr_info("------------------------------------------------------------\r\n");
    }

    mem_mng_free(&sys.mem_mng,&sys.boot_mem_mng);
    memset(&sys.mem_mng,0,sizeof(sys.mem_mng));

    return 0;
}

/*****************************************************************************/
static void sys_print_mem_leak(void        *p_memory,
                            uint32_t    size,
                            char        *info,
                            char        *filename,
                            int         line)
{
    UNUSED(size);
    UNUSED(info);

    pr_info("memory leak: 0x%09p, file: %s (%d)\r\n", p_memory, filename, line);
}

/*****************************************************************************/
int  sys_get_phys_mem(uint64_t size, int mem_partition_id, uint64_t alignment,
                 uint64_t* paddr)
{
	ASSERT_COND(size > 0);
	return  mem_mng_get_phys_mem(&sys.mem_mng,
                                mem_partition_id,
                                size,
	                        alignment,
	                        paddr);
}

/*****************************************************************************/
void  sys_put_phys_mem(uint64_t paddr)
{
	mem_mng_put_phys_mem(&sys.mem_mng,paddr);
}


__END_COLD_CODE

