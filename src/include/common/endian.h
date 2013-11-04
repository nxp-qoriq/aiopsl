/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          endian.h

 @Description   Big/little endian swapping routines.
*//***************************************************************************/

#ifndef __FSL_ENDIAN_H
#define __FSL_ENDIAN_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_lib_g   Utility Library Application Programming Interface

 @Description   External routines.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_lib_endian_g Big/Little-Endian Conversion

 @Description   Routines and macros for Big/Little-Endian conversion and
                general byte swapping.

                All routines and macros are expecting unsigned values as
                parameters, but will generate the correct result also for
                signed values. Therefore, signed/unsigned casting is allowed.
 @{
*//***************************************************************************/

/**************************************************************************//**
 @Collection    Byte-Swap Macros

                Macros for swapping byte order.

 @Cautions      The parameters of these macros are evaluated multiple times.
                For calculated expressions or expressions that contain function
                calls it is recommended to use the byte-swap routines.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Swaps the byte order of a given 16-bit value.

 @Param[in]     val - The 16-bit value to swap.

 @Return        The byte-swapped value..

 @Cautions      The given value is evaluated multiple times by this macro.
                For calculated expressions or expressions that contain function
                calls it is recommended to use the SwapUint16() routine.

 @hideinitializer
*//***************************************************************************/
#define SWAP_UINT16(val) \
    ((uint16_t)((((val) & 0x00ff) << 8) | (((val) & 0xff00) >> 8)))

/**************************************************************************//**
 @Description   Swaps the byte order of a given 32-bit value.

 @Param[in]     val - The 32-bit value to swap.

 @Return        The byte-swapped value..

 @Cautions      The given value is evaluated multiple times by this macro.
                For calculated expressions or expressions that contain function
                calls it is recommended to use the SwapUint32() routine.

 @hideinitializer
*//***************************************************************************/
#define SWAP_UINT32(val) \
    ((uint32_t)((((val) & 0x000000ff) << 24) | \
                (((val) & 0x0000ff00) <<  8) | \
                (((val) & 0x00ff0000) >>  8) | \
                (((val) & 0xff000000) >> 24)))

/**************************************************************************//**
 @Description   Swaps the byte order of a given 64-bit value.

 @Param[in]     val - The 64-bit value to swap.

 @Return        The byte-swapped value..

 @Cautions      The given value is evaluated multiple times by this macro.
                For calculated expressions or expressions that contain function
                calls it is recommended to use the SwapUint64() routine.

 @hideinitializer
*//***************************************************************************/
#define SWAP_UINT64(val) \
    ((uint64_t)((((val) & 0x00000000000000ffULL) << 56) | \
                (((val) & 0x000000000000ff00ULL) << 40) | \
                (((val) & 0x0000000000ff0000ULL) << 24) | \
                (((val) & 0x00000000ff000000ULL) <<  8) | \
                (((val) & 0x000000ff00000000ULL) >>  8) | \
                (((val) & 0x0000ff0000000000ULL) >> 24) | \
                (((val) & 0x00ff000000000000ULL) >> 40) | \
                (((val) & 0xff00000000000000ULL) >> 56)))

/* @} */

/**************************************************************************//**
 @Collection    Byte-Swap Routines

                Routines for swapping the byte order of a given parameter and
                returning the swapped value.

                These inline routines are safer than the byte-swap macros,
                because they evaluate the parameter expression only once.
 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      swap_uint16

 @Description   Returns the byte-swapped value of a given 16-bit value.

 @Param[in]     val - The 16-bit value.

 @Return        The byte-swapped value of the parameter.
*//***************************************************************************/
static __inline__ uint16_t swap_uint16(uint16_t val)
{
    return (uint16_t)(((val & 0x00ff) << 8) |
                      ((val & 0xff00) >> 8));
}

/**************************************************************************//**
 @Function      swap_uint32

 @Description   Returns the byte-swapped value of a given 32-bit value.

 @Param[in]     val - The 32-bit value.

 @Return        The byte-swapped value of the parameter.
*//***************************************************************************/
static __inline__ uint32_t swap_uint32(uint32_t val)
{
    return (uint32_t)(((val & 0x000000ff) << 24) |
                      ((val & 0x0000ff00) <<  8) |
                      ((val & 0x00ff0000) >>  8) |
                      ((val & 0xff000000) >> 24));
}

/**************************************************************************//**
 @Function      swap_uint64

 @Description   Returns the byte-swapped value of a given 64-bit value.

 @Param[in]     val - The 64-bit value.

 @Return        The byte-swapped value of the parameter.
*//***************************************************************************/
static __inline__ uint64_t swap_uint64(uint64_t val)
{
    return (uint64_t)(((val & 0x00000000000000ffULL) << 56) |
                      ((val & 0x000000000000ff00ULL) << 40) |
                      ((val & 0x0000000000ff0000ULL) << 24) |
                      ((val & 0x00000000ff000000ULL) <<  8) |
                      ((val & 0x000000ff00000000ULL) >>  8) |
                      ((val & 0x0000ff0000000000ULL) >> 24) |
                      ((val & 0x00ff000000000000ULL) >> 40) |
                      ((val & 0xff00000000000000ULL) >> 56));
}

/* @} */

/**************************************************************************//**
 @Collection    In-place Byte-Swap-And-Set Routines

                Routines for swapping the byte order of a given variable and
                setting the swapped value back to the same variable.
 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      swap_uint16_p

 @Description   Swaps the byte order of a given 16-bit variable.

 @Param[in]     p_Val - Pointer to the 16-bit variable.

 @Return        None.
*//***************************************************************************/
static __inline__ void swap_uint16_p(uint16_t *p_val)
{
    *p_val = swap_uint16(*p_val);
}

/**************************************************************************//**
 @Function      swap_uint32_p

 @Description   Swaps the byte order of a given 32-bit variable.

 @Param[in]     p_Val - Pointer to the 32-bit variable.

 @Return        None.
*//***************************************************************************/
static __inline__ void swap_uint32_p(uint32_t *p_val)
{
    *p_val = swap_uint32(*p_val);
}

/**************************************************************************//**
 @Function      swap_uint64_p

 @Description   Swaps the byte order of a given 64-bit variable.

 @Param[in]     p_Val - Pointer to the 64-bit variable.

 @Return        None.
*//***************************************************************************/
static __inline__ void swap_uint64_p(uint64_t *p_val)
{
    *p_val = swap_uint64(*p_val);
}

/* @} */


/**************************************************************************//**
 @Collection    Little-Endian Conversion Macros

                These macros convert given parameters to or from Little-Endian
                format. Use these macros when you want to read or write a specific
                Little-Endian value in memory, without a-priori knowing the CPU
                byte order.

                These macros use the byte-swap routines. For conversion of
                constants in initialization structures, you may use the CONST
                versions of these macros (see below), which are using the
                byte-swap macros instead.
 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Converts a given 16-bit value from CPU byte order to
                Little-Endian byte order.

 @Param[in]     val - The 16-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CPU_TO_LE16(val)        swap_uint16(val)

/**************************************************************************//**
 @Description   Converts a given 32-bit value from CPU byte order to
                Little-Endian byte order.

 @Param[in]     val - The 32-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CPU_TO_LE32(val)        swap_uint32(val)

/**************************************************************************//**
 @Description   Converts a given 64-bit value from CPU byte order to
                Little-Endian byte order.

 @Param[in]     val - The 64-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CPU_TO_LE64(val)        swap_uint64(val)

/**************************************************************************//**
 @Description   Converts a given 16-bit value from Little-Endian byte order to
                CPU byte order.

 @Param[in]     val - The 16-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define LE16_TO_CPU(val)        CPU_TO_LE16(val)

/**************************************************************************//**
 @Description   Converts a given 32-bit value from Little-Endian byte order to
                CPU byte order.

 @Param[in]     val - The 32-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define LE32_TO_CPU(val)        CPU_TO_LE32(val)

/**************************************************************************//**
 @Description   Converts a given 64-bit value from Little-Endian byte order to
                CPU byte order.

 @Param[in]     val - The 64-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define LE64_TO_CPU(val)        CPU_TO_LE64(val)

/* @} */

/**************************************************************************//**
 @Collection    Little-Endian Constant Conversion Macros

                These macros convert given constants to or from Little-Endian
                format. Use these macros when you want to read or write a specific
                Little-Endian constant in memory, without a-priori knowing the
                CPU byte order.

                These macros use the byte-swap macros, therefore can be used for
                conversion of constants in initialization structures.

 @Cautions      The parameters of these macros are evaluated multiple times.
                For non-constant expressions, use the non-CONST macro versions.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Converts a given 16-bit constant from CPU byte order to
                Little-Endian byte order.

 @Param[in]     val - The 16-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CONST_CPU_TO_LE16(val)  SWAP_UINT16(val)

/**************************************************************************//**
 @Description   Converts a given 32-bit constant from CPU byte order to
                Little-Endian byte order.

 @Param[in]     val - The 32-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CONST_CPU_TO_LE32(val)  SWAP_UINT32(val)

/**************************************************************************//**
 @Description   Converts a given 64-bit constant from CPU byte order to
                Little-Endian byte order.

 @Param[in]     val - The 64-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CONST_CPU_TO_LE64(val)  SWAP_UINT64(val)

/**************************************************************************//**
 @Description   Converts a given 16-bit constant from Little-Endian byte order
                to CPU byte order.

 @Param[in]     val - The 16-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CONST_LE16_TO_CPU(val)  CONST_CPU_TO_LE16(val)

/**************************************************************************//**
 @Description   Converts a given 32-bit constant from Little-Endian byte order
                to CPU byte order.

 @Param[in]     val - The 32-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CONST_LE32_TO_CPU(val)  CONST_CPU_TO_LE32(val)

/**************************************************************************//**
 @Description   Converts a given 64-bit constant from Little-Endian byte order
                to CPU byte order.

 @Param[in]     val - The 64-bit value to convert.

 @Return        The converted value.

 @hideinitializer
*//***************************************************************************/
#define CONST_LE64_TO_CPU(val)  CONST_CPU_TO_LE64(val)
/* @} */

/** @} */ /* end of fsl_lib_endian_g group */
/** @} */ /* end of fsl_lib_g group */


#endif /* __FSL_ENDIAN_H */
