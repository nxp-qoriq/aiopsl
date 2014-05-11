/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          dbg.h

 @Description   Debug mode definitions.
*//***************************************************************************/

#ifndef __FSL_DBG_H
#define __FSL_DBG_H

#include "common/types.h"
#include "common/io.h"
#include "common/fsl_stdio.h"


/**************************************************************************//**
 @Group         gen_g  General Drivers Utilities

 @Description   External routines.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         dump_g  Memory and Registers Dump Mechanism

 @Description   Macros for dumping memory mapped structures.

 @{
*//***************************************************************************/

#if (DEBUG_ERRORS > 0)
/* Internally used macros */
#define dump_print          fsl_os_print
#define DUMP_MAX_LEVELS     6
#define DUMP_MAX_STR        64

#define _CREATE_DUMP_SUBSTR(phrase) \
    dump_tmp_level = 0; dump_sub_str[0] = '\0'; \
    sprintf(dump_tmp_str, "%s", #phrase); \
    p_dump_token = strtok(dump_tmp_str, (dump_is_arr[0] ? "[" : ".")); \
    while (p_dump_token != NULL) \
    { \
        strcat(dump_sub_str, p_dump_token); \
        if (dump_is_arr[dump_tmp_level]) \
        { \
            strcat(dump_sub_str, dump_idx_str[dump_tmp_level]); \
            p_dump_token = strtok(NULL, "."); \
        } \
        if ((p_dump_token = strtok(NULL, (dump_is_arr[++dump_tmp_level] ? "[" : "."))) != 0) \
            strcat(dump_sub_str, "."); \
    }\

/**************************************************************************//**
 @Description   Declaration of dump mechanism variables.

                This macro must be declared at the beginning of each routine
                which uses the dump mechanism macros, before the routine's code
                starts.
*//***************************************************************************/
#define DECLARE_DUMP \
    char    dump_idx_str[DUMP_MAX_LEVELS + 1][6] = { "", }; \
    char    dump_sub_str[DUMP_MAX_STR] = ""; \
    char    dump_tmp_str[DUMP_MAX_STR] = ""; \
    char    *p_dump_token = NULL; \
    int     dump_arr_idx = 0, dump_arr_size = 0, dump_var_size = 0, dump_level = 0, dump_tmp_level = 0; \
    uint8_t dump_is_arr[DUMP_MAX_LEVELS + 1] = { 0 }; \
    /* Prevent warnings if not all used */ \
    UNUSED(dump_idx_str[0][0]); \
    UNUSED(dump_sub_str[0]); \
    UNUSED(dump_tmp_str[0]); \
    UNUSED(p_dump_token); \
    UNUSED(dump_arr_idx); \
    UNUSED(dump_arr_size); \
    UNUSED(dump_var_size); \
    UNUSED(dump_level); \
    UNUSED(dump_tmp_level); \
    UNUSED(dump_is_arr[0]);

/**************************************************************************//**
 @Description   Prints a title for a subsequent dumped structure or memory.

                The inputs for this macro are the structure/memory title and
                its base addresses.
*//***************************************************************************/
#define DUMP_TITLE(addr, msg)           \
    dump_print("\r\n"); dump_print msg; \
    if (addr)                           \
        dump_print(" (%p)", (addr));    \
    dump_print("\r\n---------------------------------------------------------\r\n");

/**************************************************************************//**
 @Description   Prints a subtitle for a subsequent dumped sub-structure (optional).

                The inputs for this macro are the sub-structure subtitle.
                A separating line with this subtitle will be printed.
*//***************************************************************************/
#define DUMP_SUBTITLE(subtitle)  \
    dump_print("----------- "); dump_print subtitle; dump_print("\r\n")

/**************************************************************************//**
 @Description   Dumps a memory region in 4-bytes aligned format.

                The inputs for this macro are the base addresses and size
                (in bytes) of the memory region.
*//***************************************************************************/
#define DUMP_MEMORY(addr, size)  \
    mem_disp((uint8_t *)(addr), (int)(size))

/**************************************************************************//**
 @Description   Declares a dump loop, for dumping a sub-structure array.

                The inputs for this macro are:
                - idx: an index variable, for indexing the sub-structure items
                       inside the loop. This variable must be declared separately
                       in the beginning of the routine.
                - cnt: the number of times to repeat the loop. This number should
                       equal the number of items in the sub-structures array.

                Note, that the body of the loop must be written inside brackets.
*//***************************************************************************/
#define DUMP_SUBSTRUCT_ARRAY(idx, cnt) \
    for (idx=0, dump_is_arr[dump_level++] = 1; \
         (idx < cnt) && sprintf(dump_idx_str[dump_level-1], "[%d]", idx); \
         idx++, ((idx < cnt) || ((dump_is_arr[--dump_level] = 0) == 0)))

/**************************************************************************//**
 @Description   Dumps a structure's member variable.

                The input for this macro is the full reference for the member
                variable, where the structure is referenced using a pointer.

                Note, that a members array must be dumped using DUMP_ARR macro,
                rather than using this macro.

                If the member variable is part of a sub-structure hierarchy,
                the full hierarchy (including array indexing) must be specified.

                Examples:   p_Struct->member
                            p_Struct->sub.member
                            p_Struct->sub[i].member
*//***************************************************************************/
#define DUMP_VAR(st, phrase) \
    do { \
        void            *addr = (void *)&((st)->phrase); \
        dma_addr_t   phys_addr = fsl_os_virt_to_phys(addr); \
        _CREATE_DUMP_SUBSTR(phrase); \
        dump_var_size = sizeof((st)->phrase); \
        switch (dump_var_size) \
        { \
            case 1:  dump_print("0x%010ll_x: 0x%02x%14s\t%s\r\n", \
                                phys_addr, ioread8(addr), "", dump_sub_str); break; \
            case 2:  dump_print("0x%010ll_x: 0x%04x%12s\t%s\r\n", \
                                phys_addr, ioread16(addr), "", dump_sub_str); break; \
            case 4:  dump_print("0x%010ll_x: 0x%08x%8s\t%s\r\n", \
                                phys_addr, ioread32(addr), "", dump_sub_str); break; \
            case 8:  dump_print("0x%010ll_x: 0x%016llx\t%s\r\n", \
                                phys_addr, ioread64(addr), dump_sub_str); break; \
            default: dump_print("bad size %d (" #st "->" #phrase ")\r\n", dump_var_size); \
        } \
    } while (0)

/**************************************************************************//**
 @Description   Dumps a structure's members array.

                The input for this macro is the full reference for the members
                array, where the structure is referenced using a pointer.

                If the members array is part of a sub-structure hierarchy,
                the full hierarchy (including array indexing) must be specified.

                Examples:   p_Struct->array
                            p_Struct->sub.array
                            p_Struct->sub[i].array
*//***************************************************************************/
#define DUMP_ARR(st, phrase) \
    do { \
        dma_addr_t phys_addr; \
        _CREATE_DUMP_SUBSTR(phrase); \
        dump_arr_size = ARRAY_SIZE((st)->phrase); \
        dump_var_size = sizeof((st)->phrase[0]); \
        switch (dump_var_size) \
        { \
            case 1: \
                for (dump_arr_idx=0; dump_arr_idx < dump_arr_size; dump_arr_idx++) { \
                    phys_addr = fsl_os_virt_to_phys((void *)&((st)->phrase[dump_arr_idx])); \
                    dump_print("0x%010ll_x: 0x%02x%14s\t%s[%d]\r\n", \
                               phys_addr, ioread8(&((st)->phrase[dump_arr_idx])), "", dump_sub_str, dump_arr_idx); \
                } break; \
            case 2: \
                for (dump_arr_idx=0; dump_arr_idx < dump_arr_size; dump_arr_idx++) { \
                    phys_addr = fsl_os_virt_to_phys((void *)&((st)->phrase[dump_arr_idx])); \
                    dump_print("0x%010ll_x: 0x%04x%12s\t%s[%d]\r\n", \
                               phys_addr, ioread16(&((st)->phrase[dump_arr_idx])), "", dump_sub_str, dump_arr_idx); \
                } break; \
            case 4: \
                for (dump_arr_idx=0; dump_arr_idx < dump_arr_size; dump_arr_idx++) { \
                    phys_addr = fsl_os_virt_to_phys((void *)&((st)->phrase[dump_arr_idx])); \
                    dump_print("0x%010ll_x: 0x%08x%8s\t%s[%d]\r\n", \
                               phys_addr, ioread32(&((st)->phrase[dump_arr_idx])), "", dump_sub_str, dump_arr_idx); \
                } break; \
            case 8: \
                for (dump_arr_idx=0; dump_arr_idx < dump_arr_size; dump_arr_idx++) { \
                    phys_addr = fsl_os_virt_to_phys((void *)&((st)->phrase[dump_arr_idx])); \
                    dump_print("0x%010ll_x: 0x%016llx\t%s[%d]\r\n", \
                               phys_addr, ioread64(&((st)->phrase[dump_arr_idx])), dump_sub_str, dump_arr_idx); \
                } break; \
            default: dump_print("bad size %d (" #st "->" #phrase "[0])\r\n", dump_var_size); \
        } \
    } while (0)
#endif /* DEBUG_ERRORS > 0 */

#include "common/errors.h"

#if (!defined(DEBUG_ERRORS) || (DEBUG_ERRORS == 0))
/* No debug/error/event messages at all */
#define DBG(_level, _vmsg)

#else /* DEBUG_ERRORS > 0 */

#if ((defined(DEBUG_USING_STATIC_LEVEL)) && (DEBUG_DYNAMIC_LEVEL < REPORT_LEVEL_WARNING))
/* No need for DBG macro - debug level is higher anyway */
#define DBG(_level, _vmsg)
#else
#define DBG(_level, ...) \
do { \
    if (_level <= DEBUG_DYNAMIC_LEVEL) { \
        fsl_os_print("> %s " PRINT_FORMAT ": ", \
                     dbg_level_strings[_level - 1], \
                     PRINT_FMT_PARAMS); \
        fsl_os_print(__VA_ARGS__); \
        /*fsl_os_print("\r\n");*/ \
    } \
} while (0)
#endif /* (defined(DEBUG_USING_STATIC_LEVEL) && (DEBUG_DYNAMIC_LEVEL < WARNING)) */
#endif /* (!defined(DEBUG_ERRORS) || (DEBUG_ERRORS == 0)) */

#ifdef pr_debug
#undef pr_debug
#endif
#define pr_debug(...) 	DBG(REPORT_LEVEL_TRACE, __VA_ARGS__)


#ifdef pr_info
#undef pr_info
#endif
#define pr_info(...) 	DBG(REPORT_LEVEL_INFO, __VA_ARGS__)

#ifdef pr_warn
#undef pr_warn
#endif
#define pr_warn(...) 	DBG(REPORT_LEVEL_WARNING, __VA_ARGS__)

#ifdef pr_err
#undef pr_err
#endif
#define pr_err(...) 	DBG(REPORT_LEVEL_MAJOR, __VA_ARGS__)


#ifdef pr_crit
#undef pr_crit
#endif
#define pr_crit(...)	DBG(REPORT_LEVEL_CRITICAL, __VA_ARGS__)

/** @} */ /* end of dump_g group */
/** @} */ /* end of gen_g group */


#endif /* __FSL_DBG_H */
